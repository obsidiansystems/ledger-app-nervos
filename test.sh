
speculos --display headless bin/app.elf --button-port 5667 --deterministic-rng 42 &
appPid=$!

sleep 1
echo "Starting bats"
bats -p tests
echo "Done with bats"

kill $appPid
