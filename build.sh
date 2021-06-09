#!/bin/bash

MY_DIR=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $MY_DIR
mkdir -p build
cd build
cmake .. "-DCMAKE_TOOLCHAIN_FILE=/home/simkitv2-0/Documents/repos/vcpkg/scripts/buildsystems/vcpkg.cmake"
make
