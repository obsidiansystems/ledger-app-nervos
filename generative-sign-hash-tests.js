var chai = require('chai');
var { expect, assert } = chai.use(require('chai-bytes'));
chai.config.showDiff=true;
chai.config.truncateThreshold=0;
// var jsc = require('jsverify');
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

describe("Sign Hash tests", ()=> {
  context('Generative tests', function () {
    it('can sign a hash-sized sequence of bytes', async function () { // Needed to get 'this' for mocha.
      return await fc.assert(fc.asyncProperty(accountGen, fc.array(subAddressGen), fc.hexaString(64, 64), async (account, subAccts, hashHex) => {
        this.speculosProcess.stdio[2].read(); // Drain the stderr buffer.
        ui = await flowAccept(this.speculos);
        let hash=Buffer.from(hashHex, 'hex');
        sigs=this.ava.signHash(account, subAccts, hash);

        expectedPrompts = [
          { '3': 'Sign', '17': 'Hash' },
          { '3': 'Derivation Prefix', '17': account.toString(true) },
          { '3': 'Hash', '17': hashHex.toUpperCase() }
        ];
        expect(await ui.prompts).to.deep.equal(expectedPrompts);
        
        sv=await sigs;
        for (ks of sv) {
          [keySuffix, sig] = ks;

          await flowAccept(this.speculos);
          key=await this.ava.getWalletExtendedPublicKey(account.toString() + "/" + keySuffix);
          
          recovered=secp256k1.recover(Buffer.from(hash, 'hex'), sig.slice(0,64), sig[64], false);
          expect(recovered).is.equalBytes(key.public_key);
        }
      }));
    });
    it('User rejection does not produce signatures', async function () { // Needed to get 'this' for mocha.
      return await fc.assert(fc.asyncProperty(accountGen, fc.array(subAddressGen), fc.hexaString(64, 64), async (account, subAccts, hashHex) => {
        this.speculosProcess.stdio[2].read(); // Drain the stderr buffer.
        if(subAccts.length==0) return;
        ui = await flowAccept(this.speculos, "Reject");
        let hash=Buffer.from(hashHex, 'hex');
        let sigs;
        try {
          sigs=await this.ava.signHash(account, subAccts, hash);
          throw("Rejected prompts should reject");
        } catch(e) {
          expect(e).has.property('statusCode', 0x6985);
          expect(e).has.property('statusText', 'CONDITIONS_OF_USE_NOT_SATISFIED');
        }

        expectedPrompts = [
          { '3': 'Sign', '17': 'Hash' },
          { '3': 'Derivation Prefix', '17': account.toString(true) },
          { '3': 'Hash', '17': hashHex.toUpperCase() }
        ];
        expect(await ui.prompts).to.deep.equal(expectedPrompts);
      }));
    });
    it('rejects incorrectly-sized hashes', async function () {
      return await fc.assert(fc.asyncProperty(accountGen, fc.array(subAddressGen), fc.hexaString(), async (account, subAccts, hashHex) => {
        this.speculosProcess.stdio[2].read(); // Drain the stderr buffer.
        let hash=Buffer.from(hashHex, 'hex');
        const firstMessage = Buffer.concat([
          this.ava.uInt8Buffer(subAccts.length),
          hash,
          this.ava.encodeBip32Path(account)
        ]);
        try {
          await this.speculos.send(this.ava.CLA, this.ava.INS_SIGN_HASH, 0x00, 0x00, firstMessage);
        } catch (e) {
          expect(e).has.property('statusCode', 0x6C00); // WRONG_LENGTH
        }
        return;
      }));
    });
  });
});
