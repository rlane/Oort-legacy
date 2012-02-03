#!/bin/sh
set -e
tag=`git log --pretty=format:%h -1`
#./devel/build-nacl.sh
./devel/nacl-publish.rb $tag
