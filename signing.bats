
. ./tests/lib.sh

@test "Signing APDU rejects when given garbage to sign" {
  run apdu_fixed "8004400011048000002c800023288000000080000000"
  run apdu_fixed "8004c0000400001111"
  [ "$status" -eq 0 ]
  grep -q "<= b''6a80" <(echo "$output")
}

@test "Signing APDU works for a hash-sized sequence of bytes" {
  # Turn on the Sign Hash opt
  hash="111122223333444455556666777788889999aaaabbbbccccddddeeeeffff0000"
  path="048000002c800023280000000000000000"
  run apdu_fixed "8004000011$path"
  [ "$status" -eq 0 ]
  grep -q "<= b''9000" <(echo "$output")
  run apdu_with_clicks "8004810020$hash" "$ACCEPT_CLICKS"
  promptsCheck 3 tests/sign-unsafe-prompts.txt

  rv="$(egrep "<= b'.*'9000" <(echo "$output")|cut -d"'" -f2)"
  rhash="$(echo "$rv" | head -c64)"
  diff <(echo "$hash") <(echo "$rhash")

  rsig="$(echo "$rv" | tail -c+65)"

  run check_signature "$path" "$hash" "$rsig"
  diff <(echo $output) - <<<"Signature Verified Successfully"
}
