#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "audio_params.h"

#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include <alsa/asoundlib.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>
#include<syslog.h>

#define AUDIO_PF(fmt,args...) \
do\
{\
	if(1)\
	{\
		syslog(LOG_ERR,"(%s|%s|%d):" fmt,__FILE__, __func__, __LINE__,##args);\
	}\
	else\
	{\
		fprintf(stderr, "player_ser ERROR(%s|%s|%d): ", __FILE__, __func__, __LINE__);\
		fprintf(stderr, fmt"\r\n",##args); \
	}\
}\
while(0);
typedef struct{
	snd_pcm_t *record_pcm_hdl;		
	set_audioparams param;
}_record_para;
typedef struct{
	unsigned char sample_bit;
	unsigned char channel;
	snd_pcm_t *play_pcm_hdl;	
	set_audioparams param;
}_play_para;
typedef struct{
	_play_para play_hdl;
	_record_para record_hdl;
}_audio_para;
static _audio_para audio_lib_hdl={.play_hdl={.play_pcm_hdl=NULL},.record_hdl={.record_pcm_hdl=NULL}};
char *audioParams[]={
//	"numid=1,iface=MIXER,name='Master Playback Volume'",
//	"numid=2,iface=MIXER,name='MIC output volume'",
//	"numid=3,iface=MIXER,name='Fm output Volume'",
//	"numid=4,iface=MIXER,name='Line output Volume'",
//	"numid=5,iface=MIXER,name='LINEIN APM Volume'",
//	"numid=6,iface=MIXER,name='Capture Volume'",
//	"numid=7,iface=MIXER,name='Mic2 gain Volume'",
//	"numid=8,iface=MIXER,name='Mic1 gain Volume'",
//	"numid=4,iface=MIXER,name='Line output Volume'",
	"numid=9,iface=MIXER,name='Audio speaker out'",
	"numid=10,iface=MIXER,name='Audio earpiece out'",
	"numid=11,iface=MIXER,name='Audio dac phoneout'",
	"numid=12,iface=MIXER,name='Audio phone phonemic'",
	"numid=13,iface=MIXER,name='Audio phone out'",
//	"numid=14,iface=MIXER,name='Audio phone headsetmic'",
	"numid=15,iface=MIXER,name='Audio phone capture'"
};
/*
typedef enum AUDIO_NUM_ID{
	MASTER_PLAYBACK_VOLUME = 0,
	MIC_OUTPUT_VOLUME,
	FM_OUTPUT_VOLUME,	
	LINEIN_APM_VOLUME,
	CAPTURE_VOLUME,
	MIC2_GAIN_VOLUME,
	MIC1_GAIN_VOLUME,
	LINE_OUTPUT_VOLUME,
	AUDIO_SPEAKER_OUT,
	AUDIO_EARPIECE_OUT,
	AUDIO_DAC_PHONEOUT,
	AUDIO_PHONE_PHONEMIC,
	AUDIO_PHONE_OUT,
	AUDIO_PHONE_HEADSETMIC,
	AUDIO_PHONE_CAPTURE
}AUDIO_NUM_IDS;
*/
static int cset(int argc, char *argv[], int roflag, int keep_handle, char *card);
static int audio_open_volume(char *card);
static int audio_open_dev(snd_pcm_t **phandle,const char *name, nvr_pcm_stream_t mode);
static int audio_set_hw_params(snd_pcm_t *pcmhandle,set_audioparams *param,snd_pcm_uframes_t *chunk_size,snd_pcm_uframes_t *buffer_size);
static int audio_set_sw_params(snd_pcm_t *pcmhandle,snd_pcm_uframes_t chunk_size,snd_pcm_uframes_t buffer_size);
static int audio_put_dat(snd_pcm_t *pcmhandle, char * buffer, int nframe,int frame_len);
static int audio_get_audio_params(snd_pcm_hw_params_t *hwparams);
int audio_record_get_data(snd_pcm_t *pcmhandle, unsigned char *buffer, unsigned long nframe);
int audio_record_init(set_audioparams *param)
{
//	unsigned int buf_len;
	snd_pcm_uframes_t chunk_size,buffer_size;
	audio_open_volume("default");
	audio_open_dev(&audio_lib_hdl.record_hdl.record_pcm_hdl,"default",NVR_PCM_STREAM_CAPTURE);
	audio_set_hw_params(audio_lib_hdl.record_hdl.record_pcm_hdl,param,&chunk_size,&buffer_size);
	audio_set_sw_params(audio_lib_hdl.record_hdl.record_pcm_hdl,chunk_size,0);
	memcpy(&audio_lib_hdl.record_hdl.param,param,sizeof(set_audioparams));//参数备份
	return 0;
}
int audio_record_exit(void)
{
	if(audio_lib_hdl.record_hdl.record_pcm_hdl!=NULL)
	{
		snd_pcm_drop(audio_lib_hdl.record_hdl.record_pcm_hdl);
		snd_pcm_drain(audio_lib_hdl.record_hdl.record_pcm_hdl);
		snd_pcm_close(audio_lib_hdl.record_hdl.record_pcm_hdl);
		audio_lib_hdl.record_hdl.record_pcm_hdl=NULL;
	}
	return 0;
}
int audio_record_get_stram(char*buf,unsigned int len)
{
	int nframe=0,frame_len,ret=0;
	set_audioparams *param=&audio_lib_hdl.record_hdl.param;
	frame_len=(param->bits>>3)*param->channels;//每一帧的长度
	nframe=len/frame_len;//有多少帧	
	if((ret=audio_record_get_data(audio_lib_hdl.record_hdl.record_pcm_hdl,(unsigned char *)buf,nframe)))
	{
		AUDIO_PF("audio_record_get_data error");
	}
	return ret;
}
int audio_record_get_data(snd_pcm_t *pcmhandle, unsigned char *buffer, unsigned long nframe)
{
	int res = 0;
	do
	{
		res = snd_pcm_readi(pcmhandle,buffer,nframe);//返回帧数
	} while (res == -EAGAIN);
	if (res < 0) 
	{
		AUDIO_PF("snd_pcm_readi read data error :%s",snd_strerror(res));
		return -1;
	}
	return 0;
}


int audio_playback_exit(void)
{
	if(audio_lib_hdl.play_hdl.play_pcm_hdl!=NULL)
	{
		snd_pcm_drop(audio_lib_hdl.play_hdl.play_pcm_hdl);
		snd_pcm_drain(audio_lib_hdl.play_hdl.play_pcm_hdl);
		snd_pcm_close(audio_lib_hdl.play_hdl.play_pcm_hdl);		
		audio_lib_hdl.play_hdl.play_pcm_hdl=NULL;
	}
	return 0;
}
int audio_get_playback_state(void)
{
	return snd_pcm_state(audio_lib_hdl.play_hdl.play_pcm_hdl);// == SND_PCM_STATE_XRUN
}
int audio_playback_init(set_audioparams *param)
{
	//unsigned int buf_len;	
	snd_pcm_uframes_t chunk_size,buffer_size;
	audio_open_volume("default");
	audio_open_dev(&audio_lib_hdl.play_hdl.play_pcm_hdl,"default",NVR_PCM_STREAM_PLAYBACK);
	audio_set_hw_params(audio_lib_hdl.play_hdl.play_pcm_hdl,param,&chunk_size,&buffer_size);
	audio_set_sw_params(audio_lib_hdl.play_hdl.play_pcm_hdl,chunk_size,buffer_size);
	memcpy(&audio_lib_hdl.play_hdl.param,param,sizeof(set_audioparams));//参数备份
	return 0;
}
int audio_playback_put_stram(char*buf,unsigned int len)
{
	int nframe=0,frame_len;
	set_audioparams *param=&audio_lib_hdl.play_hdl.param;
	frame_len=(param->bits>>3)*param->channels;//每一帧的长度
	nframe=len/frame_len;//有多少帧	
//	AUDIO_PF("frame len:%d,nframe:%d\r\n",frame_len,nframe);
	audio_put_dat(audio_lib_hdl.play_hdl.play_pcm_hdl,buf,nframe,frame_len);
	return 0;
}
static int audio_put_dat(snd_pcm_t *pcmhandle, char * buffer, int nframe,int frame_len)
{
	int write_nframe = 0;
	assert(buffer);
	while (nframe > 0)
	{
			write_nframe = snd_pcm_writei(pcmhandle, buffer, nframe);//返回帧数
		   if (write_nframe == -EAGAIN)
			{
				continue;
		   	}
		   if (write_nframe < 0)
			{
			   AUDIO_PF("snd_pcm_writei ret:%d error:%s",write_nframe,snd_strerror(write_nframe));
			   write_nframe=snd_pcm_prepare(pcmhandle);
			   if(write_nframe<0)			   
			   {
				   AUDIO_PF("snd_pcm_writei ret:%d error:%s",write_nframe,snd_strerror(write_nframe));
				   return write_nframe;
			   }
			   AUDIO_PF("prepare sta:%d",snd_pcm_state(pcmhandle));
			   //break;
			}
			else
			{
			   buffer += write_nframe * frame_len;
			   nframe -= write_nframe;
			}
	}
	return nframe;
}

static int audio_get_audio_params(snd_pcm_hw_params_t *hwparams)
{
	int dir=0;
	int ret;
//	int i;		
	unsigned long period_size; //一个音频处理周期的长度，以桢为单位
	unsigned int period_time;  //一个音频处理周期的时间，以us为单位
	unsigned int periods_buffer;//一个buffer包括多少个音频周期
	if((ret = snd_pcm_hw_params_get_periods(hwparams, &periods_buffer, &dir)) < 0)
	{
		AUDIO_PF("snd_pcm_hw_params_get_periods");
		return(-1);
	}
	AUDIO_PF("per buffer periods = %d nums",periods_buffer);
	
	if((ret = snd_pcm_hw_params_get_period_size(hwparams, &period_size, &dir)) < 0)
	{
		AUDIO_PF("snd_pcm_hw_params_get_period_size");
		return(-1);
	}
	AUDIO_PF("per period = %lu frames",period_size);

	if((ret = snd_pcm_hw_params_get_period_time(hwparams, &period_time, &dir)) < 0)
	{
		AUDIO_PF("snd_pcm_hw_params_get_period_time");
		return(-1);
	}
	AUDIO_PF("per period time = %u",period_time);

	return 0;
}

static int audio_set_sw_params(snd_pcm_t *pcmhandle,snd_pcm_uframes_t chunk_size,snd_pcm_uframes_t buffer_size)
{
	snd_pcm_sw_params_t *swparams;
//	int n;
//	snd_pcm_uframes_t start_threshold, stop_threshold;
	snd_pcm_sw_params_alloca(&swparams);
	snd_pcm_sw_params_current(pcmhandle, swparams);
#if 1
	if(snd_pcm_sw_params_set_avail_min(pcmhandle, swparams,chunk_size))
	{
		AUDIO_PF("snd_pcm_sw_params_set_avail_min error");
		return -1;
	}
#endif
	/*n = buffer_size;
	if (start_delay <= 0)
	{
		start_threshold = n + (double) rate * start_delay / 1000000;
	}
	else
	{
		start_threshold = (double) rate * start_delay / 1000000;
	}
	start_threshold=n;
	if (start_threshold < 1)
	{
		start_threshold = 1;
	}
	if (start_threshold > n)
	{
		start_threshold = n;
	}*/
	if(snd_pcm_sw_params_set_start_threshold(pcmhandle, swparams,buffer_size))
	{
		AUDIO_PF("snd_pcm_sw_params_set_start_threshold error");
		return -1;
	}
#if 1
	/*if (stop_delay <= 0) 
		stop_threshold = buffer_size + (double) rate * stop_delay / 1000000;
	else
		stop_threshold = (double) rate * stop_delay / 1000000;

	stop_threshold=n;*/
	if(buffer_size&&snd_pcm_sw_params_set_stop_threshold(pcmhandle, swparams,buffer_size))
	{
		AUDIO_PF("snd_pcm_sw_params_set_start_threshold error");
		return -1;
	}
#endif
	if (snd_pcm_sw_params(pcmhandle, swparams) < 0)
	{
		AUDIO_PF("snd_pcm_sw_params error");
		return (-1);
	}
	return 0;
}
static int audio_set_hw_params(snd_pcm_t *pcmhandle,set_audioparams *param,snd_pcm_uframes_t *chunk_size,snd_pcm_uframes_t *buffer_size)
{
	int ret=0;
//	int dir = 0;
	snd_pcm_hw_params_t *hwparams;
	size_t bits_per_sample, bits_per_frame;
	size_t chunk_bytes;	
	unsigned int rate = param->rate;
	unsigned int exact_rate = rate;
	unsigned int buffer_time,period_time;
	snd_pcm_uframes_t period_frames = 0;
	snd_pcm_uframes_t buffer_frames = 0;
	if((ret = snd_pcm_hw_params_malloc(&hwparams)) < 0)
	{
		AUDIO_PF("snd_pcm_hw_params_malloc error");
		return(-1);
	}

	if((ret = snd_pcm_hw_params_any(pcmhandle, hwparams)) < 0)
	{
		AUDIO_PF("snd_pcm_hw_params_any error");
		goto audio_set_params_error;
	}

	if((ret = snd_pcm_hw_params_set_access(pcmhandle,hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
	{
		AUDIO_PF("snd_pcm_hw_params_set_access error");
		goto audio_set_params_error;
	}

	if((ret = snd_pcm_hw_params_set_format(pcmhandle, hwparams, param->audioFormat)) < 0)
	{
		AUDIO_PF("snd_pcm_hw_params_set_format:%d",param->audioFormat);
		goto audio_set_params_error;
	}

	if((ret = snd_pcm_hw_params_set_channels(pcmhandle,hwparams, param->channels)) < 0)
	{
		AUDIO_PF("snd_pcm_hw_params_set_channels:%d",param->channels);
		goto audio_set_params_error;
	}

	if((ret = snd_pcm_hw_params_set_rate_near(pcmhandle, hwparams, &exact_rate, 0)) < 0)
	{
		AUDIO_PF("snd_pcm_hw_params_set_rate_near error:%d",param->channels);
		goto audio_set_params_error;
	}
	if(exact_rate != rate)
	{
		AUDIO_PF("not support rate = %dHz,using exact_rate = %dHz instead",rate,exact_rate);
	}
#if 1
	if(snd_pcm_hw_params_get_buffer_time_max(hwparams,&buffer_time, 0))
	{
		AUDIO_PF("snd_pcm_hw_params_get_buffer_time_max error:%d",param->channels);	
		goto audio_set_params_error;
	}
	if (buffer_time > 500000)
	{
		buffer_time = 500000;
	}
	if(buffer_time>0)
	{
		period_time = buffer_time / 4;
	}
	else
	{
		period_frames = buffer_frames / 4;
	}
	AUDIO_PF("buffer_time:%d,period_time:%d",buffer_time,period_time);
	if (period_time > 0)
	{
		if(snd_pcm_hw_params_set_period_time_near(pcmhandle, hwparams,&period_time, 0))
		{
			AUDIO_PF("snd_pcm_hw_params_set_period_time_near error"); 
			goto audio_set_params_error;
		}
	}
	else
	{
		if(snd_pcm_hw_params_set_period_size_near(pcmhandle, hwparams,&period_frames, 0))
		{
			AUDIO_PF("snd_pcm_hw_params_set_period_size_near error"); 
			goto audio_set_params_error;
		}
	}
	if (buffer_time > 0)
	{
		if(snd_pcm_hw_params_set_buffer_time_near(pcmhandle, hwparams,&buffer_time, 0))
		{
			AUDIO_PF("snd_pcm_hw_params_set_buffer_time_near error"); 
			goto audio_set_params_error;
		}
	}
	else 
	{
		if(snd_pcm_hw_params_set_buffer_size_near(pcmhandle, hwparams,&buffer_frames))
		{
			AUDIO_PF("snd_pcm_hw_params_set_buffer_size_near error"); 
			goto audio_set_params_error;
		}
	}
#endif
	if((ret = snd_pcm_hw_params(pcmhandle, hwparams)) < 0)
	{
		AUDIO_PF("snd_pcm_hw_params error");
		goto audio_set_params_error;
	}
	snd_pcm_hw_params_get_period_size(hwparams, chunk_size, 0);
	if(snd_pcm_hw_params_get_buffer_size(hwparams, buffer_size))
	{
		AUDIO_PF("snd_pcm_hw_params_get_buffer_size error");		
		goto audio_set_params_error;
	}
	if (*chunk_size == *buffer_size) 
	{
		AUDIO_PF("Can't use period equal to buffer size (%lu == %lu)",*chunk_size, *buffer_size);
		ret=-1;
		goto audio_set_params_error;
	}
	audio_get_audio_params(hwparams);
	bits_per_sample = snd_pcm_format_physical_width(param->audioFormat);
	bits_per_frame = bits_per_sample * param->channels;
	chunk_bytes = *chunk_size * bits_per_frame / 8;
	AUDIO_PF("chunk_bytes:%d,chunk_size:%d,buffer_size:%d",(int)chunk_bytes,(int)*chunk_size,(int)*buffer_size);
audio_set_params_error:
	snd_pcm_hw_params_free(hwparams);
	return ret;
}

static int audio_open_dev(snd_pcm_t **phandle,const char *name, nvr_pcm_stream_t mode)
{
	int ret;
	if((ret = snd_pcm_open(phandle, name, mode, 0/*SND_PCM_NONBLOCK*/)) < 0)//阻塞
	{
		AUDIO_PF("%s :%d :error code %s",__func__,__LINE__,snd_strerror(ret));
		return(-1);
	}
	return 0;
}

static int audio_open_volume(char *card)
{
	int i;
	int numId = 2;
	char *params[2];
	int ret;

	for(i=0; i<sizeof(audioParams)/sizeof(audioParams[0]); i++)
	{
		params[0] = audioParams[i];
		params[1] = "on";
		ret = cset(numId, params, 0, 0, card);
		if(ret != 0)
		{
			 AUDIO_PF("<<<<<<%s [%d]: cset failed>>>>>>>",__func__,__LINE__);
			 return ret;
		}
	}
	return ret;
}
static int ignore_error = 0;
static int cset(int argc, char *argv[], int roflag, int keep_handle, char *card)//0,0
{
	int err;
	static snd_ctl_t *handle = NULL;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_value_t *control;
	snd_ctl_elem_info_alloca(&info);
	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_value_alloca(&control);

	if (argc < 1) {
		AUDIO_PF("Specify a full control identifier: [[iface=<iface>,][name='name',][index=<index>,][device=<device>,][subdevice=<subdevice>]]|[numid=<numid>]");
		return -1;
	}
	if (snd_ctl_ascii_elem_id_parse(id, argv[0])) 
	{
		AUDIO_PF("Wrong control identifier: %s", argv[0]);
		return -1;
	}
	if (handle == NULL &&
	    (err = snd_ctl_open(&handle, card, 0)) < 0) 
	    {
		AUDIO_PF("Control %s open error: %s", card, snd_strerror(err));
		return err;
	}
	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(handle, info)) < 0) 
	{
		AUDIO_PF("Cannot find the given element from control %s", card);
		if (! keep_handle) 
		{
			snd_ctl_close(handle);
			handle = NULL;
		}
		return err;
	}
	snd_ctl_elem_info_get_id(info, id);     /* FIXME: Remove it when hctl find works ok !!! */
	if (!roflag) {
		snd_ctl_elem_value_set_id(control, id);
		if ((err = snd_ctl_elem_read(handle, control)) < 0) 
		{
			AUDIO_PF("Cannot read the given element from control %s", card);
			if (! keep_handle) 
			{
				snd_ctl_close(handle);
				handle = NULL;
			}
			return err;
		}
		err = snd_ctl_ascii_value_parse(handle, control, info, argv[1]);
		if (err < 0) 
		{
 			if (!ignore_error)
				AUDIO_PF("Control %s parse error: %s", card, snd_strerror(err));
			if (!keep_handle) 
			{
				snd_ctl_close(handle);
				handle = NULL;
			}
			return ignore_error ? 0 : err;
		}
		if ((err = snd_ctl_elem_write(handle, control)) < 0) 
		{
			if (!ignore_error)
				AUDIO_PF("Control %s element write error: %s", card, snd_strerror(err));
			if (!keep_handle) 
			{
				snd_ctl_close(handle);
				handle = NULL;
			}
			return ignore_error ? 0 : err;
		}
	}
	if (! keep_handle)
	{
		snd_ctl_close(handle);
		handle = NULL;
	}
	return 0;
}
