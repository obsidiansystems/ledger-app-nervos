const chai = require('chai');
const { expect, assert } = chai.use(require('chai-bytes'));
chai.config.showDiff = true;
chai.config.truncateThreshold = 0;
const fc = require('fast-check');

describe("APDU protocol integrity generative tests", function () {
  context('Generative tests', function () {
    it('rejects incorrect APDU numbers', async function () {
      return await fc.assert(fc.asyncProperty(fc.integer(6, 255), fc.hexaString(), async (apdu, hashHex) => {
        const body = Buffer.from(hashHex, 'hex');
        try {
          await this.speculos.send(this.ckb.CLA, apdu, 0x00, 0x00, body);
          throw("Expected error");
        } catch (e) {
          this.flushStderr();
          expect(e).has.property('statusCode', 0x6e00); // INS_NOT_SUPPORTED
        }
      }), { examples: [[9, '']] });
    });

    it('rejects garbage on correct APDU numbers', async function () {
      return await fc.assert(fc.asyncProperty(fc.integer(0, 4), fc.hexaString(), fc.integer(0,255), async (apdu, hashHex, p1) => {
        const body = Buffer.from(hashHex, 'hex');
        try {
          await this.speculos.send(this.ckb.CLA, apdu, p1, 0x00, body);
          throw "Expected error";
        } catch (e) {
          this.flushStderr();
        }
      }));
    });

    it('rejects garbage dumped straight to the device', async function () {
      return await fc.assert(fc.asyncProperty(fc.hexaString(2, 512), async hashHex => {
        const body = Buffer.from(hashHex, 'hex');
        const rv = await this.speculos.exchange(body);
        expect(rv).to.not.equalBytes(this.ckb.MAGIC_9K);
        this.flushStderr();
      }));
    });

    it('rejects short garbage dumped straight to the device', async function () {
      return await fc.assert(fc.asyncProperty(fc.hexaString(10, 64), async hashHex => {
        const body = Buffer.from(hashHex, 'hex');
        let rv = Buffer.from(this.ckb.MAGIC_9K, "hex");
        try {
          rv = await this.speculos.exchange(body);
        } catch(e) {
          return; // Errors that get here are probably from the transport, not from the ledger.
        }

        expect(rv).to.not.equalBytes(this.ckb.MAGIC_9K);
        this.flushStderr();
      }));
    });
  });
});
