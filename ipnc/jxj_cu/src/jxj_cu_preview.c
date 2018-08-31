
#include "jxj_cu_heads.h"

#include "jxj_cu_preview.h"
#include "jxj_cu_param.h"
#include "jxj_cu_logon.h"
#include "jxj_cu_ipnc_mng.h"
#include "ipnc_lib_server.h"

#include <av_pool.h>
#include <rtsp_client.h>


//流回调函数 将帧数据写入avpool
static int jxj_cu_write_frame(RtspFrameInfo *rtspFrameInfo, void *rtspUserData)
{
	AV_FRAME_HEAD_S stFrameHdr = {0};
	JXJ_CU_STREAM_S *pStream = (JXJ_CU_STREAM_S *)rtspUserData;

	//__DBUG("cu get a frame.\n");
	if(NULL == rtspFrameInfo)
		return -1;
	if(rtspFrameInfo->frameType == RTSP_AUDIO_FRAME_A)
		pStream->lastAudioPts = rtspFrameInfo->timestamp;
	else
		pStream->lastVideoPts = rtspFrameInfo->timestamp;
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
	
	stFrameHdr.u16FrameFlag = 0x3448;
	switch(rtspFrameInfo->frameType)
	{
		case RTSP_VIDEO_FRAME_I:
			stFrameHdr.u8FrameType = AV_VIDEO_FRAME_I;
			break;
		case RTSP_VIDEO_FRAME_P:
			stFrameHdr.u8FrameType = AV_VIDEO_FRAME_P;
			break;
		case RTSP_VIDEO_FRAME_B:
			stFrameHdr.u8FrameType = AV_VIDEO_FRAME_B;
			break;
		case RTSP_AUDIO_FRAME_A:
			stFrameHdr.u8FrameType = AV_AUDIO_FRAME_A;
			break;
		default:
			__ERR("The rtsp frame type %d is wrong.\n", rtspFrameInfo->frameType);
			return -1;
			break;
	}
	stFrameHdr.u32FrameNo = rtspFrameInfo->frameNo;
	stFrameHdr.u32Time = rtspFrameInfo->timestamp / 1000000;
	stFrameHdr.u32FrameSize = rtspFrameInfo->frameSize;
	stFrameHdr.u64TimeStamp = rtspFrameInfo->timestamp;
	if(stFrameHdr.u8FrameType == AV_AUDIO_FRAME_A)
	{
		stFrameHdr.unAudioInfo.u8AudioSamples = rtspFrameInfo->audioFrameInfo.audioSample;
		stFrameHdr.unAudioInfo.u16EncodeType = rtspFrameInfo->audioFrameInfo.encodeType;
		stFrameHdr.unAudioInfo.u8AudioChannels = rtspFrameInfo->audioFrameInfo.audioChannel;
		stFrameHdr.unAudioInfo.u8AudioBits = rtspFrameInfo->audioFrameInfo.audioBits;
	}
	else
	{
		stFrameHdr.unVidoeInfo.u16ImageWidth = rtspFrameInfo->videoFrameInfo.width;
		stFrameHdr.unVidoeInfo.u16ImageHeight = rtspFrameInfo->videoFrameInfo.height;
		stFrameHdr.unVidoeInfo.u8EncodeType = rtspFrameInfo->videoFrameInfo.encodeType;
		stFrameHdr.unVidoeInfo.u8FrameRate = rtspFrameInfo->videoFrameInfo.frameRate;
	}
	if(av_pool_write_frame(pStream->videoId, rtspFrameInfo->frameBuffer, rtspFrameInfo->frameSize, 
							   (unsigned char *)&stFrameHdr, sizeof(stFrameHdr),
							   stFrameHdr.u32FrameNo, stFrameHdr.u8FrameType, stFrameHdr.u64TimeStamp))
	{
		__ERR("Write frame to av pool failed.\n");
		return -1;
	}

	return 0;
}

static int jxj_cu_begin_write_frame(void *rtspUserData)
{
	JXJ_CU_STREAM_S *pStream = (JXJ_CU_STREAM_S *)rtspUserData;
	JXJ_CU_PROTOCOL_MNG *pMng = (JXJ_CU_PROTOCOL_MNG *)pStream->ipncHandle;

	if(av_pool_start_write_frame(pStream->poolChn, 
					pStream->poolStreamNo, &pStream->videoId) < 0)
	{
		__ERR("Start write frame failed.\n");
		return -1;
	}
	av_pool_set_pool_ipnc_handle(pStream->poolChn, (long)pMng);
	av_pool_set_pool_stream_handle(pStream->poolChn, pStream->poolStreamNo, (long)pStream);
	av_pool_set_pool_info(pMng->poolCh, pMng->ipAddr, JXJ_CU_PROTOCOL_NAME, SOCK_STREAM, pMng->port);
	return 0;
}

static int jxj_cu_end_write_frame(void *rtspUserData)
{
	JXJ_CU_STREAM_S *pStream = (JXJ_CU_STREAM_S *)rtspUserData;
	av_pool_stop_write_frame(pStream->videoId);
	pStream->videoId = 0;
	return 0;
}

//开始写帧
int jxj_cu_start_write_frame(long ipncHandle, int chn, int streamNo, long *pStreamHandle)
{
	JXJ_CU_PROTOCOL_MNG *pMng = (JXJ_CU_PROTOCOL_MNG *)ipncHandle;
	JXJ_CU_STREAM_S *pStream = NULL;
	char url[128] = {0};
	
	// 检查输入参数有效性
	if(!jxj_cu_ipnc_mng_check_handle_valid(pMng) || 
		chn < 0     || 
		streamNo < 0 || 
		streamNo >= 2)
	{
		__ERR("The input handle of starting get stream is null.\n");
		return -1;
	}
	sprintf(url, "rtsp://%s:7554/dev=JXJ-NVR-12345678/media=0/channel=%d&level=%d", 
			pMng->ipAddr, chn, streamNo);

	__DBUG("ipaddr: %s, port: %d\n", pMng->ipAddr, pMng->port);
	__DBUG("user: %s, passwd: %s\n", pMng->user, pMng->passwd);
	__DBUG("chn: %d, stream: %d\n", chn, streamNo);
	__DBUG("poolchn: %d, poolstream: %d\n", pMng->poolCh, streamNo);
	__DBUG("url: %s\n", url);
	
	// 如果这个流已经在获取了,则只增加相应的计数
	pStream = (JXJ_CU_STREAM_S *)pMng->stream[streamNo];
	if(pStream != NULL)
	{
		if(pStream->chn == chn 
			&& pStream->streamNo == streamNo
			&& pStream->poolChn == pMng->poolCh
			&& pStream->poolStreamNo == streamNo)
		{
			pStream->refCount++;
			jxj_cu_get_request_iframe(ipncHandle, (long)pStream);
			*pStreamHandle = (long)pStream;
			return 0;
		}
		else
		{
			__ERR("Warning: The stream is write to the pool ch %d stream %d, can't write to the "
					"pool ch %d stream %d\n", 
					pStream->poolChn, pStream->poolStreamNo,
					pMng->poolCh, streamNo);
			return -1;
		}
	}
	pStream = NULL;

	if(av_pool_is_writing(pMng->poolCh, streamNo))
	{
		__ERR("The pool ch%d stream%d is busy.\n", pMng->poolCh, streamNo);
		return -1;
	}
	
	// 如果这个流没有在获取, 则开始获取
	pStream = (JXJ_CU_STREAM_S*)malloc(sizeof(JXJ_CU_STREAM_S));
	if(pStream == NULL)
	{
		__ERR("Malloc for TI_IPNC_STREAM_S failed.\n");
		return -1;
	}
	memset(pStream, 0, sizeof(JXJ_CU_STREAM_S));

	pMng->stream[streamNo] = pStream;
	pStream->chn = chn;
	pStream->streamNo = streamNo;
	pStream->poolChn = pMng->poolCh;
	pStream->poolStreamNo = streamNo;
	pStream->refCount = 1;
	pStream->ipncHandle = pMng;
	//开始获取rtsp流
	rtspClientOpenUrl(url, NULL, NULL, 1, 
						jxj_cu_write_frame, jxj_cu_begin_write_frame,
						jxj_cu_end_write_frame,
						pStream, &pStream->lRealPlayHandle);
	jxj_cu_ipnc_mng_ref(pMng);
	*pStreamHandle = (long)pStream;
	
	return 0;
}

//停止写帧
int jxj_cu_stop_write_frame(long ipncHandle, long streamHandle)
{
	JXJ_CU_PROTOCOL_MNG *pMng = (JXJ_CU_PROTOCOL_MNG *)ipncHandle;
	JXJ_CU_STREAM_S *pStream = (JXJ_CU_STREAM_S *)streamHandle;
	
	if(!jxj_cu_ipnc_mng_check_ipnc_handle_valid(pMng, pStream))
	{
		__ERR("input param of stopping get stream is wrong.\n");
		return -1;
	}
	if(pMng->stream[0] != pStream && pMng->stream[1] != pStream)
	{
		__ERR("The ipnc stream handle is wrong.\n");
		return -1;
	}

	if(--pStream->refCount == 0 )
	{
		if(pMng->stream[0] == pStream)
			pMng->stream[0] = NULL;
		else
			pMng->stream[1] = NULL;
			
		rtspClientCloseUrl(pStream->lRealPlayHandle);
		free(pStream);
		jxj_cu_ipnc_mng_unref(pMng);
	}

	return 0;
}

//强制I帧
int jxj_cu_get_request_iframe(long ipncHandle, long streamHandle)
{

	JXJ_CU_PROTOCOL_MNG *pMng = (JXJ_CU_PROTOCOL_MNG *)ipncHandle;
	JXJ_CU_STREAM_S *pStream = (JXJ_CU_STREAM_S *)streamHandle;

	if(!jxj_cu_ipnc_mng_check_ipnc_handle_valid(pMng, pStream))
	{
		__ERR("input param of request iframe get stream is wrong.\n");
		return -1;
	}
	if(pMng->stream[0] != pStream && pMng->stream[1] != pStream)
	{
		__ERR("The ipnc stream handle is wrong.\n");
		return -1;
	}
	//请求I帧
	rtspClientRequestIFrame(pStream->lRealPlayHandle);
	
	return 0;
}

