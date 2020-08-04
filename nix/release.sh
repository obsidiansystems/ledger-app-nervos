#! /usr/bin/env nix-shell
#! nix-shell -i bash -p gitAndTools.hub coreutils

root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && git rev-parse --show-toplevel)"

dontTest=
nanoXSDK=
checkTag=true

build() {
  descr=$(git describe --tags --abbrev=8 --always --long --dirty 2>/dev/null)
  echo >&2 "Git description: $descr"
  exec nix-build "$root" --no-out-link --argstr gitDescribe "$descr" ${dontTest} ${nanoXSDK} "$@" ${NIX_BUILD_ARGS:-}
}

usage() {
  echo "release installer: creates tarballs and checksums from src"
  echo "usage: ./release.sh [options]"
  echo "options:"
  echo "  -n : do not run tests before build"
  echo "  -t : do not check that a git tag exists on HEAD that matches the current version number in the makefile before continuing"
  echo "  -x : path to optional nano X SDK. Cannot contain a trailing slash. Will fetch from github if not specified"
  exit 0
}

checkTagAgainstAppVersion() {
  local APP_VM=$(cat Makefile | sed -n -e 's/^.*APPVERSION_M=//p' | head -n 1)
  local APP_VN=$(cat Makefile | sed -n -e 's/^.*APPVERSION_N=//p' | head -n 1)
  local APP_VP=$(cat Makefile | sed -n -e 's/^.*APPVERSION_P=//p' | head -n 1)
  local tag=$(git tag --points-at HEAD)
  local expectedTag="v$APP_VM.$APP_VN.$APP_VP"
  if [[ tag != expectedTag ]]
  then
    echo "To proceed, HEAD must contain a tag matching the current application version: $expectedTag"
    echo "To disable this check, pass '-t'"
    exit 1
  fi
}

while getopts ":hntx:" opt; do
  case ${opt} in
    ## no-test: Don't run tests prior to installation
    n ) dontTest="--arg runTest false"
      ;;
    ## Check that the tag on HEAD matches the app version in the Makefile
    t ) checkTag=false
      ;;
    ## Optional path to nanoXSDK. If not specified, use the default one
    x ) nanoXSDK="--arg nanoXSdk $OPTARG"
      ;;
    h ) usage
      ;;
    \? ) usage
      ;;
  esac
done
shift "$((OPTIND-1))"

if [[ $checkTag == true ]]
then
  checkTagAgainstAppVersion
fi

nano_s_tarball=$(build -A "nano.s.release.all" "$@")
nano_x_tarball=$(build -A "nano.x.release.all" "$@")

cp -f $nano_s_tarball nano-s-release.tar.gz
cp -f $nano_x_tarball nano-x-release.tar.gz

# hub release create \
#   -a $nano_s_tarball'#'nano-s-release.tar.gz \
#   -a $nano_x_tarball'#'nano-x-release.tar.gz \
#   -F -

echo '## Checksums'
echo '### nano-s-release.tar.gz'
echo 'Type | Value'
echo '-- | --'
echo "MD5 | $(md5sum nano-s-release.tar.gz | cut -f1 -d' ')"
echo "SHA256 | $(sha256sum nano-s-release.tar.gz | cut -f1 -d' ')"
echo "SHA512 | $(sha512sum nano-s-release.tar.gz | cut -f1 -d' ')"

echo '### nano-x-release.tar.gz'
echo 'Type | Value'
echo '-- | --'
echo "MD5 | $(md5sum nano-x-release.tar.gz | cut -f1 -d' ')"
echo "SHA256 | $(sha256sum nano-x-release.tar.gz | cut -f1 -d' ')"
echo "SHA512 | $(sha512sum nano-x-release.tar.gz | cut -f1 -d' ')"
