# Revision history for `ledger-app-ckb`

This project's release branch is `master`.
This log is written from the perspective of the release branch: when changes hit `master`, they are considered released.

## v0.5.0 - 2020-??-??

* Allow signing of transactions with custom scripts when the 'allow transactions with scripts' in-app configuration is enabled.
  This also provides a framework for supporting popular scripts in future releases.

* Fixed a bug with U2F communication.

* Fixed a bug with 'dao prepare' operations which sometimes prevented them from being signed.

* Simplified our notation of "self-transfers".
  There are no longer errors from transactions improperly identified as self-transfers, and only the change path, and not source address, is used to identifier a self transfer.
