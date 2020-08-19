#! /usr/bin/env nix-shell
#! nix-shell -i bash -A wallet.s
source tests/lib.sh

echo Installed git hash:
result=$(apdu_fixed '8000000000' | grep 'Clear bytearray' | cut -d"'" -f2)
python -c "print('$result'[3:].split('\x00')[0])"

