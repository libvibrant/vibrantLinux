{
  lib,
  stdenv,
  cmake,
  libvibrant,
  libxcb,
  libXrandr,
  linuxPackages,
  qtbase,
  qttools,
  wrapQtAppsHook,
}:

stdenv.mkDerivation (finalAttrs: {
  pname = "vibrantLinux";
  version = "2.1.10";

  src = lib.fileset.toSource {
    root = ../.;
    fileset = lib.fileset.unions (
      map (fileName: ../${fileName}) [
        "assets"
        "src"
        "CMakeLists.txt"
      ]
    );
  };

  nativeBuildInputs = [
    cmake
    wrapQtAppsHook
  ];

  buildInputs = [
    libvibrant
    libxcb
    libXrandr
    linuxPackages.nvidia_x11.settings.libXNVCtrl
    qtbase
    qttools
  ];

  meta = with lib; {
    description = "A tool to automate managing your screen's saturation depending on what programs are running";
    homepage = "https://github.com/libvibrant/vibrantLinux";
    license = licenses.gpl3;
    maintainers = with maintainers; [ Scrumplex ];
    platforms = platforms.linux;
    mainProgram = "vibrantLinux";
  };
})
