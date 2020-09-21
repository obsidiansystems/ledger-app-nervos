let
  ledger-app-nervos = import ./. {};
in {
  analysis-nanos = ledger-app-nervos.clangAnalysis.s.wallet;
  release-nanos = ledger-app-nervos.nano.s.release.all;
  release-nanox = ledger-app-nervos.nano.x.release.all;
  debug-build = (import ./. {debug=true;}).nano.s.release.all;
  ckb-cli = import ./nix/dep/ckb-cli {};
  ckb-plugin-ledger = import ./nix/dep/ckb-plugin-ledger {};
  ckb = import ./nix/dep/ckb {};
  usbtool = import ./nix/usbtool.nix {};
}
