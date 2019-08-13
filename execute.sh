#!/bin/bash
libtoolize
aclocal
autoconf
autoheader
automake --add-missing
#./configure --host=arm-hismall-linux
#./configure --host=mipsel-24kec-linux-uclibc

#export FINAL_PATH=/home/zouqing/work/carnet/linux/auto/vmp/premake/ubuntu/install
# for protoc
#export PKG_CONFIG_PATH=/home/zouqing/osource/network/pc/final_protobuf/lib/pkgconfig
#../configure --prefix=/home/zouqing/work/carnet/linux/auto/vmp/vmp-rl/install --enable-zlog
#export PATH=/home/zouqing/osource/network/pc/final_protobuf/bin:$PATH


if false; then
export FINAL_PATH=/home/zouqing/work/carnet/linux/auto/vmp/premake/ubuntu/install
WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}"   )" && pwd   )"
echo "WORKDIR: $WORKDIR "

if [ ! "$1"x == x ]; then 
    echo "$1"
    platform=$1
else
    #platform=nt966x
    platform=mips
fi
if [ "$platform" == mips ]; then
    ./configure --host=mipsel-24kec-linux-uclibc enable_x86=no enable_sqlite3=no \
        enable_zlog=no --prefix=$WORKDIR/nt966x/install
else
    ./configure --host=x86_64-unknown-linux-gnu enable_x86=yes enable_sqlite3=no \
        enable_zlog=no --prefix=$WORKDIR/x86/install
fi
make
make install
fi

