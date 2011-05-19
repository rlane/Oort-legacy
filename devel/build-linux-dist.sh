#!/bin/sh
set -e
#./build-linux.sh
git clean -f -X -d
(cd luajit && git clean -f -X -d)
./autogen.sh
./configure
make dist
rm -rf tmp
mkdir tmp
cd tmp
tar -xjvf ../oort-0.1.0.tar.bz2
cd oort*
#find -name '*.stamp' -exec rm {} \;
./configure
make check
