#! /bin/bash

# run this script script from the project root: ./scripts/buildForDebugger.sh
# It will build with g++ -g compiler flags, so
# gdb can be used to debug
# Note: This is only tested on Ubuntu 20
set -e
mkdir -p build
cmake -S . -B build -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:STRING=/usr/bin/g++ -DCMAKE_BUILD_TYPE=Debug
cmake --build build
