{ pkgs ? import ./nix/dep/nixpkgs {}, gitDescribe ? "TEST-dirty", nanoXSdk ? null, ... }:
let
  fetchThunk = p:
    if builtins.pathExists (p + /git.json)
      then pkgs.fetchgit { inherit (builtins.fromJSON (builtins.readFile (p + /git.json))) url rev sha256; }
    else if builtins.pathExists (p + /github.json)
      then pkgs.fetchFromGitHub { inherit (builtins.fromJSON (builtins.readFile (p + /github.json))) owner repo rev sha256; }
    else p;

  targets =
    {
      s = rec {
        name = "s";
        sdk = fetchThunk ./nix/dep/nanos-secure-sdk;
        env = pkgs.callPackage ./nix/bolos-env.nix { clangVersion = 4; };
        target = "TARGET_NANOS";
        targetId = "0x31100004";
        iconHex = pkgs.runCommand "nano-s-icon-hex" {
          nativeBuildInputs = [ (pkgs.python.withPackages (ps: [ps.pillow])) ];
        } ''
          python ${sdk + /icon.py} '${icons/nano-s-nervos.gif}' hexbitmaponly > "$out"
        '';
      };
      x = rec {
        name = "x";
        sdk = if nanoXSdk == null
          then throw "No NanoX SDK"
          else assert builtins.typeOf nanoXSdk == "path"; nanoXSdk;
        env = pkgs.callPackage ./nix/bolos-env.nix { clangVersion = 7; };
        target = "TARGET_NANOX";
        targetId = "0x33000004";
        iconHex = pkgs.runCommand "${name}-icon-hex" {
          nativeBuildInputs = [ (pkgs.python3.withPackages (ps: [ps.pillow])) ];
        } ''
          python '${sdk + /icon3.py}' --hexbitmaponly '${icons/nano-x-nervos.gif}' > "$out"
        '';
      };
    };

  src = pkgs.lib.sources.sourceFilesBySuffices (pkgs.lib.sources.cleanSource ./.) [".c" ".h" ".gif" "Makefile" ".sh" ".json"];

  build = bolos:
    let
      app = pkgs.stdenv.mkDerivation {
        name = "ledger-app-nervos-nano-${bolos.name}";
        inherit src;
        postConfigure = ''
          PATH="$BOLOS_ENV/clang-arm-fropi/bin:$PATH"
        '';
        nativeBuildInputs = [
          (pkgs.python3.withPackages (ps: [ps.pillow ps.ledgerblue]))
          pkgs.jq
        ];
        TARGET = bolos.target;
        GIT_DESCRIBE = gitDescribe;
        BOLOS_SDK = bolos.sdk;
        BOLOS_ENV = bolos.env;
        installPhase = ''
          mkdir -p $out
          cp -R bin $out
          cp -R debug $out

          echo
          echo ">>>> Application size: <<<<"
          size $out/bin/app.elf
        '';
      };
      nvramDataSize = appDir: pkgs.runCommand "nvram-data-size" {} ''
        envram_data="$(grep _envram_data '${appDir + /debug/app.map}' | tr -s ' ' | cut -f2 -d' ')"
        nvram_data="$(grep _nvram_data '${appDir + /debug/app.map}' | tr -s ' ' | cut -f2 -d' ')"
        echo "$(($envram_data - $nvram_data))" > "$out"
      '';
      mkRelease = short_name: name: appDir: pkgs.runCommand "${short_name}-nano-${bolos.name}-release-dir" {} ''
        mkdir -p "$out"

        cp '${appDir + /bin/app.hex}' "$out/app.hex"

        cat > "$out/app.manifest" <<EOF
        name='${name}'
        nvram_size=$(cat '${nvramDataSize appDir}')
        target='nano_${bolos.name}'
        target_id=${bolos.targetId}
        version=$(echo '${gitDescribe}' | cut -f1 -d- | cut -f2 -dv)
        icon_hex=$(cat '${bolos.iconHex}')
        EOF
      '';

      ledgerApp = app;
    in {
      inherit app;

      release = rec {
        app = mkRelease "nervos_wallet" "Nervos Wallet" ledgerApp;
        all = pkgs.runCommand "ledger-app-nervos-${bolos.name}.tar.gz" {} ''
          mkdir ledger-app-nervos-${bolos.name}

          cp -r ${app} ledger-app-nervos-${bolos.name}/app

          install -m a=rx ${./release-installer.sh} ledger-app-nervos-${bolos.name}/install.sh

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
          printf "<p>App: <code>"nervos_wallet"</code></p>"
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

  mkTargets = mk: {
    s = mk targets.s;
    x = mk targets.x;
  };
in rec {
  inherit pkgs;

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
  speculosPkgs = import pkgs.path {
    crossSystem = {
      config = "armv6l-unknown-linux-gnueabihf";
      #config = "armv6l-unknown-linux-musleabihf";
      #useLLVM = true;
      #platform = {
      #  gcc = {
      #    arch = "armv6t2";
      #    fpu = "vfpv2";
      #  };
      #};
    };
    config = { allowUnsupportedSystem = true; };
    overlays = [
      (self: super: {
        speculos = speculosPkgs.callPackage ({ stdenv, cmake, ninja, perl, pkg-config, openssl, cmocka }: stdenv.mkDerivation {
          name = "speculos";

          src = fetchThunk ./nix/dep/speculos;

          nativeBuildInputs = [ 
            cmake
            ninja
            perl
            pkg-config
          ];

          buildInputs = [
            openssl
            cmocka
          ];

          makeFlags = [ "emu" "launcher" ];

          #patches = [ ./speculos.patch ];
        }) {};
      })
    ];
  };
  inherit (speculosPkgs) speculos;
  # speculos = let
  #   opensslSrc = speculosPkgs.buildPackages.fetchurl {
  #     url = "https://www.openssl.org/source/openssl-1.1.1d.tar.gz";
  #     sha256 = "1e3a91bc1f9dfce01af26026f856e064eab4c8ee0a8f457b5ae30b40b8b711f2";
  #   };
  #   mockaSrc = speculosPkgs.buildPackages.fetchurl {
  #     url = "https://cmocka.org/files/1.1/cmocka-1.1.5.tar.xz";
  #     sha256 = "f0ccd8242d55e2fd74b16ba518359151f6f8383ff8aef4976e48393f77bba8b6";
  #   };
  #   mocka = speculosPkgs.stdenv.mkDerivation {
  #     name = "mocka";
  #     src = mockaSrc;
  #     nativeBuildInputs = [ speculosPkgs.buildPackages.cmake ];
  #   };
  # in
    
}
