
#ifndef __AV_POOL_MODULE_H__
#define __AV_POOL_MODULE_H__

#include <linux/ioctl.h>

enum
{
	AV_POOL_H264_1 = 0,
	AV_POOL_H264_2 = 1,
	AV_POOL_AUDIO = 2,
};

typedef struct Tag_AvWriteCmd
{
	unsigned char chn;
	unsigned char streamNo;
	unsigned char hdrLen;
	unsigned char frameType;
	unsigned char *hdrData;
	unsigned char *frameBuf;
	unsigned long frameLen;
	unsigned long frameNo;
	unsigned long long pts;
}AvWriteCmd;

typedef struct Tag_ReadCmd
{
	unsigned char chn;
	unsigned char streamNo;
	unsigned char *frameBuf;
	unsigned long bufLen;
	int frameLen;
	unsigned long needIFrame;
	unsigned long needFrameNo;
	unsigned long frameNo;
	unsigned long lastPos;
	unsigned long long pts;
	unsigned long maxDelay;
	unsigned long newestFrameNo;

	// 0: no wait
	// > 0: wait ms
	// < 0: wait forever
	int timeout;		// ms
}AvReadCmd;

typedef struct Tag_GetMaxFrameSize
{
	unsigned char chn;
	unsigned char streamNo;
	int maxFrameSize;
}GetMaxFrameSize;

typedef struct Tag_AvPoolInfo
{
	unsigned char chn;
	char ipAddr[32];		// ipc ip address of the stream
	char protocol[32];		// ipc protocol name
	int type;				// tcp or udp of the stream, SOCK_STREAM or SOCK_DGRAM
	int port;				// port of the stream
	unsigned int writeCnt;
	unsigned int readCnt;
}AvPoolInfo;

typedef enum
{
	POOL_IDLE = 0,
	POOL_WRITING = 0x1,
	POOL_READING = 0x2,
}AvPoolWorkStatus;

typedef struct Tag_AvPoolStaus
{
	unsigned char chn;
	unsigned char streamNo;
	AvPoolWorkStatus	status;
}AvPoolStaus;

typedef struct Tag_AvPoolFrameInfo
{
	unsigned char chn;
	unsigned char streamNo;
	unsigned int writeFrameCnt;
	unsigned int readFrameCnt;
}AvPoolFrameInfo;

typedef struct Tag_AvPoolId
{
	unsigned char chn;
	unsigned char streamNo;
}AvPoolId;

typedef struct Tag_AvPoolCnt
{
	unsigned char chn;
	unsigned char streamNo;
	int cnt;
}AvPoolCnt;

typedef struct Tag_AvPoolHandle
{
	unsigned char chn;
	unsigned char streamNo;
	long handle;
}AvPoolHandle;

typedef struct Tag_AvPoolAlarm
{
	unsigned char chn;
	unsigned int status;
}AvPoolAlarm;

#define AV_POOL_WRITE_FRAME	_IOW('A', 0x01, AvWriteCmd*)
#define AV_POOL_READ_FRAME	_IOR('A', 0x02, AvReadCmd*)
#define AV_POOL_GET_MAX_FRAME_SIZE	_IOR('A', 0x03, GetMaxFrameSize*)
#define AV_POOL_RESET_POOL	_IOW('A', 0x04, AvPoolId*)

#define AV_POOL_SET_POOL_INFO	_IOW('A', 0x10, AvPoolInfo*)
#define AV_POOL_GET_POOL_INFO	_IOR('A', 0x11, AvPoolInfo*)
#define AV_POOL_SET_POOL_STATUS	_IOW('A', 0x12, AvPoolStaus*)
#define AV_POOL_GET_POOL_STATUS	_IOR('A', 0x13, AvPoolStaus*)
#define AV_POOL_GET_FRAME_INFO	_IOR('A', 0x14, AvPoolFrameInfo*)

#define AV_POOL_INC_READING_STATUS	_IOW('A', 0x20, AvPoolId*)
#define AV_POOL_DEC_READING_STATUS	_IOW('A', 0x21, AvPoolId*)
#define AV_POOL_GET_READING_STATUS	_IOR('A', 0x22, AvPoolCnt*)
#define AV_POOL_INC_WRITING_STATUS	_IOW('A', 0x23, AvPoolId*)
#define AV_POOL_DEC_WRITING_STATUS	_IOW('A', 0x24, AvPoolId*)
#define AV_POOL_GET_WRITING_STATUS	_IOR('A', 0x25, AvPoolCnt*)

#define AV_POOL_SET_IPNC_HANDLE	_IOW('A', 0x30, AvPoolHandle*)
#define AV_POOL_GET_IPNC_HANDLE	_IOR('A', 0x31, AvPoolHandle*)
#define AV_POOL_SET_STREAM_HANDLE	_IOW('A', 0x32, AvPoolHandle*)
#define AV_POOL_GET_STREAM_HANDLE	_IOR('A', 0x33, AvPoolHandle*)

#define AV_POOL_SET_ALARM_STATUS		_IOW('A', 0x40, AvPoolAlarm*)
#define AV_POOL_GET_ALARM_STATUS		_IOR('A', 0x41, AvPoolAlarm*)
#define AV_POOL_RESET_ALARM_STATUS	_IOW('A', 0x42, AvPoolAlarm*)
#define AV_POOL_WAIT_ALARM_CHANGE	_IOR('A', 0x43, AvPoolAlarm*)



#endif
