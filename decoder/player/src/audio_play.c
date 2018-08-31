#include<player.h>
#include<pthread.h>
#include<unistd.h>
#include<audio_params.h>
#include<audio_play.h>
#include<sys/prctl.h>
#include<stdlib.h>
#define FILE_LEN (20)
#define CHUNK_BYTES (4096)

#define AUDIO_BUF_NUM (100)
#define AUDIO_BUFFER_SIZE (sizeof(AV_AUDIO_INFO_S)+160)
typedef struct{
	u32 wr;
	u32 rd;
	u8 init;	
	char *buf_index[AUDIO_BUF_NUM];
}_buf_ctl;
static _buf_ctl audio_buf;
enum{
	AUDIO_STA_IDLE=0,
	AUDIO_STA_WORK,
	AUDIO_STA_ERROR,
};
typedef struct{
	int audio_fd;
	alsa_handle phandle;
	char alarm_file[FILE_LEN];	
}_audio_hdl;
typedef struct{	
	long avpool_hdl;
	u32 put_len;
	set_audioparams param_back;
	FILE *fd_alarm;
	u8 play_flag;
	u8 exit;
	u8 audio_sta;
	u8 play_type;
	u8 read_file_end;
}_audio_run;

static OSA_ThrHndl audio_thr;
pthread_mutex_t audio_lock;
static _audio_hdl audio_hdl;
static int audio_init = 0;
#ifdef PLAYER_TEST
_audio_run *audio_play_test;
#endif
static u8 audio_play_ctl=0x00;//高四位是类型(bit6:ch,bit5:alarm,bit4:phone)，低四位是命令(bit2:ch,bit1:alarm,bit0:phone)
void *audio_thr_main(void*arg);
static s8 audio_play_par_change(set_audioparams*now,set_audioparams *back);
static void audio_alsa_exit(void);
static s8 audio_alsa_init(set_audioparams*sparam);
static void audio_play_cmd_pro(_audio_run *hdl,u8 type_cmd);
static s8 audio_play_pro(_audio_run*hdl,u8*buffer,set_audioparams *param,u32 data_len);
static u8 audio_get_play_type(u8 flag);
static s32 audio_read_alarm_file(_audio_run*hdl,char*buf);
static s8 audio_open_alarm_file(_audio_run*hdl,set_audioparams*params);
static void audio_sta_change(_audio_run *hdl,u8 dst_sta);
static int audio_data_dec(char *in_buf,u8 enc_type,int frame_len,char*out_buf);
static void audio_set_audio_param(AV_AUDIO_INFO_S*head,set_audioparams *sparam);
static u32 audio_rcv_nbytes(u8 init,char*data,u32 data_len,char*out_buf,char*remain_buf);
int HH_ADEC_G711ADecode(char * pInData, int nInLen, char * pOutData, int *pOutLen);
int HH_ADEC_G711UDecode(char * pInData, int nInLen, char * pOutData, int *nOutLen);
static u8 audio_get_cmd(void);
static s8 audio_alloc_buf(void);
static void audio_free_buf(void);
static s8 audio_get_channel_data(char*out_buf);
static s8 audio_combine_ch_data(char*enc_buf,char*dec_buf,char*buf_back,char*buffer_remain,set_audioparams*audio_para,u32 *rcv_len);
static s8 audio_combine_alarm_data(_audio_run *hdl,set_audioparams*audio_para,char*buffer);
static s8 audio_combine_phone_data(_audio_run *hdl,char*enc_buf,char*dec_buf,char*buffer_back,char*buffer_remain,set_audioparams*audio_para);
int audio_thr_create(void *param)
{
	int status;	
	if(audio_alloc_buf())
	{
		OSA_ERROR("audio_alloc_buf");
		return -1;
	}
	if(player_mutex_create(&audio_lock))
	{
		audio_free_buf();
		OSA_ERROR("player_mutex_create()!!!");
		return -1;
	}	
	if((status=pthread_create(&audio_thr.hndl,NULL,audio_thr_main,(void*)NULL)))
	{
		OSA_ERROR("OSA_tskCreate()");	
		if(player_mutex_delete(&audio_lock))
		{
			OSA_ERROR("player_mutex_delete()!!!");
		}
		audio_free_buf();
		return status;
	}		
	audio_hdl.audio_fd=-1;
	audio_hdl.phandle=0;
	return status;
}
int audio_thr_delete(void)
{
	if(player_mutex_delete(&audio_lock))
	{
		OSA_ERROR("player_mutex_delete()!!!");
	}
	OSA_thrDelete(&audio_thr);
	return OSA_SOK;
}
void *audio_thr_main(void*arg)
{
//	AV_AUDIO_INFO_S head;	
	set_audioparams audio_par;
	u8 rcv_over=0,type_cmd; 
	_audio_run audio_ctl;
	char *buffer=NULL,*buffer_remain=NULL,*buffer_back=NULL,*enc_buf=NULL,*dec_buf=NULL;
	unsigned int all_len=CHUNK_BYTES;
	if(NULL==(buffer=malloc(all_len)))
	{
		OSA_ERROR("malloc pipe data fail");
	}
	if(NULL==(buffer_remain=malloc(DATA_SIZE<<1)))
	{
		OSA_ERROR("malloc pipe data fail");
	}
	if(NULL==(enc_buf=malloc(DATA_SIZE+sizeof(AV_FRAME_HEAD_S))))
	{
		OSA_ERROR("malloc enc buf fail");
	}
	if(NULL==(dec_buf=malloc(DATA_SIZE<<1)))
	{
		OSA_ERROR("malloc dec buf fail");
	}	
	prctl(PR_SET_NAME,"audio_play");
	buffer_back=buffer;
	memset(&audio_ctl.param_back,0,sizeof(set_audioparams));
	audio_ctl.play_flag=0;
	audio_ctl.exit=0;
	audio_ctl.audio_sta=AUDIO_STA_IDLE;
	audio_ctl.avpool_hdl=0;
	audio_ctl.play_type=0xff;
#ifdef PLAYER_TEST
	audio_play_test=&audio_ctl;
#endif
	DEBUG_PF("audio play thread run");
	while(!audio_ctl.exit)
	{
		type_cmd=audio_get_cmd();
		if(type_cmd&0xf0)
		{
			u8 type;		
			audio_play_cmd_pro(&audio_ctl,type_cmd);
			type=audio_get_play_type(audio_ctl.play_flag);		
			if(audio_ctl.play_type!=type)
			{
				buffer=buffer_back;
				audio_ctl.play_type=type;
				audio_rcv_nbytes(1,NULL,0,buffer_back,NULL);
				if(AUDIO_TYPE_CHANNEL==audio_ctl.play_type)
				{
					audio_put_data_init();
				}
			}
		}
		switch(audio_ctl.play_type)
		{
			case AUDIO_TYPE_CHANNEL:
				rcv_over=audio_combine_ch_data(enc_buf,dec_buf,buffer_back,buffer_remain,&audio_par,&audio_ctl.put_len)?0:1;		
				break;
			case AUDIO_TYPE_ALARM:
				rcv_over=audio_combine_alarm_data(&audio_ctl,&audio_par,buffer_back)?0:1;
				break;				
			case AUDIO_TYPE_PHONE:
				rcv_over=audio_combine_phone_data(&audio_ctl,enc_buf,dec_buf,buffer_back,buffer_remain,&audio_par)?0:1;
				break;
			default:;
		}
		if((audio_ctl.audio_sta==AUDIO_STA_WORK)&&rcv_over)
		{
			rcv_over=0;
			if(audio_play_pro(&audio_ctl,(u8*)buffer_back,&audio_par,audio_ctl.put_len))
			{//状态改变
				OSA_ERROR("audio_play_pro fail");
				audio_sta_change(&audio_ctl,AUDIO_STA_ERROR);
			}
		}		
		usleep(5000);
	}
	free(buffer_back);	
	free(buffer_remain);
	free(enc_buf);
	free(dec_buf);
	audio_alsa_exit();	
	av_pool_stop_read_frame(audio_ctl.avpool_hdl);
	if(audio_ctl.fd_alarm!=NULL)	
	{
		fclose(audio_ctl.fd_alarm);
	}
	audio_free_buf();
	DEBUG_PF("audio play thread exit");
	return NULL;
}
s8 audio_thr_exit(void)
{	
	s8 ret=0;
	pthread_mutex_lock(&audio_lock);
	audio_play_ctl|=(1<<7);
	audio_play_ctl|=(1<<3);
	pthread_mutex_unlock(&audio_lock);
	return ret;
}
static u8 audio_get_cmd(void)
{
	u8 ret;
	pthread_mutex_lock(&audio_lock);
	ret=audio_play_ctl;
	audio_play_ctl=0;
	pthread_mutex_unlock(&audio_lock);
	return ret;
}
s8 audio_stop(u8 audio_type)
{	
	pthread_mutex_lock(&audio_lock);
	audio_play_ctl|=1<<(audio_type+4);
	audio_play_ctl&=(~(1<<audio_type));
	pthread_mutex_unlock(&audio_lock);
	return 0;
}
s8 audio_play(u8 audio_type)
{	
	pthread_mutex_lock(&audio_lock);	
	audio_play_ctl|=1<<(audio_type+4);
	audio_play_ctl|=1<<audio_type;
	pthread_mutex_unlock(&audio_lock);
	return 0;
}
void audio_set_alarm_file(char*file,char flag)
{
	pthread_mutex_lock(&audio_lock);	
	if(flag)
	{//设置报警文件
		memcpy(audio_hdl.alarm_file,file,sizeof(audio_hdl.alarm_file));
	}
	else
	{//获取报警文件
		memcpy(file,audio_hdl.alarm_file,sizeof(audio_hdl.alarm_file));
	}
	pthread_mutex_unlock(&audio_lock);
}
static s8 audio_combine_ch_data(char*enc_buf,char*dec_buf,char*buf_back,char*buffer_remain,set_audioparams*audio_para,u32 *rcv_len)
{	
	u32 dec_len;
	AV_AUDIO_INFO_S head;
	s8 ret=-1;
	if(!audio_get_channel_data(enc_buf))
	{
		memcpy(&head,enc_buf,sizeof(AV_AUDIO_INFO_S));
		dec_len=audio_data_dec(enc_buf+sizeof(AV_AUDIO_INFO_S),head.u16EncodeType,DATA_SIZE,dec_buf);
		if(audio_rcv_nbytes(0,dec_buf,dec_len,buf_back,buffer_remain)==CHUNK_BYTES)
		{
			audio_set_audio_param(&head,audio_para);
			*rcv_len=CHUNK_BYTES;
			ret=0;
		}
	}
	return ret;
}
static s8 audio_combine_alarm_data(_audio_run *hdl,set_audioparams*audio_para,char*buffer)
{
	s8 ret=-1;
	static set_audioparams alarm_audio_par;
	if((NULL==hdl->fd_alarm)&&audio_open_alarm_file(hdl,&alarm_audio_par))
	{
		OSA_ERROR("audio_open_alarm_file fail");				
	}
	else
	{
		if(!hdl->read_file_end)
		{
			hdl->put_len=audio_read_alarm_file(hdl,buffer);
			memcpy(audio_para,&alarm_audio_par,sizeof(set_audioparams));
			ret=0;
		}
		else if(SND_PCM_STATE_XRUN==audio_get_playback_state()) 
		{					
			audio_stop(AUDIO_TYPE_ALARM);				
		}
	}
	return ret;
}
static s8 audio_combine_phone_data(_audio_run *hdl,char*enc_buf,char*dec_buf,char*buffer_back,char*buffer_remain,set_audioparams*audio_para)
{
	AV_AUDIO_INFO_S head;
	s8 ret=-1;
	if((!hdl->avpool_hdl)&&(av_pool_start_read_frame(AV_POOL_TALK_CHN,AV_POOL_TALK_IN_STREAM,1000,&hdl->avpool_hdl) < 0))
	{
		OSA_ERROR("Start read av pool failed.\n");
	}	
	else
	{
		u32 out_len;
		int buflen;
		char *pbuf=NULL;
		out_len = av_pool_read_frame(hdl->avpool_hdl,&pbuf,&buflen,10);
		if(out_len > 0&&buflen>0&&pbuf!=NULL)
		{
			memcpy(&head,&((AV_FRAME_HEAD_S*)pbuf)->unAudioInfo,sizeof(AV_AUDIO_INFO_S));
			out_len=audio_data_dec(pbuf+sizeof(AV_FRAME_HEAD_S),head.u16EncodeType,out_len-sizeof(AV_FRAME_HEAD_S),dec_buf);
			if(audio_rcv_nbytes(0,dec_buf,out_len,buffer_back,buffer_remain)==CHUNK_BYTES)
			{
				audio_set_audio_param(&head,audio_para);
				hdl->put_len=CHUNK_BYTES;
				ret=0;
			}
		}
	}
	return ret;
}
static u32 audio_rcv_nbytes(u8 init,char*data,u32 data_len,char*out_buf,char*remain_buf)
{
	static char*buffer;
	static u32 remain_len=0,all_len=CHUNK_BYTES;
	u32 copy_len=0,nbytes=0;
	if(init)
	{
		buffer=out_buf;
		all_len=CHUNK_BYTES;
		remain_len=0;
	}
	else
	{
		if(remain_len)
		{
			memcpy(buffer,remain_buf,remain_len);
			buffer+=remain_len;
			all_len-=remain_len;
			remain_len=0;					
		}			
		copy_len=all_len>=data_len?data_len:all_len;
		memcpy(buffer,data,copy_len);
		all_len-=copy_len;
		buffer+=copy_len;
		nbytes=CHUNK_BYTES-all_len;
		if(!all_len)
		{
			remain_len=data_len-copy_len;
			memcpy(remain_buf,data+copy_len,remain_len);
			buffer=out_buf;
			all_len=CHUNK_BYTES;
		}		
	}
	return nbytes;
}
static void audio_sta_change(_audio_run *hdl,u8 dst_sta)
{
	switch(dst_sta)
	{
		case AUDIO_STA_IDLE:
			if(hdl->audio_sta==AUDIO_STA_WORK)
			{
				hdl->audio_sta=AUDIO_STA_IDLE;				
			}
			break;
		case AUDIO_STA_WORK:
			if(hdl->audio_sta==AUDIO_STA_IDLE)
			{
				hdl->audio_sta=AUDIO_STA_WORK;				
			}
			break;
		case AUDIO_STA_ERROR:
			hdl->audio_sta=AUDIO_STA_ERROR;
			break;
		default:;
	}
}
static u8 audio_get_play_type(u8 flag)
{
	u8 type=0xff;
	if(flag&0x01)
	{
		type=AUDIO_TYPE_PHONE;
	}
	else if(flag&0x02)
	{
		type=AUDIO_TYPE_ALARM;
	}
	else if(flag&0x04)
	{
		type=AUDIO_TYPE_CHANNEL;
	}
	return type;
}
static s8 audio_open_alarm_file(_audio_run*hdl,set_audioparams*params)
{
	char file_path[FILE_LEN],*buf=NULL;
	audio_set_alarm_file(file_path,0);
	if(NULL==(hdl->fd_alarm=fopen(file_path,"rb")))
	{
		OSA_ERROR("open alarm file error");
		return -1;
	}
	buf=(char*)malloc(44);
	memset(buf,0,44);
	fread(buf,44,1,hdl->fd_alarm);
	params->bits=(*(buf+0x23)<<8)+*(buf+0x22);
	params->channels=(*(buf+0x17)<<8)+*(buf+0x16);	
	params->rate=(*(buf+0x1b)<<24)+(*(buf+0x1a)<<16)+(*(buf+0x19)<<8)+*(buf+0x18);	
	params->periodFrames = 256;		//此参数有可能设置失败，采用系统默认的周期大小
	if(params->bits==8)
	{
		params->audioFormat=ALSA_PCM_FORMAT_U8;
	}
	else
	{
		params->audioFormat=ALSA_PCM_FORMAT_S16_LE;
	}
	if(*(buf+0x10)!=0x10)
	{
		free(buf);
		OSA_ERROR("this alarm file is not pcm format or sample rate error");
		return -1;
	}
	fseek(hdl->fd_alarm,44,SEEK_SET);
	free(buf);	
	hdl->read_file_end=0;
	return 0;
}
static s32 audio_read_alarm_file(_audio_run*hdl,char*buf)
{
	s32 ret=-1;
	if(CHUNK_BYTES!=(ret=fread(buf,1,CHUNK_BYTES,hdl->fd_alarm)))
	{
		if(feof(hdl->fd_alarm))
		{
			DEBUG_PF("read alarm data end");	
			hdl->read_file_end=1;
		}
		else
		{
			DEBUG_PF("read alarm data fail");
			ret=-1;
		}
	}
	return ret;
}
static void audio_play_cmd_pro(_audio_run *hdl,u8 type_cmd)
{
	u8 type=type_cmd>>4,cmd=type_cmd&0x0f;	
	if(cmd&(1<<3))
	{
		hdl->exit=1;
	}
	if(type&(1<<AUDIO_TYPE_PHONE))
	{
		if(cmd)
		{
			audio_sta_change(hdl,AUDIO_STA_WORK);
			hdl->play_flag|=(1<<AUDIO_TYPE_PHONE);
		}
		else
		{
			av_pool_stop_read_frame(hdl->avpool_hdl);
			hdl->avpool_hdl=0;			
			hdl->play_flag&=(~(1<<AUDIO_TYPE_PHONE)); 					
		}
	}
	if(type&(1<<AUDIO_TYPE_ALARM))
	{
		if(cmd)
		{
			audio_sta_change(hdl,AUDIO_STA_WORK);
			hdl->play_flag|=(1<<AUDIO_TYPE_ALARM);
		}
		else if(hdl->fd_alarm!=NULL)
		{
			//关闭报警文件
			fclose(hdl->fd_alarm);
			hdl->fd_alarm=NULL; 					
			hdl->play_flag&=(~(1<<AUDIO_TYPE_ALARM)); 					
		}
	}
	if(type&(1<<AUDIO_TYPE_CHANNEL))
	{
		if(cmd)
		{			
			audio_sta_change(hdl,AUDIO_STA_WORK);
			hdl->play_flag|=(1<<AUDIO_TYPE_CHANNEL);
		}
		else
		{
			hdl->play_flag&=(~(1<<AUDIO_TYPE_CHANNEL)); 					
		}
	}	
	if(!hdl->play_flag)
	{
		audio_sta_change(hdl,AUDIO_STA_IDLE);
		audio_alsa_exit();			
		memset(&hdl->param_back,0,sizeof(set_audioparams));
	}
}
static s8 audio_play_pro(_audio_run*hdl,u8*buffer,set_audioparams *param,u32 data_len)
{
	if(!audio_play_par_change(param,&hdl->param_back))
	{
		audio_alsa_exit();
		audio_alsa_init(param);		
		memcpy(&hdl->param_back,param,sizeof(set_audioparams));
	}
	audio_playback_put_stram((char*)buffer,data_len);
	return 0;
}
static void audio_set_audio_param(AV_AUDIO_INFO_S*head,set_audioparams *sparam)
{
	sparam->bits=head->u8AudioBits;
	sparam->channels=head->u8AudioChannels;
	if(0==head->u8AudioSamples)
	{
		sparam->rate =8000;
	}
	else if(1==head->u8AudioSamples)
	{
		sparam->rate =16000;
	}
	else if(2==head->u8AudioSamples)
	{
		sparam->rate =32000;
	}
	else if(3==head->u8AudioSamples)
	{
		sparam->rate =11025;
	}
	if(sparam->bits==8)
	{
		sparam->audioFormat=ALSA_PCM_FORMAT_U8;
	}
	else if(sparam->bits==16)
	{
		sparam->audioFormat=ALSA_PCM_FORMAT_S16_LE;
	}
	sparam->periodFrames = 256;		//此参数有可能设置失败，采用系统默认的周期大小
}
static s8 audio_play_par_change(set_audioparams*now,set_audioparams *back)
{
	if((now->bits!=back->bits)
		||(now->channels!=back->channels)
		||(now->rate!=back->rate)
		||(now->audioFormat!=back->audioFormat))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
static void audio_alsa_exit(void)
{
	if(audio_init)
	{
		audio_playback_exit();
		audio_init = 0;
	}
}
static s8 audio_alsa_init(set_audioparams*sparam)
{
	DEBUG_PF("audio ch:%d,samples:%d,bit:%d,format:%d",sparam->channels,sparam->rate,sparam->bits,sparam->audioFormat);
	pthread_mutex_lock(&audio_lock);
	if(audio_init == 0)
	{
		audio_playback_init(sparam);
		audio_init = 1;
	}
	pthread_mutex_unlock(&audio_lock);
	return 0;
}
static int audio_data_dec(char *in_buf,u8 enc_type,int frame_len,char*out_buf)
{
	int audio_len=0;
	if(!enc_type)
	{
		memcpy(out_buf,in_buf,frame_len);
		audio_len=frame_len;
	}
	else if(1==enc_type)
	{
		if(-1==HH_ADEC_G711ADecode(in_buf,frame_len,out_buf,&audio_len))
		{
			OSA_ERROR("HH_ADEC_G711ADecode fail");
		}
	}
	else if(2==enc_type)
	{	
		if(-1==HH_ADEC_G711UDecode(in_buf,frame_len,out_buf,&audio_len))
		{
			OSA_ERROR("HH_ADEC_G711ADecode fail");
		}
	}
	else
	{
		OSA_ERROR("can't decode this audio encode format :%d",enc_type);
	}
	return audio_len;
}
/////////
static s8 audio_alloc_buf(void)
{
	u8 i=AUDIO_BUF_NUM;
	s8 ret=0;
	while(i)
	{
		i--;
		if(NULL==(audio_buf.buf_index[i]=(char*)malloc(AUDIO_BUFFER_SIZE)))
		{
			OSA_ERROR("malloc audio buf fail\r\n");
			ret=-1;
			break;
		}
	}
	return ret;
}
static void audio_free_buf(void)
{
	u8 i=AUDIO_BUF_NUM;
	while(i)
	{
		i--;
		free(audio_buf.buf_index[i]);
	}
}
void audio_put_data_init(void)
{
	pthread_mutex_lock(&audio_lock);	
	audio_buf.rd=0;
	audio_buf.wr=0;
	audio_buf.init=1;
	pthread_mutex_unlock(&audio_lock);	
}
void audio_put_channel_data(char*in_buf)
{
	pthread_mutex_lock(&audio_lock);	
	audio_buf.init=0;		
	memcpy(audio_buf.buf_index[audio_buf.wr],in_buf,AUDIO_BUFFER_SIZE);
	audio_buf.wr=(audio_buf.wr+1)%AUDIO_BUF_NUM;
	pthread_mutex_unlock(&audio_lock);		
}
static s8 audio_get_channel_data(char*out_buf)
{
	s8 ret=-1;
	pthread_mutex_lock(&audio_lock);
	if(audio_buf.rd!=audio_buf.wr)
	{
		memcpy(out_buf,audio_buf.buf_index[audio_buf.rd],AUDIO_BUFFER_SIZE);
		audio_buf.rd=(audio_buf.rd+1)%AUDIO_BUF_NUM;
		ret=0;
	}
	pthread_mutex_unlock(&audio_lock);	
	return ret;
}
