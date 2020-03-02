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

Remember the ledger_id given above for the latest commands.

### Get Public Key ###

Get the public key:

``` sh
$ ckb-cli account extended-address --path "m/44'/309'/0'/1/0" --account-id 0x69c46b6dd072a2693378ef4f5f35dcd82f826dc1fdcc891255db5870f54b06e6
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

The “testnet” address is the one you need to save. Keep it for later.

### Transfering ###

#### Starting a node ####

Clone and run ckb:

``` sh
$ git clone https://github.com/obsidiansystems/ckb.git
$ cd ckb/
```

Next, you must setup the test network following this guide:
https://docs.nervos.org/dev-guide/testnet.html

Edit ckb.toml in ckb/, changing:

```
spec = { bundled = "specs/mainnet.toml" }
```

to

```
spec = { bundled = "specs/testnet.toml" }
```

Run the node with:

```
$ nix-shell --run 'cargo run run'
```

Leave this open in a separate terminal as you continue on the next steps.

#### Getting CKB from the faucet ####

Visit https://faucet.nervos.org/auth and follow the process to receive
testnet tokens. You should use the address generated above in “Get
Public Key” and the capacity should be 100000.

#### Verify address balance ####

To continue, you need at least 100 CKB in your wallet. Do this with:

``` sh
$ ckb-cli wallet get-capacity --address
ckt1qyqqmpgzhw94kdj6d8jwtmt6xgumjyr0negq3d2345
total: 100.0 (CKB)
```

If the number is less than 100, you need to somehow get the coins some
other way.

#### Transfer ####

Transfer operation (use correct --from-account and
derive-change-address value from “List Ledger Wallets” and “Get Public
Key” above).

``` sh
$ ckb-cli wallet transfer --from-account
0x27fe5acb022cd7b8be0eb7009d42ff4600c597d28b6affefcab6f7396d1311c2
--to-address ckb1qyqqmpgzhw94kdj6d8jwtmt6xgumjyr0negqvg5weg --capacity
100 --tx-fee 0.00001 --derive-change-address ckb1qyqqmpgzhw94kdj6d8jwtmt6xgumjyr0negqvg5weg --derive-receiving-address-length 0 --derive-change-address-length 1
```
