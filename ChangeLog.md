# Revision history for `ledger-app-ckb`

This project's release branch is `master`.
This log is written from the perspective of the release branch: when changes hit `master`, they are considered released.

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
