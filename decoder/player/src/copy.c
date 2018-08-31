#include<player.h>
#include <osa_mutex.h>
#include <unistd.h>
#include <osa_file.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<asm-generic/int-ll64.h>
#include<linux/g2d_driver.h>
#include<render.h>
#include<png.h>
#include<pngconf.h>
#include<sys/prctl.h>
#include<enc_inc/venc.h>
#include<encode.h>

#include"storage_lib.h"

enum{
	COPY_THR_STA_WORK_ON=0,
	COPY_THR_STA_STANDBY,
};
typedef struct
{
	u8 copy_thr_sta;
	u8 copy_id;
	u8 discard_num;
	u16 single_pic;	
	u16 cap_res;
	u8* buf_720p;
	u8* buf_360p;
////////消息队列
	int msg_id;
	int msg_flg;
	_thr_msg copy_msgbuf;
////////////
}copy_ctl;
#ifdef PLAYER_TEST
static copy_ctl* copy_test[COPY_CHANNEL_MAX];
#endif
//u8 *copy_720p_noMB_buf=NULL,*copy_360p_noMB_buf=NULL;
volatile static gint copy_play_id[COPY_CHANNEL_MAX]={0,0,0,0};
volatile static u64 copy_frame_pts[COPY_CHANNEL_MAX]={0,0,0,0};
volatile static gint copy_cmd[COPY_CHANNEL_MAX]={0,0,0,0};
volatile static gint copy_cat_num[COPY_CHANNEL_MAX]={0,0,0,0};
volatile static gint copy_single_ch=-1;
static int fd_g2d=-1;
static pthread_mutex_t msg_lock;
volatile cp_handle copy_handle[COPY_CHANNEL_MAX];
const u8 copy_msg_type[COPY_CHANNEL_MAX]={COPY1_MSG_TYPE_RCV,COPY2_MSG_TYPE_RCV,COPY3_MSG_TYPE_RCV,COPY4_MSG_TYPE_RCV};
static OSA_ThrHndl copy_thr[COPY_CHANNEL_MAX];
static void *copy_thr_main(void*param);
static void copy_data_pro(copy_ctl *hdl);
static void copy_black_screen_pro(copy_ctl *hdl);
static void copy_data_pro(copy_ctl *hdl);
static u8 copy_clr_data(u8 ch);
static void copy_capture_pic(copy_ctl*hdl,u16 width,u16 height,u8*ysrc,u8*uvsrc);
static u8 copy_get_cat_num(u8 ch);
static void copy_dec_cat_num(u8 ch);
static int copy_get_single_channel(void);
static void copy_720p_MB2normal(u8 *ysrc,u8*uvsrc,u8*dst);
static s8 copy_720_rsz_360(u8 *src_720p,u8*dst_360);
static void copy_360p_normal2MB(u8 *src,u8*ydst,u8*uvdst);
static u8* copy_thr_720rsz360(u8*ysrc,u8*uvsrc,u8*dst720,u8*dst360);
int copy_thr_create(u8* param)
{
	int status=0;
	static u8 cnt=0;
	if(!cnt)
	{
		cnt=1;		
		status|=player_mutex_create(&msg_lock);
		if(status)
		{
			OSA_ERROR("player_mutex_create():%d!!!",status);
			return status;
		}
		fd_g2d = open("/dev/g2d", O_RDWR);
		if(fd_g2d < 0)
		{
			OSA_ERROR("open g2d fail!");
			return -1;
		}	

	}	
	//status = OSA_thrCreate(&copy_thr[*param], copy_thr_main, COPY_THR_PRI, 0, param);	
	status=pthread_create(&copy_thr[*param].hndl,NULL,copy_thr_main,(void*)param);
	if(status!=OSA_SOK) 
	{
		OSA_ERROR("OSA_tskCreate()");
		return status;
	}
	return status;
}
static void *copy_thr_main(void*param)
{
	u8 exit=0,get_stream_mode=GET_STREAM_NULL;
	key_t keyval;	
	char pr_name[10];
	copy_ctl copy_hdl;
	_thr_msg_data *copy_data;
	copy_hdl.copy_id=*((u8*)param);
	keyval=ftok(".",PLAYER_THR_IPC_KEY_ID);//获取一个消息队列的键值
	copy_hdl.msg_id=msgget(keyval,IPC_CREAT|0666);//创建一个消息队列,并且设置读写权限。如果消息队列已经存在则打开
	sprintf(pr_name,"copy%d",copy_hdl.copy_id);	
	prctl(PR_SET_NAME, pr_name);
	DEBUG_PF("copy id:%d run",copy_hdl.copy_id);
	copy_hdl.discard_num=0;
	copy_hdl.single_pic=0;
	copy_hdl.cap_res=0;
	copy_hdl.buf_360p=copy_hdl.buf_720p=NULL;
	copy_hdl.copy_thr_sta=COPY_THR_STA_STANDBY;
	copy_data=&copy_hdl.copy_msgbuf.thr_data;
	if((copy_hdl.buf_720p= (u8*)cedar_sys_phymalloc(1280*(720+360),4))==NULL)
	{
		OSA_ERROR("request back buf 720p fail!!!");
		return NULL;
	}
	if((copy_hdl.buf_360p= (u8*)cedar_sys_phymalloc(640*(360+180),4))==NULL)
	{
		OSA_ERROR("request back buf 360p fail!!!");
		return NULL;
	}
#ifdef PLAYER_TEST
	copy_test[copy_hdl.copy_id]=&copy_hdl;
#endif			
	while(!exit)
	{
		if(-1==(msgrcv(copy_hdl.msg_id,&copy_hdl.copy_msgbuf,sizeof(_thr_msg_data),copy_msg_type[copy_hdl.copy_id],0)))
		{
			OSA_ERROR("copy rcv msg fail ,str error: %s",strerror(errno));
		}	
		else
		{
///////////////////////////****************************///////////////////////////////////////
			switch(copy_data->cmd)
			{
				case COPY_CMD_NEW_DATA:
					if(COPY_THR_STA_WORK_ON==copy_hdl.copy_thr_sta)
					{
						int single_ch=copy_get_single_channel();
						if(copy_clr_data(copy_hdl.copy_id)
							||((single_ch>=0)&&(single_ch!=copy_hdl.copy_id))
							|| (copy_data->play_id!=copy_get_play_id(copy_hdl.copy_id)))
						{//清除数据或者是这个通道不是全屏播放时的通道这丢弃帧
							cedarv_picture_t picture;	
							if(!decode_thr_display_request(copy_hdl.copy_id,&picture))
							{
								decode_thr_release_dis(copy_hdl.copy_id,picture.id); 
							}
							copy_hdl.discard_num = 4;
						}
						else
						{
							copy_data_pro(&copy_hdl);
						}
					}
					break;
				case COPY_CMD_IGNORE_DATA:
					if(COPY_THR_STA_WORK_ON==copy_hdl.copy_thr_sta)
					{
						cedarv_picture_t picture;	
						if(!decode_thr_display_request(copy_hdl.copy_id,&picture))
						{
							decode_thr_release_dis(copy_hdl.copy_id,picture.id); 
						}
					}
					break;
				case COPY_CMD_BLACK:
					if(COPY_THR_STA_WORK_ON==copy_hdl.copy_thr_sta)
					{
						DEBUG_PF("copy:%d rcv black cmd",copy_hdl.copy_id);
						copy_black_screen_pro(&copy_hdl);
					}
					break;
				case COPY_CMD_STANDBY:
					if((COPY_THR_STA_WORK_ON==copy_hdl.copy_thr_sta)
						||(COPY_THR_STA_STANDBY==copy_hdl.copy_thr_sta))
					{
						DEBUG_PF("copy id:%d rcv standby cmd at sta:%d",copy_hdl.copy_id,copy_hdl.copy_thr_sta);	
						copy_hdl.copy_thr_sta=COPY_THR_STA_STANDBY;		
						copy_data->channel=copy_hdl.copy_id;
						copy_set_clr_cmd(copy_hdl.copy_id,0);
						if(dis_put_msg_to_queue(copy_data,DISPLAY_CMD_STANDBY,0))
						{
							OSA_ERROR("copy send msg fail!!!");								
						}
					}
					break;			
				case COPY_CMD_FRAME_SYNC:
					if((COPY_THR_STA_WORK_ON==copy_hdl.copy_thr_sta)
						||(COPY_THR_STA_STANDBY==copy_hdl.copy_thr_sta))
					{
						DEBUG_PF("copy %d rcv frame sync at sta:%d",copy_hdl.copy_id,copy_hdl.copy_thr_sta);		
						copy_hdl.discard_num=copy_data->discard_num;//解码库中有3帧
					}
					break;		
				case DISPLAY_CMD_STANDBY_ALREADY:
					if(COPY_THR_STA_STANDBY==copy_hdl.copy_thr_sta)
					{					
						DEBUG_PF("copy %d rcv dis standby already cmd",copy_hdl.copy_id);
						if(fluency_put_msg_to_queue(copy_hdl.copy_id,copy_data,COPY_CMD_STANDBY_ALREADY,0))
						{
							OSA_ERROR("copy send msg fail!!!"); 							
						}
					}
					break;		
				case DISPLAY_CMD_WORK_ON_ALREADY:
					if(COPY_THR_STA_STANDBY==copy_hdl.copy_thr_sta)
					{					
						DEBUG_PF("copy %d rcv dis work on alreadby cmd",copy_hdl.copy_id);								
						if((get_stream_mode!=copy_data->record)&&(GET_STREAM_NULL!=get_stream_mode))
						{
							copy_hdl.discard_num=3;//切换取流方式时，需要丢掉解码库中的3帧
						}						
						get_stream_mode=copy_data->record;
						copy_hdl.copy_thr_sta=COPY_THR_STA_WORK_ON;
						copy_cat_num[copy_hdl.copy_id]=0;
						if(fluency_put_msg_to_queue(copy_hdl.copy_id,copy_data,COPY_CMD_WORK_ON_ALREADY,0))
						{
							OSA_ERROR("copy send msg fail!!!"); 							
						}						
					}
					break;		
				case COPY_CMD_WORK_ON:
					if(COPY_THR_STA_STANDBY==copy_hdl.copy_thr_sta)
					{					
						DEBUG_PF("copy id:%d rcv work on cmd",copy_hdl.copy_id);
						copy_data->channel=copy_hdl.copy_id;
						if(dis_put_msg_to_queue(copy_data,DISPLAY_CMD_WORK_ON,0))
						{
							OSA_ERROR("copy send msg fail!!!"); 							
						}	
					}
					break;
				case COPY_CMD_WAIT_CMD:
					DEBUG_PF("copy:%d rcv wait cmd at sta:%d",copy_hdl.copy_id,copy_hdl.copy_thr_sta);
					copy_set_clr_cmd(copy_hdl.copy_id,0);					
					if(dis_put_msg_to_queue(copy_data,DISPLAY_CMD_WAIT_CMD,0))
					{
						OSA_ERROR("send msg fail!!!");
					}
					break;
				case DISPLAY_CMD_WAIT_CMD_ALREADY:
					DEBUG_PF("copy:%d rcv dis wait cmd already at sta:%d",copy_hdl.copy_id,copy_hdl.copy_thr_sta);
					if(fluency_put_msg_to_queue(copy_hdl.copy_id,copy_data,COPY_CMD_WAIT_CMD_ALREADY,0))
					{
						OSA_ERROR("send msg fail!!!");
					}					
					break;				
				case COPY_CMD_PUT_BACK_BUF:
					DEBUG_PF("copy%d rcv put back buf cmd",copy_hdl.copy_id);							
					if(COPY_THR_STA_WORK_ON==copy_hdl.copy_thr_sta)
					{
						dis_thr_slip_scn_buf_pro(copy_hdl.copy_id,NULL,NULL,0,1);
					}
					break;
				case COPY_CMD_EXIT:
					if(COPY_THR_STA_STANDBY==copy_hdl.copy_thr_sta)
					{					
						DEBUG_PF("copy :%d rcv exit cmd",copy_hdl.copy_id);
						exit=1;
					}
					break;
				default:;
			}
///////////////////////////****************************///////////////////////////////////////	
		}
	}		
	if(copy_hdl.buf_360p!=NULL)
	{
		cedar_sys_phyfree(copy_hdl.buf_360p);
	}
	if(copy_hdl.buf_720p!=NULL)
	{
		cedar_sys_phyfree(copy_hdl.buf_720p);
	}
	pthread_mutex_lock(&all_decode_lock);
	enc_exit();
	pthread_mutex_unlock(&all_decode_lock);
	DEBUG_PF("copy %d thr exit",copy_hdl.copy_id);		
	return NULL;
}
int copy_thr_delete(u8 param)
{
	static int cnt=0;
	int status=0;
	if(!cnt)
	{
		cnt=1;		
		close(fd_g2d);
		status|=player_mutex_delete(&msg_lock);		
		if(status)
		{
			OSA_ERROR("player_mutex_delete:%d!!!",status);
		}
	}
	OSA_thrDelete(&copy_thr[param]);
	return OSA_SOK;
}

static void copy_mb_2normal(u32 Width,u32 Height,u8*src,u8*dst)
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
static void copy_720p_MB2normal(u8 *ysrc,u8*uvsrc,u8*dst)
{
	copy_mb_2normal(1280,720,ysrc,dst);
	copy_mb_2normal(1280,360,uvsrc,dst+1280*720);
}
static s8 copy_720_rsz_360(u8 *src_720p,u8*dst_360)
{
	g2d_stretchblt str;
	int ret=0;	
	memset(&str,0,sizeof(g2d_stretchblt));
	str.src_image.addr[0]=mem_get_phy_addr(src_720p);
	str.src_image.addr[1]=mem_get_phy_addr(src_720p+1280*720);
	str.src_image.w=1280;
	str.src_image.h=720;
	str.src_image.format=G2D_FMT_PYUV420UVC;
	str.src_image.pixel_seq=G2D_SEQ_NORMAL;
	str.src_rect.x=0;
	str.src_rect.y=0;
	str.src_rect.w=1280;
	str.src_rect.h=720;

	str.dst_image.addr[0]=mem_get_phy_addr(dst_360);
	str.dst_image.addr[1]=mem_get_phy_addr(dst_360+640*360);		
	str.dst_image.w=640;
	str.dst_image.h=360;
	str.dst_image.format=G2D_FMT_PYUV420UVC;
	str.dst_image.pixel_seq=G2D_SEQ_NORMAL;
	str.dst_rect.x=0;
	str.dst_rect.y=0;
	str.dst_rect.w=640;
	str.dst_rect.h=360;

	str.flag=G2D_BLT_NONE;	
	if((ret=ioctl(fd_g2d, G2D_CMD_STRETCHBLT,&str))<0)
	{
		OSA_ERROR("G2D_CMD_STRETCHBLT failed!\n");				
		return ret;
	}	
	return ret;
}
static void copy_normal2MB(u32 height,u32 width,u8 *normal_src,u8*MB_dst)
{
	u32 i,j,m,offset=0,height32=height/32,width32=width/32,remain_height=height-height32*32;	
	u8 *dst_asm,*src_asm,*src=normal_src,*dst=MB_dst;
	for(i=0;i<height32;i++)
	{
		for(j=0;j<32;j++)
		{
			for(m=0;m<width32;m++)
			{			
				offset =32*32*m+j*32+width32*32*32*i;
				src_asm = src;
				dst_asm = dst+offset;
				asm volatile (
				        "vld1.8         {d0 - d3}, [%[src_asm]]              \n\t"
				        "vst1.8         {d0 - d3}, [%[dst_asm]]              \n\t"
				        : [dst_asm] "+r" (dst_asm), [src_asm] "+r" (src_asm)
				        :  //[srcY] "r" (srcY)
				        : "cc", "memory", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d16", "d17", "d18", "d19", "d20", "d21", "d22", "d23", "d24", "d28", "d29", "d30", "d31"
				        );
				src +=32;
			}
		}
	}
	for(j=0;j<remain_height;j++)
	{
		for(m=0;m<width32;m++)
		{			
			offset =32*32*m+j*32+width32*32*32*height32;
			src_asm = src;
			dst_asm = dst+offset;
			asm volatile (
					"vld1.8 		{d0 - d3}, [%[src_asm]] 			 \n\t"
					"vst1.8 		{d0 - d3}, [%[dst_asm]] 			 \n\t"
					: [dst_asm] "+r" (dst_asm), [src_asm] "+r" (src_asm)
					:  //[srcY] "r" (srcY)
					: "cc", "memory", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d16", "d17", "d18", "d19", "d20", "d21", "d22", "d23", "d24", "d28", "d29", "d30", "d31"
					);
			src +=32;
		}
	}
}
static void copy_360p_normal2MB(u8 *src,u8*ydst,u8*uvdst)
{
	copy_normal2MB(360,640,src,ydst);
	copy_normal2MB(180,640,src+640*360,uvdst);
}
static void copy_black_screen_pro(copy_ctl *hdl)
{
	int single_ch=copy_get_single_channel();
	if(single_ch>=0)
	{		
		DEBUG_PF("channel:%d black",hdl->copy_id);
		if(single_ch==hdl->copy_id)
		{
			dis_thr_full_scn_buf_pro(hdl->copy_id,NULL,NULL,1);
		}
		if(copy_thr_copy_image(hdl->copy_id,3,NULL,NULL,dis_back_buf))
		{
			OSA_ERROR("copy_thr_copy_image failed!");							
		}
	}
	else if(single_ch<0)
	{	
		DEBUG_PF("channel:%d black",hdl->copy_id);
		dis_thr_slip_scn_buf_pro(hdl->copy_id,NULL,NULL,1,0);
	}
	else
	{
		DEBUG_PF("copy%d black no this resolution",hdl->copy_id);
	}
}
static void copy_data_pro(copy_ctl *hdl)
{
	cedarv_picture_t picture;	
	u8 *ysrc_com,*uvsrc_com;
	if(decode_thr_display_request(hdl->copy_id,&picture))
	{//没有可以显示的数据
		OSA_ERROR("copy%d no data cann't get to display",hdl->copy_id);
		return ;
	}
	if(hdl->discard_num)
	{
		hdl->discard_num--;
		DEBUG_PF("copy:%d discard this frame,discard num:%d",hdl->copy_id,hdl->discard_num);
		decode_thr_release_dis(hdl->copy_id,picture.id); 
		return ;
	}
///////////////////////////////////////
	copy_set_pts(hdl->copy_id, picture.pts);
	ysrc_com=(u8*)cedarv_address_phy2vir(picture.y);
	uvsrc_com=(u8*)cedarv_address_phy2vir(picture.u);
	if(720==picture.height)
	{				
		cedarx_cache_op((long)ysrc_com,(long)ysrc_com+1280*736,1);
		cedarx_cache_op((long)uvsrc_com,(long)uvsrc_com+1280*384,1);
		if(copy_get_single_channel()==hdl->copy_id)
		{
			if(copy_get_cat_num(hdl->copy_id))
			{
				copy_dec_cat_num(hdl->copy_id);
				copy_capture_pic(hdl,1280,720,ysrc_com,uvsrc_com);
			}			
			dis_thr_full_scn_buf_pro(hdl->copy_id,ysrc_com,uvsrc_com,0);
		}
		else if(copy_get_single_channel()<0)
		{
			u8*buf=copy_thr_720rsz360(ysrc_com,uvsrc_com,hdl->buf_720p,hdl->buf_360p);
			cedarx_cache_op((u32)buf,(u32)buf+640*(384+192),1);	
			if(copy_get_cat_num(hdl->copy_id))
			{
				copy_dec_cat_num(hdl->copy_id);
				copy_capture_pic(hdl,640,360,buf,buf+640*384);
			}
			dis_thr_slip_scn_buf_pro(hdl->copy_id,buf,buf+640*384,0,0);
		}
	}
	else if(360==picture.height)
	{					
		cedarx_cache_op((long)ysrc_com,(long)ysrc_com+640*384,1);
		cedarx_cache_op((long)uvsrc_com,(long)uvsrc_com+640*192,1);
		if(copy_get_cat_num(hdl->copy_id))
		{
			copy_dec_cat_num(hdl->copy_id);
			copy_capture_pic(hdl,640,360,ysrc_com,uvsrc_com);
		}		
		dis_thr_slip_scn_buf_pro(hdl->copy_id,ysrc_com,uvsrc_com,0,0);
	}
	else
	{
		DEBUG_PF("copy:%d no this resolution:%d",hdl->copy_id,picture.height);
		decode_thr_release_dis(hdl->copy_id,picture.id); 
		return ;
	}	
	decode_thr_release_dis(hdl->copy_id,picture.id);	
	return;
}
static u8* copy_thr_720rsz360(u8*ysrc,u8*uvsrc,u8*dst720,u8*dst360)
{
	copy_720p_MB2normal((ysrc),(uvsrc),dst720);
	cedarx_cache_op((u32)dst720,(u32)dst720+1280*(720+360),1);
	if(copy_720_rsz_360(dst720,dst360))
	{
		OSA_ERROR("copy_720_rsz_360 fail");
	}
	cedarx_cache_op((u32)dst360,(u32)dst360+640*(360+180),1);
	copy_360p_normal2MB(dst360,dst720,dst720+640*384);
	return dst720;
}
int copy_thr_copy_image(u8 ch_id,u8 func,u8 *y_src,u8*uv_src,u8 *dst)
{
	int ret=0;
	g2d_blt blit;
//	u8 *dst_back=dst;
	if(!func)
	{//720P拷贝
		memset(&blit,0,sizeof(g2d_blt));
		blit.src_image.addr[0] = mem_get_phy_addr((u32)y_src);
		//blit.src_image.addr[1] = blit.src_image.addr[0]+1280*720;
		blit.src_image.w = 1280;
		blit.src_image.h = 736;
		blit.src_image.format = G2D_FMT_8BPP_MONO;
		blit.src_image.pixel_seq =G2D_SEQ_NORMAL;
		blit.src_rect.w = 1280;
		blit.src_rect.h = 736;
	
		blit.src_rect.x = 0;
		blit.src_rect.y = 0;
		blit.dst_x=0;
		blit.dst_y=0;
		blit.dst_image.addr[0] = mem_get_phy_addr((u32)dst);
		//blit.dst_image.addr[1] = blit.dst_image.addr[0]+1280*720;
		blit.dst_image.w = 1280;
		blit.dst_image.h = 736;
		blit.dst_image.format =G2D_FMT_8BPP_MONO;
		blit.dst_image.pixel_seq = G2D_SEQ_NORMAL;

		blit.flag = G2D_BLT_NONE;
		if((ret=ioctl(fd_g2d, G2D_CMD_BITBLT,&blit))<0)
		{
			OSA_ERROR("G2D_CMD_BITBLT failed!");				
			//return ret;
		}			

		dst+=1280*736;
		memset(&blit,0,sizeof(g2d_blt));
		blit.src_image.addr[0] = mem_get_phy_addr((u32)uv_src);
		//blit.src_image.addr[1] = blit.src_image.addr[0]+1280*720;
		blit.src_image.w = 1280;
		blit.src_image.h = 384;
		blit.src_image.format = G2D_FMT_8BPP_MONO;
		blit.src_image.pixel_seq =G2D_SEQ_NORMAL;
		blit.src_rect.w = 1280;
		blit.src_rect.h = 384;
	
		blit.src_rect.x = 0;
		blit.src_rect.y = 0;
		blit.dst_x=0;
		blit.dst_y=0;
		blit.dst_image.addr[0] = mem_get_phy_addr((u32)dst);
		//blit.dst_image.addr[1] = blit.dst_image.addr[0]+1280*720;
		blit.dst_image.w = 1280;
		blit.dst_image.h = 384;
		blit.dst_image.format =G2D_FMT_8BPP_MONO;
		blit.dst_image.pixel_seq = G2D_SEQ_NORMAL;

		blit.flag = G2D_BLT_NONE;
		if((ret=ioctl(fd_g2d, G2D_CMD_BITBLT,&blit))<0)
		{
			OSA_ERROR("G2D_CMD_BITBLT failed!");				
			//return ret;
		}
	}
	else if(1==func)
	{////////从备份buffer将数据拷贝到显示buffer
		u8 i,*y_dst,*uv_dst,j,*y_src_back,*uv_src_back;
		y_src_back=y_src;
		uv_src_back=uv_src;
		for(i=0;i<FLUENCY_CHANNEL_MAX;i++)
		{
			switch(ch_id)
			{
				case 0:
					y_dst=dst;
					uv_dst=dst+1280*720;
					y_src=y_src_back;
					uv_src=uv_src_back;
					break;									
				case 1:
					y_dst=dst+640*32;
					uv_dst=dst+1280*720+640*32;
					y_src=y_src_back+640*32;
					uv_src=uv_src_back+640*32;
					break;										
				case 2:
					y_dst=dst+1280*360;
					uv_dst=dst+1280*900;
					y_src=y_src_back+1280*360;
					uv_src=uv_src_back+1280*180;					
					break;
				case 3:
					y_dst=dst+1280*360+640*32;
					uv_dst=dst+1280*900+640*32;
					y_src=y_src_back+1280*360+640*32;
					uv_src=uv_src_back+1280*180+640*32;	
					break;
				default:;
			}
			if(i==ch_id)
			{
				continue;
			}
			///////////Y数据拷贝11个32行
			for(j=0;j<11;j++)
			{
				//memset(&blit,0,sizeof(g2d_blt));
				blit.src_image.addr[0] = mem_get_phy_addr((u32)y_src);
				//blit.src_image.addr[1] = blit.src_image.addr[0]+1280*720;
				blit.src_image.w = 640;
				blit.src_image.h = 32;
				blit.src_image.format = G2D_FMT_8BPP_MONO;
				blit.src_image.pixel_seq =G2D_SEQ_NORMAL;
				blit.src_rect.w = 640;
				blit.src_rect.h = 32;
				
				blit.src_rect.x = 0;
				blit.src_rect.y = 0;

				blit.dst_image.addr[0] = mem_get_phy_addr((u32)y_dst);
				//blit.dst_image.addr[1] = blit.dst_image.addr[0]+1280*720;
				blit.dst_image.w = 640;
				blit.dst_image.h = 32;
				blit.dst_image.format =G2D_FMT_8BPP_MONO;
				blit.dst_image.pixel_seq = G2D_SEQ_NORMAL;
				
				blit.dst_x=0;
				blit.dst_y=0;
				blit.flag = G2D_BLT_NONE;
				if((ret=ioctl(fd_g2d, G2D_CMD_BITBLT,&blit))<0)
				{
					OSA_ERROR("G2D_CMD_STRETCHBLT failed!");				
				}		
				y_src+=1280*32;
				y_dst+=1280*32;
			}			
			///////////Y数据拷贝8个32行
			for(i=0;i<20;i++)
			{
				memcpy(y_dst,y_src,32*8);
				y_dst+=32*32;
				y_src+=32*32;							
			}
			////////////uv数据拷贝5个32行
			for(i=0;i<5;i++)
			{
				blit.src_image.addr[0] = mem_get_phy_addr((u32)uv_src);
				blit.src_image.w = 640;
				blit.src_image.h = 32;
				blit.src_image.format = G2D_FMT_8BPP_MONO;
				blit.src_image.pixel_seq =G2D_SEQ_NORMAL;
				blit.src_rect.w = 640;
				blit.src_rect.h = 32;
				blit.src_rect.x = 0;
				blit.src_rect.y = 0;
			
				blit.dst_image.addr[0] = mem_get_phy_addr((u32)uv_dst);
				blit.dst_image.w = 640;
				blit.dst_image.h = 32;
				blit.dst_image.format =G2D_FMT_8BPP_MONO;
				blit.dst_image.pixel_seq = G2D_SEQ_NORMAL;
				blit.dst_x=0;
				blit.dst_y=0;
				blit.flag = G2D_BLT_NONE;
				if(ioctl(fd_g2d, G2D_CMD_BITBLT,&blit)<0)
				{
					OSA_ERROR("G2D_CMD_BITBLT failed!");				
				}	
				uv_src+=1280*32;
				uv_dst+=1280*32;
			}
			//UV数据拷贝剩下的20行
			for(i=0;i<20;i++)
			{
				memcpy(uv_dst,uv_src,32*20);
				uv_dst+=32*32;
				uv_src+=32*32;							
			}
		}
	}		
	else if(2==func)
	{//将四分屏幕拷贝到显示buf或者备份buf,灰度拷贝
		u8 i,*y_dst,*uv_dst;
		switch(ch_id)
		{
			case 0:
				y_dst=dst;
				uv_dst=dst+1280*736;
				break;									
			case 1:
				y_dst=dst+640*32;
				uv_dst=dst+1280*736+640*32;
				break;										
			case 2:
				y_dst=dst+1280*352;
				uv_dst=dst+1280*736 + 1280*160;
				break;
			case 3:
				y_dst=dst+1280*352+640*32;
				uv_dst=dst+1280*736+1280*160+640*32;
				break;
			default:;
		}
		//Y数据拷贝12个32行
		if(ch_id<=1)
		{
			for(i=0;i<11;i++)
			{
				memcpy(y_dst,y_src,640*32);
				y_dst+=1280*32;
				y_src+=640*32;
			}
			//Y数据拷贝剩下的8行
			for(i=0;i<20;i++)
			{
				memcpy(y_dst,y_src,32*8);
				y_dst+=32*32;
				y_src+=32*32; 							
			}
		}
		else
		{
			for(i=0;i<11;i++)
			{	
				int j;
				u8 *src = y_src;
				u8 *dst;
				for(j=0;j<20;j++)
				{
					memcpy(y_dst+256,y_src,32*24);
					y_dst+=32*32;
					y_src+=32*32; 							
				}
				y_dst+=640*32;
				dst = y_dst;
				y_src = src;
				for(j=0;j<20;j++)
				{
					memcpy(y_dst,y_src+768,32*8);
					y_dst+=32*32;
					y_src+=32*32; 							
				}
				y_dst = dst;

				if(i == 10)
				{
					for(j=0;j<20;j++)
					{
						memcpy(y_dst+256,y_src,32*8);
						y_dst+=32*32;
						y_src+=32*32; 							
					}
					y_dst+=640*32;
				}					
			}
		}
		if(ch_id<=1)
		{
			//UV数据拷贝5个32行
			for(i=0;i<5;i++)
			{
				memcpy(uv_dst,uv_src,640*32);
				uv_dst+=1280*32;
				uv_src+=640*32;
			}	
			//UV数据拷贝剩下的20行
			for(i=0;i<20;i++)
			{
				memcpy(uv_dst,uv_src,32*20);
				uv_dst+=32*32;
				uv_src+=32*32; 							
			}
		}else
		{
			//UV数据拷贝5个32行
			for(i=0;i<5;i++)
			{
				int j;
				u8 *src = uv_src;
				u8 *dst;
				//UV数据拷贝12行
				for(j=0;j<20;j++)
				{
					memcpy(uv_dst+640,uv_src,32*12);
					uv_dst+=32*32;
					uv_src+=32*32; 							
				}
				uv_dst+= 640*32;
				dst = uv_dst;
				uv_src = src;
				//UV数据拷贝剩下的20行				
				for(j=0;j<20;j++)
				{
					memcpy(uv_dst,uv_src+384,32*20);
					uv_dst+=32*32;
					uv_src+=32*32; 							
				}
				uv_dst = dst;
				if(i == 4)
				{
					src = uv_src;
					for(j=0;j<20;j++)
					{
						memcpy(uv_dst+640,uv_src,32*12);
						uv_dst+=32*32;
						uv_src+=32*32; 							
					}
					
					uv_src = src;
					uv_dst+= 640*32;
					for(j=0;j<20;j++)
					{
						memcpy(uv_dst,uv_src+384,32*8);
						uv_dst+=32*32;
						uv_src+=32*32; 							
					}						
				}
			}		
		}
	}
	else if(3==func)
	{
		u8 i,*y_dst,*uv_dst;
		switch(ch_id)
		{
			case 0:
				y_dst=dst;
				uv_dst=dst+1280*736;
				break;									
			case 1:
				y_dst=dst+640*32;
				uv_dst=dst+1280*736+640*32;
				break;										
			case 2:
				y_dst=dst+1280*352;
				uv_dst=dst+1280*736 + 1280*160;
				break;
			case 3:
				y_dst=dst+1280*352+640*32;
				uv_dst=dst+1280*736+1280*160+640*32;
				break;
			default:;
		}
		//Y数据拷贝12个32行
		if(ch_id<=1)
		{
			for(i=0;i<11;i++)
			{
				memset(y_dst,0,640*32);//memcpy(y_dst,y_src,640*32);
				y_dst+=1280*32;
			}
			//Y数据拷贝剩下的8行
			for(i=0;i<20;i++)
			{
				memset(y_dst,0,32*8);//memcpy(y_dst,y_src,32*8);
				y_dst+=32*32;
			}
		}
		else
		{
			for(i=0;i<11;i++)
			{	
				int j;
				u8 *dst;
				for(j=0;j<20;j++)
				{
					memset(y_dst+256,0,32*24);//memcpy(y_dst+256,y_src,32*24);
					y_dst+=32*32;
				}
				y_dst+=640*32;
				dst = y_dst;
				for(j=0;j<20;j++)
				{
					memset(y_dst,0,32*8);//memcpy(y_dst,y_src+768,32*8);
					y_dst+=32*32;
				}
				y_dst = dst;

				if(i == 10)
				{
					for(j=0;j<20;j++)
					{
						memset(y_dst+256,0,32*8);//memcpy(y_dst+256,y_src,32*8);
						y_dst+=32*32;
					}
					y_dst+=640*32;
				}					
			}
		}
		if(ch_id<=1)
		{
			//UV数据拷贝5个32行
			for(i=0;i<5;i++)
			{
				memset(uv_dst,0x80,640*32);//memcpy(uv_dst,uv_src,640*32);
				uv_dst+=1280*32;
			}	
			//UV数据拷贝剩下的20行
			for(i=0;i<20;i++)
			{
				memset(uv_dst,0x80,32*20);//memcpy(uv_dst,uv_src,32*20);
				uv_dst+=32*32;
			}
		}else
		{
			//UV数据拷贝5个32行
			for(i=0;i<5;i++)
			{
				int j;
				u8 *dst;
				//UV数据拷贝12行
				for(j=0;j<20;j++)
				{
					memset(uv_dst+640,0x80,32*12);//memcpy(uv_dst+640,uv_src,32*12);
					uv_dst+=32*32;
				}
				uv_dst+= 640*32;
				dst = uv_dst;
				//UV数据拷贝剩下的20行				
				for(j=0;j<20;j++)
				{
					memset(uv_dst,0x80,32*20);//memcpy(uv_dst,uv_src+384,32*20);
					uv_dst+=32*32;
				}
				uv_dst = dst;
				if(i == 4)
				{
					for(j=0;j<20;j++)
					{
						memset(uv_dst+640,0x80,32*12);//memcpy(uv_dst+640,uv_src,32*12);
						uv_dst+=32*32;
					}
					uv_dst+= 640*32;
					for(j=0;j<20;j++)
					{
						memset(uv_dst,0x80,32*8);//memcpy(uv_dst,uv_src+384,32*8);
						uv_dst+=32*32;
					}						
				}					
			}		
		}
	}
	return ret;
}
void copy_set_single_channel(int ch)
{	
	g_atomic_int_set(&copy_single_ch,(gint)ch);
}
static int copy_get_single_channel(void)
{
	return (int)g_atomic_int_get(&copy_single_ch);
}
void copy_set_clr_cmd(u8 ch,u8 flag)
{	
	g_atomic_int_set(&copy_cmd[ch],(gint)flag);
}
static u8 copy_clr_data(u8 ch)
{
	u8 cmd=0;
	cmd=(u8)g_atomic_int_get(&copy_cmd[ch]);
	return cmd;
}
void copy_add_cat_num(u8 ch,u8 num)
{
	g_atomic_int_add(&copy_cat_num[ch],(gint)num);
}
static u8 copy_get_cat_num(u8 ch)
{
	return (u8)g_atomic_int_get(&copy_cat_num[ch]);
}
static void copy_dec_cat_num(u8 ch)
{
	g_atomic_int_dec_and_test(&copy_cat_num[ch]);
}
void copy_capture_pic(copy_ctl*hdl,u16 width,u16 height,u8*ysrc,u8*uvsrc)
{
	VencInputBuffer input_buffer;
	static int cnt=0;
	char name[64] = {0};
	int ret = 0;
	
	pthread_mutex_lock(&all_decode_lock);	
	if(hdl->cap_res!=height)
	{	
		hdl->cap_res=height;
		enc_exit();
		enc_init(width,height);
	}
	if(enc_get_input_buf(&input_buffer))
	{
		OSA_ERROR("copy:%d enc_get_input_buf fail",hdl->copy_id);
	}
	else
	{
		enc_mb2_nonmb(width,height,ysrc,uvsrc,input_buffer.addrvirY,input_buffer.addrvirC);
		sprintf(name,"/tmp/ch%d_%d.jpeg",hdl->copy_id,cnt);
		ret = enc_jpeg(width,height,&input_buffer,name);
		if(0 == ret)
		{
			time_t val;
			val = time(NULL);
			unsigned long long pts = val*1000; //ms
			//printf("!!!!!!!!!!!!!!!!!!!!!!pts: %llu,jpegPath: %s\n",pts,name);
			ret = storage_lib_save_jpeg_by_path(hdl->copy_id, 0, name, pts);
			if(ret < 0)
				printf("save %s to disk fail!!!\n", name);								
		}
		cnt++;
	}
	pthread_mutex_unlock(&all_decode_lock); 		
}
void copy_set_play_id(u8 ch, u32 play_id)
{
	g_atomic_int_set(&copy_play_id[ch], (gint)play_id);
}
u32 copy_get_play_id(u8 ch)
{
	return (u32)g_atomic_int_get(&copy_play_id[ch]);
}
void copy_set_pts(u8 ch, u64 pts)
{
	pthread_mutex_lock(&msg_lock);
	//g_atomic_int_set(&copy_frame_pts[ch], pts);
	copy_frame_pts[ch]=pts;
	pthread_mutex_unlock(&msg_lock);
}
u64 copy_get_pts(u8 ch)
{
	u64 pts;
	pthread_mutex_lock(&msg_lock);
	pts=copy_frame_pts[ch];
	pthread_mutex_unlock(&msg_lock);
	return pts;
}
s8 copy_put_msg_to_queue(u8 ch,_thr_msg_data *data,u8 cmd,int flag)
{
	_thr_msg send_buf;
	static char init=0;
	static int msg_id;
	key_t keyval;	
	pthread_mutex_lock(&msg_lock);
	if(!init)
	{
		init=1;
		if(-1==(keyval=ftok(".",PLAYER_THR_IPC_KEY_ID)))//获取一个消息队列的键值
		{
			OSA_ERROR("get keyval fail");
			pthread_mutex_unlock(&msg_lock);	
			return -1;
		}
		if(-1==(msg_id=msgget(keyval,IPC_CREAT|0666)))
		{
			OSA_ERROR("get msgid fail");
			pthread_mutex_unlock(&msg_lock);	
			return -1;
		}
	}
	send_buf.msg_type=copy_msg_type[ch];
	memcpy(&send_buf.thr_data,data,sizeof(_thr_msg_data));
	send_buf.thr_data.cmd=cmd;
	if(msgsnd(msg_id,&send_buf,sizeof(_thr_msg_data),flag))
	{
		OSA_ERROR("copy%d send cmd:%d fail!!!",ch,cmd);								
		pthread_mutex_unlock(&msg_lock);	
		return -1;
	}
	pthread_mutex_unlock(&msg_lock);	
	return 0;
}

