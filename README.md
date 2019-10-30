# Vibrant Linux

Main Program:

![Main program](assets/program.png)

Program entry editor:

![Program vibrance editor](assets/entryeditor.png)

vibrant linux is a gui tool that serves the same purpose as vibrance gui on windows.

It currently only supports nvidia as I have yet to find a way to tweak saturation/vibrance on amd, and intel gpus. If you find a way to adjust saturation/vibrace on amd, intel, or any other gpu brand that I never heard of please create an issue with the method of how to adjust saturation and it will be added to the program.

Trust me, I'm more digusted with myself than anyone could be for writing software that only supports nvidia.

# Requirements

The only things needed to run this program is:

```
git
nvidia-settings
qt
libxcb (optional)
libxcb-ewmh (optional)
```

## Ubuntu

```
sudo apt-get install build-essential qtcreator qt5-default git
#xcb is optional
sudo apt-get install libxcb-dev libxcb-ewmh-dev
```

## Arch Linux

```
sudo pacman -S nvidia-settings qt5-base git
#xcb is optional
sudo pacman -S libxcb xcb-util-wm
```

# Installation

```
git clone https://gitlab.com/zee220/vibrantlinux
cd vibrantlinux
./update.sh
#or alternatively if you know you don't want to use ewmh to get the active window
./update.sh --noxcb
```

# Updates

Head to your installation folder and run
```
./update.sh
#or for no xcb
./update.sh --noxcb
```

