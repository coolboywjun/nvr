#include<player.h>
#include<pthread.h>
#include<unistd.h>
#include<audio_params.h>
#include<audio_play.h>
#include<sys/prctl.h>
#define AUDIO_RECORD_SIZE (320)
enum{
	AUDIO_RECORD_STA_RUNNING=1,
	AUDIO_RECORD_STA_STOP,
	AUDIO_RECORD_STA_EXIT,
};
static volatile u8 audio_record=0;
static AV_AUDIO_INFO_S enc_type={.u16EncodeType=1,.u8AudioBits=16,.u8AudioChannels=1,.u8AudioSamples=0};
static OSA_ThrHndl audio_record_thr;
static pthread_mutex_t audio_record_lock=PTHREAD_MUTEX_INITIALIZER;


static void *audio_record_thr_main(void*arg);
static void audio_record_get_cmd(u8 *cmd);
static s8 audio_record_rec_init(AV_AUDIO_INFO_S *head);
int HH_AENC_G711AEncode(char * pInData, int nInLen, char * pOutData, int *pOutLen);
int HH_AENC_G711UEncode(char * pInData, int nInLen, char * pOutData,int *pOutLen);
void audio_record_set_rec_par(AV_AUDIO_INFO_S*par)
{
	pthread_mutex_lock(&audio_record_lock);
	memcpy(&enc_type,par,sizeof(AV_AUDIO_INFO_S));
	pthread_mutex_unlock(&audio_record_lock);
}
void audio_record_get_rec_par(AV_AUDIO_INFO_S *par)
{
	pthread_mutex_lock(&audio_record_lock);
	memcpy(par,&enc_type,sizeof(AV_AUDIO_INFO_S));
	pthread_mutex_unlock(&audio_record_lock);
}
void audio_record_thr_exit(void)
{
	pthread_mutex_lock(&audio_record_lock);
	audio_record=AUDIO_RECORD_EXIT_CMD;
	pthread_mutex_unlock(&audio_record_lock);
}
void audio_record_set_cmd(u8 cmd)
{
	pthread_mutex_lock(&audio_record_lock);
	audio_record=cmd;
	pthread_mutex_unlock(&audio_record_lock);
}
static void audio_record_get_cmd(u8 *cmd)
{
	pthread_mutex_lock(&audio_record_lock);
	*cmd=audio_record;
	audio_record=0;
	pthread_mutex_unlock(&audio_record_lock);
}
int audio_record_thr_create(void *param)
{
	int status;
	if((status=pthread_create(&audio_record_thr.hndl,NULL,audio_record_thr_main,(void*)NULL)))
	{
		OSA_ERROR("OSA_tskCreate()");	
		return status;
	}	
	return status;
}
int audio_record_thr_delete(void)
{
	OSA_thrDelete(&audio_record_thr);
	return OSA_SOK;
}
static void *audio_record_thr_main(void*arg)
{
	char*buf,*buf_enc,exit=0;
	u32 start_cnt=0;
	u8 record_cmd=0,record_sta=AUDIO_RECORD_STA_STOP;
	long av_audio_hdl=0;
	AV_FRAME_HEAD_S cur_audio_head;

	if(NULL==(buf=malloc(AUDIO_RECORD_SIZE)))
	{
		printf("audio_record_thr_main malloc fail\r\n");
	}
	if(NULL==(buf_enc=malloc(AUDIO_RECORD_SIZE)))
	{
		free(buf);
		printf("audio_record_thr_main malloc fail\r\n");
	}	
	prctl(PR_SET_NAME,"audio_record");
	DEBUG_PF("audio record thr start");
	cur_audio_head.u32FrameNo=1;
	cur_audio_head.u8FrameType=AV_AUDIO_FRAME_A;
	cur_audio_head.unAudioInfo.u16EncodeType=1;
	cur_audio_head.unAudioInfo.u8AudioBits=16;
	cur_audio_head.unAudioInfo.u8AudioChannels=1;
	cur_audio_head.unAudioInfo.u8AudioSamples=0;	
	cur_audio_head.u16FrameFlag=0x3448;
	cur_audio_head.u32FrameSize=AUDIO_RECORD_SIZE>>1;	
	while(!exit)
	{
		audio_record_get_cmd(&record_cmd);
		if((AUDIO_RECORD_START_CMD==record_cmd)&&(record_sta=AUDIO_RECORD_STA_STOP))
		{//开始录音			
			DEBUG_PF("audio record start cmd");
			if(start_cnt==0)
			{		
				DEBUG_PF("audio record init alsa lib");				
				record_sta=AUDIO_RECORD_STA_RUNNING;			
				audio_record_get_rec_par(&cur_audio_head.unAudioInfo);
				DEBUG_PF("enc type:%d",cur_audio_head.unAudioInfo.u16EncodeType);
				if(audio_record_rec_init(&cur_audio_head.unAudioInfo))
				{					
					audio_record_exit();						
					record_sta=AUDIO_RECORD_STA_STOP;			
				}
				if(av_pool_start_write_frame(AV_POOL_TALK_CHN,AV_POOL_TALK_OUT_STREAM,&av_audio_hdl))
				{
					OSA_ERROR("av_pool_start_write_frame error");
					audio_record_exit();						
					record_sta=AUDIO_RECORD_STA_STOP;			
				}
			}
			start_cnt++;
		}
		else if((AUDIO_RECORD_STOP_CMD==record_cmd)&&(record_sta=AUDIO_RECORD_STA_RUNNING))
		{//停止录音			
			DEBUG_PF("audio record stop cmd");
			start_cnt--;
			if(start_cnt==0)
			{
				DEBUG_PF("audio record exit alsa lib");				
				record_sta=AUDIO_RECORD_STA_STOP;		
				audio_record_exit();					
				av_pool_stop_write_frame(av_audio_hdl);
				av_audio_hdl=0;
				cur_audio_head.u32FrameNo=1;
			}
		}
		else if(AUDIO_RECORD_EXIT_CMD==record_cmd)
		{//线程退出
			DEBUG_PF("audio record rcv exit cmd");
			exit=1;
			record_sta=AUDIO_RECORD_STA_EXIT;					
			audio_record_exit();					
			av_pool_stop_write_frame(av_audio_hdl);
			av_audio_hdl=0;
		}
		switch(record_sta)
		{
			case AUDIO_RECORD_STA_RUNNING://录音			
				cur_audio_head.u64TimeStamp=OSA_getCurTimeInUsec();
				if(!audio_record_get_stram(buf,AUDIO_RECORD_SIZE))
				{
					int out_len;
					if(cur_audio_head.unAudioInfo.u16EncodeType==1)
					{
						HH_AENC_G711AEncode(buf,AUDIO_RECORD_SIZE,buf_enc,&out_len);
					}
					else if(cur_audio_head.unAudioInfo.u16EncodeType==2)
					{
						HH_AENC_G711UEncode(buf,AUDIO_RECORD_SIZE,buf_enc,&out_len);
					}
					else 
					{
						OSA_ERROR("audio enc type error");
					}
					if(av_pool_write_frame(av_audio_hdl,(u8*)buf_enc,out_len,
						(u8*)&cur_audio_head,sizeof(AV_FRAME_HEAD_S),
						cur_audio_head.u32FrameNo,cur_audio_head.u8FrameType,cur_audio_head.u64TimeStamp))
					{
						OSA_ERROR("av_pool_write_frame error");
					}
					else
					{
						cur_audio_head.u32FrameNo++;
					}
					usleep(5000);
				}
				else
				{
					DEBUG_PF("audio_record_get_stram fail");
				}
				break;
			case AUDIO_RECORD_STA_STOP://停止录音,睡眠
				player_waitMsecs(10);
				break;
			default:;
		}
		usleep(5000);
	}
	DEBUG_PF("audio record exit\r\n");
	free(buf);
	free(buf_enc);
	return NULL;
}
static s8 audio_record_rec_init(AV_AUDIO_INFO_S *head)
{
	set_audioparams sparam;
	memset(&sparam, 0, sizeof(set_audioparams));
	sparam.bits=head->u8AudioBits;
	sparam.channels =head->u8AudioChannels;
	if(8==sparam.bits)
	{
		sparam.audioFormat =ALSA_PCM_FORMAT_U8;
	}
	else if(16==sparam.bits)
	{
		sparam.audioFormat =ALSA_PCM_FORMAT_S16_LE;
	}
	else 
	{
		OSA_ERROR("sample bit:%d error",sparam.bits);
		return -1;
	}
	if(0==head->u8AudioSamples)
	{
		sparam.rate =8000;
	}
	else if(1==head->u8AudioSamples)
	{
		sparam.rate =16000;
	}
	else if(2==head->u8AudioSamples)
	{
		sparam.rate =32000;
	}
	else if(3==head->u8AudioSamples)
	{
		sparam.rate =11025;
	}
	else
	{		
		OSA_ERROR("sample rate:%d error",head->u8AudioSamples);
		return -1;
	}
	sparam.periodFrames = 256;		//此参数有可能设置失败，采用系统默认的周期大小
	pthread_mutex_lock(&audio_lock);
	audio_record_init(&sparam);
	pthread_mutex_unlock(&audio_lock);
	return 0;
}
