
. ./tests/lib.sh

@test "Signing APDU rejects when given garbage to sign and strict checking is enabled." {
  run apdu_fixed "8005400011048000002c800023288000000080000000"
  run apdu_fixed "8005c0000400001111"
  [ "$status" -eq 0 ]
  grep -q "<= b''6a80" <(echo "$output")
}

@test "Signing with a transaction having a type script outside of strict checking prompts with a hash" {
  # Turn on the Sign Hash opt
  # clicks "rRrRlrLRrRlrLRrRrlRL"
  run apdu_fixed "8005000011048000002c800023280000000000000000"
  [ "$status" -eq 0 ]
  # grep -q "<= b''9000" <(echo "$output")
  run apdu_with_clicks "8005810020111122223333444455556666777788889999aaaabbbbccccddddeeeeffff0000" "$ACCEPT_CLICKS"
  promptsCheck 2 tests/sign-unsafe-prompts.txt
}
