#!/bin/sh

# Time-stamp: <2013-07-18 18:06:07 Thursday by xq>

# @version 1.0
# @author xieqinag

# dm81xx
#CROSS_COMPILE=arm-none-linux-gnueabi-gcc
#PREFIX=/opt/arm-2009q1/arm-none-linux-gnueabi

# a20
CROSS_COMPILE=arm-linux-gnueabi-gcc
PREFIX=/opt/a20/external-toolchain/arm-linux-gnueabi/libc/usr

function build_e2fsprogs()
{
    cd e2fsprogs-1.42.8/
    ./configure --prefix=$PREFIX --host=arm-linux CC=$CROSS_COMPILE
    make -j4
    make install
    cd ../
}

function build_libev()
{
    cd libev-4.15
    ./configure --prefix=$PREFIX --host=arm-linux CC=$CROSS_COMPILE
    make -j4
    make install
    cd ../
}

function build_libffi()
{
    cd libffi-3.0.13
    ./configure --prefix=$PREFIX --host=arm-linux CC=$CROSS_COMPILE
    make -j4
    make install
    cd ../
}

function build_zlib()
{
    cd zlib-1.2.8
    ./configure --prefix=$PREFIX
    ###############################
    # modify the zlib makefile
    ###############################
    make -j4
    make install
    cd ../
}

function build_glib()
{
    cd glib-2.36.2
#    export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig
    cp ../arm-linux.cache .
    #./configure --prefix=$PREFIX --build=i686-linux --host=arm-linux --cache-file=arm-linux.cache --disable-man CC=$CROSS_COMPILE
    ./configure --cache-file=arm-linux.cache --disable-man 
    make -j4
#    make install
    cd ../
}

function build_util_linux()
{
    cd util-linux-2.23.1
    ./configure --prefix=$PREFIX --host=arm-linux --enable-libuuid --without-ncurses --disable-use-tty-group CC=$CROSS_COMPILE
    make -j4
    make install
    cd ../
}

function build_parted()
{
    cd parted-3.1
    ./configure --prefix=$PREFIX --disable-device-mapper --without-readline --disable-rpath --host=arm-linux CC=$CROSS_COMPILE
    make -j4
    make install
    cd ../
}

function build_ffmpeg()
{
    cd ffmpeg-2.1
    ./configure --enable-version3 --prefix=$PREFIX  --arch=arm --target-os=linux --enable-cross-compile \
    --cc=arm-linux-gnueabi-gcc --cxx=arm-linux-gnueabi-g++ --enable-shared --enable-doc \
    --disable-programs --disable-stripping \
    --disable-static --enable-shared --disable-swscale-alpha --disable-swscale \
    --disable-postproc --disable-avfilter --disable-dct --disable-dwt --disable-lsp --disable-lzo \
    --disable-mdct --disable-rdft --disable-fft --disable-everything --enable-encoder=pcm_alaw --enable-encoder=pcm_mulaw \
    --enable-muxer=avi --enable-demuxer=avi --enable-protocol=file --disable-yasm 
    make -j4
    make install
    cd ../
}

function build_alsa()
{
    cd alsa-lib-1.0.27.2  
    ./configure --prefix=$PREFIX --host=arm-linux --build=i686-linux \
        --enable-shared --disable-python \
        --with-configdir=/usr/local/share --with-plugindir=/usr/local/lib/alsa_lib \
        CC=$CROSS_COMPILE
    make -j4
#    make install
    cd ../
}

function build_jpeg()
{
    cd jpeg-6b
    ./configure --prefix=$PREFIX --enable-shared CC=$CROSS_COMPILE
    make -j4
    make install
    cd ../
}

function build_popt()
{
    cd popt-1.7
    ./configure --prefix=$PREFIX --host=arm-linux --build=i686-linux CC=$CROSS_COMPILE
    make -j4
    make install
    cd ../
}

function build_minigui()
{
    cd libminigui-1.6.10
#
#--with-style=classic
#--with-style=fashion

    ./configure  \
--prefix=$PREFIX \
--host=arm-linux \
--target=arm-linux \
--build=i386-linux \
--with-osname=linux \
--with-style=fashion \
--with-targetname=fbcon \
--enable-autoial \
--enable-rbf16 \
--disable-qvfbial \
--disable-videoqvfb \
CFLAGS=-g \
CC=$CROSS_COMPILE
    # make -j4
    # make install
    cd ../
}

function build_qrencode()
{
    cd ./qrencode-3.4.3
    ./configure \
        --prefix=$PREFIX \
        --build=i386-linux \
        --host=arm-linux \
        CC=arm-linux-gnueabi-gcc \
        png_CFLAGS=-I/opt/a20/external-toolchain/arm-linux-gnueabi/libc/usr/include \
        png_LIBS="-L/opt/a20/external-toolchain/arm-linux-gnueabi/libc/usr/lib -shared -lpng -lz"
    make -j4
    cd ../
}

function build_mxml()
{
    cd mxml-2.7
#
#--with-style=classic
#--with-style=fashion

    ./configure  \
--prefix=$PREFIX \
--host=arm-linux \
--target=arm-linux \
CC=$CROSS_COMPILE
    make -j4
    make install
    cd ../
}
function build_opensll()
{
    cd openssl
    ./config no-asm shared \
        --prefix=$PREFIX \
        
}
function build_dbus()
{
    cd dbus-1.8.2

    ./configure  \
--prefix=$PREFIX \
--host=arm-linux \
--target=arm-linux \
CC=$CROSS_COMPILE
    make -j4
    make install
    cd ../
}

#build_e2fsprogs
#build_libev
#build_libffi
#build_zlib
#build_glib
#build_util_linux
#build_parted
#build_ffmpeg
#build_alsa
#build_jpeg
#build_popt
#build_minigui
#build_qrencode
#build_mxml
#build_opensll
#build_dbus
