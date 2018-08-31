
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "nvr_rtsp_global.h"

#include <av_pool.h>


typedef struct _RTSP_STREAM_S_
{
	long lReadId;
} RTSP_STREAM_S;


int nvr_rtsp_get_audio(int nCh, char *lpFrameBuf,
	                                unsigned long *pU32Pts,
	                                unsigned long *pU32Playload,
	                                unsigned long *pU32Bits,
	                                unsigned long *pU32Sample,
	                                int nTimeOut,
	                                long lPriData)
{
	return -1;
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
	if (pRtspStream == NULL)
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
	default:
		nRet = -1;
		break;
	}
	return nRet;
}

int nvr_rtsp_init()
{
	nvr_rtsp_set_cb(NULL, nvr_rtsp_request_iframe,
	                nvr_rtsp_get_video, nvr_rtsp_get_audio,
	                nvr_rtsp_msg, NULL, NULL);
	return nvr_rtsp_start(8, 2, 554, 0, 0);
}

int nvr_rtsp_uninit()
{
	return nvr_rtsp_stop();
}

static volatile int quit = 0;
void sig_quit(int signo)
{
	quit = 1;
}


int main(int argc, char *argv[])
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, sig_quit);
	signal(SIGINT,  sig_quit);
	
	if(av_pool_init() < 0)
	{
		printf("av pool init failed.\n");
		return -1;
	}
	if(nvr_rtsp_init() < 0)
	{
		printf("Rtsp init failed.\n");
		return -1;
	}

	while(!quit)	pause();
	
	nvr_rtsp_uninit();
	av_pool_uninit();
	return 0;
}

