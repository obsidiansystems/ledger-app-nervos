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

, rustOverlay ? import
    "${fetch (builtins.fromJSON (builtins.readFile ./nix/nixpkgs-mozilla/github.json))}/rust-overlay.nix"
    pkgs
    pkgs

# Rust manifest hash must be updated when rust-toolchain file changes.
, rustPackages ? rustOverlay.rustChannelOf {
    date = "2020-05-04";
    rustToolchain = (import ./ckb-cli-src/thunk.nix) + "/rust-toolchain";
    sha256="07mp7n4n3cmm37mv152frv7p9q58ahjw5k8gcq48vfczrgm5qgiy";
    # sha256 = "sha256-Pj5c6sufuY0IZg/NwiVUqOB0z85OlLDrGbWyYYk9tx4=";
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
  src = import ./ckb-cli-src/thunk.nix; # gitignoreSource (import ./ckb-cli-src/thunk.nix);
  nativeBuildInputs = [ pkgs.pkgconfig ];
  buildInputs = [ rustPackages.rust-std pkgs.openssl pkgs.libudev ];
  verifyCargoDeps = true;

  # Cargo hash must be updated when Cargo.lock file changes.
  cargoSha256 = "039bvmi7ai5zd3k46jfr1r82sfn84x4hg7c34sxsncwifd8p5yc6";
}
