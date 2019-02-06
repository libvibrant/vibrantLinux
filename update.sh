#!/bin/sh
git reset --hard
git pull
qmake
make
rm *.o *.cpp *.ui *.pro* Makefile README.md

