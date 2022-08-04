{ alamgu ? import ./nix/dep/alamgu {}
, gitDescribe ? "TEST-dirty"
, debug ? false
, runTest ? true
, ...
}:

let
  # TODO: Rename
  fetchThunk = alamgu.thunkSource;

  inherit (alamgu)
    pkgs ledgerPkgs
    gitignoreNix gitignoreSource
    usbtool
    speculos;

  inherit (pkgs) lib;

  blake2_simd = import ./nix/dep/b2sum.nix { };

  patchSDKBinBash = name: sdk: pkgs.stdenv.mkDerivation {
    # Replaces SDK's Makefile instances of /bin/bash with /bin/sh
    name = name + "_patched_bin_bash";
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
        target = "TARGET_NANOS";
        targetId = "0x31100004";
        test = true;
        iconHex = pkgs.runCommand "nano-s-icon-hex" {
          nativeBuildInputs = [ (pkgs.python3.withPackages (ps: [ps.pillow])) ];
        } ''
          python ${sdk + /icon.py} '${icons/nano-s-nervos.gif}' hexbitmaponly > "$out"
        '';
      };
      x = rec {
        name = "x";
        sdk = patchSDKBinBash "ledger-nanox-sdk" (fetchThunk ./nix/dep/ledger-nanox-sdk);
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

  gitIgnoredSrc = gitignoreSource ./.;

  makeFilterPass0 = dirs: bad: lib.sources.cleanSourceWith {
    src = gitIgnoredSrc;
    filter = p: _: let
      p' = baseNameOf p;
      srcStr = builtins.toString ./.;
    in p' != "glyphs.c" && p' != "glyphs.h"
      && (p == (srcStr + "/Makefile")
          || lib.any (dir: lib.hasPrefix (srcStr + "/" + dir) p) dirs
          && bad srcStr p
         );
  };

  src0 = makeFilterPass0
    ["src" "glyphs" "tests"]
    (_: _: true);

  src = lib.sources.sourceFilesBySuffices src0 [
    ".c" ".h" ".gif" "Makefile" ".sh" ".json" ".js" ".bats" ".txt" ".der"
  ];

  src0-fuzzing = makeFilterPass0
    ["fuzzing"]
    (srcStr: p: !lib.hasPrefix (srcStr + "/fuzzing/build") p);

  src-fuzzing = lib.sources.sourceFilesBySuffices src0-fuzzing [
    ".c" ".txt" ".sh"
  ];

  tests = import ./tests { inherit alamgu; };

  build = bolos:
    let
      # We want GNU as not LLVM as, which doesn't something completely
      # different, or Clang's internal assembler which doesn't take quite the
      # same syntax. It also expects a C compiler to not complain about extra
      # args, do C pre-processing, etc. We could get clang to use GNU as, but
      # we'll just use GCC as they presumably do for this.
      setAssembler = ''
        export AS=${ledgerPkgs.gccStdenv.cc}/bin/${ledgerPkgs.clangStdenv.cc.targetPrefix}gcc
      '';

      app = ledgerPkgs.lldClangStdenv.mkDerivation {
        name = "ledger-app-nervos-nano-${bolos.name}";
        inherit src;
        postConfigure = ''
          # hack to get around no tests for cross logic
          doCheck=${toString (if runTest then bolos.test else false)};
          export USE_NIX=1
        '';
        nativeBuildInputs = [
          (pkgs.python3.withPackages (ps: [ps.pillow ps.ledgerblue]))
          pkgs.jq
          speculos.speculos
          usbtool

          # Test harness
          tests
          pkgs.nodejs
          pkgs.gdb
          pkgs.entr
          pkgs.yarn
        ];
        TARGET = bolos.target;
        GIT_DESCRIBE = gitDescribe;
        BOLOS_SDK = bolos.sdk;
        # note trailing slash
        GCCPATH = "${ledgerPkgs.stdenv.cc}/bin/";
        DEBUG=if debug then "1" else "0";

        # Do both ways, so nix builds and users running make both work.
        preBuild = setAssembler;
        shellHook = ''
          export USE_NIX=1
        '' + setAssembler;

        installPhase = ''
          mkdir -p $out
          cp -R bin $out
          cp -R debug $out

          echo
          echo ">>>> Application size: <<<<"
          $SIZE $out/bin/app.elf
        '';

        checkTarget = "test";
        enableParallelBuilding = true;
      };
      fuzzing = pkgs.clangStdenv.mkDerivation {
        name = "ledger-app-nervos-nano-${bolos.name}-fuzzing";
        inherit src;
        postUnpack = ''
          set -x
          sourceRoot+=/fuzzing
          cp -R --no-preserve=mode "${src-fuzzing}/fuzzing" "$sourceRoot"
          set +x
        '';
        nativeBuildInputs = with pkgs.buildPackages; [
          (pkgs.python3.withPackages (ps: [ps.pillow]))
          cmake
        ];
        BOLOS_SDK = bolos.sdk;
      };
      ## Note: This has been known to change between sdk upgrades. Make sure to consult
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
      inherit app fuzzing;

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
  #   (pointing at `.../libexec/ccc-analyzer`).
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
       preBuild = (old.preBuild or "") + ''
         mkdir -p $out
         export CCC_CC=$CC
         export CCC_CXX=$CXX
       '';
       makeFlags = old.makeFlags or []
         ++ [ "CC=${pkgs.clangAnalyzer}/libexec/ccc-analyzer" ];
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

  mkTargets = mk: {
    s = mk targets.s;
    x = mk targets.x;
  };
in rec {
  inherit
    pkgs
    usbtool;

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

    inherit (bolos) sdk;
  });
  inherit speculos;
}
