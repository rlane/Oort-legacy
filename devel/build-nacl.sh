#!/bin/sh
set -e
NACL_SDK_ROOT=$HOME/src/nacl-sdk-update/pepper_15
NACL_TOOLCHAIN_ROOT=$NACL_SDK_ROOT/toolchain/linux_x86
NACLPORTS=$HOME/src/naclports
NACL_PACKAGES_BITSIZE=64

source $NACLPORTS/src/build_tools/common.sh

git clean -f -X -d
(cd third_party/lua && git clean -f -X -d)

export CC=${NACLCC}
export CXX=${NACLCXX}
export AR=${NACLAR}
export RANLIB=${NACLRANLIB}
export PKG_CONFIG_PATH=${NACL_SDK_USR_LIB}/pkgconfig
export PKG_CONFIG_LIBDIR=${NACL_SDK_USR_LIB}
export PATH=${NACL_BIN_PATH}:${PATH};

./autogen.sh
./configure \
	--host=nacl \
	--disable-luajit \
	--prefix=${NACL_SDK_USR} \
	--exec-prefix=${NACL_SDK_USR} \
	--libdir=${NACL_SDK_USR}/lib \
	--oldincludedir=${NACL_SDK_USR}/include
make all
