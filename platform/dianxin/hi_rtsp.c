/******************************************************************************

                  版权所有 (C), 2011-2015, 佳信捷电子

 ******************************************************************************
  文 件 名   : hi_rtsp.c
  版 本 号   : v2.0
  作    者   : 梁锦明
  生成日期   : 2011年9月8日
  最近修改   :
  功能描述   : 主控模块rtsp实现
  函数列表   :
              hi_rtsp_get_audio
              hi_rtsp_get_video
              hi_rtsp_init
              hi_rtsp_msg
              hi_rtsp_uninit
  修改历史   :
  1.日    期   : 2011年9月8日
    作    者   : 梁锦明
    修改内容   : 创建文件

  2.日    期   : 2011年10月19日
    作    者   : 周毅
    修改内容   : _SAMPLE_CODE 用于编译模拟器程序，模拟器程序不需要本文件内容。
******************************************************************************/

#ifndef _SAMPLE_SOURCE 

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/ 
#include <mxml.h>
#include "hi_rtsp_global.h"
#include "hi_rtsp.h"
#include "av_pool.h"
#include "net_api.h"

#include "stun_buffer.h"
#include "storage_interface.h"
#include "hi_dianxin_proc.h"
#include "storage_lib.h"
#include "hi_platform_dianxin.h"
#include "hi_dianxin_common.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 函数实现                                     *
 *----------------------------------------------*/

typedef struct _RTSP_STREAM_S_
{
	long lReadId;
} RTSP_STREAM_S;


typedef struct _HI_RTSP_VOD_S_
{
	char szBeginTime[32];
	long lReadHandle;
	StorageFragmentInfo stFragmentInfo;
}HI_RTSP_VOD_S;

// pBeginTime format: YYYYMMDDHHMMSS
int hi_rtsp_get_vod_start(const char *pFileName, void **handle)
{
	int i, shmid, ret;
	int nChn = 0;
	long lReadHandle;
	HI_RTSP_VOD_S *pRtspVod;
	RecordQueryChnInfo *chn_info;	
	StorageFragmentInfo stFragmentInfo;
	RecordQueryInfo *query_info;
	int nBeginTime, nEndTime;
	int year, mon, day, hour, min, sec;
	struct tm stm = {0};
	char szFileName[128] = {0};
	char szBeginTime[64];
	char szCh[8] = {0};
	char *p1, *p2;
	if(handle == NULL)
		return -1;

	if(strstr(pFileName, "mp4") == NULL)
		return -1;
	if(pFileName == NULL || strlen(pFileName) < 14)
		return -1;
	strncpy(szFileName, pFileName, sizeof(szFileName)-1);
	p1 = strchr(szFileName, '_');
	if(p1 == NULL)
		return -1;
	p2 = strrchr(szFileName, '.');
	if(p2 == NULL)
		return -1;
	*p2 = 0;

	p2 = strrchr(p1+1, '_');
	if (p2 == NULL)
		return -1;
	
	if (p2 - p1 - 1 > sizeof(szCh) - 1) 
		return -1;
	
	strncpy(szCh, p1+1, p2 - p1 - 1);
	nChn = atoi(szCh);
	//filename_ch_time.mp4
	strcpy(szBeginTime, p2+1);

	//printf("###############szCh = %s\r\n", szCh);
	//printf("###########szBeginTime = %s\r\n", szBeginTime);
	//printf("szCh = %s\r\n", szCh);
	for(i = 0; i < 14; i++)
	{
		if(!isdigit(szBeginTime[i]))
			return -1;
	}
	if(sscanf(szBeginTime, "%04d%02d%02d%02d%02d%02d", 
			&year, &mon, &day, &hour, &min, &sec) != 6)
	{
		printf("The record fragment for rtsp is wrong.\n");
		return -1;
	}
	printf("Begin time:  %d-%d-%d %d:%d:%d\n", year, mon, day, hour, min, sec);
	stm.tm_year = year - 1900;
	stm.tm_mon = (mon - 1) % 12;
	stm.tm_mday = day % 32;
	stm.tm_hour = hour % 24;
	stm.tm_min = min % 60;
	stm.tm_sec = sec % 60;
	nBeginTime = (int)mktime(&stm);
	nEndTime = nBeginTime + 3600;	// 搜索一个小时内的录像
	
	ret = storage_lib_record_query(1<<nChn,
							 	nBeginTime, nEndTime, RECORD_ALL_TYPE,
							 	&shmid, &chn_info, &query_info);
	if(ret < 0)
	{
		printf("Failed to query record\n");
		return -1;
	}
	
	if(ret == 0)
	{	
		storage_lib_record_query_free(shmid, chn_info, query_info);	
		return -1;
	}
	
	if(storage_lib_read_start(query_info, &stFragmentInfo, &lReadHandle) < 0)
	{
		printf("Start read failed\n");
		return -1;
	}
	
	pRtspVod = malloc(sizeof(HI_RTSP_VOD_S));
	if(pRtspVod == NULL)
	{
		storage_lib_record_query_free(shmid, chn_info, query_info);	
		storage_lib_read_stop(lReadHandle);
		return -1;
	}

	memset(pRtspVod, 0, sizeof(HI_RTSP_VOD_S));	
	memcpy(&pRtspVod->stFragmentInfo, &stFragmentInfo, sizeof(stFragmentInfo));
	strcpy(pRtspVod->szBeginTime, szBeginTime);
	//printf("stFragmentInfo begin_time = %lu, end_time = %lu\r\n", stFragmentInfo.begin_time, stFragmentInfo.end_time);
	pRtspVod->lReadHandle = lReadHandle;	
	*handle = (void *)pRtspVod;		
	storage_lib_record_query_free(shmid, chn_info, query_info); 
	
	return 0;
}

int hi_rtsp_get_vod_stop(void *handle)
{	
	HI_RTSP_VOD_S *pRtspVod = (HI_RTSP_VOD_S*)handle;

	if(pRtspVod == NULL)
		return -1;
	storage_lib_read_stop(pRtspVod->lReadHandle);
	free(pRtspVod);

	return 0;
}

int hi_rtsp_get_vod_property(void *handle)
{
	int sec;
	HI_RTSP_VOD_S *pRtspVod = (HI_RTSP_VOD_S*)handle;

	if(pRtspVod == NULL)
		return 0;

	//printf("fragement end_time = %lu, begin_time = %lu\r\n", 
	//	pRtspVod->stFragmentInfo.end_time,
	//		pRtspVod->stFragmentInfo.begin_time);

	if (pRtspVod->stFragmentInfo.end_time > pRtspVod->stFragmentInfo.begin_time)
		sec = pRtspVod->stFragmentInfo.end_time - pRtspVod->stFragmentInfo.begin_time;
	else
		sec = 0;
	
	return sec;
}

int hi_rtsp_vod_seek(void *handle, int sec)
{
	int ret;
	HI_RTSP_VOD_S *pRtspVod = (HI_RTSP_VOD_S*)handle;

	if(pRtspVod == NULL)
		return -1;
	
	
	if(pRtspVod->stFragmentInfo.end_time < pRtspVod->stFragmentInfo.begin_time)
		return -1;
	
	if(sec > pRtspVod->stFragmentInfo.end_time - pRtspVod->stFragmentInfo.begin_time)
		return -1;

	//printf("vod seek %d sec\r\n", sec);
	ret = storage_lib_read_set(pRtspVod->lReadHandle, pRtspVod->stFragmentInfo.begin_time + sec);
	if (ret == -1) 
	{
		return -1;
	}
	
	return 0;
	
}

int hi_rtsp_get_vod_frame(void *handle, 
							char ** lpFrameBuf, unsigned long u32FrameBufSize,
							unsigned long *pFrameType,
							unsigned long *pU32Pts, unsigned long *pU32PlayLoad,
							unsigned long *pU32Sample, unsigned long *pU32Bits,
							unsigned long * pU32Offset)
{
	int nFrameSize;
	StorageFrameHdr stFrameHdr;	
	AV_FRAME_HEAD_S *pFrameHdr;
	HI_RTSP_VOD_S *pRtspVod = (HI_RTSP_VOD_S*)handle;
	unsigned long u32VideoArrPt[3] = {RTP_PT_H264, RTP_PT_JPEG, RTP_PT_JPEG};
	unsigned long u32ArrSampleRate[3]  = {8000, 16000, 32000};
	unsigned long u32AudioArrPt[4] = {RTP_PT_ADPCM, RTP_PT_ALAW, RTP_PT_ULAW, RTP_PT_G726};

	if(pRtspVod == NULL)
		return -1;
	if(pU32Pts == NULL || pU32PlayLoad == NULL)
		return -1;
	
	nFrameSize = storage_lib_read_frame(pRtspVod->lReadHandle, READ_NORMAL, &stFrameHdr, (unsigned char **)lpFrameBuf);
	if(nFrameSize <= 0)	
		return -1;

	pFrameHdr = (AV_FRAME_HEAD_S*)(*lpFrameBuf);
	if (pFrameHdr->u16FrameFlag != 0x3448)
		return -1;

	*pU32Pts = pFrameHdr->u64TimeStamp / 1000;
	
	if(pFrameHdr->u8FrameType != 0xa)	// video
	{
		*pFrameType = 1;
		*pU32PlayLoad =  u32VideoArrPt[pFrameHdr->unVidoeInfo.u8EncodeType % 3];
		*pU32Sample = 0;
		*pU32Bits = 0;
	}
	else		// audio
	{	
		//printf("pFrameHdr->unAudioInfo.u16EncodeType = %u\n", pFrameHdr->unAudioInfo.u16EncodeType);
		*pFrameType = 0;
		*pU32PlayLoad 	=  u32AudioArrPt[pFrameHdr->unAudioInfo.u16EncodeType % 4];
		*pU32Sample 	= u32ArrSampleRate[pFrameHdr->unAudioInfo.u8AudioSamples % 3];
		*pU32Bits 		= pFrameHdr->unAudioInfo.u8AudioBits;
	}
	*pU32Offset = sizeof(AV_FRAME_HEAD_S);
	
	return pFrameHdr->u32FrameSize;
}

///// 注意，这个接口只获取当前音频信息使用,
///// 不能做获取音频数据使用
int nvr_rtsp_get_audio(int nCh, char **lpFrameBuf,
                       unsigned long *pU32Pts,
                       unsigned long *pU32Playload,
                       unsigned long *pU32Bits,
                       unsigned long *pU32Sample,
                       long lPriData)
{
	RTSP_STREAM_S *pRtspStream = (RTSP_STREAM_S *)lPriData;
	//char szData[2048] = {0};
	unsigned long u32ArrSampleRate[3]  = {8000, 16000, 32000};
	unsigned long u32ArrPt[4] = {RTP_PT_ADPCM, RTP_PT_ALAW, RTP_PT_ULAW, RTP_PT_G726};
	char *pFrameBuf =  NULL;
	int nBufLen = 0;
	int nFrameLen;

	nFrameLen = av_pool_read_frame(pRtspStream->lReadId, &pFrameBuf, &nBufLen, 100);
	if (nFrameLen > 0)
	{
		AV_FRAME_HEAD_S *pFrameHdr = (AV_FRAME_HEAD_S *)pFrameBuf;
		if(pFrameHdr->u8FrameType != AV_AUDIO_FRAME_A)
			return -1;
		*pU32Playload	=  u32ArrPt[pFrameHdr->unAudioInfo.u16EncodeType % 4];
		*pU32Bits		=  pFrameHdr->unAudioInfo.u8AudioBits;
		*pU32Sample 	= u32ArrSampleRate[pFrameHdr->unAudioInfo.u8AudioSamples%3];
		*pU32Pts		= (unsigned long)(pFrameHdr->u64TimeStamp/1000);
		nFrameLen	-= sizeof(AV_FRAME_HEAD_S);
	}

	return nFrameLen;
}

int nvr_rtsp_get_video(int nCh, int nStream,
                       char **lpFrameBuf,
                       unsigned long *pU32Pts,
                       unsigned long *pU32PlayLoad,
                       unsigned long *pU32FrameType,
                       int nTimeOut,
                       long lPriData)
{
	RTSP_STREAM_S *pRtspStream = (RTSP_STREAM_S *)lPriData;
	char *pFrameBuf =  NULL;
	int nBufLen = 0;
	int nFrameLen;
	unsigned long u32ArrPt[3] = {RTP_PT_H264, RTP_PT_JPEG, RTP_PT_JPEG};
	if(pRtspStream == NULL)
		return -1;
	*lpFrameBuf = NULL;
	//printf("^^^^^^^^^^^^lPriData = 0x%08x\n", lPriData);
	//printf("^^^^^^^^^^lReadId = %d\n", pRtspStream->lReadId);
	//printf("^^^^^^^^^^^^^^^^^^ nCh: %d, nStream: %d\n", nCh, nStream);
	nFrameLen = av_pool_read_frame(pRtspStream->lReadId, &pFrameBuf, &nBufLen, nTimeOut);
	if(nFrameLen > 0 && nBufLen > 0 && pFrameBuf != NULL)
	{
		AV_FRAME_HEAD_S *pFrameHdr = (AV_FRAME_HEAD_S *)pFrameBuf;
		if(pFrameHdr->u8FrameType == AV_AUDIO_FRAME_A)
			return -1;
		//printf("pridata: %08x, framelen: %d, frametype: %d\n", lPriData, pFrameHdr->u32FrameSize, pFrameHdr->u8FrameType);
		*lpFrameBuf = (char *)(pFrameBuf + sizeof(AV_FRAME_HEAD_S));
		*pU32PlayLoad = u32ArrPt[pFrameHdr->unVidoeInfo.u8EncodeType % 3];
		*pU32FrameType = pFrameHdr->u8FrameType;
		*pU32Pts = (unsigned long)(pFrameHdr->u64TimeStamp / 1000);
		nFrameLen -= sizeof(AV_FRAME_HEAD_S);
		return nFrameLen;
	}
	return -1;
}

int nvr_rtsp_get_frame(int nCh, int nStream,
                       char **lpFrameBuf,
                       unsigned long *pU32Pts,
                       unsigned long *pU32PlayLoad,
                       unsigned long *pU32FrameType,
                       int nTimeOut,
                       long lPriData)
{
	RTSP_STREAM_S *pRtspStream = (RTSP_STREAM_S *)lPriData;
	char *pFrameBuf =  NULL;
	int nBufLen = 0;
	int nFrameLen;
	unsigned long u32VideoArrPt[3] = {RTP_PT_H264, RTP_PT_JPEG, RTP_PT_JPEG};
	unsigned long u32AudioArrPt[4] = {RTP_PT_ADPCM, RTP_PT_ALAW, RTP_PT_ULAW, RTP_PT_G726};
	if(pRtspStream == NULL)
		return -1;
	*lpFrameBuf = NULL;
	//printf("^^^^^^^^^^^^lPriData = 0x%08x\n", lPriData);
	//printf("^^^^^^^^^^lReadId = %d\n", pRtspStream->lReadId);
	//printf("^^^^^^^^^^^^^^^^^^ nCh: %d, nStream: %d\n", nCh, nStream);
	nFrameLen = av_pool_read_frame(pRtspStream->lReadId, &pFrameBuf, &nBufLen, nTimeOut);
	if(nFrameLen > 0 && nBufLen > 0 && pFrameBuf != NULL)
	{
		AV_FRAME_HEAD_S *pFrameHdr = (AV_FRAME_HEAD_S *)pFrameBuf;
			
		if(pFrameHdr->u8FrameType == AV_AUDIO_FRAME_A)
		{
			*pU32PlayLoad = u32AudioArrPt[pFrameHdr->unVidoeInfo.u8EncodeType % 3];
		}
		else
		{
			//printf("pridata: %08x, framelen: %d, frametype: %d\n", lPriData, pFrameHdr->u32FrameSize, pFrameHdr->u8FrameType);
			*pU32PlayLoad = u32VideoArrPt[pFrameHdr->unVidoeInfo.u8EncodeType % 3];
		}	
		*pU32FrameType = pFrameHdr->u8FrameType;
		*pU32Pts = (unsigned long)(pFrameHdr->u64TimeStamp / 1000);
		*lpFrameBuf = (char *)(pFrameBuf + sizeof(AV_FRAME_HEAD_S));
		nFrameLen -= sizeof(AV_FRAME_HEAD_S);
		
		return nFrameLen;
	}
	return -1;
}


static inline int send_udp_message(int sock, stun_buffer *message, struct sockaddr *addr)
{
	if(net_udp_send(sock, (char *)message->buf, message->len, addr) != message->len)
	{
		printf("Send message to turn server failed.\n");
		return -1;
	}
	return message->len;
}

int recv_udp_message(int sock, stun_buffer *message, struct sockaddr *addr)
{
	int nLen;

	if(net_select(&sock, 1, 0x1, 3000) <= 0)
	{
		printf("Receive message from turn server timeout.\n");
		return -1;
	}
	nLen = sizeof(struct sockaddr);
	message->len = sizeof(message->buf);
	if((message->len = net_udp_recv(sock, (char *)message->buf, message->len, 0, addr, &nLen)) < 0)
	{
		printf("Receive message from turn server failed.\n");
		return -1;
	}
	return 0;
}


int hi_rtsp_get_mapping(RTSP_MAP_ADDRESS *pMapAddr)
{
	int sock;
	stun_buffer message;
	stun_attr_ref attr;
	ioa_addr addr;
	struct sockaddr_in sockAddr = {0};
	
	if(pMapAddr == NULL || pMapAddr->nTurnServerPort < 1024)
		return -1;

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(pMapAddr->nTurnServerPort);
	sockAddr.sin_addr.s_addr = inet_addr(pMapAddr->szTurnServerIp);
	
	sock = pMapAddr->sock;
	//sock = hi_create_sock(2);
	//if(sock < 0)
	//	return -1;
	//hi_bind_sock(sock, INADDR_ANY, pMapAddr->innerPort);
	memset(&message, 0, sizeof(message));
	stun_set_binding_request(&message);
	if(send_udp_message(sock, &message, (struct sockaddr*)&sockAddr) < 0)
	{
		printf("Send message to turn server failed.\n");
		return -1;
	}
	if(recv_udp_message(sock, &message, (struct sockaddr*)&sockAddr) < 0)
	{
		printf("Recv message from turn server.\n");
		return -1;
	}
	if(!stun_is_binding_response(&message))
	{
		printf("The response is not binding response.\n");
		return -1;
	}
	attr = stun_attr_get_first_by_type(&message, STUN_ATTRIBUTE_MAPPED_ADDRESS);
	if(attr == NULL)
	{
		printf("Get response address attribute failed.\n");
		return -1;
	}
	stun_attr_get_addr(&message, attr, &addr, NULL);
	addr_to_string(&addr, (u08bits*)pMapAddr->natIp);
	inet_ntop(AF_INET, &addr.s4.sin_addr, pMapAddr->natIp, sizeof(pMapAddr->natIp));
	pMapAddr->natPort = addr_get_port(&addr);

	printf("@@@@@@@@ port %d mapping to ip address ==> %s:%d\n", pMapAddr->innerPort, pMapAddr->natIp, pMapAddr->natPort);
	
	return 0;
}

int nvr_rtsp_request_iframe(int nCh, int nStream, long lPriData)
{
	RTSP_STREAM_S *pRtspStream = (RTSP_STREAM_S *)lPriData;
	if(pRtspStream == NULL)
		return -1;	
	return av_pool_request_iframe(nCh, nStream);
}

static int nvr_rtsp_start_preview(int nCh, int nStream, long *lPriData)
{
	RTSP_STREAM_S *pRtspStream;
	long lReadId;

	pRtspStream = (RTSP_STREAM_S *)malloc(sizeof(RTSP_STREAM_S));
	if(pRtspStream == NULL)
		return -1;
	if(av_pool_start_read_frame(nCh, nStream, 1000, &lReadId) < 0)
	{
		printf("Start read av pool failed.\n");
		free(pRtspStream);
		return -1;
	}
	pRtspStream->lReadId = lReadId;
	//printf("[%s:%d]pRtspStream = %p\n", __func__, __LINE__, pRtspStream);
	//printf("[%s:%d]lReadId: 0x%08x\n", __func__, __LINE__, pRtspStream->lReadId);
	*lPriData = (long)pRtspStream;
	return 0;
}

static int nvr_rtsp_stop_preview(int nCh, int nStream, long *lPriData)
{
	RTSP_STREAM_S *pRtspStream = (RTSP_STREAM_S *)(*lPriData);
	if(pRtspStream == NULL)
		return -1;
	//printf("[%s:%d]pRtspStream = %p\n", __func__, __LINE__, pRtspStream);
	//printf("[%s:%d]lReadId: 0x%08x\n", __func__, __LINE__, pRtspStream->lReadId);
	av_pool_stop_read_frame(pRtspStream->lReadId);
	free(pRtspStream);
	*lPriData = 0;
	return 0;
}




int nvr_rtsp_msg(RTSP_MSG_NOTIFY eMsgNotify, int nCh, int nStream, long *lPriData)
{
		int nRet;
	switch(eMsgNotify)
	{
	case RTSP_MSG_PREVIEW_LINK:
		nRet = nvr_rtsp_start_preview(nCh, nStream, lPriData);
		break;
	case RTSP_MSG_PREVIEW_UNLINK:
		nRet = nvr_rtsp_stop_preview(nCh, nStream, lPriData);
		break;
	case RTSP_MSG_GET_MAP_ADDRESS:
		{
			nRet = hi_rtsp_get_mapping((RTSP_MAP_ADDRESS *)lPriData);
			break;
		}	
	default:
		
		nRet = -1;
		break;
	}
	return nRet;
}



extern int dianxin_video_varify_proc(char *pDevId, char *pChannelNo, 
			char *pStreamType, char *pHashToken);

extern int dianxin_get_video_flag();

extern int dianxin_vod_varify_proc(char *pDevId, char *pChannelNo, 
			char *pFileName, char *pHashToken);

extern int dianxin_get_vod_flag();



int hi_rtsp_init(int rtspPort)
{
	hi_rtsp_start(1, 2, rtspPort, 0, 0);
	
	hi_rtsp_set_cb(NULL, nvr_rtsp_request_iframe,
						nvr_rtsp_get_video, nvr_rtsp_get_audio,
						nvr_rtsp_get_frame, nvr_rtsp_msg);

	hi_rtsp_set_vod_cb(hi_rtsp_get_vod_start, hi_rtsp_get_vod_stop,
							hi_rtsp_get_vod_frame, hi_rtsp_vod_seek,
							hi_rtsp_get_vod_property);
	hi_rtsp_set_varify_cb(dianxin_video_varify_proc, dianxin_vod_varify_proc,
							dianxin_get_video_flag, dianxin_get_vod_flag);
	return 0;	
}

int hi_rtsp_uninit()
{
	hi_rtsp_stop();
	return 0;
}

#endif
