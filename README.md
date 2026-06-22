## build for linux

```sh
cc main.c -o out -lraylib
```
then you can just do `./out`

make sure you have raylib installed first

## build for web

slightly more annoying cos my build system is terrible.

this also only works on unix-like shells and I haven't tested outside linux. I don't recommend bothering with this tbh just use the one I've already built cos its painful

```sh
# first clone raylib here
git clone https://github.com/raysan5/raylib

# then clone emsdk
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
git pull
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh

# build raylib
cd .. # or go back to root repo, i think this should but idk if the previous commends take you further
      # down the file tree
cd raylib/src
make -f ../../Makefile.raylib -e

# finally build the game
cd ../.. # aka to root repo
./build.sh
```
