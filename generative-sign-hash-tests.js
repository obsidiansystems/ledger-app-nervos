const chai = require('chai');
const { expect } = chai.use(require('chai-bytes'));
chai.config.showDiff = true;
chai.config.truncateThreshold = 0;
const secp256k1 = require('bcrypto/lib/secp256k1');
const fc = require('fast-check');
const bip = require('bip32-path');

const prefix = bip.fromString("m/44'/9000'").toPathArray();
const bipNotHardened = fc.integer(0x7fffffff);
const bipHardened = bipNotHardened.map(a => a + 0x80000000);
const accountGen = bipHardened.map(a => bip.fromPathArray(prefix.concat([a])));
const subAddressGen = fc.array(fc.integer(0,4294967295),2,2).map(bip.fromPathArray);

describe("Sign Hash tests", () => {
  context('Generative tests', function () {
    it('can sign a hash-sized sequence of bytes', async function () { // Need 'function' to get 'this' for mocha.
      return await fc.assert(fc.asyncProperty(accountGen, fc.array(subAddressGen), fc.hexaString(64, 64), async (account, subAccts, hashHex) => {
        this.flushStderr();

        expectedPrompts = [
          { '3': 'Sign', '17': 'Hash' },
          { '3': 'Derivation Prefix', '17': account.toString(true) },
          { '3': 'Hash', '17': hashHex.toUpperCase() }
        ];

        const ui = await flowAccept(this.speculos, expectedPrompts);
        const hash = Buffer.from(hashHex, "hex");
        const sigs = this.ava.signHash(account, subAccts, hash);

        await ui.promptsMatch;

        const sv = await sigs;
        for (ks of sv) {
          const [keySuffix, sig] = ks;

          await flowAccept(this.speculos);
          const key = await this.ava.getWalletExtendedPublicKey(account.toString() + "/" + keySuffix);

          const recovered = secp256k1.recover(Buffer.from(hash, "hex"), sig.slice(0, 64), sig[64], false);
          expect(recovered).is.equalBytes(key.public_key);
        }
      }));
    });

    it('does not produce signatures when prompt is rejected', async function () { // Need 'function' to get 'this' for mocha.
      return await fc.assert(fc.asyncProperty(accountGen, fc.array(subAddressGen), fc.hexaString(64, 64), async (account, subAccts, hashHex) => {
        this.flushStderr();
        if (subAccts.length == 0) return;

        const expectedPrompts = [
          { '3': 'Sign', '17': 'Hash' },
          { '3': 'Derivation Prefix', '17': account.toString(true) },
          { '3': 'Hash', '17': hashHex.toUpperCase() }
        ];

        const ui = await flowAccept(this.speculos, expectedPrompts, "Reject");
        const hash = Buffer.from(hashHex, "hex");
        try {
          await this.ava.signHash(account, subAccts, hash);
          throw "Rejected prompts should reject";
        } catch(e) {
          expect(e).has.property('statusCode', 0x6985);
          expect(e).has.property('statusText', 'CONDITIONS_OF_USE_NOT_SATISFIED');
        }

        await ui.promptsMatch;
      }));
    });

    it('rejects incorrectly-sized hashes', async function () { // Need 'function' to get 'this' for mocha.
      return await fc.assert(fc.asyncProperty(accountGen, fc.array(subAddressGen), fc.hexaString(), async (account, subAccts, hashHex) => {
        this.flushStderr();
        const hash = Buffer.from(hashHex, "hex");
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
      }));
    });
  });
});
