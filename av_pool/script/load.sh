#!/bin/sh

# @version 1.0
# @author xieqiang

# load av_pool.ko

echo "Load av_pool.ko, alloc size is 16MB"
insmod /opt/nvr/modules/av_pool.ko alloc_size=0x2000000


