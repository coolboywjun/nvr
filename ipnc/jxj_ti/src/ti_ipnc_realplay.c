
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>


#include "ti_struct.h"
#include "ti_net_preview.h"
#include "ti_net_interface.h"
#include "ti_net_common.h"

#include <av_pool.h>

#include <ipnc_lib.h>
#include <ipnc_lib_server.h>

#include "ti_ipnc_mng.h"

static void inline ti_ipnc_real_play_ref(TI_IPNC_STREAM_S *pStream)
{
	pStream->refCount++;
}
static void inline ti_ipnc_real_play_unref(TI_IPNC_STREAM_S *pStream)
{
	if(--pStream->refCount == 0)
		free(pStream);
}

static int ti_ipnc_real_play_farme_cb(long lRealHandle, int dwDataType, const unsigned char *pBuffer, int dwBufSize, void *pUserData)
{
	TI_IPNC_STREAM_S *pStream = (TI_IPNC_STREAM_S *)pUserData;
	int videoId = pStream->videoId;
	TI_FRAME_HEAD_S *pFrameHdr = (TI_FRAME_HEAD_S*)pBuffer;
	int nFrameType;

	//if(dwDataType != AUDIO_FRAME_A)
	//{
	//	if(pStream->preVideoFrameNo + 1 != pFrameHdr->u32FrameNo)
	//		printf("recv pre frame cnt: %u, cur frame no: %u\n", pStream->preVideoFrameNo, pFrameHdr->u32FrameNo);
	//	pStream->preVideoFrameNo = pFrameHdr->u32FrameNo;
	//}
	if(pFrameHdr->u8FrameType == 0xa)
		pStream->lastAudioPts = pFrameHdr->u64TimeStamp;
	else
		pStream->lastVideoPts = pFrameHdr->u64TimeStamp;
	if(pStream->lastAudioPts != 0 && pStream->lastVideoPts != 0 && 
		((pStream->lastAudioPts > pStream->lastVideoPts && 
		pStream->lastAudioPts - pStream->lastVideoPts > 10000000) ||
		(pStream->lastVideoPts > pStream->lastAudioPts && 
		pStream->lastVideoPts - pStream->lastAudioPts > 10000000)))
	{
		__ERR("The preview last video pts: %llu, last audio pts: %llu.\n", 
				pStream->lastVideoPts, pStream->lastAudioPts);
		pStream->lastVideoPts = 0;
		pStream->lastAudioPts = 0;
		return -1;
	}
	switch(dwDataType)
	{
		case VIDEO_FRAME_I:
		case VIDEO_FRAME_P:
		case VIDEO_FRAME_B:
		case AUDIO_FRAME_A:
			nFrameType = dwDataType;
			break;
		default:
			printf("The frame type %d don't support.\n", dwDataType);
			return -1;
	}
	
	if(videoId != 0 && av_pool_write_frame(videoId, (unsigned char *)(pBuffer + sizeof(TI_FRAME_HEAD_S)), dwBufSize-sizeof(TI_FRAME_HEAD_S), 
				(unsigned char *)pFrameHdr, sizeof(TI_FRAME_HEAD_S),
				pFrameHdr->u32FrameNo, nFrameType, pFrameHdr->u64TimeStamp) < 0)
	{
		printf("Write video frame to pool failed.\n");
		return -1;
	}
	return 0;
}

static void ti_ipnc_real_play_begin_cb(long lRealHandle, void *pUserData)
{
	int ret;
	TI_IPNC_STREAM_S *pStream = (TI_IPNC_STREAM_S *)pUserData;
	TI_IPNC_PROTOCOL_MNG *pMng = (TI_IPNC_PROTOCOL_MNG *)pStream->ipncHandle;

	ret = av_pool_start_write_frame(pStream->poolChn, pStream->streamNo, &pStream->videoId);
	if(ret < 0)
	{
		if(errno == EBUSY)
			printf("The pool ch%d stream%d is busy.\n", pStream->poolChn, pStream->streamNo);
		else
			printf("The pool ch%d stream%d write frame failed.\n", pStream->poolChn, pStream->streamNo);
	}
	av_pool_set_pool_ipnc_handle(pMng->poolCh, (long)pMng);
	av_pool_set_pool_stream_handle(pMng->poolCh, pStream->streamNo, (long)pStream);
	av_pool_set_pool_info(pMng->poolCh, pMng->ipAddr, JXJ_TI_PROTOCOL_NAME, SOCK_STREAM, pMng->port);
}

static void ti_ipnc_real_play_end_cb(long lRealHandle, void *pUserData)
{
	TI_IPNC_STREAM_S *pStream = (TI_IPNC_STREAM_S *)pUserData;
	
	av_pool_stop_write_frame(pStream->videoId);
}

int ti_ipnc_start_get_stream(long handle, int chn, int streamNo, long *streamHandle)
{
	long lRealHandle;
	TI_IPNC_PROTOCOL_MNG *pMng = (TI_IPNC_PROTOCOL_MNG *)handle;
	TI_IPNC_STREAM_S *pStream;
	// 检查输入参数有效性
	if(!ti_ipnc_mng_check_handle_valid(pMng) || chn != 0 || streamNo < 0 || streamNo >= 2)
	{
		printf("The input handle of starting get stream is null.\n");
		return -1;
	}
	
	// 如果这个流已经在获取了,则只增加相应的计数
	pStream = (TI_IPNC_STREAM_S *)pMng->stream[streamNo];
	if(pStream != NULL)
	{
		if(pStream->chn == chn 
			&& pStream->streamNo == streamNo
			&& pStream->poolChn == pMng->poolCh
			&& pStream->poolStreamNo == streamNo)
		{
			pStream->refCount++;
			ti_ipnc_request_iframe(handle, (long)pStream);
			*streamHandle = (long)pStream;
			return 0;
		}
		else
		{
			printf("Warning: The stream is write to the pool ch %d stream %d, can't write to the "
					"pool ch %d stream %d\n", 
					pStream->poolChn, pStream->poolStreamNo,
					pMng->poolCh, streamNo);
			return -1;
		}
	}

	if(av_pool_is_writing(pMng->poolCh, streamNo))
	{
		printf("The pool ch%d stream%d is busy.\n", pMng->poolCh, streamNo);
		return -1;
	}
	
	// 如果这个流没有在获取, 则开始获取
	pStream = (TI_IPNC_STREAM_S*)malloc(sizeof(TI_IPNC_STREAM_S));
	if(pStream == NULL)
	{
		printf("Malloc for TI_IPNC_STREAM_S failed.\n");
		return -1;
	}
	memset(pStream, 0, sizeof(TI_IPNC_STREAM_S));

	//printf("ipaddr: %s, port: %d\n", pMng->ipAddr, pMng->port);
	//printf("user: %s, passwd: %s\n", pMng->user, pMng->passwd);
	//printf("chn: %d, stream: %d\n", chn, streamNo);
	//printf("poolchn: %d, poolstream: %d\n", poolChn, poolStreamNo);
	pMng->stream[streamNo] = pStream;
	pStream->chn = chn;
	pStream->streamNo = streamNo;
	pStream->poolChn = pMng->poolCh;
	pStream->poolStreamNo = streamNo;
	pStream->refCount = 1;
	pStream->ipncHandle = pMng;
	//av_pool_set_pool_ipnc_handle(pMng->poolCh, handle);
	//av_pool_set_pool_stream_handle(pMng->poolCh, streamNo, (long)pStream);
	//av_pool_set_pool_info(pMng->poolCh, pMng->ipAddr, JXJ_TI_PROTOCOL_NAME, SOCK_STREAM, pMng->port);
	if(TI_NET_Start_RealPlay(pMng->ipAddr, pMng->port, 
				pMng->user, pMng->passwd, chn, streamNo, 
				ti_ipnc_real_play_farme_cb, 
				ti_ipnc_real_play_begin_cb, 
				ti_ipnc_real_play_end_cb, 
				pStream, &lRealHandle) < 0)
	{
		printf("Failed to start real play.\n");
		free(pStream);
		return -1;
	}
	pStream->lRealPlayHandle = lRealHandle;
	ti_ipnc_mng_ref(pMng);
	*streamHandle = (long)pStream;
	//printf("%s: chn: %d, streamNo: %d, poolChn: %d, poolStreamNo: %d ipncHandle: 0x%08lx, streamHandle: 0x%08lx\n", __func__, chn, streamNo, poolChn, poolStreamNo, handle, *streamHandle);
	
	return 0;
}

int ti_ipnc_stop_get_stream(long handle, long streamHandle)
{
	TI_IPNC_PROTOCOL_MNG *pMng = (TI_IPNC_PROTOCOL_MNG *)handle;
	TI_IPNC_STREAM_S *pStream = (TI_IPNC_STREAM_S *)streamHandle;
	//printf("%s: ipncHandle: 0x%08lx, streamHandle: 0x%08lx\n", __func__, handle, streamHandle);
	if(!ti_ipnc_mng_check_ipnc_handle_valid(pMng, pStream))
	{
		printf("input param of stopping get stream is wrong.\n");
		return -1;
	}
	if(pMng->stream[0] != pStream && pMng->stream[1] != pStream)
	{
		printf("The ipnc stream handle is wrong.\n");
		return -1;
	}

	if(--pStream->refCount == 0)
	{
		if(pMng->stream[0] == pStream)
			pMng->stream[0] = NULL;
		else
			pMng->stream[1] = NULL;
		TI_NET_Stop_RealPlay(pStream->lRealPlayHandle);
		free(pStream);
		ti_ipnc_mng_unref(pMng);
	}
	return 0;
}

int ti_ipnc_request_iframe(long handle, long streamHandle)
{
	TI_IPNC_PROTOCOL_MNG *pMng = (TI_IPNC_PROTOCOL_MNG *)handle;
	TI_IPNC_STREAM_S *pStream = (TI_IPNC_STREAM_S *)streamHandle;
	if(!ti_ipnc_mng_check_ipnc_handle_valid(pMng, pStream))
		return -1;
	//printf("%s %d force request iframe.\n", __func__, __LINE__);
	return TI_NET_Request_IFrame(pStream->lRealPlayHandle);
}

