#!/bin/sh

usexcb=true

for arg in "$@"; do
	if [ $arg = "--noxcb" ]; then
		usexcb=false
	fi
done

git reset --hard
git pull

if $usexcb; then
	qmake
else
	qmake "DEFINES+=VIBRANT_LINUX_NO_XCB"
fi

make
rm -r include forms src *.o *.h *.cpp *.pro* Makefile LICENSE README.md \
assets/ resources.qrc

