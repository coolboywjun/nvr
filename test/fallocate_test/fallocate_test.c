/* -*- c -*- */

// Time-stamp: <2013-06-24 19:54:29 Monday by xq>

/**
 * @file 
 * @author xq
 */
#define _GNU_SOURCE
#include <fcntl.h>
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
    int buf[4] = {
        111,222,333,444
    };
    if(argc != 2)
    {
        printf ("Usage: %s path\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_WRONLY|O_CREAT|O_LARGEFILE, 0644);
    if(fd < 0)
    {
        printf ("Failed to open file\n");
        return -1;
    }
    if(fallocate(fd, 0, SEEK_SET, 16 * 1024) < 0)
    {
        printf("Failed to fallocate!\n");
        return -1;
    }
    if(write(fd, buf, sizeof(buf))< 0)
    {
        printf ("Failed to write file.\n");
        return -1;
    }
    close(fd);
    return 0;
}
