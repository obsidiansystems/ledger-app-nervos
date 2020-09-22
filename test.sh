export USE_SPECULOS=1
export SPECULOS_VERBOSE=0
export JOBS=8

while getopts "hsv" opt; do
  case $opt in
    h)
      USE_SPECULOS=0
      JOBS=1
      ;;
    s)
      USE_SPECULOS=1
      JOBS=8
      ;;
    v)
      SPECULOS_VERBOSE=1
      ;;
    *) ;;
  esac
done

echo "Starting bats"
if [ -n "$DEBUG" ] ; then echo "In debug mode (testing prompts)"; fi;
bats --jobs "$JOBS" -p tests/
bats_result=$?
echo "Done with bats"

grep "Free space between globals and maximum stack:" speculos.log | sort -t: -k2 -un | head -n1

exit $bats_result
