#! /usr/bin/env nix-shell
#! nix-shell -i bash ./ledgerblue.nix -A shell

set -Eeuo pipefail

root="$(git rev-parse --show-toplevel)"

# HELP=false
appDir=""
dontCheck=""

describe() {
  echo "install the ledger app"
}

build() {
  descr=$(git describe --tags --abbrev=8 --always --long --dirty 2>/dev/null)
  echo >&2 "Git description: $descr"
  exec nix-build "$root" --no-out-link --argstr gitDescribe "$descr" ${dontCheck} "$@" ${NIX_BUILD_ARGS:-}
}

install() {
  local release_file
  release_file=$(build -A "nano.${target}.release.app" "$@")
  bash "$root/release-installer.sh" "$release_file"
}

while getopts ":ht:x:n" opt; do
  case ${opt} in
    h ) describe
      ;;
    t ) 
        if [[ "$OPTARG" == "s" || "$OPTARG" == "x" ]]
        then
            target="$OPTARG"
        else
          echo "Please select x or s"
          exit 1
        fi
        echo $target
      ;;

    ## Optional app directory, to download from app.hex file
    x ) appDir=${OPTARG}
      ;;

    ## no-test: Don't run tests prior to installation
    n ) dontCheck="--arg runTest false"
      ;;

    \? ) echo "Usage: cmd [-h] [-t] [-x]"
      ;;
  esac
done
shift "$((OPTIND-1))"

if [[ -e $appDir ]] 
then
    bash "$root/release-installer.sh" "${appDir}"
else
    install "$@"
fi

