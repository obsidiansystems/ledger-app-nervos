context('Public Keys', function () {
  it("Ledger app produces a public key upon request", async function() {
    const flow = await flowAccept(this.speculos);
    const key = await this.ckb.getWalletPublicKey("44'/309'/0'/0'");

    expect(key).to.have.property('publicKey').to.equal('04d066dbe5603004dc4c83e27106b098f5e9b9b17b6bdec965810cd0921193b1c87206518153a0b44c7ae6b0be92a60d515d454e71ab27943bbb323273b8bdd46a');
    expect(key).to.have.property('lockArg').to.equal('a3912c9414bfd45b830478c05385dfa4d560c859');
    expect(key).to.have.property('address').to.equal('ckb1qyq28yfvjs2tl4zmsvz83sznsh06f4tqepvsc3zqeu');

    await flow.promptsPromise;
  });

  it("Ledger app produces a different public key upon request", async function() {
    const flow = await flowAccept(
      this.speculos,
      [
        {header:"Provide", body:"Public Key"},
        {header:"Address", body:"ckb1qyqtdy8k8dhu574l7a79tzll5aqpa3qdedfquq4vvy"}
      ]
    );

    // Also checking that we prompt with a mainnet address.
    const key = await this.ckb.getWalletPublicKey("44'/309'/1'/1'");

    expect(key).to.have.property('publicKey').to.equal('0457aa28c6e9e4f188e54bd687c34391920d2b53c2c83251cb93275cb955b83c35db429b5c5a8559a2deb2e5056f82468b1612b1ea94d75028ef8dfcd952ee0d84');
    expect(key).to.have.property('lockArg').to.equal('b690f63b6fca7abff77c558bffa7401ec40dcb52');
    expect(key).to.have.property('address').to.equal('ckb1qyqtdy8k8dhu574l7a79tzll5aqpa3qdedfquq4vvy');

    await flow.promptsPromise;
  });

  it('Ledger app produces the expected top-level extended key', async function() {
    await flowAccept(this.speculos);
    const key = await this.ckb.getWalletExtendedPublicKey("44'/309'");
    expect(key).to.have.property('public_key').to.equal('047fba43fe55f433b7a1d806a8432679aebaa652738dae4afe6b8161257e6a96d39519312488d0dc92aca92b80ee970f856f72177cbc5bf1332df4d1bb8fa328fd');
    expect(key).to.have.property('chain_code').to.equal('4fd0dbde97deebcb9935816176fa33f97a78e8d61e018ea761f491398936e62b');
  });

  it('can retrieve an extended public key from the app', async function() {
    await flowAccept(this.speculos);
    const key = await this.ckb.getWalletExtendedPublicKey("44'/309'/0'/0/0");
    expect(key).to.have.property('public_key').to.equal('043abf7271ee6b1abdaf6d38021c2dc4cce9bb112a69c3992394627b793f94c4084ce99ce10f0e8ed51d1911af769b9cc87f16f5b842415c1b65895111a4ec2221');
    expect(key).to.have.property('chain_code').to.equal('a1cd2b47aaf6186e2b3f227d553fde0fc4661c6adc968b4005a19d0b5d43e293');
  });

  // it('Ledger app prompts with a mainnet key by default', async function() {
  // });
});
