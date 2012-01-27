#!/bin/sh
rm -rf build-linux
mkdir build-linux
cd build-linux
../configure
make -j8
