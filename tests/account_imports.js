
it("Ledger app does account import for account 1", async function() {
  const flow = await flowAccept(this.speculos, [{header:"Import", body:"Account"}]);
  const account = await this.speculos.exchange(Buffer.from('800500000400000000', 'hex'));
  expect(account.toString('hex')).to.equal('41040221fbe070c5a6dea584b700a23f80dac6138b4f2fc602eba40cecde6ccc4b6f5f10f4bb9bc6fe7349e4d9607ea99ec15aff49aa1dc57e22abe9f7a52536ea352047550f332614039f5d463b4f9ac7b3b355f506d71cffedebc5f50f271cd9b06b' + this.ckb.MAGIC_9K);
  await flow.promptsPromise;
});
