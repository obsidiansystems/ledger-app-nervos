
. tests/lib.sh

@test "Ledger app version returns 0.1.0" {
  run apdu_fixed "8000000000"
  [ "$status" -eq 0 ]
  diff tests/version_apdu_stdout.txt <(echo "$output")
}

@test "Ledger app git hash returns current hash" {
  run apdu_fixed "8009000000"
  [ "$status" -eq 0 ]
  diff <(sed "s/HEXHASH/${HEXCOMMIT}/; s/HASH/${COMMIT}/" tests/git_apdu_stdout.txt) <(echo "$output")
}

# This is currently testing for a stub version of wallet ID that blake2b hashes
# the root public key, instead of the correct signing implementation that is
# proving to have determinism problems.

@test "Ledger app wallet ID returns ID for speculos default key" {
  run apdu_fixed "8001000000"
  [ "$status" -eq 0 ]
  diff tests/wallet_id_apdu_stdout.txt <(echo "$output")
}

