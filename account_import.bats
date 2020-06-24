
. tests/lib.sh

@test "Ledger app does account import for account 1" {
  run apdu_with_clicks "800500000400000000" "rR"
  promptsCheck 2 tests/account_import_apdu_prompts_0.txt
  [ "$status" -eq 0 ]
  diff tests/account_import_apdu_0.txt <(echo "$output")
}

@test "Ledger app reject account import for invalid account" {
  run apdu_with_clicks "800500000480000000" "rR"
  [ "$status" -eq 0 ]
  grep -q "<= b''6b00" <(echo "$output")
}

