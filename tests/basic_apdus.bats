
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

COMMIT="$(echo "$GIT_DESCRIBE" | sed 's/-dirty/*/')"
HEXCOMMIT="$(echo -n ${COMMIT}|xxd -ps -g0)"

@test "Ledger app git hash returns current hash" {
  run apdu_fixed "8009000000"
  [ "$status" -eq 0 ]
  diff <(sed "s/HEXHASH/${HEXCOMMIT}/; s/HASH/${COMMIT}/" tests/git_apdu_stdout.txt) <(echo "$output")
}

