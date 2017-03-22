#!/bin/bash

BUILD_DIR="build"

if [ ! -d "$BUILD_DIR" ]; then
        echo "Project not built. Run 'build.sh' first."
        exit 0
fi

pushd "$BUILD_DIR" > /dev/null
./DrawGlyph
popd > /dev/null
