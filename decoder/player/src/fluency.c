#include<player.h>
#include<osa_thr.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdlib.h>
//#include<dec_inc/libcedarv.h>
#include <stdio.h> // void perror(const char *msg);
#include <string.h> // char *strerror(int errnum);
#include <errno.h> //errno
#include <sys/time.h>
#include <signal.h> 
#include<time.h>
#include <render.h>
#include<sys/prctl.h>

enum{
	FLU_THR_STA_WORK_ON=0,
	FLU_THR_STA_STANDBY,
};

typedef struct
{
	u32 wait_time;
	u32 timestamp;
	u64 cur_pts;
	u64 pre_pts;
	u32 average_timestamp;
	u32 dif_tm;
	u32 average_tmstamp_back;
	u8 flu_thr_sta;
	u8 flu_id;
	u8 set_rate;
	u8 record_en;
//	StorageReadCmd read_cmd;
	u32 play_id;
////////消息队列
	int msg_id;
	int msg_flg;
	_thr_msg flu_msgbuf;
////////////
}flu_ctl;
#ifdef PLAYER_TEST
static flu_ctl* flu_test[FLUENCY_CHANNEL_MAX];
#endif
volatile static volatile gint flu_play_id[FLUENCY_CHANNEL_MAX]={0,0,0,0};
volatile static volatile gint flu_cmd[FLUENCY_CHANNEL_MAX]={0,0,0,0};
volatile static volatile gint flu_rate[FLUENCY_CHANNEL_MAX]={FLU_WORK_NOMAL_RATE,FLU_WORK_F2_RATE,FLU_WORK_F2_RATE,FLU_WORK_F2_RATE};
const u8 flu_msg_type[FLUENCY_CHANNEL_MAX]={FLU1_MSG_TYPE_RCV,FLU2_MSG_TYPE_RCV,FLU3_MSG_TYPE_RCV,FLU4_MSG_TYPE_RCV};
static OSA_ThrHndl fluency_thr[FLUENCY_CHANNEL_MAX];
static pthread_mutex_t msg_lock;
static void *fluency_thr_main(void*param);
static void fluency_parameter_init(flu_ctl *hdl);
static void fluency_sleep_ctl(flu_ctl*hdl);
static void fluency_thr_ff_ctl(flu_ctl*hdl);
static u8 fluency_clr_data(u8 ch);
static u8 fluency_get_rate(u8 ch);

int fluency_thr_create(u8 *param)
{
	int status=0;	
	static u8 flu_init=0;
	if(!flu_init)
	{
		flu_init=1;
		status|=player_mutex_create(&msg_lock);
		if(status)
		{
			OSA_ERROR("player_mutex_create():%d!!!",status);
			return status;
		}
	}
	//status = OSA_thrCreate(&fluency_thr[*param], fluency_thr_main, FLU_THR_PRI, 0, param);	
	status=pthread_create(&fluency_thr[*param].hndl,NULL,fluency_thr_main,(void*)param);
	if(status!=OSA_SOK) 
	{
		OSA_ERROR("OSA_tskCreate()");
		return status;
	}
	return status;
}
static void *fluency_thr_main(void*param)
{
	u8 exit=0;
	u32 pre_dis_tm=0,cur_dis_tm=0;
	key_t keyval;	
	char pr_name[10];
	_thr_msg_data *flu_data;
	flu_ctl flu_hdl;
	flu_hdl.flu_id=*((u8*)param);	
	if(-1==(keyval=ftok(".",PLAYER_THR_IPC_KEY_ID)))//获取一个消息队列的键值
	{
		OSA_ERROR("get keyval fail");
	}
	if(-1==(flu_hdl.msg_id=msgget(keyval,IPC_CREAT|0666)))//创建一个消息队列,并且设置读写权限。如果消息队列已经存在则打开
	{
		OSA_ERROR("get msgid fail");
	}	
	sprintf(pr_name,"flu%d",flu_hdl.flu_id);	
	prctl(PR_SET_NAME, pr_name);
	DEBUG_PF("flu%d thr run",flu_hdl.flu_id);
	fluency_parameter_init(&flu_hdl);
	flu_data=&flu_hdl.flu_msgbuf.thr_data;
#ifdef PLAYER_TEST
	flu_test[flu_hdl.flu_id]=&flu_hdl;
#endif		
	while(!exit)
	{		
		if(-1==(msgrcv(flu_hdl.msg_id,&flu_hdl.flu_msgbuf,sizeof(_thr_msg_data),flu_msg_type[flu_hdl.flu_id],0)))
		{
			OSA_ERROR("flu rcv msg fail ,str error: %s",strerror(errno));
		}	
		else
		{
///////////////////////////****************************///////////////////////////////////////
			switch(flu_data->cmd)
			{
				case FLU_CMD_NEW_DATA:
					if(FLU_THR_STA_WORK_ON==flu_hdl.flu_thr_sta)
					{				
						if(fluency_clr_data(flu_hdl.flu_id))
						{
							if(copy_put_msg_to_queue(flu_hdl.flu_id,flu_data,COPY_CMD_NEW_DATA,0))
							{
								OSA_ERROR("flu send msg fail!!!");
							}
						}
						else
						{
							u8 rate;								
							flu_hdl.average_timestamp=flu_data->head_infor.average_timestamp;
							flu_hdl.timestamp=flu_data->head_infor.time_stamp;
							flu_hdl.pre_pts = flu_hdl.cur_pts;
							flu_hdl.cur_pts=flu_data->pts;
							cur_dis_tm=OSA_getRunTimeInMsec();
							flu_hdl.wait_time=cur_dis_tm-pre_dis_tm;							
							rate=fluency_get_rate(flu_hdl.flu_id);
							if(rate!=flu_hdl.set_rate)
							{
								flu_hdl.set_rate=rate;
								if((flu_hdl.set_rate>=FLU_WORK_F2_RATE)&&(flu_hdl.set_rate<=FLU_WORK_F8_IFRAME_RATE))
								{
									fluency_thr_ff_ctl(&flu_hdl);
								}
							}						
							fluency_sleep_ctl(&flu_hdl);
							pre_dis_tm=OSA_getRunTimeInMsec();	
							if(flu_hdl.set_rate != FLU_WORK_F8_RATE)
							{
								if(copy_put_msg_to_queue(flu_hdl.flu_id,flu_data,COPY_CMD_NEW_DATA,0))
								{
									OSA_ERROR("flu send msg fail!!!");
								}
							}
							else
							{
								// FLU_WORK_F8_RATE 快播时丢弃一半的帧
								static int flag = 0;
								int send_cmd = flag ? COPY_CMD_IGNORE_DATA : COPY_CMD_NEW_DATA;
								if(copy_put_msg_to_queue(flu_hdl.flu_id,flu_data,send_cmd,0))
								{
									OSA_ERROR("flu send msg fail!!!");
								}
								flag = !flag;							
							}
						}
					}
					break;
				case FLU_CMD_BLACK:
					if(FLU_THR_STA_WORK_ON==flu_hdl.flu_thr_sta)
					{
						DEBUG_PF("flu id:%d rcv black cmd",flu_hdl.flu_id);	
						if(copy_put_msg_to_queue(flu_hdl.flu_id,flu_data,COPY_CMD_BLACK,0))
						{
							OSA_ERROR("flu send msg fail!!!");
						}						
					}
					break;
				case FLU_CMD_STANDBY:
					if((FLU_THR_STA_WORK_ON==flu_hdl.flu_thr_sta)
						||(FLU_THR_STA_STANDBY==flu_hdl.flu_thr_sta))
					{
						DEBUG_PF("flu id:%d rcv standby cmd at sta:%d",flu_hdl.flu_id,flu_hdl.flu_thr_sta);	
						flu_hdl.flu_thr_sta=FLU_THR_STA_STANDBY;	
						fluency_set_clr_cmd(flu_hdl.flu_id,0);
						if(copy_put_msg_to_queue(flu_hdl.flu_id,flu_data,COPY_CMD_STANDBY,0))
						{
							OSA_ERROR("flu send msg fail!!!");
						}							
					}
					break;	
				case FLU_CMD_FRAME_SYNC:
					if((FLU_THR_STA_WORK_ON==flu_hdl.flu_thr_sta)
						||(FLU_THR_STA_STANDBY==flu_hdl.flu_thr_sta))
					{
						DEBUG_PF("flu %d rcv frame sync at sta:%d",flu_hdl.flu_id,flu_hdl.flu_thr_sta);					
						if(copy_put_msg_to_queue(flu_hdl.flu_id,flu_data,COPY_CMD_FRAME_SYNC,0))
						{
							OSA_ERROR("flu send msg fail!!!");
						}	
					}
					break;		
				case COPY_CMD_STANDBY_ALREADY:
					if(FLU_THR_STA_STANDBY==flu_hdl.flu_thr_sta)
					{					
						DEBUG_PF("flu %d rcv copy standby already cmd",flu_hdl.flu_id);
						if(decode_put_msg_to_queue(flu_hdl.flu_id,flu_data,FLU_CMD_STANDBY_ALREADY,0))
						{
							OSA_ERROR("flu send msg fail!!!");
						}							
					}
					break;		
				case COPY_CMD_WORK_ON_ALREADY:
					if(FLU_THR_STA_STANDBY==flu_hdl.flu_thr_sta)
					{					
						DEBUG_PF("flu %d rcv copy work on alreadby cmd",flu_hdl.flu_id);								
						flu_hdl.record_en=flu_data->record;
						if(flu_hdl.record_en==GET_STREAM_FROME_NET)
						{
							fluency_set_rate(flu_hdl.flu_id,FLU_WORK_NOMAL_RATE);
						}
						flu_hdl.flu_thr_sta=FLU_THR_STA_WORK_ON;
						if(decode_put_msg_to_queue(flu_hdl.flu_id,flu_data,FLU_CMD_WORK_ON_ALREADY,0))
						{
							OSA_ERROR("flu send msg fail!!!");
						}
					}
					break;		
				case FLU_CMD_WORK_ON:
					if(FLU_THR_STA_STANDBY==flu_hdl.flu_thr_sta)
					{					
						DEBUG_PF("flu id:%d rcv work on cmd",flu_hdl.flu_id);
						flu_data->channel=flu_hdl.flu_id;
						if(copy_put_msg_to_queue(flu_hdl.flu_id,flu_data,COPY_CMD_WORK_ON,0))
						{
							OSA_ERROR("flu send msg fail!!!");
						}
					}
					break;		
				case FLU_CMD_WAIT_CMD:
					DEBUG_PF("flu:%d rcv wait cmd at sta:%d",flu_hdl.flu_id,flu_hdl.flu_thr_sta);
					fluency_set_clr_cmd(flu_hdl.flu_id,0);					
					if(copy_put_msg_to_queue(flu_hdl.flu_id,flu_data,COPY_CMD_WAIT_CMD,0))
					{
						OSA_ERROR("send msg fail!!!");
					}
					break;
				case COPY_CMD_WAIT_CMD_ALREADY:
					DEBUG_PF("flu:%d rcv copy wait cmd already at sta:%d",flu_hdl.flu_id,flu_hdl.flu_thr_sta);
					if(decode_put_msg_to_queue(flu_hdl.flu_id,flu_data,FLU_CMD_WAIT_CMD_ALREADY,0))
					{
						OSA_ERROR("send msg fail!!!");
					}					
					break;
				case FLU_CMD_EXIT:
					if(FLU_THR_STA_STANDBY==flu_hdl.flu_thr_sta)
					{					
						DEBUG_PF("flu :%d rcv exit cmd",flu_hdl.flu_id);
						exit=1;
					}
					break;
				default:;
			}
///////////////////////////****************************///////////////////////////////////////	
		}
	}		
	DEBUG_PF("flu thr exit");	
	return NULL;
}
int fluency_thr_delete(u8 param)
{
	static u8 flu_exit=0;
	int status=0;
	if(!flu_exit)
	{
		flu_exit=1;
		status|=player_mutex_delete(&msg_lock);		
		if(status)
		{
			OSA_ERROR("player_mutex_delete:%d!!!",status);
		}
	}
	OSA_thrDelete(&fluency_thr[param]);
	return OSA_SOK;
}
static void fluency_thr_ff_ctl(flu_ctl*hdl)
{	
	u32 tm_step;		
	if(hdl->average_timestamp>DIS_TIME_LIMIT)
	{
		if(hdl->set_rate>=FLU_WORK_F2_RATE && hdl->set_rate<=FLU_WORK_F8_RATE)
		{
			hdl->average_tmstamp_back=hdl->average_timestamp;
			tm_step=hdl->average_timestamp/(FLU_WORK_F8_RATE-FLU_WORK_F2_RATE+1);
			hdl->dif_tm=tm_step*(hdl->set_rate-FLU_WORK_F2_RATE+1);
		}
		else if(hdl->set_rate>=FLU_WORK_F2_IFRAME_RATE && hdl->set_rate<=FLU_WORK_F8_IFRAME_RATE)
		{
			hdl->average_tmstamp_back=hdl->average_timestamp;
			tm_step=hdl->average_timestamp/(FLU_WORK_F8_IFRAME_RATE-FLU_WORK_F2_IFRAME_RATE+1);
			hdl->dif_tm=tm_step*(hdl->set_rate-FLU_WORK_F2_IFRAME_RATE+1);
		}
	}
}
static void fluency_sleep_ctl(flu_ctl*hdl)
{
	if((hdl->set_rate>=FLU_WORK_S2_RATE)&&(hdl->set_rate<=FLU_WORK_S8_RATE))
	{	//慢放	
		hdl->timestamp<<=hdl->set_rate-FLU_WORK_S2_RATE+1;
	}
	if((hdl->set_rate>=FLU_WORK_F2_RATE)&&(hdl->set_rate<=FLU_WORK_F8_RATE))
	{	//快进
		if(hdl->average_tmstamp_back!=hdl->average_timestamp)
		{//帧率改变
			fluency_thr_ff_ctl(hdl);
		}
		if(hdl->timestamp>hdl->dif_tm)
		{
			hdl->timestamp-=hdl->dif_tm;
		}
		else
		{
			hdl->timestamp = 0;
		}
	}
	if((hdl->set_rate>=FLU_WORK_F2_IFRAME_RATE)&&(hdl->set_rate<=FLU_WORK_F8_IFRAME_RATE))
	{	//快进
		if(hdl->average_tmstamp_back!=hdl->average_timestamp)
		{//帧率改变
			fluency_thr_ff_ctl(hdl);
		}
		if(hdl->timestamp>hdl->dif_tm)
		{
			hdl->timestamp-=hdl->dif_tm;
		}
		else
		{
			hdl->timestamp = 0;
		}
		if(hdl->cur_pts > hdl->pre_pts && hdl->cur_pts - hdl->pre_pts < 10000000)
		{
			u32 interval_ms = (hdl->cur_pts - hdl->pre_pts) / 1000;
			u32 sleep_ms = interval_ms / (hdl->set_rate - FLU_WORK_F2_IFRAME_RATE + 2);
			player_waitMsecs(sleep_ms);
		}
		//player_waitMsecs((2 - (hdl->set_rate - FLU_WORK_F2_IFRAME_RATE) + 2) * 100);
		return;
	}
	if((hdl->timestamp>3)&&(hdl->record_en==GET_STREAM_FROME_NET))
	{
		hdl->timestamp-=3;//防止临界情况导致视频越来越慢
	}	
	if((hdl->timestamp>1)&&(hdl->record_en==GET_STREAM_FROME_DISK))
	{
		hdl->timestamp-=1;//防止临界情况导致视频越来越慢
	}
	if(hdl->timestamp>hdl->wait_time)
	{			
		if((hdl->timestamp-hdl->wait_time)>2000)
		{
			OSA_ERROR("flu:%d sleep time is:%d,time:%d,wait_time:%d",hdl->flu_id,hdl->timestamp-hdl->wait_time,hdl->timestamp,hdl->wait_time);
			hdl->timestamp=hdl->wait_time+2000;
		}	
		if(hdl->set_rate != FLU_WORK_F8_RATE)
		{
			player_waitMsecs(hdl->timestamp-hdl->wait_time);
		}
	}
}
static void fluency_parameter_init(flu_ctl *hdl)
{
	hdl->set_rate=FLU_WORK_NOMAL_RATE;
	hdl->flu_thr_sta=FLU_THR_STA_STANDBY;
	hdl->wait_time=0;
	hdl->average_timestamp=40;
	hdl->average_tmstamp_back=0;
	hdl->dif_tm=0;
//	hdl->read_cmd=READ_NORMAL;
	hdl->record_en=GET_STREAM_FROME_NET;
}
void fluency_set_clr_cmd(u8 ch,u8 flag)
{	
	g_atomic_int_set(&flu_cmd[ch],(gint)flag);
}
void fluency_set_rate(u8 ch,u8 rate)
{
	g_atomic_int_set(&flu_rate[ch],(gint)rate);
}
static u8 fluency_clr_data(u8 ch)
{
	u8 cmd=0;
	cmd=(u8)g_atomic_int_get(&flu_cmd[ch]);
	return cmd;
}
static u8 fluency_get_rate(u8 ch)
{
	return (u8)g_atomic_int_get(&flu_rate[ch]);
}
void fluency_set_play_id(u8 ch, u32 play_id)
{
	g_atomic_int_set(&flu_play_id[ch], (gint)play_id);
}
u32 fluency_get_play_id(u8 ch)
{
	return (s32)g_atomic_int_get(&flu_play_id[ch]);
}
s8 fluency_put_msg_to_queue(u8 ch,_thr_msg_data *data,u8 cmd,int flag)
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
	send_buf.msg_type=flu_msg_type[ch];
	memcpy(&send_buf.thr_data,data,sizeof(_thr_msg_data));
	send_buf.thr_data.cmd=cmd;
	if(msgsnd(msg_id,&send_buf,sizeof(_thr_msg_data),flag))
	{
		OSA_ERROR("flu%d send cmd:%d fail!!!",ch,cmd);								
		pthread_mutex_unlock(&msg_lock);	
		return -1;
	}
	pthread_mutex_unlock(&msg_lock);	
	return 0;
}

