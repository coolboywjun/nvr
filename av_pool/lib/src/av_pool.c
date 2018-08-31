#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <errno.h>
#include <syslog.h>
#include <sys/ipc.h>
#include <sys/msg.h> 

#include <av_pool.h>
#include <av_pool_module.h>


#define PRINT(fmt, args...)	\
do{		\
	printf(fmt, ##args);		\
	syslog(LOG_ERR | LOG_USER, "AvPool " fmt, ##args);	\
}while(0);


#define AV_POOL_DEVICE	"/dev/av_pool"


typedef struct _AVPoolRead
{
	int nCh;
	int nStreamNo;
	
	unsigned long Iframe;
	unsigned long needFrameNo;
	unsigned long frameNo;
	unsigned long lastPos;
	unsigned long long pts;
	unsigned long newestFrameNo;
	int nMaxDelay;		// 单位ms
	
	char *pFrameBuf;
	int bufLen;

	unsigned long preVideoFrameNo;
}AVPoolRead;

typedef struct _AVPoolWrite
{
	int nCh;
	int nStreamNo;

	unsigned long preVideoFrameNo;
}AVPoolWrite;


static int av_fd = 0;
//static int init = 0;
static int msg_id = 0;

int av_pool_recv_msg(void *pData, int nDataLen)
{
	int recv_len = 0;
	key_t keyval;
	
	keyval=ftok("/opt/nvr", 0xaa00);
	if(msg_id == 0)
	{
		msg_id = msgget(keyval, IPC_CREAT|0666);
		if(msg_id < 0)
			PRINT("Create msg id failed.\n");
	}
	if((recv_len = msgrcv(msg_id, pData, nDataLen - sizeof(long), 0, 0)) < 0)
	{
		PRINT("Receive msg failed.\n");
		return -1;
	}
	return recv_len;
}

int av_pool_send_msg(void *pData, int nDataLen)
{
	int msg_id;
	key_t keyval;
	
	keyval=ftok("/opt/nvr", 0xaa00);
	msg_id = msgget(keyval, 0);
	if(msg_id < 0)
	{
		PRINT("Get msg id failed.\n");
		return -1;
	}
	if(msgsnd(msg_id, pData, nDataLen-sizeof(long), IPC_NOWAIT) < 0)
	{
		PRINT("Send msg failed.\n");
		return -1;
	}
	return 0;
}

/***********************************************
* 输入参数
* nCh: 通道号
* nStream: 码流号
*
* 返回值:
*  >0, 最大帧大小
* =0, 这个通道没有写入数据
* <0, 获取失败
************************************************/
int av_pool_get_max_frame_size(int nCh, int nStream)
{
	GetMaxFrameSize cmd;
	
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStream > AV_POOL_MAX_SUPPORT_STREAM)
		return -1;

	cmd.chn = nCh;
	cmd.streamNo = nStream;
	cmd.maxFrameSize = 0;
	if(ioctl(av_fd, AV_POOL_GET_MAX_FRAME_SIZE, &cmd) < 0)
	{
		PRINT("Failed to get max frame size.\n");
		return -1;
	}

	return cmd.maxFrameSize;
}

int av_pool_start_write_frame(int nCh, int nStream, long *id)
{
	AVPoolWrite *pool;

	*id = 0;
	pool = (AVPoolWrite*)malloc(sizeof(AVPoolWrite));
	if(pool == NULL)
		return -1;
	memset(pool, 0, sizeof(AVPoolWrite));
	pool->nCh = nCh;
	pool->nStreamNo = nStream;
	if(av_pool_inc_pool_writing_cnt(nCh, nStream) < 0)
	{
		free(pool);
		return -1;
	}
	av_pool_reset_pool(nCh, nStream);
	*id = (long)pool;
	PRINT("Ch %d stream %d start to write frame\n", nCh, nStream);
	
	return 0;
}

void av_pool_stop_write_frame(long id)
{
	AVPoolWrite *pool = (AVPoolWrite*)id;
	if(pool == NULL)
		return;
	PRINT("Ch %d stream %d stop to write frame.\n", pool->nCh, pool->nStreamNo);
	av_pool_dec_pool_writing_cnt(pool->nCh, pool->nStreamNo);
	free(pool);
}

int av_pool_write_frame(long id, unsigned char *frameBuf, unsigned long frameLen, 
							unsigned char *frameHdr, unsigned long hdrLen,
							unsigned long frameNo, int frameType, unsigned  long long pts)
{
	AVPoolWrite *pool;
	AvWriteCmd cmd;
	AV_FRAME_HEAD_S *pFrameHdr;
	//struct timeval tv;
	//long long starttime, endtime;
	
	pool = (AVPoolWrite*)id;
	if(pool == NULL)
		return -1;
		
	cmd.chn = pool->nCh;
	cmd.streamNo = pool->nStreamNo;
	cmd.hdrLen = hdrLen;
	cmd.frameType = frameType;
	cmd.hdrData = frameHdr;
	cmd.frameBuf = frameBuf;
	cmd.frameLen = frameLen;
	cmd.frameNo = frameNo;
	cmd.pts = pts;
	//if(pool->nCh == 0)
	//{
	//	gettimeofday(&tv, NULL);
	//	starttime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	//}
	if(ioctl(av_fd, AV_POOL_WRITE_FRAME, &cmd) < 0)
	{
		PRINT("Failed to write frame data to av pool.\n");
		return -1;
	}
	//if(pool->nCh == 0)
	//{
	//	gettimeofday(&tv, NULL);
	//	endtime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	//	PRINT("write elapse time: %lldms\n", endtime - starttime);
	//}
	// print the frame no
	pFrameHdr = (AV_FRAME_HEAD_S *)frameHdr;
	if(pFrameHdr->u8FrameType != AV_AUDIO_FRAME_A)
	{
		if(pool->preVideoFrameNo != 0 && pool->preVideoFrameNo + 1 != pFrameHdr->u32FrameNo)
			PRINT("Write handle %p Chn %d stream %d write pre frame no: %lu, cur frame no: %lu\n", 
					pool,
					pool->nCh, pool->nStreamNo,
					pool->preVideoFrameNo, pFrameHdr->u32FrameNo);
		pool->preVideoFrameNo = pFrameHdr->u32FrameNo;
	}
	
	return 0;
}

static int __av_pool_read_frame(int nCh, int nStreamNo, char *pBuf, int nBufLen, unsigned long *pU32IFrame,
			unsigned long u32NeedFrameNo, unsigned long *pU32FrameNo, unsigned long *pLastPos, unsigned long long *pU64Pts, 
			unsigned long	u32MaxDelay, unsigned long *pU32NewestFrameNo, int timeout)
{
	volatile AvReadCmd cmd;
	
	//struct timeval tv;
	//long long starttime, endtime;
		
	if (nCh > AV_POOL_MAX_SUPPORT_CHN || nStreamNo > AV_POOL_MAX_SUPPORT_STREAM ||
		pBuf == NULL ||pU32IFrame == NULL || pU32FrameNo == NULL || pLastPos == NULL 
		|| pU64Pts == NULL || nBufLen < 0 || pU32NewestFrameNo == NULL)
	{
		PRINT("input parameter error.\n");
		return -1;
	}	
	
	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;
	cmd.bufLen = nBufLen;
	cmd.frameBuf = (unsigned char *)pBuf;
	cmd.frameLen = 0;
	cmd.needIFrame = *pU32IFrame;
	cmd.needFrameNo = u32NeedFrameNo;
	cmd.frameNo = *pU32FrameNo;
	cmd.lastPos = *pLastPos;
	cmd.pts = *pU64Pts;
	cmd.maxDelay = u32MaxDelay;
	cmd.newestFrameNo = 0;
	cmd.timeout = timeout;
	//{
	//	gettimeofday(&tv, NULL);
	//	starttime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	//}
	if(ioctl(av_fd, AV_POOL_READ_FRAME, &cmd) < 0)
	{
		//PRINT("Failed to ioctl av pool.\n");
		return -1;
	}
	//{
	//	gettimeofday(&tv, NULL);
	//	endtime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	//	PRINT("read elapse time: %lldms\n", endtime - starttime);
	//}
	if(cmd.frameLen > 0)
	{
		//PRINT("frameSize: %d\n", cmd.frameLen);
		//PRINT("needIFrame: %lu\n", cmd.needIFrame);
		//PRINT("frameNo: %lu\n", cmd.frameNo);
		*pU32IFrame = cmd.needIFrame;
		*pU32FrameNo = cmd.frameNo;
		*pLastPos = cmd.lastPos;
		*pU64Pts = cmd.pts;
		*pU32NewestFrameNo = cmd.newestFrameNo;
	}
	//else
	//{
	//	PRINT("Last pos: %lu, cmd pos: %lu\n", *pLastPos, cmd.lastPos);
	//}
	return cmd.frameLen;
}

int av_pool_start_read_frame(int nCh, int nStreamNo, int nMaxDelay, long *id)
{
	AVPoolRead *pool;
	AV_MSG_CHN_INFO_S msg;
	
	*id = 0;
	pool = (AVPoolRead*)malloc(sizeof(AVPoolRead));
	if(pool == NULL)
		return -1;
	memset(pool, 0, sizeof(*pool));
	pool->nCh = nCh;
	pool->nStreamNo = nStreamNo;
	pool->nMaxDelay = nMaxDelay;
	
	pool->Iframe = 1;

	if(av_pool_inc_pool_reading_cnt(nCh, nStreamNo) < 0)
	{
		free(pool);
		return -1;
	}
	*id = (long)pool;

	msg.msgType = AV_POOL_START_READ_FRAME;
	msg.chn = nCh;
	msg.stream = nStreamNo;
	av_pool_send_msg(&msg, sizeof(msg));
	PRINT("Ch %d stream %d start to read frame.\n", nCh, nStreamNo);
	
	return 0;
}

void av_pool_stop_read_frame(long id)
{
	AVPoolRead *pool = (AVPoolRead *)id;
	AV_MSG_CHN_INFO_S msg;
	if(pool == NULL)
		return;
	PRINT("Ch %d stream %d stop to read frame.\n", pool->nCh, pool->nStreamNo);
	
	msg.msgType = AV_POOL_STOP_READ_FRAME;
	msg.chn = pool->nCh;
	msg.stream = pool->nStreamNo;
	av_pool_send_msg(&msg, sizeof(msg));
	
	av_pool_dec_pool_reading_cnt(pool->nCh, pool->nStreamNo);
	if(pool->pFrameBuf != NULL)
		free(pool->pFrameBuf);
	free(pool);
}

int av_pool_read_frame(long id, char **pBuf, int *pBufLen, int nTimeOut)
{
	AVPoolRead *pool = (AVPoolRead*)id;
	AV_FRAME_HEAD_S *pFrameHdr;
	int nMaxFrameLen;
	int nFrameSize;
	char *pFrameBuf;
	int bufLen;
	
	if(*pBuf != NULL && *pBufLen > 0)
	{
		pFrameBuf = *pBuf;
		bufLen =  *pBufLen;
	}
	else
	{
		if(pool->bufLen == 0 || pool->pFrameBuf == NULL)
		{
			pool->bufLen = 256 * 1024;
			pool->pFrameBuf = malloc(pool->bufLen);
			if(pool->pFrameBuf == NULL)
			{
				pool->bufLen = 0;
				return -ENOMEM;
			}
		}
		pFrameBuf = pool->pFrameBuf;
		bufLen = pool->bufLen;
	}

	// 强制读取I帧
	if(nTimeOut == -1)
	{	
		pool->Iframe = 1;
		pool->needFrameNo = ~0;
		pool->frameNo = 0;
		pool->lastPos = ~0;
		pool->pts = 0;
		pool->newestFrameNo = 0;
	}
	if((nFrameSize = __av_pool_read_frame(pool->nCh,
								pool->nStreamNo,
								pFrameBuf,
								bufLen,
								&pool->Iframe,
								pool->needFrameNo,
								&pool->frameNo,
								&pool->lastPos,
								&pool->pts,
								pool->nMaxDelay,
								&pool->newestFrameNo,
								nTimeOut)) <= 0)
	{
		if(errno == ENOMEM)
		{
			nMaxFrameLen = av_pool_get_max_frame_size(pool->nCh, pool->nStreamNo);
			if(*pBuf != NULL && *pBufLen < nMaxFrameLen)
				return -ENOMEM;
			if(pool->bufLen < nMaxFrameLen)
			{
				if(pool->pFrameBuf != NULL)
					free(pool->pFrameBuf);
				pool->bufLen = (nMaxFrameLen + 0x10000) & (~(0x10000 - 1));
				pool->pFrameBuf = malloc(pool->bufLen);
				if(pool->pFrameBuf == NULL)
				{
					pool->bufLen = 0;
					return -ENOMEM;
				}
			}
		}
		return -EAGAIN;
	}
	pool->Iframe = 0;
	pool->needFrameNo = pool->frameNo + 1;
	// print the frame no
	pFrameHdr = (AV_FRAME_HEAD_S *)pool->pFrameBuf;
	if(pFrameHdr->u8FrameType != AV_AUDIO_FRAME_A)
	{
		if(pool->preVideoFrameNo != 0 && pool->preVideoFrameNo + 1 != pFrameHdr->u32FrameNo)
			PRINT("Read handle %p Chn %d stream %d read pre frame no: %lu, cur frame no %lu, need frame no: %lu, frame type: %u\n", 
				pool,
				pool->nCh, pool->nStreamNo,
				pool->preVideoFrameNo, pFrameHdr->u32FrameNo, 
				pool->frameNo, pFrameHdr->u8FrameType);
		pool->preVideoFrameNo = pFrameHdr->u32FrameNo;
		//printf("timestamp: %llu\n", pFrameHdr->u64TimeStamp / 1000);
	}
	if(*pBuf == NULL)
	{
		*pBuf = pool->pFrameBuf;
		*pBufLen = pool->bufLen;
	}
	
	return nFrameSize;
}

int av_pool_request_iframe(int nChn, int nStreamNo)
{
	AV_MSG_CHN_INFO_S msg;
	
	msg.msgType = AV_POOL_REQUEST_IFRAME;
	msg.chn = nChn;
	msg.stream = nStreamNo;
	
	return av_pool_send_msg(&msg, sizeof(msg));
}

int av_pool_reset_pool(int nCh, int nStreamNo)
{
	AvPoolId cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;

	if(ioctl(av_fd, AV_POOL_RESET_POOL, &cmd) < 0)
		return -1;

	PRINT("Ch %d stream %d reset pool.\n", nCh, nStreamNo);
	
	return 0;
}

int av_pool_set_pool_info(int nCh, 
			const char *ipAddr, const char *protocol, 
			int type, int port)
{
	AvPoolInfo cmd;

	PRINT("Ch %d set pool info, ip %s protocol %s type %d port %d\n",
			nCh, ipAddr, protocol, type, port);
	cmd.chn = nCh;
	strncpy(cmd.ipAddr, ipAddr, sizeof(cmd.ipAddr)-1);
	cmd.ipAddr[sizeof(cmd.ipAddr) - 1] = '\0';
	strncpy(cmd.protocol, protocol, sizeof(cmd.protocol)-1);
	cmd.protocol[sizeof(cmd.protocol) - 1] = '\0';
	cmd.type = type;
	cmd.port = port;
	return ioctl(av_fd, AV_POOL_SET_POOL_INFO, &cmd);
}

int av_pool_get_pool_info(int nCh, 
			char *ipAddr, char *protocol, 
			int *type, int *port)
{
	AvPoolInfo cmd = {0};

	cmd.chn = nCh;
	if(ioctl(av_fd, AV_POOL_GET_POOL_INFO, &cmd) < 0)
		return -1;
	if(ipAddr  != NULL)
		strcpy(ipAddr, cmd.ipAddr);
	if(protocol != NULL)
		strcpy(protocol, cmd.protocol);
	if(type != NULL)
		*type = cmd.type;
	if(port != NULL)
		*port = cmd.port;
	return 0;
}

int av_pool_set_pool_status(int nCh, int nStreamNo, int status)
{
	AvPoolStaus cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;
	cmd.status = status;

	return ioctl(av_fd, AV_POOL_SET_POOL_STATUS, &cmd);
}

int av_pool_get_pool_status(int nCh, int nStreamNo, int *status)
{
	AvPoolStaus cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;
	cmd.status = 0;

	if(ioctl(av_fd, AV_POOL_GET_POOL_STATUS, &cmd) < 0)
		return -1;
	*status = cmd.status;
	return 0;
}

int av_pool_get_frame_info(int nCh, int nStreamNo, 
						unsigned int *writeFrameCnt, unsigned int *readFrameCnt)
{
	AvPoolFrameInfo cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;

	if(ioctl(av_fd, AV_POOL_GET_FRAME_INFO, &cmd) < 0)
		return -1;
	if(writeFrameCnt != NULL)
		*writeFrameCnt = cmd.writeFrameCnt;
	if(readFrameCnt != NULL)
		*readFrameCnt = cmd.readFrameCnt;
		
	return 0;
}

unsigned int av_pool_is_writing(int nCh, int nStreamNo)
{
	unsigned int writeFrameCnt = 0;
	int writeCnt = 0;
	if(av_pool_get_frame_info(nCh, nStreamNo, &writeFrameCnt, NULL) < 0)
		return 0;
	writeCnt = av_pool_get_pool_writing_cnt(nCh, nStreamNo);
	if(writeCnt > 0 && writeFrameCnt > 0)
		return 1;
	return 0;
}

unsigned int av_pool_is_reading(int nCh, int nStreamNo)
{
	unsigned int readFrameCnt;
	int readCnt;
	if(av_pool_get_frame_info(nCh, nStreamNo, NULL, &readFrameCnt) < 0)
		return 0;
	readCnt = av_pool_get_pool_reading_cnt(nCh, nStreamNo);
	if(readCnt > 0 && readFrameCnt > 0)
		return 1;
	return 0;
}

int av_pool_inc_pool_reading_cnt(int nCh, int nStreamNo)
{
	AvPoolId cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;

	if(ioctl(av_fd, AV_POOL_INC_READING_STATUS, &cmd) < 0)
		return -1;
	return 0;
}

int av_pool_dec_pool_reading_cnt(int nCh, int nStreamNo)
{
	AvPoolId cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;

	if(ioctl(av_fd, AV_POOL_DEC_READING_STATUS, &cmd) < 0)
		return -1;
	return 0;
}

int av_pool_get_pool_reading_cnt(int nCh, int nStreamNo)
{
	AvPoolCnt cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;

	if(ioctl(av_fd, AV_POOL_GET_READING_STATUS, &cmd) < 0)
		return -1;
	return cmd.cnt;
}

int av_pool_inc_pool_writing_cnt(int nCh, int nStreamNo)
{
	AvPoolId cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;

	if(ioctl(av_fd, AV_POOL_INC_WRITING_STATUS, &cmd) < 0)
		return -1;
	return 0;
}

int av_pool_dec_pool_writing_cnt(int nCh, int nStreamNo)
{
	AvPoolId cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;

	if(ioctl(av_fd, AV_POOL_DEC_WRITING_STATUS, &cmd) < 0)
		return -1;
	return 0;
}

int av_pool_get_pool_writing_cnt(int nCh, int nStreamNo)
{
	AvPoolCnt cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;

	if(ioctl(av_fd, AV_POOL_GET_WRITING_STATUS, &cmd) < 0)
		return -1;
	return cmd.cnt;
}

int av_pool_set_pool_ipnc_handle(int nCh, long ipncHandle)
{
	AvPoolHandle cmd;

	cmd.chn = nCh;
	cmd.streamNo = 0;
	cmd.handle = ipncHandle;

	if(ioctl(av_fd, AV_POOL_SET_IPNC_HANDLE, &cmd) < 0)
		return -1;
	return 0;
}

int av_pool_get_pool_ipnc_handle(int nCh, long *ipncHandle)
{
	AvPoolHandle cmd;

	cmd.chn = nCh;
	cmd.streamNo = 0;
	cmd.handle = 0;
	
	if(ioctl(av_fd, AV_POOL_GET_IPNC_HANDLE, &cmd) < 0)
		return -1;
	*ipncHandle = cmd.handle;
	return 0;
}

int av_pool_set_pool_stream_handle(int nCh, int nStreamNo, long streamHandle)
{
	AvPoolHandle cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;
	cmd.handle = streamHandle;

	if(ioctl(av_fd, AV_POOL_SET_STREAM_HANDLE, &cmd) < 0)
		return -1;
	return 0;
}

int av_pool_get_pool_stream_handle(int nCh, int nStreamNo, long *streamHandle)
{
	AvPoolHandle cmd;

	cmd.chn = nCh;
	cmd.streamNo = nStreamNo;
	cmd.handle = 0;

	if(ioctl(av_fd, AV_POOL_GET_STREAM_HANDLE, &cmd) < 0)
		return -1;
	*streamHandle = cmd.handle;
	return 0;
}

int av_pool_set_alarm_status(int nCh, int status)
{
	
	AvPoolAlarm cmd;

	cmd.chn = nCh;
	cmd.status = status;
	if(ioctl(av_fd, AV_POOL_SET_ALARM_STATUS, &cmd) < 0)
		return -1;

	return 0;
}

int av_pool_get_alarm_status(int nCh, int *status)
{
	AvPoolAlarm cmd;

	cmd.chn = nCh;
	cmd.status = 0;
	*status = 0;
	if(ioctl(av_fd, AV_POOL_GET_ALARM_STATUS, &cmd) < 0)
		return -1;
	*status = cmd.status;
	return 0;
}

int av_pool_reset_alarm_status(int nCh)
{
	AvPoolAlarm cmd;

	cmd.chn = nCh;
	cmd.status = 0;
	if(ioctl(av_fd, AV_POOL_RESET_ALARM_STATUS, &cmd) < 0)
		return -1;
	return 0;
}

int av_pool_wait_alarm_change(int nCh, int oldStatus, int *status)
{
	AvPoolAlarm cmd;

	cmd.chn = nCh;
	cmd.status = oldStatus;
	if(ioctl(av_fd, AV_POOL_WAIT_ALARM_CHANGE, &cmd) < 0)
		return -1;
	*status = cmd.status;
	return 0;
}

int av_pool_init()
{
	if(av_fd == 0)
	{
		av_fd = open(AV_POOL_DEVICE, O_RDWR);
		fcntl(av_fd, F_SETFD, 1);
	}
	return av_fd;
}

void av_pool_uninit()
{
	if(av_fd > 0)
		close(av_fd);
	if(msg_id)
		msgctl(msg_id, IPC_RMID, 0);
}

