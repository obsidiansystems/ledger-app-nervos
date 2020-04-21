# Overview

This repository contains the Nervos BOLOS application for the Ledger Nano S and Ledger Nano X and tools for testing the application. While this app is currently under development, we do not recommend using it with mainnet CKB. 

This application has been developed against our forks of [CKB-CLI](https://github.com/obsidiansystems/ckb-cli) and [CKB](https://github.com/obsidiansystems/ckb). Most instructions assume you have the [Nix](https://nixos.org/nix/) Package Manager, which you can install on any Linux distribution or MacOS. Application and wallet developers who would like to support Ledger can do so with [LedgerJS](https://github.com/obsidiansystems/ledgerjs/tree/nervos).

# Installation on the Nano S #

To install from the source directory, ensure that you have nix installed, run

```
$ ./nix/install.sh s
```

and accept the prompts on your ledger device.

# Development #

## Developing on the Ledger Nano S ##

### Incremental Build ###

``` sh
$ nix-shell -A wallet.s --run 'make SHELL=sh all'
```

### Load ###

``` sh
$ nix-shell -A wallet.s --run 'make SHELL=sh load'
```

## Developing on the Ledger Nano X ##

### Incremental Build ###

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
also necessary. This enables end-to-end testing of the ckb. Set up the client
according to the directions in [README.md](README.md).
