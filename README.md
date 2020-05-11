# vibrantLinux
Main Program:

![Main program](assets/program.png)

Program entry editor:

![Program vibrance editor](assets/entryeditor.png)

## Overview

vibrantLinux is a GUI tool to adjust saturation and vibrance in games. It serves the same purpose as [VibranceGUI](https://vibrancegui.com/) for Windows.

It currently only supports Nvidia as I have yet to find a way to tweak saturation/vibrance on AMD and Intel GPUs. If you find a way to adjust saturation/vibrace on AMD, Intel, or any other GPU brand, please create an issue with the method of how to adjust saturation and it will be added to the program, or even better, create a pull request.

Trust me, I'm more digusted with myself than anyone could be for writing software that only supports Nvidia.

## Requirements

The required and optional packages are:

- git
- nvidia-settings
- qt
- libxcb (optional)
- libxcb-ewmh (optional)


## Installation

```bash
git clone https://gitlab.com/zee220/vibrantlinux ~/vibrantlinux
cd ~/vibrantlinux
./update.sh
```

Alternatively, if want to use [EWMH](https://en.wikipedia.org/wiki/Extended_Window_Manager_Hints) to get the active window, run:

```bash
./update.sh --noxcb
```

## Updates

Head to your installation folder. In our case, it is located at `~/vibrantLinux`. Then, run:

```bash
./update.sh
```
For no [XCB](https://xcb.freedesktop.org/), run:

```bash
./update.sh --noxcb
```
