#!/bin/sh
git reset --hard
git pull
qmake
make
rm -r *.o *.h *.cpp *.ui *.pro* Makefile LICENSE README.md assets/ resources.qrc
