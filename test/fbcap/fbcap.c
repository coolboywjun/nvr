
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <linux/videodev2.h>

const unsigned char szBitmapHead[] = 
{
0x42, 0x4d, 0x36, 0x40, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00,
0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0xd0, 0x02, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0x0e, 0x00, 0x00, 0xc4, 0x0e, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void write_bitmap(const char *pFbBuff, const char *pFilename)
{
	char *pBitmap = (char *)malloc(1280*720*4);
	char *pHeadr = pBitmap;
	FILE *fp2;
	int line, col;
	fp2 = fopen(pFilename, "wb+");
	
	fwrite(szBitmapHead, 1, 54, fp2);
	for (line = 0; line < 720; line++)
	{
		for (col = 0; col < 1280; col++)
		{
			memcpy(pBitmap, pFbBuff+line*1280*4+(1280-col-1)*4, 4);
			pBitmap += 4;
		}
	}
	
	pBitmap = pHeadr;
	for (line = 0; line < 720; line++)
	{
		for (col = 0; col < 1280; col++)
		{
			fwrite(pBitmap+(719-line)*1280*4+(1280-col-1)*4, 4, 1, fp2);
		}
	}
	
	fclose(fp2);
	free(pHeadr);
}

int main(int argc ,char *argv[])
{

    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = NULL;
    int x = 0 , y = 0;
    long int location = 0 ;
    int sav = 0;
    int fbindex = 0;
    char fbdev[32];
    
    if(argc != 2)
    {
        printf("uasage: %s num color alpha\n", argv[0]);
        return -1;
    }
    fbindex = 0;
//open framebuffer device
    sprintf(fbdev, "/dev/fb%d", fbindex);
    fbfd = open(fbdev, O_RDWR);
    if(!fbfd){

        printf("Error:cannot open the framebuffer device!\n");
        exit(1);
    }
    printf("well you open the framebuffer sucessful!\n");
    //get the fixed screen information
    if(ioctl(fbfd,FBIOGET_FSCREENINFO,&finfo)){

        printf("Error reading fixed information\n");
        exit(2);
    }
    //get the variable screen information
    if(ioctl(fbfd,FBIOGET_VSCREENINFO,&vinfo)){

        printf("Error reading variable information\n");
        exit(3);
    }
    //show these information
    printf("vinfo.xres=%d\n",vinfo.xres);
    printf("vinfo.yres=%d\n",vinfo.yres);
    printf("vinfo.bit_per_bits=%d\n",vinfo.bits_per_pixel);
    printf("vinfo.xoffset=%d\n",vinfo.xoffset);
    printf("vinfo.yoffset=%d\n",vinfo.yoffset);
    printf("finfo.line_length=%d\n",finfo.line_length);
    screensize = vinfo.xres*vinfo.yres*vinfo.bits_per_pixel/8;

    //map the device to memory
    fbp = (char *)mmap(0,screensize,PROT_READ|PROT_WRITE,MAP_SHARED,fbfd,0);
    if((int)fbp == -1){

        printf("Error:failed to map framebuffer device to memory\n");
        exit(4);
    }

	write_bitmap(fbp, argv[1]);
	
    //release the memory
    munmap(fbp,screensize);
    close(fbfd);
    return 0; 
}


