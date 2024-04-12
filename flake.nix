{
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs =
    { nixpkgs, ... }:
    let
      forAllSystems =
        function:
        nixpkgs.lib.genAttrs [
          "x86_64-linux"
          "aarch64-linux"
        ] (system: function nixpkgs.legacyPackages.${system});
    in
    {
      packages = forAllSystems (pkgs: rec {
        default = vibrantlinux;
        vibrantlinux = pkgs.vibrantlinux.overrideAttrs (prev: {
          nativeBuildInputs = prev.nativeBuildInputs or [ ] ++ [ pkgs.cmake ];
        });
      });

      formatter = forAllSystems (pkgs: pkgs.nixfmt-rfc-style);
    };
}
