# Running the Testnet (Aggron) #

You can connect to the Nervos testnet Aggron with CKB directly from this repository using Nix. These instructions largely follow [Nervos' docs for running a CKB Testnet Node](https://docs.nervos.org/docs/basics/guides/testnet). Note that you will need to get CKB from the [Aggron Faucet](https://docs.nervos.org/docs/basics/tools#nervos-aggron-faucet). For development and testing we recommend [using a devnet](devnet.md) because you will will have to wait the ~30 day period for doing a DAO withdrawal.

From within this repository, start by pulling `CKB` into your shell:

``` sh
$ nix run -f ./nix/dep/ckb
```

Then create and enter a testnet directory:

```
$ mkdir -p testnet
$ cd testnet/
```

Next, intialize the testnet chain using the most recent spec:

``` sh
$ ckb init --chain testnet
```

You can then run the node with:

```
$ ckb run
```

Leave this open in a separate terminal. CKB-CLI will then use this node to communicate with the testnet.
