
apdu() {
	LEDGER_PROXY_ADDRESS=127.0.0.1 LEDGER_PROXY_PORT=9999 python -m ledgerblue.runScript --apdu
}

apdu_fixed () {
  echo "$*" | apdu
}

apdu_with_clicks () {
  echo "$1" | apdu &
  sleep 1
  echo "$2" > /dev/tcp/localhost/5667
}

@test "Ledger app produces a public key upon request" {
  run apdu_with_clicks "8002000011048000002c800001358000000080000000" "rR"
  [ "$status" -eq 0 ]
  diff tests/provide_public_key_apdu_0_0.txt <(echo "$output")
}

@test "Ledger app produces a different public key upon request" {
  run apdu_with_clicks "8002000011048000002c800001358000000180000001" "rR"
  [ "$status" -eq 0 ]
  diff tests/provide_public_key_apdu_1_1.txt <(echo "$output")
}

