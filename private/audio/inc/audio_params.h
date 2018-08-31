#ifndef __AUDIO_PARAM_H__
#define __AUDIO_PARAM_H__

#include <stdio.h>
#include <alsa/asoundlib.h>

typedef unsigned long alsa_handle;//上层应用管理打开设备句柄

/** PCM sample format */
typedef enum _alsa_pcm_format {
	/** Unknown */
	ALSA_PCM_FORMAT_UNKNOWN = -1,
	/** Signed 8 bit */
	ALSA_PCM_FORMAT_S8 = 0,
	/** Unsigned 8 bit */
	ALSA_PCM_FORMAT_U8,
	/** Signed 16 bit Little Endian */
	ALSA_PCM_FORMAT_S16_LE,
	/** Signed 16 bit Big Endian */
	ALSA_PCM_FORMAT_S16_BE,
	/** Unsigned 16 bit Little Endian */
	ALSA_PCM_FORMAT_U16_LE,
	/** Unsigned 16 bit Big Endian */
	ALSA_PCM_FORMAT_U16_BE,
	/** Signed 24 bit Little Endian using low three bytes in 32-bit word */
	ALSA_PCM_FORMAT_S24_LE,
	/** Signed 24 bit Big Endian using low three bytes in 32-bit word */
	ALSA_PCM_FORMAT_S24_BE,
	/** Unsigned 24 bit Little Endian using low three bytes in 32-bit word */
	ALSA_PCM_FORMAT_U24_LE,
	/** Unsigned 24 bit Big Endian using low three bytes in 32-bit word */
	ALSA_PCM_FORMAT_U24_BE,
	/** Signed 32 bit Little Endian */
	ALSA_PCM_FORMAT_S32_LE,
	/** Signed 32 bit Big Endian */
	ALSA_PCM_FORMAT_S32_BE,
	/** Unsigned 32 bit Little Endian */
	ALSA_PCM_FORMAT_U32_LE,
	/** Unsigned 32 bit Big Endian */
	ALSA_PCM_FORMAT_U32_BE
}alsa_pcm_format;

/*设置音频参数的结构体*/
typedef struct set_audio_param
{
	unsigned int channels; //设置通道数
	unsigned int rate;   //设置采用频率
	alsa_pcm_format audioFormat; //设置采用格式
	unsigned long  periodFrames; //一个周期的帧数
	unsigned char bits;
}set_audioparams;

/*获取音频参数的结构体*/
typedef struct get_audio_param
{
	unsigned long period_size; //一个音频处理周期的长度，以桢为单位
	unsigned int period_time;  //一个音频处理周期的时间，以us为单位
	unsigned int periods_buffer;//一个buffer包括多少个音频周期
	
}get_audioparams;

/** PCM stream (direction) */
typedef enum nvr_pcm_stream {
	/** 播放音频 */
	NVR_PCM_STREAM_PLAYBACK = 0,
	/** 录制音频 */
	NVR_PCM_STREAM_CAPTURE,
	NVR_PCM_STREAM_LAST = NVR_PCM_STREAM_CAPTURE
} nvr_pcm_stream_t;
typedef struct {
	alsa_handle phandle;
	unsigned long wlen; 
	int pipeFd;
}play_params,*pplay_params;

typedef struct capture_play
{
	snd_pcm_t *pcmhandle;
	snd_pcm_hw_params_t *hwparams;
	alsa_handle uhandle;
	size_t  chunk_bytes;	  //一个周期的字节数
	char  *audiobuf;		  //返回一个音频周期起始地址
	size_t  bits_per_frame;   //每一帧音频数据的bit数
	pthread_t pid;			  //播放线程的pid
	pplay_params param;

}pcapture_play;
extern pcapture_play parray_handle[4];
int audio_playback_init(set_audioparams *param);
int audio_playback_put_stram(char*buf,unsigned int len);
int audio_playback_exit(void);
int audio_record_init(set_audioparams *param);
int audio_record_exit(void);
int audio_record_get_stram(char*buf,unsigned int len);
int audio_get_playback_state(void);
#endif

