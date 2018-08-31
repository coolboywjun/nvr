#!/bin/sh

# Time-stamp: <2013-06-05 16:26:00 Wednesday by xq>

# @version 1.0
# @author ahei


for filename in `ls`; do
    mv $filename `echo $filename | sed 's/ti/nvr/g'`
done

