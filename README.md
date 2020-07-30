# Overview

This repository contains the Ava BOLOS application for the Ledger Nano S and Ledger Nano X and tools for testing the application. While this app is currently under development.

These applications has been developed against our forks of [CKB-CLI](https://github.com/obsidiansystems/ckb-cli) and [CKB](https://github.com/obsidiansystems/ckb). Most instructions assume you have the [Nix](https://nixos.org/nix/) Package Manager, which you can install on any Linux distribution or MacOS. Application and wallet developers who would like to support Ledger can do so with [LedgerJS](https://github.com/obsidiansystems/ledgerjs/tree/nervos).

# System Requirements

System requirements differ based on if you are using or installing the application. If you are using a Linux machine, you will need to [Prepare your Linux Machine for Ledger Device Communication](#preparing-your-linux-machine-for-ledger-device-communication) for both installing and usaging the application.

This applications are built against Ledger Nano S firmware 1.6.0 and Ledger Nano X firmware XXX. Please use [Ledger Live](https://www.ledger.com/ledger-live) to manage your Ledger device's firmware.

## For Application Installation

Installation requirements differ based on installation method:

### Installing with Ledger Live ###
[Ledger Live](https://www.ledger.com/ledger-live) is the easiest way to install applications on your Ledger device. Please refer to [Ledger Live's system requirements](https://support.ledger.com/hc/en-us/articles/360006395553-Download-and-install-Ledger-Live) for this installation method. This application is still in active development and not yet available in Ledger Live.

### Installing Release Files ###

#### Supported Operating Systems ####

- **Linux**: Supported. Debian distributions such as Ubuntu (18.04+) and NixOS (vXXX+). Other linux distributions may work, but have not been tested.
- **Mac**: Supported. This has been tested on Catalina 10.15.5. Other versions may work, but have not been tested.
- **Windows**: Not currently supported.

#### Package/Tool Dependency ####

- **libusb and libudev**: On Debian distributions, including Ubuntu, these packages are suffixed with `-dev`. Other distributions may have their own conventions.
     - Linux Installation: `sudo apt-get install libusb-1.0.0-dev libudev-dev`
     - Mac Installation: N/A (not necessary)
- **Python3**: Use `python3 --version` to check whether you have python3 installed or to confirm an installation was successful.
     - Linux Installation: `sudo apt-get install python3-pip # Ubuntu example`
     - Mac Installation: Python 3 can be installed on mac with homebrew: `brew install python`
- **virtualenv**: Use `virtualenv --version` to check whether you have virtualenv installed or to confirm an installation was successful.
     - Linux Installation: To install using python3: `pip3 install virtualenv` (this may require `sudo`).
     - Mac Installation: To install using python3: `pip3 install virtualenv` (this may require `sudo`).

### Installing from Source ###

#### Supported Operating Systems ####

- **Linux**: Supported. Debian distributions such as Ubuntu (18.04+) and NixOS (vXXX+). Other linux distributions may work, but have not been tested.
- **Mac**: Supported on Mojave (10.14), but not supported on Catalina due to [issues with Nix installation](https://nixos.org/nix/manual/#sect-macos-installation).
- **Windows**: Not currently supported.

#### Package/Tool Dependency ####

- **Nix Package Manager**: Use `nix-env --version` to check whether you have Nix installed or to confirm an installation was successful. We recommend checking which [Nix Installation](https://nixos.org/nix/manual/#chap-installation) is right for you, but for most users the following will will work:
     - Linux Installation: `sh <(curl -L https://nixos.org/nix/install) --daemon`
     - Mac Installation: `sh <(curl -L https://nixos.org/nix/install) --daemon`

These applications are built against Ledger Nano S firmware 1.6.0 and Ledger Nano X firmware 1.2.4-1. Please use [Ledger Live](https://www.ledger.com/ledger-live) to manage your Ledger device's firmware.

## For Application Usage

Most of the instructions in this README cover how to use the Ledger application with Nervos' client, [CKB-CLI](https://github.com/nervosnetwork/ckb-cli). If you have installed CKB-CLI from its upstream location, please refer to their system requirements.

If you are using this application with a wallet, please refer to that wallet's system requirements.

## Preparing Your Linux Machine for Ledger Device Communication

On Linux, the "udev" rules must be set up to allow your user to communicate with the ledger device. MacOS devices do not need any configuration to communicate with a Ledger device, so if you are using Mac you can ignore this section.

### NixOS

On NixOS, one can easily do this with by adding the following to configuration.nix:
``` nix
{
  # ...
  hardware.ledger.enable = true;
  # ...
}
```

### Non-NixOS Linux Distros

For non-NixOS Linux distros, LedgerHQ provides a [script](https://raw.githubusercontent.com/LedgerHQ/udev-rules/master/add_udev_rules.sh) for this purpose, in its own [specialized repo](https://github.com/LedgerHQ/udev-rules). Download this script, read it, customize it, and run it as root:

```
$ wget https://raw.githubusercontent.com/LedgerHQ/udev-rules/master/add_udev_rules.sh
$ chmod +x add_udev_rules.sh
```

We recommend against running the next command without reviewing the script and modifying it to match your configuration.

```
$ sudo ./add_udev_rules.sh
```

Subsequently, unplug your ledger hardware wallet, and plug it in again for the changes to take effect.

For more details, see [Ledger's documentation](https://support.ledger.com/hc/en-us/articles/115005165269-Fix-connection-issues).

# Ledger App Installation

There are 3 ways you can install this Ledger application:
1. [Ledger Live](https://www.ledger.com/ledger-live) *(not yet available)*: Ledger Live is the easiest way to install applications on your Ledger device. However, this application is still in active development and not yet available in Ledger Live.
2. [Installing from Release](#installing-the-ledger-application-from-release): This is the recommended installation method until this app is available in Ledger Live.
3. [Installing from Source](#installing-the-ledger-application-from-source): Recommended for development only.

*Note: You can only install applications on the Ledger Nano X through Ledger Live.*

## Installing the Ledger Application from Release

Please download `nano-s-release.tar.gz` from the latest release on  the [releases](https://github.com/obsidiansystems/ledger-app-nervos/releases) page of this repo, which contains a pre-compiled app or `.hex` file ready to install on the Ledger. The following sections describe how to install it, including acquiring other tools from the Ledger project.

### Installing BOLOS Python Loader

Install `libusb` and `libudev`, with the relevant headers. On Debian-based distros, including Ubuntu, the packages with the headers are suffixed with `-dev`. Other distros will have their own conventions. So, for example, on Ubuntu, you can do this with:

```
$ sudo apt-get install libusb-1.0.0-dev libudev-dev # Ubuntu example
```

Then, install `pip3`. You must install `pip3` for this and not `pip`. On Ubuntu:

```
$ sudo apt-get install python3-pip # Ubuntu example
```

Now, on any operating system, install `virtualenv` using `pip3`. It is important to use `pip3` and not `pip` for this, as this module requires `python3` support.

```
$ sudo pip3 install virtualenv # Any OS
```

Then create a Python virtual environment (abbreviated *virtualenv*). You could call it anything, but we shall call it "ledger". This will create a directory called "ledger" containing the virtualenv:

```
$ virtualenv ledger # Any OS
```

Then, you must enter the `virtualenv`. If you do not successfully enter the `virtualenv`, future commands will fail. You can tell you have entered the virtualenv when your prompt is prefixed with `(ledger)`.

```
$ source ledger/bin/activate
```

Your terminal session -- and only that terminal session -- will now be in the virtual env. To have a new terminal session enter the virtualenv, run the above `source` command only in the same directory in the new terminal session.

### ledgerblue: The Python Module for Ledger Nano S/X

We can now install `ledgerblue`, which is a Python module designed originally for Ledger Blue, but also is needed for the Ledger Nano S/X.

Although we do not yet support Ledger Blue, you must still install the following python package. Within the virtualenv environment -- making sure that `(ledger)` is showing up before your prompt -- use pip to install the `ledgerblue` [Python package](https://pypi.org/project/ledgerblue/). This will install the Ledger Python packages into the virtualenv; they will be available only in a shell where the virtualenv has been activated.

```
$ pip install ledgerblue
```

If you have to use `sudo` or `pip3` here, that is an indication that you have not correctly set up `virtualenv`. It will still work in such a situation, but please research other material on troubleshooting `virtualenv` setup.

### Load the Application onto the Ledger Device

Next you'll use the installation script to install the application on your Ledger device.

The Ledger device must be in the following state:

  * Plugged into your computer
  * Unlocked (enter your PIN)
  * On the home screen (do not have any application open)
  * Not asleep (you should not see *vires in numeris* is scrolling across the
    screen)

If you are already in an application or the Ledger device is asleep, your installation process will fail.

We recommend staying at your computer and keeping an eye on the Ledger device's screen as you continue. You may want to read the rest of these instructions before you begin installing, as you will need to confirm and verify a few things during the process.

Still within the virtualenv, run the `./install.sh` command included in the `nano-s-release.tar.gz` that you downloaded. This `./install.sh` script takes the path to an application directory. The only such directory included in the downloaded `release.tar.gz` will be `app`, so install the app like this, replacing `~/Downloads/` with whatever directory you downloaded the file into:

```
cd ~/Downloads/
tar xzf nano-s-release.tar.gz
cd ledger-app-nervos-s
./install.sh app
```

The first thing that should come up in your terminal is a message that looks like this:

```
Generated random root public key : <long string of digits and letters>
```

Look at your Ledger device's screen and verify that the digits of that key match the digits you can see on your terminal. What you see on your Ledger hardware wallet's screen should be just the beginning and ending few characters of the longer string that printed in your terminal.

You will need to push confirmation buttons on your Ledger device a few times during the installation process and re-enter your PIN code near the end of the process. You should finally see the Nervos logo appear on the screen.

If you see the "Generated random root public key" message and then something that looks like this:

```
Traceback (most recent call last):
File "/usr/lib/python3.6/runpy.py", line 193, in _run_module_as_main
<...more file names...>
OSError: open failed
```

the most likely cause is that your `udev` rules are not set up correctly, or you did not unplug your Ledger hardware wallet between setting up the rules and attempting to install. Please confirm the correctness of your `udev` rules.

To load a new version of the Nervos application onto the Ledger device in the future, you can run the command again, and it will automatically remove any previously-loaded version.

## Installing the Ledger Application from Source

You can install the Ledger app from source if you have Nix installed. To load the latest version of the Nervos app:

``` sh
$ git clone https://github.com/obsidiansystems/ledger-app-nervos.git
$ cd ledger-app-nervos
$ git checkout master
$ ./nix/install.sh s
```
Some notes during app installation:
- 'Starting bats': When building and installing the application from source, the client will run a suite of tests found in the `tests.sh` file. 'bats' stands for "[Bash Automated Testing System](https://github.com/bats-core/bats-core)". These tests may take some time to complete. When they are done, the app installation will proceed.
- Unsafe Manager: you will see a prompt to either allow or deny 'unsafe manager' when running `./nix/install.sh s`. 'Unsafe Manager' is any manager which is not Ledger Live.
- Permission Denied: If you get a “permission denied” error, your computer is not detecting the Ledger device correctly. Make sure the Ledger is connected properly, that it was plugged in since updating the `udev` rules.

You have to accept a few prompts on the Ledger. Then you must select and load the Nervos app.

### Confirming the Installed Version

To confirm the version of the application installed on your hardware wallet, first make sure the Ledger device is:

- connected
- unlocked
- has the “Nervos” app open (shows “Use wallet to view accounts”)

Then run the following:

``` sh
./check-installed-version.sh

```

If the results of that command match the results of `git rev-parse --short HEAD`, the installation was successful.

# Using the Nervos Ledger App with CKB-CLI

The Nervos Ledger app is built to work with CKB-CLI. Some of these CKB-CLI subcommands, such as `account import`, do not require that you're connected to a network such as the testnet Aggron or a devnet. Others, such as `wallet transfer` or `DAO operations`, must be submitted to a network for their result to be actualized. For testing purposes, we recommend [Using the Nervos Devnet](using-devnet.md), but you can also [Use the Nervos Testnet Aggron](using-testnet.md).

## Installing the Client

To use the CKB command line utility with the Ledger, you must currently use the Obsidian fork of the client.
Note that Obsidian's fork of the client has a number of experimental features that do not currently exist upstream and, resultantly,
should only be used for development and testing purposes.

To build and start it, run the following from within this repository:

``` sh
$ nix run -f nix/dep/ckb-cli -c ckb-cli
```

All commands that follow prefixed with ‘CKB>’ should be run in the prompt provided by the above command.

### Installing the client manually:
It is also possible to install ckb-cli without nix. Should you choose to do so, please
follow the upstream repo's instructions regarding the installation of dependencies
```
git clone https://github.com/obsidiansystems/ckb-cli.git
cd ckb-cli
git checkout ledger-app
cargo run
 # To install
cargo install --path . -f
```

## Listing Ledger Devices ###

Use the `account list` command to see connected Ledger devices. Be sure to have the Nervos application open on the device, otherwise it will not be detected:

``` sh
CKB> account list
- "#": 0
  account_source: ledger hardware wallet
  ledger_id: 0x69c46b6dd072a2693378ef4f5f35dcd82f826dc1fdcc891255db5870f54b06e6
```

The `ledger_id` shown is the public key hash for the path m/44'/9000', which is the root Avax path. the `ledger_id` will be used for ```<ledger-id>``` argument in the `account import` command as described below.

If you have already imported the Ledger account, then `account list` command will instead give the account details.
They will be shown even if the device is not connected.

``` sh
CKB> account list
- "#": 0
  account_source: ledger hardware wallet
  address:
    mainnet: ckb1qyqry754h4tevmngdll9jrpnrnfhqqhpcccskdl3en
    testnet: ckt1qyqry754h4tevmngdll9jrpnrnfhqqhpcccstgpw40
  lock_arg: 0x327a95bd57966e686ffe590c331cd37002e1c631
  lock_hash: 0xc27b9ad3414cf5b1720713663d5f754e8968793f2da90b6428feb565bf94de4e
```

## Account Import ###

Use the `account import --ledger <ledger_id>` command to import the account to the `ckb-cli`.
You will receive a confirmation prompt on the device which should say `Import Account`.
Confirm this to import the account. This operation will provide the extended public key of path `m/44'/9000'/0'` to the `ckb-cli`.

``` sh
CKB> account import --ledger 0x69c46b6dd072a2693378ef4f5f35dcd82f826dc1fdcc891255db5870f54b06e6
- "#": 0
  account_source: ledger hardware wallet
  address:
    mainnet: ckb1qyqry754h4tevmngdll9jrpnrnfhqqhpcccskdl3en
    testnet: ckt1qyqry754h4tevmngdll9jrpnrnfhqqhpcccstgpw40
  lock_arg: 0x327a95bd57966e686ffe590c331cd37002e1c631
  lock_hash: 0xc27b9ad3414cf5b1720713663d5f754e8968793f2da90b6428feb565bf94de4e
```

Now that the account has been imported, it is remembered by the client and is visible when you run `account list`.

## Using your Account's Addresses ##

### Get BIP44 Address Public Keys ###

Use the `account bip44-addresses` command to obtain the first 20 receiving and 10 change addresses for your account.

``` sh
CKB> account bip44-addresses --lock-arg <lock-arg>
```

Note that this command is provided as a convenience by the `ckb-cli` to get a list of addresses with the derivation path quickly. Before sharing one of these receiving addresses, **it is highly recommended that you verify the address provided by this command on the Ledger device using the `account extended-address` command as described next.**

### Obtain / Verify Public Key ###

The `account extended-address` command should be used to

- Verify the public key obtained via `account bip44-addresses` command on the Ledger device
- Obtain the public key for any arbitrary BIP44 derivation path

Here's an example command that provide the first receiving address for the lock-arg `0x327a95bd57966e686ffe590c331cd37002e1c631`:

``` sh
CKB> account extended-address --path "m/44'/9000'/0'/0/1" --lock-arg 0x327a95bd57966e686ffe590c331cd37002e1c631
```

This should show up on the ledger as 2 prompts:

|   Prompt 1   	|                     Prompt 2                     	|
|:------------:	|:------------------------------------------------:	|
|   `Provide`  	|                     `Address`                    	|
| `Public Key` 	| `ckb1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsqltkvh` 	|

**Verifying the output address printed by `ckb-cli` matches the one shown on Ledger prompt is highly recommended. Please read [Ledger's documentation](https://support.ledger.com/hc/en-us/articles/360006433934) on the subject.**

*Note: If you've changed the app's configuration to show testnet addresses, the `Address` prompt will instead show the testnet address that begins with `ckt`. This setting persists between power cycles.*

After accepting the prompt on the Ledger the output on `ckb-cli` will show information about the address:

``` text
address:
  mainnet: ckb1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsqltkvh
  testnet: ckt1qyqxxtzygxvjwhgqklqlkedlqqwhp0rqjkvsa64fqt
lock_arg: 0x632c444199275d00b7c1fb65bf001d70bc609599
lock_hash: 0xee0283c2d991992d6e015a4680c54318ad42c820ca0dc862c0a1d68c415499a8
```

## Transfer CKB ##

Basic transfers can be done using the `wallet transfer` command:

``` sh
CKB> wallet transfer \
    --from-account <lock-arg> \
    --to-address <to-address> \
    --capacity <capacity> \
    --tx-fee <tx-fee>
```
The on-device prompts for this command are as follows:

|    Prompt 1   	|   Prompt 2   	|  Prompt 3  	|    Prompt 4    	|
|:-------------:	|:------------:	|:----------:	|:--------------:	|
|   `Confirm`   	|   `Amount`   	|    `Fee`   	|  `Destination` 	|
| `Transaction` 	| `<capacity>` 	| `<tx-fee>` 	| `<to-address>` 	|

### Complex Transfers ###

More complicated transactions, such as those with multiple outputs or inputs from different lock-args, can be constructed in a JSON file. We recommend the following resources for more complex transactions:
- [Handling Complex Transaction](https://github.com/nervosnetwork/ckb-cli/wiki/Handle-Complex-Transaction)
- [How to use Multisigs with CKB-CLI](https://medium.com/@obsidian.systems/how-to-use-multisigs-with-ckb-cli-5fbd7f4f56e4)

When doing more complex transactions, please note:
- Different transaction variants may have different on-device prompts so the user can verify all the aspects of what they are signing. (*TODO: show all variants of transaction prompts*)
- There are restrictions you may encounter due to device constraints. For instance, The Nano S can only sign a transaction with a maximum of 3 non-change outputs. The Nano X can sign a maximum of 5 non-change outputs in a single transaction.

## Checking Chain Data ##

### Verifying Address Balances ####

Before or after a transaction, you may wish to verify the balance of an address. Do this with:

``` sh
CKB> wallet get-capacity --address <address>
total: 100.0 (CKB)
```

### Geting an Address's Live Cells ####

Get live cells:

``` sh
CKB> wallet get-live-cells --address <address>

current_capacity: 2000.0 (CKB)
current_count: 1
live_cells:
  - capacity: 2000.0 (CKB)
    data_bytes: 0
    index:
      output_index: 0
      tx_index: 2
    lock_hash: 0x8cf5955c203e3bd9c0fa1ceac94206dca01e32a674ba17060e77f8e52750e491
    mature: true
    number: 460368
    tx_hash: 0xe58df9496d58685516291bf3db0ccbdbc30a53a7316639676b7ad98020d13146
    tx_index: 0
    type_hashes: ~
total_capacity: 2000.0 (CKB)
total_count: 1
```

## Message Signing ##
*Note that the following instructions reflect an anticipation of the future state of ckb-cli and are not all
compatible with Obsidian's current ledger-app branch. For more instructions on using the ledger's message signing
features with the ledger-app branch of ckb-cli see [Experimental-ckb-cli.md](Experimental-ckb-cli.md)*

To sign the hash of a message with their ledger a user may do the following:

```sh
CKB> util sign-data --utf8-string "hello world i love nervos" --from-account <my-ledger-account>
message-hash: <blake2b hash of: magic_bytes + message>
recoverable: false
signature: <signature>
```
If a message is longer than 64 characters the ledger will display the first 61 chars, followed by an ellipsis (`...`)
The ckb-cli accepts utf8 strings in its `--utf8-string` argument, but the ledger can not display all chars. If the ledger comes accross a
character that it is unnable to display it will display an asterisk (`\*`) instead.

Prompts on the Ledger device are as follows:

|  Prompt 1 	|   Prompt 2  	|
|:---------:	|:-----------:	|
|   `Sign`  	|  `Message`  	|
| `Message` 	| `<message>` 	|

One can verfiy the signature as follows:

```bash
CKB> util verify-signature --utf8-string "hello world i love nervos" --from-account <my-ledger-account> --signature <signature from above>
pubkey: <pubkey of my ledger's account root>
recoverable: <signature type>
verify-ok: true
```
A user can also choose to sign the hash of a message with an extended address, or with a recoverable signature ( these options are mutually inclusive):

```bash
CKB> util sign-data --utf8-string "hello world i love nervos" \
--from-account <my-ledger-account> --extended-address <address> --recoverable

message-hash: <blake2b hash of: magic_bytes + message>
recoverable: true
signature: <signature>
```

A user may also switch out the `--utf8-string` option with a `--binary-hex` option, and the ledger will display
the message as hex, instead of ascii characters
```
CKB> util sign-data --binary-hex '0x61' --from-account <my-ledger-account>
```

### Signing a Hash ###
The ledger also allows for the signing of a pre-hashed message. Due to the potential security risks of this option, a user must "opt-in" to this
setting by going into the `Configuration` section of the app, and setting `Allow sign hash` to `On`.
```
CKB> util sign-message --message <message hash> --from-account <my-ledger-account>
```
The ledger will display the following:

|    Prompt 1    	|   Prompt 2  	|
|:--------------:	|:-----------:	|
|     `Sign`     	|  `Message`  	|
| `Message Hash` 	| `<message>` 	|

If the length of the hash is greater than 64 bytes, or if the `Allow sign hash` is set to `Off`, the ledger will reject the message.

The user may also use the `Sign Hash` feature by selecting the `--no-magic-bytes` flag in a `util sign-data` command. If this is the case,
the client will hash the data for the user and send it to the ledger to sign.

## DAO Operations ##

### Deposit into the NervosDAO ###

You can deposit to the NervosDAO with the following command:

``` sh
CKB> dao deposit \
    --capacity <capacity> \
    --from-account <lock-arg> \
    --path <path> \
    --tx-fee <tx-fee>
```
Prompts on the Ledger device are as follows:

|    Prompt 1   	|     Prompt 2     	| Prompt 3   	| Prompt 4                	|
|:-------------:	|:----------------:	|:------------:|:-------------------------:	|
| `Confirm DAO` 	| `Deposit Amount` 	| `Fee`      	| `Cell Owner`            	|
|   `Deposit`   	|   `<capacity>`   	| `<tx-fee>` 	| `<cell-owners-address>` 	|

#### Get Cells Deposited in the NervosDAO ####

After you've made a deposit to the NervosDAO, you can confirm it using `dao query-deposited-cells`:

``` sh
CKB> dao query-deposited-cells --address <address>

live_cells:
  - capacity: 10200000000
    data_bytes: 8
    index:
      output_index: 0
      tx_index: 1
    lock_hash: 0xa77a89c29289311b5d6a01f234facc8244cf455909260533e11183054852ff61
    number: 472898
    tx_hash: 0xc55bad328edd74c4be1e630d0eb52733d9ed027f02eaca10f0e78b96a44053fc
    tx_index: 0
    type_hashes:
      - 0x82d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e
      - 0xcc77c4deac05d68ab5b26828f0bf4565a8d73113d7bb7e92b8362b8a74e58e58
total_capacity: 10200000000
```

Remember the values above for one of the live cells under `tx_hash` and `output_index`. You'll need these when constructing the `dao prepare` operation below which prepares a cell for withdrawal from the NervosDAO.

### Prepare Cells for Withdrawal from the NervosDAO ###

To prepare a cell for withdrawal from the NervosDAO:

``` sh
CKB> dao prepare \
    --from-account <lock-arg> \
    --path <path> \
    --out-point <tx_hash>-<output_index> \
    --tx-fee <tx-fee>
```
Prompts on the Ledger device are as follows:

|    Prompt 1   	|     Prompt 2     	|  Prompt 3  	|         Prompt 4        	|
|:-------------:	|:----------------:	|:----------:	|:-----------------------:	|
| `Confirm DAO` 	| `Deposit Amount` 	|    `Fee`   	|       `Cell Owner`      	|
|   `Prepare`   	|   `<capacity>`   	| `<tx-fee>` 	| `<cell-owners-address>` 	|

#### Get Cells Prepared for Withdrawal from NervosDAO ####

After you've prepared your cell for withdrawal from the NervosDAO, you can confirm its status using `dao-query-prepared-cells`:

``` sh
CKB> dao query-prepared-cells --address <address>

live_cells:
  - capacity: 10500000000
    data_bytes: 8
    index:
      output_index: 0
      tx_index: 1
    lock_hash: 0xa77a89c29289311b5d6a01f234facc8244cf455909260533e11183054852ff61
    maximum_withdraw: 10500154580
    number: 493786
    tx_hash: 0xae91f2a310f2cfeada391e5f76d0addcc56d99c91a39734c292c930a1cfc67c2
    tx_index: 0
    type_hashes:
      - 0x82d76d1b75fe2fd9a27dfbaa65a039221a380d76c926f378d3f81cf3e7e13f2e
      - 0xcc77c4deac05d68ab5b26828f0bf4565a8d73113d7bb7e92b8362b8a74e58e58
total_maximum_withdraw: 10500154580
```

Remember the values above for one of the live cells under `tx_hash` and `output_index`. You'll need these when constructing the `withdraw` operation below which withdraws CKB from the NervosDAO.

### Withdraw ###

To withdraw a prepared cell from the NervosDAO:

``` sh
CKB> dao withdraw \
    --from-account <lock-arg> \
    --path <path> \
    --out-point <tx_hash>-<output_index> \
    --tx-fee <tx-fee>
```
Prompts on the Ledger device are as follows:

|    Prompt 1   	|     Prompt 2     	|       Prompt 3      	|         Prompt 4        	|
|:-------------:	|:----------------:	|:-------------------:	|:-----------------------:	|
| `Confirm DAO` 	| `Deposit Amount` 	|    `Compensation`   	|       `Cell Owner`      	|
|   `Withdraw`  	|   `<capacity>`   	| `<return-capacity>` 	| `<cell-owners-address>` 	|

Compensation is the amount of CKB you have earned for having CKB deposited in the NervosDAO. It does not include the the original deposited amount. For more information about compensation, we recommend reading the [Nervos DAO RFC](https://github.com/nervosnetwork/rfcs/blob/master/rfcs/0023-dao-deposit-withdraw/0023-dao-deposit-withdraw.md).

If you attempt to withdraw from the Nervos DAO prematurely, you'll see an error such as
```
JSON-RPC 2.0 Error: Server error (Transaction: Immature)
```
or
```
JSON-RPC 2.0 Error: Server error (OutPoint: ImmatureHeader(Byte32(0xd7de1ffd49c71b5dc71fcbf1638bb72c8fb16f8fffdfd5172456a56167fea0a3)))
```
This means your prepared cell is not yet available to withdraw. You'll need to wait for the conclusion of your current deposit period before withdrawing.

# Troubleshooting #

## Application Build Failure ##

If you run into issues building the Ledger application using `nix-shell -A wallet.s --run 'make SHELL=sh all'`, we recommend trying `nix-shell -A wallet.s --run 'make SHELL=sh clean all`.

## Manually Removing Applications from a Ledger Device  ##

If an application is not listed in Ledger Live, it can only be uninstalled from the device over the command line similar to how it was installed.

To uninstall, follow the [instructions for installing a release hex file](#installing-the-ledger-application-from-release) up to the [ledgerblue](#ledgerblue-the-python-module-for-ledger-nano-sx) installation. You can then run `python -m ledgerblue.deleteApp --targetId 0x31100004 --appName "<app-name>"` to uninstall an application, replacing `<app-name>` with the name of the application you'd like to delete.

Note that side-loading applications on the Nano X is not supported with current firmware, thus applications can only be added or removed from a Nano X device through Ledger Live.
