#! /usr/bin/env nix-shell
#! nix-shell -i bash ./ledgerblue.nix -A shell

set -Eeuo pipefail

root="$(git rev-parse --show-toplevel)"

# HELP=false
appPkg=""
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
  bash "$root/nix/app-installer-impl.sh" "$release_file"
}

while getopts ":ht:x:n" opt; do
  case ${opt} in
    t ) 
    ## Required target arg: Specify x or s
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
    x ) appPkg=${OPTARG}
      ;;

    ## no-test: Don't run tests prior to installation
    n ) dontCheck="--arg runTest false"
      ;;

    h ) describe
      ;;
    \? ) echo "Usage: cmd [-h] [-t] [-x]"
      ;;
  esac
done
shift "$((OPTIND-1))"

if [[ $appPkg == *.tar.gz ]]
then
  appDir=$(tmp_dir=$(mktemp -d) && tar -zxf "$appPkg" -C "$tmp_dir" && echo "$tmp_dir")
  # this hack assumes that there is only one elem in $appDir
  ledgerDir="${appDir}/$(ls ${appDir})"
  bash "${ledgerDir}/install.sh" ${ledgerDir}/app
elif [[ -d $appPkg && -e $appPkg/app.hex && -e $appPkg/app.manifest ]]
then
  bash "$root/nix/app-installer-impl.sh" "${appDir}"
elif [[ -e $appPkg ]]
then
  echo "-x must contain a *.tar.gz file or an app/ directory containing an app.hex and app.manifest"
  exit 0
else
  ## INSTALL FROM SOURCE
    install "$@"
fi

