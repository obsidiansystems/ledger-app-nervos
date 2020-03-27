# Overview

This repository contains the Nervos BOLOS application for the Ledger Nano S and Ledger Nano X and tools for testing the application. While this app is currently under development, we do not recommend using it with mainnet CKB. 

This application has been developed against our forks of [CKB-CLI](https://github.com/obsidiansystems/ckb-cli) and [CKB](https://github.com/obsidiansystems/ckb). Most instructions assume you have the [Nix](https://nixos.org/nix/) Package Manager, which you can install on any Linux distribution or MacOS.

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

## Prepare machine to talk to ledger.

On Linux, the "udev" rules must be set up to allow your user to communicate with the ledger device.
On NixOS, one can easily do this with:
``` nix
{
  # ...
  hardware.ledger.enable = true;
  # ...
}
```

### Prepare ledger and client

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
$ nix run -f nix/dep/ckb-cli -c ckb-cli
```

All commands that follow prefixed with ‘CKB>’ should be run in the
prompt provided by the above command.

Now, make sure the Ledger is:

- connected
- unlocked
- has the “Nervos” app open (shows “Use wallet to view accounts”)

### List Ledger Wallets ###

List the wallets:

``` sh
CKB> account list
- "#": 0
  account_source: ledger hardware wallet
  ledger_id: 0x69c46b6dd072a2693378ef4f5f35dcd82f826dc1fdcc891255db5870f54b06e6
```

Remember the ledger\_id given above for the latest commands. It will be
used for ```<ledger-id>``` later on.

### Get Public Key ###

Get the public key:

``` sh
CKB> account extended-address --path "m/44'/309'/0'/1/0" --account-id <ledger-id>
```

This should show up on the ledger as (in 4 screens):

``` text
Provide 
Public Key
```
``` text
Derivation Path
44’/309’/0’/1/0.
```
``` text
Mainnet Address:
ckb1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsqltkvh
```
``` text
Testnet Address:
ckt1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsa64fqt
```

Accept it on the Ledger and verify ckb prints the
resulting address. The result should look like:

``` text
account_source: on-disk password-protected key store
address:
  mainnet: ckb1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsqltkvh
  testnet: ckt1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsa64fqt
lock_arg: 0x632c444199275d00b7c1fb65bf001d70bc609599
```

Make sure the two addresses match.  The “testnet” address is the one you
need to save. Keep it for later, as it will be used for
`<ledger-address>`.

### Transfering ###

#### Creating a new account for local dev network ####

Now, you must also create an account from ckb-cli so that an account
can get the issued cells. This can be done with:

```
CKB> account new
```

Follow the prompts and choose a password to continue. Next, get the
lock_arg for your account with:

```
CKB> account list
- "#": 0
  account_source: on-disk password-protected key store
  address:
    mainnet: ckb1qyq2htkmhdkcmcwc44xsxc3hcg7gytuyapcqpwltnt
    testnet: ckt1qyq2htkmhdkcmcwc44xsxc3hcg7gytuyapcqutp5lh
  lock_arg: 0xabaedbbb6d8de1d8ad4d036237c23c822f84e870
  lock_hash: 0x21ad4eee4fb0f079f5f8166f42f0d4fbd1fd63b8f36a7ccfb9d3657f9a5aed43
- "#": 1
  account_source: ledger hardware wallet
  ledger_id: 0x27fe5acb022cd7b8be0eb7009d42ff4600c597d28b6affefcab6f7396d1311c2
```

The value of “lock_arg: ...” is your-new-account-lock-arg.

#### Starting a local dev network ####

First, make a directory and init it for a dev network:
``` sh
$ nix run -f nix/dep/ckb # to make ckb available
$ mkdir devnet
$ cd devnet
$ ckb init --chain dev
```

then modify the value at the end of ckb-miner.toml to be small:

```
value = 20
```

and also add this block to the end of ckb.toml:

```
[block_assembler]
code_hash = "0x9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8"
args = "0xb57dd485a1b0c0a57c377e896a1a924d7ed02ab9"
hash_type = "type"
message = "0x"
```

finally, in specs/dev.toml, set genesis\_epoch\_length to 1 and
uncomment permanent\_difficulty\_in\_dummy:

```
genesis_epoch_length = 1
# For development and testing purposes only.
# Keep difficulty be permanent if the pow is Dummy. (default: false)
permanent_difficulty_in_dummy = true
```

and also pick one of the genesis issuance cells and set args to a lock
arg from password-protected account above:

```
[[genesis.issued_cells]]
capacity = 20_000_000_000_00000000
lock.code_hash = "0x9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8"
lock.args = "<your-new-account-lock-arg>"
lock.hash_type = "type"
```

Then you can run 

```
ckb run &
ckb miner
```

to start up the node and miner; the ledger account you added to
genesis.issued\_cells should have a large quantity of CKB to spend
testing.

Note that you may have to suspend your miner to avoid hitting the
maximum amount of deposited cells. You can do this with Ctrl-Z. Type
‘fg’ followed by enter to continue running it later. The miner should
be running when running any ckb-cli commands.

#### Getting CKB from the miner ####

The dev network is set up to send CKBs to the on-disk account. To get
money onto the Ledger, you need to do a transfer. This can be done
with an initial transfer:

```
CKB> wallet transfer --from-account <your-new-account-lock-arg> --to-address <ledger-address> --capacity 1000 --tx-fee 0.001
```

#### Verify address balance ####

To continue, you need at least 100 CKB in your wallet. Do this with:

``` sh
CKB> wallet get-capacity --address <ledger-address>
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
CKB> wallet transfer \
    --from-account <ledger-id> \
    --to-address <ledger-address> \
    --capacity 102 --tx-fee 0.00001 \
    --derive-change-address <ledger-address> \
    --derive-receiving-address-length 0 \
    --derive-change-address-length 1
```

##### Get live cells ######

Get live cells:

``` sh
CKB> wallet get-live-cells --address <ledger-address>
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

#### DAO ####

##### Deposit #####

You can deposit to the dao like:

``` sh
CKB> dao deposit \
    --capacity 102 \
    --from-account <ledger-id> \
    --tx-fee 0.00001 \
    --path "m/44'/309'/0'/1/0"
```

##### Get deposited cells #####

Get deposited cells:

``` sh
CKB> dao query-deposited-cells --address <ledger-address>
live_cells:
  - capacity: 10200000000
    data_bytes: 8
    index:
      output_index: 0
      tx_index: 1
    lock_hash: 0xa77a89c29289311b5d6a01f234facc8244cf455909260533e11183054852ff61
    number: 472898
    tx_hash: 0xc55bad328edd74c4be1e630d0eb52733d9ed027f02eaca10f0e78b96a44053fc
    tx_index: 0
    type_hashes:
      - 0x82d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e
      - 0xcc77c4deac05d68ab5b26828f0bf4565a8d73113d7bb7e92b8362b8a74e58e58
total_capacity: 10200000000
```

Remember the values above for one of live cells under “tx\_hash” and “output\_index”. You'll need these when constructing the `prepare` operation below which prepares a cell for withdrawal from the NervosDAO.

##### Prepare #####

Prepare a cell for withdrawal from the NervosDAO:

``` sh
CKB> dao prepare --from-account <ledger-id> --out-point <tx_hash>-<output_index> --tx-fee 0.0001 --path "m/44'/309'/0'/1/0"
```

##### Get prepared cells #####

Get prepared cells:

``` sh
CKB> dao query-prepared-cells --address <ledger-address>
live_cells:
  - capacity: 10500000000
    data_bytes: 8
    index:
      output_index: 0
      tx_index: 1
    lock_hash: 0xa77a89c29289311b5d6a01f234facc8244cf455909260533e11183054852ff61
    maximum_withdraw: 10500154580
    number: 493786
    tx_hash: 0xae91f2a310f2cfeada391e5f76d0addcc56d99c91a39734c292c930a1cfc67c2
    tx_index: 0
    type_hashes:
      - 0x82d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e
      - 0xcc77c4deac05d68ab5b26828f0bf4565a8d73113d7bb7e92b8362b8a74e58e58
total_maximum_withdraw: 10500154580
```

Remember the values above for one of live cells under “tx\_hash” and “output\_index”. You'll need these when constructing the `withdraw` operation below which withdraw's CKB from the NervosDAO.

##### Withdraw #####

Withdraw a prepared cell:

``` sh
CKB> dao withdraw --from-account <ledger-id> --out-point <tx_hash>-<output_index> --tx-fee 0.00001 --path "m/44'/309'/0'/1/0"
```

At this point, either
```
JSON-RPC 2.0 Error: Server error (OutPoint: ImmatureHeader(Byte32(0xd7de1ffd49c71b5dc71fcbf1638bb72c8fb16f8fffdfd5172456a56167fea0a3)))
```
will be reported, showing that the prepared cell is not yet available to withdraw, or a transaction hash if it is.

# Running the testnet #

You can also run the above commands in the testnet instead of a
devnet. This allows you to make transactions that are sent to the
wider test network. Note that this means you will have to wait the 30
day period for doing a DAO withdraw.

Get ckb in your shell:

``` sh
$ nix run -f ./nix/dep/ckb
```

Create a testnet directory

```
$ mkdir -p testnet
$ cd testnet/
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


# Troubleshooting #

## Application Build Failure ##

If you run into issues building the Ledger application using `nix-shell -A wallet.s --run 'make SHELL=sh all'`, we recommend trying `nix-shell -A wallet.s --run 'make SHELL=sh clean all`.

## Devnet stops at 59594 blocks ##

At some point, you hit an issue where the node can’t hold the capacity of the miner. This can be resolved by, clearing your devnet and restarting like so:

``` sh
CTRL-C
$ rm -rf data/
$ ckb run &
$ ckb miner
```

## Invalid cell status ##

This can happen when you have switched networks between running
ckb-cli. If this is the case, it can be fixed by clearing your cache.
This can be done on the command line.

First, quit out of ckb-cli so that we can modify our index by typing
‘quit’. Then, clear your cache with:

``` sh
$ rm -rf $HOME/.ckb-cli/index-v1/
```
