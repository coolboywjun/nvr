/* -*- c -*- */

// Time-stamp: <2013-06-27 14:32:41 Thursday by xq>

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
#include <signal.h>



int quit = 0;
void sig_int(int signo)
{
    quit = 1;
}



int main(int argc, char *argv[])
{
    char data[1024];
    FILE *fp = NULL;
    time_t begin, end;
    int writekb;
    char filename[128];
//    int cnt = 50;
    signal(SIGINT, sig_int);
    begin = time(NULL);
    sprintf(filename, "/media/sda1/data_%d.dat", begin);
    fp = fopen(filename, "wb");
    if(fp == NULL)
    {
        printf("Failed to open data.dat file.\n");
        return -1;
    }
    while(!quit)
    {
        fwrite(data, sizeof(data), 1, fp);
        writekb++;
//        if(writekb % 50 == 0)
//            fseek(fp, SEEK_CUR, -10240);
    }
    fclose(fp);

    end = time(NULL);
    printf("begin: %d, end: %d\n", begin, end);
    printf("%dKB/s\n", writekb/(end - begin));
    
    return 0;
}
