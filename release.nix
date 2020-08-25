{ runTest ? true }:
let
  ledger-app = import ./. { inherit runTest; };
in {
  analysis-nanos = ledger-app.clangAnalysis.s.wallet;
  release-nanos = ledger-app.nano.s.release.all;
  debug-build = (import ./. { debug = true; inherit runTest; }).nano.s.release.all;
  usbtool = import ./nix/dep/usbtool.nix {};
}
