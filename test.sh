
speculos --display headless bin/app.elf &
appPid=$!
sleep 1;
apdu() {
	LEDGER_PROXY_ADDRESS=127.0.0.1 LEDGER_PROXY_PORT=9999 python -m ledgerblue.runScript --apdu
}

echo "8000000000" | apdu

echo "8009000000" | apdu
sleep 1;
kill $appPid
