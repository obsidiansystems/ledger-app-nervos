#! /usr/bin/env nix-shell
#! nix-shell -i bash -p gitAndTools.hub coreutils

root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && git rev-parse --show-toplevel)"

build() {
  descr=$(git describe --tags --abbrev=8 --always --long --dirty 2>/dev/null)
  echo >&2 "Git description: $descr"
  exec nix-build "$root" --no-out-link --argstr gitDescribe "$descr" ${dontCheck} "$@" ${NIX_BUILD_ARGS:-}
}

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
