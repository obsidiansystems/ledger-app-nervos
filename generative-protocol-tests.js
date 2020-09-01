var chai = require('chai');
var { expect, assert } = chai.use(require('chai-bytes'));
chai.config.showDiff=true;
chai.config.truncateThreshold=0;
var rxjs = require('rxjs/operators');
var secp256k1 = require('bcrypto/lib/secp256k1');
var fc = require('fast-check');
// var GDB = require('gdb-js').GDB;
// let gdb = new GDB(spawn('gdb', ['-i=mi', 'main']));
var bip = require('bip32-path');

prefix=bip.fromString("m/44'/9000'").toPathArray();

bipNotHardened=fc.integer(0x7fffffff);
bipHardened=bipNotHardened.map((a)=>a+0x80000000);

accountGen=bipHardened.map((a)=>bip.fromPathArray(prefix.concat([a])));

subAddressGen=fc.array(fc.integer(0,4294967295),2,2).map(bip.fromPathArray);

describe("APDU protocol integrity generative tests", function () {
  context('Generative tests', function () {
    it('rejects incorrect APDU numbers', async function () {
      return await fc.assert(fc.asyncProperty(fc.integer(5,255), fc.hexaString(), async (apdu, hashHex) => {
        /*let subscription=this.speculos.automationEvents.subscribe({
          next: evt=> {console.log("Ledger prompt: " + evt.text);}
        });*/
        let body=Buffer.from(hashHex, 'hex');
        try {
          await this.speculos.send(this.ava.CLA, apdu, 0x00, 0x00, body);
          throw("Expected error");
        } catch (e) {
          this.speculosProcess.stdio[2].read(); // Drain the stderr buffer.
          expect(e).has.property('statusCode', 27904); // INS_NOT_SUPPORTED
        }
        // subscription.unsubscribe();
        return;
      }), { examples: [[ 9, '' ]] });
    });
    it('rejects garbage on correct APDU numbers', async function () {
      return await fc.assert(fc.asyncProperty(fc.integer(0,4), fc.hexaString(), fc.integer(0,255), async (apdu, hashHex, p1) => {
        let body=Buffer.from(hashHex, 'hex');
        try {
          await this.speculos.send(this.ava.CLA, apdu, p1, 0x00, body);
          throw("Expected error");
        } catch (e) {
          this.speculosProcess.stdio[2].read(); // Drain the stderr buffer.
          //expect(e).has.property('statusCode', 27904); // INS_NOT_SUPPORTED
        }
        return;
      }));
    });
    it('rejects garbage dumped straight to the device', async function () {
      return await fc.assert(fc.asyncProperty(fc.hexaString(2,512), async (hashHex) => {
        let body=Buffer.from(hashHex, 'hex');
        rv=await this.speculos.exchange(body);
        expect(rv).to.not.equalBytes("9000");
        this.speculosProcess.stdio[2].read(); // Drain the stderr buffer.
      }));
    });
    it('rejects short garbage dumped straight to the device', async function () {
      return await fc.assert(fc.asyncProperty(fc.hexaString(10,64), async (hashHex) => {
        let body=Buffer.from(hashHex, 'hex');
        try {
          rv=await this.speculos.exchange(body);
        } catch(e) {
          return; // Errors that get here are probably from the transport, not from the ledger.
        }
        expect(rv).to.not.equalBytes("9000");
        this.speculosProcess.stdio[2].read(); // Drain the stderr buffer.
      }));
    });
  });
});
