let
  ledger-app-nervos = import ./. {};
in {
  analysis-nanos = ledger-app-nervos.clangAnalysis.s;
  release-nanos = ledger-app-nervos.nano.s.release;
  ckb-cli = (import ./nix/dep/ckb-cli {});
}
