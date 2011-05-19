#/bin/sh
set -e
git clean -f -X -d
(cd luajit && git clean -f -X -d)
./autogen.sh
./configure PKG_CONFIG_PATH="/usr/i486-mingw32/lib/pkgconfig/" --host=i486-mingw32 --with-mingw32
make oort-installer-win32.exe
tag=`git log --pretty=format:%h -1`
cp oort-installer-win32.exe oort-installer-$tag.exe
echo output oort-installer-$tag.exe
