#!/bin/bash

BUILD_DIR="build"

CFLAGS="-g -O0 -I/usr/include/X11R5 -L/usr/lib/X11R5"
LFLAGS="-lX11"

if [ ! -d "$BUILD_DIR" ]; then
        mkdir "$BUILD_DIR"
fi

pushd "$BUILD_DIR" > /dev/null
g++ $CFLAGS $LFLAGS -o DrawGlyph ../DrawGlyph.cpp
popd > /dev/null
