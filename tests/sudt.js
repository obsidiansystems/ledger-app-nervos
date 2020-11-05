const createSudtTx = {
  "signPath": [
    2147483692,
    2147483957,
    2147483648
  ],
  "changePath": [
    2147483692,
    2147483957,
    2147483648
  ],
  "inputCount": 2,
  "raw": {
    "version": 0,
    "cell_deps": [
      {
        "out_point": {
          "tx_hash": "f8de3bb47d055cdf460d93a2a6e1b05f7432f9777c8c474abf4eec1d4aee5d37",
          "index": 0
        },
        "dep_type": 1
      },
      {
        "out_point": {
          "tx_hash": "c1b2ae129fad7465aaa9acc9785f842ba3e6e8b8051d899defa89f5508a77958",
          "index": 0
        },
        "dep_type": 0
      }
    ],
    "header_deps": [],
    "inputs": [
      {
        "input": {
          "since": "0000000000000000",
          "previous_output": {
            "tx_hash": "33c4aa927cee3dde055820355d1d86c79daafd8de2781a69469afb572796c0c2",
            "index": 1
          }
        },
        "source": {
          "version": 0,
          "cell_deps": [
            {
              "out_point": {
                "tx_hash": "f8de3bb47d055cdf460d93a2a6e1b05f7432f9777c8c474abf4eec1d4aee5d37",
                "index": 0
              },
              "dep_type": 1
            }
          ],
          "header_deps": [],
          "inputs": [
            {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "0fdc433d61a63975c3523e806f91fb400c3a5110993b3e5e6275d7ce7fb79949",
                "index": 1
              }
            }
          ],
          "outputs": [
            {
              "capacity": "0000000a3814d0a4",
              "lock": {
                "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                "hash_type": 1,
                "args": "1e08c86ef36b9b13e8fbebcfd6af6905bc188ca8"
              },
              "type_": null
            },
            {
              "capacity": "000000016b969d00",
              "lock": {
                "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                "hash_type": 1,
                "args": "632c444199275d00b7c1fb65bf001d70bc609599"
              },
              "type_": null
            }
          ],
          "outputs_data": [
            "",
            ""
          ]
        }
      },
      {
        "input": {
          "since": "0000000000000000",
          "previous_output": {
            "tx_hash": "5efe4be10b8703f5e4b49195518029f74c4325477c960539d0bdbc7761d1f6f8",
            "index": 1
          }
        },
        "source": {
          "version": 0,
          "cell_deps": [
            {
              "out_point": {
                "tx_hash": "f8de3bb47d055cdf460d93a2a6e1b05f7432f9777c8c474abf4eec1d4aee5d37",
                "index": 0
              },
              "dep_type": 1
            }
          ],
          "header_deps": [],
          "inputs": [
            {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "acbeff01d0e4180df845581a70d709c7bc3069e9824e99fc1f145782a2cd7b62",
                "index": 0
              }
            }
          ],
          "outputs": [
            {
              "capacity": "000000016b009d96",
              "lock": {
                "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                "hash_type": 1,
                "args": "53d8aa5f8795be30afdf9294058f98fb4ea088d3"
              },
              "type_": null
            },
            {
              "capacity": "00000072febbe930",
              "lock": {
                "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                "hash_type": 1,
                "args": "632c444199275d00b7c1fb65bf001d70bc609599"
              },
              "type_": null
            }
          ],
          "outputs_data": [
            "",
            ""
          ]
        }
      }
    ],
    "outputs": [
      {
        "capacity": "000000034e62ce00",
        "lock": {
          "code_hash": "86a1c6987a4acbe1a887cca4c9dd2ac9fcb07405bbeda51b861b18bbf7492c4b",
          "hash_type": 1,
          "args": "632c444199275d00b7c1fb65bf001d70bc609599"
        },
        "type_": null
      },
      {
        "capacity": "000000711b14b3ef",
        "lock": {
          "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
          "hash_type": 1,
          "args": "632c444199275d00b7c1fb65bf001d70bc609599"
        },
        "type_": null
      }
    ],
    "outputs_data": [
      "00000000000000000000000000000000",
      ""
    ]
  },
  "witnesses": [
    "55000000100000005500000055000000410000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
  ]
};

describe("sUDT operations", () => {
  it("Rejects sUDT create account by default", async function() {
    try {
      await this.ckb.signAnnotatedTransaction(createSudtTx);
      throw "Expected failure";
    } catch (e) {
      expect(e).has.property('statusCode', 0x6985);
      expect(e).has.property('statusText', 'CONDITIONS_OF_USE_NOT_SATISFIED');
    }
  });

  it("Accepts sUDT create account when enabled in settings", async function() {
    let flipContractDataPolicy = async (target) => {return await automationStart(this.speculos, async (speculos, screens) => {
      speculos.button("Rr");
      while((await screens.next()).body != "Configuration") speculos.button("Rr");
      speculos.button("RLrl");
      let policy;
      while((policy = await screens.next()).header != "Allow contract data") {
        speculos.button("Rr");
      }
      while(policy.body != target) {
        speculos.button("RLrl");
        policy = await screens.next();
      }
      do { speculos.button("Rr") } while((await screens.next()).body != "Main menu");
      speculos.button("RLrl");

      return { promptsMatch: true };
    })};

    await (await flipContractDataPolicy("On")).promptsPromise;

    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"4999.85646063"},
      {header:"Fee", body:"0.14353473"},
      {header:"Destination", body:"ckb1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsqltkvh"},
      {header:"Contract", body:"Present"},
    ]);

    await this.ckb.signAnnotatedTransaction(createSudtTx);

    await flow.promptsPromise;

    await (await flipContractDataPolicy("Off")).promptsPromise;
  });
});
