#!/bin/sh


build_nvr()
{
    make clean;
    JXJ_VERSION=y DEST_DIR=a20_target make
    JXJ_VERSION=y DEST_DIR=a20_target make pack
    JXJ_VERSION=n DEST_DIR=a20_target make pack    
}

build_nvr_jxj()
{
    make clean;
    JXJ_VERSION=y DEST_DIR=a20_target make
    JXJ_VERSION=y DEST_DIR=a20_target make pack
    # login to ubuntu64 to build image, and then run command behand
    # JXJ_VERSION=y DEST_DIR=a20_target make image
}

build_nvr_n()
{
    make clean;
    JXJ_VERSION=n DEST_DIR=a20_target make
    JXJ_VERSION=n DEST_DIR=a20_target make pack
    # login to ubuntu64 to build image, and then run command behand
    # JXJ_VERSION=n DEST_DIR=a20_target make image
}

