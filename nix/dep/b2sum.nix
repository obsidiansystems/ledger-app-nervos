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
  cargoSha256 = "0n8qrljh8q5fcg79gf7gb5a2q63i8dqcn4486fh95ss5sgy5isfh";
  buildInputs = [ openssl pkgconfig ];
}
