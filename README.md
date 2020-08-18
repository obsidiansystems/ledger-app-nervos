# Overview

This repository contains the Ava BOLOS application for the Ledger Nano S and Ledger Nano X and tools for testing the application.

Most instructions assume you have the [Nix](https://nixos.org/nix/) Package Manager, which you can install on any Linux distribution or MacOS.

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

Please download `nano-s-release.tar.gz` from the latest release on  the [releases](https://github.com/obsidiansystems/ledger-app-avax/releases) page of this repo, which contains a pre-compiled app or `.hex` file ready to install on the Ledger. The following sections describe how to install it, including acquiring other tools from the Ledger project.

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
cd ledger-app-avax-s
./install.sh app
```

The first thing that should come up in your terminal is a message that looks like this:

```
Generated random root public key : <long string of digits and letters>
```

Look at your Ledger device's screen and verify that the digits of that key match the digits you can see on your terminal. What you see on your Ledger hardware wallet's screen should be just the beginning and ending few characters of the longer string that printed in your terminal.

You will need to push confirmation buttons on your Ledger device a few times during the installation process and re-enter your PIN code near the end of the process. You should finally see the AVA logo appear on the screen.

If you see the "Generated random root public key" message and then something that looks like this:

```
Traceback (most recent call last):
File "/usr/lib/python3.6/runpy.py", line 193, in _run_module_as_main
<...more file names...>
OSError: open failed
```

the most likely cause is that your `udev` rules are not set up correctly, or you did not unplug your Ledger hardware wallet between setting up the rules and attempting to install. Please confirm the correctness of your `udev` rules.

To load a new version of the AVA application onto the Ledger device in the future, you can run the command again, and it will automatically remove any previously-loaded version.

## Installing the Ledger Application from Source

You can install the Ledger app from source if you have Nix installed. To load the latest version of the AVA app:

``` sh
$ git clone https://github.com/obsidiansystems/ledger-app-avax.git
$ cd ledger-app-avax
$ git checkout master
$ ./nix/install.sh -t s
```
Some notes during app installation:
- 'Starting bats': When building and installing the application from source, the client will run a suite of tests found in the `tests.sh` file. 'bats' stands for "[Bash Automated Testing System](https://github.com/bats-core/bats-core)". These tests may take some time to complete. When they are done, the app installation will proceed.
- Unsafe Manager: you will see a prompt to either allow or deny 'unsafe manager' when running `./nix/install.sh s`. 'Unsafe Manager' is any manager which is not Ledger Live.
- Permission Denied: If you get a “permission denied” error, your computer is not detecting the Ledger device correctly. Make sure the Ledger is connected properly, that it was plugged in since updating the `udev` rules.

You have to accept a few prompts on the Ledger. Then you must select and load the AVA app.

### Confirming the Installed Version

To confirm the version of the application installed on your hardware wallet, first make sure the Ledger device is:

- connected
- unlocked
- has the “AVA” app open (shows “Use wallet to view accounts”)

Then run the following:

``` sh
./check-installed-version.sh

```

If the results of that command match the results of `git rev-parse --short HEAD`, the installation was successful.


# Troubleshooting #

## Application Build Failure ##

If you run into issues building the Ledger application using `nix-shell -A wallet.s --run 'make SHELL=sh all'`, we recommend trying `nix-shell -A wallet.s --run 'make SHELL=sh clean all`.

## Manually Removing Applications from a Ledger Device  ##

If an application is not listed in Ledger Live, it can only be uninstalled from the device over the command line similar to how it was installed.

To uninstall, follow the [instructions for installing a release hex file](#installing-the-ledger-application-from-release) up to the [ledgerblue](#ledgerblue-the-python-module-for-ledger-nano-sx) installation. You can then run `python -m ledgerblue.deleteApp --targetId 0x31100004 --appName "<app-name>"` to uninstall an application, replacing `<app-name>` with the name of the application you'd like to delete.

Note that side-loading applications on the Nano X is not supported with current firmware, thus applications can only be added or removed from a Nano X device through Ledger Live.
