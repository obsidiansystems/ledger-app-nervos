
export LEDGER_PROXY_ADDRESS=127.0.0.1 LEDGER_PROXY_PORT=9999
speculos_output_file="speculos.log"

writeFile() {
  cat > $1
}
speculos_output_cmd="writeFile"

while getopts "hsv" opt; do
  case $opt in
    h)
      unset LEDGER_PROXY_ADDRESS
      unset LEDGER_PROXY_PORT
      ;;
    s)
      export LEDGER_PROXY_ADDRESS=127.0.0.1 LEDGER_PROXY_PORT=9999
      ;;
    v)
      speculos_output_cmd="tee"
      ;;
    *) ;;
  esac
done

killSpeculos() {
  { echo rRrRrRrRrlRLrRrRrRrRrlRLrlRL > /dev/tcp/localhost/5667; } > /dev/null 2>&1 || :
  sleep 0.3
  kill $appPid >& /dev/null
}

if [ -n "$LEDGER_PROXY_PORT" ] ; then
  speculos --display headless bin/app.elf --button-port 5667 --deterministic-rng 42 |& $speculos_output_cmd $speculos_output_file &
  trap killSpeculos EXIT
else
  usbtool -v 0x2c97 log
fi

appPid=$!

sleep 1
echo "Starting bats"
if [ -n "$DEBUG" ] ; then echo "In debug mode (testing prompts)"; fi;
bats -p tests/
bats_result=$?
echo "Done with bats"

exit $bats_result
