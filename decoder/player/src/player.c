#include<player.h> 

#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<osa_thr.h>
#include<sys/time.h>
#include<signal.h> 
#include<syslog.h>
#include<render.h>

enum{
	PLAYER_THR_STA_IDLE=0,
	PLAYER_THR_STA_PLAY,
	PLAYER_THR_STA_STOP,
	PLAYER_THR_STA_UPDATE_CH_NUM,
	PLAYER_THR_STA_SINGLE_DIS,
	PLAYER_THR_STA_RECORD_PLAYE,
	PLAYER_THR_STA_CHOOSE_FILE,
	PLAYER_THR_STA_PAUSE,
	PLAYER_THR_STA_SINGLE_FRAME,
	PLAYER_THR_STA_SET_RATE,
	PLAYER_THR_STA_GET_PLAYER_STATUS,
	PLAYER_THR_STA_EXIT,
};
typedef struct{
	u8 player_thr_sta;
	u8 str_standby_already;
	u8 str_pause_already;
	u8 record_en;
	u8 single_en;
	u8 wait_return;
	u8 exit;
	long rcv_type;	
	player_status state[STREAM_CHANNEL_MAX];
	__disp_rect_t rect;
	/////
	int msg_flag;
	int msg_id;
	_thr_msg thr_msg;	
	_pro_msg pro_msg;
}player_ctl;
#ifdef PLAYER_TEST
static player_ctl*player_test;
#endif
static pthread_mutex_t msg_lock;
static s32 player_buf_alloc(void);
static void player_buf_free(void);
static s32 player_que_get(buf_queue*queue, u8 *buf_id, Uint32 timeout);
static void player_dec_buf_clr(u8 ch);
static void player_all_thr_exit(int id);
static s32 player_queue_create(buf_queue *queue,u8 len,u8 *buf_dest);
static s32 player_queue_delete(buf_queue *queue);
static s8 player_send_msg_to_ui(_pro_msg_data *data,int flag);
static void player_signal_quit(int signo);
static u8 player_que_get_num(buf_queue*queue);
static s8 player_pro_dis_mod_cmd(player_ctl*hdl);
static void player_pro_stop_already_cmd(player_ctl*hdl);
static s8 player_pro_stop_cmd(player_ctl*hdl);
static s8 player_set_ch_stop(player_ctl*hdl,u8 ch,u8 black,u8 *send);
static void player_pro_exit_already_cmd(player_ctl*hdl);
static s8 player_pro_exit_cmd(player_ctl*hdl);
static void player_pro_play_already_cmd(player_ctl*hdl);
static s8 player_pro_play_cmd(player_ctl*hdl);
static s8 player_set_ch_play(player_ctl*hdl,u8 ch,u8 *send);
static s8 player_pro_get_sta_already_cmd(player_ctl*hdl);
static s8 player_pro_get_sta_cmd(player_ctl*hdl);
static s8 player_pro_play_mode_cmd(player_ctl*hdl);
static void player_pro_set_rate_already_cmd(player_ctl *hdl);
static s8 player_pro_set_rate_cmd(player_ctl *hdl);
static s8 player_pro_choose_file_already_cmd(player_ctl *hdl);
static s8 player_pro_choose_file_cmd(player_ctl *hdl);
static s8 player_set_ch_pause(player_ctl *hdl,u8 ch,u8 pause,u8 *send);
static void player_pro_pause_already_cmd(player_ctl *hdl);
static s8 player_pro_pause_cmd(player_ctl *hdl);
static s8 player_pro_single_frame_play(player_ctl *hdl);
static void player_pro_set_rect_cmd(player_ctl*hdl);
static s8 player_pro_get_dis_mode_cmd(player_ctl *hdl);
static s8 player_pro_get_preview_sta_cmd(player_ctl*hdl);
static s8 player_pro_set_audio_cmd(player_ctl*hdl);
static s8 player_pro_set_elec_rsz_cmd(player_ctl*hdl);
static void player_pro_alarm_audio_cmd(player_ctl*hdl);
static void player_set_sta(player_ctl *hdl,u8 sta);
static void player_parameter_init(player_ctl *hdl);
static void player_pro_set_cap_num_cmd(player_ctl *hdl);
static void player_pro_start_phone_audio_cmd(void);
static void player_pro_start_audio_record_cmd(player_ctl*hdl);
static void player_pro_stop_audio_record_cmd(player_ctl*hdl);
static void player_pro_stop_phone_audio_cmd(void);
static void player_pro_set_audio_rec_par_cmd(player_ctl*hdl);
static s8 player_pro_set_src_rect(player_ctl*hdl);
static s8 player_pro_set_scn_rect(player_ctl*hdl);
////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{ 		
	player_ctl player_hdl;
	int status=0,pro_msg_id;	
	u8 i,thr_id[STREAM_CHANNEL_MAX]={0,1,2,3};
	key_t keyval;
	keyval=ftok(".",PLAYER_THR_IPC_KEY_ID);//获取一个消息队列的键值
	player_hdl.msg_id=msgget(keyval,IPC_CREAT|0666);//创建一个消息队列,并且设置读写权限。如果消息队列已经存在则打开	

	keyval=ftok(".",PLAYER_PRO_IPC_KEY_ID);//获取一个消息队列的键值
	pro_msg_id=msgget(keyval,IPC_CREAT|0666);//创建一个消息队列,并且设置读写权限。如果消息队列已经存在则打开	
	openlog("player_log", LOG_PID|LOG_CONS, LOG_USER);
	DEBUG_PF("msg queue creat success id:%d",(int)player_hdl.msg_id);
#ifdef PLAYER_TEST
	player_test=&player_hdl;
#endif
////////////
	player_parameter_init(&player_hdl);
	cedarx_hardware_init(0);		
	render_init();
	status=player_buf_alloc();//内存分配	
	if(status)
	{		
		render_exit();
		OSA_ERROR("player_buf_alloc():%d!!!",status);
		return status;
	}	
	status=player_mutex_create(&msg_lock);//创建一个互斥锁
	if(status)
	{
		render_exit();	
		OSA_ERROR("player_mutex_create():%d!!!",status);
		return status;
	}	
//////////
	for(i=0;i<DECODER_MAX;i++)
	{
		status |= decode_thr_create(&thr_id[i]);	
		status |= fluency_thr_create(&thr_id[i]);	
		status |= copy_thr_create(&thr_id[i]);	
		status |= stream_thr_create(&thr_id[i]);	
	}
	status |= display_thr_create(&thr_id[0]);	
	status |= audio_thr_create(&thr_id[0]);		
	status |= audio_record_thr_create(&thr_id[0]);			
	if(status!=OSA_SOK)
	{
		render_exit();	
		OSA_ERROR("thr create fail!!!");
		return status;
	}
	signal(SIGTERM, player_signal_quit);
	signal(SIGINT, player_signal_quit); 
	signal(SIGPIPE, SIG_IGN);
 	while(!player_hdl.exit)
	{
		if(-1==(msgrcv(player_hdl.rcv_type==PLAYER_MSG_TYPE_RCV_PRO?pro_msg_id:player_hdl.msg_id,
			(player_hdl.rcv_type==PLAYER_MSG_TYPE_RCV_PRO)?((void*)&player_hdl.pro_msg):((void*)&player_hdl.thr_msg),
			(player_hdl.rcv_type==PLAYER_MSG_TYPE_RCV_PRO)?sizeof(_pro_msg_data):sizeof(_thr_msg_data),
			player_hdl.rcv_type,0)))
		{
			if(EINTR!=errno)
			{
				OSA_ERROR("player rcv msg fail ,str error: %s",strerror(errno)); //转换错误码为对应的错误信息
			}
			//OSA_ERROR("strerror: %s", strerror(errno)); //转换错误码为对应的错误信息
			//OSA_ERROR("player rcv msg fail!!!");
			//return OSA_EFAIL;
		}	
		else
		{
			u8 cmd=player_hdl.rcv_type==PLAYER_MSG_TYPE_RCV_PRO?player_hdl.pro_msg.pro_data.cmd:player_hdl.thr_msg.thr_data.cmd;
			if(!cmd)
			{
				DEBUG_PF("player thr sta reset");
				player_set_sta(&player_hdl,PLAYER_THR_STA_IDLE);
			}
			else
			{
				switch(player_hdl.player_thr_sta)
				{
					case PLAYER_THR_STA_IDLE:
						switch(cmd)
						{
							case PLAYER_CMD_GET_PLAYER_STATUS:
								if(player_pro_get_sta_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_get_sta_cmd fail");
								}
								break;	
							case PLAYER_CMD_PLAY:									
								if(player_pro_play_cmd(&player_hdl))
								{
									OSA_ERROR("player_set_ch_play fail");
								}
								break;
							case PLAYER_CMD_STOP:								
								if(player_pro_stop_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_stop_cmd fail");
								}
								break;	
							case PLAYER_CMD_SINGAL_DISPLAY:								
								if(player_pro_dis_mod_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_dis_mod_cmd fail");
								}
								break;	
							case PLAYER_CMD_ALL_EXIT:								
								if(player_pro_exit_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_exit_cmd fail");
								}
								break;
							case PLAYER_CMD_SET_RATE:								
								if(player_pro_set_rate_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_exit_cmd fail");
								}
								break;
							case PLAYER_CMD_REAL_PLAY:
							case PLAYER_CMD_RECORD_PLAY:		
								if(player_pro_play_mode_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_play_mode_cmd fail");
								}
								break;
							case PLAYER_CMD_CHOOSE_FILE:
								if(player_pro_choose_file_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_choose_file_cmd fail");
								}
								break;
							case PLAYER_CMD_UP_FRAME:							
							case PLAYER_CMD_DOWN_FRAME:		
								if(player_pro_single_frame_play(&player_hdl))
								{
									OSA_ERROR("player_pro_single_frame_play fail");
								}
								break;
							case PLAYER_CMD_PAUSE:
								if(player_pro_pause_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_pause_cmd fail");
								}
								break;
							case PLAYER_CMD_GET_DISPLAY_MODE:
								if(player_pro_get_dis_mode_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_get_dis_mode_cmd fail");
								}
								break;
							case PLAYER_CMD_SET_RECORD_RECT:
								player_pro_set_rect_cmd(&player_hdl);
								break;
							case PLAYER_CMD_GET_PREVIEW_STATUS:
								if(player_pro_get_preview_sta_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_get_preview_sta fail");
								}							
								break;
							case PLAYER_CMD_SET_AUDIO:
								if(player_pro_set_audio_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_set_audio_cmd fail");
								}
								break;
							case PLAYER_CMD_SET_ELEC_RSZ:
								if(player_pro_set_elec_rsz_cmd(&player_hdl))
								{
									OSA_ERROR("player_pro_set_elec_rsz_cmd fail");
								}
								break;
							case PLAYER_CMD_ALARM_AUDIO:
								player_pro_alarm_audio_cmd(&player_hdl);
								break;
							case PLAYER_CMD_CAPTURE:
								player_pro_set_cap_num_cmd(&player_hdl)	;					
								break;
							case PLAYER_CMD_SET_FB_ALPHA:		
								DEBUG_PF("player rcv set alpha %d cmd",player_hdl.pro_msg.pro_data.alpha);								
								render_set_fb_alpha(player_hdl.pro_msg.pro_data.alpha);
								break;
							case PLAYER_CMD_SET_COLORKEY:
								DEBUG_PF("player rcv set colorkey r:%d g:%d b%d cmd",player_hdl.pro_msg.pro_data.r,player_hdl.pro_msg.pro_data.g,player_hdl.pro_msg.pro_data.b);								
								render_set_colorkey(player_hdl.pro_msg.pro_data.r,player_hdl.pro_msg.pro_data.g,player_hdl.pro_msg.pro_data.b);								
								break;
							case PLAYER_CMD_START_PHONE_AUDIO:
								player_pro_start_phone_audio_cmd();
								break;
							case PLAYER_CMD_STOP_PHONE_AUDIO:
								player_pro_stop_phone_audio_cmd();
								break;
							case PLAYER_CMD_START_AUDIO_RECORD:
								player_pro_start_audio_record_cmd(&player_hdl);
								break;
							case PLAYER_CMD_STOP_AUDIO_RECORD:
								player_pro_stop_audio_record_cmd(&player_hdl);
								break;
							case PLAYER_CMD_SET_AUDIO_REC_PAR:
								player_pro_set_audio_rec_par_cmd(&player_hdl);
								break;
							case PLAYER_CMD_SET_SRC_RECT:
								player_pro_set_src_rect(&player_hdl);
								break;
							case PLAYER_CMD_SET_SCN_RECT:
								player_pro_set_scn_rect(&player_hdl);
								break;
							default:;
							}
						break;
					case PLAYER_THR_STA_CHOOSE_FILE:
						player_pro_choose_file_already_cmd(&player_hdl);
						break;
					case PLAYER_THR_STA_PLAY:						
						if(STREAM_CMD_WORK_ALREADY==cmd)
						{
							player_pro_play_already_cmd(&player_hdl);
						}
						break;
					case PLAYER_THR_STA_STOP:
						if(STREAM_CMD_STANDBY_ALREADY==cmd)
						{
							player_pro_stop_already_cmd(&player_hdl);
						}
						break;
					case PLAYER_THR_STA_PAUSE:
						if(STREAM_CMD_PAUSE_ALREADY==cmd)
						{
							player_pro_pause_already_cmd(&player_hdl);
						}
						break;
					case PLAYER_THR_STA_SET_RATE:
						if(STREAM_CMD_SET_RATE_ALREADY==cmd)
						{
							player_pro_set_rate_already_cmd(&player_hdl);
						}
						break;
					case PLAYER_THR_STA_GET_PLAYER_STATUS:
						if(STREAM_CMD_QUERY_PLAY_TIME_ALREADY==cmd)
						{
							if(player_pro_get_sta_already_cmd(&player_hdl))
							{
								OSA_ERROR("player_pro_get_sta_already_cmd fail");
							}
						}
						break;
					case PLAYER_THR_STA_RECORD_PLAYE:
						if(STREAM_CMD_STANDBY_ALREADY==cmd)
						{						
							player_pro_stop_already_cmd(&player_hdl);
						}
						break;
					case PLAYER_THR_STA_SINGLE_DIS:
						if(STREAM_CMD_STANDBY_ALREADY==cmd)
						{
							player_pro_stop_already_cmd(&player_hdl);
						}
						break;
					case PLAYER_THR_STA_EXIT:					
						if(STREAM_CMD_STANDBY_ALREADY==cmd)
						{
							player_pro_stop_already_cmd(&player_hdl);
							player_pro_exit_already_cmd(&player_hdl);
						}
						break;
					default:;
				}
			}
		}
	}	
	OSA_waitMsecs(500);
	for(i=0;i<STREAM_CHANNEL_MAX;i++)
	{
		stream_thr_delete(i);
		decode_thr_delete(i);
		fluency_thr_delete(i);		
		copy_thr_delete(i);	
	}
	display_thr_delete();
	audio_thr_delete();
	audio_record_thr_delete();
	player_buf_free();
	if((status=player_mutex_delete(&msg_lock)))
	{
		OSA_ERROR("player_mutex_delete:%d!!!",status);
	}
	if(msgctl(player_hdl.msg_id,IPC_RMID,NULL))
	{
		OSA_ERROR("del msg queue fail!!!");
		return OSA_EFAIL;
	}	
	if(msgctl(pro_msg_id,IPC_RMID,NULL))
	{
		OSA_ERROR("del msg queue fail!!!");
		return OSA_EFAIL;
	}	
	render_exit();
	cedarx_hardware_exit(0);	
	DEBUG_PF("service progress exit success");
	closelog();
  return OSA_SOK; 
}
static void player_all_thr_exit(int id)
{
	u8 j;
	_thr_msg buf;
	DEBUG_PF("player send all thr exit cmd"); 		
	audio_thr_exit();
	audio_record_thr_exit();
	for(j=0;j<4;j++)
	{
		if(stream_put_msg_to_queue(j,&buf.thr_data,STREAM_CMD_EXIT,0))
		{
			OSA_ERROR("player send ch%d cmd:%d fail!!!",j,STREAM_CMD_EXIT);
		}
		if(decode_put_msg_to_queue(j,&buf.thr_data,DECODE_CMD_EXIT,0))
		{
			OSA_ERROR("player send ch%d cmd:%d fail!!!",j,DECODE_CMD_EXIT);
		}
		if(fluency_put_msg_to_queue(j,&buf.thr_data,FLU_CMD_EXIT,0))
		{
			OSA_ERROR("player send ch%d cmd:%d fail!!!",j,FLU_CMD_EXIT);
		}
		if(copy_put_msg_to_queue(j,&buf.thr_data,COPY_CMD_EXIT,0))
		{
			OSA_ERROR("player send ch%d cmd:%d fail!!!",j,COPY_CMD_EXIT);
		}		
		usleep(100);
	}
	if(dis_put_msg_to_queue(&buf.thr_data,DISPLAY_CMD_EXIT,0))
	{
		OSA_ERROR("send msg fail!!!");
	}		
}
u8 player_get_empty_buf_num(buf_handle *buf_hdl)
{
	return player_que_get_num((buf_queue*)&(buf_hdl->empty_queue));
}
s32 player_get_empty_buf(buf_handle *buf_hdl,u8 *buf_id)
{
	s32 status=OSA_SOK;
	status=player_que_get((buf_queue*)&(buf_hdl->empty_queue),buf_id,PLAYER_TIMEOUT_FOREVER);	
	return status;
}
s32 player_get_empty_buf_nowait(buf_handle *buf_hdl,u8 *buf_id)
{
	s32 status=OSA_SOK;
	status=player_que_get((buf_queue*)&(buf_hdl->empty_queue),buf_id,PLAYER_TIMEOUT_NONE);	
	return status;
}
s32 player_get_full_buf(buf_handle *buf_hdl,u8 * buf_id)
{
	s32 status;
	status=player_que_get((buf_queue*)&(buf_hdl->full_queue), buf_id,PLAYER_TIMEOUT_FOREVER);
	return status;
}
s32 player_put_empty_buf(buf_handle *buf_hdl,u8 buf_id)
{
	s32 status=OSA_SOK;
	status=player_que_put((buf_queue*)&(buf_hdl->empty_queue),buf_id,PLAYER_TIMEOUT_FOREVER);
	return status;
}
s32 player_put_full_buf(buf_handle *buf_hdl,u8 buf_id)
{
	s32 status=OSA_SOK;
	status=player_que_put((buf_queue*)&(buf_hdl->full_queue),buf_id,PLAYER_TIMEOUT_FOREVER);
	return status;
}
static s32 player_buf_alloc(void)
{
	u8 i,j;
	//////////////////////////stream
	for(i=0;i<STREAM_CHANNEL_MAX;i++)
	{
		stream_handle[i].next_handle_buf=(buf_handle*)&decode_handle[i].cur_handle_buf;			
	}
	//////////////////////////decoder
	for(i=0;i<DECODER_MAX;i++)
	{
		decode_handle[i].buf_num=DECODE_BUF_NUM;
		decode_handle[i].buf_size=DECODE_BUF_SIZE;
		decode_handle[i].next_handle_buf=(buf_handle*)&copy_handle[i].cur_handle_buf;
		decode_handle[i].cur_handle_buf.p_dest=(u8**)decode_handle[i].buf;
		if(player_queue_create((buf_queue*)&decode_handle[i].cur_handle_buf.full_queue,decode_handle[i].buf_num,(u8*)decode_handle[i].full_buf_index)
			||player_queue_create((buf_queue*)&decode_handle[i].cur_handle_buf.empty_queue,decode_handle[i].buf_num,(u8 *)decode_handle[i].empty_buf_index))
		{
			OSA_ERROR("player_queue_create()!!!");			
			return OSA_EFAIL;
		}
		for(j=0;j<decode_handle[i].buf_num;j++)
		{
			decode_handle[i].buf[j]=(u8*)malloc(decode_handle[i].buf_size);
			if(NULL==decode_handle[i].buf[j])
			{
				OSA_ERROR("OSA_cmemAlloc()!!!");			
				return OSA_EFAIL;
			}		
			player_que_put((buf_queue*)&decode_handle[i].cur_handle_buf.empty_queue,j,OSA_TIMEOUT_FOREVER);		
		}
	}
	//////////////////////////////flu
	for(i=0;i<FLUENCY_CHANNEL_MAX;i++)
	{
		copy_handle[i].next_handle_buf=(buf_handle*)&display_handle.cur_handle_buf;
	}
	//////////////////////////////display
	display_handle.buf_num=DISPLAY_BUF_NUM;
	display_handle.buf_size=DISPLAY_BUF_SIZE;
	display_handle.next_handle_buf=(buf_handle*)&display_handle.cur_handle_buf;	
	display_handle.cur_handle_buf.p_dest=(u8 **)display_handle.buf;
	if(player_queue_create((buf_queue*)&display_handle.cur_handle_buf.full_queue,display_handle.buf_num,(u8 *)display_handle.full_buf_index)
		||player_queue_create((buf_queue*)&display_handle.cur_handle_buf.empty_queue,display_handle.buf_num,(u8 *)display_handle.empty_buf_index))
	{
		OSA_ERROR("player_queue_create()!!!");			
		return OSA_EFAIL;
	}
	for(i=0;i<display_handle.buf_num;i++)
	{
		display_handle.buf[i]= cedar_sys_phymalloc(display_handle.buf_size,4);		
		if(NULL==display_handle.buf[i])
		{
			OSA_ERROR("OSA_cmemAlloc()!!!");			
			return OSA_EFAIL;
		}		
		memset(display_handle.buf[i],0,1280*736);
		memset(display_handle.buf[i]+1280*736,0x80,1280*384);	
		cedarx_cache_op((long)display_handle.buf[i],(long)(display_handle.buf[i]+1280*(736+384)),1);
		player_que_put((buf_queue*)&display_handle.cur_handle_buf.empty_queue,i,OSA_TIMEOUT_FOREVER);
	}	
	if((dis_back_buf= (u8*)cedar_sys_phymalloc(1280*(736+384),4))==NULL)
	{
		OSA_ERROR("request back buf fail!!!");
		return OSA_EFAIL;
	}
	memset(dis_back_buf,0,1280*736);
	memset(dis_back_buf+1280*736,0x80,1280*384);
	cedarx_cache_op((long)dis_back_buf,(long)(dis_back_buf+1280*(736+384)),1);
	return OSA_SOK;
}
static void player_buf_free(void)
{
	u8 i,j;
	//////////////////释放decoder的内存
	for(j=0;j<DECODER_MAX;j++)	
	{
		for(i=0;i<decode_handle[j].buf_num;i++)
		{
			free(decode_handle[j].buf[i]);
		}	
		player_queue_delete((buf_queue*)&decode_handle[j].cur_handle_buf.full_queue);
		player_queue_delete((buf_queue*)&decode_handle[j].cur_handle_buf.empty_queue);
	}
	//////////////释放显示的内存
	for(i=0;i<display_handle.buf_num;i++)
	{
		cedar_sys_phyfree(display_handle.buf[i]);
	}	
	player_queue_delete((buf_queue*)&display_handle.cur_handle_buf.full_queue);
	player_queue_delete((buf_queue*)&display_handle.cur_handle_buf.empty_queue);
	////////显示的备份back	
	cedar_sys_phyfree(dis_back_buf);
//	cedar_sys_phyfree(copy_720p_noMB_buf);
//	cedar_sys_phyfree(copy_360p_noMB_buf);
}
s32 player_cond_create(pthread_cond_t *cond)
{
	int status=OSA_SOK;
	pthread_condattr_t cond_attr;	
	status |= pthread_condattr_init(&cond_attr);  	
	status |= pthread_cond_init(cond, &cond_attr);    
	if(status!=OSA_SOK)
	{
		OSA_ERROR("cond init fail!!! %d ", status);
	}   
	pthread_condattr_destroy(&cond_attr);
	return status;
}
s32 player_cond_delete(pthread_cond_t *cond)
{
	pthread_cond_destroy(cond);
	return OSA_SOK;
}
static s32 player_queue_create(buf_queue *queue,u8 len,u8 *buf_dest)
{
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;
	int status=OSA_SOK;

	queue->buf_id_queue=buf_dest;//buf_id_queue指向用来存放线程数据buf的数组，在线程句柄中
	queue->curRd = queue->curWr = 0;
	queue->count = 0;
	queue->len   = len;
	status |= pthread_mutexattr_init(&mutex_attr);
	status |= pthread_condattr_init(&cond_attr);  

	status |= pthread_mutex_init(&queue->lock, &mutex_attr);
	status |= pthread_cond_init(&queue->condRd, &cond_attr);    
	status |= pthread_cond_init(&queue->condWr, &cond_attr);  
	if(status!=OSA_SOK)
	{
		OSA_ERROR("queue parameter init fail!!! %d ", status);
	}   
	pthread_condattr_destroy(&cond_attr);
	pthread_mutexattr_destroy(&mutex_attr);
	
	return status;
}
static s32 player_queue_delete(buf_queue *queue)
{
	pthread_cond_destroy(&queue->condRd);
	pthread_cond_destroy(&queue->condWr);
	pthread_mutex_destroy(&queue->lock);  
	return OSA_SOK;
}

int player_mutex_create(pthread_mutex_t *lock)
{
	pthread_mutexattr_t mutex_attr;
	int status=OSA_SOK;

	status |= pthread_mutexattr_init(&mutex_attr);
	status |= pthread_mutex_init(lock,&mutex_attr);
  
	if(status!=OSA_SOK)
	{	
		OSA_ERROR("player create lock fail!!! ");
		return status;
	}
	if((status=pthread_mutexattr_destroy(&mutex_attr)))
	{
		OSA_ERROR("player destroy lock attr fail!!! ");
		return status;
	}
	return status;
}

int player_mutex_delete(pthread_mutex_t *lock)
{
	int status;
	if((status=pthread_mutex_destroy(lock)))
	{
		OSA_ERROR("player destroy lock fail!!! ");
		return status;
	}
	return OSA_SOK;
}
static u8 player_que_get_num(buf_queue*queue)
{
	u8 num;
	pthread_mutex_lock(&queue->lock);	
	num=queue->count;
	pthread_mutex_unlock(&queue->lock);
	return num;
}

s32 player_que_put(buf_queue*queue,u8 buf_id, Uint32 timeout)
{
	int status = OSA_EFAIL;	
	pthread_mutex_lock(&queue->lock);
	while(1) 
	{
		if( queue->count < queue->len )
		{
			queue->buf_id_queue[queue->curWr] = buf_id;
			queue->curWr = (queue->curWr+1)%queue->len;
			queue->count++;
			status = OSA_SOK;
			pthread_cond_signal(&queue->condRd);
			break;
		}
		else 
		{
			if(timeout == OSA_TIMEOUT_NONE)
			{
				break;
			}
			status = pthread_cond_wait(&queue->condWr, &queue->lock);
	    }
	}	
	pthread_mutex_unlock(&queue->lock);
	return status;
}

static s32 player_que_get(buf_queue*queue, u8 *buf_id, Uint32 timeout)
{
	int status = OSA_EFAIL;
	pthread_mutex_lock(&queue->lock);	
	while(1)
	{
		if(queue->count > 0 )
		{	
			if(buf_id!=NULL)
			{				
				*buf_id = queue->buf_id_queue[queue->curRd];
			}
			queue->curRd = (queue->curRd+1)%queue->len;
			queue->count--;
			status = OSA_SOK;
			pthread_cond_signal(&queue->condWr);			
			break;
		} 
		else
		{
			if(timeout == OSA_TIMEOUT_NONE)
	  		{
	  			break;
			}
			status = pthread_cond_wait(&queue->condRd, &queue->lock);
		}
	}	
	pthread_mutex_unlock(&queue->lock);
	return status;
}
void player_dis_buf_clr(void)
{
	u8 i;
	display_handle.cur_handle_buf.empty_queue.curRd = display_handle.cur_handle_buf.empty_queue.curWr = 0;
	display_handle.cur_handle_buf.empty_queue.count = 0;
	display_handle.cur_handle_buf.empty_queue.len	= display_handle.buf_num;	
	display_handle.cur_handle_buf.full_queue.curRd = display_handle.cur_handle_buf.full_queue.curWr = 0;
	display_handle.cur_handle_buf.full_queue.count = 0;
	display_handle.cur_handle_buf.full_queue.len   = display_handle.buf_num;	
	for(i=0;i<display_handle.buf_num;i++)
	{
		player_que_put((buf_queue*)&display_handle.cur_handle_buf.empty_queue,i,OSA_TIMEOUT_FOREVER);		
	}	
}
void player_dec_buf_clr(u8 ch)
{
	u8 i;
	decode_handle[ch].cur_handle_buf.empty_queue.curRd = decode_handle[ch].cur_handle_buf.empty_queue.curWr = 0;
	decode_handle[ch].cur_handle_buf.empty_queue.count = 0;
	decode_handle[ch].cur_handle_buf.empty_queue.len   = decode_handle[ch].buf_num;	
	decode_handle[ch].cur_handle_buf.full_queue.curRd = decode_handle[ch].cur_handle_buf.full_queue.curWr = 0;
	decode_handle[ch].cur_handle_buf.full_queue.count = 0;
	decode_handle[ch].cur_handle_buf.full_queue.len   = decode_handle[ch].buf_num;	
	for(i=0;i<decode_handle[ch].buf_num;i++)
	{
		player_que_put((buf_queue*)&decode_handle[ch].cur_handle_buf.empty_queue,i,OSA_TIMEOUT_FOREVER);		
	}
}
//延迟ms
void player_waitMsecs(u32 msecs)
{
	struct timespec delayTime, elaspedTime;
	int ret;
	delayTime.tv_sec  = msecs/1000;
	delayTime.tv_nsec = (msecs%1000)*1000000;
	do
	{
		ret=nanosleep(&delayTime, &elaspedTime);
		if(ret)
		{		
				delayTime.tv_sec=elaspedTime.tv_sec;
				delayTime.tv_nsec=elaspedTime.tv_nsec;
		}
	}while(ret);
}
//延迟ns
void player_waitnsecs(u32 nsecs)
{
	struct timespec delayTime, elaspedTime;
	int ret;
	delayTime.tv_sec  =0;
	delayTime.tv_nsec =nsecs;
	do
	{
		ret=nanosleep(&delayTime, &elaspedTime);
		if(ret)
		{		
			delayTime.tv_sec=0;
			delayTime.tv_nsec=elaspedTime.tv_nsec;
		}
	}while(ret);
}
static s8 player_send_msg_to_ui(_pro_msg_data *data,int flag)
{
	_pro_msg send_buf;
	static char init=0;
	static int msg_id;
	key_t keyval;	
	if(!init)
	{
		init=1;
		if(-1==(keyval=ftok(".",PLAYER_PRO_IPC_KEY_ID)))//获取一个消息队列的键值
		{
			OSA_ERROR("get keyval fail");
			return -1;
		}
		if(-1==(msg_id=msgget(keyval,IPC_CREAT|0666)))
		{
			OSA_ERROR("get msgid fail");
			return -1;
		}
	}
	send_buf.msg_type=PLAYER_MSG_TYPE_SEND;
	memcpy(&send_buf.pro_data,data,sizeof(_pro_msg_data));
	if(msgsnd(msg_id,&send_buf,sizeof(_pro_msg_data),flag))
	{
		OSA_ERROR("player send msg fail!!!");								
		return -1;
	}
	return 0;
}
static s8 player_pro_dis_mod_cmd(player_ctl*hdl)
{
	hdl->single_en=hdl->pro_msg.pro_data.single_en;
	hdl->thr_msg.thr_data.channel=hdl->pro_msg.pro_data.channel;		
	if(dis_put_msg_to_queue(&hdl->thr_msg.thr_data,hdl->single_en?DISPLAY_CMD_TEMP_FULLSCREEN:DISPLAY_CMD_TEMP_SLIPSCREEN,0))
	{
		OSA_ERROR("player send dis mode cmd%d fail!!!",hdl->single_en);
		return OSA_EFAIL;
	}
	if(hdl->single_en)
	{
		char i;
		decode_set_out_res(hdl->thr_msg.thr_data.channel,hdl->single_en?720:360);
		for(i=0;i<STREAM_CHANNEL_MAX;i++)
		{
			if(i!=hdl->thr_msg.thr_data.channel)
			{
				decode_set_out_res(i,360);
			}
		}
	}
	else
	{
		char i;
		for(i=0;i<STREAM_CHANNEL_MAX;i++)
		{
			decode_set_out_res(i,hdl->single_en?720:360);
		}
	}
	copy_set_single_channel(hdl->single_en?hdl->thr_msg.thr_data.channel:-1);
	if(hdl->single_en&&(!stream_preview_status(hdl->thr_msg.thr_data.channel)))
	{
		if((!(hdl->str_standby_already&(1<<hdl->thr_msg.thr_data.channel)))
			&&(decode_put_msg_to_queue(hdl->thr_msg.thr_data.channel,&hdl->thr_msg.thr_data,DECODE_CMD_BLACK,0)))
		{
			OSA_ERROR("player send ch%d cmd%d fail!!!",hdl->thr_msg.thr_data.channel,DECODE_CMD_BLACK);
		}
	}
	return OSA_SOK;
}
static void player_pro_stop_already_cmd(player_ctl*hdl)
{
	u8 ch=hdl->thr_msg.thr_data.channel;	
	DEBUG_PF("player rcv str%d standby already cmd",ch);						
	player_dec_buf_clr(ch);	
	hdl->str_standby_already|=(1<<ch);
	hdl->wait_return&=(~(1<<ch));	
	if(hdl->str_standby_already==0x0f)
	{								
		player_dis_buf_clr();
	}						
	if(!hdl->wait_return)
	{
		player_set_sta(hdl,PLAYER_THR_STA_IDLE);
	}
}
static s8 player_pro_stop_cmd(player_ctl*hdl)
{
	u8 st_index,end_index,send=0;
	_pro_msg_data *pro_data=&hdl->pro_msg.pro_data;	
	DEBUG_PF("player rcv ch%d stop cmd",pro_data->channel);						
	st_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel:0;
	end_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel+1:STREAM_CHANNEL_MAX;
	for(;st_index<end_index;st_index++)
	{
		if(player_set_ch_stop(hdl,st_index,1,&send))
		{
			OSA_ERROR("player_set_ch_stop fail");
			return OSA_EFAIL;
		}
		if(send)
		{			
			hdl->wait_return|=(1<<st_index);
			player_set_sta(hdl,PLAYER_THR_STA_STOP);
		}
	}	
	return OSA_SOK;
}
static s8 player_set_ch_stop(player_ctl*hdl,u8 ch,u8 black,u8 *send)
{
	_thr_msg_data *thr_data=&hdl->thr_msg.thr_data;
	if(!(hdl->str_standby_already&(1<<ch)))
	{
		hdl->str_pause_already&=(~(1<<ch));
		thr_data->black_screen=black;
		hdl->state[ch].channel_status=CHANNEL_STANDBY; 	
		*send=1;		
		thr_data->channel=ch;
		if(stream_put_msg_to_queue(ch,thr_data,STREAM_CMD_STANDBY,0))
		{
			OSA_ERROR("player send ch%d cmd%d fail!!!",ch,STREAM_CMD_STANDBY);
			return OSA_EFAIL;
		}
	}
	else
	{		
		*send=0;
		DEBUG_PF("ch%d is stop sta",ch);
	}	
	return OSA_SOK;
}
static void player_pro_exit_already_cmd(player_ctl*hdl)
{
	if(hdl->str_standby_already==0x0f)
	{
		hdl->exit=1; 				
		player_all_thr_exit(hdl->msg_id);
	}
}
static s8 player_pro_exit_cmd(player_ctl*hdl)
{
	u8 i,send;	
	DEBUG_PF("player rcv all exit cmd");				
	if(0x0f==hdl->str_standby_already)
	{								
		hdl->exit=1; 				
		player_all_thr_exit(hdl->msg_id); 								
	}
	else
	{
		for(i=0;i<STREAM_CHANNEL_MAX;i++)
		{
			if(player_set_ch_stop(hdl,i,0,&send))
			{
				OSA_ERROR("player_set_ch_stop fail");
				return OSA_EFAIL;
			}		
			if(send)
			{
				hdl->wait_return|=(1<<i);
				player_set_sta(hdl,PLAYER_THR_STA_EXIT);
			}
		}
	}
	return OSA_SOK;
}
static void player_pro_play_already_cmd(player_ctl*hdl)
{
	u8 ch=hdl->thr_msg.thr_data.channel;
	DEBUG_PF("player rcv str%d work already cmd",ch);	
	hdl->wait_return&=(~(1<<ch));
	if(!hdl->wait_return)
	{
		player_set_sta(hdl,PLAYER_THR_STA_IDLE);
	}
}
static s8 player_pro_play_cmd(player_ctl*hdl)
{
	u8 st_index,end_index,send=0;
	_pro_msg_data *pro_data=&hdl->pro_msg.pro_data;	
	DEBUG_PF("player rcv ch%d connect cmd",pro_data->channel);						
	st_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel:0;
	end_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel+1:STREAM_CHANNEL_MAX;
	for(;st_index<end_index;st_index++)
	{
		if(player_set_ch_play(hdl,st_index,&send))
		{
			OSA_ERROR("player_set_ch_play fail");
			return OSA_EFAIL;
		}
		if(send)
		{			
			hdl->wait_return|=(1<<st_index);
			player_set_sta(hdl,PLAYER_THR_STA_PLAY);
		}
	}	
	return OSA_SOK;
}
static s8 player_set_ch_play(player_ctl*hdl,u8 ch,u8 *send)
{
	_thr_msg_data *thr_data=&hdl->thr_msg.thr_data;
	if(hdl->str_standby_already&(1<<ch))
	{//此通道现在处于standby状态,并且不在回放状态。可以播放
		hdl->str_standby_already&=(~(1<<ch));		
		memcpy(&thr_data->rect,&hdl->rect,sizeof(__disp_rect_t));
//		thr_data->single_en=hdl->single_en; 
		thr_data->record=(hdl->record_en&(1<<ch))?GET_STREAM_FROME_DISK:GET_STREAM_FROME_NET;
		hdl->state[ch].channel_status=CHANNEL_PALY;				
		*send=1;		
		thr_data->channel=ch;
		if(stream_put_msg_to_queue(ch,thr_data,STREAM_CMD_CONNECT,0))
		{
			OSA_ERROR("player send ch%d cmd%d fail!!!",ch,STREAM_CMD_CONNECT);
			return OSA_EFAIL;
		}
	}
	else
	{
		*send=0;
		DEBUG_PF("ch%d is play sta",ch);
	}
	return OSA_SOK;
}

static s8 player_pro_get_sta_already_cmd(player_ctl*hdl)
{
	_thr_msg_data *thr_data=&hdl->thr_msg.thr_data;
	_pro_msg_data *pro_data=&hdl->pro_msg.pro_data;
	u8 ch=thr_data->channel;	
	hdl->state[ch].query_num=thr_data->run_status.query_num;
	hdl->state[ch].play_time_point=thr_data->run_status.play_time_point;
	hdl->state[ch].replay_end=thr_data->run_status.replay_end;
	hdl->wait_return&=(~(1<<ch));
	if(!hdl->wait_return)
	{
		memcpy(pro_data->run_status,hdl->state,sizeof(player_status)*STREAM_CHANNEL_MAX);
		pro_data->cmd=PLAYER_CMD_RET_STATUS;
		if(player_send_msg_to_ui(pro_data,0))
		{
			OSA_ERROR("player send cmd:%d fail!!!",PLAYER_CMD_RET_STATUS);
			return OSA_EFAIL;
		}	
		player_set_sta(hdl,PLAYER_THR_STA_IDLE);
	}
	return OSA_SOK;
}
static s8 player_pro_get_sta_cmd(player_ctl*hdl)
{
	u8 st_index,end_index;
	_pro_msg_data *pro_data=&hdl->pro_msg.pro_data;
	_thr_msg_data *thr_data=&hdl->thr_msg.thr_data;
	st_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel:0;
	end_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel+1:STREAM_CHANNEL_MAX;
	for(;st_index<end_index;st_index++)
	{
		if(hdl->record_en&(1<<st_index))
		{//在回放状态			
			hdl->wait_return|=(1<<st_index);			
			thr_data->channel=st_index;
			player_set_sta(hdl,PLAYER_THR_STA_GET_PLAYER_STATUS);
			if(stream_put_msg_to_queue(st_index,thr_data,STREAM_CMD_QUERY_PLAY_TIME,0))
			{
				OSA_ERROR("player send ch%d cmd%d fail!!!",st_index,STREAM_CMD_QUERY_PLAY_TIME);
				return OSA_EFAIL;
			}
		}
		else
		{//不在回放状态
			hdl->state[st_index].query_num=0;
			hdl->state[st_index].play_time_point=0;
		}			
	}		
	if(!hdl->record_en)
	{//所有通道都不在回放状态，发送命令，立刻返回
		pro_data->cmd=PLAYER_CMD_RET_STATUS;
		if(player_send_msg_to_ui(pro_data,0))
		{
			OSA_ERROR("player send cmd:%d fail!!!",PLAYER_CMD_RET_STATUS);			
			return OSA_EFAIL;
		}										
	}	
	return OSA_SOK;
}
static s8 player_pro_play_mode_cmd(player_ctl*hdl)
{
	u8 st_index,end_index,send=0;
	_pro_msg_data *pro_data=&hdl->pro_msg.pro_data;	
	DEBUG_PF("player rcv ch%d set play mode cmd:%d",pro_data->channel,pro_data->cmd); 							
	st_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel:0;
	end_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel+1:STREAM_CHANNEL_MAX;
	for(;st_index<end_index;st_index++)
	{
		if(PLAYER_CMD_RECORD_PLAY==pro_data->cmd)
		{
			DEBUG_PF("player rcv ch%d record cmd",st_index);									
			hdl->record_en|=(1<<st_index);
		}
		else
		{
			DEBUG_PF("player rcv ch%d real play cmd",st_index);									
			hdl->record_en&=(~(1<<st_index));
		}		
		if(player_set_ch_stop(hdl,st_index,1,&send))
		{
			OSA_ERROR("player_set_ch_stop fail");
			return OSA_EFAIL;
		}
		if(send)
		{
			hdl->wait_return|=(1<<st_index);
			player_set_sta(hdl,PLAYER_THR_STA_RECORD_PLAYE);
		}
	}
	return OSA_SOK;
}
static void player_pro_set_rate_already_cmd(player_ctl *hdl)
{
	u8 ch=hdl->thr_msg.thr_data.channel;
	DEBUG_PF("player rcv str%d set rate already cmd",ch);
	hdl->wait_return&=(~(1<<ch));
	if(!hdl->wait_return)
	{
		player_set_sta(hdl,PLAYER_THR_STA_IDLE);
	}
}
static s8 player_pro_set_rate_cmd(player_ctl *hdl)
{
	u8 st_index,end_index;
	_pro_msg_data *pro_data=&hdl->pro_msg.pro_data;
	_thr_msg_data *thr_data=&hdl->thr_msg.thr_data;	
	DEBUG_PF("player rcv ch%d set rate cmd",pro_data->channel);	
	st_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel:0;
	end_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel+1:STREAM_CHANNEL_MAX;
	for(;st_index<end_index;st_index++)
	{		
		if(hdl->record_en&(1<<st_index))
		{
			hdl->wait_return|=(1<<st_index);			
			thr_data->channel=st_index;
			thr_data->rate=pro_data->rate;
			//thr_data->single_en=hdl->single_en;
			player_set_sta(hdl,PLAYER_THR_STA_SET_RATE);
			if(stream_put_msg_to_queue(st_index,thr_data,STREAM_CMD_SET_RATE,0))
			{
				OSA_ERROR("player send ch%d cmd:%d fail!!!",st_index,STREAM_CMD_SET_RATE);
				return OSA_EFAIL;
			}
			/*
			if(((pro_data->rate>=FLU_WORK_F2_RATE)&&(pro_data->rate<=FLU_WORK_F8_IFRAME_RATE))
				||((pro_data->rate>=FLU_WORK_S2_RATE)&&(pro_data->rate<=FLU_WORK_S8_RATE))
				||(pro_data->rate==FLU_WORK_NOMAL_RATE))										
			{
				fluency_set_rate(st_index,pro_data->rate);
			}
			*/
		}
		else
		{
			DEBUG_PF("ch%d isn't at record status",st_index);
		}
	}	
	return OSA_SOK;
}
static s8 player_pro_choose_file_already_cmd(player_ctl *hdl)
{
	u8 ch=hdl->thr_msg.thr_data.channel,send=0;
	if(STREAM_CMD_CHOOSE_FILE_ALREADY==hdl->thr_msg.thr_data.cmd)
	{
		DEBUG_PF("player rcv str:%d choose file already cmd",ch); 		
		if(hdl->str_standby_already&(1<<ch))
		{
			if(player_set_ch_play(hdl,ch,&send))
			{
				OSA_ERROR("player_set_ch_play fail");
				return OSA_EFAIL;
			}
		}
		else if(hdl->str_pause_already&(1<<ch))
		{
			if(player_set_ch_pause(hdl,ch,0,&send))
			{
				OSA_ERROR("player_set_ch_pause fail");
				return OSA_EFAIL;			
			}		

		}
		else
		{			
			hdl->wait_return&=(~(1<<ch));
			if(!hdl->wait_return)
			{
				player_set_sta(hdl,PLAYER_THR_STA_IDLE);
			}
		}
	}
	else if((STREAM_CMD_WORK_ALREADY==hdl->thr_msg.thr_data.cmd)
			||(STREAM_CMD_PAUSE_ALREADY==hdl->thr_msg.thr_data.cmd))
	{					
		DEBUG_PF("player rcv str:%d work or pause already cmd",ch); 	
		hdl->wait_return&=(~(1<<ch));
		if(!hdl->wait_return)
		{
			player_set_sta(hdl,PLAYER_THR_STA_IDLE);
		}
	}
	else if(STREAM_CMD_STANDBY_ALREADY==hdl->thr_msg.thr_data.cmd)
	{	
		DEBUG_PF("player rcv str:%d standby already cmd",ch); 	
		player_pro_stop_already_cmd(hdl);
	}
	return OSA_SOK;
}
static s8 player_pro_choose_file_cmd(player_ctl *hdl)
{
	u8 st_index,end_index,send;	
	_pro_msg_data *pro_data=&hdl->pro_msg.pro_data;
	_thr_msg_data *thr_data=&hdl->thr_msg.thr_data;	
	DEBUG_PF("player rcv ch%d choose file cmd",pro_data->channel);																
	st_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel:0;
	end_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel+1:STREAM_CHANNEL_MAX;
	for(;st_index<end_index;st_index++)
	{
		if(hdl->record_en&(1<<st_index))			
		{			
			if(pro_data->replay_infor[st_index].query.chn>=0)
			{//有录像文件				
				memcpy(&thr_data->replay_infor,&pro_data->replay_infor[st_index],sizeof(file_info));
				thr_data->update_now=pro_data->update_now;				
				thr_data->channel=st_index;
				hdl->wait_return|=(1<<st_index);
				if(stream_put_msg_to_queue(st_index,&hdl->thr_msg.thr_data,STREAM_CMD_CHOOSE_FILE,0))
				{
					OSA_ERROR("player send msg fail!!!");
					return OSA_EFAIL;
				}	
				player_set_sta(hdl,PLAYER_THR_STA_CHOOSE_FILE);
			}
			else
			{//没有录像文件
				player_set_ch_stop(hdl,st_index,1,&send);
				if(send)
				{					
					hdl->wait_return|=(1<<st_index);
					player_set_sta(hdl,PLAYER_THR_STA_CHOOSE_FILE);
				}
			}
		}
		else
		{
			DEBUG_PF("ch%d isn't record mode,record_en:%x",st_index,hdl->record_en);
		}		
	}
	return OSA_SOK;	
}
static s8 player_set_ch_pause(player_ctl *hdl,u8 ch,u8 pause,u8 *send)
{	
	if(hdl->record_en&(1<<ch))
	{
		if(pause
			&&(!(hdl->str_pause_already&(1<<ch)))
			&&(!(hdl->str_standby_already&(1<<ch))))
		{
			hdl->str_pause_already|=(1<<ch);
			hdl->thr_msg.thr_data.pause=pause;
			hdl->state[ch].channel_status=CHANNEL_PAUSE;				
			*send=1;
			hdl->thr_msg.thr_data.channel=ch;
			if(stream_put_msg_to_queue(ch,&hdl->thr_msg.thr_data,STREAM_CMD_PAUSE,0))
			{
				OSA_ERROR("player send cmd%d fail!!!",STREAM_CMD_PAUSE);
				return OSA_EFAIL;				
			}	
		}
		else if((!pause)
				&&(hdl->str_pause_already&(1<<ch)))
		{
			hdl->str_pause_already&=(~(1<<ch));
			hdl->thr_msg.thr_data.pause=pause;
			hdl->state[ch].channel_status=CHANNEL_PALY;					
			hdl->thr_msg.thr_data.channel=ch;
			if(stream_put_msg_to_queue(ch,&hdl->thr_msg.thr_data,STREAM_CMD_PAUSE,0))
			{
				OSA_ERROR("player send msg fail!!!");
				return OSA_EFAIL;
			}			
			*send=1;
		}	
		else
		{
			*send=0;
			OSA_ERROR("player ch%d can't execute pause cmd",ch);
		}
	}
	else
	{
		*send=0;				
		OSA_ERROR("ch%d don't at replay sta",ch);
	}
	return OSA_SOK;
}
static void player_pro_pause_already_cmd(player_ctl *hdl)
{
	u8 ch=hdl->thr_msg.thr_data.channel;	
	DEBUG_PF("player rcv str%d pause already cmd",ch);
	hdl->wait_return&=(~(1<<ch));
	if(!hdl->wait_return)
	{
		player_set_sta(hdl,PLAYER_THR_STA_IDLE);
	}
}
static s8 player_pro_pause_cmd(player_ctl *hdl)
{
	u8 st_index,end_index,send;
	_pro_msg_data *pro_data=&hdl->pro_msg.pro_data;	
	DEBUG_PF("player rcv ch%d pause cmd:%d",pro_data->channel,pro_data->pause);								
	st_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel:0;
	end_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel+1:STREAM_CHANNEL_MAX;
	for(;st_index<end_index;st_index++)
	{
		if(player_set_ch_pause(hdl,st_index,pro_data->pause,&send))
		{
			OSA_ERROR("player_pro_pause_cmd");
			return OSA_EFAIL;
		}
		if(send)
		{
			hdl->wait_return|=(1<<st_index);
			player_set_sta(hdl,PLAYER_THR_STA_PAUSE);
		}
	}
	return OSA_SOK;
}
static s8 player_pro_single_frame_play(player_ctl *hdl)
{
	u8 st_index,end_index;
	_pro_msg_data *pro_data=&hdl->pro_msg.pro_data;	
	_thr_msg_data *thr_data=&hdl->thr_msg.thr_data;		
	DEBUG_PF("player rcv ch%d single play cmd",pro_data->channel);																
	st_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel:0;
	end_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel+1:STREAM_CHANNEL_MAX;
	for(;st_index<end_index;st_index++)
	{
		if((hdl->record_en&(1<<st_index))
			&&(hdl->str_pause_already&(1<<st_index)))
		{	//此通道处于pause 状态	
			if(PLAYER_CMD_DOWN_FRAME==pro_data->cmd)
			{
				DEBUG_PF("player rcv ch%d down frame cmd",st_index);
				thr_data->single_dir=DOWN_FRAME;
			}
			else
			{
				DEBUG_PF("player rcv ch%d up frame cmd",st_index);
				thr_data->single_dir=UP_FRAME;
			}
			//hdl->player_msgbuf.msg_type=str_msg_type[st_index];
			thr_data->channel=st_index;
			if(stream_put_msg_to_queue(st_index,thr_data,STREAM_CMD_SINGLE_FRAME,0))
			{
				OSA_ERROR("player send ch%d cmd:%d fail!!!",st_index,STREAM_CMD_SINGLE_FRAME);
				return OSA_EFAIL;
			}
		}
		else
		{
			DEBUG_PF("please pause ch%d channel",st_index);
		}
	}
	return OSA_SOK;
}
static void player_pro_set_rect_cmd(player_ctl*hdl)
{
	memcpy(&hdl->rect,&hdl->pro_msg.pro_data.rect,sizeof(__disp_rect_t));
	DEBUG_PF("player rcv set record rect cmd x:%d,y:%d,w:%d,h:%d",hdl->rect.x,hdl->rect.y,hdl->rect.width,hdl->rect.height);									
}
static s8 player_pro_get_dis_mode_cmd(player_ctl *hdl)
{
	DEBUG_PF("player rcv get display mode cmd");																	
	hdl->pro_msg.pro_data.cmd=PLAYER_CMD_RET_DISPLAY_MODE;
	hdl->pro_msg.pro_data.single_en=hdl->single_en;
	if(player_send_msg_to_ui(&hdl->pro_msg.pro_data,0))
	{
		OSA_ERROR("player send ui cmd:%d fail!!!",PLAYER_CMD_RET_DISPLAY_MODE);
		return OSA_EFAIL;
	}										
	player_set_sta(hdl,PLAYER_THR_STA_IDLE);
	return OSA_SOK;
}
static s8 player_pro_get_preview_sta_cmd(player_ctl*hdl)
{
	u8 st_index,end_index;
	_pro_msg_data *pro_data=&hdl->pro_msg.pro_data;	
	DEBUG_PF("player rcv ch:%d get preview status cmd",pro_data->channel);									
	pro_data->preview_status=0;
	st_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel:0;
	end_index=((pro_data->channel>=0)&&(pro_data->channel<=3))?pro_data->channel+1:STREAM_CHANNEL_MAX;
	for(;st_index<end_index;st_index++)
	{
		if((!(hdl->str_standby_already&(1<<st_index)))
			&&(!(hdl->record_en&(1<<st_index)))
			&&stream_preview_status(st_index))
		{//在实时预览状态
			pro_data->preview_status|=(1<<st_index);
		}
		else
		{
			OSA_ERROR("this ch isn't at preview stat"); 								
			pro_data->preview_status&=(~(1<<st_index));
		}
	}
	pro_data->cmd=PLAYER_CMD_RET_PREVIEW_STATUS;								
	if(player_send_msg_to_ui(pro_data,0))
	{
		OSA_ERROR("player send msg fail!!!");
		return OSA_EFAIL;
	}											
	return OSA_SOK;
}
static s8 player_pro_set_audio_cmd(player_ctl*hdl)
{
	u8 ch=hdl->pro_msg.pro_data.channel;	
	DEBUG_PF("player rcv ch:%d set audio cmd:%d",ch,hdl->pro_msg.pro_data.audio_ctl);		
	hdl->thr_msg.thr_data.audio_ctl=hdl->pro_msg.pro_data.audio_ctl;
	if(stream_put_msg_to_queue(ch,&hdl->thr_msg.thr_data,STREAM_CMD_SET_AUDIO,0))
	{
		OSA_ERROR("player send msg fail!!!");
		return OSA_EFAIL;
	}	
	return OSA_SOK;
}
static s8 player_pro_set_src_rect(player_ctl*hdl)
{	
	__disp_rect_t *rect=&hdl->pro_msg.pro_data.rect;
	DEBUG_PF("player rcv set src rect cmd x:%u,y:%u,width:%u,height:%u",rect->x,rect->y,rect->width,rect->height);			
	memcpy(&hdl->thr_msg.thr_data.rect,&hdl->pro_msg.pro_data.rect,sizeof(__disp_rect_t));
	if(dis_put_msg_to_queue(&hdl->thr_msg.thr_data,DISPLAY_CMD_SET_SRC_RECT,0))
	{
		OSA_ERROR("player send msg fail!!!");
		return OSA_EFAIL;
	}
	return OSA_SOK;
}
static s8 player_pro_set_scn_rect(player_ctl*hdl)
{
	__disp_rect_t *rect=&hdl->pro_msg.pro_data.rect;
	DEBUG_PF("player rcv set scr rect cmd x:%u,y:%u,width:%u,height:%u",rect->x,rect->y,rect->width,rect->height);			
	memcpy(&hdl->thr_msg.thr_data.rect,&hdl->pro_msg.pro_data.rect,sizeof(__disp_rect_t));
	if(dis_put_msg_to_queue(&hdl->thr_msg.thr_data,DISPLAY_CMD_SET_SCN_RECT,0))
	{
		OSA_ERROR("player send msg fail!!!");
		return OSA_EFAIL;
	}
	return OSA_SOK;
}
static s8 player_pro_set_elec_rsz_cmd(player_ctl*hdl)
{
	DEBUG_PF("player rcv set elec cmd:%d",hdl->pro_msg.pro_data.elec_rsz);									
	hdl->thr_msg.thr_data.elec_rsz=hdl->pro_msg.pro_data.elec_rsz;
	memcpy(&hdl->thr_msg.thr_data.rect,&hdl->pro_msg.pro_data.rect,sizeof(__disp_rect_t));
	if(dis_put_msg_to_queue(&hdl->thr_msg.thr_data,DISPLAY_CMD_SET_ELEC_RSZ,0))
	{
		OSA_ERROR("player send msg fail!!!");
		return OSA_EFAIL;
	}
	return OSA_SOK;
}
static void player_pro_stop_phone_audio_cmd(void)
{
	audio_stop(AUDIO_TYPE_PHONE);
}
static void player_pro_start_phone_audio_cmd(void)
{	
	audio_play(AUDIO_TYPE_PHONE);
}
static void player_pro_start_audio_record_cmd(player_ctl*hdl)
{	
	audio_record_set_cmd(AUDIO_RECORD_START_CMD);
}
static void player_pro_stop_audio_record_cmd(player_ctl*hdl)
{	
	audio_record_set_cmd(AUDIO_RECORD_STOP_CMD);
}
static void player_pro_set_audio_rec_par_cmd(player_ctl*hdl)
{
	audio_record_set_rec_par(&hdl->pro_msg.pro_data.audio_enc_par);
}
static void player_pro_alarm_audio_cmd(player_ctl*hdl)
{
	DEBUG_PF("player rcv set alarm audio cmd:%d,file %s",hdl->pro_msg.pro_data.alarm_play,hdl->pro_msg.pro_data.alarm_file);	
	if(hdl->pro_msg.pro_data.alarm_play)
	{
		audio_set_alarm_file((char*)hdl->pro_msg.pro_data.alarm_file,1);
		audio_play(AUDIO_TYPE_ALARM);
	}
	else
	{
		audio_stop(AUDIO_TYPE_ALARM);
	}
}
static void player_pro_set_cap_num_cmd(player_ctl *hdl)
{
	DEBUG_PF("player rcv ch%d set capture num:%d cmd",hdl->pro_msg.pro_data.channel,hdl->pro_msg.pro_data.cap_num);	
	copy_add_cat_num(hdl->pro_msg.pro_data.channel,hdl->pro_msg.pro_data.cap_num);
}
static void player_set_sta(player_ctl *hdl,u8 sta)
{
	if(PLAYER_THR_STA_IDLE==sta)
	{
		hdl->rcv_type=PLAYER_MSG_TYPE_RCV_PRO;
	}
	else
	{
		hdl->rcv_type=PLAYER_MSG_TYPE_RCV;
	}
	hdl->player_thr_sta=sta;	
}
static void player_parameter_init(player_ctl *hdl)
{
	memset(hdl->state,0,sizeof(play_status)*STREAM_CHANNEL_MAX);
	hdl->exit=0;
	hdl->rcv_type=PLAYER_MSG_TYPE_RCV_PRO;
	hdl->str_standby_already=0x0f;
	hdl->player_thr_sta=PLAYER_THR_STA_IDLE;
	hdl->str_pause_already=0x00;
	hdl->single_en=0;
	hdl->record_en=0x00;
	hdl->wait_return=0x00;
	hdl->rect.x=0;
	hdl->rect.y=0;
	hdl->rect.width=1280;
	hdl->rect.height=720;	
}
s8 player_put_msg_to_queue(_thr_msg_data *data,u8 cmd,int flag)
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
	send_buf.msg_type=PLAYER_MSG_TYPE_RCV;
	memcpy(&send_buf.thr_data,data,sizeof(_thr_msg_data));
	send_buf.thr_data.cmd=cmd;
	if(msgsnd(msg_id,&send_buf,sizeof(_thr_msg_data),flag))
	{
		OSA_ERROR("player send cmd:%d fail!!!",cmd);								
		pthread_mutex_unlock(&msg_lock);	
		return -1;
	}
	pthread_mutex_unlock(&msg_lock);	
	return 0;
}

static void player_signal_quit(int signo)
{			
	int msg_id;
	key_t keyval;	
	_pro_msg player_msgbuf;	
	keyval=ftok(".",PLAYER_PRO_IPC_KEY_ID);//获取一个消息队列的键值
	msg_id=msgget(keyval,IPC_CREAT|0666);//创建一个消息队列,并且设置读写权限。如果消息队列已经存在则打开		
	player_msgbuf.msg_type=PLAYER_MSG_TYPE_RCV_PRO;
	player_msgbuf.pro_data.cmd=PLAYER_CMD_ALL_EXIT;
	if(msgsnd(msg_id,&player_msgbuf,sizeof(_pro_msg_data),0))
	{
		OSA_ERROR("signal quit send msg fail!!!");
	}
}
