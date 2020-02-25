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

No automated tests exist yet. Here is how to set up an environment to
test the Ledger app on Nano S using the ckb command line:

Load the latest version of th Nervos app:

``` sh
$ nix-shell -A wallet.s --run 'make SHELL=sh load'
```

Build a version of the ckb-cli:

``` sh
$ nix-shell -p '(import ./nix/dep/ckb-cli {})'
```

Get the public key:

``` sh
$ ckb-cli account extended-address-ledger --path "m/44'/309'/0'/0/0"
```

This should show up on the ledger as “Provide Public Key” for
44’/309’/0’/0/0. Accept it on the Ledger and verify ckb prints the
resulting address. The result should lookk like:

``` text
account_source: on-disk password-protected key store
address:
  mainnet: ckb1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsqltkvh
  testnet: ckt1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsa64fqt
lock_arg: 0x632c444199275d00b7c1fb65bf001d70bc609599
```
