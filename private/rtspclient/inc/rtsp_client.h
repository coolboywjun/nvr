#ifndef __JXJ_RTSP_CLIENT_H__
#define __JXJ_RTSP_CLIENT_H__

#ifdef  __cplusplus
extern "C" {
#endif

typedef enum RtspFrameType
{
	RTSP_FRAME_UNKNOW = 0,
	RTSP_VIDEO_FRAME_I = 0x7,
	RTSP_VIDEO_FRAME_P = 0x8,
	RTSP_VIDEO_FRAME_B = 0xB,
	RTSP_AUDIO_FRAME_A = 0xA, 
}RtspFrameType;

typedef enum RtspEncodeType
{
	RTSP_VIDEO_H264 = 0,
	RTSP_VIDEO_JPEG,
	RTSP_VIDEO_MPEG,
	RTSP_AUDIO_G711A,
	RTSP_AUDIO_G722U,
	RTSP_AUDIO_PCM,
}RtspEncodeType;


typedef struct RtspVideoFrameInfo
{
	unsigned int 		width;
	unsigned int 		height;
	RtspEncodeType	encodeType;
	unsigned int		frameRate;
}RtspVideoFrameInfo;

typedef struct RtspAudioFrameInfo
{
	unsigned int 		audioSample;		//采样率		0--8k 1--16k 2-32k
	unsigned int 		encodeType;		//音频编码格式0--pcm 1-g711a 2-g711u 3--g726
	unsigned int		audioChannel;	//通道数		暂只支持1	
	unsigned int		audioBits;		//位数			16bit
}RtspAudioFrameInfo;

typedef struct RtspFrameInfo
{
	unsigned char 	*frameBuffer;
	unsigned long 	frameSize;
	RtspFrameType 	frameType;
	unsigned long 	frameNo;
	unsigned long long	timestamp;
	union
	{
		RtspVideoFrameInfo videoFrameInfo;
		RtspAudioFrameInfo audioFrameInfo;
	};
}RtspFrameInfo;

typedef int (*RtspClientFrameCb)(RtspFrameInfo *rtspFrameInfo, void *rtspUserData);
typedef int (*RtspClientBeginFrameCb)(void *rtspUserData);
typedef int (*RtspClientEndFrameCb)(void *rtspUserData);

int rtspClientOpenUrl(char const* rtspURL, const char *userName, const char *userPasswd, 
					int bKeepAlive, RtspClientFrameCb frameCb, 
					RtspClientBeginFrameCb beginFrameCb, RtspClientEndFrameCb endFrameCb,
					void *rtspUserData, long *rtspHandle);

void rtspClientCloseUrl(long rtspHandle);

void rtspClientRequestIFrame(long rtspHandle);

int rtspClientInit();

void rtspClientUninit();

#ifdef __cplusplus
}
#endif


#endif

