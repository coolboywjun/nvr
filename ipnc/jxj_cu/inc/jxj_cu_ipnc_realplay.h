
#ifndef __JXJ_CU_REALPLAY_H__
#define __JXJ_CU_REALPLAY_H__

typedef struct _JXJ_CU_STREAM_S_
{
	int chn;			  //ipc通道号
	int streamNo;		  //主次码流(0主码流, 1次码流)
	int poolChn;		  //avpool通道号
	int poolStreamNo; 	  //码流号
	long lRealPlayHandle; //rtsp句柄
	long videoId;		  //avpool句柄ID
	int refCount;		  //连接计数
	unsigned long long lastVideoPts;	// 最后视频时间戳
	unsigned long long lastAudioPts;	// 最后音频时间戳
	
	void *ipncHandle;	  //ipnc 句柄
	pthread_t pthId;	  //取流重连线程
}JXJ_CU_STREAM_S;


int jxj_cu_ipnc_start_get_stream(long handle, int chn, int streamNo, long *streamHandle);
int jxj_cu_ipnc_stop_get_stream(long handle, long streamHandle);
int jxj_cu_ipnc_request_iframe(long handle, long streamHandle);

#endif





