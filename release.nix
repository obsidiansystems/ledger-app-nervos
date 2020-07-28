let
  ledger-app-nervos = import ./. {};
in {
  release-nanox = ledger-app-nervos.nano.x.release.all;
}
