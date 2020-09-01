const SpeculosTransport = require('@ledgerhq/hw-transport-node-speculos').default;
const Avalanche = require('hw-app-avalanche').default;
const spawn = require('child_process').spawn;
const fc = require('fast-check');

const APDU_PORT = 9999;
const BUTTON_PORT = 8888;
const AUTOMATION_PORT = 8899;

exports.mochaHooks = {
  beforeAll: async function () {
    this.timeout(10000); // We'll let this wait for up to 10 seconds to get a speculos instance.
    speculosProcessOptions=process.env.SPECULOS_DEBUG?{stdio:"inherit"} : {};
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
        if(process.env.DEBUG_BUTTONS) {
          subButton=this.speculos.button;
          this.speculos.button=((btns)=>{console.log("Speculos Buttons: " + btns); return subButton(btns);});
        }
      } catch(e) {
        await new Promise(r => setTimeout(r, 500));
      }
    }
    this.ava = new Avalanche(this.speculos, "Avalanche", (_) => { return; });
  },
  afterAll: async function () {
    this.speculosProcess.kill();
  },
  afterEach: async function () {
    stdoutVal=this.speculosProcess.stdio[1].read();
    stderrVal=this.speculosProcess.stdio[2].read();
    if(this.currentTest.state === 'failed') {
      console.log("SPECULOS STDOUT:\n"+stdoutVal);
      console.log("SPECULOS STDERR:\n"+stderrVal);
    }
  }
}

async function flowAccept(speculos, acceptPrompt="Accept") {
  let promptsPromiseResolve;
  let promptsPromise=new Promise(r => { promptsPromiseResolve = r; });
  let readyPromiseResolve;
  let readyPromise=new Promise(r => { readyPromiseResolve = r; });
  let isReady = false;
  let isPrimed = false;
  let prompts = [{}];
  let isFirst = false;
  let isLast = false;

  // This is so that you can just "await flowAccept(this.speculos);" in a test
  // without actually waiting for the prompts.  If we don't do this, you can
  // end up with two flowAccept calls active at once, causing issues.
  
  await speculos.promptsEndPromise; // Wait for any previous interaction to end.
  speculos.promptsEndPromise=promptsPromise; // Set ourselves as the interaction.

  let subscript = speculos.automationEvents.subscribe({
    next: evt => {
      if (!isReady) {
        if(!isPrimed) {
          if(evt.y == 19 && evt.text === "Quit") {
            isPrimed = true;
          }
          speculos.button("Ll");
          return;
        } else {
          if(evt.y === 17 && evt.text === "0.1.0") {
            isReady=true;
            readyPromiseResolve({prompts: promptsPromise});
            return;
          } else {
            speculos.button("Ll");
          }
        }
      } else {
        if (evt.text === "Quit") {
          // speculos.button("Rr");
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
        if(! { "Reject":true, "Accept":true }[evt.text]) {
          if (isFirst) {
            prompts[prompts.length-1][evt.y] = evt.text;
          } else if (evt.y !== 3) {
            prompts[prompts.length-1][evt.y] = prompts[prompts.length-1][evt.y] + evt.text;
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
          promptsPromiseResolve(prompts.filter(a=>(Object.keys(a).length!=0))); //.slice(-(n+3), -3));
        }
      }
    }
  });
  speculos.button("Rr");
  return readyPromise;
}



fcConfig={
	interruptAfterTimeLimit: parseInt(process.env.GEN_TIME_LIMIT || 1000),
	markInterruptAsFailure: false,
  numRuns: parseInt(process.env.GEN_NUM_RUNS || 100)
};


console.log(fcConfig);

fc.configureGlobal(fcConfig);

global.flowAccept = flowAccept;

