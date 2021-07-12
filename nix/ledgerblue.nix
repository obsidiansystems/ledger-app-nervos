{ pkgs ? import (import ./dep/ledger-platform/thunk.nix + "/dep/nixpkgs") {}
, ...
}:

rec {
  withLedgerblue = (pkgs.python3.withPackages (ps: with ps; [
    ecpy hidapi pycrypto python-u2flib-host requests ledgerblue pillow pkgs.hidapi protobuf
  ]));
  shell = withLedgerblue.env;
}
