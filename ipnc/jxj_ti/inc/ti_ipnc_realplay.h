#ifndef __TI_IPNC_REALPLAY_H__
#define __TI_IPNC_REALPLAY_H__

//typedef struct _TI_IPNC_PROTOCOL_MNG_ TI_IPNC_PROTOCOL_MNG;


typedef struct _TI_IPNC_STREAM_S_
{
	int chn;
	int streamNo;
	int poolChn;
	int poolStreamNo;
	long lRealPlayHandle;
	long videoId;
	int refCount;

	unsigned long preVideoFrameNo;
	unsigned long long lastVideoPts;	// 最后视频时间戳
	unsigned long long lastAudioPts;	// 最后音频时间戳
	
	void *ipncHandle;
}TI_IPNC_STREAM_S;



int ti_ipnc_start_get_stream(long handle, int chn, int streamNo, long *streamHandle);
int ti_ipnc_stop_get_stream(long handle, long streamHandle);
int ti_ipnc_request_iframe(long handle, long streamHandle);



#endif

