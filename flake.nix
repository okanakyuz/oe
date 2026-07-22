{
  description = "OE - Minimalist C Terminal Text Editor";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = f: nixpkgs.lib.genAttrs supportedSystems (system: f system);
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
        in
        {
          default = pkgs.stdenv.mkDerivation {
            pname = "oe";
            version = "0.0.1";

            src = ./.;

            nativeBuildInputs = with pkgs; [
              autoconf
              automake
              git
            ];

            preConfigure = ''
              ./autogen.sh || autoreconf -i
            '';

            meta = with pkgs.lib; {
              description = "A minimalist terminal text editor written in C";
              homepage = "https://github.com/okanakyuz/oe";
              license = licenses.mit;
              platforms = supportedSystems;
            };
          };
        });

      apps = forAllSystems (system: {
        default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/oe";
        };
      });
    };
}
