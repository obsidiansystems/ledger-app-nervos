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
    this.speculos.
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
  automationStart(speculos, acceptPrompts(expectedPrompts, acceptPrompt));
}


/* State machine to read screen events and turn them into screens of prompts. */
async function automationStart(speculos, interactionFunc) {
  let readyPromiseResolve;
  const readyPromise = new Promise(r => { readyPromiseResolve = r; });

  // If this doens't exist, we're running against a hardware ledger; just call
  // interactionFunc with no events iterator.
  if(!speculos.automationEvents) {
    readyPromiseResolve({ promptsPromise: interactionFunc(speculos) });
    return readyPromise;
  }

  // This is so that you can just "await flowAccept(this.speculos);" in a test
  // without actually waiting for the prompts.  If we don't do this, you can
  // end up with two flowAccept calls active at once, causing issues.

  await speculos.promptsEndPromise; // Wait for any previous interaction to end.
  speculos.promptsEndPromise = promptsPromise; // Set ourselves as the interaction.
        
  // Make an async iterator we can push stuff into.
  let sendEvent;
  let sendPromise=new Promise(r=>{sendEvent = r;});
  let asyncEventIter = {
    next: async ()=>{
      promptVal=await sendPromise;
      sendPromise=new Promise(r=>{sendEvent = r;});
      return promptVal;
    }
  };
  
  // Sync up with the ledger; wait until we're on the home screen, and do some
  // clicking back and forth to make sure we see the event.
  // Then pass screens to interactionFunc.
  readyPromise = syncWithLedger(speculos, asyncEventIter, interactionFunc);

  let promptHeader;
  let promptBody;

  let subscript = speculos.automationEvents.subscribe({
    next: evt => {
        // Wrap up two-line prompts into one:
        if(evt.y == 3) {
          promptHeader = evt.text;
          return; // The top line comes out first, so now wait for the next draw.
        } else {
          promptBody = evt.text;
        }

        sendEvent({ promptHeader, promptBody });
        promptBody=undefined;
        promptHeader=undefined;
    }});
  
  asyncEventIter.unsubscribe = subscript.unsubscribe;

  // Send a rightward-click to make sure we get _an_ event and our state
  // machine starts.
  speculos.button("Rr");

  return readyPromise;
}

async function syncWithLedger(speculos, source, interactionFunc) {
  let screen = await source.next();
  // Scroll to the end; we do this because we might have seen "Avalanche" when
  // we subscribed, but needed to send a button click to make sure we reached
  // this point.
  while(screen.promptBody != "Quit") {
    speculos.button("Rr");
    screen = await source.next();
    body = body + screen.promptBody;
  }
  // Scroll back to "Avalanche", and we're ready and pretty sure we're on the
  // home screen.
  while(screen.promptHeader != "Avalanche") {
    speculos.button("Ll");
    screen = await source.next();
    body = body + screen.promptBody;
  }
  // And continue on to interactionFunc
  return { promptsPromise: interactionFunc(speculos, asyncEventIter).finally(source.unsubscribe()) };
}

async function readMultiScreenPrompt(speculos, source) {
  let header;
  let body;
  let screen = await source.next();
  let m = screen.promptHeader.match(/^(.*) \(([0-9])\/([0-9])\)$/);
  if (m) {
    header = m[1];
    body = screen.promptBody;
    while(m[2] !== m[3]) {
      speculos.button("Rr");
      let screen = await source.next();
      let m = screen.promptHeader.match(/^(.*) \(([0-9])\/([0-9])\)$/);
      body = body + screen.promptBody;
    }
    return { promptHeader: header, promptBody: body };
  } else {
    return screen;
  }
}

async function acceptPrompts(expectedPrompts, selectPrompt) {
  return (speculos, screens) => {
    if(!screens) {
      // We're running against hardware, so we can't prompt but
      // should tell the person running the test what to do.
      if (expectedPrompts) {
        console.log("Expected prompts: ");
        for (p in expectedPrompts) {
          console.log("Prompt %d", p);
          console.log(expectedPrompts[p][3]);
          console.log(expectedPrompts[p][17]);
        }
      }
      console.log("Please %s this prompt", acceptPrompt);
    } else {
      let promptList = [];
      while((screen = readMultiScreenPrompt(speculos, screens)) != selectPrompt) {
        if(screen.promptHeader != selectPrompt && screen.promptHeader != "Reject") {
          promptList.push(screen);
        }
        if(screen.promptHeader != selectPrompt) speculos.button("Rr");
        if(screen.promptHeader === selectPrompt) {
          speculos.button("RLrl");
        }
      }
      if (expectedPrompts) {
        expect(promptList).to.deep.equal(expectedPrompts);
        return { promptsList, promptsMatch: true };
      } else {
        return { promptsList };
      }
    }
  }
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
