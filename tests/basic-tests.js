const { expect } = require('chai').use(require('chai-bytes'));
const { recover } = require('bcrypto/lib/secp256k1');

describe("Basic Tests", () => {
  context('Basic APDUs', function () {
    it('Can fetch the version of the app', async function () {
      cfg = await this.ava.getAppConfiguration();
      expect(cfg).to.be.a('object');
      expect(cfg).to.have.property("version", "0.1.0");
      expect(cfg).to.have.property("name", "Avax");
    });
    it('Returns the expected wallet ID', async function () {
      id = await this.ava.getWalletId();
      expect(id).to.equalBytes('f0e476edaffc');
    });
  });

  context('Public Keys', function () {
    it('Can retrieve a public key from the app', async function() {
      flowAccept(this.speculos);
      key = this.ava.getWalletPublicKey("44'/9000'/0'/0/0");
      expect(await key).to.equalBytes('046b3cdd6f3313c11165a28463715f9cdb704f8163d04f25e814c0471c58da35637469a60d22c1eab5347c3a0a2920f27539730ebfc74d172c200a8164eaa70878');
    });
    it('Can retrieve a different public key from the app', async function() {
      flowAccept(this.speculos);
      key = this.ava.getWalletPublicKey("44'/9000'/1'/0/0");
      expect(await key).to.equalBytes('04617266d6f67281160fcfbebb285308d4f806cdae7f0bdf63045cca515056ee15b32d0de1fe9708094938d0633abc22d113a7c3a83d2d9f7399b792df3543a48d');
    });
    it('Produces the expected top-level extended key', async function() {
      flowAccept(this.speculos);
      key = this.ava.getWalletExtendedPublicKey("44'/9000'");
      expect(await key).to.have.property('public_key').equalBytes('044b68da714d7f8b9d97a9071f2977b587183972f0aa18a6af0b5917d3b2820686c521a7d4ac90a6565df51cb9e7a5309cd2d46907450bd8d8dd89ba16751ed8ee');
      expect(await key).to.have.property('chain_code').to.equalBytes('3b0c30e8b72f70ebe99698aca6ef8f380290c235337916b27730b301e978e664');
    });
    it('Can retrieve an extended public key from the app', async function() {
      flowAccept(this.speculos);
      key = this.ava.getWalletExtendedPublicKey("44'/9000'/0'/0/0");
      expect(await key).to.have.property('public_key').to.equalBytes('046b3cdd6f3313c11165a28463715f9cdb704f8163d04f25e814c0471c58da35637469a60d22c1eab5347c3a0a2920f27539730ebfc74d172c200a8164eaa70878');
      expect(await key).to.have.property('chain_code').to.equalBytes('3b63e0f576c7b865a46c357bcfb2751e914af951f84e5eef0592e9ea7e3ea3c2');
    });
  });

  context('Signing', function () {
    it('can sign a hash-sized sequence of bytes', async function () {
      prompts = flowAccept(this.speculos, 3);
      let hash = "111122223333444455556666777788889999aaaabbbbccccddddeeeeffff0000";
      let path = "44'/9000'/1'/0/0";
      sig = this.ava.signHash(path, Buffer.from(hash, "hex"));
      expect(await sig).to.have.property('hash').to.equalBytes(hash);
      expect(await sig).to.have.property('signature');

      expect(await prompts).to.deep.equal([
        {"3":"Sign","17":"Hash"},
        {"3":"Derivation Path","17":"44'/9000'/1'/0/0"},
        {"3":"Hash","17":"111122223333444455556666777788889999AAAABBBBCCCCDDDDEEEEFFFF0000"},
      ]);

      flowAccept(this.speculos);
      key = await this.ava.getWalletPublicKey(path);
      recovered = recover(Buffer.from(hash, 'hex'), (await sig).signature.slice(0,64), (await sig).signature[64], false);
      expect(recovered).is.equalBytes(key);
    });

    it('Signing rejects when given garbage', async function () {
      await this.speculos.send(0x80, this.ava.INS_SIGN_HASH, 0x00, 0x00, Buffer.from("048000002c800023288000000080000000", 'hex'));
      let err = {};
      rv = await this.speculos.send(0x80, this.ava.INS_SIGN_HASH, 0x01, 0x00, Buffer.from("00001111", 'hex')).catch(e => err = e);
      expect(err).has.property('statusCode', 0x6982);
      expect(err).has.property('statusText', 'SECURITY_STATUS_NOT_SATISFIED');
    });
  });

});

