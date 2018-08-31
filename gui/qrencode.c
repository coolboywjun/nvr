#include<qrencode.h>
#include<png.h>
#include<string.h>
#include<stdlib.h>
#define MUL_TIME (3)
int write_png(const char *nameout,int img_w, int img_h, unsigned char *img, int img_comp, char*title);
//int main(int argc, char *argv[])
int  create_qrcode_png(const char *stBuf, const char *pngName, unsigned int *width, unsigned int *height)
{
	QRcode *qrcode;
	char *new_buf_1,*new_buf_2,*buf1_bk;

	if((NULL == stBuf) && (NULL == pngName)&&(NULL == width)&&(NULL == height))
		return -1;
		
	if(NULL==(qrcode = QRcode_encodeString(stBuf, 0, QR_ECLEVEL_M, QR_MODE_8, 1)))
	{
		printf("QRcode_encodeString error\r\n");
		return -1;
	}
	printf("width is:%d,version:%d\r\n",qrcode->width,qrcode->version);
	if(NULL==(new_buf_1=(char*)malloc(qrcode->width*qrcode->width*MUL_TIME)))
	{
		printf("malloc fail\r\n");
		return -1;
	}
	unsigned int i,j,k,pro_h,pro_w;
	buf1_bk=new_buf_1;
	for(i=0;i<qrcode->width;i++)
	{
		new_buf_1=buf1_bk+i*qrcode->width*MUL_TIME;
		for(j=0;j<MUL_TIME;j++)
		{
			memcpy(new_buf_1+j*qrcode->width,qrcode->data+i*qrcode->width,qrcode->width);
		}
	}
	pro_h=qrcode->width*MUL_TIME;
	pro_w=pro_h;
	if(NULL==(new_buf_2=(char*)malloc(pro_h*pro_w)))
	{
		printf("malloc fail\r\n");
		return -1;
	}	
	for(i=0;i<pro_h;i++)
	{
		for(j=0;j<qrcode->width;j++)
		{
			for(k=0;k<MUL_TIME;k++)
			{
				new_buf_2[i*pro_w+j*MUL_TIME+k]=buf1_bk[i*qrcode->width+j];
			}
		}
	}
	printf("after pro width is:%d\r\n",pro_w);
	for(i=0;i<pro_w*pro_h;i++)
	{
		new_buf_2[i]=new_buf_2[i]&1?0:255;
	}

	*width = pro_w;
	*height = pro_h;
	if(write_png(pngName,pro_w,pro_h,(unsigned char *)new_buf_2,1,NULL) < 0)
	{
		printf("Write qr png file failed.\n");
		QRcode_free(qrcode);
		free(buf1_bk);
		free(new_buf_2);	
		return -1;
	}
	printf("Write qr png file %s successfully.\n", pngName);
	QRcode_free(qrcode);
	free(buf1_bk);
	free(new_buf_2);	
	return 0;
}
int write_png(const char *nameout,int img_w, int img_h, unsigned char *img, int img_comp, char*title)
{ 
	int ret = 0;
	int i,j;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row;
	FILE *fout = fopen(nameout, "wb");;
	if(NULL==fout) 
	{
		printf("can not open %s\n",nameout); 
		return -1;
	}
	// initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		printf("Could not alloc png_ptr struct\n");
		ret = -1;
		goto Lab99;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		printf("Could not allo info_ptr struct\n"); 
		ret = -1;
		goto Lab99;
	}
	if (setjmp(png_jmpbuf(png_ptr))) 
	{
		printf("Error during png creation\n"); 
		ret = -1;
		goto Lab99;
	}
	png_init_io(png_ptr, fout);
	png_set_IHDR(png_ptr, info_ptr, img_w, img_h, 8, /*PNG_COLOR_TYPE_RGB*/PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
	// write comments: set PNG title
	if (title !=NULL)
	{
		png_text t;
		t.compression = PNG_TEXT_COMPRESSION_NONE;
		t.text = title;
		t.key = "Title";
		png_set_text(png_ptr,info_ptr,&t,1);
	}
	png_write_info(png_ptr, info_ptr); //write header info
	row = (png_bytep) malloc(sizeof(png_byte) * img_w * img_comp);
	if (!row)
	{
		printf("Could not alloc row \n"); 
		goto Lab99;
	}
	for (j=0;j< img_h;j++)
	{
		for (i=0;i< img_w * img_comp;i++) 
		{
			row[i] = img[j * img_w * img_comp + i];  
		}
		png_write_row(png_ptr, row);
	}
	png_write_end(png_ptr, NULL);
	printf("Write Qr file: %s\n", nameout);
	Lab99:;
	 /* cleanup heap allocation */
	if (row != NULL) free(row);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);      
	fclose(fout);
	return ret;
}
