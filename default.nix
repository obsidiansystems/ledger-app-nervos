{ pkgs ? import ../nix/dep/nixpkgs {} }:
let
  yarn2nix = import deps/yarn2nix { inherit pkgs; };
  getThunkSrc = (import ./deps/reflex-platform { }).hackGet;
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
          };
        y = registries.yarn;
        s = self;
      in {
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

        "usb@1.6.3" = {
          key = super."usb@1.6.3".key;
          drv = super."usb@1.6.3".drv.overrideAttrs (attrs: {
            nativeBuildInputs = [ pkgs.python pkgs.systemd pkgs.v8_5_x pkgs.nodejs pkgs.libusb1 ];
            dontBuild = false;
            buildPhase = ''
              ln -s ${nixLib.linkNodeDeps { name=attrs.name; dependencies=attrs.passthru.nodeBuildInputs; }} node_modules
              ${pkgs.nodePackages.node-gyp}/bin/node-gyp rebuild --nodedir=${pkgs.lib.getDev pkgs.nodejs} # /include/node
            '';
          });
        };

        "node-hid@1.3.0" = {
          key = super."node-hid@1.3.0".key;
          drv = super."node-hid@1.3.0".drv.overrideAttrs (attrs: {
            nativeBuildInputs = [ pkgs.python pkgs.systemd pkgs.v8_5_x pkgs.nodejs pkgs.libusb1 pkgs.pkg-config ];
            dontBuild = false;
            buildPhase = ''
              ln -s ${nixLib.linkNodeDeps { name=attrs.name; dependencies=attrs.passthru.nodeBuildInputs; }} node_modules
              ${pkgs.nodePackages.node-gyp}/bin/node-gyp rebuild --nodedir=${pkgs.lib.getDev pkgs.nodejs} # /include/node
            '';
          });
        };

        "hw-app-avalanche@0.1.0" = super._buildNodePackage rec {
          key="hw-app-avalanche";
          version="0.1.0";
          src = getThunkSrc ./hw-app-avalanche;
          buildPhase = ''
          echo "NODE_GYP TYME"
          echo $nodeModules
          node $nodeModules/.bin/babel --source-maps -d lib src
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
    ( { src = nixLib.removePrefixes [ "node_modules" ] ./.; passthru = { inherit deps npmDepsNix npmPackageNix getThunkSrc;}; } //
      nixLib.callTemplate npmPackageNix
      (nixLib.buildNodeDeps (pkgs.lib.composeExtensions (pkgs.callPackage npmDepsNix {fetchgit=builtins.fetchGit;}) localOverrides)))
