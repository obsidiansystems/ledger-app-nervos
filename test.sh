
speculos --display headless bin/app.elf &
appPid=$!

sleep 1
echo "Starting bats"
bats -p tests
echo "Done with bats"

kill $appPid
