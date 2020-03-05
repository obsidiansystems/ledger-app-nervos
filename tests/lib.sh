
apdu() {
  python -m ledgerblue.runScript --apdu
}

apdu_fixed () {
  echo "$*" | apdu | sed 's/HID //'
}

apdu_with_clicks () {
  echo "$1" | apdu &
  sleep 1
  echo "$2" > /dev/tcp/localhost/5667
}


COMMIT="$(echo "$GIT_DESCRIBE" | sed 's/-dirty/*/')"
HEXCOMMIT="$(echo -n ${COMMIT}|xxd -ps -g0)"

check_signature () {
  xxd -r -ps <<<"$1" | blake2 --length 32 --personal 636b622d64656661756c742d68617368 | xxd -p -r | openssl pkeyutl -verify -pubin -inkey tests/public_key_0_0.pem -sigfile <(xxd -r -ps <<<"$2")
}

sendTransaction() {
  bytesToSign=$(($(wc -c <<<"$1")/2))
  toSend=$1
  while [ "$bytesToSign" -gt 230 ] ;
  do 
    apdu_fixed "80034100e6$(head -c 460 <<<"$toSend")"
    # [ "$status" -eq 0 ]
    # grep -q "<= b''9000" <(echo "$output")
    toSend="$(tail -c+461 <<<"$toSend")";
    bytesToSign=$(($(wc -c <<<"$toSend")/2))
  done
  bytes=$(printf "%x" $(($(wc -c <<<"$toSend")/2)))
  if [ -z "$2" ]; then
    run apdu_with_clicks "8003c100$bytes$toSend" "rR"
  else
    run apdu_fixed "8003e100$bytes$toSend"
  fi
}

doSign() {
  run apdu_fixed "8003400011048000002c800001358000000080000000"
  [ "$status" -eq 0 ]
  grep -q "<= b''9000" <(echo "$output")
  bytesToSign=$(($(wc -c <<<"$1")/2))
  toSend=$1
  while [ "$bytesToSign" -gt 230 ] ;
  do 
    apdu_fixed "80034100e6$(head -c 460 <<<"$toSend")"
    # [ "$status" -eq 0 ]
    # grep -q "<= b''9000" <(echo "$output")
    toSend="$(tail -c+461 <<<"$toSend")";
    bytesToSign=$(($(wc -c <<<"$toSend")/2))
  done
  bytes=$(printf "%x" $(($(wc -c <<<"$toSend")/2)))
  run apdu_with_clicks "8003c100$bytes$toSend" "rR"
}

