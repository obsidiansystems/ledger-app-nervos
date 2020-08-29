{ pkgs }:
let
  yarn2nix = import deps/yarn2nix { inherit pkgs; };
  hackGet = (pkgs.lib.fix (import deps/hack-get { lib=pkgs.lib; } ) ).hackGet;
  npmDepsNix = pkgs.runCommand "npm-deps.nix" {} ''
    ${yarn2nix}/bin/yarn2nix --offline ${./yarn.lock} > $out
  '';
  npmPackageNix = pkgs.runCommand "npm-package.nix" {} ''
    # We sed hw-app-avalanche to a constant here, so that the package.json can be whatever; we're overriding it anyways.
    ${yarn2nix}/bin/yarn2nix --template <(sed 's/hw-app-avalanche".*$/hw-app-avalanche": "0.1.0",/' ${./package.json}) > $out
  '';
  nixLib = yarn2nix.nixLib;

  localOverrides = self: super:
      let
        registries = {
          yarn = n:
            v:
              "https://registry.yarnpkg.com/${n}/-/${n}-${v}.tgz";
          npm = n:
            v:
              "https://registry.npmjs.org/${n}/-/${n}-${v}.tgz";
          };
        nodeFilePackage = key:
          version:
            registry:
              sha1:
                deps:
                  super._buildNodePackage {
                    inherit key version;
                    src = pkgs.fetchurl {
                      url = registry key version;
                      inherit sha1;
                      };
                    nodeBuildInputs = deps;
                    };
        nodeFileLocalPackage = key:
          version:
            path:
              sha1:
                deps:
                  super._buildNodePackage {
                    inherit key version;
                    src = builtins.path { inherit path; };
                    nodeBuildInputs = deps;
                    };
        nodeGitPackage = key:
          version:
            url:
              rev:
                sha256:
                  deps:
                    super._buildNodePackage {
                      inherit key version;
                      src = pkgs.fetchgit { inherit url rev; };
                      nodeBuildInputs = deps;
                      };
        identityRegistry = url:
          _:
            _:
              url;
        scopedName = scope:
          name:
            { inherit scope name; };
        ir = identityRegistry;
        l = nodeFileLocalPackage;
        f = nodeFilePackage;
        g = nodeGitPackage;
        n = registries.npm;
        y = registries.yarn;
        sc = scopedName;
        s = self;
      in {
        #"bcrypto@5.3.0" = (let sup=super."bcrypto@5.3.0"; in {
        #  key = sup.key;
        #  drv = sup.drv.overrideAttrs (attrs: {
        #    buildPhase = ''
        #      ${pkgs.nodePackages.node-gyp}/bin/node-gyp rebuild --nodedir=${pkgs.lib.getDev pkgs.nodejs} # /include/node
        #    '';
        #  });}
        #  );

        "bcrypto@5.3.0" = super._buildNodePackage {
          key="bcrypto";
          version="5.3.0";
          src = pkgs.fetchurl {
            url = y "bcrypto" "5.3.0";
            sha1 = "d2d7d8a808b5efeb09fe529034a30bd772902d84";
          };
          buildPhase = ''
          ${pkgs.nodePackages.node-gyp}/bin/node-gyp rebuild --nodedir=${pkgs.lib.getDev pkgs.nodejs} # /include/node
          '';
         nativeBuildInputs = [ pkgs.python ];
          nodeBuildInputs = [
            (s."bufio@~1.0.7")
            (s."loady@~0.0.5")
          ];
        };

        "hw-app-avalanche@0.1.1" = {
          key = super."hw-app-avalanche@0.1.0".key;
          drv = super."hw-app-avalanche@0.1.0".drv.overrideAttrs (attrs: {
            nodeModules=nixLib.linkNodeDeps { name = "hw-app-avalanche"; dependencies = attrs.nodeBuildInputs; };
          });
        };

        "hw-app-avalanche@0.1.0" = super._buildNodePackage rec {
          key="hw-app-avalanche";
          version="0.1.0";
          src = hackGet ./hw-app-avalanche;
          buildPhase = ''
          echo "NODE_GYP TYME"
          echo $nodeModules
          node $nodeModules/.bin/babel --source-maps -d lib src
          # ${s."flow-remove-types@2.132.0".drv}/flow-remove-types src -d lib
          # ${pkgs.nodePackages.node-gyp}/bin/node-gyp rebuild --nodedir=${pkgs.lib.getDev pkgs.nodejs} # /include/node
          '';
          nodeModules=nixLib.linkNodeDeps { name = "hw-app-avalanche"; dependencies = nodeBuildInputs; };
          passthru={ inherit nodeModules; };
          NODE_PATH=nodeModules;
          nodeBuildInputs = [
            (s."@ledgerhq/hw-transport@^5.9.0")
            (s."bip32-path@0.4.2")
          (s."babel-cli@^6.26.0")
          (s."babel-eslint@^8.0.2")
          (s."babel-preset-env@^1.7.0")
          (s."babel-preset-flow@^6.23.0")
          (s."babel-preset-react@^6.24.1")
          (s."babel-preset-stage-0@^6.24.1")
          (s."flow-bin@^0.109.0")
          (s."flow-copy-source@^2.0.8")
          (s."flow-mono-cli@^1.5.0")
          (s."flow-typed@^2.6.1")
          (s."prettier@^1.18.2")
          (s."uglify-js@^3.6.1")
          ];
        };
      };
  deps = nixLib.buildNodeDeps (pkgs.lib.composeExtensions (pkgs.callPackage npmDepsNix {fetchgit=builtins.fetchGit;}) localOverrides);
in
  nixLib.buildNodePackage
    ( { src = nixLib.removePrefixes [ "node_modules" ] ./.; passthru = { inherit deps npmDepsNix npmPackageNix hackGet;}; } //
      nixLib.callTemplate npmPackageNix
      (nixLib.buildNodeDeps (pkgs.lib.composeExtensions (pkgs.callPackage npmDepsNix {fetchgit=builtins.fetchGit;}) localOverrides)))
