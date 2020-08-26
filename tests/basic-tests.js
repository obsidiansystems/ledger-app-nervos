const { expect } = require('chai').use(require('chai-bytes'));
const { recover } = require('bcrypto/lib/secp256k1');
const BIPPath = require("bip32-path");

describe("Basic Tests", () => {
  context('Basic APDUs', function () {
    it('Can fetch the version of the app', async function () {
      const cfg = await this.ava.getAppConfiguration();
      expect(cfg).to.be.a('object');
      expect(cfg).to.have.property("version", "0.1.0");
      expect(cfg).to.have.property("name", "Avalanche");
    });
    it('Returns the expected wallet ID', async function () {
      const id = await this.ava.getWalletId();
      expect(id).to.equalBytes('f0e476edaffc');
    });
  });

  context('Public Keys', function () {
    it('Can retrieve an address from the app', async function() {
      flowAccept(this.speculos);
      const key = await this.ava.getWalletAddress("44'/9000'/0'/0/0");
      expect(key).to.equalBytes('41c9cc6fd27e26e70f951869fb09da685a696f0a');
    });
    it('Can retrieve a different address from the app', async function() {
      flowAccept(this.speculos);
      const key = await this.ava.getWalletAddress("44'/9000'/1'/0/0");
      expect(key).to.equalBytes('f14c91be3a26e3ce30f970d87257fd2fb3dfbb7f');
    });
    it('Produces the expected top-level extended key', async function() {
      flowAccept(this.speculos);
      const key = await this.ava.getWalletExtendedPublicKey("44'/9000'");
      expect(key).to.have.property('public_key').equalBytes('044b68da714d7f8b9d97a9071f2977b587183972f0aa18a6af0b5917d3b2820686c521a7d4ac90a6565df51cb9e7a5309cd2d46907450bd8d8dd89ba16751ed8ee');
      expect(key).to.have.property('chain_code').to.equalBytes('3b0c30e8b72f70ebe99698aca6ef8f380290c235337916b27730b301e978e664');
    });
    it('Can retrieve an extended public key from the app', async function() {
      flowAccept(this.speculos);
      const key = await this.ava.getWalletExtendedPublicKey("44'/9000'/0'/0/0");
      expect(key).to.have.property('public_key').to.equalBytes('046b3cdd6f3313c11165a28463715f9cdb704f8163d04f25e814c0471c58da35637469a60d22c1eab5347c3a0a2920f27539730ebfc74d172c200a8164eaa70878');
      expect(key).to.have.property('chain_code').to.equalBytes('3b63e0f576c7b865a46c357bcfb2751e914af951f84e5eef0592e9ea7e3ea3c2');
    });
  });

  context('Signing', function () {
    it('can sign a hash-sized sequence of bytes with one path', async function () {
      const hash = "111122223333444455556666777788889999aaaabbbbccccddddeeeeffff0000";
      const pathPrefix = "44'/9000'/1'";
      const pathSuffix = "0/0";

      const prompts = flowAccept(this.speculos, 3);
      const sigs = await this.ava.signHash(
        BIPPath.fromString(pathPrefix),
        [BIPPath.fromString(pathSuffix, false)],
        Buffer.from(hash, "hex"),
      );
      expect(sigs).to.have.keys([pathSuffix]);
      expect(sigs.get(pathSuffix)).to.have.length(65);

      expect(await prompts).to.deep.equal([
        {"3":"Sign","17":"Hash"},
        {"3":"Derivation Prefix","17":pathPrefix},
        {"3":"Hash","17":"111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEEFFFF0000"},
      ]);

      for (suffix in sigs) {
        const sig = sigs.get(suffix);
        flowAccept(this.speculos);
        const key = (await this.ava.getWalletExtendedPublicKey(pathPrefix + "/" + suffix)).public_key;
        const recovered = recover(Buffer.from(hash, 'hex'), sig.slice(0, 64), sig[64], false);
        expect(recovered).is.equalBytes(key);
      }
    });

    it('Signing rejects when given garbage', async function () {
      const pathPrefix = "44'/9000'/1'";
      const firstMessage = Buffer.concat([
        this.ava.uInt8Buffer(1),
        Buffer.from("111122223333444455556666777788889999aaaabbbbccccddddeeeeffff0000", "hex"),
        this.ava.encodeBip32Path(BIPPath.fromString(pathPrefix)),
      ]);

      const prompts = flowAccept(this.speculos, 3);
      await this.speculos.send(this.ava.CLA, this.ava.INS_SIGN_HASH, 0x00, 0x00, firstMessage);

      expect(await prompts).to.deep.equal([
        {"3":"Sign","17":"Hash"},
        {"3":"Derivation Prefix","17":pathPrefix},
        {"3":"Hash","17":"111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEEFFFF0000"},
      ]);

      let err = {};
      await this.speculos.send(this.ava.CLA, this.ava.INS_SIGN_HASH, 0x01, 0x00, Buffer.from("00001111", 'hex')).catch(e => err = e);
      expect(err).has.property('statusCode', 0x6a80); // WRONG_VALUES
      expect(err).has.property('statusText', 'INCORRECT_DATA');
    });
  });

});

