{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    git-hooks.url = "github:cachix/git-hooks.nix";
    git-hooks.inputs.nixpkgs.follows = "nixpkgs";
  };

  outputs =
    {
      git-hooks,
      nixpkgs,
      self,
      ...
    }:
    let
      forAllSystems =
        function:
        nixpkgs.lib.genAttrs [
          "x86_64-linux"
          "aarch64-linux"
        ] (system: function nixpkgs.legacyPackages.${system} system);
    in
    {
      packages = forAllSystems (
        pkgs: _: rec {
          default = vibrantlinux;
          vibrantlinux = pkgs.qt6Packages.callPackage ./nix/package.nix { };
        }
      );

      checks = forAllSystems (
        pkgs: system: {
          pre-commit-check = git-hooks.lib.${system}.run {
            src = ./.;
            hooks = {
              clang-format = {
                enable = true;
                types_or = [
                  "c"
                  "c++"
                ];
              };
              nixfmt = {
                enable = true;
                package = self.formatter.${system};
              };
            };
          };
        }
      );

      devShells = forAllSystems (
        pkgs: system: {
          default = pkgs.mkShell {
            inherit (self.checks.${system}.pre-commit-check) shellHook;
            buildInputs = self.checks.${system}.pre-commit-check.enabledPackages;

            inputsFrom = [ self.packages.${system}.vibrantlinux ];
          };
        }
      );

      formatter = forAllSystems (pkgs: _: pkgs.nixfmt-rfc-style);
    };
}
