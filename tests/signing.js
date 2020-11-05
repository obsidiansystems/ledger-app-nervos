describe("Signing transactions", () => {
  it("Signing with strict checking and a valid useful transaction passes", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Self-Transfer"},
      {header:"Amount", body:"2011.73959913"},
      {header:"Fee", body:"0.01"},
      {header:"Destination", body:"ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7sugcvv9"},
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
            "type_": null
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
          "",
          ""
        ]
      },
      "witnesses": [
        "",
        ""
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing a valid transaction: From account 'm/44'/309'/0'/1/1', change 'm/44'/309'/0'/1/1'", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"200"},
      {header:"Fee", body:"0.001"},
      {header:"Destination", body:"ckb1qyq0js0a0d04aumk2cdr95x9kza3e5gcqttq46cate"},
    ]);

    await this.ckb.signAnnotatedTransaction({
      "signPath": [
        2147483692,
        2147483957,
        2147483648,
        1,
        1
      ],
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        1,
        1
      ],
      "inputCount": 1,
      "raw": {
        "version": 0,
        "cell_deps": [
          {
            "out_point": {
              "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                "tx_hash": "39641a05511b85d641cc844d0e0e7766983c9a26b1d62d51f251de7125390b16",
                "index": 1
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                    "tx_hash": "39738e9ad7b3ac419a51a9d301cb96722d3e01f2ffbae7779bd31791fc24adad",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "00000004a817c800",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "f941fd7b5f5ef376561a32d0c5b0bb1cd11802d6"
                  },
                  "type_": null
                },
                {
                  "capacity": "00000012a05d9960",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "71882946c87d62f9b2f2b5be3be6b5c2704fec59"
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
            "capacity": "00000004a817c800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "f941fd7b5f5ef376561a32d0c5b0bb1cd11802d6"
            },
            "type_": null
          },
          {
            "capacity": "0000000df8444ac0",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "71882946c87d62f9b2f2b5be3be6b5c2704fec59"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          ""
        ]
      },
      "witnesses": [
        ""
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing a valid self transfer transaction", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Self-Transfer"},
      {header:"Amount", body:"5999.999"},
      {header:"Fee", body:"0.001"},
      {header:"Destination", body:"ckb1qyqqtw74sngmqqqzauwulk6e3rc0l46gmuxquwdcdl"},
    ]);

    await this.ckb.signAnnotatedTransaction({
      "signPath": [
        2147483692,
        2147483957,
        2147483648,
        0,
        3
      ],
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        0,
        3
      ],
      "inputCount": 1,
      "raw": {
        "version": 0,
        "cell_deps": [
          {
            "out_point": {
              "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                "tx_hash": "2e4f4999bc2ae5d1bf1964d1fb4fa2d7c9dff014faa0fb4130fcaf053a828b6b",
                "index": 0
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                    "tx_hash": "4f26933c260b39c605b4fe848f506459d97f3f7a3b4c1a3de80b026dd7a31114",
                    "index": 1
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "0000008bb2c97000",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "05bbd584d1b00002ef1dcfdb5988f0ffd748df0c"
                  },
                  "type_": null
                },
                {
                  "capacity": "1bc16b16c0d70360",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "cc4e78b857b8ea477304925ac0f67b7348b86761"
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
            "capacity": "000000574fbde600",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "05bbd584d1b00002ef1dcfdb5988f0ffd748df0c"
            },
            "type_": null
          },
          {
            "capacity": "00000034630a0360",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "05bbd584d1b00002ef1dcfdb5988f0ffd748df0c"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          ""
        ]
      },
      "witnesses": [
        ""
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing a valid transaction: From account 'm/44'/309'/0'/0/2', and same change path", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"1560"},
      {header:"Fee", body:"0.001"},
      {header:"Destination", body:"ckb1qyq2yksyd5g6jkumltkrx35qvz6hdlypatlslmrhdc"},
    ]);

    await this.ckb.signAnnotatedTransaction({
      "signPath": [
        2147483692,
        2147483957,
        2147483648,
        0,
        2
      ],
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        0,
        2
      ],
      "inputCount": 1,
      "raw": {
        "version": 0,
        "cell_deps": [
          {
            "out_point": {
              "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                "tx_hash": "4f26933c260b39c605b4fe848f506459d97f3f7a3b4c1a3de80b026dd7a31114",
                "index": 0
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                    "tx_hash": "be3abd7565dd5e106a454368d7acec71c7a5114121d9dfe07ad54c48434f615c",
                    "index": 1
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "0000002e90edd000",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "65c634f3e742961c8a4e71191138a71ee5ef9591"
                  },
                  "type_": null
                },
                {
                  "capacity": "1bc16ba273a1fa00",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "cc4e78b857b8ea477304925ac0f67b7348b86761"
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
            "capacity": "0000002452531800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "a25a046d11a95b9bfaec33468060b576fc81eaff"
            },
            "type_": null
          },
          {
            "capacity": "0000000a3e993160",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "65c634f3e742961c8a4e71191138a71ee5ef9591"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          ""
        ]
      },
      "witnesses": [
        ""
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing a valid transaction: From account 'm/44'/309'/0'/0/4', and change path 'm/44'/309'/0'/1/5'", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"1400"},
      {header:"Fee", body:"0.001"},
      {header:"Destination", body:"ckb1qyq8rzpfgmy86chektett03mu66uyuz0a3vstntx4r"},
    ]);

    await this.ckb.signAnnotatedTransaction({
      "signPath": [
        2147483692,
        2147483957,
        2147483648,
        0,
        4
      ],
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        1,
        5
      ],
      "inputCount": 1,
      "raw": {
        "version": 0,
        "cell_deps": [
          {
            "out_point": {
              "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                "tx_hash": "f44b26dda9f1f48b20029654be67c2e636b7fec62b877e20b695c2307dae4a52",
                "index": 0
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                    "tx_hash": "2e4f4999bc2ae5d1bf1964d1fb4fa2d7c9dff014faa0fb4130fcaf053a828b6b",
                    "index": 1
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "00000022ecb25c00",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "4c73d21a2cd6501255bf410dcab0265c2ee1fdcb"
                  },
                  "type_": null
                },
                {
                  "capacity": "1bc16af3d42320c0",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "cc4e78b857b8ea477304925ac0f67b7348b86761"
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
            "capacity": "0000002098a67800",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "71882946c87d62f9b2f2b5be3be6b5c2704fec59"
            },
            "type_": null
          },
          {
            "capacity": "00000002540a5d60",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "622f480dff8731d15c832d2234ee4eded0cfe398"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          ""
        ]
      },
      "witnesses": [
        ""
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing a valid transaction to multisig address passes (contains two inputs)", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"900"},
      {header:"Fee", body:"0.001"},
      {header:"Destination", body:"ckb1qyqljq0w3nk3gyu0xmztt9c4f02rtqujt3ts069qc6"},
    ]);

    await this.ckb.signAnnotatedTransaction({
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
              "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                "tx_hash": "3772e5f47d00993b8db0da7685ac27902c399ba6961275b8c7f8cd1286a3d1d6",
                "index": 1
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                    "tx_hash": "be3abd7565dd5e106a454368d7acec71c7a5114121d9dfe07ad54c48434f615c",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "00000014f46b0400",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "71882946c87d62f9b2f2b5be3be6b5c2704fec59"
                  },
                  "type_": null
                },
                {
                  "capacity": "00000002540a5d60",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "8d5520741f06a062543cdea9a21fc20d07ee29b0"
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
                "tx_hash": "21ea7484a585b81222114e36e4b6bb1d2026a0d7db5a1985727fe5d8ae264fcb",
                "index": 0
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                    "tx_hash": "13b9eda5f8a22a510f3a7fda74f5a4c0e0396dba1d328ecaa40925fd12d9b374",
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
                    "args": "8d5520741f06a062543cdea9a21fc20d07ee29b0"
                  },
                  "type_": null
                },
                {
                  "capacity": "1bc16a96b243fe50",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "cc4e78b857b8ea477304925ac0f67b7348b86761"
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
            "capacity": "00000014f46b0400",
            "lock": {
              "code_hash": "5c5069eb0857efc65e1bca0c07df34c31663b3622fd3876c876320fc9634e2a8",
              "hash_type": 1,
              "args": "f901ee8ced14138f36c4b597154bd43583925c57"
            },
            "type_": null
          },
          {
            "capacity": "00000004a814bac0",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "8d5520741f06a062543cdea9a21fc20d07ee29b0"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          ""
        ]
      },
      "witnesses": [
        "",
        ""
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing a valid transaction to multisig timelock address passes", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"300"},
      {header:"Fee", body:"0.001"},
      {header:"Destination", body:"ckb1q3w9q60tppt7l3j7r09qcp7lxnp3vcanvgha8pmvsa3jplykxn323x9vd6ppzz5775yjxd6mlh977m7e0xu2ccqqqquqgzq8yqvzdv4v"},
    ]);

    await this.ckb.signAnnotatedTransaction({
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
              "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                "tx_hash": "276d5447f20df863b13bcafd63de2ad851e4f35eda337268b19ab0cf7c29c608",
                "index": 1
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                    "tx_hash": "3772e5f47d00993b8db0da7685ac27902c399ba6961275b8c7f8cd1286a3d1d6",
                    "index": 1
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "21ea7484a585b81222114e36e4b6bb1d2026a0d7db5a1985727fe5d8ae264fcb",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "00000014f46b0400",
                  "lock": {
                    "code_hash": "5c5069eb0857efc65e1bca0c07df34c31663b3622fd3876c876320fc9634e2a8",
                    "hash_type": 1,
                    "args": "f901ee8ced14138f36c4b597154bd43583925c57"
                  },
                  "type_": null
                },
                {
                  "capacity": "00000004a814bac0",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "8d5520741f06a062543cdea9a21fc20d07ee29b0"
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
                "tx_hash": "40e54fc2d1aabbd39ae6cd8a5df922995dd856f24f670d3d5f283690a534a544",
                "index": 0
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                    "tx_hash": "21ea7484a585b81222114e36e4b6bb1d2026a0d7db5a1985727fe5d8ae264fcb",
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
                    "args": "8d5520741f06a062543cdea9a21fc20d07ee29b0"
                  },
                  "type_": null
                },
                {
                  "capacity": "1bc16a7f69cb8fb0",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "cc4e78b857b8ea477304925ac0f67b7348b86761"
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
            "capacity": "00000006fc23ac00",
            "lock": {
              "code_hash": "5c5069eb0857efc65e1bca0c07df34c31663b3622fd3876c876320fc9634e2a8",
              "hash_type": 1,
              "args": "98ac6e82110a9ef50923375bfdcbef6fd979b8ac6000003804080720"
            },
            "type_": null
          },
          {
            "capacity": "00000014f4667020",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "8d5520741f06a062543cdea9a21fc20d07ee29b0"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          ""
        ]
      },
      "witnesses": [
        "",
        ""
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing a valid multisig transaction passes", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"499.999"},
      {header:"Fee", body:"0.001"},
      {header:"Destination", body:"ckb1qyqvph9q9t9643hufg56y0dxls75z4j4c4ws0enyar"},
    ]);

    await this.ckb.signAnnotatedTransaction({
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
      "inputCount": 1,
      "raw": {
        "version": 0,
        "cell_deps": [
          {
            "out_point": {
              "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
              "index": 1
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
                "tx_hash": "4a533a3a693d25bdd3d2cebfcdcf9a9b664d89a6e75baadcaae2c475078e6525",
                "index": 0
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                    "tx_hash": "80e736727ac08e31741f33359850a2fa1ed8e082908c1fbb02cf9d9133cee566",
                    "index": 1
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "0000000ba43b7400",
                  "lock": {
                    "code_hash": "5c5069eb0857efc65e1bca0c07df34c31663b3622fd3876c876320fc9634e2a8",
                    "hash_type": 1,
                    "args": "f901ee8ced14138f36c4b597154bd43583925c57"
                  },
                  "type_": null
                },
                {
                  "capacity": "1bc16ac54333a310",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "cc4e78b857b8ea477304925ac0f67b7348b86761"
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
            "capacity": "0000000ba439ed60",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "c0dca02acbaac6fc4a29a23da6fc3d415655c55d"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          ""
        ]
      },
      "witnesses": [
        "c200000010000000c2000000c2000000ae000000000002028d5520741f06a062543cdea9a21fc20d07ee29b0e421a57de6ed580189463131ccb6d5843dab975d00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing a valid multisig transaction passes (2)", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"1799.999"},
      {header:"Fee", body:"0.001"},
      {header:"Destination", body:"ckb1qyqvcnnchptm36j8wvzfykkq7eahxj9cvassyxfpys"},
    ]);

    await this.ckb.signAnnotatedTransaction({
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
      "inputCount": 1,
      "raw": {
        "version": 0,
        "cell_deps": [
          {
            "out_point": {
              "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
              "index": 1
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
                "tx_hash": "e52f7a2c51a88db162abade844ecba29c64f7e1b50c8ee30ac4240095bf3a9c3",
                "index": 0
              }
            },
            "source": {
              "version": 0,
              "cell_deps": [
                {
                  "out_point": {
                    "tx_hash": "01ef8910ef4e71349763523a077eea304b0e852c45b04d5b56c482306f4f6d93",
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
                    "tx_hash": "40e54fc2d1aabbd39ae6cd8a5df922995dd856f24f670d3d5f283690a534a544",
                    "index": 1
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "00000029e8d60800",
                  "lock": {
                    "code_hash": "5c5069eb0857efc65e1bca0c07df34c31663b3622fd3876c876320fc9634e2a8",
                    "hash_type": 1,
                    "args": "9cd462f96bd34a61e0a553e57efd2ba3414a5610"
                  },
                  "type_": null
                },
                {
                  "capacity": "1bc16a5580f560a0",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "cc4e78b857b8ea477304925ac0f67b7348b86761"
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
            "capacity": "00000029e8d48160",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "cc4e78b857b8ea477304925ac0f67b7348b86761"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          ""
        ]
      },
      "witnesses": [
        "9e010000100000009e0100009e0100008a0100000000020d8d5520741f06a062543cdea9a21fc20d07ee29b0e421a57de6ed580189463131ccb6d5843dab975dcc4e78b857b8ea477304925ac0f67b7348b867619c8ce01eaf3910b8b18c32a4fec37f3d35f84041e5260d839a786ac2a909181df9a423f1efbe863da25a046d11a95b9bfaec33468060b576fc81eaff83462eafd93f0a598ab26597e5cda6523b2fc15371882946c87d62f9b2f2b5be3be6b5c2704fec5965c634f3e742961c8a4e71191138a71ee5ef95910320d01cac0c3ca512069f6909196675cd4deab905bbd584d1b00002ef1dcfdb5988f0ffd748df0c4c73d21a2cd6501255bf410dcab0265c2ee1fdcb622f480dff8731d15c832d2234ee4eded0cfe39800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing with strict checking of a transaction having a type script fails", async function() {
    try {
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
                    "capacity": "00000017 73b2e564",
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
                "code_hash": "83d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e",
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
      });
    } catch (e) {
      expect(e.toString()).is.equal("Error: Invalid data length! Required: 8, actual: 3");
    }
  });
  it("Signing with strict checking and a different change address passes", async function() {
    const flow = await flowAccept(this.speculos, [
      {header:"Confirm", body:"Transaction"},
      {header:"Amount", body:"1000"},
      {header:"Fee", body:"0.01"},
      {header:"Destination", body:"ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7sugcvv9"},
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
        2147483649,
        2147483649
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
                  "capacity": "0000001773b2e564 ",
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
            "type_": null
          },
          {
            "capacity": "000000178e701ce9",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "b690f63b6fca7abff77c558bffa7401ec40dcb52"
            },
            "type_": null
          }
        ],
        "outputs_data": [
          "",
          ""
        ]
      },
      "witnesses": [
        "",
        ""
      ]
    });

    await flow.promptsPromise;
  });
  it("Signing with strict checking, a plain transfer, and data in an output cell fails", async function() {
    try {
      await this.ckb.signAnnotatedTransaction({
        "signPath": [
          2147483692,
          2147483957,
          2147483648,
          0,
          0
        ],
        "changePath": [
          7168,
          8192,
          28160,
          45568,
          57856
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
              "type_": null
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
            "deadbeef",
            ""
          ]
        },
        "witnesses": [
          "",
          ""
        ]
      });
    } catch (e) {
      expect(e).has.property('statusCode', 0x6985);
      expect(e).has.property('statusText', 'CONDITIONS_OF_USE_NOT_SATISFIED');
    }
  });
  it("Signing with strict checking, a plain transfer, and data in an output cell fails even with eight bytes of data", async function() {
    try {
      await this.ckb.signAnnotatedTransaction({
        "signPath": [
          2147483692,
          2147483957,
          2147483648,
          0,
          0
        ],
        "changePath": [
          50177,
          47105,
          7168,
          8192,
          28160
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
                    "capacity": "00000017634361c5 ",
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
              "capacity": "000000174876e800 ",
              "lock": {
                "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                "hash_type": 1,
                "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
              },
              "type_": null
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
            "deadbeefdeadbeef",
            ""
          ]
        },
        "witnesses": [
          "",
          ""
        ]
      });
    } catch (e) {
      expect(e).has.property('statusCode', 0x6985);
      expect(e).has.property('statusText', 'CONDITIONS_OF_USE_NOT_SATISFIED');
    }
  });
});
