# Using the Nervos Devnet

Using a Nervos devnet is recommended when doing testing, development, or experimentation with the Nervos Ledger app. This instructions build upon [Nervos' dev chain docs](https://docs.nervos.org/docs/basics/guides/devchain) and explain how to use the devnet directly from this repository.

## Getting Set Up

Before starting the devnet, we may want to create an account to either 1) receive an allocation of CKB in the genesis block or 2) receive block rewards for mining. If you'd like to use the account stored on your ledger device, see [Account Import](README.md#account-import) instructions. To use a software-based account, use the `account new` command. You will have to set a password to protect your key:

```
ckb-cli account new

Your new account is locked with a password. Please give a password. Do not forget this password.
Password: 
Repeat password: 
address:
  mainnet: ckb1qyqyrm8w0w8uq7puwhdp7s6xqzdjuknhf2tqzdztph
  testnet: ckt1qyqyrm8w0w8uq7puwhdp7s6xqzdjuknhf2tqlgu5dt
lock_arg: 0x41ecee7b8fc0783c75da1f4346009b2e5a774a96
lock_hash: 0xeb31c5232b322905b9d52350c0d0cf55987f676d86704146ce67d92ddef05ed3
```

You can now use the `lock-arg` of this account when configuring your devnet.

## Creating and Configuring a Local Devnet

From within this repository, we will first make [CKB](https://github.com/nervosnetwork/ckb) available then proceed with initializing the dev chain. Do this in a separate, dedicated terminal:

``` sh
$ nix run -f nix/dep/ckb # make ckb available
$ mkdir devnet # create a folder for the devnet
$ cd devnet # enter the devnet folder
$ ckb init --chain dev --ba-message 0x --ba-arg <your-lock-arg> # initialize the devnet
```

`ckb init ...` creates the files we'll use to configure our network for development.

### Modifications: ckb-miner.toml

The `value` in this file determines the mining interval in milliseconds. The default value is 5000, which means a new block can be created every 5 seconds. We recommend adjusting this to a smaller value such as 20 so epochs are quick.

### Modifications: specs/dev.toml

In this file we will make two adjustments to make time move faster in our devnet.
1. Reduce the default epoch length (`genesis_epoch_length`). The default value is `1000` blocks. We recommend setting this to `1` for testing purposes.
2. Skip difficulty adjustments to keep epoch lengths constant (`permanent_difficulty_in_dummy`). Setting this to `true` keeps difficulty constant.

When you are done with your modifications, this portion of the `[params]` section should look like this:

```toml
genesis_epoch_length = 1 # Make sure this line is uncommented!
# For development and testing purposes only.
# Keep difficulty be permanent if the pow is Dummy. (default: false)
permanent_difficulty_in_dummy = true # Make sure this line is uncommented by removing the '#' from the beginning of the line
```

This file also allocates CKB in the genesis block to a few addresses. We recommend allocating CKB to one of your accounts for testing purposes:

```toml
[[genesis.issued_cells]]
capacity = 20_000_000_000_00000000
lock.code_hash = "0x9bd7e06f3ecf4be0f2fcd2188b23f1b9fcc88e5d4b65a8637b17723bbda3cce8"
lock.args = "<your-lock-arg-here>" # set this value to your lock-arg to receive CKB in the genesis block
lock.hash_type = "type"
```

## Starting a Local Devnet

Once you have set all the configurations of your devnet, we can start the node and the miner. As a best practice, we recommend keeping the node running at all times, but only running the miner when you want blocks to be produced. This gives you maximum control over time and helps avoid the issue where the devnet stops at block 59594 because the miner has hit the maximum number of deposited cells.

To start the node and the miner, run the following commands:

```
$ ckb run &
$ ckb miner
```

After entering that command, you will need to press 'Enter' to intiate the miner. The miner can be suspended with 'Ctrl + Z', then resumed by typing `fg` followed by 'Enter'.

# Troubleshooting

## Devnet Stops at 59594 Blocks ##

At some point, you hit an issue where the node can’t hold the capacity of the miner. This can be resolved by, clearing your devnet and restarting like so:

``` sh
CTRL-C
$ rm -rf data/
$ ckb run &
$ ckb miner
```

## Invalid Cell Status ##

This can happen when you have switched networks between running
`ckb-cli`. If this is the case, it can be fixed by clearing your cache.
This can be done on the command line.

First, quit out of ckb-cli so that we can modify our index by typing
‘quit’. Then, clear your cache with:

``` sh
$ rm -rf $HOME/.ckb-cli/index-v1/
```
