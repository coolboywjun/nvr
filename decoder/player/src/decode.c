#include<player.h>
#include<osa_thr.h>
#include <unistd.h>
//#include<vdecoder.h>
#include<sys/prctl.h>

#define HEAD_NUM (10)
enum{
	DEC_THR_STA_WORK_ON=0,
	DEC_THR_STA_STANDBY,
	DEC_THR_STA_EXIT,
};
typedef struct
{
	u64 cur_frame_stamp;
	u64 pre_frame_stamp;
	u64 dec_num;
	u32 discard_frame_num;
	u32 cur_dec_time;
	u32 pre_dec_time;
	u32 delay_time;
	u32 all_time;
	u32 cur_out_res;
	u32 old_out_res;
	u32 average_timestamp;
	u32 dec_frame_no;
	u8 num;
	u8 dec_thr_sta;
	u8 dec_id;
	u8 record_en;
	u32 play_id;
////////消息队列
	int msg_id;
	int msg_flg;
	_thr_msg dec_msgbuf;
////////////
}decode_ctl;

typedef struct{
	buf_head cur_head_infor[HEAD_NUM];//不能小于解码器的要求的输入buf数量
	u8 buf_head_index;
	u16 real_frame_rate;
	u8 put_decoder_num;
	cedarv_decoder_t* decoder; 
	pthread_mutex_t decode_lock;	
	pthread_cond_t decode_cond;
}pic_infor;
#define FRAME_NUM (40)
#define DECODE_MAX_STORAGE_FRAME_NUM (7)//解码库中能够在同一时刻存储的帧的数量，超过此数量，解码函数会返回4
#define DECODE_BUF_CLR_TIME (2000)//(2000)
#define DECODE_IPC_CHANGE (1500000)//500000us

#ifdef PLAYER_TEST
static decode_ctl* dec_test[DECODER_MAX];
#endif
volatile static gint dec_cmd[DECODER_MAX]={0,0,0,0};
volatile static gint dec_res[DECODER_MAX]={360,360,360,360};
static OSA_ThrHndl decode_thr[DECODER_MAX];
static pthread_mutex_t msg_lock;
static pic_infor picture_infor[DECODER_MAX];

pthread_mutex_t all_decode_lock;
volatile dec_handle decode_handle[DECODER_MAX];
const u8 dec_msg_type[DECODER_MAX]={DEC1_MSG_TYPE_RCV,DEC2_MSG_TYPE_RCV,DEC3_MSG_TYPE_RCV,DEC4_MSG_TYPE_RCV};
////////////
static void *decode_thr_main(void*param);
static int decode_thr_decoder_init(cedarv_decoder_t**decoder,u32 out_res);
static void decode_thr_decoder_exit(cedarv_decoder_t**decoder);
static void decode_thr_check_put_deocder_num(pic_infor *handle);
static void decode_thr_releas_decoder_num(pic_infor *handle);
static void decode_save_buf_head(pic_infor *handle,buf_head *head);
static void decode_get_buf_head(pic_infor *handle,buf_head *head);
static s8 decode_discard_frame_judge(decode_ctl*hdl,u8 frame_type,u32 frame_no,u32 *first_frame);
static void decode_delay_count(decode_ctl*hdl,u32 timestamp);;
static void decode_rate_count(decode_ctl*hdl,u32 timestamp);
static void decode_check_continue(decode_ctl*hdl,u32 *first_frame);
static s8 decode_res_check(decode_ctl*hdl);
static s8 decode_lib_buf_request_copy(decode_ctl*hdl,u32 len,u8 *src_buf);
static void decode_lib_buf_update(decode_ctl*hdl,u32 len,u64 pts);
static s8 decode_lib_decode(decode_ctl *hdl);
static void decode_lib_abnormal_conf(decode_ctl*hdl,u8 cfg);
static s8 decode_decode_pro(decode_ctl*hdl,u8*buf,u32 len);
static s8 decode_video_data_pro(decode_ctl*hdl);
static void decode_parameter_init(decode_ctl *hdl);
static u8 decode_clr_data(u8 ch);
void decode_set_out_res(u8 ch,int res);
static u32 decode_get_out_res(u8 ch);
static u8 decode_thr_decode_ready(u8 dec_id);
/*
decode_put_msg_to_queue
decode_get_msg
*/
///////////

int decode_thr_create(u8* param)
{
	int status=0;
	static u8 dec_init=0;
	if(!dec_init)
	{
		dec_init=1;
		status|=player_mutex_create(&all_decode_lock);//创建一个互斥锁
		status|=player_mutex_create(&msg_lock);
		if(status)
		{
			OSA_ERROR("player_mutex_create():%d!!!",status);
			return status;
		}
		memset((char*)picture_infor,0,sizeof(picture_infor));//帧率默认也是1
	}
	
	//status=pthread_create(&decode_thr[*param].hndl,NULL,decode_thr_main,(void*)param);
	status = OSA_thrCreate(&decode_thr[*param], decode_thr_main, DECODE_THR_PRI, 0,param);  
	if(status!=OSA_SOK) 
	{
		OSA_ERROR("OSA_tskCreate()");
	}
	return status;
}
static void *decode_thr_main(void*param)
{
	u8 exit=0;
	int status=0;
	char pr_name[10];
	key_t keyval;	
	decode_ctl dec_hdl;	
	_thr_msg_data *dec_data;
	dec_hdl.dec_id=*((u8*)param);
	status|=player_mutex_create((pthread_mutex_t*)&picture_infor[dec_hdl.dec_id].decode_lock);//创建一个互斥锁
	status|=player_cond_create(&picture_infor[dec_hdl.dec_id].decode_cond);//条件变量初始化
	if(status)
	{
		OSA_ERROR("player_mutex_create()or player_cond_create() fail:%d!!!",status);
		return NULL;
	}		
	sprintf(pr_name,"dec%d",dec_hdl.dec_id);	
	prctl(PR_SET_NAME, pr_name);
	DEBUG_PF("dec id:%d run",dec_hdl.dec_id);
	if(-1==(keyval=ftok(".",PLAYER_THR_IPC_KEY_ID)))//获取一个消息队列的键值
	{
		OSA_ERROR("get keyval fail");
	}
	if(-1==(dec_hdl.msg_id=msgget(keyval,IPC_CREAT|0666)))
	{
		OSA_ERROR("get msgid fail");
	}
	decode_parameter_init(&dec_hdl);
	dec_data=&dec_hdl.dec_msgbuf.thr_data;
#ifdef PLAYER_TEST
	dec_test[dec_hdl.dec_id]=&dec_hdl;
#endif	
	while(!exit)
	{
		if(-1==(msgrcv(dec_hdl.msg_id,&dec_hdl.dec_msgbuf,sizeof(_thr_msg_data),dec_msg_type[dec_hdl.dec_id],0)))//0表示收不到消息就阻塞
		{
			OSA_ERROR("dec rcv msg fail ,str error: %s",strerror(errno));
		}	
		else
		{			
////////************************************************************/////////////////////////////
			switch(dec_data->cmd)
			{
				case DECODE_CMD_NEW_DATA:
					if(DEC_THR_STA_WORK_ON==dec_hdl.dec_thr_sta)
					{
						if(decode_clr_data(dec_hdl.dec_id))
						{
							u8 buf_id;
							if(player_get_full_buf((buf_handle*)&decode_handle[dec_hdl.dec_id].cur_handle_buf,&buf_id))
							{
								OSA_ERROR("get empty buf fail");								
								dec_hdl.dec_thr_sta=DEC_THR_STA_STANDBY;
							}	
							if(player_put_empty_buf((buf_handle*)&decode_handle[dec_hdl.dec_id].cur_handle_buf,buf_id))
							{
								OSA_ERROR("get empty buf fail");
								dec_hdl.dec_thr_sta=DEC_THR_STA_STANDBY;								
							}
						}
						else
						{
							s8 ret;							
							if(dec_hdl.cur_out_res!=decode_get_out_res(dec_hdl.dec_id))
							{
								dec_hdl.cur_out_res=decode_get_out_res(dec_hdl.dec_id);
							}							
							if((ret=decode_video_data_pro(&dec_hdl)))
							{
								OSA_ERROR("dec%d decode_video_data_pro fail,ret:%d!!!",dec_hdl.dec_id,ret);								
								if(ret==-2)
								{
									decode_lib_abnormal_conf(&dec_hdl,1);
								}
								else
								{
									dec_hdl.dec_thr_sta=DEC_THR_STA_STANDBY;
								}
							}
						}
					}
					break;
				case DECODE_CMD_BLACK:
					if(DEC_THR_STA_WORK_ON==dec_hdl.dec_thr_sta)
					{
						DEBUG_PF("dec %d rcv black cmd!!!",dec_hdl.dec_id);
						if(fluency_put_msg_to_queue(dec_hdl.dec_id,dec_data,FLU_CMD_BLACK,0))
						{
							OSA_ERROR("send msg fail!!!");
						}
					}
					break;
				case DECODE_CMD_STANDBY:
					if((DEC_THR_STA_WORK_ON==dec_hdl.dec_thr_sta)
						||(DEC_THR_STA_STANDBY==dec_hdl.dec_thr_sta))
					{
						DEBUG_PF("dec %d rcv standby cmd at sta:%d!!!",dec_hdl.dec_id,dec_hdl.dec_thr_sta);
						decode_set_clr_cmd(dec_hdl.dec_id,0);
						dec_hdl.dec_thr_sta=DEC_THR_STA_STANDBY;
						dec_hdl.discard_frame_num=0;
						picture_infor[dec_hdl.dec_id].real_frame_rate=25;
						if(fluency_put_msg_to_queue(dec_hdl.dec_id,dec_data,FLU_CMD_STANDBY,0))
						{
							OSA_ERROR("send msg fail!!!");
						}
					}
					break;
				case FLU_CMD_STANDBY_ALREADY:
					if(DEC_THR_STA_STANDBY==dec_hdl.dec_thr_sta)
					{
						DEBUG_PF("dec id:%d rcv flu standby already cmd",dec_hdl.dec_id);	
						if(stream_put_msg_to_queue(dec_hdl.dec_id,dec_data,DECODE_CMD_STANDBY_ALREADY,0))
						{
							OSA_ERROR("send msg fail!!!");
						}
					}
					break;
				case DECODE_CMD_WORK_ON:
					if(DEC_THR_STA_STANDBY==dec_hdl.dec_thr_sta)
					{
						DEBUG_PF("dec id:%d rcv work on cmd",dec_hdl.dec_id); 							
						if(fluency_put_msg_to_queue(dec_hdl.dec_id,dec_data,FLU_CMD_WORK_ON,0))
						{
							OSA_ERROR("send msg fail!!!");
						}						
					}
					break;		
				case FLU_CMD_WORK_ON_ALREADY:
					if(DEC_THR_STA_STANDBY==dec_hdl.dec_thr_sta)
					{
						/*if(dec_hdl.single_en!=dec_data->single_en)
						{
							dec_hdl.cur_out_res=0;
						}
						dec_hdl.single_en=dec_data->single_en;	*/					
						dec_hdl.record_en=dec_data->record;
						dec_hdl.dec_thr_sta=DEC_THR_STA_WORK_ON;	
						DEBUG_PF("dec id:%d rcv flu work on alreadby cmd",dec_hdl.dec_id); 	
						if(stream_put_msg_to_queue(dec_hdl.dec_id,dec_data,DECODE_CMD_WORK_ON_ALREADY,0))
						{
							OSA_ERROR("send msg fail!!!");
						}							
					}
					break;			
				case DECODE_CMD_FRAME_SYNC:
					if((DEC_THR_STA_STANDBY==dec_hdl.dec_thr_sta)||(DEC_THR_STA_WORK_ON==dec_hdl.dec_thr_sta))
					{
						DEBUG_PF("decode :%d rcv frame sync cmd",dec_hdl.dec_id);
						if(fluency_put_msg_to_queue(dec_hdl.dec_id,dec_data,FLU_CMD_FRAME_SYNC,0))
						{
							OSA_ERROR("send msg fail!!!");
						}
					}
					break;
				case DECODE_CMD_WAIT_CMD:
					DEBUG_PF("dec:%d rcv wait cmd at sta:%d",dec_hdl.dec_id,dec_hdl.dec_thr_sta);					
					decode_set_clr_cmd(dec_hdl.dec_id,0);
					if(fluency_put_msg_to_queue(dec_hdl.dec_id,dec_data,FLU_CMD_WAIT_CMD,0))
					{
						OSA_ERROR("send msg fail!!!");
					}
					break;
				case FLU_CMD_WAIT_CMD_ALREADY:
					DEBUG_PF("dec:%d rcv flu wait cmd already at sta:%d",dec_hdl.dec_id,dec_hdl.dec_thr_sta);
					if(stream_put_msg_to_queue(dec_hdl.dec_id,dec_data,DECODE_CMD_WAIT_CMD_ALREADY,0))
					{
						OSA_ERROR("send msg fail!!!");
					}					
					break;
				case DECODE_CMD_EXIT:
					if(DEC_THR_STA_STANDBY==dec_hdl.dec_thr_sta)
					{
						DEBUG_PF("dec %d rcv exit cmd",dec_hdl.dec_id);
						exit=1;
					}
					break;
				default:;
			}
		}		
	}	
	if(picture_infor[dec_hdl.dec_id].decoder!=NULL)
	{
		decode_thr_decoder_exit(&picture_infor[dec_hdl.dec_id].decoder);
	}
	if((status=player_mutex_delete((pthread_mutex_t*)&picture_infor[dec_hdl.dec_id].decode_lock)))
	{
		OSA_ERROR("player_mutex_delete():%d!!!",status);
	}
	if((status=player_cond_delete(&picture_infor[dec_hdl.dec_id].decode_cond)))
	{
		OSA_ERROR("player_cond_delete():%d!!!",status);
	}
	DEBUG_PF("decode %d thr exit",dec_hdl.dec_id);	
	return NULL;
}
int decode_thr_delete(u8 param)
{
	static u8 dec_exit=0;
	int status=0;
	if(!dec_exit)
	{
		dec_exit=1;
		status|=player_mutex_delete(&all_decode_lock);
		status|=player_mutex_delete(&msg_lock);		
		if(status)
		{
			OSA_ERROR("player_mutex_delete:%d!!!",status);
		}
	}
	OSA_thrDelete(&decode_thr[param]);
	return OSA_SOK;
}

static void decode_save_buf_head(pic_infor *handle,buf_head *head)
{
    pthread_mutex_lock(&handle->decode_lock);
	if(handle->buf_head_index<HEAD_NUM)
	{
		memcpy(&handle->cur_head_infor[handle->buf_head_index],head,sizeof(buf_head));
		handle->buf_head_index++;			
	}
	else
	{
		OSA_ERROR("dec save buf head overflow,put num:%d",handle->put_decoder_num);		
	}
	pthread_mutex_unlock(&handle->decode_lock);		
}
static void decode_get_buf_head(pic_infor *handle,buf_head *head)
{
	u8 i;
	pthread_mutex_lock(&handle->decode_lock);
	memcpy(head,&handle->cur_head_infor[0],sizeof(buf_head));
	if(!handle->buf_head_index)
	{
		OSA_ERROR("dec get buf head error%d",handle->buf_head_index);
	}
	for(i=0;i<(handle->buf_head_index-1);i++)
	{
		memcpy(&handle->cur_head_infor[i],&handle->cur_head_infor[i+1],sizeof(buf_head));
	}	
	handle->buf_head_index--;		
	pthread_mutex_unlock(&handle->decode_lock);	
}
static int decode_thr_decoder_init(cedarv_decoder_t**decoder,u32 out_res)
{	
	cedarv_stream_info_t      stream_info;	
	int status=0;
	pthread_mutex_lock(&all_decode_lock);
	*decoder = cedarvDecInit(&status);
	pthread_mutex_unlock(&all_decode_lock);
	if(status<0)
	{
		OSA_ERROR("libcedarv_init fail");
		return -1;
	}
	pthread_mutex_lock(&all_decode_lock);		
	if(360==out_res)
	{
		(*decoder)->ioctrl(*decoder, CEDARV_COMMAND_ENABLE_SCALE_DOWN, 1);
		(*decoder)->ioctrl(*decoder, CEDARV_COMMAND_SET_HORIZON_SCALE_RATIO, 1);
		(*decoder)->ioctrl(*decoder, CEDARV_COMMAND_SET_VERTICAL_SCALE_RATIO, 1);	
	}
	pthread_mutex_unlock(&all_decode_lock);

	memset(&stream_info, 0, sizeof(cedarv_stream_info_t));
	stream_info.format			 = CEDARV_STREAM_FORMAT_H264;//STREAM_FORMAT_H264;
	stream_info.sub_format		 = CEDARV_SUB_FORMAT_UNKNOW;
	stream_info.container_format =CONTAINER_FORMAT_UNKNOW;
	stream_info.video_width 	 = 1280;
	stream_info.video_height	 = 720;
	stream_info.frame_rate		 = 30000;
	stream_info.frame_duration	 =0;
	stream_info.aspect_ratio	 = stream_info.video_width/stream_info.video_height*1000;//?????
	stream_info.init_data_len	 = 0;
	stream_info.init_data		 = 0;
	//stream_info.is_pts_correct	 = 1;

	status=(*decoder)->set_vstream_info(*decoder, &stream_info);		//* this decoder operation do not use hardware, so need not lock the mutex.
	pthread_mutex_lock(&all_decode_lock);
	status= (*decoder)->open(*decoder);
	pthread_mutex_unlock(&all_decode_lock);
	if(status < 0)
	{
		OSA_ERROR("can not open decoder");
		pthread_mutex_lock(&all_decode_lock);
		cedarvDecExit(*decoder);
		pthread_mutex_unlock(&all_decode_lock);
		return -1;
	}
	pthread_mutex_lock(&all_decode_lock);
	(*decoder)->ioctrl(*decoder, CEDARV_COMMAND_PLAY, 0);
	pthread_mutex_unlock(&all_decode_lock);
	return 0;
}
static void decode_thr_decoder_exit(cedarv_decoder_t**decoder)
{
	if(*decoder!=NULL)
	{
		pthread_mutex_lock(&all_decode_lock);
		(*decoder)->ioctrl(*decoder, CEDARV_COMMAND_STOP, 0);
//		pthread_mutex_unlock(&all_decode_lock);

//		pthread_mutex_lock(&all_decode_lock);
		(*decoder)->close(*decoder);
		cedarvDecExit(*decoder);
		*decoder=NULL;
		pthread_mutex_unlock(&all_decode_lock);
	}
}
static void decode_thr_check_put_deocder_num(pic_infor *handle)
{	
	pthread_mutex_lock(&handle->decode_lock);	
	handle->put_decoder_num++;
	if(handle->put_decoder_num>DECODE_MAX_STORAGE_FRAME_NUM)
	{
		pthread_cond_wait(&handle->decode_cond,&handle->decode_lock);	
	}
	pthread_mutex_unlock(&handle->decode_lock);
}
static void decode_thr_releas_decoder_num(pic_infor *handle)
{
	pthread_mutex_lock(&handle->decode_lock);	
	handle->put_decoder_num--; 
	if(handle->put_decoder_num<=2)
	{
		DEBUG_PF("put decoder num :%d error",handle->put_decoder_num);
	}
	pthread_cond_signal(&handle->decode_cond);			
	pthread_mutex_unlock(&handle->decode_lock);
}
void decode_thr_release_dis(u8 dec_id,u32 id)
{
	// 加锁  ?????
	picture_infor[dec_id].decoder->display_release(picture_infor[dec_id].decoder,id);	
	decode_thr_releas_decoder_num(&picture_infor[dec_id]);
}
static u8 decode_thr_decode_ready(u8 dec_id)
{
	return picture_infor[dec_id].decoder->picture_ready(picture_infor[dec_id].decoder);
}
s8 decode_thr_display_request(u8 dec_id,cedarv_picture_t* picture)
{
	//加锁 ?????
	return picture_infor[dec_id].decoder->display_request(picture_infor[dec_id].decoder,picture);
}
static void decode_lib_abnormal_conf(decode_ctl*hdl,u8 cfg)
{
	int cnt=0;
	DEBUG_PF("dec%d enter abnormal conf",hdl->dec_id);
	while(1)
	{
		pthread_mutex_lock(&picture_infor[hdl->dec_id].decode_lock);	
		if((picture_infor[hdl->dec_id].put_decoder_num<=3)||(cnt>=1000))
		{
			pthread_mutex_unlock(&picture_infor[hdl->dec_id].decode_lock);
			break;
		}
		else
		{//copy线程还可以拿到buf,继续等待
			pthread_mutex_unlock(&picture_infor[hdl->dec_id].decode_lock);		
			player_waitMsecs(5);
			cnt++;
		}
	}
	DEBUG_PF("dec%d exit abnormal conf,cnt:%d",hdl->dec_id,cnt);
	if(cfg)
	{
		hdl->discard_frame_num=1;//为了等待I帧
		hdl->old_out_res=0;//重启解码库
	}
}
static s8 decode_discard_frame_judge(decode_ctl*hdl,u8 frame_type,u32 frame_no,u32 *first_frame)
{
	//if(hdl->dec_id==1)
	//DEBUG_PF("discard num:%d,frame_type:%d",hdl->discard_frame_num,frame_type);
	if(!hdl->discard_frame_num)
	{
		return 0;
	}
	else
	{
		hdl->discard_frame_num--;
		if((!hdl->discard_frame_num)||(frame_no>=(hdl->dec_frame_no+hdl->discard_frame_num)))
		{
			if(AV_VIDEO_FRAME_I==frame_type)
			{
				*first_frame=1;
				return 0;
			}
			hdl->discard_frame_num=1;//继续等I 帧
		}
		return -1;
	}
}

static void decode_delay_count(decode_ctl*hdl,u32 timestamp)
{
	
	if((hdl->cur_dec_time-hdl->pre_dec_time)>timestamp)
	{
		hdl->delay_time+=(hdl->cur_dec_time-hdl->pre_dec_time)-timestamp;
		if(hdl->delay_time>=DECODE_BUF_CLR_TIME)
		{//等待已经远远大于两帧之间的时间戳,清buf						
			if(hdl->delay_time>=5000)
			{//如果延迟时间太大，可能有未知错误发生。
				hdl->delay_time=5000;
			}
			hdl->discard_frame_num=hdl->delay_time/picture_infor[hdl->dec_id].real_frame_rate;//计算丢弃的帧数
			DEBUG_PF("decode %d delay time:%d,rate:%d,discard num:%d!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",hdl->dec_id,(int)hdl->delay_time,picture_infor[hdl->dec_id].real_frame_rate,hdl->discard_frame_num);
			hdl->delay_time=0;
		}
	}
	else
	{
		if(hdl->delay_time>=(timestamp-(hdl->cur_dec_time-hdl->pre_dec_time)))
		{//如果等待两之间时间间隔比时间戳小，就可以减掉一些延迟时间
			hdl->delay_time-=timestamp-(hdl->cur_dec_time-hdl->pre_dec_time);
		}
		else
		{
			hdl->delay_time=0;
		}
	}
}
static void decode_rate_count(decode_ctl*hdl,u32 timestamp)
{
	hdl->num++;	
	hdl->all_time+=timestamp;
	if(FRAME_NUM==hdl->num)
	{
		if(hdl->all_time)
		{
			picture_infor[hdl->dec_id].real_frame_rate=(1000.0*FRAME_NUM)/hdl->all_time+0.5;;//(1000*FRAME_NUM)/hdl->all_time;//(1000.0*FRAME_NUM)/all_time+0.5;//四舍五入									
			if(picture_infor[hdl->dec_id].real_frame_rate)
			{
				hdl->average_timestamp=1000/picture_infor[hdl->dec_id].real_frame_rate;
			}
		}
		hdl->num=0;
		hdl->all_time=0;
	}
}
static void decode_check_continue(decode_ctl*hdl,u32 *first_frame)
{
	if((hdl->cur_frame_stamp>=(hdl->pre_frame_stamp+DECODE_IPC_CHANGE))
		||(hdl->cur_frame_stamp<=hdl->pre_frame_stamp)
		||*first_frame)
	{//时间戳差别在500ms以上，可能是由于ipc已经切换造成的		
		*first_frame=1;
		hdl->num=0;
		hdl->all_time=0;
		hdl->delay_time=0;
		hdl->cur_dec_time=hdl->pre_dec_time=0;
	}
}
static s8 decode_res_check(decode_ctl*hdl)
{
	if(hdl->cur_out_res!=hdl->old_out_res)
	{				
		DEBUG_PF("dec:%d out res:%d",hdl->dec_id,hdl->cur_out_res);		
		decode_lib_abnormal_conf(hdl,0);
		decode_thr_decoder_exit(&picture_infor[hdl->dec_id].decoder);
		if(decode_thr_decoder_init(&picture_infor[hdl->dec_id].decoder,hdl->cur_out_res))
		{
			OSA_ERROR("decode_thr_decoder_init resolution fail");
			return -1;
		}
		hdl->old_out_res=hdl->cur_out_res; 		
		hdl->dec_num=0;
		picture_infor[hdl->dec_id].buf_head_index=0;
		picture_infor[hdl->dec_id].real_frame_rate=25;			
		picture_infor[hdl->dec_id].put_decoder_num=0;
	}
	return 0;
}
static s8 decode_lib_buf_request_copy(decode_ctl*hdl,u32 len,u8 *src_buf)
{
	int status;
	u32 bufsize0,bufsize1;
	u8 *buf0,*buf1;
	status= picture_infor[hdl->dec_id].decoder->request_write(picture_infor[hdl->dec_id].decoder,len, &buf0, &bufsize0, &buf1, &bufsize1);
	if(status< 0)
	{
		return -1;
	}
	if(len <= bufsize0) 
	{
		memcpy(buf0,src_buf,len);
		cedarx_cache_op((long)buf0,(long)(buf0+len),1);
	}
	else 
	{				
		memcpy(buf0,src_buf,bufsize0);
		cedarx_cache_op((long)buf0,(long)(buf0+bufsize0),1);
		memcpy(buf1,src_buf+bufsize0,bufsize1);
		cedarx_cache_op((long)buf1,(long)(buf1+bufsize1),1);
	}	
	return 0;
}
static void decode_lib_buf_update(decode_ctl*hdl,u32 len,u64 pts)
{	
	cedarv_stream_data_info_t data_info;
	memset(&data_info, 0, sizeof(cedarv_stream_data_info_t));
	data_info.length=len;
	data_info.pts=pts;
	data_info.flags =CEDARV_FLAG_FIRST_PART | CEDARV_FLAG_LAST_PART | CEDARV_FLAG_PTS_VALID;					
	picture_infor[hdl->dec_id].decoder->update_data(picture_infor[hdl->dec_id].decoder, &data_info);		//* this decoder operation do not use hardware, so need not lock the mutex.
}
static s8 decode_lib_decode(decode_ctl *hdl)
{
	int status;
	pthread_mutex_lock(&all_decode_lock);
	status= picture_infor[hdl->dec_id].decoder->decode(picture_infor[hdl->dec_id].decoder);
	pthread_mutex_unlock(&all_decode_lock);
	hdl->dec_num++;
	if(((hdl->dec_num>=3)&&(1!=status)&&(3!=status))
		||((hdl->dec_num<3)/*&&(status<0)*/&&(0!=status)&&(5!=status)))			
	{
		OSA_ERROR("dec:%d fail num:%llu return:%d",hdl->dec_id,hdl->dec_num,status);									
		decode_thr_releas_decoder_num(&picture_infor[hdl->dec_id]);
		return -2;
	}			
	return 0;
}
static s8 decode_decode_pro(decode_ctl*hdl,u8*buf,u32 len)
{
	s8 ret=0;
	AV_FRAME_HEAD_S *av_head=(AV_FRAME_HEAD_S*)buf;
	if((AV_VIDEO_FRAME_I==av_head->u8FrameType)&&decode_res_check(hdl))
	{
		OSA_ERROR("decoder init fail");
		return -1;
	}
//	DEBUG_PF("num:%llu",hdl->dec_num);
	decode_thr_check_put_deocder_num(&picture_infor[hdl->dec_id]);
	if(decode_lib_buf_request_copy(hdl,len,buf+sizeof(AV_FRAME_HEAD_S)))
	{
		OSA_ERROR("decoder request buffer fail");
		return -1;
	}
//	DEBUG_PF("num:%llu",hdl->dec_num);
	decode_lib_buf_update(hdl,len,av_head->u64TimeStamp);
//	DEBUG_PF("num:%llu",hdl->dec_num);
	if((ret=decode_lib_decode(hdl)))
	{
		OSA_ERROR("decoder:%d lib decode fail",hdl->dec_id);
		return ret;
	}
	return 0;
}
static s8 decode_video_data_pro(decode_ctl*hdl)
{
	u8 buf_id;
	buf_head *put,get;
	AV_FRAME_HEAD_S *frame_head;		
	u32 timestamp;
	s8 ret=0;
	if(player_get_full_buf((buf_handle*)&decode_handle[hdl->dec_id].cur_handle_buf,&buf_id))
	{
		OSA_ERROR("get empty buf fail");
		return -1;
	}	
	frame_head=(AV_FRAME_HEAD_S*)decode_handle[hdl->dec_id].cur_handle_buf.p_dest[buf_id];
	put=(buf_head*)(decode_handle[hdl->dec_id].cur_handle_buf.p_dest[buf_id]+DECODE_BUF_SIZE-sizeof(buf_head));
////丢帧处理
	if(decode_discard_frame_judge(hdl,frame_head->u8FrameType,frame_head->u32FrameNo,&put->first_frame))
	{//丢帧
		if(player_put_empty_buf((buf_handle*)&decode_handle[hdl->dec_id].cur_handle_buf,buf_id))
		{
			OSA_ERROR("get empty buf fail");
		}
		return 0;
	}
	hdl->cur_dec_time=OSA_getRunTimeInMsec();
	hdl->cur_frame_stamp=frame_head->u64TimeStamp;	
	decode_check_continue(hdl,&put->first_frame);
	timestamp=put->first_frame?40:(hdl->cur_frame_stamp-hdl->pre_frame_stamp)/1000;
	decode_rate_count(hdl,timestamp);
	if(hdl->record_en==GET_STREAM_FROME_NET)
	{
		decode_delay_count(hdl,timestamp);	
	}
	hdl->dec_frame_no=frame_head->u32FrameNo;
	hdl->pre_dec_time=OSA_getRunTimeInMsec();
	hdl->pre_frame_stamp=hdl->cur_frame_stamp;
///////////解码处理
	if((ret=decode_decode_pro(hdl,decode_handle[hdl->dec_id].cur_handle_buf.p_dest[buf_id],put->frame_len)))
	{		
		if(player_put_empty_buf((buf_handle*)&decode_handle[hdl->dec_id].cur_handle_buf,buf_id))
		{
			OSA_ERROR("get empty buf fail");
		}
		OSA_ERROR("dec:%d decode_decode_pro fail",hdl->dec_id);
		return ret;
	}
	////////buf头信息存取	
	put->time_stamp=timestamp;
	put->average_timestamp=hdl->average_timestamp;
	decode_save_buf_head(&picture_infor[hdl->dec_id],put);
	decode_get_buf_head(&picture_infor[hdl->dec_id],&get);
	if(player_put_empty_buf((buf_handle*)&decode_handle[hdl->dec_id].cur_handle_buf,buf_id))
	{
		OSA_ERROR("decode put empty buf fail!!!");
		return -1;
	}		
	if(decode_thr_decode_ready(hdl->dec_id))
	{
		memcpy(&hdl->dec_msgbuf.thr_data.head_infor,&get,sizeof(buf_head));
		if(fluency_put_msg_to_queue(hdl->dec_id,&hdl->dec_msgbuf.thr_data,FLU_CMD_NEW_DATA,0))
		{
			OSA_ERROR("send msg fail!!!");
		}
	}
	return 0;
}
static void decode_parameter_init(decode_ctl *hdl)
{
	picture_infor[hdl->dec_id].decoder=NULL;
	picture_infor[hdl->dec_id].real_frame_rate=25;
	picture_infor[hdl->dec_id].put_decoder_num=0;
	hdl->discard_frame_num=0;
	hdl->dec_thr_sta=DEC_THR_STA_STANDBY;
	hdl->cur_dec_time=hdl->pre_dec_time=0;
	hdl->cur_frame_stamp=hdl->pre_frame_stamp=0;
	hdl->delay_time=0;
	hdl->num=hdl->all_time=0;
	hdl->cur_out_res=360;
	hdl->old_out_res=0;
	hdl->dec_num=0;
	//hdl->single_en=0;
	hdl->record_en=GET_STREAM_FROME_NET;
	hdl->play_id=0;
}
void decode_set_clr_cmd(u8 ch,u8 flag)
{	
	g_atomic_int_set(&dec_cmd[ch],(gint)flag);
}
static u8 decode_clr_data(u8 ch)
{
	u8 cmd=0;
	cmd=(u8)g_atomic_int_get(&dec_cmd[ch]);
	return cmd;
}
void decode_set_out_res(u8 ch,int res)
{	
	g_atomic_int_set(&dec_res[ch],(gint)res);
}
static u32 decode_get_out_res(u8 ch)
{
	return (u32)g_atomic_int_get(&dec_res[ch]);
}
s8 decode_put_msg_to_queue(u8 ch,_thr_msg_data *data,u8 cmd,int flag)
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
	send_buf.msg_type=dec_msg_type[ch];
	memcpy(&send_buf.thr_data,data,sizeof(_thr_msg_data));
	send_buf.thr_data.cmd=cmd;
	if(msgsnd(msg_id,&send_buf,sizeof(_thr_msg_data),flag))
	{
		OSA_ERROR("dec%d send cmd:%d fail!!!",ch,cmd);								
		pthread_mutex_unlock(&msg_lock);	
		return -1;
	}
	pthread_mutex_unlock(&msg_lock);	
	return 0;
}

