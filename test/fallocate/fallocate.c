/* -*- c -*- */

// Time-stamp: <2013-06-28 18:21:36 Friday by xq>

/**
 * @file 
 * @author xq
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
    int fd;
    fd = open("fallocate.dat", O_CREAT | O_TRUNC | O_WRONLY);
    if(fallocate(fd, 0, SEEK_SET, 1<<20) < 0)
    {
        printf("111111111111 failed");
    }
    close(fd);
    return 0;
}
