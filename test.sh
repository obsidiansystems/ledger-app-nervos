
export LEDGER_PROXY_ADDRESS=127.0.0.1 LEDGER_PROXY_PORT=9999
speculos_output_file="speculos.log"
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
      speculos_output_file=1
  esac
done

if [ -n "$LEDGER_PROXY_PORT" ] ; then
  speculos --display headless bin/app.elf --button-port 5667 --deterministic-rng 42 >& $speculos_output_file &
else
  # Should get this into the shell.
  # usbtool -v 0x2c97 log
  echo "Don't have usbtool"
fi

appPid=$!

sleep 1
echo "Starting bats"
bats -p tests
bats_result=$?
echo "Done with bats"

kill $appPid 2> /dev/null || :

exit $bats_result
