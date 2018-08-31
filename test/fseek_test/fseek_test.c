/* -*- c -*- */

// Time-stamp: <2013-06-22 16:56:33 Saturday by xq>

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
    char c = 'E';
    if(argc != 2)
    {
        printf("usage: %s path\n", argv[0]);
        return -1;
    }
    fp = fopen(argv[1], "wb");
    if(fp == NULL)
    {
        printf("Failed to open!!!\n");
        return -1;
    }
    if(fseek(fp, 1024-1, SEEK_SET) < 0)
    {
        printf("Failed to fseek!!!\n");
        return -1;
    }
    if(fwrite(&c, sizeof(c), 1, fp) != 1)
    {
        printf("Failed to write!\n");
        return -1;
    }
    fclose(fp);
    
    return 0;
}
