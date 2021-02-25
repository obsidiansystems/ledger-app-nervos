#! /usr/bin/env nix-shell
#! nix-shell -i bash -A wallet.s

apdu() {
  python -m ledgerblue.runScript --apdu
}

apdu_fixed () {
  echo "$*" | apdu | sed 's/HID //'
}

#echo Installed version:
#apdu_fixed "8000000000"
echo Installed git hash:
apdu_fixed "8009000000" | grep "Clear bytearray" | cut -d"'" -f2 | cut -d'\' -f1

