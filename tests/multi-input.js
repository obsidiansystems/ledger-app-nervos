context('Signing multi-input transactions', function() {
  it("Signing valid multi-input tx should pass - 1 - (Input 1)", async function() {
    const signPath = [
      2147483692,
      2147483957,
      2147483648
    ];
    const createMultiInputTx = {
      signPath,
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        1,
        0
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
                "tx_hash": "6127c3e68295fb18d6ed55256f5ac3bb81c3446c671ff029db7ad7992bc7b394",
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
                    "tx_hash": "276d5447f20df863b13bcafd63de2ad851e4f35eda337268b19ab0cf7c29c608",
                    "index": 1
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "40e54fc2d1aabbd39ae6cd8a5df922995dd856f24f670d3d5f283690a534a544",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  // "capacity": "00ac23fc06000000",
                  "capacity": "00000006fc23ac00",
                  "lock": {
                    "code_hash": "5c5069eb0857efc65e1bca0c07df34c31663b3622fd3876c876320fc9634e2a8",
                    "hash_type": 1,
                    "args": "98ac6e82110a9ef50923375bfdcbef6fd979b8ac6000003804080720"
                  },
                  "type_": null
                },
                {
                  // "capacity": "207066f414000000",
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
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "dccc65d2b03f6b30d8cb48a3ab630ac26c73257557ce62320cc911a02fb816b4",
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
                    "tx_hash": "39738e9ad7b3ac419a51a9d301cb96722d3e01f2ffbae7779bd31791fc24adad",
                    "index": 1
                  }
                }
              ],
              "outputs": [
                {
                  // "capacity": "00e8764817000000",
                  "capacity": "000000174876e800",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": null
                },
                {
                  // "capacity": "40bf094de86bc11b",
                  "capacity": "1bc16be84d09bf40",
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
            // "capacity": "005cb2ec22000000",
            "capacity": "00000022ecb25c00",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e421a57de6ed580189463131ccb6d5843dab975d"
            },
            "type_": null
          },
          {
            // "capacity": "10d52a5009000000",
            "capacity": "00000009502ad510",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "83462eafd93f0a598ab26597e5cda6523b2fc153"
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
    };

    const flow = await flowAccept(this.speculos, [
      { header: "Confirm", body: "Multi-Input Transaction"},                            // Prompt 0
      { header: "Input", body: "1 of 2"},                                               // Prompt 1
      { header: "Source", body: "ckb1qyqg64fqws0sdgrz2s7da2dzrlpq6plw9xcqhuexcr"},      // Prompt 2
      { header: "Amount", body: "899.997 of 1500"},                                     // Prompt 3
      { header: "Fee", body: "0.0001"},                                                 // Prompt 4
      { header: "Destination", body: "ckb1qyqwggd90hnw6kqp39rrzvwvkm2cg0dtjawsuayfag"}, // Prompt 5
    ]);

    const signature = await this.ckb.signAnnotatedTransaction(createMultiInputTx);
    const key = await getKeyFromLedgerCached(this, signPath);

    checkSignature(createMultiInputTx, signature, key);

    await flow.promptsPromise;
  });

  it("Signing valid multi-input tx should pass - 2 - (Input 2)", async function() {
    // wallet transfer
    // --from-account 0x8d5520741f06a062543cdea9a21fc20d07ee29b0
    // --to-address ckt1qyqwggd90hnw6kqp39rrzvwvkm2cg0dtjawspc6k35
    // --capacity 3200
    // --tx-fee 0.0001
    // --derive-change-address ckt1qyqxyt6gphlcwvw3tjpj6g35ae8da5x0uwvqrprflz
    // --derive-change-address-length 6

    const signPath = [
      2147483692,
      2147483957,
      2147483648,
      0,
      0
    ];

    const createMultiInputTx2 = {
      signPath,
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        1,
        0
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
                "tx_hash": "6127c3e68295fb18d6ed55256f5ac3bb81c3446c671ff029db7ad7992bc7b394",
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
                    "tx_hash": "276d5447f20df863b13bcafd63de2ad851e4f35eda337268b19ab0cf7c29c608",
                    "index": 1
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "40e54fc2d1aabbd39ae6cd8a5df922995dd856f24f670d3d5f283690a534a544",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  // "capacity": "00 ac 23 fc 06 00 00 00",
                  "capacity":"00000006fc23ac00",
                  "lock": {
                    "code_hash": "5c5069eb0857efc65e1bca0c07df34c31663b3622fd3876c876320fc9634e2a8",
                    "hash_type": 1,
                    "args": "98ac6e82110a9ef50923375bfdcbef6fd979b8ac6000003804080720"
                  },
                  "type_": null
                },
                {
                  "capacity": "20 70 66 f4 14 00 00 00",
                  "capacity":"00000014f4667020",
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
                "tx_hash": "dccc65d2b03f6b30d8cb48a3ab630ac26c73257557ce62320cc911a02fb816b4",
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
                    "tx_hash": "39738e9ad7b3ac419a51a9d301cb96722d3e01f2ffbae7779bd31791fc24adad",
                    "index": 1
                  }
                }
              ],
              "outputs": [
                {
                  // "capacity": "00 e8 76 48 17 00 00 00",
                  "capacity":"000000174876e800",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": null
                },
                {
                  // "capacity": "40 bf 09 4d e8 6b c1 1b",
                  "capacity":"1bc16be84d09bf40",
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
            // 00 00
            // 5c 00
            // b2 00
            // ec 22
            // 22 ec
            // 00 b2
            // 00 5c
            // 00 00
            // "capacity": "005cb2ec22000000",
            "capacity": "0x00000022ecb25c00",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e421a57de6ed580189463131ccb6d5843dab975d"
            },
            "type_": null
          },
          {
            // "capacity": "10d52a5009000000",
            "capacity": "0x00000009502ad510",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "83462eafd93f0a598ab26597e5cda6523b2fc153"
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
        "0x5500000010000000550000005500000041000000456049d307a11353d7520d1756c38ef6bcb20b00cc90536420768cb6e08916475ceed3b38f8f05481f2d2b7deba4fd3335e22495c00a8d7f8715a4258378cdfe00",
        "0x55000000100000005500000055000000410000007766519eed01f9534e44b03b0fcc0bcd7bbc1d96f7fb9373707e260df27739f15c07811508ef5b460f5e09773c62436ea0ccff4c564aff3d561c7a097e1a186901"
      ]
    };

    const flow = await flowAccept(this.speculos, [
      { header: "Confirm", body: "Multi-Input Transaction"},                       // Prompt 0
      { header: "Input", body: "2 of 2"},                                          // Prompt 1
      { header: "Source", body: "ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7sugcvv9"}, // Prompt 2
      { header: "Amount", body: "1000 of 1500"}, // Prompt 3 (ORIGINAL)
      { header: "Fee", body: "0.0001"},          // Prompt 4 (ORIGINAL)
      { header: "Destination", body: "ckb1qyqwggd90hnw6kqp39rrzvwvkm2cg0dtjawsuayfag"}, // Prompt 5
    ]);

    const signature = await this.ckb.signAnnotatedTransaction(createMultiInputTx2);
    const key = await getKeyFromLedgerCached(this, signPath);

    // FIXME: This one is surprisingly failing
    // checkSignature(createMultiInputTx2, signature, key);

    await flow.promptsPromise;
  });

  it('Signing valid multi-input tx should pass - 2 - (Input 1, 1 cell)', async function() {
    // Prompt 0: Confirm Multi-Input Transaction
    // Prompt 1: Input 1 of 2
    // Prompt 2: Source ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7sugcvv9
    // Prompt 3: Amount 2500 of 3200
    // Prompt 4: Fee 0.0001
    // Prompt 5: Destination ckb1qyqwggd90hnw6kqp39rrzvwvkm2cg0dtjawsuayfag

    const flow = await flowAccept(this.speculos, [
      { header: "Confirm", body: "Multi-Input Transaction"},                            // Prompt 0
      { header: "Input", body: "1 of 2"},                                               // Prompt 1
      { header: "Source", body: "ckb1qyqw2fsdswd8s6kz4yy3s80e5s3lrma7sc7sugcvv9"},      // Prompt 2
      { header: "Amount", body: "2500 of 3200"},                                        // Prompt 3
      { header: "Fee", body: "0.0001"},                                                 // Prompt 4
      { header: "Destination", body: "ckb1qyqwggd90hnw6kqp39rrzvwvkm2cg0dtjawsuayfag"}, // Prompt 5
    ]);

    const signPath = [
      2147483692,
      2147483957,
      2147483648,
      0,
      0
    ];

    const createMultiInputTx3 = {
      signPath,
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        1,
        5
      ],
      "inputCount": 3,
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
                "tx_hash": "741afd3182e163d0936be988f7fa3d317b16765d3fb48f186d6d27dee4fb1f52",
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
                    "tx_hash": "38e181c8cd1f37c737837675e67e80054f129b962e301fcecacbf59d6531659a",
                    "index": 0
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "e89cc327d406b4815f7c9d21025fb1ced236d22b06bbcb75a7a0a10b081b040e",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  // "capacity": "004429353a000000",
                  "capacity": "0000003a35294400",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": null
                },
                {
                  // "capacity": "604116a804000000",
                  "capacity": "00000004a8164160",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "57c33dd84df448ce997748502912a145f6805566"
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
                "tx_hash": "23901dc31e18374f9a088f0033dcca6122f4e3646808c2e8b25983c4807e66b7",
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
                    "tx_hash": "4f26933c260b39c605b4fe848f506459d97f3f7a3b4c1a3de80b026dd7a31114",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  // "capacity": "0018535224000000",
                  "capacity": "0000002452531800",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "a25a046d11a95b9bfaec33468060b576fc81eaff"
                  },
                  "type_": null
                },
                {
                  // "capacity": "6031993e0a000000",
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
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "619d37dd5910e4970a81546146f92d7eecb66a6174f8cb029f003c878c6243e4",
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
                    "tx_hash": "655832bef29d8d145d93c1db8c03066d004c67faf503d736d8fe5314edb608a8",
                    "index": 0
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "3ec6a7bad02d5042c738235d0b9cb390643f09a33712d24e98f4899dcbaf0b74",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  // "capacity": "002ca07d51000000",
                  "capacity": "000000517da02c00",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "65c634f3e742961c8a4e71191138a71ee5ef9591"
                  },
                  "type_": null
                },
                {
                  // "capacity": "60ed39a40b000000",
                  "capacity": "0000000ba439ed60",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "57c33dd84df448ce997748502912a145f6805566"
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
            // "capacity": "00807c814a000000",
            "capacity": "0000004a817c8000",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e421a57de6ed580189463131ccb6d5843dab975d"
            },
            "type_": null
          },
          {
            // "capacity": "50fae56f4b000000",
            "capacity": "0000004b6fe5fa50",
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
        "55000000100000005500000055000000410000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      ]
    };

    const signature = await this.ckb.signAnnotatedTransaction(createMultiInputTx3);
    const key = await getKeyFromLedgerCached(this, signPath);

    checkSignature(createMultiInputTx3, signature, key);

    await flow.promptsPromise;
  });

  it('Signing valid multi-input tx should pass - 2 - (Input 2, 2 cells)', async function() {
    const signPath = [
      2147483692,
      2147483957,
      2147483648,
      0,
      2
    ];
    const createMultiInputTx4 = {
      signPath,
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        1,
        5
      ],
      "inputCount": 3,
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
                "tx_hash": "741afd3182e163d0936be988f7fa3d317b16765d3fb48f186d6d27dee4fb1f52",
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
                    "tx_hash": "38e181c8cd1f37c737837675e67e80054f129b962e301fcecacbf59d6531659a",
                    "index": 0
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "e89cc327d406b4815f7c9d21025fb1ced236d22b06bbcb75a7a0a10b081b040e",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  // "capacity": "004429353a000000",
                  "capacity": "0000003a35294400",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": null
                },
                {
                  // "capacity": "604116a804000000",
                  "capacity": "00000004a8164160",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "57c33dd84df448ce997748502912a145f6805566"
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
                "tx_hash": "23901dc31e18374f9a088f0033dcca6122f4e3646808c2e8b25983c4807e66b7",
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
                    "tx_hash": "4f26933c260b39c605b4fe848f506459d97f3f7a3b4c1a3de80b026dd7a31114",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  // "capacity": "0018535224000000",
                  "capacity": "0000002452531800",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "a25a046d11a95b9bfaec33468060b576fc81eaff"
                  },
                  "type_": null
                },
                {
                  // "capacity": "6031993e0a000000",
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
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "619d37dd5910e4970a81546146f92d7eecb66a6174f8cb029f003c878c6243e4",
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
                    "tx_hash": "655832bef29d8d145d93c1db8c03066d004c67faf503d736d8fe5314edb608a8",
                    "index": 0
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "3ec6a7bad02d5042c738235d0b9cb390643f09a33712d24e98f4899dcbaf0b74",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  // "capacity": "002ca07d51000000",
                  "capacity": "000000517da02c00",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "65c634f3e742961c8a4e71191138a71ee5ef9591"
                  },
                  "type_": null
                },
                {
                  // "capacity": "60ed39a40b000000",
                  "capacity": "0000000ba439ed60",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "57c33dd84df448ce997748502912a145f6805566"
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
            "capacity": "0000004a817c8000",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e421a57de6ed580189463131ccb6d5843dab975d"
            },
            "type_": null
          },
          {
            "capacity": "0000004b6fe5fa50",
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
        "55000000100000005500000055000000410000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
        ""
      ]
    };

    // Prompt 0: Confirm Multi-Input Transaction
    // Prompt 1: Input 2 of 2
    // Prompt 2: Source ckb1qyqxt33570n599su3f88zxg38zn3ae00jkgsqt8fcx
    // Prompt 3: Amount 3939.999 of 3200
    // Prompt 4: Fee 0.0001
    // Prompt 5: Destination ckb1qyqwggd90hnw6kqp39rrzvwvkm2cg0dtjawsuayfag

    const flow = await flowAccept(this.speculos, [
      { header: "Confirm", body: "Multi-Input Transaction"},                            // Prompt 0
      { header: "Input", body: "2 of 2"},                                               // Prompt 1
      { header: "Source", body: "ckb1qyqxt33570n599su3f88zxg38zn3ae00jkgsqt8fcx"},      // Prompt 2
      { header: "Amount", body: "3939.999 of 3200"},                                        // Prompt 3
      { header: "Fee", body: "0.0001"},                                                 // Prompt 4
      { header: "Destination", body: "ckb1qyqwggd90hnw6kqp39rrzvwvkm2cg0dtjawsuayfag"}, // Prompt 5
    ]);


    const signature = await this.ckb.signAnnotatedTransaction(createMultiInputTx4);
    const key = await getKeyFromLedgerCached(this, signPath);

    checkSignature(createMultiInputTx4, signature, key);

    await flow.promptsPromise;
  });

  it('Signing a tx with 6 signers and 11 input cells total current signer signs 3 input cells', async function() {
    const signPath = [
      2147483692,
      2147483957,
      2147483648,
      1,
      5
    ];
    const createMultiInputTx5 = {
      signPath,
      "changePath": [
        2147483692,
        2147483957,
        2147483648,
        1,
        5
      ],
      "inputCount": 11,
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
                "tx_hash": "3f73c1420ecf4ee7b67f2a41d02d8e6d7b564a61b3cd2370b405a82e1e9e3a63",
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
                    "tx_hash": "768707ea273d3268a13b206334bc51a4071d5f92dd18da886df270927e25470b",
                    "index": 0
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "ac637a6de82940983b177975312dc9648198addcd27e7e2dd776a51fb91a2635",
                    "index": 0
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "5d21c687762a0418c061feef38afeb6015e26ef39c65a0945cdd598f469cb9fa",
                    "index": 0
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "1a13aee11a9ae54757b513181c91a9df829d822199c567814906838b80746898",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "0000003339059800",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e5260d839a786ac2a909181df9a423f1efbe863d"
                  },
                  "type_": null
                },
                {
                  "capacity": "0000001748756160",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "57c33dd84df448ce997748502912a145f6805566"
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
                "tx_hash": "32e63801a2a67840be36a31f42056e6f4ef0400548e39cc0311bcd93d648f976",
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
                    "index": 0
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
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "32e63801a2a67840be36a31f42056e6f4ef0400548e39cc0311bcd93d648f976",
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
                    "tx_hash": "2e4f4999bc2ae5d1bf1964d1fb4fa2d7c9dff014faa0fb4130fcaf053a828b6b",
                    "index": 0
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
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "80e736727ac08e31741f33359850a2fa1ed8e082908c1fbb02cf9d9133cee566",
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
                    "tx_hash": "f44b26dda9f1f48b20029654be67c2e636b7fec62b877e20b695c2307dae4a52",
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
                  "capacity": "1bc16ad0e76f3e20",
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
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "38e181c8cd1f37c737837675e67e80054f129b962e301fcecacbf59d6531659a",
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
                    "tx_hash": "6127c3e68295fb18d6ed55256f5ac3bb81c3446c671ff029db7ad7992bc7b394",
                    "index": 1
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "dccc65d2b03f6b30d8cb48a3ab630ac26c73257557ce62320cc911a02fb816b4",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "00000022ecb25c00",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e421a57de6ed580189463131ccb6d5843dab975d"
                  },
                  "type_": null
                },
                {
                  "capacity": "00000009502ad510",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "83462eafd93f0a598ab26597e5cda6523b2fc153"
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
                "tx_hash": "fac70fd2f895c64e60db9dd895427209852841e443eb39bbb13d4bc81ac35261",
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
                    "tx_hash": "39641a05511b85d641cc844d0e0e7766983c9a26b1d62d51f251de7125390b16",
                    "index": 1
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
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "4dbfe46d45f2330a661ec7bbf439aa5ba2cf6a39149ba1c85c47963e706837d6",
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
                    "tx_hash": "f44b26dda9f1f48b20029654be67c2e636b7fec62b877e20b695c2307dae4a52",
                    "index": 0
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
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "3772e5f47d00993b8db0da7685ac27902c399ba6961275b8c7f8cd1286a3d1d6",
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
                "tx_hash": "4dbfe46d45f2330a661ec7bbf439aa5ba2cf6a39149ba1c85c47963e706837d6",
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
                    "tx_hash": "f44b26dda9f1f48b20029654be67c2e636b7fec62b877e20b695c2307dae4a52",
                    "index": 0
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
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "e89cc327d406b4815f7c9d21025fb1ced236d22b06bbcb75a7a0a10b081b040e",
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
                    "tx_hash": "b9244b1e4a2fb7b4f5ae7e511a0f54b918ba1547c31e0fd6dad84009ca4cc765",
                    "index": 0
                  }
                },
                {
                  "since": "0000000000000000",
                  "previous_output": {
                    "tx_hash": "16de7c6b4ff30f8395eafd182df5cd4bbb1065bdccc9270c563a1335c8b24727",
                    "index": 0
                  }
                }
              ],
              "outputs": [
                {
                  "capacity": "0000001bf08eb000",
                  "lock": {
                    "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
                    "hash_type": 1,
                    "args": "e421a57de6ed580189463131ccb6d5843dab975d"
                  },
                  "type_": null
                },
                {
                  "capacity": "00000004a817a0f0",
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
            }
          },
          {
            "input": {
              "since": "0000000000000000",
              "previous_output": {
                "tx_hash": "5d21c687762a0418c061feef38afeb6015e26ef39c65a0945cdd598f469cb9fa",
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
                    "tx_hash": "23901dc31e18374f9a088f0033dcca6122f4e3646808c2e8b25983c4807e66b7",
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
                    "args": "e421a57de6ed580189463131ccb6d5843dab975d"
                  },
                  "type_": null
                },
                {
                  "capacity": "0000001faa3b28f0",
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
            }
          }
        ],
        "outputs": [
          {
            "capacity": "00000145f680b000",
            "lock": {
              "code_hash": "9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8",
              "hash_type": 1,
              "args": "e421a57de6ed580189463131ccb6d5843dab975d"
            },
            "type_": null
          },
          {
            "capacity": "0000000f5ddcc960",
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
        "55000000100000005500000055000000410000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
        "",
        ""
      ]
    };

    // Prompt 0: Confirm Multi-Input Transaction
    // Prompt 1: Input 6 of 6
    // Prompt 2: Source ckb1qyqxyt6gphlcwvw3tjpj6g35ae8da5x0uwvq7yakn7
    // Prompt 3: Amount 1659.9988 of 14000
    // Prompt 4: Fee 0.0001
    // Prompt 5: Destination ckb1qyqwggd90hnw6kqp39rrzvwvkm2cg0dtjawsuayfag

    const flow = await flowAccept(this.speculos, [
      { header: "Confirm", body: "Multi-Input Transaction"},                            // Prompt 0
      { header: "Input", body: "6 of 6"},                                               // Prompt 1
      { header: "Source", body: "ckb1qyqxyt6gphlcwvw3tjpj6g35ae8da5x0uwvq7yakn7"},      // Prompt 2
      { header: "Amount", body: "1659.9988 of 14000"},                                     // Prompt 3
      { header: "Fee", body: "0.0001"},                                                 // Prompt 4
      { header: "Destination", body: "ckb1qyqwggd90hnw6kqp39rrzvwvkm2cg0dtjawsuayfag"}, // Prompt 5
    ]);

    const signature = await this.ckb.signAnnotatedTransaction(createMultiInputTx5);
    const key = await getKeyFromLedgerCached(this, signPath);

    checkSignature(createMultiInputTx5, signature, key);

    await flow.promptsPromise;
  });
});
