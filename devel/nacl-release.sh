#!/bin/sh
set -e
tag=`git log --pretty=format:%h -1`
NACL_PACKAGES_BITSIZE=32 ./devel/build-nacl.sh
NACL_PACKAGES_BITSIZE=64 ./devel/build-nacl.sh
./devel/nacl-publish.rb $tag
