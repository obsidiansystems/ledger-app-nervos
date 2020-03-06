
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

