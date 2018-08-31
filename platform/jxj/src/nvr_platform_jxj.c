

#include <av_pool.h>
#include <nvr_lib_interface.h>

#include "nvr_platform_jxj.h"
#include "nvr_param_jxj.h"
#include "nvr_vod_jxj.h"
#include "nvr_talk_jxj.h"
#include "nvr_struct.h"
#include "nvr_headers.h"

#include "syscfg.h"


#undef	G_LOG_DOMAIN
#define	G_LOG_DOMAIN "platform"

//#define DEBUG_JXJ_MEDIA	1

//#define JXJ_CLOUD_DEF_SVR_IP			"192.168.5.13"
#define JXJ_CLOUD_DEF_SVR_IP			"www.ipdcs001.com"
#define JXJ_CLOUD_DEF_SVR_PORT		6903

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/
typedef struct _JXJ_STREAM_HANDLE_S
{
	int index;
	int channel;
	int streamType;
	int used;
	int get_h264;
	volatile int sta;
	volatile int quit;
	int videoCnt;			
	int audioCnt;
	pthread_t thid;
	avs_media *avsMedia;
	long lReadId;
	
}JXJ_STREAM_HANDLE_S;

enum
{
	MEDIA_IDLE = 0,
	MEDIA_USED,
	MEDIA_START,
	MEDIA_RUN,
};

#define STREAM_HANDLE_NUM 16

static JXJ_STREAM_HANDLE_S gStreamHandle[STREAM_HANDLE_NUM];
static int gTotalConnect = 0;
static int gCallOpenCnt = 0;
static int gCallCloseCnt = 0;
static int gCallProbeCnt = 0;
static int gCallPlayCnt = 0;
static int gCallCtrlCnt = 0;
static pthread_mutex_t mutex;


static JXJ_STREAM_HANDLE_S *nvr_get_idle_stream_handle(int channelNo, int level)
{
	int i;
	JXJ_STREAM_HANDLE_S *pStreamHandle = NULL;

	pthread_mutex_lock(&mutex);
	for(i = 0; i<STREAM_HANDLE_NUM; i++)
	{
		if(gStreamHandle[i].used == 0)
		{
			pStreamHandle = &gStreamHandle[i];
			break;
		}
	}
	if(pStreamHandle == NULL)
	{	
		pthread_mutex_unlock(&mutex);
		return NULL;
	}
	
	memset(pStreamHandle, 0, sizeof(JXJ_STREAM_HANDLE_S));
	pStreamHandle->index = i;
	pStreamHandle->channel = channelNo;
	pStreamHandle->streamType = level;
	pStreamHandle->used = 1;

	gTotalConnect++;
	j_message("The jxj platform connect number is %d", gTotalConnect);
	pthread_mutex_unlock(&mutex);

	return pStreamHandle;
}


static void nvr_free_stream_handle(JXJ_STREAM_HANDLE_S *pStreamHandle)
{
	if(pStreamHandle == NULL)
	{
		j_warning("Thr free stream handle is null.");
		return;
	}

	pthread_mutex_lock(&mutex);
	memset(pStreamHandle, 0, sizeof(JXJ_STREAM_HANDLE_S));
	gTotalConnect--;
	j_message("The jxj platform connect number is %d.", gTotalConnect);
	pthread_mutex_unlock(&mutex);
}

static int nvr_start_preview(JXJ_STREAM_HANDLE_S *pStreamHandle)
{
	int nChn;
	int nStream;
	
	nChn = pStreamHandle->channel;
	nStream = pStreamHandle->streamType;
	j_debug("nChn:%d, nStream:%d", nChn, nStream);
	if(av_pool_start_read_frame(nChn, nStream, 1000, &pStreamHandle->lReadId) < 0)
	{
		j_warning("Start read av pool chn%d stream%d failed.", nChn, nStream);
		return -1;
	}
	
	return 0;
}

static void nvr_stop_preview(JXJ_STREAM_HANDLE_S *pStreamHandle)
{
	av_pool_stop_read_frame(pStreamHandle->lReadId);
}

static int nvr_get_h264_sps_pps(int nChn, int nStreamType, 
		unsigned char *spsBuf, unsigned short *spsBufLen,
		unsigned char *ppsBuf, unsigned short *ppsBufLen)
{
	int nTryNum;
	int nFrameSize;
	int nCurPos;
	int nNalBeginPos;
	int nNalEndPos;
	int nNalLen;
	int nNalSize;
	int tmpSpsBufLen = *spsBufLen;
	int tmpPpsBufLen = *ppsBufLen;
	char *pBuf;
	int  bufSize;
	char bCopySpsFlag = 0;
	char bCopyPpsFlag = 0;
	AV_FRAME_HEAD_S *pFrameHdr = NULL;
	long lReadId;
	
	if(av_pool_start_read_frame(nChn, nStreamType, 1000, &lReadId) < 0)
	{
		j_warning("Start read av pool failed.");
		return -1;
	}
	
	for(nTryNum=0; nTryNum < 500; nTryNum++)
	{
		pBuf = NULL;
		bufSize = 0;
		nFrameSize = av_pool_read_frame(lReadId, &pBuf, &bufSize, -1);	
		//j_message("nFrameSize = %d", nFrameSize);
		if(nFrameSize <= 0)
		{
			usleep(10000);
			continue;
		}

		pFrameHdr = (AV_FRAME_HEAD_S*)pBuf;
		if(pFrameHdr->u8FrameType != AV_VIDEO_FRAME_I)
			continue;
		nNalBeginPos = -1;
		nNalEndPos = -1;
		bCopySpsFlag = 0;
		bCopyPpsFlag = 0;

		for(nCurPos = sizeof(AV_FRAME_HEAD_S); nCurPos < nFrameSize-4; nCurPos++)
		{
			if(pBuf[nCurPos] == 0
					&& pBuf[nCurPos+1] == 0
					&& pBuf[nCurPos+2] == 0
					&& pBuf[nCurPos+3] == 1)	// find the nal
			{
				if(nNalBeginPos < 0)
				{
					nNalBeginPos = nCurPos;
				}
				else
				{
					nNalEndPos = nCurPos;
					nNalLen = nNalEndPos - nNalBeginPos;
					switch(pBuf[nNalBeginPos + 4] & 0x1f)	// NAL TPYE
					{
						case 7: // Sequence parameter set
							nNalSize = nNalLen - 4;
							if(nNalSize <= tmpSpsBufLen)
							{
								*spsBufLen = nNalSize;
								memcpy(spsBuf, &pBuf[nNalBeginPos+4], nNalSize);
								bCopySpsFlag = 1;
							}
							break;
						case 8: // Picture parameter set
							nNalSize = nNalLen - 4;
							if(nNalSize <= tmpPpsBufLen)
							{
								*ppsBufLen = nNalSize;
								memcpy(ppsBuf, &pBuf[nNalBeginPos+4], nNalSize);
								bCopyPpsFlag = 1;
							}
							break;
						default:
							break;
					}
					if(bCopySpsFlag && bCopyPpsFlag)
						goto exit;
					nNalBeginPos = nNalEndPos;
					nNalEndPos = -1;
				}
			}
		}
	}
	
exit:
	av_pool_stop_read_frame(lReadId);
	if(nTryNum == 1000)
		return -1;
	return 0;
}

static inline void ls_print_cnt()
{
#ifdef DEBUG_JXJ_MEDIA
	j_message("total connect: %d", gTotalConnect);
	j_message("total open: %d", gCallOpenCnt);
	j_message("total close: %d", gCallCloseCnt);
	j_message("total probe: %d", gCallProbeCnt);
	j_message("total play: %d", gCallPlayCnt);
	j_message("total ctrl: %d", gCallCtrlCnt);
#endif
}

static int nvr_get_nalDesc(int nFrameType, char *pFrameBuf, int nFrameSize, nal_desc_t *pNalDesc)
{
	int nCurPos;
	int nNalEndPos = -1;
	int nNalBeginPos = -1;
	int nNalLen;
	int i = 0;
	int nMax_num;

	if(nFrameType == FRAME_P)
	{
		nMax_num = 2;	//P帧含的Nal数为2，即分别是：0x06,0x61。
	}else
	{
		nMax_num = 4;	//I帧含的nal数为4，即分别是：0x67,0x68,0x06,x0x65
	}

	memset(pNalDesc, 0, sizeof(nal_desc_t));
	for(nCurPos = 0; nCurPos < nFrameSize-4; nCurPos++)
	{
		if(pFrameBuf[nCurPos] == 0
				&& pFrameBuf[nCurPos+1] == 0
				&& pFrameBuf[nCurPos+2] == 0
				&& pFrameBuf[nCurPos+3] == 1)	// find the nal
		{
			if(nNalBeginPos < 0)
			{
				nNalBeginPos = nCurPos;
			}
			else
			{
				nNalEndPos = nCurPos;
				nNalLen = nNalEndPos - nNalBeginPos;
				pNalDesc->nal[i].nal_off = nNalBeginPos;
				pNalDesc->nal[i].nal_size = nNalLen;
				nNalBeginPos = nNalEndPos;
				i++;
				pNalDesc->nal_num++;
				nNalEndPos = - 1;	
				if(pNalDesc->nal_num == nMax_num - 1)
				{
					break;
				}
			}
		}
	}
	if(nNalBeginPos >= 0 && nNalEndPos < 0)
	{
		nNalLen = nFrameSize - nNalBeginPos;
		pNalDesc->nal[i].nal_off = nNalBeginPos;
		pNalDesc->nal[i].nal_size = nNalLen;
		pNalDesc->nal_num++;
	}
	return 0;
}

static int ls_send_video(JXJ_STREAM_HANDLE_S *pStreamHandle, char *pFrameBuf, int nFrameSize)
{
	avs_media *m = pStreamHandle->avsMedia;
	frame_t avsFrame;
	frame_hdr_t *pAvsFrameHdr = &avsFrame.hdr;

	AV_FRAME_HEAD_S *pFrameHdr = (AV_FRAME_HEAD_S*)pFrameBuf;
	char nalDescBuff[128];
	nal_desc_t *pNalDesc = (nal_desc_t *)nalDescBuff;

	// 填充avs 帧头数据
	pAvsFrameHdr->no = pFrameHdr->u32FrameNo;
	pAvsFrameHdr->enc_type = ENC_H264;
	pAvsFrameHdr->timestamp = pFrameHdr->u64TimeStamp/1000;

	if(pFrameHdr->u8FrameType == AV_VIDEO_FRAME_I)
		pAvsFrameHdr->type =  FRAME_I;
	else
		pAvsFrameHdr->type =  FRAME_P;
	pAvsFrameHdr->size = nFrameSize - sizeof(AV_FRAME_HEAD_S);	// 不需要之前的视频头NVR_FRAME_HEAD_S

	// 填充视频头扩展信息
	pAvsFrameHdr->v.width = pFrameHdr->unVidoeInfo.u16ImageWidth;
	pAvsFrameHdr->v.height = pFrameHdr->unVidoeInfo.u16ImageHeight;
	pAvsFrameHdr->v.utc_time = pFrameHdr->u32Time;

	// 查找nal信息
	nvr_get_nalDesc(pAvsFrameHdr->type, pFrameBuf+sizeof(AV_FRAME_HEAD_S), nFrameSize-sizeof(AV_FRAME_HEAD_S), pNalDesc);

	// 填充NAL信息, 设置为NULL, 平台库内部进行计算
	avsFrame.nal_desc = pNalDesc;

	// 指定帧数据, 去掉之前的帧头数据

	avsFrame.data = (uint8_t*)pFrameBuf+ sizeof(AV_FRAME_HEAD_S);
	// 每发送1000帧记录发送帧数
	pStreamHandle->videoCnt++;
	if(pStreamHandle->videoCnt % 10000 == 0)
	{
		j_message("Send video %d frames to JXJ platform!", pStreamHandle->videoCnt);
	}
	return avs_media_fill(m, &avsFrame);
}

static int ls_send_audio(JXJ_STREAM_HANDLE_S *pStreamHandle, char *pFrameBuf, int nFrameSize)
{
	avs_media *m = pStreamHandle->avsMedia;
	frame_t avsFrame;
	frame_hdr_t *pAvsFrameHdr = &avsFrame.hdr;
	//nal_desc_t *pAvsNalDesc = avsFrame.nal_desc;
	AV_FRAME_HEAD_S *pFrameHdr = (AV_FRAME_HEAD_S*)pFrameBuf;
	nal_desc_t nalDesc;

	// 填充avs 帧头数据
	pAvsFrameHdr->no = pFrameHdr->u32FrameNo;
	if(pFrameHdr->unAudioInfo.u16EncodeType == 1)
		pAvsFrameHdr->enc_type = ENC_G711A;
	else if(pFrameHdr->unAudioInfo.u16EncodeType == 2)
		pAvsFrameHdr->enc_type = ENC_G711U;
	else// 默认使用G711A
		pAvsFrameHdr->enc_type = ENC_G711A;
	pAvsFrameHdr->timestamp = pFrameHdr->u64TimeStamp/1000;
	pAvsFrameHdr->type =  FRAME_A;
	pAvsFrameHdr->size = nFrameSize - sizeof(AV_FRAME_HEAD_S);
	//__D("audio frame size : %d", pAvsFrameHdr->size);

	// 填充音频头扩展信息
	pAvsFrameHdr->a.samples_per_sec = SAMPLE_8K;
	pAvsFrameHdr->a.bits_per_sample = 16;
	pAvsFrameHdr->a.channel_num = 1;
	pAvsFrameHdr->a.reserved = 0;

	// 填充NAL信息, 设置为NULL, 平台库内部进行计算
	nalDesc.nal_num = 0;
	avsFrame.nal_desc = &nalDesc;

	// 指定帧数据, 去掉之前的帧头数据
	avsFrame.data = (uint8_t*)pFrameBuf + sizeof(AV_FRAME_HEAD_S);

	// 每发送1000帧记录发送帧数
	pStreamHandle->audioCnt++;
	if(pStreamHandle->audioCnt % 10000 == 0)
	{
		j_message("Send audio %d frames to JXJ platform!", pStreamHandle->audioCnt);
	}
	return avs_media_fill(m, &avsFrame);
}

static void* ls_thread(void *data)
{
	JXJ_STREAM_HANDLE_S *pStreamHandle = (JXJ_STREAM_HANDLE_S*)data;
	AV_FRAME_HEAD_S *pFrameHdr;
	int nFrameSize;
	char *pBuf;
	int nBufLen;
	
	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "JXJPFLS");

	pStreamHandle->sta = MEDIA_RUN;
	av_pool_request_iframe(pStreamHandle->channel, pStreamHandle->streamType);
	while(!pStreamHandle->quit)
	{
		pBuf = NULL;
		nBufLen = 0;
		//printf("begin to get frame.\n");
		nFrameSize = av_pool_read_frame(pStreamHandle->lReadId, &pBuf, &nBufLen, 0);
		if(nFrameSize <= 0)
		{
			usleep(10000);
			continue;
		}
		//printf("frame size: %d\n", nFrameSize);
		pFrameHdr = (AV_FRAME_HEAD_S *)pBuf;
		if(pFrameHdr->u8FrameType == AV_AUDIO_FRAME_A)
			ls_send_audio(pStreamHandle, pBuf, nFrameSize);
		else
			ls_send_video(pStreamHandle, pBuf, nFrameSize);
	}

	avs_media_unref(pStreamHandle->avsMedia);
	nvr_stop_preview(pStreamHandle);
	nvr_free_stream_handle(pStreamHandle);

	return (void *)0;
}

int32_t ls_probe_cb(int32_t channel, int32_t level, media_info_t *mi)
{
	
	j_message("Begin to probe video steam --> channel:%u, level:%d", channel, level);
	if(channel < 0 || channel >=4  || ( level != 0 && level != 1))
	{
		j_warning("The probe channel or the level don't support!");
		return -1;
	}
	pthread_mutex_lock(&mutex);
	gCallProbeCnt++;
	ls_print_cnt();
	pthread_mutex_unlock(&mutex);

	mi->mask = MEDIA_VIDEO | MEDIA_AUDIO;	// 一定支持视频
	mi->begin_time = 0;
	mi->end_time = 0;
	mi->video.enc_type = ENC_H264;

	mi->video.bitrate = 1000;		//////////
	mi->video.samples_per_sec = 25;	////////
	mi->video.frame_rate = 25;		/////////
	mi->video.pic_parm.sps_size = sizeof(mi->video.pic_parm.sps);
	mi->video.pic_parm.pps_size = sizeof(mi->video.pic_parm.pps);

	if(nvr_get_h264_sps_pps(channel, level, 
				mi->video.pic_parm.sps, &mi->video.pic_parm.sps_size,
				mi->video.pic_parm.pps, &mi->video.pic_parm.pps_size) < 0)
	{
		j_warning("Failed to get h264 sps and pps.");
		return -EFAULT;
	}
#ifdef DEBUG_JXJ_MEDIA	
	{
		int i;
		printf("===================== SPS =================\n");
		for(i = 0; i<mi->video.pic_parm.sps_size; i++)
		{
			printf("%02X ", mi->video.pic_parm.sps[i]);
			if(i % 8 == 7)
				printf("\n");
		}
		printf("\n===================== PPS =================\n");
		for(i = 0; i<mi->video.pic_parm.pps_size; i++)
		{
			printf("%02X ", mi->video.pic_parm.pps[i]);
			if(i % 8 == 7)
				printf("\n");
		}
		printf("\n");
	}
#endif
	////////////////////////////////////
	// 设置audio 编码类型
	mi->audio.enc_type = ENC_G711A;
	mi->audio.bitrate = 64;
	mi->audio.samples_per_sec = 8000;
	mi->audio.bits_per_sample = 16;
	mi->audio.frame_rate = (mi->audio.samples_per_sec * mi->audio.bits_per_sample /16)/160;	// 50fps
	mi->audio.channel_num = 1;	// 单通道
	j_message("End to probe video steam --> channel:%u, level:%d", channel, level);
	return 0;
}

int32_t ls_open_cb(avs_media *m, int32_t channel, int32_t level)
{
	JXJ_STREAM_HANDLE_S *pStreamHandle = NULL;

	j_message("Begin to open video steam --> channel:%u, level:%d", channel, level);
	if(channel < 0 || channel >=4  || ( level != 0 && level != 1))
	{
		j_message("The probe channel or the level don't support!");
		return -1;
	}
	
	if(gIsSetReboot)
		return -1;
		
	pthread_mutex_lock(&mutex);
	gCallOpenCnt++;
	ls_print_cnt();
	pthread_mutex_unlock(&mutex);

	pStreamHandle = nvr_get_idle_stream_handle(channel, level);
	if(pStreamHandle == NULL)
	{
		j_warning("Don't find the idle stream handle.");
		return -EFAULT;
	}
	if(nvr_start_preview(pStreamHandle) < 0)
	{
		j_warning("Don't start preview");
		nvr_free_stream_handle(pStreamHandle);
		return -EFAULT;
	}
	avs_media_set_u(m, pStreamHandle);
	pStreamHandle->avsMedia = m;
	pStreamHandle->sta = MEDIA_USED;
	j_message("End to initialize video steam --> channel:%u, level:%d", channel, level);

	return 0;
}

int32_t ls_play_cb(avs_media *m)
{
	JXJ_STREAM_HANDLE_S *pStreamHandle = NULL;

	j_message("Begin to play video steam");

	pthread_mutex_lock(&mutex);
	gCallPlayCnt++;
	ls_print_cnt();
	pthread_mutex_unlock(&mutex);

	pStreamHandle = avs_media_get_u(m);
	if(pStreamHandle == NULL)
	{
		j_warning("The stream handle is null.");
		return -EFAULT;
	}
	j_message("Begin to play video steam --> channel:%u, level:%d", pStreamHandle->channel, pStreamHandle->streamType);
	pStreamHandle->quit = 0;
	pStreamHandle->sta = MEDIA_START;
	avs_media_ref(pStreamHandle->avsMedia);
	if(pthread_create(&pStreamHandle->thid, NULL, ls_thread, pStreamHandle) != 0)
	{
		j_warning("Failed to create video thread.");
		avs_media_set_u(m, NULL);
		avs_media_unref(pStreamHandle->avsMedia);
		nvr_stop_preview(pStreamHandle);
		nvr_free_stream_handle(pStreamHandle);
		return -EFAULT;
	}
	j_message("End to play video steam --> channel:%u, level:%d", pStreamHandle->channel, pStreamHandle->streamType);
	return 0;
}

int32_t ls_ctrl_cb(avs_media *m, uint32_t cmd, void *data)
{
	JXJ_STREAM_HANDLE_S *pStreamHandle = NULL;

	j_message("Begin to control video steam");

	pthread_mutex_lock(&mutex);
	gCallCtrlCnt++;
	ls_print_cnt();
	pthread_mutex_unlock(&mutex);

	pStreamHandle = avs_media_get_u(m);
	if(pStreamHandle == NULL)
	{
		j_warning("The stream handle is null.");
		return -EFAULT;
	}
	j_message("Begin to control video steam --> channel:%u, level:%d", pStreamHandle->channel, pStreamHandle->streamType);
	if(cmd == 1 && data == NULL)
	{
		j_message("video control,= cmd: %d, data: %p", cmd, data);
		av_pool_request_iframe(pStreamHandle->channel, pStreamHandle->streamType);
	}
	j_message("End to control video steam --> channel:%u, level:%d", pStreamHandle->channel, pStreamHandle->streamType);

	return 0;
}
void  ls_close_cb(avs_media *m)
{
	JXJ_STREAM_HANDLE_S *pStreamHandle = NULL;

	j_message("Begin to close video steam");

	pthread_mutex_lock(&mutex);
	gCallCloseCnt++;
	ls_print_cnt();
	pthread_mutex_unlock(&mutex);

	pStreamHandle = avs_media_get_u(m);
	if(pStreamHandle == NULL)
	{
		j_warning("The stream handle is null.");
		return;
	}
	j_message("Begin to close video steam --> channel:%u, level:%d", pStreamHandle->channel, pStreamHandle->streamType);
	pStreamHandle->quit = 1;
	if(pStreamHandle->sta == MEDIA_USED)
	{
		avs_media_set_u(m, NULL);
		nvr_stop_preview(pStreamHandle);
		nvr_free_stream_handle(pStreamHandle);
	}
	
	j_message("End to close video steam");
	return;
}

// 实时流回调接口
static ls_avs_ops m_ls_avs_cb = 
{
	.probe = ls_probe_cb,
	.open = ls_open_cb,
	.play = ls_play_cb,
	.ctrl = ls_ctrl_cb,
	.close = ls_close_cb,
};


int nvr_jxj_pf_init()
{
	int nRet = 0;
	NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg = {0};
	NVR_DEV_STABLE_INFO_S stStableInfo = {0};
	struct platfrom_param param;
	struct cloud_param cloudParam;
	DEV_SystemCfg *pSystemCfg = get_system_cfg();
	
	j_message("start JXJ PLATFORM RTSP server.");
	memset(&gStreamHandle, 0, sizeof(gStreamHandle));
	pthread_mutex_init(&mutex, NULL);

	nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg);
	nvr_logon_get_stable_cfg(0, &stStableInfo);
	
	j_message("pu_id:%s", stJxjPlatformCfg.szPuId);
	j_message("cmsip:%s", stJxjPlatformCfg.szCmsIp);
	j_message("cms port:%d", stJxjPlatformCfg.u16CmsPort);

	// 如果没有设置平台参数,使用默认的参数进行初始化
	// 保证NVR可以正常搜索到设备
	if(strlen(stJxjPlatformCfg.szPuId) != 0)
		strcpy(param.pu_id, stJxjPlatformCfg.szPuId);
	else
		strcpy(param.pu_id, "JXJ-NVR-00000000");
	if(strlen(stJxjPlatformCfg.szCmsIp) != 0)
		strcpy(param.cms_host, stJxjPlatformCfg.szCmsIp);
	else
		strcpy(param.cms_host, "192.168.1.100");

	if(stJxjPlatformCfg.u16CmsPort != 0)
		param.cms_port = stJxjPlatformCfg.u16CmsPort; 
	else
		param.cms_port = 9902;
	param.conn_cms  = !!stJxjPlatformCfg.u8Enable;
	param.protocol  = stJxjPlatformCfg.u8ProtocolType;
	param.data_port = JXJ_RTSP_PORT;
	param.pu_type   = PU_TYPE;

	//j_message("is_connect_cms = %d", param.conn_cms);
	//if(hi_jxj_upgrade_init() != 0)	return -1;

	strncpy(cloudParam.dev_code, stStableInfo.szSerialId, sizeof(cloudParam.dev_code)-1);
	strncpy(cloudParam.software, stStableInfo.szSoftVer, sizeof(cloudParam.software)-1);
	strncpy(cloudParam.dispense_host, JXJ_CLOUD_DEF_SVR_IP, sizeof(cloudParam.dispense_host)-1);
	cloudParam.dispense_port = JXJ_CLOUD_DEF_SVR_PORT;
	cloudParam.conn_cloud = !!(pSystemCfg->platformServer & PLATFORM_VIVI_SERVER);

	if((nRet = j_dev_sdk_init(JXJ_SDK_PORT)) != 0) 
	{
		j_message("@@@@@@@@@@@@j_dev_sdk_init Ret = %d", nRet);
		return -1;	
	}
	if((nRet = j_dev_sdk_register_device_ops(&dev_ops)) != 0)
	{
		j_message("@@@@@@@@@@@@j_dev_sdk_register_device_ops Ret = %d", nRet);
		return -1;	
	}
	if((nRet = j_dev_sdk_register_avs_ops(&m_ls_avs_cb, &m_hs_avs_cb)) != 0)
	{
		j_message("@@@@@@@@@@@@j_dev_sdk_register_rtsp_funcs Ret = %d", nRet);
		return -1;	
	}
	if((nRet = j_dev_sdk_set_platfrom_param(&param)) != 0)
	{
		j_message("@@@@@@@@@@@@j_dev_sdk_set_platfrom_param Ret = %d", nRet);
		return -1;	
	}
	if((nRet = j_dev_sdk_set_cloud_param(&cloudParam)) != 0)
	{
		j_message("@@@@@@@@@@@@j_dev_sdk_set_cloud_param Ret = %d", nRet);
		return -1;	
	}
	if((nRet = j_dev_sdk_run_server()) != 0)
	{
		j_message("@@@@@@@@@@@@j_dev_sdk_run_server Ret = %d", nRet);
		return -1;
	}
	pu_mb_init(GROUP_IP, GROUP_PORT, &pu_process_cb);

	nvr_jxj_talk_init();

	j_debug("start JXJ PLATFORM RTSP server over.");
	return 0;
}

int nvr_jxj_pf_uninit()
{
	j_debug("stop JXJ PLATFORM SDK server.");
	pu_mb_uinit();
	j_dev_sdk_stop_server();
	nvr_jxj_talk_uninit();
	// 平台库暂时不支持，调用会有段错误，退出时暂时不调用
	//j_dev_sdk_cleanup();
	pthread_mutex_destroy(&mutex);
	j_debug("stop JXJ PLATFORM SDK server successfully.");

	return 0;
}



