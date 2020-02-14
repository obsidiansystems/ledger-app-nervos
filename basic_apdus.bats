
apdu() {
	LEDGER_PROXY_ADDRESS=127.0.0.1 LEDGER_PROXY_PORT=9999 python -m ledgerblue.runScript --apdu
}

apdu_fixed () {
  echo "$*" | apdu
}

@test "Ledger app version returns 0.1.0" {
  run apdu_fixed "8000000000"
  [ "$status" -eq 0 ]
  diff tests/version_apdu_stdout.txt <(echo "$output")
}

@test "Ledger app git hash returns current hash" {
  run apdu_fixed "8009000000"
  [ "$status" -eq 0 ]
  diff tests/git_apdu_stdout.txt <(echo "$output")
}

