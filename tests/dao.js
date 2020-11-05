describe("DAO operations", () => {
  it("Signing with strict checking and a DAO deposit passes", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm DAO", body:"Deposit"},
      {header:"Deposit Amount", body:"1000"},
      {header:"Fee", body:"0.01"},
      {header:"Cell Owner", body:"ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7sugcvv9"},
    ]);

    signPath = [
        2147483692,
        2147483957,
        2147483648,
        0,
        0
    ];
    let txn = {
      signPath,
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        0,
        0
      ],
      "inputCount": 2,
      "raw": {
        "version": 0,
        "cell_deps": [
          {
            "out_point": {
              "tx_hash": "a563884b3686078ec7e7677a5f86449b15cf2693f3c1241766c6996f206cc541",
              "index": 2
            },
            "dep_type": 0
          },
          {
            "out_point": {
              "tx_hash": "ace5ea83c478bb866edf122ff862085789158f5cbff155b7bb5f13058555b708",
              "index": 0
            },
            "dep_type": 1
          }
        ],
        "header_deps": [],
        "inputs": [
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "b1b547956a0dfb7ea618231563b3acd23607586e939f88e5a6db5f392b2e78d5",
                "index": 1
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "a563884b3686078ec7e7677a5f86449b15cf2693f3c1241766c6996f206cc541",
                    "index": 2
                  },
                  "dep_type": 0
                },
                {
                  "out_point": {
                    "tx_hash": "ace5ea83c478bb866edf122ff862085789158f5cbff155b7bb5f13058555b708",
                    "index": 0
                  },
                  "dep_type": 1
                }
              ],
              "header_deps": [
                "327f1fc62c53530c6c27018f1e8cee27c35c0370c3b4d3376daf8fe110e7d8cb"
              ],
              "inputs": [
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "c399495011b912999dbc72cf54982924e328ae170654ef76c8aba190ca376307",
                    "index": 0
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "c317d0b0b2a513ab1206e6d454c1960de7d7b4b80d0748a3e1f9cb197b74b8a5",
                    "index": 1
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "000000174876e800",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": {
                    "code_hash": "82d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e",
                    "hash_type": 1,
                    "args": ""
                  }
                },
                {
                  "capacity": "0000001773b2e564",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": null
                }
              ],
              "outputs_data": [
                "5207000000000000",
                ""
              ]
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "258e82bab2af21fd8899fc872742f4acea831f5e4c232297816b9bf4a19597a9",
                "index": 0
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "a563884b3686078ec7e7677a5f86449b15cf2693f3c1241766c6996f206cc541",
                    "index": 2
                  },
                  "dep_type": 0
                },
                {
                  "out_point": {
                    "tx_hash": "ace5ea83c478bb866edf122ff862085789158f5cbff155b7bb5f13058555b708",
                    "index": 0
                  },
                  "dep_type": 1
                }
              ],
              "header_deps": [
                "327f1fc62c53530c6c27018f1e8cee27c35c0370c3b4d3376daf8fe110e7d8cb",
                "4930ba433e606a53f4f283f02dddeb6d51b0dc3e463629b14a27995de9c71eca"
              ],
              "inputs": [
                {
                  "since": "ba08000000010020",
                  "previous_output": {
                    "tx_hash": "b1b547956a0dfb7ea618231563b3acd23607586e939f88e5a6db5f392b2e78d5",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "00000017634361c5",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": null
                }
              ],
              "outputs_data": [
                ""
              ]
            }
          }
        ],
        "outputs": [
          {
            "capacity": "000000174876e800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
            },
            "type_": {
              "code_hash": "82d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e",
              "hash_type": 1,
              "args": ""
            }
          },
          {
            "capacity": "000000178e701ce9",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "0000000000000000",
          ""
        ]
      },
      "witnesses": [
        "",
        ""
      ]
    };
    sig = await this.ckb.signAnnotatedTransaction(txn);
    key = await getKeyFromLedgerCached(this, signPath);

    checkSignature(txn, sig, key);

    await flow.promptsPromise;
  });
  it("Signing with strict checking and a DAO prepare passes", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm DAO", body:"Prepare"},
      {header:"Deposit Amount", body:"1000"},
      {header:"Fee", body:"0.01"},
      {header:"Cell Owner", body:"ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7sugcvv9"},
    ]);

    await this.ckb.signAnnotatedTransaction({
      "signPath": [
        2147483692,
        2147483957,
        2147483648,
        0,
        0
      ],
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        0,
        0
      ],
      "inputCount": 2,
      "raw": {
        "version": 0,
        "cell_deps": [
          {
            "out_point": {
              "tx_hash": "a563884b3686078ec7e7677a5f86449b15cf2693f3c1241766c6996f206cc541",
              "index": 2
            },
            "dep_type": 0
          },
          {
            "out_point": {
              "tx_hash": "ace5ea83c478bb866edf122ff862085789158f5cbff155b7bb5f13058555b708",
              "index": 0
            },
            "dep_type": 1
          }
        ],
        "header_deps": [
          "6ee4972d1e676e0c921c611d1f3c2d58da3d843492ca60c659dc48d688d7d081"
        ],
        "inputs": [
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "67c3c7d31dddd84f834d9ab79cd46da9be8a6218c520fa87bcee9277609638ad",
                "index": 0
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "a563884b3686078ec7e7677a5f86449b15cf2693f3c1241766c6996f206cc541",
                    "index": 2
                  },
                  "dep_type": 0
                },
                {
                  "out_point": {
                    "tx_hash": "ace5ea83c478bb866edf122ff862085789158f5cbff155b7bb5f13058555b708",
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
                    "tx_hash": "b1b547956a0dfb7ea618231563b3acd23607586e939f88e5a6db5f392b2e78d5",
                    "index": 1
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "258e82bab2af21fd8899fc872742f4acea831f5e4c232297816b9bf4a19597a9",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "000000174876e800",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": {
                    "code_hash": "82d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e",
                    "hash_type": 1,
                    "args": ""
                  }
                },
                {
                  "capacity": "000000178e701ce9",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": null
                }
              ],
              "outputs_data": [
                "0000000000000000",
                ""
              ]
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "e8f6e0ee550df41b5ed579f0f991758affe5f971bf5d980afdf720e56f7cfcac",
                "index": 1
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "a563884b3686078ec7e7677a5f86449b15cf2693f3c1241766c6996f206cc541",
                    "index": 2
                  },
                  "dep_type": 0
                },
                {
                  "out_point": {
                    "tx_hash": "ace5ea83c478bb866edf122ff862085789158f5cbff155b7bb5f13058555b708",
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
                    "tx_hash": "455c8fdbaa43b45a1bbbc1cc9c93b4aa65897b2e443e057f5fd7c4dd23f07354",
                    "index": 1
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "000000174876e800",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": {
                    "code_hash": "82d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e",
                    "hash_type": 1,
                    "args": ""
                  }
                },
                {
                  "capacity": "00000076be02de80",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": null
                }
              ],
              "outputs_data": [
                "0000000000000000",
                ""
              ]
            }
          }
        ],
        "outputs": [
          {
            "capacity": "000000174876e800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
            },
            "type_": {
              "code_hash": "82d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e",
              "hash_type": 1,
              "args": ""
            }
          },
          {
            "capacity": "00000076bdf39c40",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "4b0a000000000000",
          ""
        ]
      },
      "witnesses": [
        "10000000100000001000000010000000",
        "10000000100000001000000010000000"
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing with strict checking and a DAO withdrawal passes", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm DAO", body:"Withdrawal"},
      {header:"Deposit Amount", body:"1000"},
      {header:"Compensation", body:"2.89011952"},
      {header:"Cell Owner", body:"ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7sugcvv9"},
    ]);

    await this.ckb.signAnnotatedTransaction({
      "signPath": [
        2147483692,
        2147483957,
        2147483648,
        0,
        0
      ],
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        0,
        0
      ],
      "inputCount": 1,
      "raw": {
        "version": 0,
        "cell_deps": [
          {
            "out_point": {
              "tx_hash": "a563884b3686078ec7e7677a5f86449b15cf2693f3c1241766c6996f206cc541",
              "index": 2
            },
            "dep_type": 0
          },
          {
            "out_point": {
              "tx_hash": "ace5ea83c478bb866edf122ff862085789158f5cbff155b7bb5f13058555b708",
              "index": 0
            },
            "dep_type": 1
          }
        ],
        "header_deps": [
          "6ee4972d1e676e0c921c611d1f3c2d58da3d843492ca60c659dc48d688d7d081",
          "d72c819927184fdb04e161dc3beea5b6adeba37a0575d2d965de98c1e56e4890"
        ],
        "inputs": [
          {
            "input": {
              "since": "b30b000000010020",
              "previous_output": {
                "tx_hash": "380bd6a76e0f9715491e2962af2cd6d5e9eeb87ca7192e02423b392746c2a46c",
                "index": 0
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "a563884b3686078ec7e7677a5f86449b15cf2693f3c1241766c6996f206cc541",
                    "index": 2
                  },
                  "dep_type": 0
                },
                {
                  "out_point": {
                    "tx_hash": "ace5ea83c478bb866edf122ff862085789158f5cbff155b7bb5f13058555b708",
                    "index": 0
                  },
                  "dep_type": 1
                }
              ],
              "header_deps": [
                "6ee4972d1e676e0c921c611d1f3c2d58da3d843492ca60c659dc48d688d7d081"
              ],
              "inputs": [
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "67c3c7d31dddd84f834d9ab79cd46da9be8a6218c520fa87bcee9277609638ad",
                    "index": 0
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "e8f6e0ee550df41b5ed579f0f991758affe5f971bf5d980afdf720e56f7cfcac",
                    "index": 1
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "000000174876e800",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": {
                    "code_hash": "82d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e",
                    "hash_type": 1,
                    "args": ""
                  }
                },
                {
                  "capacity": "00000076bdf39c40",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": null
                }
              ],
              "outputs_data": [
                "4b0a000000000000",
                ""
              ]
            }
          }
        ],
        "outputs": [
          {
            "capacity": "0000001759b0e0f0",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          ""
        ]
      },
      "witnesses": [
        "1c00000010000000100000001c000000080000000000000000000000"
      ]
    });

    await flow.promptsPromise;
  });
});
