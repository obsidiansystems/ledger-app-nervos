{nixpkgs ? import ./nixpkgs { }}:
with nixpkgs;
rustPlatform.buildRustPackage rec {
  pname = "b2sum";
  version = "0.5.10";
  src = fetchFromGitHub {
    owner = "obsidiansystems";
    repo = "blake2_simd";
    rev = "a0eafd0f86d8fd4f50720e198116dfdea6fbd576";
    sha256 = "08f922c2j88a076j2w869xzh6sw2xy1aaim68s5wx01dlcw6wrpm";
  };
  sourceRoot = "source/blake2_bin";
  cargoSha256 = "14yvl2xg9qdksld1zmmkzakma045kyp4q8187yxfvqsq4m4ys9kk";
  buildInputs = [ openssl pkgconfig ];
}
