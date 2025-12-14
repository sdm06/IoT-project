{ pkgs ? import <nixpkgs> {} }:

(pkgs.buildFHSUserEnv {
  name = "azure-functions-env";
  
  # Packages to make available inside the "bubble"
  targetPkgs = pkgs: (with pkgs; [
    python3
    python3Packages.pip
    python3Packages.virtualenv
    
    azure-functions-core-tools
    azurite
    
    # Libraries that Python/Azure tools expect to find in /usr/lib
    stdenv.cc.cc.lib
    zlib
    openssl
    icu
    libunwind
    libuuid
  ]);

  # Command to run when entering the shell
  runScript = "bash";
}).env
