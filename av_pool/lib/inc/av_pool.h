#ifndef __AV_POOL_H__
#define __AV_POOL_H__

#include <errno.h>
#include <netinet/in.h>

typedef enum _AV_FRAME_TYPE_E_
{
	AV_VIDEO_FRAME_I = 0x7,
	AV_VIDEO_FRAME_P = 0x8,
	AV_VIDEO_FRAME_B = 0xB,
	AV_AUDIO_FRAME_A = 0xA, 
}AV_FRAME_TYPE_E;

typedef struct _AV_VIDEO_INFO_S
{
	unsigned short			u16ImageWidth;	//图像宽度
	unsigned short			u16ImageHeight;	//图像高度
	unsigned char				u8EncodeType;	//视频编码格式0-h264 1-mjpeg 2-jpeg
	unsigned char				u8FrameRate;	//帧率(保留)
	unsigned char				u8Pal;			//制式		0-n制1-pal制
	unsigned char				u8Res[1];
}AV_VIDEO_INFO_S,*LPAV_VIDEO_INFO_S;

typedef struct _AV_AUDIO_INFO_S
{
	unsigned char		u8AudioSamples;			//采样率		0--8k 1--16k 2-32k
	unsigned char		u16EncodeType;			//音频编码格式0--pcm 1-g711a 2-g711u 3--g726
	unsigned char		u8AudioChannels;			//通道数		暂只支持1	
	unsigned char		u8AudioBits;				//位数			16bit
	unsigned char		u8Res[4];
}AV_AUDIO_INFO_S, *LPAV_AUDIO_INFO_S;

typedef struct _AV_FRAME_HEAD_S
{
	unsigned short	u16FrameFlag;		//0x3448 magic data
	unsigned char		u8FrameType;		//I-0x7 p--0x8 b--0xb A--0xa
	unsigned char		u8Res[1];			//保留
	unsigned long		u32FrameNo;		//帧号
	unsigned long		u32Time;			//系统时间
	unsigned long		u32FrameSize;		//数据流长度
	unsigned long long	u64TimeStamp;		//时间戳
	union
	{
		AV_VIDEO_INFO_S	unVidoeInfo;
		AV_AUDIO_INFO_S	unAudioInfo;
	};	
}AV_FRAME_HEAD_S, *LPAV_FRAME_HEAD_S;
#define AV_FRAME_HEAD_LEN		(sizeof(AV_FRAME_HEAD_S))

// 最大支持ipnc通道数和一路主机流
#define AV_POOL_MAX_SUPPORT_CHN		5
// 每个通道最大码流数，主码流，次码流
#define AV_POOL_MAX_SUPPORT_STREAM	2
// TALK通道
#define AV_POOL_TALK_CHN	4
#define AV_POOL_TALK_OUT_STREAM	0
#define AV_POOL_TALK_IN_STREAM	1


enum
{
	AV_POOL_START_READ_FRAME = 1,
	AV_POOL_STOP_READ_FRAME,
	AV_POOL_REQUEST_IFRAME,
};

typedef struct _AV_MSG_CHN_INFO_S
{
	long msgType;
	int chn;
	int stream;
}AV_MSG_CHN_INFO_S;

int av_pool_recv_msg(void *pData, int nDataLen);

int av_pool_send_msg(void *pData, int nDataLen);

int av_pool_get_max_frame_size(int nCh, int nStream);

int av_pool_start_write_frame(int nCh, int nStream, long *id);
	
void av_pool_stop_write_frame(long id);

int av_pool_write_frame(long id, unsigned char *frameBuf, unsigned long frameLen, 
							unsigned char *frameHdr, unsigned long hdrLen,
							unsigned long frameNo, int frameType, unsigned  long long pts);
							
int av_pool_start_read_frame(int nCh, int nStreamNo, int nMaxDelay, long *id);

void av_pool_stop_read_frame(long id);

int av_pool_read_frame(long id, char **pBuf, int *pBufLen, int nTimeOut);

int av_pool_request_iframe(int nChn, int nStreamNo);

int av_pool_reset_pool(int nCh, int nStreamNo);

int av_pool_set_pool_info(int nCh, 
			const char *ipAddr, const char *protocol, 
			int type, int port);

int av_pool_get_pool_info(int nCh, 
			char *ipAddr, char *protocol, 
			int *type, int *port);

int av_pool_set_pool_status(int nCh, int nStreamNo, int status);

int av_pool_get_pool_status(int nCh, int nStreamNo, int *status);

int av_pool_get_frame_info(int nCh, int nStreamNo, 
						unsigned int *writeFrameCnt, unsigned int *readFrameCnt);

unsigned int av_pool_is_writing(int nCh, int nStreamNo);

unsigned int av_pool_is_reading(int nCh, int nStreamNo);

int av_pool_inc_pool_reading_cnt(int nCh, int nStreamNo);

int av_pool_dec_pool_reading_cnt(int nCh, int nStreamNo);

int av_pool_get_pool_reading_cnt(int nCh, int nStreamNo);

int av_pool_inc_pool_writing_cnt(int nCh, int nStreamNo);

int av_pool_dec_pool_writing_cnt(int nCh, int nStreamNo);

int av_pool_get_pool_writing_cnt(int nCh, int nStreamNo);

int av_pool_set_pool_ipnc_handle(int nCh, long ipncHandle);
	
int av_pool_get_pool_ipnc_handle(int nCh, long *ipncHandle);

int av_pool_set_pool_stream_handle(int nCh, int nStreamNo, long streamHandle);

int av_pool_get_pool_stream_handle(int nCh, int nStreamNo, long *streamHandle);

int av_pool_set_alarm_status(int nCh, int status);

int av_pool_get_alarm_status(int nCh, int *status);

int av_pool_reset_alarm_status(int nCh);

int av_pool_wait_alarm_change(int nCh, int oldStatus, int *status);

int av_pool_init();

void av_pool_uninit();

#endif
