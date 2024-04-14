# vibrantLinux
<a href="https://repology.org/project/vibrantlinux/versions">
    <img src="https://repology.org/badge/vertical-allrepos/vibrantlinux.svg" alt="Packaging status" align="right">
</a>

<a href='https://flathub.org/apps/details/io.github.libvibrant.vibrantLinux'><img width='240' alt='Download on Flathub' src='https://flathub.org/assets/badges/flathub-badge-en.png'/></a>

Main Program:

![Main program](assets/program.png)

Program entry editor:

![Program vibrance editor](assets/entryeditor.png)

## Overview

vibrantLinux is a GUI tool to serves the same purpose as [VibranceGUI](https://vibrancegui.com/) for Windows, which to modify the saturation of any given screen whenever certain programs are launched/are in focus.

It currently only supports Nvidia and other GPUs that implement the Color Transformation Matrix (CTM) property. If you find a way to add this support to any other GPUs, please inform us so that vibrantLinux can support those GPUs.

## Requirements

The required packages are:

- Qt 6
- libxcb
- libxcb-ewmh
- [libvibrant](https://github.com/libvibrant/libvibrant/)

## Installation

AUR: https://aur.archlinux.org/packages/vibrantlinux
Flathub: https://flathub.org/apps/details/io.github.libvibrant.vibrantLinux
Nixpkgs: https://search.nixos.org/packages?channel=unstable&show=vibrantlinux
Repology: https://repology.org/project/vibrantlinux/versions

```bash
git clone https://github.com/libvibrant/vibrantLinux.git
cd vibrantLinux
cmake -Bbuild
cmake --build build
build/vibrantLinux
```
