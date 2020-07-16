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

Next, get aggron.toml and intialize the testnet chain using the spec from aggron.toml:

``` sh
$ curl -o aggron.toml https://gist.githubusercontent.com/doitian/573513c345165c0fe4f3504ebc1c8f9f/raw/3032bed68550e0a50e91df2c706481e80b579c70/aggron.toml
$ ckb init --import-spec ./aggron.toml --chain testnet
```

You can then run the node with:

```
$ ckb run
```

Leave this open in a separate terminal. CKB-CLI will then use this node to communicate with the testnet.
