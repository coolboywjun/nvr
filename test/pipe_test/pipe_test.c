/* Last modified Time-stamp: <2013-07-20 17:05:19 Saturday by xq>
 * @(#)pipe_test.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char *argv[])
{
    FILE *fp;
    const char *cmd = "/opt/nvr/mkfs.ext4 -b 4096 -O \"^has_journal sparse_super\" /dev/sda1";
   char buff[256];
    fp = popen(cmd, "r");
    if(fp == NULL)
    {
        printf("Failed to run command!!!\n");
        return -1;
    }

    while(fgets(buff, sizeof(buff), fp) != NULL)
    {
        printf("==== %s", buff);
    }
    
    pclose(fp);
    return 0;
}

