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
  return await automationStart(speculos, acceptPrompts(expectedPrompts, acceptPrompt));
}


/* State machine to read screen events and turn them into screens of prompts. */
async function automationStart(speculos, interactionFunc) {
  // If this doens't exist, we're running against a hardware ledger; just call
  // interactionFunc with no events iterator.
  if(!speculos.automationEvents) {
    return new Promise(r=>{ promptsPromise: interactionFunc(speculos) });
  }

  // This is so that you can just "await flowAccept(this.speculos);" in a test
  // without actually waiting for the prompts.  If we don't do this, you can
  // end up with two flowAccept calls active at once, causing issues.
  let promptLockResolve;
  let promptsLock=new Promise(r=>{promptLockResolve=r});
  if(speculos.promptsEndPromise) {
    await speculos.promptsEndPromise; // Wait for any previous interaction to end.
    speculos.promptsEndPromise = promptsLock; // Set ourselves as the interaction.
  }
        
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
  let readyPromise = syncWithLedger(speculos, asyncEventIter, interactionFunc);
  
  // Resolve our lock when we're done
  readyPromise.then(r=>r.promptsPromise.then(promptLockResolve(true)));

  let header;
  let body;

  let subscript = speculos.automationEvents.subscribe({
    next: evt => {
        // Wrap up two-line prompts into one:
        if(evt.y == 3) {
          header = evt.text;
          return; // The top line comes out first, so now wait for the next draw.
        } else {
          body = evt.text;
        }
        sendEvent({ ...(header && {header}), body });
        body=undefined;
        header=undefined;
    }});
  
  asyncEventIter.unsubscribe = () => { subscript.unsubscribe(); };

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
  while(screen.body != "Quit") {
    speculos.button("Rr");
    screen = await source.next();
  }
  // Scroll back to "Avalanche", and we're ready and pretty sure we're on the
  // home screen.
  while(screen.header != "Avalanche") {
    speculos.button("Ll");
    screen = await source.next();
  }
  // And continue on to interactionFunc
  let interactFP = interactionFunc(speculos, source);
  console.log("Calling interactionFunc, letting test proceed.");
  return { promptsPromise: interactFP };
}

async function readMultiScreenPrompt(speculos, source) {
  let header;
  let body;
  let screen = await source.next();
  let m = screen.header && screen.header.match(/^(.*) \(([0-9])\/([0-9])\)$/);
  if (m) {
    header = m[1];
    body = screen.body;
    while(m[2] !== m[3]) {
      speculos.button("Rr");
      screen = await source.next();
      m = screen.header && screen.header.match(/^(.*) \(([0-9])\/([0-9])\)$/);
      body = body + screen.body;
    }
    return { header: header, body: body };
  } else {
    return screen;
  }
}

function acceptPrompts(expectedPrompts, selectPrompt) {
  return async (speculos, screens) => {
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
      let done = false;
      while(!done && (screen = await readMultiScreenPrompt(speculos, screens))) {
        if(screen.body != selectPrompt && screen.body != "Reject") {
          promptList.push(screen);
        }
        if(screen.body !== selectPrompt) {
          speculos.button("Rr");
        } else {
          speculos.button("RLrl");
          done = true;
        }
      }

      screens.unsubscribe();
      if (expectedPrompts) {
        expect(promptList).to.deep.equal(expectedPrompts);
        return { promptList, promptsMatch: true };
      } else {
        return { promptList };
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
    {header:"Sign",body:"Hash"},
    {header:"DANGER!",body:"YOU MUST verify this manually!!!"},
    {header:"Derivation Prefix",body:pathPrefix},
    {header:"Hash",body:hash},
    {header:"Are you sure?",body:"This is very dangerous!"},
  ];
};
