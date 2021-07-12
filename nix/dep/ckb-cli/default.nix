{ pkgs ? import (builtins.fetchTarball { # 2020-02-13 (nixos-19.09)
    url = "https://github.com/NixOS/nixpkgs/archive/e02fb6eaf70d4f6db37ce053edf79b731f13c838.tar.gz";
    sha256 = "1dbjbak57vl7kcgpm1y1nm4s74gjfzpfgk33xskdxj9hjphi6mws";
  }) {}

, fetch ? { private ? false, fetchSubmodules ? false, owner, repo, rev, sha256, ... }:
    if !fetchSubmodules && !private then builtins.fetchTarball {
      url = "https://github.com/${owner}/${repo}/archive/${rev}.tar.gz"; inherit sha256;
    } else (import <nixpkgs> {}).fetchFromGitHub {
      inherit owner repo rev sha256 fetchSubmodules private;
    }
, thunkSource ? (import ../nix-thunk {inherit pkgs;}).thunkSource

, rustOverlay ? import
    "${fetch (builtins.fromJSON (builtins.readFile ./nix/nixpkgs-mozilla/github.json))}/rust-overlay.nix"
    pkgs
    pkgs

# Rust manifest hash must be updated when rust-toolchain file changes.
, rustPackages ? rustOverlay.rustChannelOf {
    date = "2020-05-04";
    rustToolchain = (thunkSource ./ckb-cli-src) + "/rust-toolchain";
    sha256="0yvh2ck2vqas164yh01ggj4ckznx04blz3jgbkickfgjm18y269j";
  }

, gitignoreNix ? fetch (builtins.fromJSON (builtins.readFile ./nix/gitignore.nix/github.json))

}:

let
  rustPlatform = pkgs.makeRustPlatform {
    inherit (rustPackages) cargo;
    rustc = rustPackages.rust;
  };
  inherit (import gitignoreNix { inherit (pkgs) lib; }) gitignoreSource;
in rustPlatform.buildRustPackage {
  name = "ckb-cli";
  src = thunkSource ./ckb-cli-src; # import ./ckb-cli-src/thunk.nix; # gitignoreSource (import ./ckb-cli-src/thunk.nix);
  nativeBuildInputs = [ pkgs.pkgconfig ];
  buildInputs = [ rustPackages.rust-std pkgs.openssl pkgs.libudev ];
  verifyCargoDeps = true;

  # Cargo hash must be updated when Cargo.lock file changes.
  cargoSha256 = "1wb8bqrhm14dzz8f2hkhjmc9bd4gzb80hnjgfbgsfh7n0gn0zfpa";
}
