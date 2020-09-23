# Debugging

In cases where you need to debug Ledger communication, you may need:

1. Debug Firmware on the Ledger Device
2. A debug version of the Nervos Ledger App
3. Monitored communication between the device and the client via usbtool

## Debugging on NixOS

### Installing Debug Ledger Firmware

todo

### Logging Communication with usbtool

Simply run `nix/env.sh s` to load debug logging tools.


### Installing the Debug Nervos Ledger App

Simply run `NIX_BUILD_ARGS="--arg debug true" nix/install.sh -t s`


## Debugging on Mac

### Installing Debug Ledger Firmware

Ledger provides debug firmware and installation instructions [here](https://ledger.readthedocs.io/en/latest/userspace/debugging.html). Installing the firmware requires `ledgerblue`, which can be made available with the same steps used to [Install Release Files of the Nervos Ledger App](readme.md#installing-the-ledger-application-from-release):

```
sudo pip3 install virtualenv
virtualenv ledger
```

After successfully running those commands, you can follow the steps outlined on Ledger's Debug Firmware Installation Instructions.

### Logging Communication with usbtool

usbtool is available for [download from Google Drive](https://drive.google.com/file/d/16D5vlrbczmBxqpDJml6QUV0RGWs7aZeZ/view). It requilres `libusb`, which we recommend installing with Homebrew. You'll need the compat version:

```
brew install libusb-compat
```

Then, from the `usbtool` directory:
```
make clean
make all
libusb-config --cflags
./usbtool -v 0x2c97 log
```

This will leave an open terminal that logs communication with your Ledger device.

## Debugging on Ubuntu

todo
