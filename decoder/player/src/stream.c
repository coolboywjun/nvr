#include<player.h>
#include<osa_thr.h>
#include<sys/msg.h>
#include <unistd.h>
#include <semaphore.h>
#include<sys/prctl.h>
#define QUERY_BUF_NUM (2)
#define BLACK_TIME (5)
typedef struct _RTSP_STREAM_S_
{
	long lReadId;
} RTSP_STREAM_S;
typedef struct _str_stream_ctl_
{
	s64 frame_no_back;
	long net_hdl;	
	u8 str_thr_sta;
	u8 temp_sta;
	u8 str_id;
	u8 set_rate;
	u8 play_normal;
/////////回放相关
	StorageFrameHdr replay_tm;
	long disk_hdl;
	StorageReadCmd read_cmd;	
	file_info replay_infor[QUERY_BUF_NUM];
	u8 query_num;	// replay_infor中空闲数量
	u32 play_id;
////////消息队列
	int msg_id;
	int msg_flg;
	_thr_msg str_msgbuf;
//重连计数器
	u32 repeat_cnt;
	u8 cnt;
}stream_ctl;
typedef struct
{
	pthread_mutex_t audio_lock;
	char *audio_buf;
	s8 audio_ch;
}_audio_ctl;
enum{
	STR_THR_STA_WORK_ON=0,
	STR_THR_STA_CONNECT_REPEAT,		
	STR_THR_STA_STANDBY,
	STR_THR_STA_PAUSE,
	STR_THR_STA_CHOOSE_FILE,
	STR_THR_STA_EXIT,
};
#ifdef PLAYER_TEST
static stream_ctl* str_test[STREAM_CHANNEL_MAX];
#endif
static pthread_mutex_t msg_lock;
static _audio_ctl audio_ctl={.audio_buf=NULL,.audio_ch=-1,.audio_lock=PTHREAD_MUTEX_INITIALIZER};
static OSA_ThrHndl stream_thr[STREAM_CHANNEL_MAX];
volatile str_handle stream_handle[STREAM_CHANNEL_MAX];
const u8 str_msg_type[STREAM_CHANNEL_MAX]={STR1_MSG_TYPE_RCV,STR2_MSG_TYPE_RCV,STR3_MSG_TYPE_RCV,STR4_MSG_TYPE_RCV};
static void * stream_thr_main(void*param);
static int stream_init_preview(void);
static void stream_uninit_preview(void);
static int stream_start_preview(int nCh,int nStream,long*lPriData);
static int stream_get_video(int nCh, int nStream,
                       char **lpFrameBuf,
                       int buflen,
                       unsigned long *pU32Pts,
                       unsigned long *pU32PlayLoad,
                       unsigned long *pU32FrameType,
                       long lPriData);
static int stream_iframe_request(int nCh, int nStream, long lPriData);
static int stream_stop_preview(int nCh, int nStream, long *lPriData);
static s8 stream_pro_disk_avdata(stream_ctl *hdl);
static s8 stream_find_frame_pos(long handle,u8 cur_cmd,u8 back_cmd,s8 *reference_frame);
static void stream_save_replay_file(stream_ctl *hdl,file_info*infor,u8 play_now);
static void stream_replay_file_end(stream_ctl *hdl);
//将query[1]设置到query[0]
static s8 stream_replay_file_ring(stream_ctl *hdl,u8 play_now);
//播放query[0]
static void stream_sta_change(stream_ctl *hdl,u8 sta);
static s8 stream_replay_file_play(stream_ctl *hdl,u8 update);
static s8 stream_video_continue(stream_ctl *hdl,u8 cur_cmd);
static s8 stream_pro_net_avdata(stream_ctl *hdl);
static void stream_black_screen_pro(stream_ctl *hdl,u8 cmd);
static s8 stream_repeat_connect_count(stream_ctl *hdl);
static void stream_thr_stop_replay_video(stream_ctl *hdl);
static void stream_parameter_init(stream_ctl *hdl);
static void stream_audio_pro(stream_ctl*hdl,AV_FRAME_HEAD_S *buf,int frame_len);
static void stream_replay_discard_check(stream_ctl *hdl);
static void stream_set_all_clr_cmd(u8 ch);
int stream_thr_create(u8* param)
{
	int status;
	static int init=1;
	if(init)
	{
		init=0;
		audio_ctl.audio_buf=NULL;
		audio_ctl.audio_ch=-1;
		if(NULL ==(audio_ctl.audio_buf= (char*)malloc(sizeof(AV_AUDIO_INFO_S)+DATA_SIZE)))
		{
			OSA_ERROR("malloc audio_ctl.audio_buf mem failed");
			return -1;
		}
		status=player_mutex_create(&msg_lock);
		if(status)
		{
			OSA_ERROR("player_mutex_create():%d!!!",status);
			return status;
		}
		if((status=stream_init_preview()))
		{
			OSA_ERROR("stream_init_preview()");			
			player_mutex_delete(&msg_lock);
			return status;
		}
		if(storage_lib_init()<0)
		{
			stream_uninit_preview();
			player_mutex_delete(&msg_lock);		
			DEBUG_PF("Failed to initialize storage lib");
			return -1;
		}
	}
	//status = OSA_thrCreate(&stream_thr[*param], stream_thr_main, STREAM_THR_PRI, 0, param);
	status=pthread_create(&stream_thr[*param].hndl,NULL,stream_thr_main,(void*)param);	
	if(status!=OSA_SOK) 
	{
		OSA_ERROR("OSA_tskCreate()");
	}
	return status;
}
static void * stream_thr_main(void*param)
{
	u8 get_stream_mode=GET_STREAM_NULL;
	u8 start_record=0,exit=0;		
	char pr_name[10];
	key_t keyval;	
	stream_ctl str_hdl;	
	_thr_msg_data *str_data;
	keyval=ftok(".",PLAYER_THR_IPC_KEY_ID);//获取一个消息队列的键值
	str_hdl.msg_id=msgget(keyval,IPC_CREAT|0666);//创建一个消息队列,并且设置读写权限。如果消息队列已经存在则打开
	str_hdl.msg_flg=0;
	str_hdl.str_id=*((u8*)param);
	str_data=&str_hdl.str_msgbuf.thr_data;
	stream_parameter_init(&str_hdl);
	sprintf(pr_name,"str%d",str_hdl.str_id);	
	prctl(PR_SET_NAME, pr_name);
	DEBUG_PF("str id:%d run",str_hdl.str_id);
#ifdef PLAYER_TEST
	str_test[str_hdl.str_id]=&str_hdl;
#endif
	while(!exit)
	{
		if(-1==(msgrcv(str_hdl.msg_id,&str_hdl.str_msgbuf,sizeof(str_hdl.str_msgbuf.thr_data),str_msg_type[str_hdl.str_id],str_hdl.msg_flg)))
		{//没有收到消息
			switch(str_hdl.str_thr_sta)
			{
				case STR_THR_STA_WORK_ON:						
					//if(player_get_empty_buf_num(stream_handle[str_hdl.str_id].next_handle_buf))
					//{//有空buf
					if(GET_STREAM_FROME_DISK==get_stream_mode)
					{//从硬盘取流		
						if(stream_pro_disk_avdata(&str_hdl))
						{				
							OSA_ERROR("str:%d pro disk video data fail",str_hdl.str_id);
						}
					}
					else
					{//从网络取流
						if(stream_pro_net_avdata(&str_hdl))
						{
							OSA_ERROR("str:%d pro net video data fail",str_hdl.str_id);
						}
					}
					//}
					//else
					//{//没有空buf
					//	player_waitMsecs(5);
					//}
					break;
				case STR_THR_STA_CONNECT_REPEAT:					
					player_waitMsecs(10);							
					if(stream_repeat_connect_count(&str_hdl))
					{
						break;
					}
					if(GET_STREAM_FROME_DISK==get_stream_mode)
					{
						stream_black_screen_pro(&str_hdl,0);
					}
					else if(GET_STREAM_FROME_NET==get_stream_mode)
					{
						if(!stream_start_preview(str_hdl.str_id,0,&str_hdl.net_hdl))
						{
							stream_iframe_request(str_hdl.str_id,0,str_hdl.net_hdl);								
							stream_sta_change(&str_hdl,STR_THR_STA_WORK_ON);
							DEBUG_PF("str%d connect success!!",str_hdl.str_id);
						}						
						else
						{
							stream_black_screen_pro(&str_hdl,0);
						}
					}
					break;
				default:;
			}
		}
		else
		{//收到正确的消息
//////////////////////////////////////////////***********************new//////////////////////////////////////////
			switch(str_data->cmd)
			{
				case STREAM_CMD_STANDBY:
					if((STR_THR_STA_CONNECT_REPEAT==str_hdl.str_thr_sta)
						||(STR_THR_STA_WORK_ON==str_hdl.str_thr_sta)
						||(STR_THR_STA_PAUSE==str_hdl.str_thr_sta))
					{
						DEBUG_PF("stream :%d rcv standby cmd at sta:%d",str_hdl.str_id,str_hdl.str_thr_sta);
						if(GET_STREAM_FROME_DISK==get_stream_mode)
						{
							stream_thr_stop_replay_video(&str_hdl);					
						}
						else if(GET_STREAM_FROME_NET==get_stream_mode)
						{
							stream_stop_preview(str_hdl.str_id,0,&str_hdl.net_hdl);
						}
						stream_black_screen_pro(&str_hdl,str_data->black_screen);
					}
					stream_sta_change(&str_hdl,STR_THR_STA_STANDBY);					
					stream_set_all_clr_cmd(str_hdl.str_id);
					if(decode_put_msg_to_queue(str_hdl.str_id,str_data,DECODE_CMD_STANDBY,0))
					{
						OSA_ERROR("str:%d send msg fail",str_hdl.str_id);
					}
					break;
				case STREAM_CMD_PAUSE:
					if((STR_THR_STA_CONNECT_REPEAT==str_hdl.str_thr_sta)
						||(STR_THR_STA_WORK_ON==str_hdl.str_thr_sta)
						||(STR_THR_STA_PAUSE==str_hdl.str_thr_sta))						
					{
						DEBUG_PF("str %d rcv pause cmd at sta:%d",str_hdl.str_id,str_hdl.str_thr_sta);				
						if(!str_data->pause)
						{
							if(!stream_video_continue(&str_hdl,str_hdl.read_cmd))
							{
								stream_sta_change(&str_hdl,STR_THR_STA_WORK_ON);
							}	
							else
							{
								stream_sta_change(&str_hdl,STR_THR_STA_CONNECT_REPEAT);//是否到standby
							}
						}
						else
						{
							stream_sta_change(&str_hdl,STR_THR_STA_PAUSE);
						}						
						if(player_put_msg_to_queue(str_data,STREAM_CMD_PAUSE_ALREADY,0))
						{
							OSA_ERROR("str:%d send msg fail",str_hdl.str_id);
						}
					}
					break;
				case STREAM_CMD_QUERY_PLAY_TIME:	
					str_data->run_status.query_num=str_hdl.query_num;
					str_data->run_status.replay_end=QUERY_BUF_NUM==str_hdl.query_num?1:0;
					str_data->run_status.play_time_point=str_hdl.replay_tm.ts.tv_sec;
//					DEBUG_PF("str%d,replay time:%d",str_hdl.str_id,str_hdl.replay_tm.ts.tv_sec);
					if(player_put_msg_to_queue(str_data,STREAM_CMD_QUERY_PLAY_TIME_ALREADY,0))
					{
						OSA_ERROR("str:%d send msg fail",str_hdl.str_id);
					}
					break;
				case STREAM_CMD_SET_RATE:
					DEBUG_PF("str %d rcv set rate cmd %d at sta:%d",str_hdl.str_id,str_data->rate,str_hdl.str_thr_sta);		
					StorageReadCmd cmd;
					str_hdl.set_rate=str_data->rate;
					if((str_data->rate>=FLU_WORK_B2_RATE)&&(str_data->rate<=FLU_WORK_B8_RATE))
					{
						cmd=READ_BACK_1_IFRAME+str_data->rate-FLU_WORK_B2_RATE;
					}
					else if((str_data->rate==FLU_WORK_NOMAL_RATE)
								||((str_data->rate>=FLU_WORK_F2_RATE)&&(str_data->rate<=FLU_WORK_F8_RATE))
								|| (str_data->rate>=FLU_WORK_S2_RATE&&str_data->rate<=FLU_WORK_S8_RATE))
					{
						cmd=READ_NORMAL;
					}
					else if(str_data->rate>=FLU_WORK_F2_IFRAME_RATE&&str_data->rate<=FLU_WORK_F8_IFRAME_RATE)
					{
						cmd=READ_FORWARD_1_IFRAME;
					}
					else
					{
						cmd=str_hdl.read_cmd;
					}
					if(str_hdl.read_cmd!=cmd)
					{
						if(stream_video_continue(&str_hdl,cmd))
						{								
							if(str_hdl.str_thr_sta==STR_THR_STA_WORK_ON)
							{
								stream_sta_change(&str_hdl,STR_THR_STA_CONNECT_REPEAT);
							}
						}
						str_hdl.read_cmd=cmd;
					}
					fluency_set_rate(str_hdl.str_id,str_data->rate);
					if(player_put_msg_to_queue(str_data,STREAM_CMD_SET_RATE_ALREADY,0))
					{
						OSA_ERROR("str:%d send msg fail",str_hdl.str_id);
					}
					break;
				case STREAM_CMD_CHOOSE_FILE:
					DEBUG_PF("str %d rcv choose file cmd at sta:%d",str_hdl.str_id,str_hdl.str_thr_sta);											
					stream_save_replay_file(&str_hdl,&str_data->replay_infor,str_data->update_now);
					stream_replay_discard_check(&str_hdl);
					if(!stream_replay_file_ring(&str_hdl,str_data->update_now))
					{
						if(stream_replay_file_play(&str_hdl,str_data->update_now))
						{					
							OSA_ERROR("stream_replay_file_play fail");
							stream_replay_file_end(&str_hdl);
							start_record=0;
							if(STR_THR_STA_WORK_ON==str_hdl.str_thr_sta)
							{//进入重连状态，再次设置播放文件。防止播放状态下，更新文件，但是函数返回失败
								stream_sta_change(&str_hdl,STR_THR_STA_CONNECT_REPEAT);
							}
						}
						else
						{
							start_record=1;
							if(STR_THR_STA_CONNECT_REPEAT==str_hdl.str_thr_sta)
							{//进入重连状态，再次设置播放文件。
								stream_sta_change(&str_hdl,STR_THR_STA_WORK_ON);
							}
						}								
					}	
					if(player_put_msg_to_queue(str_data,STREAM_CMD_CHOOSE_FILE_ALREADY,0))
					{
						OSA_ERROR("str:%d send msg fail",str_hdl.str_id);
					}
					break;
				case STREAM_CMD_SINGLE_FRAME:
					if(STR_THR_STA_PAUSE==str_hdl.str_thr_sta)
					{
						DEBUG_PF("str %d rcv single %d frame cmd at sta:%d",str_hdl.str_id,str_data->single_dir,str_hdl.str_thr_sta);	
						if(!stream_video_continue(&str_hdl,DOWN_FRAME==str_data->single_dir?READ_FORWARD_VIDEO_FRAME:READ_BACK_1_IFRAME))
						{
							str_hdl.read_cmd=str_data->single_dir==DOWN_FRAME?READ_FORWARD_VIDEO_FRAME:READ_BACK_1_IFRAME;														
							if(stream_pro_disk_avdata(&str_hdl))
							{				
								OSA_ERROR("str:%d pro disk video data fail",str_hdl.str_id);							
								stream_sta_change(&str_hdl,STR_THR_STA_PAUSE);//不能到repeat状态,因为会黑屏
							}
						}
						else
						{
							stream_sta_change(&str_hdl,STR_THR_STA_PAUSE);//不能到repeat状态，因为会黑屏
						}
					}
					break;		
				case DECODE_CMD_WORK_ON_ALREADY:// ?????
					if(STR_THR_STA_STANDBY==str_hdl.str_thr_sta)
					{		
						DEBUG_PF("stream :%d rcv dec work on alreadby cmd",str_hdl.str_id);
						get_stream_mode=str_data->record;	
						if(get_stream_mode==GET_STREAM_FROME_NET)
						{
							str_hdl.read_cmd=READ_NORMAL;
						}
						if(GET_STREAM_FROME_DISK==get_stream_mode)
						{//从硬盘读取帧
							if(start_record)
							{							
								stream_sta_change(&str_hdl,STR_THR_STA_WORK_ON);
							}
							else
							{						
								stream_sta_change(&str_hdl,STR_THR_STA_CONNECT_REPEAT);							
							}
						}
						else if((GET_STREAM_FROME_NET==get_stream_mode)||(GET_STREAM_NULL==get_stream_mode))
						{//从网路上取流	
							get_stream_mode=GET_STREAM_FROME_NET;
							if(!stream_start_preview(str_hdl.str_id,0,&str_hdl.net_hdl))
							{//链接上，开始工作 									
								stream_iframe_request(str_hdl.str_id,0,str_hdl.net_hdl);
								stream_sta_change(&str_hdl,STR_THR_STA_WORK_ON);															
							}		
							else
							{//没有链接上，继续重连						
								stream_sta_change(&str_hdl,STR_THR_STA_CONNECT_REPEAT);
								stream_black_screen_pro(&str_hdl,1);
							}																	
						}				
						if(player_put_msg_to_queue(str_data,STREAM_CMD_WORK_ALREADY,0))
						{
							OSA_ERROR("str:%d send msg fail",str_hdl.str_id);
						}
					}
					break;
				case STREAM_CMD_CONNECT:
					if(STR_THR_STA_STANDBY==str_hdl.str_thr_sta)
					{
						DEBUG_PF("str %d rcv connect cmd",str_hdl.str_id);
						if(decode_put_msg_to_queue(str_hdl.str_id,str_data,DECODE_CMD_WORK_ON,0))
						{
							OSA_ERROR("str:%d send msg fail",str_hdl.str_id);
						}
					}
					break;
				case DECODE_CMD_STANDBY_ALREADY:
					if(STR_THR_STA_STANDBY==str_hdl.str_thr_sta)
					{
						DEBUG_PF("str %d rcv decode standby already cmd",str_hdl.str_id);
						if(player_put_msg_to_queue(str_data,STREAM_CMD_STANDBY_ALREADY,0))
						{
							OSA_ERROR("str:%d send msg fail",str_hdl.str_id);
						}						
					}
					break;
				case DECODE_CMD_WAIT_CMD_ALREADY:
					DEBUG_PF("str %d rcv dec wait cmd already at sta:%d",str_hdl.str_id,str_hdl.str_thr_sta);
					break;
				case STREAM_CMD_SET_AUDIO:
					DEBUG_PF("str %d rcv set audio cmd",str_hdl.str_id);
					if(str_data->audio_ctl)
					{
						audio_ctl.audio_ch=str_hdl.str_id;		
						audio_play(AUDIO_TYPE_CHANNEL);
						audio_put_data_init();
					}
					else
					{
						audio_stop(AUDIO_TYPE_CHANNEL);
						audio_ctl.audio_ch=-1;
					}
					break;		
				case STREAM_CMD_EXIT:
					if(STR_THR_STA_STANDBY==str_hdl.str_thr_sta)
					{
						DEBUG_PF("str %d rcv exit cmd",str_hdl.str_id);
						exit=1;
					}
					break;
				default:;
			}
//////////////////////////////////////////////***********************new//////////////////////////////////////////		
		}
	}
	stream_uninit_preview();
	if(str_hdl.str_id==audio_ctl.audio_ch)
	{	
		audio_stop(AUDIO_TYPE_CHANNEL);
	}
	DEBUG_PF("stream thr %d exit",str_hdl.str_id);	
	return NULL;
}
int stream_thr_delete(u8 param)
{
	static u8 str_exit=0;
	int status=0;
	if(!str_exit)
	{
		str_exit=1;		
		storage_lib_uninit();			
		if(audio_ctl.audio_buf!=NULL)
		{
			free(audio_ctl.audio_buf);
			audio_ctl.audio_buf=NULL;
		}	
		status=player_mutex_delete(&msg_lock);
		if(status)
		{
			OSA_ERROR("player_mutex_delete:%d!!!",status);
		}
	}
	OSA_thrDelete(&stream_thr[param]);
	return OSA_SOK;
}
static s8 stream_repeat_connect_count(stream_ctl *hdl)
{
	hdl->cnt++;
	if(hdl->cnt<100)
	{						
		return -1;
	}
	hdl->cnt=0;
	return 0;
}
static void stream_sta_change(stream_ctl *hdl,u8 sta)
{
	switch(sta)
	{
		case STR_THR_STA_WORK_ON:
			hdl->repeat_cnt=0;			
			hdl->cnt=0;		
			hdl->msg_flg=IPC_NOWAIT;//收不到消息不阻塞。继续取流
			hdl->str_thr_sta=STR_THR_STA_WORK_ON;
			break;
		case STR_THR_STA_STANDBY:			
			hdl->replay_infor[0].query.chn=-1;
			hdl->replay_infor[1].query.chn=-1;
			hdl->query_num=QUERY_BUF_NUM;			
			hdl->msg_flg=0;//收不到消息阻塞。继续取流
			hdl->str_thr_sta=STR_THR_STA_STANDBY;			
			break;
		case STR_THR_STA_CONNECT_REPEAT:
			hdl->repeat_cnt=0;			
			hdl->cnt=0;			
			hdl->msg_flg=IPC_NOWAIT;//收不到消息不阻塞。继续取流
			hdl->str_thr_sta=STR_THR_STA_CONNECT_REPEAT;
			break;
		case STR_THR_STA_PAUSE:
			hdl->msg_flg=0;//收不到消息阻塞。继续取流
			hdl->str_thr_sta=STR_THR_STA_PAUSE;
			break;
		case STR_THR_STA_CHOOSE_FILE:			
			hdl->msg_flg=0;
			hdl->str_thr_sta=STR_THR_STA_CHOOSE_FILE;//从某个时间点开始播放时，才用到这个状态
			break;
	}
}
static void stream_uninit_preview(void)
{
	av_pool_uninit();
}
static int stream_init_preview(void)
{
	if(av_pool_init() < 0)
	{
		DEBUG_PF("av pool init failed.");
		return -1;
	}	
	return 0;
}
static int stream_start_preview(int nCh,int nStream,long*lPriData)
{
	RTSP_STREAM_S *pRtspStream;
	long lReadId;

	if(av_pool_is_writing(nCh, nStream) <= 0)
	{
		// 此pool没有数据写入,直接返回错误
		//DEBUG_PF("There is no data in the pool ch%d stream%d", nCh, nStream);
		return -1;
	}
	pRtspStream = (RTSP_STREAM_S *)malloc(sizeof(RTSP_STREAM_S));
	if(pRtspStream == NULL)
		return -1;
	if(av_pool_start_read_frame(nCh, nStream, 1000, &lReadId) < 0)
	{
		DEBUG_PF("Start read av pool failed.");
		free(pRtspStream);
		return -1;
	}
	pRtspStream->lReadId = lReadId;
	*lPriData = (long)pRtspStream;
	return 0;
}
static int stream_stop_preview(int nCh, int nStream, long *lPriData)
{
	RTSP_STREAM_S *pRtspStream = (RTSP_STREAM_S *)(*lPriData);
	if(pRtspStream == NULL)
		return -1;
	av_pool_stop_read_frame(pRtspStream->lReadId);
	free(pRtspStream);
	*lPriData = 0;
	return 0;
}
static int stream_get_video(int nCh, int nStream,
                       char **lpFrameBuf,
                       int buflen,
                       unsigned long *pU32Pts,
                       unsigned long *pU32PlayLoad,
                       unsigned long *pU32FrameType,
                       long lPriData)
{
	RTSP_STREAM_S *pRtspStream = (RTSP_STREAM_S *)lPriData;
	char *pFrameBuf =  NULL;
	int nBufLen = 0;
	int nFrameLen;
	if(pRtspStream == NULL)
		return -1;
	//pFrameBuf = *lpFrameBuf;//*lpFrameBuf = NULL;
	//nBufLen=buflen;
	nFrameLen = av_pool_read_frame(pRtspStream->lReadId, &pFrameBuf, &nBufLen, 100);
	if(nFrameLen > 0 && nBufLen > 0 && pFrameBuf != NULL)
	{
		*lpFrameBuf=pFrameBuf;
		return nFrameLen;
	}
	return -1;
}
static int stream_iframe_request(int nCh, int nStream, long lPriData)
{
	RTSP_STREAM_S *pRtspStream = (RTSP_STREAM_S *)lPriData;
	if(pRtspStream == NULL)
		return -1;
	return av_pool_request_iframe(nCh, nStream);
}
//////////// TBD
/// 取流可以改进，增加效率
static s8 stream_pro_net_avdata(stream_ctl *hdl)
{	
	buf_head *buf_info;	
	int frame_len;
	u8 *frame_buff=NULL,buf_id,*data_buf;	
	buf_handle *dec_handle_buf=stream_handle[hdl->str_id].next_handle_buf;	
	if((frame_len=stream_get_video(hdl->str_id,0,(char**)&frame_buff,0,NULL,NULL,NULL,hdl->net_hdl))>0)
	{//取到视频流
		if(AV_AUDIO_FRAME_A!=((AV_FRAME_HEAD_S*)frame_buff)->u8FrameType)
		{
			if(720==((AV_FRAME_HEAD_S*)frame_buff)->unVidoeInfo.u16ImageHeight)
			{
				if(hdl->frame_no_back+1!=((AV_FRAME_HEAD_S*)frame_buff)->u32FrameNo)
				{
					if(AV_VIDEO_FRAME_I!=((AV_FRAME_HEAD_S*)frame_buff)->u8FrameType)
					{				
						DEBUG_PF("str%d frame no cut,pre no:%lld,cur no:%ld",hdl->str_id,hdl->frame_no_back,((AV_FRAME_HEAD_S*)frame_buff)->u32FrameNo);
						stream_iframe_request(hdl->str_id,0,hdl->net_hdl);
						return -1;
					}
				}					
				if(player_get_empty_buf(dec_handle_buf,&buf_id))
				{
					OSA_ERROR("stream get empty buf fail!!!");				
					stream_sta_change(hdl,STR_THR_STA_STANDBY);
					return -1;
				}	
				data_buf=dec_handle_buf->p_dest[buf_id];
				memcpy(data_buf,frame_buff,frame_len);
				buf_info=(buf_head*)(dec_handle_buf->p_dest[buf_id]+DECODE_BUF_SIZE-sizeof(buf_head));
				buf_info->frame_len=frame_len-sizeof(AV_FRAME_HEAD_S);
				buf_info->first_frame=-1==hdl->frame_no_back?1:0;
				buf_info->play_id=hdl->play_id;
				if(hdl->frame_no_back+1!=((AV_FRAME_HEAD_S*)frame_buff)->u32FrameNo)
				{
					buf_info->first_frame=1;
				}
				hdl->frame_no_back=((AV_FRAME_HEAD_S*)frame_buff)->u32FrameNo;
				if(player_put_full_buf(dec_handle_buf,buf_id))
				{
					OSA_ERROR("str%d put full buf fail!!!",hdl->str_id);				
					stream_sta_change(hdl,STR_THR_STA_STANDBY);
					return -1;
				}	
				hdl->str_msgbuf.thr_data.play_id=hdl->play_id;
				hdl->str_msgbuf.thr_data.pts=((AV_FRAME_HEAD_S*)frame_buff)->u64TimeStamp;
				if(decode_put_msg_to_queue(hdl->str_id,&hdl->str_msgbuf.thr_data,DECODE_CMD_NEW_DATA,0))
				{
					OSA_ERROR("str:%d send msg fail",hdl->str_id);				
					stream_sta_change(hdl,STR_THR_STA_STANDBY);
					return -1;
				}	
			}
		}
		else
		{//取到音频流	
			stream_audio_pro(hdl,(AV_FRAME_HEAD_S*)frame_buff,frame_len);
		}
	}
	else
	{
		if(!stream_preview_status(hdl->str_id))
		{//没有取到流	
			DEBUG_PF("stream %d get stream error,change to repeat status !!!",hdl->str_id);
			stream_stop_preview(hdl->str_id,0,&hdl->net_hdl);		
			stream_sta_change(hdl,STR_THR_STA_CONNECT_REPEAT);
			return -1;
		}
	}	
	return 0;
}
static void stream_thr_stop_replay_video(stream_ctl *hdl)
{
	if(hdl->disk_hdl)
	{
		storage_lib_read_stop(hdl->disk_hdl);
		hdl->disk_hdl=0;
	}
	DEBUG_PF("str:%d read storage stop",hdl->str_id);
}
static s8 stream_pro_disk_avdata(stream_ctl *hdl)
{	
	buf_head *buf_info;	
	buf_handle *dec_handle_buf=stream_handle[hdl->str_id].next_handle_buf;
	int frame_len;
	u8 *frame_buff,buf_id,*data_buf;				
	if(player_get_empty_buf_nowait(dec_handle_buf,&buf_id))
	{
		player_waitMsecs(10);
		return 0;
	}
	frame_len = storage_lib_read_frame(hdl->disk_hdl,hdl->read_cmd,&hdl->replay_tm,&frame_buff);
	if((frame_len> sizeof(AV_FRAME_HEAD_S))&&(frame_len>0))//第一帧一定是I帧
	{//从硬盘取流正确
		if((AV_AUDIO_FRAME_A!=((AV_FRAME_HEAD_S*)frame_buff)->u8FrameType)
			&&(720==((AV_FRAME_HEAD_S*)frame_buff)->unVidoeInfo.u16ImageHeight))
		{	
			if(hdl->frame_no_back+1!=((AV_FRAME_HEAD_S*)frame_buff)->u32FrameNo)
			{
				if(AV_VIDEO_FRAME_I!=((AV_FRAME_HEAD_S*)frame_buff)->u8FrameType)
				{
					OSA_ERROR("str%d frame no cut,pre no:%lld,cur no:%ld",hdl->str_id,hdl->frame_no_back,((AV_FRAME_HEAD_S*)frame_buff)->u32FrameNo);
					player_put_empty_buf(dec_handle_buf, buf_id);
					return -1;
				}	
			}
			((AV_FRAME_HEAD_S*)frame_buff)->u64TimeStamp=((u64)hdl->replay_tm.ts.tv_sec)*1000000+(u64)hdl->replay_tm.ts.tv_usec;
			data_buf=dec_handle_buf->p_dest[buf_id];
			memcpy(data_buf,frame_buff,frame_len);
			buf_info=(buf_head*)(data_buf+DECODE_BUF_SIZE-sizeof(buf_head));
			buf_info->frame_len=frame_len-sizeof(AV_FRAME_HEAD_S);
			buf_info->first_frame=READ_NORMAL!=hdl->read_cmd?1:0;//如果不是正常取帧，时间戳是没有意义的
			buf_info->play_id=hdl->play_id;
			if(hdl->frame_no_back+1!=((AV_FRAME_HEAD_S*)frame_buff)->u32FrameNo)
			{
				buf_info->first_frame=1;
			}
			hdl->frame_no_back=((AV_FRAME_HEAD_S*)data_buf)->u32FrameNo;		
			if(player_put_full_buf(dec_handle_buf,buf_id))
			{
				OSA_ERROR("str%d put full buf fail!!!",hdl->str_id);				
				stream_sta_change(hdl,STR_THR_STA_STANDBY);
				return -1;
			}	
			hdl->str_msgbuf.thr_data.play_id=hdl->play_id;
			hdl->str_msgbuf.thr_data.pts=((AV_FRAME_HEAD_S*)frame_buff)->u64TimeStamp;
			if(decode_put_msg_to_queue(hdl->str_id,&hdl->str_msgbuf.thr_data,DECODE_CMD_NEW_DATA,0))
			{
				OSA_ERROR("str:%d send msg fail",hdl->str_id);				
				stream_sta_change(hdl,STR_THR_STA_STANDBY);
				return -1;
			}
		}
		else if(AV_AUDIO_FRAME_A==((AV_FRAME_HEAD_S*)frame_buff)->u8FrameType)
		{//取到音频流
			stream_audio_pro(hdl,(AV_FRAME_HEAD_S*)frame_buff,frame_len);
			player_put_empty_buf(dec_handle_buf, buf_id);
		}
		else
		{
			goto get_disk_over;
		}
		return 0;
	}
	//从硬盘取流错误
get_disk_over:
	player_put_empty_buf(dec_handle_buf, buf_id);
	stream_replay_file_end(hdl);
	stream_thr_stop_replay_video(hdl); 		
	stream_replay_discard_check(hdl);
	if(!stream_replay_file_ring(hdl,0))
	{
		if(!stream_replay_file_play(hdl,0))
		{				
			return 0;	
		}
	}		
	if(hdl->str_thr_sta==STR_THR_STA_WORK_ON)
	{//防止所有线程都是在standby状态，但是stream收到set rate等命令可能进入repeat,
	//再次收到choose file时单独到work状态(其他线程还是在standby)
		stream_sta_change(hdl,STR_THR_STA_CONNECT_REPEAT);
	}
	OSA_ERROR("str:%d get disk data fail",hdl->str_id);
	return -1;
}

/**
单帧播放的时候，上一帧情况下参数设置为READ_BACK_1_IFRAME
下一帧情况下参数设置为READ_FORWARD_VIDEO_FRAME
ret>0 解码库要丢4帧，有一帧用来做参考也要丢
ret ==0解码库要丢没有解出来的3帧
ret <0不需要进行任何操作
*/
static s8 stream_find_frame_pos(long handle,u8 cur_cmd,u8 back_cmd,s8 *reference_frame)
{
	u8 cnt=0,discar_num=0;
	DEBUG_PF("cur_cmd:%d,back_cmd:%d",cur_cmd,back_cmd);
	if(READ_FORWARD_VIDEO_FRAME==cur_cmd)
	{
		cur_cmd=READ_NORMAL;
	}
	if(READ_FORWARD_VIDEO_FRAME==back_cmd)
	{
		back_cmd=READ_NORMAL;
	}
	if((READ_NORMAL==back_cmd)&&((cur_cmd>=READ_BACK_1_IFRAME)&&(cur_cmd<=READ_BACK_4_IFRAME)))
	{//先跳1个i帧,保证跳过现在播放的时间点
		//storage_lib_read_frame(handle,READ_BACK_1_IFRAME,NULL,&buf);
		discar_num=3;
		*reference_frame=-1;
		//由于之后的操作是后退，所以不需要再安排i帧做参考
	}
	else if(((back_cmd>=READ_FORWARD_1_IFRAME)&&(back_cmd<=READ_FORWARD_4_IFRAME))&&(READ_NORMAL==cur_cmd))
	{//快进一次的间隔乘上解码库中可以存放的数量等到要后退的次数.回到正在显示的时间点
		for(cnt=0;cnt<((back_cmd-READ_FORWARD_1_IFRAME+1)*10-1);cnt++)
		{
			//storage_lib_read_frame(handle,READ_BACK_1_IFRAME,NULL,&buf);
		}
		*reference_frame=READ_BACK_1_IFRAME;
		discar_num=4;
		//由于快退了I帧，下一帧是正常播放。所以要在安排在退一个I帧做参考
	}
	else if(((back_cmd>=READ_FORWARD_1_IFRAME)&&(back_cmd<=READ_FORWARD_4_IFRAME))&&((cur_cmd>=READ_BACK_1_IFRAME)&&(cur_cmd<=READ_BACK_4_IFRAME)))
	{//快进一次的间隔乘上解码库中可以存放的数量等到要后退的次数.回到正在显示的时间点
		for(cnt=0;cnt<(back_cmd-READ_FORWARD_1_IFRAME+1)*10;cnt++)
		{
			//storage_lib_read_frame(handle,READ_BACK_1_IFRAME,NULL,&buf);
		}
		discar_num=3;
		*reference_frame=-1;
		//不需要安排I帧做参考
	}
	else if(((back_cmd>=READ_BACK_1_IFRAME)&&(back_cmd<=READ_BACK_4_IFRAME))&&(READ_NORMAL==cur_cmd))
	{//快退一次的间隔乘上解码库中可以存放的数量等到要前进的次数.回到正在显示的时间点
		for(cnt=0;cnt<((back_cmd-READ_BACK_1_IFRAME+1)*3-1);cnt++)
		{
			//storage_lib_read_frame(handle,READ_FORWARD_1_IFRAME,NULL,&buf);
		}
		*reference_frame=READ_FORWARD_1_IFRAME;
		discar_num=4;
		//要安排再前进I帧做参考
	}
	else if(((back_cmd>=READ_BACK_1_IFRAME)&&(back_cmd<=READ_BACK_4_IFRAME))&&((cur_cmd>=READ_FORWARD_1_IFRAME)&&(cur_cmd<=READ_FORWARD_4_IFRAME)))
	{//快退一次的间隔乘上解码库中可以存放的数量等到要前进的次数.回到正在显示的时间点
		for(cnt=0;cnt<((back_cmd-READ_BACK_1_IFRAME+1)*3-1);cnt++)
		{
			//storage_lib_read_frame(handle,READ_FORWARD_1_IFRAME,NULL,&buf);
		}
		*reference_frame=READ_FORWARD_1_IFRAME;
		discar_num=4;
		//要安排I帧做参考
	}
	else
	{//不需要进行任何跳帧
		*reference_frame=-1;
		discar_num=0;
	}
	DEBUG_PF("stream_find_frame_pos discard frame num:%d",discar_num);
	return discar_num;
}
static void stream_replay_discard_check(stream_ctl *hdl)
{
	if((hdl->replay_infor[0].query.end_time!=hdl->replay_infor[1].query.begin_time)
		&&(hdl->replay_infor[0].query.begin_time!=hdl->replay_infor[1].query.end_time))
	{
		hdl->play_id++;
		hdl->str_msgbuf.thr_data.discard_num=3;		
		hdl->str_msgbuf.thr_data.play_id=hdl->play_id;
		fluency_set_play_id(hdl->str_id, hdl->play_id);
		copy_set_play_id(hdl->str_id, hdl->play_id);
		if(decode_put_msg_to_queue(hdl->str_id,&hdl->str_msgbuf.thr_data,DECODE_CMD_FRAME_SYNC,0))
		{
			OSA_ERROR("str:%d send msg fail",hdl->str_id);
		}
	}
}
static void stream_save_replay_file(stream_ctl *hdl,file_info*infor,u8 play_now)
{
	if((-1==hdl->replay_infor[1].query.chn)||play_now)
	{
		hdl->query_num--;
		memcpy(&hdl->replay_infor[1].query,infor,sizeof(file_info));		
		if(play_now)
		{
			hdl->query_num=QUERY_BUF_NUM-1;
		}
	}
	else
	{
		OSA_ERROR("str:%d stream save replay file fail",hdl->str_id);
	}
}
static void stream_replay_file_end(stream_ctl *hdl)
{
	hdl->replay_infor[0].query.chn=-1;
	hdl->query_num++;//不能放在stream_replay_file_end中，停止到重连之间有10ms延时。中间收到消息时会异常
}
//将query[1]设置到query[0]
static s8 stream_replay_file_ring(stream_ctl *hdl,u8 update)
{	
	if((-1!=hdl->replay_infor[1].query.chn)
		&&(update||(-1==hdl->replay_infor[0].query.chn)))
	{
		memcpy(&hdl->replay_infor[0],&hdl->replay_infor[1],sizeof(file_info));		
		hdl->replay_infor[1].query.chn=-1;
		return 0;
	}
	OSA_ERROR("str:%d stream_replay_file_ring fail",hdl->str_id);
	return -1;
}
//播放query[0]
static s8 stream_replay_file_play(stream_ctl *hdl,u8 update)
{	
	if(update)
	{
		stream_thr_stop_replay_video(hdl);
	}
	if(storage_lib_read_start(&hdl->replay_infor[0].query,NULL,&hdl->disk_hdl) < 0)
	{
		DEBUG_PF("str:%d start read file failed",hdl->str_id);
		return -1;
	}	
	else
	{				
		if(hdl->replay_infor[0].set_time)
		{//跳到设置的时间点播放
			DEBUG_PF("str:%d set time",hdl->str_id);
			if(storage_lib_read_set(hdl->disk_hdl,hdl->replay_infor[0].set_time))
			{
				DEBUG_PF("str%d set play point fail",hdl->str_id);
				return -1;
			}
		}				
	}		
	return 0;
}
static void stream_black_screen_pro(stream_ctl *hdl,u8 cmd)
{
	hdl->repeat_cnt++;
	if((hdl->repeat_cnt==BLACK_TIME)||cmd)
	{//5s钟还连不上，则拿一个buffer填充黑色发过去
		if(decode_put_msg_to_queue(hdl->str_id,&hdl->str_msgbuf.thr_data,DECODE_CMD_BLACK,0))
		{
			OSA_ERROR("str:%d send msg fail",hdl->str_id);
		}
	}
}
static s8 stream_video_continue(stream_ctl *hdl,u8 cur_cmd)
{
	s8 reference_frame=-1;	
	u8 discard_num=0;
	if(!hdl->disk_hdl)
	{
		OSA_ERROR("str:%d disk_hdl is null",hdl->str_id);
		return -1;
	}
	//discard_num=stream_find_frame_pos(hdl->disk_hdl,cur_cmd,hdl->read_cmd,&reference_frame);
	if((discard_num=stream_find_frame_pos(hdl->disk_hdl,cur_cmd,hdl->read_cmd,&reference_frame))>0)
	{
		hdl->play_id++;
		fluency_set_play_id(hdl->str_id, hdl->play_id);
		copy_set_play_id(hdl->str_id, hdl->play_id);
		hdl->str_msgbuf.thr_data.discard_num=discard_num;
		hdl->str_msgbuf.thr_data.play_id=hdl->play_id;
		if(decode_put_msg_to_queue(hdl->str_id,&hdl->str_msgbuf.thr_data,DECODE_CMD_FRAME_SYNC,0))
		{
			OSA_ERROR("str:%d send msg fail",hdl->str_id);
		}
		hdl->read_cmd=-1!=reference_frame?reference_frame:cur_cmd;
		/*					
		for(i=0;i<discard_num;i++)
		{
			if(stream_pro_disk_avdata(hdl))
			{				
				OSA_ERROR("str:%d pro disk video data fail",hdl->str_id);				
				hdl->read_cmd=cur_cmd;
				return -1;
			}		
			hdl->read_cmd=cur_cmd;
		}
		*/
		storage_lib_read_set(hdl->disk_hdl,copy_get_pts(hdl->str_id)/1000000);
		hdl->read_cmd=cur_cmd;
	}
	return 0;
}
static void stream_audio_pro(stream_ctl*hdl,AV_FRAME_HEAD_S *buf,int frame_len)
{
	AV_AUDIO_INFO_S *head=&buf->unAudioInfo;	
	char *enc_data=((char*)buf)+sizeof(AV_FRAME_HEAD_S);
	pthread_mutex_lock(&audio_ctl.audio_lock);	
	if((audio_ctl.audio_ch==hdl->str_id)&&(FLU_WORK_NOMAL_RATE==hdl->set_rate))
	{
		memcpy(audio_ctl.audio_buf,head,sizeof(AV_AUDIO_INFO_S));
		memcpy(audio_ctl.audio_buf+sizeof(AV_AUDIO_INFO_S),enc_data,frame_len-sizeof(AV_FRAME_HEAD_S));
		audio_put_channel_data(audio_ctl.audio_buf);
	}
	pthread_mutex_unlock(&audio_ctl.audio_lock);
}
u8 stream_preview_status(u8 ch)
{
	return av_pool_is_writing(ch,0)>0?1:0;
}
static void stream_parameter_init(stream_ctl *hdl)
{
	hdl->replay_infor[0].query.chn=-1;
	hdl->replay_infor[1].query.chn=-1;	
	hdl->frame_no_back=-1;
	hdl->disk_hdl=0;
	hdl->query_num=QUERY_BUF_NUM;
	hdl->repeat_cnt=0;
	hdl->cnt=0;
	hdl->str_thr_sta=STR_THR_STA_STANDBY;
	hdl->set_rate=FLU_WORK_NOMAL_RATE;
	hdl->play_normal=0;
	hdl->play_id=0;
}
static void stream_set_all_clr_cmd(u8 ch)
{
	decode_set_clr_cmd(ch,1);
	fluency_set_clr_cmd(ch,1);
	copy_set_clr_cmd(ch,1);
	dis_set_clr_cmd(ch,1);
}
s8 stream_put_msg_to_queue(u8 ch,_thr_msg_data *data,u8 cmd,int flag)
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
	send_buf.msg_type=str_msg_type[ch];
	memcpy(&send_buf.thr_data,data,sizeof(_thr_msg_data));
	send_buf.thr_data.cmd=cmd;
	if(msgsnd(msg_id,&send_buf,sizeof(_thr_msg_data),flag))
	{
		OSA_ERROR("str%d send cmd:%d fail!!!",ch,cmd);								
		pthread_mutex_unlock(&msg_lock);	
		return -1;
	}
	pthread_mutex_unlock(&msg_lock);	
	return 0;
}


