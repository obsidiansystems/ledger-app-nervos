# Local AVA testnets

This directory contains scripts to initialize a local testnet, currently consisting of starting a four-node testnet without staking, creating some accounts in the Gecko keystore of one of those nodes, and initializing some addresses with AVA tokens.

## Usage

```
$ cd ledger-app-avax/testnet
$ path/to/avash

avash> runscript testnet.lua
avash> runscript accounts.lua
```

Then, one can verify that things worked by switching to another terminal, and running

```
$ bash get_balances.sh
```

## Misc Notes

There's a poorly documented feature that, at least with Gecko JSONRPC calls, you can use `"AVA"` as an `assetID`.  I don't think this is allowed inside the TX format proper, though.

You have to `cd` into the `testnet` directory so that `accounts.lua` can find `json.lua`.  It would be nice to fix this at some point.

Not sure why `test2` isn't getting funded... the rpc call params look right, next I need to figure out how to get useful results of RPC calls out of avash.
