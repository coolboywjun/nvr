#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/nvr_target/usr/lib

# export for glib
export G_DEBUG=gc-friendly
export G_SLICE=always-malloc

function memcheck()
{
	export G_SLICE=always-malloc
	export G_DEBUG=gc-friendly
	valgrind --tool=memcheck --leak-check=full --leak-resolution=high --num-callers=20 --show-reachable=yes --log-file=`basename $1`.log $1
#	valgrind --tool=memcheck --leak-check=full --num-callers=18 --show-reachable=yes --log-file=`basename $1`.log $1
#	valgrind --tool=memcheck --log-file=`basename $1`.log $1
}

function mknvr()
{
	(cd /at/a20/nvr; make $1)
}

