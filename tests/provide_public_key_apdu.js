context('Public Keys', function () {
  it("Ledger app produces a public key upon request", async function() {
    const flow = await flowAccept(this.speculos);
    const key = await this.ckb.getWalletPublicKey("44'/309'/0'/0'");
    expect(key).to.equalBytes('4104a8b62ce15a23548ff07e103d611899607e156692097ab49a656dfc680e16dc7f13b1fe16f341359e71e0d198bc912e37435d1f0893f675131cb190b005fbe10720bc3885f4beef7d23afaecf015f5945e76cdcee45bd1b0c557be3627d5d8cbe63');
    await flow.promptsPromise;
  });
  it("Ledger app produces a different public key upon request", async function() {
    const flow = await flowAccept(this.speculos, [{header:"Provide", body:"Public Key"}, {header:"Address", body:"ckb1qyqtdy8k8dhu574l7a79tzll5aqpa3qdedfquq4vvy"}]);
    // Also checking that we prompt with a mainnet address.
    const key = await this.ckb.getWalletPublicKey("44'/309'/1'/1'");
    expect(key).to.equalBytes('4104a8b62ce15a23548ff07e103d611899607e156692097ab49a656dfc680e16dc7f13b1fe16f341359e71e0d198bc912e37435d1f0893f675131cb190b005fbe10720bc3885f4beef7d23afaecf015f5945e76cdcee45bd1b0c557be3627d5d8cbe63');
    await flow.promptsPromise;
  });
  it('Ledger app produces the expected top-level extended key', async function() {
    await flowAccept(this.speculos);
    const key = await this.ckb.getWalletExtendedPublicKey("44'/309'");
    expect(key).to.have.property('public_key').equalBytes('044b68da714d7f8b9d97a9071f2977b587183972f0aa18a6af0b5917d3b2820686c521a7d4ac90a6565df51cb9e7a5309cd2d46907450bd8d8dd89ba16751ed8ee');
    expect(key).to.have.property('chain_code').to.equalBytes('3b0c30e8b72f70ebe99698aca6ef8f380290c235337916b27730b301e978e664');
  });
  it('can retrieve an extended public key from the app', async function() {
    await flowAccept(this.speculos);
    const key = await this.ckb.getWalletExtendedPublicKey("44'/309'/0'/0/0");
    expect(key).to.have.property('public_key').to.equalBytes('046b3cdd6f3313c11165a28463715f9cdb704f8163d04f25e814c0471c58da35637469a60d22c1eab5347c3a0a2920f27539730ebfc74d172c200a8164eaa70878');
    expect(key).to.have.property('chain_code').to.equalBytes('3b63e0f576c7b865a46c357bcfb2751e914af951f84e5eef0592e9ea7e3ea3c2');
  });
  it('Ledger app prompts with a mainnet key by default', async function() {
    
  });
});
