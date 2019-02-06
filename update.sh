#!/bin/sh
git reset --hard
git pull
qmake
make
rm *.o *.h *.cpp *.ui *.pro* Makefile README.md

