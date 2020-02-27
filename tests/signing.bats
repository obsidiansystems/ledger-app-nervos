
. ./tests/lib.sh

check_signature () {
  xxd -r -ps <<<"$1" | b2sum -l 256 | xxd -p -r | openssl pkeyutl -verify -pubin -inkey tests/public_key_0_0.pem -sigfile <(xxd -r -ps <<<"$2")
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
  run check_signature "00001111" "$rv"
  diff <(echo $output) - <<<"Signature Verified Successfully"
 } 
