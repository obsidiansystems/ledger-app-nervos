const { recover } = require('bcrypto/lib/secp256k1')
const BIPPath = require("bip32-path");
const { expect } = require('chai').use(require('chai-bytes'));
const createHash = require('create-hash');

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

      const prompts = flowAccept(this.speculos, signHashPrompts("", "").length);
      await this.speculos.send(this.ava.CLA, this.ava.INS_SIGN_HASH, 0x00, 0x00, firstMessage);

      expect(await prompts).to.deep.equal(
        signHashPrompts(
          "111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEEFFFF0000",
          pathPrefix,
        )
      );

      try {
        await this.speculos.send(this.ava.CLA, this.ava.INS_SIGN_HASH, 0x81, 0x00, Buffer.from("00001111", 'hex'));
        throw "Expected failure";
      } catch (e) {
        expect(e).has.property('statusCode', 0x6a80); // WRONG_VALUES
        expect(e).has.property('statusText', 'INCORRECT_DATA');
      }
    });

    it('can sign the transaction from the serialization reference in verbose mode', async function () {
      const txn = Buffer.from([
        // Type ID
        0x00, 0x00, 0x00, 0x00,
        // networkID:
        0x00, 0x00, 0x00, 0x03,
        // blockchainID:
        0x71, 0x30, 0x1a, 0x03, 0x75, 0x0a, 0x14, 0x8a,
        0xb5, 0x1e, 0xad, 0x71, 0x8c, 0x20, 0x89, 0xda,
        0xd3, 0x8a, 0x28, 0x54, 0x5e, 0xdb, 0xe0, 0xc7,
        0xe0, 0xc3, 0xfe, 0x1d, 0x25, 0xdc, 0x7f, 0x03,
        // number of outputs:
        0x00, 0x00, 0x00, 0x01,
        // transferable output:
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x30, 0x39, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xd4, 0x31, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x02, 0x51, 0x02, 0x5c, 0x61,
        0xfb, 0xcf, 0xc0, 0x78, 0xf6, 0x93, 0x34, 0xf8,
        0x34, 0xbe, 0x6d, 0xd2, 0x6d, 0x55, 0xa9, 0x55,
        0xc3, 0x34, 0x41, 0x28, 0xe0, 0x60, 0x12, 0x8e,
        0xde, 0x35, 0x23, 0xa2, 0x4a, 0x46, 0x1c, 0x89,
        0x43, 0xab, 0x08, 0x59,
        // number of inputs:
        0x00, 0x00, 0x00, 0x01,
        // transferable input:
        0xf1, 0xe1, 0xd1, 0xc1, 0xb1, 0xa1, 0x91, 0x81,
        0x71, 0x61, 0x51, 0x41, 0x31, 0x21, 0x11, 0x01,
        0xf0, 0xe0, 0xd0, 0xc0, 0xb0, 0xa0, 0x90, 0x80,
        0x70, 0x60, 0x50, 0x40, 0x30, 0x20, 0x10, 0x00,
        0x00, 0x00, 0x00, 0x05, 0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
        0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
        0x1c, 0x1d, 0x1e, 0x1f, 0x00, 0x00, 0x00, 0x05,
        0x00, 0x00, 0x00, 0x00, 0x07, 0x5b, 0xcd, 0x15,
        0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03,
        0x00, 0x00, 0x00, 0x07,
        // Memo length:
        0x00, 0x00, 0x00, 0x04,
        // Memo:
        0x00, 0x01, 0x02, 0x03,
      ]);

      console.log("Sending transaction... (" + txn.length + ")");
      const prefixPath = BIPPath.fromString("44'/9000'/1'");
      const suffixPath = BIPPath.fromString("0/0", false);
      const sigPromise = this.ava.signTransaction(prefixPath, [suffixPath], txn);
      prompts1 = await flowAccept(this.speculos, 1);
      prompts2 = await flowAccept(this.speculos, 1);
      prompts3 = await flowAccept(this.speculos, 1);
      prompts4 = await flowAccept(this.speculos, 1);
      const sig = await sigPromise;
      expect(sig).to.have.property('hash');
      expect(sig).to.have.property('signatures');

      const hash = Buffer.from(createHash('sha256').update(txn).digest());
      expect(sig.hash).to.equalBytes(hash);

      expect(prompts1).to.deep.equal([{"3":"Sign","17":"Transaction"}]);
      expect(prompts2).to.deep.equal([{"3":"Amount","17":"12345"}]);
      expect(prompts3).to.deep.equal([{"3":"To Address","17":"denali12yp9cc0melq83a5nxnurf0nd6fk4t224dtg0lx"}]);
      expect(prompts4).to.deep.equal([{"3":"To Address","17":"denali1cv6yz28qvqfgah34yw3y53su39p6kzzexk8ar3"}]);

      expect(sig.signatures).to.have.keys([suffixPath.toString(true)]);

      for (suffix in sig.signatures) {
        const sig = sigs.get(suffix);
        expect(sig).to.have.length(65);

        flowAccept(this_.speculos);
        const key = (await this.ava.getWalletExtendedPublicKey(pathPrefix + "/" + suffix)).public_key;
        const recovered = recover(Buffer.from(hash, 'hex'), sig.slice(0, 64), sig[64], false);
        expect(recovered).is.equalBytes(key);
      }
    });
  });
});

async function checkSignHash(this_, pathPrefix, pathSuffixes, hash) {
  const prompts = flowAccept(this_.speculos, signHashPrompts("").length);
  const sigs = await this_.ava.signHash(
    BIPPath.fromString(pathPrefix),
    pathSuffixes.map(x => BIPPath.fromString(x, false)),
    Buffer.from(hash, "hex"),
  );

  expect(await prompts).to.deep.equal(
    signHashPrompts(
      "111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEEFFFF0000",
      pathPrefix,
    )
  );

  expect(sigs).to.have.keys(pathSuffixes);

  for (suffix in sigs) {
    const sig = sigs.get(suffix);
    expect(sig).to.have.length(65);

    flowAccept(this_.speculos);
    const key = (await this_.ava.getWalletExtendedPublicKey(pathPrefix + "/" + suffix)).public_key;
    const recovered = recover(Buffer.from(hash, 'hex'), sig.slice(0, 64), sig[64], false);
    expect(recovered).is.equalBytes(key);
  }
}

function signHashPrompts(hash, pathPrefix) {
  return [
    {"3":"Sign","17":"Hash"},
    {"3":"DANGER!","17":"YOU MUST verify this manually!!!"},
    {"3":"Derivation Prefix","17":pathPrefix},
    {"3":"Hash","17":hash},
    {"3":"Are you sure?","17":"This is very dangerous!"},
  ];
}
