const { recover } = require('bcrypto/lib/secp256k1')
const BIPPath = require("bip32-path");
const { expect } = require('chai').use(require('chai-bytes'));

describe("Basic Tests", () => {
  context('Basic APDUs', function () {
    it('can fetch the version of the app', async function () {
      const cfg = await this.ckb.getAppConfiguration();
      expect(cfg).to.be.a('object');
      // Restore these to check that the version and hash actually match.
      expect(cfg).to.have.property("version");
      expect(cfg).to.have.property("hash");
    });
    it('returns the expected wallet ID', async function () {
      const id = await this.ckb.getWalletId();
      expect(id).to.equal('9c6e60f3e812ef5c859bbc900f427bffe63294c5490f93e4e50beb688c0798bf');
    });
  });
});

async function flowMultiPrompt(speculos, prompts, nextPrompt="Next", finalPrompt="Accept") {
  return await automationStart(speculos, async (speculos, screens) => {
    for (p of prompts.slice(0,-1)) {
      const rp = (await acceptPrompts(undefined, nextPrompt)(speculos, screens)).promptList;
      // Only looking at the last prompt, because we bounce off the home screen sometimes during this process:
      expect([ rp[rp.length-1] ]).to.deep.equal(p);
    }
    const rp = (await acceptPrompts(undefined, finalPrompt)(speculos, screens)).promptList;
    expect([ rp[rp.length-1] ]).to.deep.equal(prompts[prompts.length-1]);
    return true;
  });
}

