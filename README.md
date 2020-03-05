# Development #

## Developing on the Ledger Nano S ##

### Build ###

``` sh
$ nix-shell -A wallet.s --run 'make SHELL=sh all'
```

### Load ###

``` sh
$ nix-shell -A wallet.s --run 'make SHELL=sh load'
```

## Developing on the Ledger Nano X ##

### Build ###

``` sh
$ nix-shell -A wallet.x --run 'make SHELL=sh all'
```

### Load ###

Ledger Nano X does not currently have a way to load unsigned apps.
Testing need to be done through Ledger Live.


# Testing #


## Automated Testing ##

You can run automated tests through speculos via the ./test.sh script. Just run
this:

``` sh
$ nix-shell -A wallet.s --run 'make SHELL=sh test'
```

Running the same test suite on a live ledger currently requires that the ledger
be configured with the recovery phrase

```
glory promote mansion idle axis finger extra february uncover one trip resource
lawn turtle enact monster seven myth punch hobby comfort wild raise skin
```

and then you can (with the ledger connected) run

```
nix-shell -A wallet.s --run 'make; ./test.sh -h'
```

The test suite currently does not test any rejections, so just accept every
prompt that happens.

## Manual Testing ##

Not everything is coverd by the automated tests, so manual testing is
also necessary. This enables end-to-end testing of the ckb

Here is how to set up an environment to test the Ledger app on Nano S
using the ckb command line:

Load the latest version of the Nervos app, confirming the unsafe
prompt:

``` sh
$ ./nix/install.sh s
```

If you get a “permission denied” error, you’re computer is not
detecting the Ledger device correctly. Make sure the Ledger is
connected properly and then try it again with sudo: “sudo
./nix-install.sh s”.

You have to accept a few prompts on the Ledger. Then you must select
and load the Nervos app, confirming the unsafe prompt.

Build a version of the ckb-cli:

``` sh
$ nix-shell -p '(import ./nix/dep/ckb-cli {})'
```

Now, make sure the Ledger is:

- connected
- unlocked
- has the “Nervos” app open (shows “Use wallet to view accounts”)

### List Ledger Wallets ###

List the wallets:

``` sh
$ ckb-cli account list
- "#": 0
  account_source: ledger hardware wallet
  ledger_id: 0x69c46b6dd072a2693378ef4f5f35dcd82f826dc1fdcc891255db5870f54b06e6
```

Remember the ledger_id given above for the latest commands. It will be
used for ```<ledger-id>``` later on.

### Get Public Key ###

Get the public key:

``` sh
$ ckb-cli account extended-address --path "m/44'/309'/0'/1/0" --account-id <ledger-id>
```

This should show up on the ledger as “Provide Public Key” for
44’/309’/0’/1/0. Accept it on the Ledger and verify ckb prints the
resulting address. The result should look like:

``` text
account_source: on-disk password-protected key store
address:
  mainnet: ckb1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsqltkvh
  testnet: ckt1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsa64fqt
lock_arg: 0x632c444199275d00b7c1fb65bf001d70bc609599
```

The “testnet” address is the one you need to save. Keep it for later,
as it will be used for ```<ledger-address>```.

### Transfering ###

#### Starting a node (locally) ####

Get ckb in your shell:

``` sh
$ nix-shell -p '(import ./nix/dep/ckb {})'
```

Get aggron.toml:

``` sh
$ curl -o aggron.toml https://gist.githubusercontent.com/doitian/573513c345165c0fe4f3504ebc1c8f9f/raw/3032bed68550e0a50e91df2c706481e80b579c70/aggron.toml
```

Init the testnet toml:

``` sh
$ ckb init --import-spec ./aggron.toml --chain testnet
```

Run the node with:

```
$ ckb run
```

Leave this open in a separate terminal as you continue on the next steps.


#### Starting a node (port forwarding) ####

If you have access to a local node on your network, you can run ssh
port forwarding. This can be done like this:

```
$ ssh -L 8114:<host>:8114 localhost
```

where <host> is the name of your machine running a local node. Note
this is discourage, and running your own node is preferred.

#### Getting CKB from the faucet ####

Visit https://faucet.nervos.org/auth and follow the process to receive
testnet tokens. You should use the address generated above in “Get
Public Key” and the capacity should be 100000.

#### Verify address balance ####

To continue, you need at least 100 CKB in your wallet. Do this with:

``` sh
$ ckb-cli wallet get-capacity --address <ledger-address>
total: 100.0 (CKB)
```

If your node is not synced up, this will take up to a few hours. If
the number is less than 100, you need to somehow get the coins some
other way.

#### Transfer ####

Transfer operation (use correct --from-account and
derive-change-address value from “List Ledger Wallets” and “Get Public
Key” above).

``` sh
$ ckb-cli wallet transfer \
    --from-account <ledger-id> \
    --to-address <ledger-address> \
    --capacity 100 --tx-fee 0.00001 \
    --derive-change-address <ledger-address> \
    --derive-receiving-address-length 0 \
    --derive-change-address-length 1
```

#### DAO ####

##### Deposit #####

You can deposit to the dao like:

``` sh
$ dao deposit \
    --capacity 100 \
    --from-account <ledger-id> \
    --tx-fee 0.00001 \
    --path "m/44'/309'/0'/1/0
```

##### Get live cells ######

Get live cells:

``` sh
$ wallet get-live-cells --address <ledger-address>
current_capacity: 2000.0 (CKB)
current_count: 1
live_cells:
  - capacity: 2000.0 (CKB)
    data_bytes: 0
    index:
      output_index: 0
      tx_index: 2
    lock_hash: 0x8cf5955c203e3bd9c0fa1ceac94206dca01e32a674ba17060e77f8e52750e491
    mature: true
    number: 460368
    tx_hash: 0xe58df9496d58685516291bf3db0ccbdbc30a53a7316639676b7ad98020d13146
    tx_index: 0
    type_hashes: ~
total_capacity: 2000.0 (CKB)
total_count: 1
```

Remember the value above for one of live cells under “tx\_hash” and “tx\_index”.

##### Withdraw #####

Withdraw from DAO:

``` sh
$ dao withdraw
    --from-account <ledger-account> \
    --out-point <tx_hash>-<tx_index> \
    --path "m/44'/309'/0'/1/0" \
    --tx-fee 0.00001
```
