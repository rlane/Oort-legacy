#!/bin/sh
set -e
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
./configure PKG_CONFIG_PATH="/usr/i486-mingw32/lib/pkgconfig/" --host=i486-mingw32 --with-mingw32
make
make oort-installer-win32.exe
