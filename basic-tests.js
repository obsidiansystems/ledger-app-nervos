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
      await flowAccept(this.speculos);
      const key = await this.ava.getWalletAddress("44'/9000'/0'/0/0");
      expect(key).to.equalBytes('41c9cc6fd27e26e70f951869fb09da685a696f0a');
    });
    it('Can retrieve a different address from the app', async function() {
      await flowAccept(this.speculos);
      const key = await this.ava.getWalletAddress("44'/9000'/1'/0/0");
      expect(key).to.equalBytes('f14c91be3a26e3ce30f970d87257fd2fb3dfbb7f');
    });
    it('Produces the expected top-level extended key', async function() {
      await flowAccept(this.speculos);
      const key = await this.ava.getWalletExtendedPublicKey("44'/9000'");
      expect(key).to.have.property('public_key').equalBytes('044b68da714d7f8b9d97a9071f2977b587183972f0aa18a6af0b5917d3b2820686c521a7d4ac90a6565df51cb9e7a5309cd2d46907450bd8d8dd89ba16751ed8ee');
      expect(key).to.have.property('chain_code').to.equalBytes('3b0c30e8b72f70ebe99698aca6ef8f380290c235337916b27730b301e978e664');
    });
    it('Can retrieve an extended public key from the app', async function() {
      await flowAccept(this.speculos);
      const key = await this.ava.getWalletExtendedPublicKey("44'/9000'/0'/0/0");
      expect(key).to.have.property('public_key').to.equalBytes('046b3cdd6f3313c11165a28463715f9cdb704f8163d04f25e814c0471c58da35637469a60d22c1eab5347c3a0a2920f27539730ebfc74d172c200a8164eaa70878');
      expect(key).to.have.property('chain_code').to.equalBytes('3b63e0f576c7b865a46c357bcfb2751e914af951f84e5eef0592e9ea7e3ea3c2');
    });
  });

  context('Signing', function () {
    it('can sign a hash-sized sequence of bytes with one path', async function () {
      await checkSignHash(
        this,
        "44'/9000'/1'",
        ["0/0"],
        "111122223333444455556666777788889999aaaabbbbccccddddeeeeffff0000"
      );
    });

    it('can sign a hash-sized sequence of bytes with many paths', async function () {
      await checkSignHash(
        this,
        "44'/9000'/1'",
        ["0/0", "1/20", "1'/200'", "3000'/90030'"],
        "111122223333444455556666777788889999aaaabbbbccccddddeeeeffff0000"
      );
    });

    it('cannot sign a hash-sized sequence of bytes with long paths', async function () {
      try {
        await checkSignHash(
          this,
          "44'/9000'/1'",
          ["0/0/0/0/0"],
          "111122223333444455556666777788889999aaaabbbbccccddddeeeeffff0000"
        );
        throw "Expected failure";
      } catch (e) {
        expect(e).has.property('statusCode', 0x9200); // MEMORY_ERROR
        expect(e).has.property('statusText', 'UNKNOWN_ERROR');
      }
    });

    it('refuses to sign when given an invalid path suffix', async function () {
      const pathPrefix = "44'/9000'/1'";
      const firstMessage = Buffer.concat([
        this.ava.uInt8Buffer(1),
        Buffer.from("111122223333444455556666777788889999aaaabbbbccccddddeeeeffff0000", "hex"),
        this.ava.encodeBip32Path(BIPPath.fromString(pathPrefix)),
      ]);

      const prompts = await flowAccept(this.speculos, [
        {"3":"Sign","17":"Hash"},
        {"3":"Derivation Prefix","17":pathPrefix},
        {"3":"Hash","17":"111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEEFFFF0000"},
      ]);
      await this.speculos.send(this.ava.CLA, this.ava.INS_SIGN_HASH, 0x00, 0x00, firstMessage);

      await prompts.promptsMatch;

      try {
        await this.speculos.send(this.ava.CLA, this.ava.INS_SIGN_HASH, 0x81, 0x00, Buffer.from("00001111", 'hex'));
        throw "Expected failure";
      } catch (e) {
        expect(e).has.property('statusCode', 0x6a80); // WRONG_VALUES
        expect(e).has.property('statusText', 'INCORRECT_DATA');
      }
    });
  });
});

async function checkSignHash(this_, pathPrefix, pathSuffixes, hash) {
  const prompts = await flowAccept(this_.speculos, [
    {"3":"Sign","17":"Hash"},
    {"3":"Derivation Prefix","17":pathPrefix},
    {"3":"Hash","17":"111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEEFFFF0000"},
  ]);
  const sigs = await this_.ava.signHash(
    BIPPath.fromString(pathPrefix),
    pathSuffixes.map(x => BIPPath.fromString(x, false)),
    Buffer.from(hash, "hex"),
  );

  await prompts.promptsMatch;

  expect(sigs).to.have.keys(pathSuffixes);

  for (suffix in sigs) {
    const sig = sigs.get(suffix);
    expect(sig).to.have.length(65);

    await flowAccept(this_.speculos);
    const key = (await this_.ava.getWalletExtendedPublicKey(pathPrefix + "/" + suffix)).public_key;
    const recovered = recover(Buffer.from(hash, 'hex'), sig.slice(0, 64), sig[64], false);
    expect(recovered).is.equalBytes(key);
  }
}
