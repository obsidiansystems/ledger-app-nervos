# Revision history for `ledger-app-ckb`

This project's release branch is `master`.
This log is written from the perspective of the release branch: when changes hit `master`, they are considered released.

## v0.5.2 - 2020-11-18

### Release Notes

This release is primary focused on dev workflow changes that are not user visible.
However, we did make one quite noticeable change:

- Updated app icons

This release has been tested with:
- CKB-CLI: [babc52ae593474f7fddb80f7c229374203310d6a](https://github.com/nervosnetwork/ckb-cli/tree/ledger-support)
- CKB-Plugin-Ledger: [v0.2.1 - f03d7f3a2d696d3a48b8be78211452b5679165cb](https://github.com/obsidiansystems/ckb-plugin-ledger/releases/tag/v0.2.1)
- hw-app-ckb: [v0.1.2 - d348841af4e2a023f760356e98059a45b1d6d6b7](https://github.com/obsidiansystems/hw-app-ckb/releases/tag/v0.1.2)

## v0.5.1 - 2020-11-18

### Release Notes

This release is primarily focused on bugfixes and improving the documentation.

- Updated installation instructions and references to LedgerJS in the README.
- Fixed DAO deposit prompt.
- Fixed ordering of UI prompt assignment.
- Fixed `release.sh` tag matching.
- Fixed signature corruption due to improper variable initialization.
- Simplified instructions for initializing a devnet.
- Fixed wording of version test.
- Overhauled test-suite: replaced BATS with MochaJS

This release has been tested with:
- CKB-CLI: [babc52ae593474f7fddb80f7c229374203310d6a](https://github.com/nervosnetwork/ckb-cli/tree/ledger-support)
- CKB-Plugin-Ledger: [v0.2.1 - f03d7f3a2d696d3a48b8be78211452b5679165cb](https://github.com/obsidiansystems/ckb-plugin-ledger/releases/tag/v0.2.1)
- hw-app-ckb: [v0.1.2 - d348841af4e2a023f760356e98059a45b1d6d6b7](https://github.com/obsidiansystems/hw-app-ckb/releases/tag/v0.1.2)

## v0.5.0 - 2020-09-30

### Release Notes
- Allow signing of transactions with custom scripts when the 'allow transactions with scripts' in-app configuration is enabled.
  This also provides a framework for supporting popular scripts in future releases.
- Fixed a bug with U2F communication.
- Fixed a bug with 'dao prepare' operations which sometimes prevented them from being signed.
- Simplified our notation of "self-transfers". There are no longer errors from transactions improperly identified as self-transfers, and only the change path, and not source address, is used to identifier a self transfer.

### Known Issues
- Transfers with a multisig input to multiple outputs where at least one output is a long multisig address will result in incorrect display of the standard secp256k1 sighash addresses. In particular, the 8th character and the last 6 characters are incorrect.
- Transfers with a multisig input to multiple outputs display the first user prompt as an output. If you navigate back to the first prompt, you'll then correctly see 'Confirm Transaction'. All information shown in the UI is correct and the signed operation is valid, but the order is temporarily incorrect.
- Multi-input operations where at least one input is a multisig address are not yet supported

This release has been tested with:
- CKB-CLI: [b460c998d6681a89a47b3af203ecc5f12d7b2507](https://github.com/obsidiansystems/ckb-cli/commit/b460c998d6681a89a47b3af203ecc5f12d7b2507)
- CKB-Plugin-Ledger: [v0.2.0 - 283065c4d13212586a0dca709540277988043395](https://github.com/obsidiansystems/ckb-plugin-ledger/commit/283065c4d13212586a0dca709540277988043395)
- hw-app-ckb: [v0.1.0 / 9a6754bdec67aea44a6d70d62cb8c8766ae49ccc](https://github.com/obsidiansystems/hw-app-ckb/commit/9a6754bdec67aea44a6d70d62cb8c8766ae49ccc)

## v0.4.0 - 2020-08-07

### Release Notes
- Increase number of supported output cells from 3 to 5
- Memory savings for more stability
- Improve recognition of self-transfers

## v0.3.0 - 2020-07-24

### Release Notes

NOTE: This release has been tested with git hash `8b7bbcb437bdb99a91f19c836cd5ef60667e2718` on the `ledger-app` branch of https://github.com/obsidiansystems/ckb-cli, which is intended for testing and development purposes only. This repository comes with scripts that run this specialized fork of CBK-CLI for the user. **Do not use this release or [our fork of CKB-CLI](https://github.com/obsidiansystems/ckb-cli) in production.**

v0.3.0 supports a much broader range of transaction display and signing capabilities. Specifically, new features since the last release (v0.1.0):
* Verifying and signing transactions with multiple different input lock arg, displaying which input we are currently signing for.
* Updated prompts for self-transfers and transactions to consolidate tokens within an account.
* Verifying and signing transactions with multiple outputs (currently maximum of 3), displaying destination lock args for all outputs.
* Updated prompts for verifying and signing DAO deposit, prepare, and withdraw operations.
* Signing arbitrary messages
  * Securely verified in text or hex form.
  * As an advanced opt-in feature, verifying and signing hashes of arbitrary data.
* Verifying and signing transactions with multisig and timelock inputs or outputs
* New FLOW user interface

Additional Notes:
* Nano S SDK (1.6.0 Unified)
* Nano X SDK (1.5)

## v0.1.0 2020-04-03

Initial release for Nervos CKB Ledger app
