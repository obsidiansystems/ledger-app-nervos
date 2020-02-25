
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


@test "Signing APDU returns something when given something to sign and clickthrough happens." {
  run apdu_fixed "8003000011048000002c800001358000000080000000"
  [ "$status" -eq 0 ]
  grep -q "<= b''9000" <(echo "$output")
  run apdu_with_clicks "800381000400001111" "rR"
  [ "$status" -eq 0 ]
  rv="$(egrep "<= b'.*'9000" <(echo "$output")|cut -d"'" -f2)"
  echo $rv
  # 142 characters of hexadecimal der-encoded signature, one newline.
  # TODO: use openssl to verify the signature.
  diff <(wc -c <<<"$rv") - <<<"141"
 } 
