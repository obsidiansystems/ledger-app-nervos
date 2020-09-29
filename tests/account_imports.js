
it("Ledger app does account import for account 1", async function() {
  const flow = await flowAccept(this.speculos, [{header:"Import", body:"Account"}]);
  this.speculos.send(Buffer.from("800500000400000000", "hex"));
  await flow.promptsPromise;
});
