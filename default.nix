with import <nixpkgs> {}; rec {
     pinpogEnv = stdenv.mkDerivation {
        name = "monkey";
        buildInputs = [ gcc clang gdb readline ];
    };
}
