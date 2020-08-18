# Operation Prompts

This document summarizes on-device prompts for each command.

# Table of Contents

- [Account Commands](#account-commands)
     - [Account Import](#account-import)
     - [Account Extended-address](#account-extended-address)
- [Transfers](#transfers)
     - [1 Input Address, 1 Output Address (`wallet transfer`)](#1-input-address-1-output-address-wallet-transfer)
     - [From Multisig Address](#transfers-from-a-multisig-address)
     - [1 Input Address, 2-5 Outputs Addresses (`tx sign-inputs`)](#1-input-address-2-5-outputs-addresses-tx-sign-inputs)
     - [2+ Input Addresses, 1 Output Address (`wallet transfer`)](#2-input-addresses-1-output-address-wallet-transfer)
     - [2+ Input Addresses, 2-5 Output Addresses (`tx sign-inputs`)](#2-input-addresses-2-5-output-addresses-tx-sign-inputs)
- [DAO Commands](#dao-commands)
     - [DAO Deposit](#dao-deposit)
     - [DAO Prepare](#dao-prepare)
     - [DAO Withdraw](#dao-withdraw)
- [Message Signing](#message-signing)
     - [Raw String](#raw-string)
     - [Binary Hex](#binary-hex)
     - [Hashed Data](#hashed-data)

## Account Commands

#### Account Import
`account import --account <account-id>`

|  Prompt 1 	|
|:---------:	|
|  `Import` 	|
| `Account` 	|

#### Account Extended-address
`account extended-address --lock-arg <lock-arg>`

|   Prompt 1   	|       Prompt 2      	|
|:------------:	|:-------------------:	|
|   `Provide`  	|      `Address`      	|
| `Public Key` 	| `<sighash address>` 	|

## Transfers

The prompts for transfers differ based on which command is used and the number of input and output cells there are. The maximum number of outputs (including change) currently supported is 5.

#### 1 Input Address, 1 Output Address (`wallet transfer`)
`wallet transfer --capacity <capacity> --from-account <from-account> --to-address <to-address> --tx-fee <tx-fee>`

|    Prompt 1   	|   Prompt 2   	|  Prompt 3  	|    Prompt 4    	|
|:-------------:	|:------------:	|:----------:	|:--------------:	|
|   `Confirm`   	|   `Amount`   	|    `Fee`   	|  `Destination` 	|
| `Transaction` 	| `<capacity>` 	| `<tx-fee>` 	| `<to-address>` 	|

#### Transfers from a Multisig Address
`tx sign-inputs --from-account <lock-arg> --tx-file <tx-file>`

|    Prompt 1   	|   Prompt 2   	|  Prompt 3  	|    Prompt 4    	|
|:-------------:	|:------------:	|:----------:	|:--------------:	|
|   `Confirm`   	|   `Amount`   	|    `Fee`   	|  `Destination` 	|
| `Transaction` 	| `<capacity>` 	| `<tx-fee>` 	| `<to-address>` 	|

#### 1 Input Address, 2-5 Outputs Addresses (`tx sign-inputs`)
`tx sign-inputs --from-account <lock-arg> --tx-file <tx-file>`

|    Prompt 1   	|   Prompt 2   	|  Prompt 3  	|            Prompt 4            	| Prompt 5-8 (as necessary) 	|
|:-------------:	|:------------:	|:----------:	|:------------------------------:	|:-------------------------:	|
|   `Confirm`   	|   `Amount`   	|    `Fee`   	|     `Output <num>/<total>`     	|     (repeat Prompt 4)     	|
| `Transaction` 	| `<capacity>` 	| `<tx-fee>` 	|  `<capacity> -> <to-address>` 	|     (repeat Prompt 4)     	|

Prompt 4 repeats as necessary for each of the operation's outputs. Examples of the top and bottom line are `Output 2/3` and `300 -> ckb1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsqltkvh`. 
Note: The operation's change will appear as an output prompt.

#### 2+ Input Addresses, 1 Output Address (`wallet transfer`)
`wallet transfer --capacity <capacity> --from-account <from-account> --to-address <to-address> --tx-fee <tx-fee>`

|          Prompt 1         	|      Prompt 2      	|       Prompt 3      	|                Prompt 4                	|  Prompt 5  	|    Prompt 6    	|
|:-------------------------:	|:------------------:	|:-------------------:	|:--------------------------------------:	|:----------:	|:--------------:	|
|         `Confirm`         	|       `Input`      	|       `Source`      	|                `Amount`                	|    `Fee`   	|  `Destination` 	|
| `Multi-Input Transaction` 	| `<num> of <total>` 	| `<sighash address>` 	| `<input's amount> of <total-capacity>` 	| `<tx-fee>` 	| `<to-address>` 	|

These prompts will appears for each input the user is signing for after the `wallet transfer` command is run once. So if they user is signing an operation with two input addresses they own, they will see these 6 prompts twice.

Note: The input's amount may be more than the total capacity being transferred. The `<input's amount>` is the total capacity of that address's input cells.

#### 2+ Input Addresses, 1 Output Address (`tx sign-inputs`)
`tx sign-inputs --from-account <lock-arg> --tx-file <tx-file>`

|          Prompt 1         	|      Prompt 2      	|       Prompt 3      	|                Prompt 4                	|  Prompt 5  	|    Prompt 6    	|
|:-------------------------:	|:------------------:	|:-------------------:	|:--------------------------------------:	|:----------:	|:--------------:	|
|         `Confirm`         	|       `Input`      	|       `Source`      	|                `Amount`                	|    `Fee`   	|  `Destination` 	|
| `Multi-Input Transaction` 	| `<num> of <total>` 	| `<sighash address>` 	| `<input's amount> of <total-capacity>` 	| `<tx-fee>` 	| `<to-address>` 	|

These prompts will appear once each time they are the command is run with a `from-account` who's key is stored on the hardware wallet.

#### 2+ Input Addresses, 2-5 Output Addresses (`tx sign-inputs`)
`tx sign-inputs --from-account <lock-arg> --tx-file <tx-file>`

|          Prompt 1         	|      Prompt 2      	|       Prompt 3      	|                Prompt 4                	|  Prompt 5  	|           Prompt 6           	|
|:-------------------------:	|:------------------:	|:-------------------:	|:--------------------------------------:	|:----------:	|:----------------------------:	|
|         `Confirm`         	|       `Input`      	|       `Source`      	|                `Amount`                	|    `Fee`   	|    `Output <num>/<total>`    	|
| `Multi-Input Transaction` 	| `<num> of <total>` 	| `<sighash address>` 	| `<input's amount> of <total-capacity>` 	| `<tx-fee>` 	| `<capacity> -> <to-address>` 	|

Prompt 6 repeats as necessary for each of the operation's outputs. Examples of the top and bottom line are `Output 2/3` and `300 -> ckb1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsqltkvh`. 

Notes:
1. The operation's change will appear as an output prompt.
2. The input's amount may be more than the total capacity being transferred. The `<input's amount>` is the total capacity of that address's input cells.

## DAO Commands

#### DAO Deposit
`dao deposit --capacity <capacity> --from-account <lock-arg> --tx-fee <tx-fee>`

|    Prompt 1   	|     Prompt 2     	| Prompt 3   	| Prompt 4                	|
|:-------------:	|:----------------:	|:------------:|:-------------------------:	|
| `Confirm DAO` 	| `Deposit Amount` 	| `Fee`      	| `Cell Owner`            	|
|   `Deposit`   	|   `<capacity>`   	| `<tx-fee>` 	| `<cell-owners-address>` 	|

#### DAO Prepare
`dao prepare --from-account <lock-arg> --out-point <tx_hash>-<output-index> <tx-fee>`

|    Prompt 1   	|     Prompt 2     	|  Prompt 3  	|         Prompt 4        	|
|:-------------:	|:----------------:	|:----------:	|:-----------------------:	|
| `Confirm DAO` 	| `Deposit Amount` 	|    `Fee`   	|       `Cell Owner`      	|
|   `Prepare`   	|   `<capacity>`   	| `<tx-fee>` 	| `<cell-owners-address>` 	|

#### DAO Withdraw
`dao withdraw --from-account <lock-arg> --out-point <tx_hash>-<output-index> --tx-fee <tx-fee>`

|    Prompt 1   	|     Prompt 2     	|       Prompt 3      	|         Prompt 4        	|
|:-------------:	|:----------------:	|:-------------------:	|:-----------------------:	|
| `Confirm DAO` 	| `Deposit Amount` 	|    `Compensation`   	|       `Cell Owner`      	|
|   `Withdraw`  	|   `<capacity>`   	| `<return-capacity>` 	| `<cell-owners-address>` 	|


## Message Signing

#### Raw String 
`util sign-data --from-account <lock-arg> --utf8-string <string>`

|  Prompt 1 	|   Prompt 2  	|
|:---------:	|:-----------:	|
|   `Sign`  	|  `Message`  	|
| `Message` 	| `<string>`  	|

#### Binary Hex
`util sign-data --from-account <lock-arg> --binary-hex <binary-hex>`

|  Prompt 1 	|   Prompt 2  	|
|:---------:	|:-----------:	|
|   `Sign` 	 |  `Message`  	|
| `Binary Hex` 	| `<binary-hex>`  	|

#### Hashed Data 
`util sign-message --from-account <lock-arg> --message <message>`

|    Prompt 1    	|   Prompt 2  	|
|:--------------:	|:-----------:	|
|     `Sign`     	|  `Message`  	|
| `Message Hash` 	|  `<message>` 	|

