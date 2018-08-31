#include<player.h>
#include<osa_thr.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdlib.h>
#include<dec_inc/libcedarv.h>
#include<linux/g2d_driver.h>
#include <stdio.h> // void perror(const char *msg);
#include <string.h> // char *strerror(int errnum);
#include <errno.h> //errno
#include <sys/time.h>
#include <signal.h> 
#include <render.h>
#include<sys/prctl.h>

enum{
	DIS_THR_STA_WORK_ON=0,
	DIS_THR_STA_STANDBY,
	DIS_THR_STA_EXIT,
};
typedef struct{
	u8 ch;	
	u8 get_buf_already;
	pthread_cond_t	cond_get_buf;	
}dis_buf_ctl;
typedef struct
{
	__disp_rect_t cur_pic_rect;
	__disp_rect_t cur_scr_rect;
	cedarv_picture_t picture_test;
	u8 dis_thr_sta;	
	u8 buf_id_loop[3];
	u8 loop_index;
	u8 single_pic;
	u8 dis_mode_720;//当前显示是整个720图片还是360的图片
	u8 standby_sta;
	u8 single_ch;
	u8 pic_mode;//当前显示的图片属性1:全屏通道的图片0:四分屏的图片
////////消息队列
	int msg_id;
	int msg_flg;
	_thr_msg dis_msgbuf;
////////////
}dis_ctl;

#define DISPLAY_BUFFER_NUM DISPLAY_BUF_NUM
#ifdef PLAYER_TEST
static dis_ctl* dis_test;
#endif
static volatile gint dis_cmd=0;
static OSA_ThrHndl diplay_thr;
static dis_buf_ctl buf_ctl[COPY_CHANNEL_MAX];
static pthread_mutex_t dis_lock,msg_lock;
volatile static u8 buf_use_reg=0x00,new_buf=0,updata=1,dis_buf_id;
u8 *dis_back_buf;

volatile dis_handle display_handle;
static void *display_thr_main(void*param);
static void dis_thr_buf_process_init(void);
static void dis_black_screen_pro(dis_ctl*hdl);
static s8 dis_display_pro(dis_ctl*hdl);
static s8 dis_get_full_buf(u8 *buf_id);
static s8 dis_put_empty_buf(dis_ctl*hdl,u8 buf_id);
static s8 dis_buf_cond_reg(dis_ctl*hdl);
static void dis_buf_cont_unreg(dis_ctl*hdl);
static void dis_parameter_init(dis_ctl *hdl);
static u8 dis_clr_data(void);
static void dis_set_temp_fullscreen(dis_ctl *hdl,u8 ch);
static void dis_set_normal_display(dis_ctl*hdl);
static s8 dis_display_mode_pro(dis_ctl*hdl,buf_head *pic_head);
static void dis_ele_rsz(dis_ctl *hdl);

int display_thr_create(void *param)
{
	int status;
	if(player_mutex_create(&dis_lock))
	{
		OSA_ERROR("player_mutex_create()!!!");
		return -1;
	}
	if(player_mutex_create(&msg_lock))
	{
		OSA_ERROR("player_mutex_create()!!!");
		return -1;
	}
	status = OSA_thrCreate(&diplay_thr, display_thr_main, DISPLAY_THR_PRI, 0, param);	
	//status=pthread_create(&diplay_thr.hndl,NULL,display_thr_main,(void*)param);	
	if(status!=OSA_SOK) 
	{
		OSA_ERROR("OSA_tskCreate()");	
		if(player_mutex_delete(&dis_lock))
		{
			OSA_ERROR("player_mutex_delete()!!!");
		}
		if(player_mutex_delete(&msg_lock))
		{
			OSA_ERROR("player_mutex_delete()!!!");
		}
		return status;
	}
	return status;
}


static void *display_thr_main(void*param)
{
	u8 exit=0;
	key_t keyval;	
	dis_ctl dis_hdl;
	_thr_msg_data *dis_data;
	keyval=ftok(".",PLAYER_THR_IPC_KEY_ID);//获取一个消息队列的键值
	dis_hdl.msg_id=msgget(keyval,IPC_CREAT|0666);//创建一个消息队列,并且设置读写权限。如果消息队列已经存在则打开
	dis_parameter_init(&dis_hdl);
	if(render_video_start_en(1))
	{
		OSA_ERROR("render video stop error");
		return NULL;
	}	
	if(dis_buf_cond_reg(&dis_hdl))
	{
		render_video_start_en(0);
		return NULL;
	}
	prctl(PR_SET_NAME,"dis");
	dis_data=&dis_hdl.dis_msgbuf.thr_data;
#ifdef PLAYER_TEST
	dis_test=&dis_hdl;
#endif	
	while(!exit)
	{		
		if(-1==(msgrcv(dis_hdl.msg_id,&dis_hdl.dis_msgbuf,sizeof(_thr_msg_data),DIS_MSG_TYPE_RCV,0)))//0表示收不到消息就阻塞
		{
			OSA_ERROR("dis rcv msg fail ,str error: %s",strerror(errno));
		}	
		else
		{
			u8 same_process=0;			
			switch(dis_data->cmd)
			{
				case DISPLAY_CMD_NEW_DATA:		
					if(DIS_THR_STA_WORK_ON==dis_hdl.dis_thr_sta)
					{
						if(0x0f==dis_clr_data())
						{
							u8 buf_id;
							if(dis_get_full_buf(&buf_id))
							{
								OSA_ERROR("dis_get_full_buf fail");
								dis_hdl.dis_thr_sta=DIS_THR_STA_STANDBY;	
							}
							if(dis_put_empty_buf(&dis_hdl,buf_id))
							{
								OSA_ERROR("dis_put_empty_buf fail");
								dis_hdl.dis_thr_sta=DIS_THR_STA_STANDBY;	
							}
							updata=1;								
							dis_thr_put_full_buf(dis_back_buf,dis_back_buf+1280*736);
						}
						else
						{
							if(dis_display_pro(&dis_hdl))
							{
								OSA_ERROR("dis_display_pro fail");
								dis_hdl.dis_thr_sta=DIS_THR_STA_STANDBY;	
							}
						}
					}
					break;
				case DISPLAY_CMD_WORK_ON:
					if(DIS_THR_STA_WORK_ON==dis_hdl.dis_thr_sta)
					{
						DEBUG_PF("dis rcv ch:%d work cmd at work",dis_data->channel);	
						same_process=1;									
					}
					else if(DIS_THR_STA_STANDBY==dis_hdl.dis_thr_sta)
					{
						DEBUG_PF("dis rcv ch:%d work on cmd at std",dis_data->channel);
						same_process=1;	
						player_dis_buf_clr();
						dis_hdl.loop_index=0;
						dis_hdl.dis_thr_sta=DIS_THR_STA_WORK_ON;//一旦收到work命令，则回到work状态						
					}
					if(same_process)
					{
						dis_set_clr_cmd(dis_data->channel,0);
						dis_hdl.standby_sta&=(~(1<<dis_data->channel));
						if(copy_put_msg_to_queue(dis_data->channel,dis_data,DISPLAY_CMD_WORK_ON_ALREADY,0))
						{
							OSA_ERROR("send msg fail!!!");								
						}
					}
					break;
				case DISPLAY_CMD_STANDBY:
					if((DIS_THR_STA_WORK_ON==dis_hdl.dis_thr_sta)||(DIS_THR_STA_STANDBY==dis_hdl.dis_thr_sta))
					{						
						DEBUG_PF("dis rcv ch:%d standby cmd at sta:%d!!!",dis_data->channel,dis_hdl.dis_thr_sta);			
						dis_hdl.standby_sta|=(1<<dis_data->channel);
						same_process=1;
					}
					if(DIS_THR_STA_WORK_ON==dis_hdl.dis_thr_sta)
					{
						if(dis_hdl.standby_sta==0x0f)
						{		
							dis_set_clr_cmd(dis_data->channel,0);
							if(dis_data->black_screen)
							{
								dis_black_screen_pro(&dis_hdl);
							}					
							dis_hdl.dis_thr_sta=DIS_THR_STA_STANDBY;
						}
					}
					if(same_process)
					{
						if(dis_hdl.standby_sta==0x0f)
						{													
							dis_thr_buf_process_init();
						}								
						if(copy_put_msg_to_queue(dis_data->channel,dis_data,DISPLAY_CMD_STANDBY_ALREADY,0))
						{
							OSA_ERROR("send msg fail!!!");								
						}
					}
					break;			
				case DISPLAY_CMD_WAIT_CMD:
					DEBUG_PF("dis rcv wait cmd at sta:%d",dis_hdl.dis_thr_sta);					
					dis_set_clr_cmd(dis_data->channel,0);
					if(copy_put_msg_to_queue(dis_data->channel,dis_data,DISPLAY_CMD_WAIT_CMD_ALREADY,0))
					{
						OSA_ERROR("send msg fail!!!");
					}
					break;
				case DISPLAY_CMD_SET_ELEC_RSZ:
					DEBUG_PF("dis rcv elec rsz cmd%d!!!",dis_data->elec_rsz);
					break;
				case DISPLAY_CMD_SET_SRC_RECT:					
					render_set_src_window(&dis_data->rect);	
					memcpy(&dis_hdl.cur_pic_rect,&dis_data->rect,sizeof(__disp_rect_t));
					dis_ele_rsz(&dis_hdl);
					break;
				case DISPLAY_CMD_SET_SCN_RECT:
					render_set_screen_window(&dis_data->rect);
					memcpy(&dis_hdl.cur_scr_rect,&dis_data->rect,sizeof(__disp_rect_t));
					break;
				case DISPLAY_CMD_TEMP_FULLSCREEN:
					dis_hdl.single_pic=1;
					if((dis_hdl.single_ch!=dis_data->channel)
						&&(COPY_CHANNEL_MAX==dis_hdl.single_ch))
					{							
						dis_hdl.dis_mode_720=0;
						dis_set_temp_fullscreen(&dis_hdl,dis_data->channel);
					}
					dis_hdl.single_ch=dis_data->channel;
					break;
				case DISPLAY_CMD_TEMP_SLIPSCREEN:
					dis_hdl.single_pic=0;
					dis_hdl.dis_mode_720=1;
					dis_set_normal_display(&dis_hdl);
					if((DIS_THR_STA_WORK_ON==dis_hdl.dis_thr_sta)
						&&(copy_put_msg_to_queue(dis_hdl.single_ch<COPY_CHANNEL_MAX?dis_hdl.single_ch:0,dis_data,COPY_CMD_PUT_BACK_BUF,0)))
					{
						OSA_ERROR("send msg fail!!!");
					}					
					dis_hdl.single_ch=COPY_CHANNEL_MAX;
					break;
				case DISPLAY_CMD_EXIT:
					DEBUG_PF("dis rcv exit cmd!!!");														
					exit=1;						
					break;
				default:;
			}
		}
	}		
	if(render_video_start_en(0))
	{
		OSA_ERROR("render video stop error");
	}
	dis_buf_cont_unreg(&dis_hdl);
	DEBUG_PF("display thr exit");
	return NULL;
}
int display_thr_delete(void)
{
	if(player_mutex_delete(&dis_lock))
	{
		OSA_ERROR("player_mutex_delete()!!!");
	}
	if(player_mutex_delete(&msg_lock))
	{
		OSA_ERROR("player_mutex_delete()!!!");
	}	
	OSA_thrDelete(&diplay_thr);
	return OSA_SOK;
}
static void dis_black_screen_pro(dis_ctl*hdl)
{
	memset(dis_back_buf,0,1280*736);
	memset(dis_back_buf+1280*736,0x80,1280*384);
	cedarx_cache_op((long)dis_back_buf,(long)(dis_back_buf+1280*(736+384)),1);
	hdl->picture_test.y=(u8*)mem_get_phy_addr((u32)dis_back_buf);//mem_in;
	hdl->picture_test.u=hdl->picture_test.y+1280*736;
	render_render(&hdl->picture_test,0);
	player_waitMsecs(50);//延时50ms等最后一帧黑屏显示完，才disable
}
static s8 dis_display_mode_pro(dis_ctl*hdl,buf_head *pic_head)
{
	s8 ret=0;
	if(hdl->single_pic)
	{//全屏显示
		if(!pic_head->single_pic)
		{//四分屏图片
			if(hdl->dis_mode_720)
			{//现在显示为全屏内容
				dis_set_temp_fullscreen(hdl,hdl->single_ch);
				hdl->dis_mode_720=0;
			}
		}
		else if(hdl->single_ch==pic_head->pic_ch)
		{//全屏图片
			if(!hdl->dis_mode_720)
			{//现在显示模式为四分屏内容放大
				dis_set_normal_display(hdl);
				hdl->dis_mode_720=1;
			}
		}
		else
		{
			ret=-1;
		}
	}
	else
	{//四分屏显示
		if(!pic_head->single_pic)
		{//四分屏图片
			if(!hdl->dis_mode_720)
			{//现在显示模式为四分屏内容放大
				dis_set_normal_display(hdl);
				hdl->dis_mode_720=1;				
			}
		}
		else
		{//全屏图片
			ret=-1;
		}
	}
	return ret;
}
static s8 dis_display_pro(dis_ctl*hdl)
{
	u8 buf_id;
	buf_head *pic_head;	
	if(dis_get_full_buf(&buf_id))
	{
		OSA_ERROR("dis_get_full_buf fail");
		return -1;
	}
	cedarx_cache_op((long)display_handle.cur_handle_buf.p_dest[buf_id],(u32)(display_handle.cur_handle_buf.p_dest[buf_id]+1280*(736+384)),1);
	pic_head=(buf_head*)(display_handle.cur_handle_buf.p_dest[buf_id]+DISPLAY_BUF_SIZE-sizeof(buf_head));
	if(!dis_display_mode_pro(hdl,pic_head))
	{
		hdl->picture_test.y=(u8*)mem_get_phy_addr((u32)display_handle.cur_handle_buf.p_dest[buf_id]);//mem_in;
		hdl->picture_test.u=hdl->picture_test.y+1280*736;
		render_render(&hdl->picture_test,buf_id);	
		if(hdl->pic_mode!=pic_head->single_pic)
		{
			hdl->pic_mode=pic_head->single_pic;
			dis_ele_rsz(hdl);
		}	
	}
	if(dis_put_empty_buf(hdl,buf_id))
	{
		OSA_ERROR("dis_put_empty_buf fail");
		return -1;
	}
	player_waitMsecs(DIS_TIME_LIMIT);
	updata=1;								
	dis_thr_put_full_buf(dis_back_buf,dis_back_buf+1280*736);
	return 0;
}
static s8 dis_get_full_buf(u8 *buf_id)
{
	if(player_get_full_buf((buf_handle *)&display_handle.cur_handle_buf,buf_id))
	{
		OSA_ERROR("dis get empty buf fail");
		return -1;
	}	
	return 0;
}
static s8 dis_put_empty_buf(dis_ctl*hdl,u8 buf_id)
{
	u8 i;
	hdl->buf_id_loop[hdl->loop_index]=buf_id;
	hdl->loop_index++;
	if(3==hdl->loop_index)
	{
		if(player_put_empty_buf((buf_handle*)&display_handle.cur_handle_buf,hdl->buf_id_loop[0]))
		{
			OSA_ERROR("put empty buf fail");
			return -1;
		}	
		hdl->loop_index--;
		for(i=0;i<hdl->loop_index;i++)
		{
			hdl->buf_id_loop[i]=hdl->buf_id_loop[i+1];
		}
	}
	return 0;
}

static s8 dis_buf_cond_reg(dis_ctl*hdl)
{
	u8 i;
	s32 status;
	for(i=0;i<COPY_CHANNEL_MAX;i++)
	{
		if((status=player_cond_create(&buf_ctl[i].cond_get_buf)))
		{
			OSA_ERROR("player_cond_create() fail:%d!!!",status);
			return -1;
		}	
		buf_ctl[i].get_buf_already=0;
	}
	return 0;
}
static void dis_buf_cont_unreg(dis_ctl*hdl)
{
	u8 i;
	s32 status;
	for(i=0;i<COPY_CHANNEL_MAX;i++)	
	{
		if((status=player_cond_delete(&buf_ctl[i].cond_get_buf)))
		{
			OSA_ERROR("player_cond_delete():%d!!!",status);
		}
	}
}
static void dis_thr_buf_process_init(void)
{
	u8 i;
	buf_use_reg=0x00;
	new_buf=0;
	updata=1;	
	for(i=0;i<COPY_CHANNEL_MAX;i++)
	{
		buf_ctl[i].get_buf_already=0;
	}
}
void dis_thr_slip_scn_buf_pro(u8 ch,u8*ysrc,u8*uvsrc,u8 black,u8 put_dis_buf)
{
	if(!dis_thr_get_buf_register(ch))
	{
		dis_thr_get_empty_buf();
	}			
	if((!put_dis_buf)&&copy_thr_copy_image(ch,black?3:2,ysrc,uvsrc,dis_back_buf))
	{
		OSA_ERROR("copy_thr_copy_image failed!");							
	}		
	dis_thr_get_buf_unregister(ch);			
	dis_thr_put_full_buf(dis_back_buf,dis_back_buf+1280*736);
}
void dis_thr_full_scn_buf_pro(u8 ch,u8*ysrc,u8*uvsrc,u8 black)
{	
	u8 buf_id=0;		
	buf_head *pic_head;	
	_thr_msg send_buf;		
	buf_handle *dis_handle_buf=(buf_handle *)&display_handle.cur_handle_buf;	
	if(player_get_empty_buf(dis_handle_buf,&buf_id))
	{
		OSA_ERROR("copy get empty buf fail!!!");			
	}	
	if(black)
	{
		memset(dis_handle_buf->p_dest[buf_id],0,1280*736);
		memset(dis_handle_buf->p_dest[buf_id]+1280*736,0x80,1280*384);
	}
	else
	{	cedarx_cache_op((long)ysrc,(u32)(ysrc+1280*(736+384)),1);
		if(copy_thr_copy_image(0,0,ysrc,uvsrc,dis_handle_buf->p_dest[buf_id]))
		{
			OSA_ERROR("copy back image error");
		}
	}
	pic_head=(buf_head*)(dis_handle_buf->p_dest[buf_id]+DISPLAY_BUF_SIZE-sizeof(buf_head));
	pic_head->single_pic=1;
	pic_head->pic_ch=ch;
	if(player_put_full_buf(dis_handle_buf,buf_id))
	{
		OSA_ERROR("put empty buf fail");
	}
	if(dis_put_msg_to_queue(&send_buf.thr_data,DISPLAY_CMD_NEW_DATA,0))
	{
		OSA_ERROR("send msg fail!!!");								
	}
}
s8 dis_thr_get_buf_register(u8 ch)
{
	s8 ret=-1;
	pthread_mutex_lock(&dis_lock);	
	if(buf_ctl[ch].get_buf_already)
	{//已经放入一帧到buf中了。这一帧要阻塞到下一个buf
		pthread_cond_wait(&buf_ctl[ch].cond_get_buf,&dis_lock);
	}
	if((!buf_use_reg)&&(!new_buf))
	{	//没有任何一个通道使用过，重新申请			
		ret=0;
		new_buf=1;
	}		
	buf_ctl[ch].get_buf_already=1;
	buf_use_reg|=(1<<ch);//登记
	pthread_mutex_unlock(&dis_lock);	
	return ret;
}
void dis_thr_get_buf_unregister(u8 ch)
{
	pthread_mutex_lock(&dis_lock);	
	buf_use_reg&=(~(1<<ch));//注销登记
	pthread_mutex_unlock(&dis_lock);			
}
void dis_thr_get_empty_buf(void)
{	
	buf_handle *dis_handle_buf=(buf_handle *)&display_handle.cur_handle_buf;	
	if(player_get_empty_buf(dis_handle_buf,(u8*)&dis_buf_id))
	{
		OSA_ERROR("copy get empty buf fail!!!");			
	}			
}
void dis_thr_put_full_buf(u8*ysrc,u8*uvsrc)
{	
	buf_handle *dis_handle_buf=(buf_handle *)&display_handle.cur_handle_buf;	
	pthread_mutex_lock(&dis_lock);	
	if(updata&&(!buf_use_reg)&&new_buf)
	{			
		u8 i;
		_thr_msg send_buf;				
		buf_head *pic_head;
		updata=0;			
		new_buf=0;						
		cedarx_cache_op((long)ysrc,(u32)(ysrc+1280*(736+384)),1);
		if(copy_thr_copy_image(0,0,ysrc,uvsrc,dis_handle_buf->p_dest[dis_buf_id]))
		{
			OSA_ERROR("copy back image error");
		}				
		pic_head=(buf_head*)(dis_handle_buf->p_dest[dis_buf_id]+DISPLAY_BUF_SIZE-sizeof(buf_head));
		pic_head->single_pic=0;
		if(player_put_full_buf(dis_handle_buf,dis_buf_id))
		{
			OSA_ERROR("put empty buf fail");				
		}
		for(i=0;i<COPY_CHANNEL_MAX;i++)
		{
			pthread_cond_signal(&buf_ctl[i].cond_get_buf);
			buf_ctl[i].get_buf_already=0;
		}
		if(dis_put_msg_to_queue(&send_buf.thr_data,DISPLAY_CMD_NEW_DATA,0))
		{
			OSA_ERROR("send msg fail!!!");								
		}
	}	
	pthread_mutex_unlock(&dis_lock);			
}
static void dis_set_temp_fullscreen(dis_ctl *hdl,u8 ch)
{
	__disp_rect_t pic_rect;
	pic_rect.width=640;
	pic_rect.height=360;
	switch(ch)
	{
		case 0:
			pic_rect.x=0;
			pic_rect.y=0;			
			break;
		case 1:
			pic_rect.x=640;
			pic_rect.y=0;			
			break;
		case 2:
			pic_rect.x=0;
			pic_rect.y=360;			
			break;
		case 3:
			pic_rect.x=640;
			pic_rect.y=360;			
			break;
		default:;
	}
	render_set_src_window(&pic_rect);
	render_set_screen_window(&hdl->cur_scr_rect);
}
static void dis_set_normal_display(dis_ctl*hdl)
{
	__disp_rect_t pic_rect;
	pic_rect.width=1280;
	pic_rect.height=720;
	pic_rect.x=0;
	pic_rect.y=0;			
	render_set_src_window(&pic_rect);
	render_set_screen_window(&hdl->cur_scr_rect);
}
static void dis_ele_rsz(dis_ctl *hdl)
{
	static u8 ele_rsz=0;
	__disp_rect_t rect;
	if(hdl->single_pic)
	{//全屏显示
		if(!hdl->pic_mode)
		{//目前正在显示的是四分屏图片
			rect.x=hdl->cur_pic_rect.x*639/1279;
			rect.y=hdl->cur_pic_rect.y*359/719;
			rect.width=hdl->cur_pic_rect.width*640/1280;
			rect.height=hdl->cur_pic_rect.height*360/720;			
			switch(hdl->single_ch)
			{
				case 1:
					rect.x+=640;
					break;
				case 2:
					rect.y+=360;
					break;
				case 3:
					rect.x+=640;
					rect.y+=360;
					break;
				default:;
			}
			render_set_src_window(&rect);
		}
		else
		{//目前正在显示的图片是单通道全屏图片
			render_set_src_window(&hdl->cur_pic_rect);
		}
	}
}
static void dis_parameter_init(dis_ctl *hdl)
{
	hdl->picture_test.is_progressive=1;
	hdl->picture_test.top_field_first=1;
	hdl->picture_test.frame_rate=25000;//驱动没有使用这个参数
	hdl->picture_test.pixel_format=CEDARV_PIXEL_FORMAT_AW_YUV420;//修改??????
	hdl->picture_test.display_width=1280;
	hdl->picture_test.display_height=720;
	hdl->picture_test.width=1280;
	hdl->picture_test.height=720;		
	hdl->dis_thr_sta=DIS_THR_STA_STANDBY;
	hdl->loop_index=0;
	hdl->single_pic=0;
	hdl->dis_mode_720=0;
	hdl->pic_mode=0;
	hdl->single_ch=COPY_CHANNEL_MAX;
	hdl->standby_sta=0x0f;
	hdl->cur_pic_rect.x=hdl->cur_pic_rect.y=0;
	hdl->cur_pic_rect.width=1280;
	hdl->cur_pic_rect.height=720;
	hdl->cur_scr_rect.x=hdl->cur_scr_rect.y=0;
	hdl->cur_scr_rect.width=1280;
	hdl->cur_scr_rect.height=720;
}
static u8 dis_clr_data(void)
{
	u8 cmd=0;
	cmd=(u8)g_atomic_int_get(&dis_cmd);
	return cmd;
}
void dis_set_clr_cmd(u8 ch,u8 cmd)
{	
	if(cmd)
	{
		g_atomic_int_or(&dis_cmd,(gint)1<<ch);	
	}
	else
	{
		g_atomic_int_and(&dis_cmd,(gint)(~(1<<ch)));
	}
}
s8 dis_put_msg_to_queue(_thr_msg_data *data,u8 cmd,int flag)
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
	send_buf.msg_type=DIS_MSG_TYPE_RCV;
	memcpy(&send_buf.thr_data,data,sizeof(_thr_msg_data));
	send_buf.thr_data.cmd=cmd;
	if(msgsnd(msg_id,&send_buf,sizeof(_thr_msg_data),flag))
	{
		OSA_ERROR("dis send cmd:%d fail!!!",cmd);								
		pthread_mutex_unlock(&msg_lock);	
		return -1;
	}
	pthread_mutex_unlock(&msg_lock);	
	return 0;
}

