{ pkgs ? import ./nixpkgs {} }:

pkgs.stdenv.mkDerivation {
  name = "usbtool";
  src = pkgs.fetchFromGitHub {
    owner = "obdev";
    repo = "v-usb";
    rev = "9a42d205eb60faca494ff4eabce8d59f0ec0fc7f";
    sha256 = "009zm7dl69fcj7jachlrxafi8scggwq9dsbqcshf3wk34pragjhw";
  };
  preBuild = ''
    cd examples/usbtool
    ./make-files.sh
  '';
  buildInputs = [ pkgs.libusb ];
  installPhase = ''
    install -D usbtool $out/bin/usbtool
    install -D Readme.txt $out/share/doc/usbtool/Readme.txt
  '';
}
