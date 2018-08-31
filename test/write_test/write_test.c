/* -*- c -*- */

// Time-stamp: <2013-06-24 20:02:30 Monday by xq>

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
    int i;
    int fd;
    char buf[10240];
    
    if(argc != 2)
    {
        printf("usage: %s path\n", argv[0]);
        return -1;
    }
    fd = open(argv[1], O_RDWR | O_TRUNC | O_CREAT, 0644);
    if(fd < 0)
    {
        printf("Failed to open file\n");
        return -1;
    }

    for(i = 0; i < 100000; i++)
    {
        //printf ("Write data to file!\n");
        int j = random() % 100000;
        
        lseek(fd, j * sizeof(buf), SEEK_SET);
        if(write(fd, buf, sizeof(buf)) < 0)
        {
            printf("Write file failed\n");
            return -1;
        }
        //printf ("Begin to sleep 10s\n");
       // sleep(10);
       // printf ("End to sleep\n");
    }
    close(fd);
    return 0;
}
