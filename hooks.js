var { expect, assert } = require('chai').use(require('chai-bytes'));
// var jsc = require('jsverify');
var SpeculosTransport = require('@ledgerhq/hw-transport-node-speculos').default;
var Avalanche = require('hw-app-avalanche').default;
var rxjs = require('rxjs/operators');
var secp256k1 = require('bcrypto/lib/secp256k1');
var spawn = require('child_process').spawn;

exports.mochaHooks = {
  beforeAll: async function () {
	  this.timeout(10000); // We'll let this wait for up to 10 seconds to get a speculos instance.
    speculosOptions=process.env.SPECULOS_DEBUG?{stdio: "inherit"} : {};
    this.speculosProcess=spawn('speculos', [process.env.LEDGER_APP, '--display', 'headless', '--button-port', '8888', '--automation-port', '8899', '--apdu-port', '9999'], speculosOptions);
    console.log("Speculos started");
    while(this.speculos === undefined) { // Let the test timeout handle the bad case
      try {
	this.speculos=await SpeculosTransport.open( { apduPort: 9999, buttonPort: 8888, automationPort: 8899 } );
      } catch(e) {
	await new Promise(r=>setTimeout(r,500));
      }
    }
    // this.speculos.automationEvents.subscribe({next: a=>console.log("AUTOMATION: " + JSON.stringify(a))});
    this.ava=new Avalanche(this.speculos);
  },
  afterAll: async function () {
    this.speculosProcess.kill();
  }
}

function flowAccept(speculos, n) {
  return new Promise(r=> {
    var prompts=[{}];
    var canContinue=true;
    var subscript=speculos.automationEvents.subscribe({
      next: evt => {
	if(evt.y == 3) {
	  let m=evt.text.match(/^(.*) \(([0-9])\/([0-9])\)$/)
	  if(m) {
	    isFirst=(m[2]==='1');
	    isLast=(m[2]===m[3]);
	    evt.text=m[1];
	  } else {
	    isFirst=true;
	    isLast=true;
	  }
	}
	if(isFirst) {
	  prompts[prompts.length-1][evt.y]=evt.text
	} else {
	  if(evt.y != 3) prompts[prompts.length-1][evt.y]=prompts[prompts.length-1][evt.y] + evt.text
	}
	if(evt.y != 3 && isLast) prompts.push({});
	if(evt.text != "Accept") {
	  if(evt.y != 3) speculos.button("Rr");
	}
	else {
	  speculos.button("RLrl");
	  subscript.unsubscribe();
	  r(prompts.slice(-(n+3), -3));
	}
      }
    });
  });
}

global.flowAccept = flowAccept;
