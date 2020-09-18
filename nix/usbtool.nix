{ pkgs ? import ./dep/nixpkgs {} }:

pkgs.stdenv.mkDerivation {
  name = "usbtool";
  src = import ./dep/v-usb/thunk.nix;
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
