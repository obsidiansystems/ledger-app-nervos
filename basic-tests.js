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
      const pathPrefix = "44'/9000'/1'";
      const pathSuffixes = ["0/0", "0/1", "100/100"];
      const sigPromise = signTransaction(this.ava, pathPrefix, pathSuffixes);
      const prompts = [
        await flowAccept(this.speculos, 1, "Next"),
        await flowAccept(this.speculos, 1, "Next"),
        await flowAccept(this.speculos, 1, "Next"),
        await flowAccept(this.speculos, 1, "Next"),
        await flowAccept(this.speculos, 1),
      ];

      await checkSignTransactionResult(this.ava, await sigPromise, pathPrefix, pathSuffixes);

      expect(prompts).to.deep.equal([
        [{"3":"Sign","17":"Transaction"}],
        [{"3":"Amount","17":"12345"}],
        [{"3":"To Address","17":"denali12yp9cc0melq83a5nxnurf0nd6fk4t224dtg0lx"}],
        [{"3":"To Address","17":"denali1cv6yz28qvqfgah34yw3y53su39p6kzzexk8ar3"}],
        [{"3":"Finalize","17":"Transaction"}],
      ]);
    });

    it('can sign a sample everest transaction', async function () {
      console.log("TESTING");

      const txn = Buffer.from([
        // Codec ID
        0x00, 0x00,
        // Type ID
        0x00, 0x00, 0x00, 0x00,
        // Network ID (everest)
        0x00, 0x00, 0x00, 0x04,
        // Blockchain ID (everest)
        0x61, 0x25, 0x84, 0x21, 0x39, 0x7C, 0x02, 0x35,
        0xBD, 0x6D, 0x67, 0x81, 0x2A, 0x8B, 0x2C, 0x1C,
        0xF3, 0x39, 0x29, 0x50, 0x0A, 0x7F, 0x69, 0x16,
        0xBB, 0x2F, 0xC4, 0xAC, 0x64, 0x6A, 0xC0, 0x91,
        // number of outputs
        0x00, 0x00, 0x00, 0x02,
        // transferrable output 1
        0x68, 0x70, 0xB7, 0xD6, 0x6A, 0xC3, 0x25, 0x40,
        0x31, 0x13, 0x79, 0xE5, 0xB5, 0xDB, 0xAD, 0x28,
        0xEC, 0x7E, 0xB8, 0xDD, 0xBF, 0xC8, 0xF4, 0xD6,
        0x72, 0x99, 0xEB, 0xB4, 0x84, 0x75, 0x90, 0x7A, // 32-byte asset ID
        0x00, 0x00, 0x00, 0x07, // output type (SECP256K1)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE8, // amount
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // locktime
        0x00, 0x00, 0x00, 0x01, // threshold
        0x00, 0x00, 0x00, 0x01, // number of addresses
        0x7F, 0x67, 0x1C, 0x73, 0x0D, 0x48, 0x07, 0xC2,
        0x9E, 0xA1, 0x9B, 0x19, 0xA2, 0x3C, 0x70, 0x0B,
        0x19, 0x8F, 0x8B, 0x51, // 20-byte address
        // transferrable ouput 2
        0x68, 0x70, 0xB7, 0xD6, 0x6A, 0xC3, 0x25, 0x40,
        0x31, 0x13, 0x79, 0xE5, 0xB5, 0xDB, 0xAD, 0x28,
        0xEC, 0x7E, 0xB8, 0xDD, 0xBF, 0xC8, 0xF4, 0xD6,
        0x72, 0x99, 0xEB, 0xB4, 0x84, 0x75, 0x90, 0x7A, // 32-byte asset ID
        0x00, 0x00, 0x00, 0x07, // output type (SECP256K1)
        0x00, 0x00, 0x00, 0x00, 0x00, 0x6A, 0xCB, 0xD8, // amount
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // locktime
        0x00, 0x00, 0x00, 0x01, // threshold
        0x00, 0x00, 0x00, 0x01, // number of addresses
        0xA4, 0xAF, 0xAB, 0xFF, 0x30, 0x81, 0x95, 0x25,
        0x99, 0x90, 0xA9, 0xE5, 0x31, 0xBD, 0x82, 0x30,
        0xD1, 0x1A, 0x9A, 0x2A, // 20-byte address
        // number of inputs
        0x00, 0x00, 0x00, 0x02,
        // transferrable input 1
        0x1C, 0x03, 0x06, 0xE5, 0x8B, 0x75, 0x4E, 0xEB,
        0x92, 0xE7, 0xA5, 0x79, 0xC5, 0x9A, 0x69, 0x33,
        0x23, 0xCD, 0x99, 0x94, 0xA5, 0x94, 0x61, 0x62,
        0x72, 0x6F, 0x3B, 0x68, 0x0E, 0x9E, 0x48, 0x34, // 32-byte TX ID
        0x00, 0x00, 0x00, 0x00, // UTXO index
        0x68, 0x70, 0xB7, 0xD6, 0x6A, 0xC3, 0x25, 0x40,
        0x31, 0x13, 0x79, 0xE5, 0xB5, 0xDB, 0xAD, 0x28,
        0xEC, 0x7E, 0xB8, 0xDD, 0xBF, 0xC8, 0xF4, 0xD6,
        0x72, 0x99, 0xEB, 0xB4, 0x84, 0x75, 0x90, 0x7A, // 32-byte asset ID
        0x00, 0x00, 0x00, 0x05, // type ID
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, // amount
        0x00, 0x00, 0x00, 0x01, // number of address indices
        0x00, 0x00, 0x00, 0x00, // address index 1

        0x29, 0x71, 0x0D, 0xE0, 0x93, 0xE2, 0xF4, 0x10,
        0xB5, 0xA3, 0x5E, 0x2C, 0x60, 0x59, 0x38, 0x39,
        0x2D, 0xA0, 0xDE, 0x80, 0x2C, 0x74, 0xE2, 0x5D,
        0x78, 0xD2, 0xBF, 0x11, 0x87, 0xDC, 0x9A, 0xD6, // 32-byte TX ID
        0x00, 0x00, 0x00, 0x00, // UTXO index
        0x68, 0x70, 0xB7, 0xD6, 0x6A, 0xC3, 0x25, 0x40,
        0x31, 0x13, 0x79, 0xE5, 0xB5, 0xDB, 0xAD, 0x28,
        0xEC, 0x7E, 0xB8, 0xDD, 0xBF, 0xC8, 0xF4, 0xD6,
        0x72, 0x99, 0xEB, 0xB4, 0x84, 0x75, 0x90, 0x7A, // 32-byte asset ID
        0x00, 0x00, 0x00, 0x05, // type ID
        0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x11, 0x9C, // amount
        0x00, 0x00, 0x00, 0x01, // number of address indices
        0x00, 0x00, 0x00, 0x00, // address index 1
        // memo length
        0x00, 0x00, 0x00, 0x04,
        // memo
        0x00, 0x00, 0x00, 0x00,
      ]);

      const pathPrefix = "44'/9000'/1'";
      const pathSuffixes = ["0/0", "0/1", "100/100"];
      console.log("Sending transaction... (" + txn.length + ")");
      const sigPromise = this.ava.signTransaction(
        BIPPath.fromString(pathPrefix),
        pathSuffixes.map(x => BIPPath.fromString(x, false)),
        txn,
      );
      const prompts = [
        await flowAccept(this.speculos, 1, "Next"),
        await flowAccept(this.speculos, 1, "Next"),
        await flowAccept(this.speculos, 1, "Next"),
        await flowAccept(this.speculos, 1, "Next"),
        await flowAccept(this.speculos, 1, "Next"),
        await flowAccept(this.speculos, 1),
      ];

      await checkSignTransactionResult(this.ava, await sigPromise, pathPrefix, pathSuffixes);

      expect(prompts).to.deep.equal([
        [{"3":"Sign","17":"Transaction"}],
        [{"3":"Amount","17":"1000"}],
        [{"3":"To Address","17":"everest10an3cucdfqru984pnvv6y0rspvvclz63qnegnr"}],
        [{"3":"Amount","17":"6999000"}],
        [{"3":"To Address","17":"everest15jh6hlessx2jtxvs48jnr0vzxrg34x32ef0ckt"}],
        [{"3":"Finalize","17":"Transaction"}],
      ]);
    });

    it('rejects a transaction that has extra data', async function () {
      try {
        const sigPromise = signTransaction(this.ava, "44'/9000'/1'", ["0/0"], {
          extraEndBytes: Buffer.from([0x00])
        });
        await flowAccept(this.speculos, 1, "Next");
        await flowAccept(this.speculos, 1, "Next");
        await flowAccept(this.speculos, 1, "Next");
        await flowAccept(this.speculos, 1, "Next");
        await sigPromise;
        throw "Signing should have been rejected";
      } catch (e) {
        expect(e).has.property('statusCode', 0x9405); // PARSE_ERROR
        expect(e).has.property('statusText', 'UNKNOWN_ERROR');
      }
    });

    it('rejects an unrecognized codec ID', async function () {
      await expectSignFailure(this.ava, { codecId: Buffer.from([0x01, 0x00]) });
    });

    it('rejects an unrecognized type ID', async function () {
      await expectSignFailure(this.ava, { typeId: Buffer.from([0x01, 0x00, 0x00, 0x00]) });
    });

    it('rejects an unrecognized network ID', async function () {
      const prompts1 = flowAccept(this.speculos, 1, "Next");
      await expectSignFailure(this.ava, { networkId: Buffer.from([0x01, 0x00, 0x00, 0x00]) });
      expect(await prompts1).to.deep.equal([{"3":"Sign","17":"Transaction"}]);
    });

    it('rejects a recognized network ID that does not match blockchain ID', async function () {
      const prompts1 = flowAccept(this.speculos, 1, "Next");
      await expectSignFailure(this.ava, { networkId: Buffer.from([0x00, 0x00, 0x00, 0x01]) });
      expect(await prompts1).to.deep.equal([{"3":"Sign","17":"Transaction"}]);
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

async function signTransaction(
  ava,
  pathPrefix,
  pathSuffixes,
  fieldOverrides = {},
) {
  const fields = {
    ...{
      codecId: Buffer.from([0x00, 0x00]),
      typeId: Buffer.from([0x00, 0x00, 0x00, 0x00]),
      networkId: Buffer.from([0x00, 0x00, 0x00, 0x03]),
      extraEndBytes: Buffer.from([]),
    },
    ...fieldOverrides,
  }

  const txn = Buffer.concat([
    fields.codecId,
    fields.typeId,
    fields.networkId,
    Buffer.from([
      // blockchainID: (denali)
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
    ]),
    fields.extraEndBytes,
  ]);

  console.log("Sending transaction... (" + txn.length + ")");
  return await ava.signTransaction(
    BIPPath.fromString(pathPrefix),
    pathSuffixes.map(x => BIPPath.fromString(x, false)),
    txn,
  );
}

async function expectSignFailure(ava, fields) {
  try {
    await signTransaction(ava, "44'/9000'/1'", ["0/0"], fields);
    throw "Signing should have been rejected";
  } catch (e) {
    expect(e).has.property('statusCode', 0x9405); // PARSE_ERROR
    expect(e).has.property('statusText', 'UNKNOWN_ERROR');
  }
}

async function checkSignTransactionResult(ava, sig, pathPrefix, pathSuffixes) {
  expect(sig).to.have.property('hash');
  expect(sig).to.have.property('signatures');

  expect(sig.hash).to.have.length(32);
  expect(sig.signatures).to.have.length(pathSuffixes.length);
  expect(sig.signatures).to.have.keys(pathSuffixes);

  for (suffix in sig.signatures) {
    const sig = sigs.get(suffix);
    expect(sig).to.have.length(65);

    const prompts = flowAccept(this_.speculos);
    const key = (await ava.getWalletExtendedPublicKey(pathPrefix + "/" + suffix)).public_key;
    await prompts;
    const recovered = recover(Buffer.from(hash, 'hex'), sig.slice(0, 64), sig[64], false);
    expect(recovered).is.equalBytes(key);
  }
}
