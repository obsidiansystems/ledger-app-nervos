const SpeculosTransport = require('@ledgerhq/hw-transport-node-speculos').default;
const HidTransport = require('@ledgerhq/hw-transport-node-hid').default;
const Avalanche = require('hw-app-avalanche').default;
const spawn = require('child_process').spawn;
const fc = require('fast-check');
const chai = require('chai');
const { expect } = chai.use(require('chai-bytes'));

const APDU_PORT = 9999;
const BUTTON_PORT = 8888;
const AUTOMATION_PORT = 8899;

exports.mochaHooks = {
  beforeAll: async function () { // Need 'function' to get 'this'
    this.timeout(10000); // We'll let this wait for up to 10 seconds to get a speculos instance.
    if (process.env.LEDGER_LIVE_HARDWARE) {
      this.speculos = await HidTransport.create();
      this.speculos.button = console.log;
      console.log(this.speculos);
    } else {
      const speculosProcessOptions = process.env.SPECULOS_DEBUG ? {stdio:"inherit"} : {};
      this.speculosProcess = spawn('speculos', [
        process.env.LEDGER_APP,
        '--display', 'headless',
        '--button-port', '' + BUTTON_PORT,
        '--automation-port', '' + AUTOMATION_PORT,
        '--apdu-port', '' + APDU_PORT,
      ], speculosProcessOptions);
      console.log("Speculos started");
      while (this.speculos === undefined) { // Let the test timeout handle the bad case
        try {
          this.speculos = await SpeculosTransport.open({
            apduPort: APDU_PORT,
            buttonPort: BUTTON_PORT,
            automationPort: AUTOMATION_PORT,
          });
          if (process.env.DEBUG_BUTTONS) {
            const subButton = this.speculos.button;
            this.speculos.button = btns => {
              console.log("Speculos Buttons: " + btns);
              return subButton(btns);
            };
          }
        } catch(e) {
          await new Promise(r => setTimeout(r, 500));
        }
      }
    }
    this.ava = new Avalanche(this.speculos, "Avalanche", _ => { return; });
    this.flushStderr = function() {
      if (this.speculosProcess) this.speculosProcess.stdio[2].read();
    };
  },
  afterAll: async function () {
    if (this.speculosProcess) {
      this.speculosProcess.kill();
    }
  },
  afterEach: async function () {
    if (this.speculosProcess) {
      stdoutVal = this.speculosProcess.stdio[1].read();
      stderrVal = this.speculosProcess.stdio[2].read();
      if (this.currentTest.state === 'failed') {
        console.log("SPECULOS STDOUT:\n" + stdoutVal);
        console.log("SPECULOS STDERR:\n" + stderrVal);
      }
    }
  }
}

async function flowAccept(speculos, expectedPrompts, acceptPrompt="Accept") {
  let promptsPromiseResolve;
  const promptsPromise = new Promise(r => { promptsPromiseResolve = r; });
  let promptsMatchPromiseResolve;
  const promptsMatchPromise = new Promise(r => { promptsMatchPromiseResolve = r; });
  let readyPromiseResolve;
  const readyPromise = new Promise(r => { readyPromiseResolve = r; });

  if (!speculos.automationEvents) {
    if (expectedPrompts) {
      console.log("Expected prompts: ");
      for (p in expectedPrompts) {
        console.log("Prompt %d", p);
        console.log(expectedPrompts[p][3]);
        console.log(expectedPrompts[p][17]);
      }
    }
    console.log("Please %s this prompt", acceptPrompt);
    promptsPromiseResolve();
    readyPromiseResolve({ prompts: promptsPromise, promptsMatch: Promise.resolve(true) });
    return readyPromise;
  }

  let isReady = false;
  let isPrimed = false;
  let prompts = [{}];
  let isFirst = false;
  let isLast = false;

  // This is so that you can just "await flowAccept(this.speculos);" in a test
  // without actually waiting for the prompts.  If we don't do this, you can
  // end up with two flowAccept calls active at once, causing issues.

  await speculos.promptsEndPromise; // Wait for any previous interaction to end.
  speculos.promptsEndPromise = promptsPromise; // Set ourselves as the interaction.

  let subscript = speculos.automationEvents.subscribe({
    next: evt => {
      if (!isReady) {
        if (!isPrimed) {
          if (evt.y == 19 && evt.text === "Quit") {
            isPrimed = true;
          }
          speculos.button("Ll");
          return;
        } else {
          if (evt.y === 17 && evt.text === "0.1.0") {
            isReady = true;
            readyPromiseResolve({prompts: promptsPromise, promptsMatch: promptsMatchPromise});
            return;
          } else {
            speculos.button("Ll");
          }
        }
      } else {
        if (evt.text === "Quit") {
          return;
        }
        if (evt.y === 3) {
          let m = evt.text.match(/^(.*) \(([0-9])\/([0-9])\)$/)
          if (m) {
            isFirst = m[2] === '1';
            isLast = m[2] === m[3];
            evt.text = m[1];
          } else {
            isFirst = true;
            isLast = true;
          }
        }
        if (evt.text !== "Reject" && evt.text !== acceptPrompt) {
          if (isFirst) {
            prompts[prompts.length - 1][evt.y] = evt.text;
          } else if (evt.y !== 3) {
            prompts[prompts.length - 1][evt.y] = prompts[prompts.length - 1][evt.y] + evt.text;
          }
        }
        if (evt.y !== 3 && isLast) prompts.push({});
        if (evt.text !== acceptPrompt) {
          if (evt.y !== 3) {
            speculos.button("Rr");
          }
        } else {
          speculos.button("RLrl");
          subscript.unsubscribe();
          const resultingPrompts = prompts.filter(a => Object.keys(a).length != 0);
          if (expectedPrompts) {
            expect(resultingPrompts).to.deep.equal(expectedPrompts);
            promptsMatchPromiseResolve(true);
          }
          promptsPromiseResolve(resultingPrompts);
        }
      }
    }
  });
  speculos.button("Rr");
  return readyPromise;
}


const fcConfig = {
  interruptAfterTimeLimit: parseInt(process.env.GEN_TIME_LIMIT || 1000),
  markInterruptAsFailure: false,
  numRuns: parseInt(process.env.GEN_NUM_RUNS || 100)
};

fc.configureGlobal(fcConfig);

global.flowAccept = flowAccept;
global.signHashPrompts = (hash, pathPrefix) => {
  return [
    {"3":"Sign","17":"Hash"},
    {"3":"DANGER!","17":"YOU MUST verify this manually!!!"},
    {"3":"Derivation Prefix","17":pathPrefix},
    {"3":"Hash","17":hash},
    {"3":"Are you sure?","17":"This is very dangerous!"},
  ];
};
