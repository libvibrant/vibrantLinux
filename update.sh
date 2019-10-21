#!/bin/sh
git reset --hard
git pull
qmake
make
rm -r include forms src *.o *.h *.cpp *.pro* Makefile LICENSE README.md \
assets/ resources.qrc
