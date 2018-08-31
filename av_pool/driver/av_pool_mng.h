
#ifndef __AV_POOL_MNG_H__
#define __AV_POOL_MNG_H__

#include <linux/mutex.h>

#include "av_pool_module.h"

// 最大支持通道数, 远程ipnc通道加一个本机通道
#define AV_POOL_MAX_SUPPORT_CHN		5
// 每个通道最大码流数，主码流，次码流，音频
#define AV_POOL_MAX_SUPPORT_STREAM	2

#define AV_POOL_MAX_SUPPORT_POOL (AV_POOL_MAX_SUPPORT_CHN * AV_POOL_MAX_SUPPORT_STREAM)

typedef enum _JPOOL_ERR_E_
{
	JPOOL_SUCCESS   		= 0,
	JPOOL_FAILED			= -1,
}JPOOL_ERR_E;

//因为设计帧占空间大小为64字节对齐
//所以在缓存里的帧标识头只支持最大11 个NAL包的帧
#define JPOOL_HDR_LEN 64
typedef struct _JPOOL_FRAME_HDR_
{
	unsigned long	u32FlagStart;		//帧头魔数
	unsigned long	u32FrameNo;//视频帧序号
	unsigned long	u32DataLen; //视频帧数据大小
	unsigned char	u8FrameType;
	unsigned char	u8Res[2];
	unsigned char	u8NalNum;
	unsigned long long	u64Pts;
	unsigned long	u32ArrNalLen[9];
	unsigned long	u32FlagEnd;
}JPOOL_FRAME_HDR, *LPJPOOL_FRAME_HDR; 


typedef struct _JPOOL_MAN_
{
	unsigned char		u8Init;
	unsigned char		u8VideoPool;//--1 表示为视频缓存--0 表示为音频缓存
	unsigned char		u8PoolNo;
	unsigned char		u8Res;
	unsigned long		u32FrameNo;		//缓存里最大帧号
	unsigned long		u32BuffSize;			//缓存大小
	unsigned long		u32MaxSize;			//缓存里最大的帧大小
	unsigned long		u32OldestFrameNo;
	unsigned long		u32OldestIFrameNo;
	
	unsigned long		u32OldestFramePos;	//缓存里最老帧的位置	
	unsigned long		u32OldestIFramePos;
	unsigned long		u32NewestIPos;		//缓存里最新I 帧的位置
	unsigned long		u32NewestIFrameNo;	// 缓存里最新I帧的帧号
	unsigned long		u32NewestFramePos;//缓存里最新帧的位置

	unsigned long long		u64OldestFramePts;	//缓存里最老帧的pts	
	unsigned long long		u64OldestIFramePts;	//缓存里最老帧I 帧的pts	
	unsigned long long		u64NewestIPts;		//缓存里最新I 帧的pts	
	
	unsigned long long		u64NewestFramePts;//缓存里最新帧的pts
	
	unsigned long		u32WritePos;

	unsigned long		u32NewestFrameNo;		// 最新帧号
	unsigned long		u32PhysAddr;
	unsigned long		u32MaxHdrLen;
	char 		*pDataBuff;

	// pool infomation
	char ipAddr[32];		// ipc ip address of the stream
	char protocol[32];		// ipc protocol name
	int port;				// port of the stream
	int type;				// tcp or udp of the stream

	// handle
	long ipncHandle;
	long streamHandle;
	
	// pool status
	AvPoolWorkStatus status;
	atomic_t readCnt;
	atomic_t writeCnt;

	unsigned int readFrameCnt;
	unsigned int writeFrameCnt;
	
	//int dmaChid;
	//struct completion dmaComplete;
	struct completion frameComplete;
	wait_queue_head_t wait;
	
//	struct mutex	lock;
	struct rw_semaphore sem;
}JPOOL_MAN, *LPJPOOL_MAN;

typedef struct _JPOOL_ATTR_
{
	int 	nMsPoolSize;	//主码流缓冲区大小
	int	nSsPoolSize;//次码流缓冲区大小
	int	nAsPoolSize;//音频缓冲区大小
}JPOOL_ATTR, *LPJPOOL_ATTR;


int av_pool_init(int nTotalCh, int nTotalStreamNo);

int av_pool_uninit(void);

int av_pool_write_frame(int nCh, int nStreamNo, char *pHdrData, int nHdrLen, 
					int nPics, char **ppPicsData, int *pPicsLen,  int nLen, int nFrameType,
					unsigned long long u64Pts, unsigned long u32CurFrameNo);

int av_pool_read_frame(int nCh, int nStreamNo, char *pBuf, int nBufLen, unsigned long *pU32IFrame,
			unsigned long u32NeedFrameNo, unsigned long *pU32FrameNo, unsigned long *pLastPos, unsigned long long *pU64Pts, 
			unsigned long	u32MaxDelay, unsigned long *pU32NewestFrameNo, int timeout);

int av_pool_get_max_frame_size(int nCh, int nStreamNo);

int av_pool_reset_pool(int nCh, int nStreamNo);

int av_pool_set_pool_info(int nCh, 
			const char *ipAddr, const char *protocol, 
			int type, int port);

int av_pool_get_pool_info(int nCh, 
			char *ipAddr, char *protocol, 
			int *type, int *port);
			
int av_pool_get_frame_info(int nCh, int nStreamNo, 
			unsigned int *writeFrameCnt, unsigned int *readFrameCnt);

int av_pool_set_pool_status(int nCh, int nStreamNo, AvPoolWorkStatus status);

int av_pool_get_pool_status(int nCh, int nStreamNo, AvPoolWorkStatus *status);


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

#endif

