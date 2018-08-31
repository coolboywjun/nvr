/* -*- c -*- */

// Time-stamp: <2013-06-24 17:51:20 Monday by xq>

/**
 * @file 
 * @author xq
 */
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
    FILE *fp;
    char buf[12] = {
        'a', 'b', 'c', 'd', 
        'a', 'b', 'c', 'd',
        'a', 'b', 'c', 'd'
    };
    if(argc != 2)
    {
        printf("usage: %s path\n", argv[0]);
        return -1;
    }
    
    fp = fopen(argv[1], "rb+");
    if(fp == NULL)
    {
        printf("Failed to open file\n");
        return -1;
    }
    if(fwrite(buf, sizeof(buf), 1, fp) != 1)
    {
        printf("Write file failed\n");
        return -1;
    }
    fclose(fp);
    return 0;
}
