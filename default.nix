with import <nixpkgs> {}; rec {
     pinpogEnv = stdenv.mkDerivation {
        name = "monkey";
        buildInputs = [ clang gdb readline ];
    };
}
