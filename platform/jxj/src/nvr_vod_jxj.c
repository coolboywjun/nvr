

#include <nvr_headers.h>
#include <av_pool.h>
#include <storage_lib.h>

#include "nvr_platform_jxj.h"
#include "nvr_param_jxj.h"

#include "j_sdk.h"
#include "media_structs.h"
#include "j_sdk_inters.h"


typedef struct _JXJ_VOD_HANDLE_S
{
	unsigned long uSendVideoFrame;
	unsigned long uSendAudioFrame;
	int bReadEnd;

	long lReadHandle;
	unsigned char *pFrameBuff;
	RecordQueryInfo stQueryInfo;
	StorageFragmentInfo stFragmentInfo;
}JXJ_VOD_HANDLE_S;


static inline StorageRecordType nvr_convert_rec_type(uint32_t nRecType)
{
	StorageRecordType recordType = 0;
	
	if(nRecType == ALL_RECODE)
		recordType = RECORD_ALL_TYPE;
	else
	{
		if(nRecType & TIMER_RECODE)
			recordType |= RECORD_TIME_TYPE;
		if(nRecType & ALARM_RECODE)
			recordType |= RECORD_ALARM_TYPE;
		if(nRecType & MOVE_RECODE)
			recordType |= RECORD_ALARM_TYPE;
		if(nRecType & MANUAL_RECODE)
			recordType |= RECORD_STARTUP_TYPE;
		if(nRecType & LOST_RECODE)
			recordType |= RECORD_ALARM_TYPE;
		if(nRecType & HIDE_RECODE)
			recordType |= RECORD_ALARM_TYPE;
		if(nRecType & NET_FAULT_RECODE)
			recordType |= RECORD_ALARM_TYPE;
	}
	return recordType;
}

static inline int nvr_reconvert_rec_type(StorageRecordType recordType)
{
	int nType = 0;
	
	if(recordType & RECORD_STARTUP_TYPE)
		nType |= MANUAL_RECODE;
	if(recordType & RECORD_TIME_TYPE)
		nType |= TIMER_RECODE;
	if(recordType & RECORD_ALARM_TYPE)
		nType |= MOVE_RECODE;
	return nType;
}


static int nvr_get_h264_rec_sps_pps(char *frameBuffer, int frrameSize, 
									unsigned char *spsBuf, unsigned short *spsBufLen,
									unsigned char *ppsBuf, unsigned short *ppsBufLen)
{
	int nCurPos;
	int nNalBeginPos;
	int nNalEndPos;
	int nNalLen;
	int nNalSize;
	int tmpSpsBufLen = *spsBufLen;
	int tmpPpsBufLen = *ppsBufLen;
	char bCopySpsFlag = 0;
	char bCopyPpsFlag = 0;

	nNalBeginPos = -1;
	nNalEndPos = -1;
	for(nCurPos = 0; nCurPos < frrameSize-4; nCurPos++)
	{
		if(frameBuffer[nCurPos] == 0
			&& frameBuffer[nCurPos+1] == 0
			&& frameBuffer[nCurPos+2] == 0
			&& frameBuffer[nCurPos+3] == 1)	// find the nal
		{
			if(nNalBeginPos < 0)
			{
				nNalBeginPos = nCurPos;
			}
			else
			{
				nNalEndPos = nCurPos;
				nNalLen = nNalEndPos - nNalBeginPos;
				switch(frameBuffer[nNalBeginPos + 4] & 0x1f)	// NAL TPYE
				{
					case 7: // Sequence parameter set
						nNalSize = nNalLen - 4;
						if(nNalSize <= tmpSpsBufLen)
						{
							*spsBufLen = nNalSize;
							memcpy(spsBuf, &frameBuffer[nNalBeginPos+4], nNalSize);
							bCopySpsFlag = 1;
						}
						break;
					case 8: // Picture parameter set
						nNalSize = nNalLen - 4;
						if(nNalSize <= tmpPpsBufLen)
						{
							*ppsBufLen = nNalSize;
							memcpy(ppsBuf, &frameBuffer[nNalBeginPos+4], nNalSize);
							bCopyPpsFlag = 1;
						}
						break;
					default:
						break;
				}
				if(bCopySpsFlag && bCopyPpsFlag)		// 找到了sps和pps
					return 0;
				nNalBeginPos = nNalEndPos;
				nNalEndPos = -1;
			}
		}
	}
	// 查找 sps和pps失败
	return -1;
}

static time_t nvr_jxj_convert_time_str_to_time(uint8_t *pTime)
{
	int year, mon, day, hour, min, sec;
	struct tm tm = {0};
	
	if(pTime == NULL)
		return -1;
	if(sscanf((char *)pTime, "%4d%2d%2d%2d%2d%2d", &year, &mon, &day,
					&hour, &min, &sec) != 6)
	{
		j_warning("Failed to scanf time string.");
		return -1;
	}
	j_debug("year: %d, mon : %d, day : %d, hour : %d, min : %d, sec : %d", 
			year, mon, day, hour, min, sec);
	tm.tm_year = year - 1900;
	tm.tm_mon = (mon - 1) % 12;
	tm.tm_mday = day % 32;
	tm.tm_hour = hour % 24;
	tm.tm_min = min % 60;
	tm.tm_sec = sec % 60;
	
	return mktime(&tm);
}

static void nvr_jxj_convert_time_to_jtime(time_t t, JTime *pTime)
{
	struct tm tmTime = {0};
	struct tm *ptm;

	ptm = localtime_r(&t, &tmTime);
	pTime->year = ptm->tm_year;
	pTime->month = ptm->tm_mon + 1;
	pTime->date = ptm->tm_mday;
	pTime->weekday = ptm->tm_wday;
	pTime->hour = ptm->tm_hour;
	pTime->minute = ptm->tm_min;
	pTime->second = ptm->tm_sec;
}

static time_t nvr_jxj_convert_jtime_to_time(JTime *pTime)
{
	struct tm tmTime = {0};

	tmTime.tm_year = pTime->year;
	tmTime.tm_mon = (pTime->month - 1) % 12;
	tmTime.tm_mday= pTime->date % 32;
	tmTime.tm_hour = pTime->hour % 24; 		
	tmTime.tm_min  = pTime->minute % 60;		
	tmTime.tm_sec  = pTime->second % 60;

	return mktime(&tmTime);
}

int nvr_jxj_query(int channel, JStoreLog *pStoreLog)
{
	int i, nCnt;
	int nOft = 0;
	int nShmid;
	int nQueryNum;
	time_t nStartTime, nEndTime;
	RecordQueryChnInfo *pChnInfo;
	RecordQueryInfo *pQueryInfo;
	
	if(channel < 0 || channel > JXJ_CHN_MAX || pStoreLog == NULL)
		return -1;

	j_message("Begin to start query");
	nStartTime = nvr_jxj_convert_jtime_to_time(&pStoreLog->beg_time);
	nEndTime = nvr_jxj_convert_jtime_to_time(&pStoreLog->end_time);
	if(nStartTime < 0 || nEndTime < 0)
	{
		j_warning("The input start time or end time format error!");
		return -1;
	}
	nQueryNum = storage_lib_record_query(1<<channel,
										nStartTime, nEndTime, 
										nvr_convert_rec_type(pStoreLog->rec_type), 
										&nShmid, &pChnInfo, &pQueryInfo);
	if(nQueryNum < 0)
	{
		j_warning("Record query failed");
		return -1;
	}
#if 0
	printf("chn num: %d", pChnInfo->max_chn);
	printf("fragment num: %d", nQueryNum);
	for(i = 0; i < pChnInfo->max_chn; i++)
	{
		printf("chn%d fragment num: %d", i, pChnInfo->query_info_num[i]);
	}
	for(i = 0; i < nQueryNum; i++)
	{
		printf("chn%d disk_idx: %d basket_idx: %d bf_idx: %d begin_iframe_idx: %d iframe_num: %d btime: %ld etime: %ld", 
			pQueryInfo[i].chn,
			pQueryInfo[i].disk_idx, pQueryInfo[i].basket_idx,
			pQueryInfo[i].bf_idx, pQueryInfo[i].begin_iframe_idx,
			pQueryInfo[i].iframe_num, pQueryInfo[i].begin_time, 
			pQueryInfo[i].end_time);
	}
#endif
	if(pChnInfo->query_info_num[channel] <= 0)
	{
		j_debug("Don't find the fragment");
		storage_lib_record_query_free(nShmid, pChnInfo, pQueryInfo);
		pStoreLog->node_count = 0;
		pStoreLog->total_count = 0;
		return 0;
	}
	for(i = 0; i < channel; i++)
		nOft += pChnInfo->query_info_num[i];
	if(pChnInfo->query_info_num[i] <= pStoreLog->beg_node)
	{
		pStoreLog->node_count = 0;
		pStoreLog->total_count = 0;//pChnInfo->query_info_num[channel];
		storage_lib_record_query_free(nShmid, pChnInfo, pQueryInfo);
		return 0;
	}
	for(nCnt= 0, i = pStoreLog->beg_node; 
		i <= pStoreLog->end_node && nCnt < J_SDK_MAX_STORE_LOG_SIZE && i < pChnInfo->query_info_num[channel]; 
		i++, nCnt++)
	{
		pStoreLog->store[nCnt].rec_type = nvr_reconvert_rec_type(pQueryInfo[nOft + i].record_type);
		pStoreLog->store[nCnt].file_size = pQueryInfo[nOft + i].fragment_size;
		nvr_jxj_convert_time_to_jtime(pQueryInfo[nOft + i].begin_time, &pStoreLog->store[nCnt].beg_time);
		nvr_jxj_convert_time_to_jtime(pQueryInfo[nOft + i].end_time, &pStoreLog->store[nCnt].end_time);	
	}
	j_debug("beg_node: %d, end_node: %d", pStoreLog->beg_node, pStoreLog->end_node);
	j_debug("node_count: %d, total_count: %d", nCnt, pChnInfo->query_info_num[channel]);
	pStoreLog->node_count = nCnt;
	pStoreLog->total_count = pChnInfo->query_info_num[channel];
	storage_lib_record_query_free(nShmid, pChnInfo, pQueryInfo);

	j_message("End to start query");
	return 0;
}

static int nvr_jxj_find(int channel, uint8_t *start_time, uint8_t *end_time, RecordQueryInfo *pQueryInfoNode)
{
	int i, nOft = 0;
	int nShmid;
	int nQueryNum;
	time_t nStartTime, nEndTime;
	RecordQueryChnInfo *pChnInfo;
	RecordQueryInfo *pQueryInfo;
	
	nStartTime = nvr_jxj_convert_time_str_to_time(start_time);
	nEndTime = nvr_jxj_convert_time_str_to_time(end_time);
	if(nStartTime < 0 || nEndTime < 0)
	{
		j_warning("Failed to convert time string to time.");
		return -EFAULT;
	}
	j_debug("converted start time : %ld", nStartTime);
	j_debug("converted end time :  %ld", nEndTime);
	
	nQueryNum = storage_lib_record_query(1<<channel, nStartTime, nEndTime, RECORD_ALL_TYPE,
					&nShmid, &pChnInfo, &pQueryInfo);
	if(nQueryNum < 0)
	{
		j_warning("Record query failed");
		return -EFAULT;
	}
	
	j_debug("chn num: %d", pChnInfo->max_chn);
	j_debug("fragment num: %d", nQueryNum);
#if 0	
	for(i = 0; i < pChnInfo->max_chn; i++)
	{
		j_debug("chn%d fragment num: %d", i, pChnInfo->query_info_num[i]);
	}
	for(i = 0; i < nQueryNum; i++)
	{
		j_debug("chn%d disk_idx: %d basket_idx: %d bf_idx: %d begin_iframe_idx: %d iframe_num: %d btime: %ld etime: %ld\n", 
			pQueryInfo[i].chn,
			pQueryInfo[i].disk_idx, pQueryInfo[i].basket_idx,
			pQueryInfo[i].bf_idx, pQueryInfo[i].begin_iframe_idx,
			pQueryInfo[i].iframe_num, pQueryInfo[i].begin_time, 
			pQueryInfo[i].end_time);
	}
#endif
	
	if(pChnInfo->query_info_num[channel] <= 0)
	{
		j_debug("Don't find the fragment");
		storage_lib_record_query_free(nShmid, pChnInfo, pQueryInfo);
		return -EFAULT;
	}
	for(i = 0; i < channel; i++)
		nOft += pChnInfo->query_info_num[i];
	for(i = nOft; i < pChnInfo->query_info_num[channel] + nOft; i++)
	{
		if(pQueryInfo[i].begin_time == nStartTime)
			break;
	}
	if(i == pChnInfo->query_info_num[channel] + nOft)
	{
		j_warning("Don't find the fragment");
		storage_lib_record_query_free(nShmid, pChnInfo, pQueryInfo);
		return -EFAULT;
	}
	memcpy(pQueryInfoNode, &pQueryInfo[i], sizeof(RecordQueryInfo));
	storage_lib_record_query_free(nShmid, pChnInfo, pQueryInfo);

	return 0;
}

static int32_t hs_probe_video(void *pFrameBuff, media_info_t *mi)
{
	AV_FRAME_HEAD_S *pFrameHead = (AV_FRAME_HEAD_S *)pFrameBuff;
	int nRet;

	j_debug("Begin to probe video");

	if(pFrameHead->u8FrameType != AV_VIDEO_FRAME_I || pFrameHead->unVidoeInfo.u8EncodeType != 0)
	{
		j_warning("The video frame type is wrong.");
		return -1;
	}	
	mi->mask |= MEDIA_VIDEO;	// 一定支持视频
	mi->video.enc_type = ENC_H264;
	mi->video.bitrate = 4096;		// 4M
	mi->video.samples_per_sec = pFrameHead->unVidoeInfo.u8FrameRate;
	mi->video.frame_rate = pFrameHead->unVidoeInfo.u8FrameRate;
	mi->video.pic_parm.sps_size = sizeof(mi->video.pic_parm.sps);
	mi->video.pic_parm.pps_size = sizeof(mi->video.pic_parm.pps);
	nRet = nvr_get_h264_rec_sps_pps(pFrameBuff + sizeof(AV_FRAME_HEAD_S), pFrameHead->u32FrameSize, 
						mi->video.pic_parm.sps, &mi->video.pic_parm.sps_size,
						mi->video.pic_parm.pps, &mi->video.pic_parm.pps_size);
#ifdef DEBUG_JXJ_VOD
	if(nRet == 0)
	{
		int i;
		printf("===================== SPS =================");
		for(i = 0; i<mi->video.pic_parm.sps_size; i++)
		{
			printf("%02X ", mi->video.pic_parm.sps[i]);
			if(i % 8 == 7)
				printf("");
		}
		printf("===================== PPS =================");
		for(i = 0; i<mi->video.pic_parm.pps_size; i++)
		{
			printf("%02X ", mi->video.pic_parm.pps[i]);
			if(i % 8 == 7)
				printf("");
		}
		printf("");
	}
#endif
	j_debug("End to probe video");
	return nRet;
}

static int32_t hs_probe_audio(int record_type, media_info_t *mi)
{
	j_debug("Begin to probe audio");
	mi->mask |= MEDIA_AUDIO;
	mi->audio.enc_type = (record_type == RECORD_AUDIO_G711A) ? ENC_G711A : ENC_G711U;
	mi->audio.bitrate = 64;
	mi->audio.samples_per_sec = 8000;
	mi->audio.bits_per_sample = 16;
	mi->audio.frame_rate = (mi->audio.samples_per_sec * mi->audio.bits_per_sample /16)/160;	// 50fps
	mi->audio.channel_num = 1;	// 单通道

	// 不需要audio data, 直接增加偏移值
	//sdk_record_lseek_offset(pRecRead, pFrameHead->u32FrameSize);
	
	j_debug("End to probe audio");
	return 0;
}

static int32_t hs_probe_cb(int32_t channel, int32_t level, int32_t type,       
        uint8_t *start_time, uint8_t *end_time, uint8_t *property,          /* 时间格式:YYYYMMDDHHMMSS */
        media_info_t *mi)
{
	int nFrameLen;
	RecordQueryInfo stQueryInfo;
	StorageFragmentInfo stFragmentInfo;
	long lReadHandle;
	unsigned char *pFrameBuff;
	int bFindVideo = 0;

	j_debug("Begin to probe record --> start time:%s, end time:%s", start_time, end_time);
	j_debug("channel : %d, level : %d, type : %d", channel, level, type);
	if(channel < 0 || channel >= JXJ_CHN_MAX)
		return -EFAULT;
//	memset(mi, 0, sizeof(media_info_t));
	if(nvr_jxj_find(channel, start_time, end_time, &stQueryInfo) < 0)
	{
		j_warning("Find the fragment failed");
		return -EFAULT;
	}
	if(storage_lib_read_start(&stQueryInfo, &stFragmentInfo, &lReadHandle) < 0)
	{
		j_warning("Storage read start failed");
		return -EFAULT;
	}
	if(stFragmentInfo.record_type & RECORD_AUDIO_G711A)
		hs_probe_audio(RECORD_AUDIO_G711A, mi);
	else if((stFragmentInfo.record_type & RECORD_AUDIO_G711U))
		hs_probe_audio(RECORD_AUDIO_G711U, mi);
	while((nFrameLen = storage_lib_read_frame(lReadHandle, READ_NORMAL, NULL, &pFrameBuff)) > 0)
	{
		AV_FRAME_HEAD_S *pFrameHdr = (AV_FRAME_HEAD_S*)pFrameBuff;
		if(bFindVideo == 0 && pFrameHdr->u8FrameType == AV_VIDEO_FRAME_I 
			&& hs_probe_video(pFrameBuff, mi) == 0)
		{
		       bFindVideo = 1;
		       break;
		}
	}
	storage_lib_read_stop(lReadHandle);
	j_debug("End to probe record --> start time:%s, end time:%s", start_time, end_time);
	if(bFindVideo)
		return 0;
	else
	return -EFAULT;
}

static int32_t hs_open_cb(avs_media *m, int32_t channel, int32_t level,
        int32_t type, uint8_t *start_time, uint8_t *end_time, uint8_t *property)
{
	JXJ_VOD_HANDLE_S *pVodHandle;
	RecordQueryInfo *pQueryInfo;

	j_debug("Begin to open record --> start time:%s, end time:%s", start_time, end_time);
	j_debug("channel : %d, level : %d, type : %d", channel, level, type);
	if(gIsSetReboot)
		return -EFAULT;
	pVodHandle = g_malloc0(sizeof(JXJ_VOD_HANDLE_S));
	pQueryInfo = &pVodHandle->stQueryInfo;
	if(nvr_jxj_find(channel, start_time, end_time, pQueryInfo) < 0)
	{
		j_warning("Find the fragment failed");
		g_free(pVodHandle);
		return -EFAULT;
	}	
	
	if(storage_lib_read_start(pQueryInfo, &pVodHandle->stFragmentInfo,
							&pVodHandle->lReadHandle) < 0)
	{
		j_warning("Storage read start failed");
		g_free(pVodHandle);
		return -EFAULT;
	}
	avs_media_set_u(m, pVodHandle);
	
	j_debug("End to open record.");
	return 0;
}
static int32_t hs_play_cb(avs_media *m)
{
	JXJ_VOD_HANDLE_S *pVodHandle = NULL;
	
	j_debug("Begin to play record.");

	pVodHandle = (JXJ_VOD_HANDLE_S *)avs_media_get_u(m);
	if(pVodHandle == NULL)
	{
		j_warning("Failed to get record read pointer form media.");
		return -EFAULT;
	}
	
	j_debug("End to play record");
	return 0;
}

static int32_t hs_pause_cb(avs_media *m)
{
	JXJ_VOD_HANDLE_S *pVodHandle = NULL;

	j_debug("Begin to pause record");

	pVodHandle = (JXJ_VOD_HANDLE_S *)avs_media_get_u(m);
	if(pVodHandle == NULL)
	{
		j_warning("Failed to get record read pointer form media.");
		return -EFAULT;
	}	
	
	j_debug("End to pause record");
	return 0;
}

static int32_t hs_lseek_cb(avs_media *m, uint32_t ts)
{
	JXJ_VOD_HANDLE_S *pVodHandle = NULL;
	int nRet = 0;
	
	j_debug("Begin to lseek record");

	pVodHandle = (JXJ_VOD_HANDLE_S *)avs_media_get_u(m);
	if(pVodHandle == NULL)
	{
		j_warning("Failed to get record read pointer form media.");
		return -EFAULT;
	}
	//j_debug("################## ts: %u", ts);
	//j_debug("#################### fragment %ld - %ld", pVodHandle->stQueryInfo.begin_time, pVodHandle->stQueryInfo.end_time);
	storage_lib_read_set(pVodHandle->lReadHandle, ts);
	j_debug("End to lseek record");
	return nRet;
}

static void hs_close_cb(avs_media *m)    
{
	JXJ_VOD_HANDLE_S *pVodHandle = NULL;

	j_debug("Begin to close record");

	pVodHandle = (JXJ_VOD_HANDLE_S *)avs_media_get_u(m);
	if(pVodHandle == NULL)
	{
		j_warning("Failed to get record read pointer form media.");
		return;
	}
	storage_lib_read_stop(pVodHandle->lReadHandle);
	g_free(pVodHandle);
	j_debug("End to close record");
}

static int32_t hs_pull_cb(avs_media *m, frame_t **frm)
{
	JXJ_VOD_HANDLE_S *pVodHandle = NULL;
	AV_FRAME_HEAD_S *pFrameHdr;
	unsigned char *pFrameBuff;
    	frame_t *fr;
	frame_hdr_t *pAvsFrameHdr;
	int nFrameSize;
	StorageFrameHdr stFrameHdr;
	
	pVodHandle = (JXJ_VOD_HANDLE_S *)avs_media_get_u(m);
	if(pVodHandle == NULL)
	{
		j_message("Failed to get record read pointer form media.");
		return -EFAULT;
	}
	nFrameSize = storage_lib_read_frame(pVodHandle->lReadHandle, READ_NORMAL, &stFrameHdr, &pFrameBuff);
	if(nFrameSize == 0)
	{
		// 读到了录像结尾处
		if(pVodHandle->bReadEnd == 0)
		{
		#define END_FRAME_SIZE 	8
			pVodHandle->bReadEnd = 1;
			
			fr = avs_alloc_frame(END_FRAME_SIZE, 0);
			//memset(fr->data, 0, END_FRAME_SIZE);
			strcpy((char *)fr->data, "end");
			fr->nal_desc->nal_num = 0;
			pAvsFrameHdr = &fr->hdr;

			pAvsFrameHdr->no = 0;
			pAvsFrameHdr->enc_type = ENC_H264;
			pAvsFrameHdr->timestamp = 0;
			pAvsFrameHdr->type = FRAME_EOF;
			pAvsFrameHdr->v.width = 0;
			pAvsFrameHdr->v.height = 0;
			pAvsFrameHdr->v.utc_time = 0;
			pAvsFrameHdr->size = END_FRAME_SIZE;
			
			*frm = fr;
			j_message("Send the eof frame, total send video frame: %lu, total send audio frame: %lu.", 
			        pVodHandle->uSendVideoFrame, pVodHandle->uSendAudioFrame);
			
			return 0;
		}
		else
		{
			j_warning("It's record file end.");
			return -EFAULT;
		}
	}
	else if(nFrameSize < 0)
	{
		j_warning("The frame size is wrong, frame size: %d", nFrameSize);
		return -EFAULT;
	}
	pFrameHdr = (AV_FRAME_HEAD_S *)pFrameBuff;
	
	fr = avs_alloc_frame(pFrameHdr->u32FrameSize, 0);
	if(fr == NULL)
	{
		j_warning("Failed to malloc for frame.");
		return -EFAULT;
	}
	memcpy(fr->data, pFrameBuff + sizeof(AV_FRAME_HEAD_S), pFrameHdr->u32FrameSize);
	pAvsFrameHdr = &fr->hdr;
	pAvsFrameHdr->no = pFrameHdr->u32FrameNo;
	if(pFrameHdr->u8FrameType == AV_AUDIO_FRAME_A)
	{
		if(pFrameHdr->unAudioInfo.u16EncodeType == 1)	// G711A
			pAvsFrameHdr->enc_type = ENC_G711A;
		else
			pAvsFrameHdr->enc_type = ENC_G711U;
		pAvsFrameHdr->type =  FRAME_A;
		// 填充音频头扩展信息
		pAvsFrameHdr->a.samples_per_sec = SAMPLE_8K;
		pAvsFrameHdr->a.bits_per_sample = 16;
		pAvsFrameHdr->a.channel_num = 1;
		pAvsFrameHdr->a.reserved = 0;
		
		pVodHandle->uSendAudioFrame++;
		if(pVodHandle->uSendAudioFrame % 1000 == 0)
		{
			j_message("Send record audio frame %lu", pVodHandle->uSendAudioFrame);
		}
	}
	else
	{
		pAvsFrameHdr->enc_type = ENC_H264;
		if(pFrameHdr->u8FrameType == AV_VIDEO_FRAME_I)
			pAvsFrameHdr->type =  FRAME_I;
		else
			pAvsFrameHdr->type =  FRAME_P;
		// 填充视频头扩展信息
		pAvsFrameHdr->v.width = pFrameHdr->unVidoeInfo.u16ImageWidth;
		pAvsFrameHdr->v.height = pFrameHdr->unVidoeInfo.u16ImageHeight;
		pAvsFrameHdr->v.utc_time = pFrameHdr->u32Time;
		
		pVodHandle->uSendVideoFrame++;
		if(pVodHandle->uSendVideoFrame % 1000 == 0)
		{
			j_message("Send record video frame %lu", pVodHandle->uSendVideoFrame);
		}
	}
	//j_debug("sec: %ld, used: %ld", stFrameHdr.ts.tv_sec, stFrameHdr.ts.tv_sec,stFrameHdr.ts.tv_usec);
	pAvsFrameHdr->timestamp = stFrameHdr.ts.tv_sec * 1000 + stFrameHdr.ts.tv_usec / 1000;//(pFrameHdr->u64TimeStamp/1000);
	pAvsFrameHdr->size = pFrameHdr->u32FrameSize;
	//j_debug("frametype: %d\n", pFrameHdr->u8FrameType);
	//j_debug("frame size : %d\n", pFrameHdr->u32FrameSize);

	// 填充NAL信息, 设置为NULL, 平台库内部进行计算
	fr->nal_desc->nal_num = 0;
	
	*frm = fr;
	
	return 0;
}

// 历史流回调接口
hs_avs_ops m_hs_avs_cb = 
{
	.probe = hs_probe_cb,
	.open = hs_open_cb,
	.play = hs_play_cb,
	.pause = hs_pause_cb,
	.lseek = hs_lseek_cb,
	.close = hs_close_cb,
	.pull = hs_pull_cb,
};



