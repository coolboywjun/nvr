#include<player.h>
#include<memory.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<asm-generic/int-ll64.h>
#include<linux/g2d_driver.h>
#include<enc_inc/venc.h>
#include<encode.h>
static cedarv_encoder_t *venc_device=NULL; 

s8 enc_jpeg(u16 width,u16 height,VencInputBuffer *input_buffer,char *file)  
{  
	int result=0;	
	VencOutputBuffer output_buffer;
///step 1	
	venc_device->ioctrl(venc_device, VENC_CMD_FLUSHCACHE_ALLOCATE_INPUT_BUFFER, input_buffer); 
	result = venc_device->ioctrl(venc_device, VENC_CMD_ENQUENE_INPUT_BUFFER, input_buffer); 
	if(result < 0)
	{	
		printf("VENC_CMD_ENQUENE_INPUT_BUFFER fail \r\n");		
		return -1;
	}
////step 2
	result = venc_device->ioctrl(venc_device, VENC_CMD_DEQUENE_INPUT_BUFFER, input_buffer);
	if(result < 0) 
	{
		printf("VENC_CMD_DEQUENE_INPUT_BUFFER fail \r\n");		
		return -1;
	}
	result = venc_device->ioctrl(venc_device, VENC_CMD_ENCODE,input_buffer);
	venc_device->ioctrl(venc_device, VENC_CMD_RETURN_ALLOCATE_INPUT_BUFFER, input_buffer);
	if(result == 0) 
	{
		memset(&output_buffer, 0, sizeof(VencOutputBuffer));
		result = venc_device->ioctrl(venc_device, VENC_CMD_GET_BITSTREAM,&output_buffer);
//		printf("output buf.size0:%d,size1:%d \r\n",output_buffer.size0,output_buffer.size1);
//		printf("first two byte 0x%x 0x%x \r\n",*(output_buffer.ptr0),*(output_buffer.ptr0+1));
		FILE *fd_wr;
		if(NULL==(fd_wr=fopen(file,"wb")))
		{
			OSA_ERROR("open file:%s fail",file);
		}
		if(output_buffer.size0) 
		{		
			cedarx_cache_op((long)output_buffer.ptr0,(long)(output_buffer.ptr0+output_buffer.size0),1);
			if(1!=fwrite(output_buffer.ptr0,output_buffer.size0,1,fd_wr))
			{
				printf("fd wr file fail\r\n");
			}
		}	
		if(output_buffer.size1) 
		{
			cedarx_cache_op((long)output_buffer.ptr1,(long)(output_buffer.ptr1+output_buffer.size1),1);
			if(1!=fwrite(output_buffer.ptr1,output_buffer.size1,1,fd_wr))
			{
				printf("fd wr file fail\r\n");
			}
		}
		fclose(fd_wr);
		if(result == 0) 
		{
			venc_device->ioctrl(venc_device, VENC_CMD_RETURN_BITSTREAM, &output_buffer);
		}			
	}
	else
	{
		printf("VENC_CMD_RETURN_ALLOCATE_INPUT_BUFFER fail \r\n");
		return -1;
	}	
	return 0;
}  
static void enc_nonmb_y(u32 Width,u32 Height,u8*src,u8*dst)
{	
	unsigned int i,j,l,m,n,mul32_line=0,rem_line=0,mul32_col=0;	
	unsigned long offset;	
	unsigned char *dst_asm,*src_asm;
	mul32_line=Height/32;
	rem_line=Height-mul32_line*32;
	mul32_col=Width/32;	
	for(i=0;i<mul32_line;i++)//360/32=11.25
	{
		for(j=0;j<mul32_col;j++)//640/32=20
		{		
			for(l=0;l<32;l++)
			{	
				m=(i<<5)/*i*32*/ + l;//垂直方向上偏移的行数(32行为单位)
				n= (j<<5)/*j*32*/;  //水平方向上偏移的列数(32列为单位)
				offset = m*Width+ n;//从图片头开始，字节为单位的偏移数
				dst_asm = dst+offset;
				src_asm = src;
				asm volatile (
				        "vld1.8         {d0 - d3}, [%[src_asm]]              \n\t"
				        "vst1.8         {d0 - d3}, [%[dst_asm]]              \n\t"
				        : [dst_asm] "+r" (dst_asm), [src_asm] "+r" (src_asm)
				        :  //[srcY] "r" (srcY)
				        : "cc", "memory", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d16", "d17", "d18", "d19", "d20", "d21", "d22", "d23", "d24", "d28", "d29", "d30", "d31"
				        );

				src += 32;
			}
		}
	}	
	//剩下的8行	
	i=mul32_line;
	for(j=0;j<mul32_col;j++)//640/32=20
	{		
		for(l=0;l<rem_line;l++)
		{	
			m=(i<<5)/*i*32*/ + l;//垂直方向上偏移的行数(32行为单位)
			n= (j<<5)/*j*32*/;	//水平方向上偏移的列数(32列为单位)
			offset = m*Width + n;//从图片头开始，字节为单位的偏移数
			dst_asm = dst+offset;
			src_asm = src;
			asm volatile (
					"vld1.8 		{d0 - d3}, [%[src_asm]] 			 \n\t"
					"vst1.8 		{d0 - d3}, [%[dst_asm]] 			 \n\t"
					: [dst_asm] "+r" (dst_asm), [src_asm] "+r" (src_asm)
					:  //[srcY] "r" (srcY)
					: "cc", "memory", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d16", "d17", "d18", "d19", "d20", "d21", "d22", "d23", "d24", "d28", "d29", "d30", "d31"
					);
	
			src += 32;
		}
		src+=32*(32-rem_line);
	}
}
static void enc_nonmb_uvc(u32 Width,u32 Height,u8*src,u8*dst)
{	
	unsigned int i,j,l,m,n,mul32_line=0,rem_line=0,mul32_col=0;	
	unsigned long offset;	
	unsigned char *dst_asm,*src_asm;
	Height>>=1;
	mul32_line=Height/32;
	rem_line=Height-mul32_line*32;
	mul32_col=Width/32;
	DEBUG_PF("mul32_line:%d,mul32_col:%d",mul32_line,mul32_col);
	for(i=0;i<mul32_line;i++)//180/32=5.625
	{
		for(j=0;j<mul32_col;j++)//640/32=20
		{			
			for(l=0;l<32;l++)
			{
				m=i*32 + l;//偏移的行数sxq
				n= j*32;
				offset = m*Width + n;
				dst_asm = dst+offset;
				src_asm = src;
				asm volatile (
				        "vld1.8         {d0 - d3}, [%[src_asm]]              \n\t"
				        "vst1.8         {d0 - d3}, [%[dst_asm]]              \n\t"
				        : [dst_asm] "+r" (dst_asm), [src_asm] "+r" (src_asm)
				        :  //[srcY] "r" (srcY)
				        : "cc", "memory", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d16", "d17", "d18", "d19", "d20", "d21", "d22", "d23", "d24", "d28", "d29", "d30", "d31"
				        );

				src += 32;
			}
		}
	}	
	i=mul32_line;
	for(j=0;j<mul32_col;j++)//640/32=20
	{			
		for(l=0;l<rem_line;l++)
		{
			m=i*32 + l;//偏移的行数sxq
			n= j*32;
			offset = m*Width + n;
			dst_asm = dst+offset;
			src_asm = src;
			asm volatile (
					"vld1.8 		{d0 - d3}, [%[src_asm]] 			 \n\t"
					"vst1.8 		{d0 - d3}, [%[dst_asm]] 			 \n\t"
					: [dst_asm] "+r" (dst_asm), [src_asm] "+r" (src_asm)
					:  //[srcY] "r" (srcY)
					: "cc", "memory", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d16", "d17", "d18", "d19", "d20", "d21", "d22", "d23", "d24", "d28", "d29", "d30", "d31"
					);	
			src += 32;
		}
		src+=32*(32-rem_line); 
	}
}
void enc_mb2_nonmb(int Width,int Height,u8*ySrc,u8 *cSrc,u8 *dsty,u8*dstuv)
{
	enc_nonmb_y(Width,Height,/*(u8*)cedarv_address_phy2vir(ySrc)*/ySrc,dsty);
	enc_nonmb_uvc(Width,Height,/*(u8*)cedarv_address_phy2vir(cSrc)*/cSrc,dstuv);
}  

s8 enc_init(u16 width,u16 height)
{
	VencBaseConfig base_cfg;
	VencAllocateBufferParam alloc_parm;
	base_cfg.codectype = VENC_CODEC_JPEG;
	base_cfg.framerate = 30;
	base_cfg.input_width = width;
	base_cfg.input_height= height;
	base_cfg.dst_width = width;
	base_cfg.dst_height =height;
	base_cfg.maxKeyInterval = 25;
	base_cfg.inputformat = VENC_PIXEL_YUV420; //uv combined
	base_cfg.targetbitrate = 2*1024*1024;
	alloc_parm.buffernum = 2;	
	venc_device = cedarvEncInit();
	venc_device->ioctrl(venc_device, VENC_CMD_BASE_CONFIG, &base_cfg);
	venc_device->ioctrl(venc_device, VENC_CMD_ALLOCATE_INPUT_BUFFER, &alloc_parm);
	venc_device->ioctrl(venc_device, VENC_CMD_OPEN, 0);
	return 0;
}
s8 enc_get_input_buf(VencInputBuffer *input_buffer)
{
	if(venc_device->ioctrl(venc_device, VENC_CMD_GET_ALLOCATE_INPUT_BUFFER, input_buffer))
	{
		OSA_ERROR("VENC_CMD_GET_ALLOCATE_INPUT_BUFFER fail \r\n");				
		return -1;
	}	
	return OSA_SOK;
}
void enc_exit(void)
{
	if(venc_device)
	{
		venc_device->ioctrl(venc_device, VENC_CMD_CLOSE, 0);
		cedarvEncExit(venc_device);
		venc_device = NULL;
	}
}

