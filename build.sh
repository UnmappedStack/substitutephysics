#!/bin/bash
# pretty much just copied this:
# https://anguscheng.com/post/2023-12-12-wasm-game-in-c-raylib/
# idk anything about wasm


set -euo pipefail

# Get EMSDK on the PATH
cd emsdk
source emsdk_env.sh

cd ..

# Build to Web Assembly
emcc -o bin/index.html \
    main.c -Os -Wall raylib/src/libraylib.web.a \
    -L. -L raylib/src \
    -s USE_GLFW=3 \
    -s ASYNCIFY \
    --shell-file shell.html \
    --preload-file assets \
    -s TOTAL_STACK=64MB \
    -s INITIAL_MEMORY=128MB \
    -s ASSERTIONS \
    -DPLATFORM_WEB

# Run the game
emrun bin/index.html
