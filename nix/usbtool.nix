{ pkgs ? import ./dep/nixpkgs {} }:

let
  # TODO: Replace this with hackGet for added safety checking once hackGet is separated from reflex-platform
  fetchThunk = p:
    if builtins.pathExists (p + /thunk.nix)
      then (import (p + /thunk.nix))
    else p;
in

pkgs.stdenv.mkDerivation {
  name = "usbtool";
  src = fetchThunk ./dep/v-usb;
  preBuild = ''
    cd examples/usbtool
    ./make-files.sh
  '';
  nativeBuildInputs = with pkgs.buildPackages; [ pkg-config ];
  buildInputs = with pkgs; [ libusb1 ];
  installPhase = ''
    install -D usbtool $out/bin/usbtool
    install -D Readme.txt $out/share/doc/usbtool/Readme.txt
  '';
}
