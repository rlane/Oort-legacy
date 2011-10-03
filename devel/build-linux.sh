#/bin/sh
set -e
git clean -f -X -d
(cd third_party/lua && git clean -f -X -d)
(cd third_party/luajit && git clean -f -X -d)
./autogen.sh
./configure --with-valgrind
make all dist check
tag=`git log --pretty=format:%h -1`
cp oort-0.1.0.tar.bz2 oort-$tag.tar.bz2
echo output oort-$tag.tar.bz2
