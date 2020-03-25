
. tests/lib.sh

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

@test "Ledger app produces expected top-level public key" {
  run apdu_with_clicks "8002000009028000002c80000135" "rR"
  [ "$status" -eq 0 ]
  diff tests/provide_public_key_apdu_root.txt <(echo "$output")
}

@test "Ledger app prompts with a mainnet key by default" {
  run apdu_with_clicks "8002000011048000002c800001358000000180000001" "rR"
  promptsCheck 3 tests/mainnet-provide-pubkey-prompts.txt
  [ "$status" -eq 0 ]
  diff tests/provide_public_key_apdu_1_1.txt <(echo "$output")
}

@test "Ledger app prompts with a testnet key when testnet addresses are enabled" {
  clicks "lLlLlLlLrRrRrlRLrlRLrRrRrlRLlLlLlL"
  sleep 1
  run apdu_with_clicks "8002000011048000002c800001358000000180000001" "rR"
  promptsCheck 3 tests/testnet-provide-pubkey-prompts.txt
  [ "$status" -eq 0 ]
  clicks "lLlLlLlLrRrRrlRLrlRLrRrRrlRLlLlLlL"
  diff tests/provide_public_key_apdu_1_1.txt <(echo "$output")
}
