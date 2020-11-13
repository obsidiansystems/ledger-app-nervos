describe("Multi-output transaction signing", () => {

  it("Signing with more than one output to distinct destinations passes and shows both outputs in the prompts.", async function() {
  // it("fred", async function() {
    const signPath = [
      2147483692,
      2147483957,
      2147483648,
      0,
      0
    ];

    const txn = {
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
          }
        ],
        "outputs": [
          {
            "capacity": "0000000ba43b7400",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863a"
            },
            "type_": null
          },
          {
            "capacity": "0000000ba43b7400",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863b"
            },
            "type_": null
          },
          {
            "capacity": "000000002b3bf97c",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          "",
          ""
        ]
      },
      "witnesses": [
        ""
      ]
    };

    // device output appears out of order
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"1000"},
      {header:"Fee", body:"0.00001"},
      {header:"Output 1/2", body:"500 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7scaqc8pnyu"},
      {header:"Output 2/2", body:"500 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7scasdxft30"},
    ]);

    const sig = await this.ckb.signAnnotatedTransaction(txn);
    const key = await getKeyFromLedgerCached(this, signPath);

    checkSignature(txn, sig, key);

    await flow.promptsPromise;
  });

  it("Signing with three outputs that roll-up to 2 distinct destinations passes and shows both outputs.", async function() {
    const signPath = [
      2147483692,
      2147483957,
      2147483648,
      0,
      0
    ];

    const txn = {
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
          }
        ],
        "outputs": [
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863a"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863a"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863c"
            },
            "type_": null
          },
          {
            "capacity": "000000002b3bf97c",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          "",
          "",
          "",
          ""
        ]
      },
      "witnesses": [
        ""
      ]
    };

    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"600"},
      {header:"Fee", body:"400.00001"},
      {header:"Output 1/2", body:"400 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7scaqc8pnyu"},
      {header:"Output 2/2", body:"200 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7qffs5ek"},
    ]);

    const sig = await this.ckb.signAnnotatedTransaction(txn);
    const key = await getKeyFromLedgerCached(this, signPath);

    checkSignature(txn, sig, key);

    await flow.promptsPromise;
  });

  it("Signing with three outputs to distinct destinations passes and shows each output.", async function() {
    const signPath = [
      2147483692,
      2147483957,
      2147483648,
      0,
      0
    ];

    const txn = {
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
          }
        ],
        "outputs": [
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863a"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863b"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863c"
            },
            "type_": null
          },
          {
            "capacity": "000000002b3bf97c",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          "",
          "",
          "",
          ""
        ]
      },
      "witnesses": [
        ""
      ]
    };

    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"600"},
      {header:"Fee", body:"400.00001"},
      {header:"Output 1/3", body:"200 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7scaqc8pnyu"},
      {header:"Output 2/3", body:"200 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7scasdxft30"},
      {header:"Output 3/3", body:"200 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7qffs5ek"},
    ]);

    const sig = await this.ckb.signAnnotatedTransaction(txn);
    const key = await getKeyFromLedgerCached(this, signPath);

    checkSignature(txn, sig, key);

    await flow.promptsPromise;
  });


  it("Signing with four outputs to distinct destinations passes and shows each output.", async function() {
    const signPath = [
      2147483692,
      2147483957,
      2147483648,
      0,
      0
    ];

    const txn = {
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
          }
        ],
        "outputs": [
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863a"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863b"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863c"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863e"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          "",
          "",
          ""
        ]
      },
      "witnesses": [
        ""
      ]
    };

    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"800"},
      {header:"Fee", body:"207.2535178"},
      {header:"Output 1/4", body:"200 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7scaqc8pnyu"},
      {header:"Output 2/4", body:"200 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7scasdxft30"},
      {header:"Output 3/4", body:"200 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7qffs5ek"},
      {header:"Output 4/4", body:"200 CKB -> ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sclqx5lwjs"},
    ]);

    const sig = await this.ckb.signAnnotatedTransaction(txn);
    const key = await getKeyFromLedgerCached(this, signPath);

    checkSignature(txn, sig, key);

    await flow.promptsPromise;
  });

  it("Signing with more than five output to distinct destinations rejects.", async function() {
    const txn = {
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
          }
        ],
        "outputs": [
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863a"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863b"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863c"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863e"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863f"
            },
            "type_": null
          },
          {
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863f"
            },
            "type_": null
          },
          {
            "capacity": "000000002b3bf97c",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          "",
          "",
          "",
          ""
        ]
      },
      "witnesses": [
        ""
      ]
    };

    try {
      await this.ckb.signAnnotatedTransaction(txn);
    } catch (e) {
      // "Error: Can't handle more than five outputs";
      expect(e).has.property('statusCode', 0x6985);
      expect(e).has.property('statusText', 'CONDITIONS_OF_USE_NOT_SATISFIED');
    }
  });
});
