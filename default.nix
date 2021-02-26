{ pkgsFunc ? import ./nix/dep/nixpkgs
, gitDescribe ? "TEST-dirty"
, debug ? false
, runTest ? true
, ...
}:

let
  pkgs = pkgsFunc {
    config = {};
    overlays = [
      (import "${fetchThunk ./nix/dep/nixpkgs-mozilla}/rust-overlay.nix")
    ];
  };

  ledgerPkgs = pkgsFunc {
    crossSystem = {
      config = "armv6l-unknown-none-eabi";
      #useLLVM = true;
      platform = {
        gcc = {
          arch = "armv6t2";
          fpu = "vfpv2";
        };
        rustc = {
          arch = "thumbv6m";
          config = "thumbv6m-none-eabi";
        };
      };
    };
    overlays = [
      (import "${fetchThunk ./nix/dep/nixpkgs-mozilla}/rust-overlay.nix")
    ];
  };

  # TODO: Replace this with hackGet for added safety checking once hackGet is separated from reflex-platform
  fetchThunk = p:
    if builtins.pathExists (p + /thunk.nix)
      then (import (p + /thunk.nix))
    else p;

  blake2_simd = import ./nix/dep/b2sum.nix { };

  usbtool = import ./nix/usbtool.nix { };

  patchSDKBinBash = name: sdk: pkgs.stdenv.mkDerivation {
    # Replaces SDK's Makefile instances of /bin/bash with /bin/sh
    name =  name + "_patched_bin_bash";
    src = sdk;
    dontBuild = true;
    installPhase = ''
      mkdir -p $out
      cp -a $src/. $out
      substituteInPlace $out/Makefile.rules_generic --replace /bin/bash /bin/sh
    '';
  };
  targets =
    {
      s = rec {
        name = "s";
        sdk = patchSDKBinBash "nanos-secure-sdk" (fetchThunk ./nix/dep/nanos-secure-sdk);
        env = pkgs.callPackage ./nix/bolos-env.nix { clangVersion = 4; };
        target = "TARGET_NANOS";
        targetId = "0x31100004";
        test = true;
        iconHex = pkgs.runCommand "nano-s-icon-hex" {
          nativeBuildInputs = [ (pkgs.python.withPackages (ps: [ps.pillow])) ];
        } ''
          python ${sdk + /icon.py} '${icons/nano-s-nervos.gif}' hexbitmaponly > "$out"
        '';
      };
      x = rec {
        name = "x";
        sdk = patchSDKBinBash "ledger-nanox-sdk" (fetchThunk ./nix/dep/ledger-nanox-sdk);
        env = pkgs.callPackage ./nix/bolos-env.nix { clangVersion = 7; };
        target = "TARGET_NANOX";
        targetId = "0x33000004";
        test = false;
        iconHex = pkgs.runCommand "${name}-icon-hex" {
          nativeBuildInputs = [ (pkgs.python3.withPackages (ps: [ps.pillow])) ];
        } ''
          python '${sdk + /icon3.py}' --hexbitmaponly '${icons/nano-x-nervos.gif}' > "$out"
        '';
      };
    };

  gitignoreNix = import (fetchThunk ./nix/dep/gitignore.nix) { inherit (pkgs) lib; };

  inherit (gitignoreNix) gitignoreSource;

  gitIgnoredSrc = gitignoreSource ./.;

  src = pkgs.lib.sources.sourceFilesBySuffices gitIgnoredSrc [
    ".c" ".h" ".gif" "Makefile" ".sh" ".json" ".js" ".bats" ".txt" ".der"
  ];

  speculos = pkgs.callPackage ./nix/dep/speculos { };
  tests = import ./tests { inherit pkgs; };

  rust-bindgen = (ledgerPkgs.buildPackages.rust-bindgen.override {
    inherit rustPlatform;
  }).overrideAttrs (_: {
    doCheck = false;
  });

  rust-sdk-bindings-src = gitignoreSource ./rust-sdk-bindings;

  build = bolos:
    let
      rust-bindings = ledgerPkgs.runCommand "${bolos.name}-rust-bindings" {
        nativeBuildInputs = [
          rust-bindgen
          rustPackages.rust
          rustPackages.rust-std
        ];
      } ''
        mkdir "$out"
        TARGET=thumbv6m-none-eabi \
        bindgen ${rust-sdk-bindings-src}/wrapper.h \
          --use-core \
          --no-prepend-enum-name \
          --no-doc-comments \
          --with-derive-default \
          --no-derive-debug \
          --ctypes-prefix=cty \
          -- \
          -fshort-enums \
          -I${bolos.sdk}/lib_ux/include \
          -I${bolos.sdk}/include \
          -I${rust-sdk-bindings-src}/include \
          > "$out/bindings.rs"
      '';

      rust-src = gitignoreSource ./rust;

      rustBits = ledgerRustPlatform.buildRustPackage {
        name = "${bolos.name}-nervos-app-rs";
        src = rust-src;
        preConfigure = ''
          cp ${rust-bindings}/bindings.rs src/
        '';
        #nativeBuildInputs = [ pkgs.pkgconfig ];
        buildInputs = [ rustPackages.rust-std ];
        verifyCargoDeps = true;
        target = "thumbv6m-none-eabi";

        # Cargo hash must be updated when Cargo.lock file changes.
        cargoSha256 = "1kdg77ijbq0y1cwrivsrnb9mm4y5vlj7hxn39fq1dqlrppr6fdrr";

        # It is more reliable to trick a stable rustc into doing unstable features
        # than use an unstable nightly rustc. Just because we want unstable
        # langauge features doesn't mean we want a less tested implementation!
        RUSTC_BOOTSTRAP = 1;

        meta = {
          platforms = pkgs.lib.platforms.all;
        };
      };
      app = pkgs.stdenv.mkDerivation {
        name = "ledger-app-nervos-nano-${bolos.name}";
        inherit src;
        postConfigure = ''
          PATH="$BOLOS_ENV/clang-arm-fropi/bin:$PATH"
        '';
        nativeBuildInputs = [
          (pkgs.python3.withPackages (ps: [ps.pillow ps.ledgerblue]))
          pkgs.jq
          speculos.speculos
          usbtool
          bolos.env.clang

          # Test harness
          tests
          pkgs.nodejs
          pkgs.gdb
          pkgs.python2
          pkgs.entr
          pkgs.yarn
        ];
        buildInputs = [
          rustBits
        ];
        rustLibs="${rustBits}/lib";
        NIX_NO_SELF_RPATH=1;
        TARGET = bolos.target;
        GIT_DESCRIBE = gitDescribe;
        BOLOS_SDK = bolos.sdk;
        BOLOS_ENV = bolos.env;
        DEBUG=if debug then "1" else "0";
        installPhase = ''
          mkdir -p $out
          cp -R bin $out
          cp -R debug $out

          echo
          echo ">>>> Application size: <<<<"
          size $out/bin/app.elf
        '';

        doCheck = if runTest then bolos.test else false;
        checkTarget = "test";
      };
      ## Note: This has been known to change between sdk upgrades. Make sure to consult
      ## the $COMMON_LOAD_PARAMS in the Makefile.defines of both SDKs
        nvramDataSize = appDir: deviceName:
          let mapPath = appDir + /debug/app.map;
          in pkgs.runCommand "nvram-data-size" {} ''
            nvram_data=0x${ if deviceName == "s"
              then "$(grep _nvram_data "+ mapPath + " | tr -s ' ' | cut -f2 -d' ' | cut -f2 -d'x')"
              else "$(grep _nvram_data "+ mapPath + " | cut -f1 -d' ')"
            }
            envram_data=0x${ if deviceName == "s"
              then "$(grep _envram_data "+ mapPath + " | tr -s ' ' | cut -f2 -d' '| cut -f2 -d'x')"
              else "$(grep _envram_data "+ mapPath + " | cut -f1 -d' ')"
            }
            echo "$(($envram_data - $nvram_data))" > "$out"
          '';
      mkRelease = short_name: name: appDir: pkgs.runCommand "${short_name}-nano-${bolos.name}-release-dir" {} ''
        mkdir -p "$out"

        cp '${appDir + /bin/app.hex}' "$out/app.hex"

        cat > "$out/app.manifest" <<EOF
        name='${name}'
        nvram_size=$(cat '${nvramDataSize appDir bolos.name}')
        target='nano_${bolos.name}'
        target_id=${bolos.targetId}
        version=$(echo '${gitDescribe}' | cut -f1 -d- | cut -f2 -dv)
        icon_hex=$(cat '${bolos.iconHex}')
        EOF
      '';

      ledgerApp = app;
    in {
      inherit app;

      rust = {
        inherit rustBits rust-bindings;
      };

      release = rec {
        app = mkRelease "nervos" "Nervos" ledgerApp;
        all = pkgs.runCommand "ledger-app-nervos-${bolos.name}.tar.gz" {} ''
          mkdir ledger-app-nervos-${bolos.name}

          cp -r ${app} ledger-app-nervos-${bolos.name}/app

          install -m a=rx ${./nix/app-installer-impl.sh} ledger-app-nervos-${bolos.name}/install.sh

          tar czf $out ledger-app-nervos-${bolos.name}/*
        '';
      };
    };

  # The package clang-analyzer comes with a perl script `scan-build` that seems
  # to get quickly lost with the cross-compiler of the SDK if run by itself.
  # So this script reproduces what it does with fewer magic attempts:
  # * It prepares the SDK like for a normal build.
  # * It intercepts the calls to the compiler with the `CC` make-variable
  #   (pointing at `.../libexec/scan-build/ccc-analyzer`).
  # * The `CCC_*` variables are used to configure `ccc-analyzer`: output directory
  #   and which *real* compiler to call after doing the analysis.
  # * After the build an `index.html` file is created to point to the individual
  #   result pages.
  #
  # See
  # https://clang-analyzer.llvm.org/alpha_checks.html#clone_alpha_checkers
  # for the list of extra analyzers that are run.
  #
  runClangStaticAnalyzer =
     let
       interestingExtrasAnalyzers = [
         # "alpha.clone.CloneChecker" # this one is waaay too verbose
         "alpha.security.ArrayBound"
         "alpha.security.ArrayBoundV2"
         "alpha.security.MallocOverflow"
         # "alpha.security.MmapWriteExec" # errors as “not found” by ccc-analyzer
         "alpha.security.ReturnPtrRange"
         "alpha.security.taint.TaintPropagation"
         "alpha.deadcode.UnreachableCode"
         "alpha.core.CallAndMessageUnInitRefArg"
         "alpha.core.CastSize"
         "alpha.core.CastToStruct"
         "alpha.core.Conversion"
         # "alpha.core.FixedAddr" # Seems noisy, and about portability.
         "alpha.core.IdenticalExpr"
         "alpha.core.PointerArithm"
         "alpha.core.PointerSub"
         "alpha.core.SizeofPtr"
         # "alpha.core.StackAddressAsyncEscape" # Also not found
         "alpha.core.TestAfterDivZero"
         "alpha.unix.cstring.BufferOverlap"
         "alpha.unix.cstring.NotNullTerminated"
         "alpha.unix.cstring.OutOfBounds"
       ];
       analysisOptions =
          pkgs.lib.strings.concatMapStringsSep
             " "
             (x: "-analyzer-checker " + x)
             interestingExtrasAnalyzers;
     in bolos: ((build bolos).app).overrideAttrs (old: {
       CCC_ANALYZER_HTML = "${placeholder "out"}";
       CCC_ANALYZER_OUTPUT_FORMAT = "html";
       CCC_ANALYZER_ANALYSIS = analysisOptions;
       CCC_CC = "${bolos.env}/clang-arm-fropi/bin/clang";
       CLANG = "${bolos.env}/clang-arm-fropi/bin/clang";
       preBuild = ''
         mkdir -p $out
       '';
       makeFlags = old.makeFlags or []
         ++ [ "CC=${pkgs.clangAnalyzer}/libexec/scan-build/ccc-analyzer" ];
       installPhase = ''
        {
          echo "<html><title>Analyzer Report</title><body><h1>Clang Static Analyzer Results</h1>"
          printf "<p>App: <code>"nervos"</code></p>"
          printf "<h2>File-results:</h2>"
          for html in "$out"/report*.html ; do
            echo "<p>"
            printf "<code>"
            grep BUGFILE "$html" | sed 's/^<!-- [A-Z]* \(.*\) -->$/\1/'
            printf "</code>"
            printf "<code style=\"color: green\">+"
            grep BUGLINE "$html" | sed 's/^<!-- [A-Z]* \(.*\) -->$/\1/'
            printf "</code><br/>"
            grep BUGDESC "$html" | sed 's/^<!-- [A-Z]* \(.*\) -->$/\1/'
            printf " → <a href=\"./%s\">full-report</a>" "$(basename "$html")"
            echo "</p>"
          done
          echo "</body></html>"
        } > "$out/index.html"
      '';
     });

  rustPackages = pkgs.rustChannelOf {
    date = "2020-01-30"; # 1.41
    channel = "stable";
    sha256 = "07mp7n4n3cmm37mv152frv7p9q58ahjw5k8gcq48vfczrgm5qgiy";
  };

  rustc = rustPackages.rust.override {
    targets = [
      "thumbv6m-none-eabi"
    ];
  };

  rustPlatform = pkgs.makeRustPlatform {
    inherit (rustPackages) cargo;
    inherit rustc;
  };

  ledgerRustPlatform = ledgerPkgs.makeRustPlatform {
    inherit (rustPackages) cargo;
    inherit rustc;
  };

  mkTargets = mk: {
    s = mk targets.s;
    x = mk targets.x;
  };
in rec {
  inherit pkgs ledgerPkgs;

  nano = mkTargets build;

  wallet = {
    s = nano.s;
    x = nano.x;
  };

  clangAnalysis = mkTargets (bolos: {
    wallet = runClangStaticAnalyzer bolos;
  });

  env = mkTargets (bolos: {
    ide = {
      config = {
        vscode = pkgs.writeText "vscode-nano-${bolos.name}.code-workspace" (builtins.toJSON {
          folders = [ { path = "."; } ];
          settings = {
            "clangd.path" = pkgs.llvmPackages.clang-unwrapped + /bin/clangd;
          };
        });
      };
    };

    inherit (bolos.env) clang gcc;
    inherit (bolos) sdk;
  });
  inherit speculos;
}
