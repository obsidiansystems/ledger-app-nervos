
apdu() {
  python -m ledgerblue.runScript --apdu
}

apdu_fixed () {
  echo "$*" | apdu | sed 's/HID //'
}

clicks() {
  echo "$1" > /dev/tcp/localhost/5667
}

apdu_with_clicks () {
  echo "$1" | apdu &
  sleep 1
  clicks "$2"
}



COMMIT="$(echo "$GIT_DESCRIBE" | sed 's/-dirty/*/')"
HEXCOMMIT="$(echo -n ${COMMIT}|xxd -ps -g0)"

blake2b_p () {
  blake2 --length 32 --personal 636b622d64656661756c742d68617368 | tee txhashdump
}

blake2b_lock_hash () {
  (blake2b_p | tee dumpfile1 ; echo -n "5500000000000000 55000000100000005500000055000000410000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000") | tee dumpfile | xxd -p -r | blake2b_p
}

check_signature () {
  sig=$(tr -d -c '[:xdigit:]' <<<"$3")
  r_val=$(head -c64 <<<"$sig")
  s_val=$(tail -c+65 <<<"$sig" | head -c64)

  # Check the high bit, add a zero byte if needed.
  if [ "$(xxd -r -ps <<<"$r_val" | xxd -b | cut -d' ' -f2 | head -c1 )" == "1" ] ; then r_val="00$r_val"; fi;
  if [ "$(xxd -r -ps <<<"$s_val" | xxd -b | cut -d' ' -f2 | head -c1 )" == "1" ] ; then s_val="00$s_val"; fi;

  rlen=$((${#r_val}/2))
  slen=$((${#s_val}/2))

  rfmt="02 $(printf "%02x" $rlen) ${r_val}"
  sfmt="02 $(printf "%02x" $slen) ${s_val}"

  SIG="30 $(printf "%02x" $(($rlen+$slen+4))) $rfmt $sfmt"

  xxd -r -ps <<<"$2" | tee dumpfile0 | blake2b_p | tee hashfile | xxd -p -r | openssl pkeyutl -verify -pubin -inkey <(get_key_in_pem $1) -sigfile <(xxd -r -ps <<<"$SIG")
}

get_key_in_pem() {
  derivation=${1:-8002000015058000002c80000135800000000000000000000000}
  result="$(apdu_with_clicks $derivation "rR" |& egrep "b'41.*'9000" | sed "s/^<= b'41//;s/'9000$//")"
  echo "-----BEGIN PUBLIC KEY-----"
  cat tests/public_key_der_prefix.der <(xxd -ps -r <<<"$result") | base64
  echo "-----END PUBLIC KEY-----"
}

sendTransaction() {
  bytesToSign=$(($(wc -c <<<"$1")/2))
  toSend=$1
  flag=40
  while [ "$bytesToSign" -gt 230 ] ;
  do
    apdu_fixed "8003${flag}00e6$(head -c 460 <<<"$toSend")"
    flag=41
    # [ "$status" -eq 0 ]
    # grep -q "<= b''9000" <(echo "$output")
    toSend="$(tail -c+461 <<<"$toSend")";
    bytesToSign=$(($(wc -c <<<"$toSend")/2))
    echo $bytesToSign
  done
  bytes=$(printf "%02x" $(($(wc -c <<<"$toSend")/2)))
  echo TO SEND: $toSend
  if [ -z "$2" ]; then
    apdu_with_clicks "8003c100$bytes$toSend" "rR"

  elif [ "$2" = "--expectReject" ]; then
    apdu_fixed "8003c100$bytes$toSend"
  else # [ "$2" = "--isCtxd" ]
    apdu_fixed "8003e100$bytes$toSend"
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
  bytes=$(printf "%02x" $(($(wc -c <<<"$toSend")/2)))
  run apdu_with_clicks "8003c100$bytes$toSend" "rR"
}


promptsCheck() {
  if [ "$DEBUG" != "1" ]; then return 0; fi;
  egrep -A2 'Prompt [0-9]:' speculos.log | tail -n $(($1*3))
  diff $2 <(egrep -A2 'Prompt [0-9]:' speculos.log | tail -n $(($1*3)))
}

rejectionMessageCheck() {
  if [ "$DEBUG" != "1" ]; then return 0; fi;
  test "$(egrep '^Rejecting: ' speculos.log | tail -n1)" = "Rejecting: $1"
}
