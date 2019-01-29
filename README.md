# Vibrant Linux


vibrant linux is a gui tool that serves the same purpose as vibrance gui on windows.

It currently only supports nvidia as I have yet to find a way to tweak saturation/vibrance on amd, and intel gpus. If you find a way to adjust saturation/vibrace on amd, intel, or any other gpu brand that I never heard of please create an issue with the method of how to adjust saturation and it will be added to the program.

# Installation

```
git clone https://gitlab.com/zee220/vibrantlinux
cd vibrantlinux
qmake
make
rm *.o *.cpp *.h *.ui *.pro *.user Makefile
```

the rm step is not required but it will clean up the directory of all the source code so that you only end up with the executable. From then you can choose to move it to your path if you wish to easily use it or keep in whatever directory you want.

