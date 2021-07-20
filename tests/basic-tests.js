const { recover } = require('bcrypto/lib/secp256k1')
const BIPPath = require("bip32-path");
const { expect } = require('chai').use(require('chai-bytes'));

describe("Basic Tests", () => {
  context('Basic APDUs', function () {
    it('can fetch the version of the app', async function () {
      const cfg = await this.ckb.getAppConfiguration();
      expect(cfg).to.be.a('object');
      expect(cfg).to.have.property("version", "0.5.2");
      if (process.env.COMMIT && process.env.COMMIT != "TEST*")
        expect(cfg).to.have.property("hash", process.env.COMMIT);
    });
    it('returns the expected wallet ID', async function () {
      const id = await this.ckb.getWalletId();
      expect(id).to.equal('9c6e60f3e812ef5c859bbc900f427bffe63294c5490f93e4e50beb688c0798bf');
    });
  });
});
