const SpeculosTransport = require('@ledgerhq/hw-transport-node-speculos').default;
const Avalanche = require('@ledgerhq/hw-app-avalanche').default;
const spawn = require('child_process').spawn;

const APDU_PORT = 9999;
const BUTTON_PORT = 8888;
const AUTOMATION_PORT = 8899;

exports.mochaHooks = {
  beforeAll: async function () {
    this.timeout(10000); // We'll let this wait for up to 10 seconds to get a speculos instance.
    this.speculosProcess = spawn('speculos', [
        '../bin/app.elf',
        '--display', 'headless',
        '--button-port', '' + BUTTON_PORT,
        '--automation-port', '' + AUTOMATION_PORT,
        '--apdu-port', '' + APDU_PORT,
      ]);
    console.log("Speculos started");
    while (this.speculos === undefined) { // Let the test timeout handle the bad case
      try {
        this.speculos = await SpeculosTransport.open({
          apduPort: APDU_PORT,
          buttonPort: BUTTON_PORT,
          automationPort: AUTOMATION_PORT,
        });
      } catch(e) {
        await new Promise(r => setTimeout(r, 500));
      }
    }
    this.ava = new Avalanche(this.speculos, "Avalanche", (_) => { return; });
  },
  afterAll: async function () {
    this.speculosProcess.kill();
  }
}

function flowAccept(speculos, n, accept = "Accept") {
  return new Promise(r => {
    var prompts = [{}];
    var subscript = speculos.automationEvents.subscribe({
      next: evt => {
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
        if (isFirst) {
          prompts[prompts.length-1][evt.y] = evt.text;
        } else if (evt.y !== 3) {
          prompts[prompts.length-1][evt.y] = prompts[prompts.length-1][evt.y] + evt.text;
        }
        if (evt.y !== 3 && isLast) prompts.push({});
        if (evt.text !== accept) {
          if (evt.y !== 3) speculos.button("Rr");
        } else {
          speculos.button("RLrl");
          subscript.unsubscribe();
          r(prompts.slice(-(n+3), -3));
        }
      }
    });
  });
}

global.flowAccept = flowAccept;
