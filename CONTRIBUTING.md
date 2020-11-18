# Overview

This repository contains the Nervos BOLOS application for the Ledger Nano S and Ledger Nano X and tools for testing the application. While this app is currently under development, we do not recommend using it with mainnet CKB. 

This application has been developed against our forks of [CKB-CLI](https://github.com/obsidiansystems/ckb-cli) and [CKB](https://github.com/obsidiansystems/ckb). Most instructions assume you have the [Nix](https://nixos.org/nix/) Package Manager, which you can install on any Linux distribution or MacOS. Application and wallet developers who would like to support Ledger can do so with [LedgerJS](https://github.com/obsidiansystems/ledgerjs/tree/nervos).

# Installation on the Nano S #

To install from the source directory, ensure that you have nix installed, run

```
$ ./nix/install.sh -t s
```

and accept the prompts on your ledger device.

# Development #

## Developing on the Ledger Nano S ##

### Incremental Build ###

``` sh
$ nix-shell -A wallet.s --run 'make all'
```

### Load ###

``` sh
$ nix-shell -A wallet.s --run 'make load'
```

## Developing on the Ledger Nano X ##

### Incremental Build ###

``` sh
$ nix-shell -A wallet.x --run 'make all'
```

### Load ###

Ledger Nano X does not currently have a way to load unsigned apps.
Testing need to be done through Ledger Live.

## Clearing Keystore Data from CKB-CLI ##

Ledger key information is stored locally in `~/.ckb-cli/ledger-keystore/` and software key data is stored in `~/.ckb-cli/keystore/`. If you need to delete either, you can do so by running `rm -rf ~/.ckb-cli/ledger-keystore/` or `rm -rf ~/.ckb-cli/keystore/`, respectively.

# Testing #


## Automated Testing ##

You can run automated tests through speculos via the `Makefile`. Just run this:

``` sh
$ nix-shell -A wallet.s --run 'make SHELL=sh test'
```

Running the same test suite on a live ledger currently requires that the ledger be configured with
the recovery phrase

```
glory promote mansion idle axis finger extra february uncover one trip resource
lawn turtle enact monster seven myth punch hobby comfort wild raise skin
```

and then you can (with the ledger connected) run:

```
nix-shell -A wallet.s --run 'make && LEDGER_LIVE_HARDWARE=1 make test'
```

You must manually accept each prompt on the device. Also as the test suite currently does not test
any rejections you must accept every prompt that happens.

## Manual Testing ##

Not everything is coverd by the automated tests, so manual testing is
also necessary. This enables end-to-end testing of the ckb. Set up the client
according to the directions in [README.md](README.md).

# Releasing #

To create a new release candidate:

* Choose a commit you want to release.
* Ensure it is part of a branch `release/M.N`
* Ensure `ChangeLog.md` describe the release
* Make a new commit on top of it, incrementing the version numbers in the Makefile appropriately.
* Tag the release candidate `vM.N.P-rcX`, where M, N, P, and X stand for appropriate numbers, e.g.: v0.4.1-rc2
* Make the actual tarballs with `app.hex` files using the provided script:
  ``` sh
  $ nix/release.sh
  ```

Once it has passed QA:

* Tag `vM.N.P`, e.g. v0.4.1
* Ensure `ChangeLog.md` has the right date
* Use the md5sum/sha256 section of the output of `nix/release.sh` as part of the markdown release notes on GitHub, and post the resulting two tarballs along with the release.
