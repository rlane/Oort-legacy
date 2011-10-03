#!/bin/sh
set -e
git clean -f -X -d
(cd third_party/luajit && git clean -f -X -d)
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
