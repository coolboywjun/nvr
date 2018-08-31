


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/prctl.h>
#include "nvr_rtsp_debug.h"
#include "nvr_rtsp_transfer.h"
#include "nvr_rtsp_global.h"
#include <net_api.h>
#include "nvr_rtsp_session.h"
#include "nvr_rtsp_h264.h"
#include "nvr_rtsp_mjpeg.h"
#include "nvr_rtsp_g7xx.h"
#include "nvr_rtsp_aac.h"



SEND_VIDEO_FUN gSendVideo[2] = {rtsp_h264_video_send,
                                rtsp_mjpeg_video_send
                               };

SEND_AUDIO_FUN gSendAudio[2] = {rtsp_g7xx_audio_send,
                                rtsp_aac_audio_send
                               };
                               
void rtsp_av_transmit_proc(void *arg);


int rtsp_send_reply(RTSP_SESSION_S *pSess, int err, char *addon , int simple)
{
	char *pTmp = pSess->szBuffSend;
	if(simple == 1)
	{
		pTmp += rtsp_make_resp_header(pSess, err);
	}
	if(addon != NULL)
	{
		pTmp += sprintf(pTmp, "%s", addon);
	}
	if(simple)
		strcat(pSess->szBuffSend, RTSP_LRLF);
	pthread_mutex_lock(&pSess->mutex);
	if((net_tcp_noblock_send(pSess->nRtspSocket, pSess->szBuffSend, strlen(pSess->szBuffSend),
	                         NULL, 5000)) != strlen(pSess->szBuffSend))
	{
		pthread_mutex_unlock(&pSess->mutex);
		RTSP_PRINT("rtsp to send %d byte error\n", strlen(pSess->szBuffSend));
		return -1;
	}
	pthread_mutex_unlock(&pSess->mutex);
	//RTSP_PRINT("Send S --> C(%s) >>>>>>\n %s\n <<<<<<" ,pSess->szRemoteIP, pSess->szBuffRecv);
	//RTSP_PRINT("&&&&&&&&&&&&&&& send: %s\n", pSess->szBuffSend);
	RTSP_PRINT("\n==================== SEND =================\n");
	RTSP_PRINT("%s\n", pSess->szBuffSend);
	RTSP_PRINT("\n==================== SEND END =================\n\n");
	return 0;
}

int create_rtsp_av_transmit(RTSP_SESSION_S *pSess)
{
	pthread_create(&(pSess->sessThd), NULL, (void *)rtsp_av_transmit_proc, pSess);
	RTSP_PRINT("create_rtsp_av_transmit %d\n", pSess->sessThd);
	return 0;
}

void rtsp_av_transmit_proc(void *arg)
{
	RTSP_SESSION_S    *pSess = (RTSP_SESSION_S *)arg;
	int   nFrameLen = 0, nEncType = 0;
	unsigned long u32Pts[2] = {0};
//	unsigned long u32Bits = 0, u32Sample = 0;
	unsigned long u32PlayLoad = 0;
	unsigned long u32FrameType = 0;
	int nHaveSndVideo = 0;
//	int nFrameBufSize = 0;
	char *pFrameBuf = NULL;
	//pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "RTSPTransmit");
	gRequestIdrCb(pSess->nChId, pSess->nStreamId, pSess->lPriData);
	while(pSess->eState != RTSP_STATE_STOP)
	{
#if 0
#if (PLATFORM_NVR==0)
		if(pSess->u8RequestStreamFlag[1] > 0 && gGetAudioCb != NULL)
		{
			if(pSess->u8RequestStreamFlag[0] > 0)
			{
				if(nHaveSndVideo == 1)
				{
					if((nFrameLen = gGetAudioCb(pSess->nChId,
					                            pSess->stRtpSender.pFrameBuf,
					                            nFrameBufSize,
					                            u32NeedFrameNo[1], &u32RealFrameNo[1],
					                            &u32Pos[1], &u32Pts[1], &u32PlayLoad,
					                            &u32Bits, &u32Sample, &u32Offset,
					                            pSess->lPriData)) > 0)
					{
						u32NeedFrameNo[1] = u32RealFrameNo[1] + 1;
						nEncType = ((u32PlayLoad == RTP_PT_ULAW) ||
						            (u32PlayLoad == RTP_PT_ALAW)) ? 0 : 1;
						if(gSendAudio[nEncType](pSess, pSess->stRtpSender.pFrameBuf + u32Offset,
						                        nFrameLen, u32Pts[1], u32Sample) < 0)
						{
							RTSP_PRINT("send data error %d\n", __LINE__);
							break;
						}
					}
				}
			}
			else
			{
				if((nFrameLen = gGetAudioCb(pSess->nChId,
				                            pSess->stRtpSender.pFrameBuf,
				                            nFrameBufSize,
				                            u32NeedFrameNo[1], &u32RealFrameNo[1],
				                            &u32Pos[1], &u32Pts[1], &u32PlayLoad,
				                            &u32Bits, &u32Sample, &u32Offset,
				                            pSess->lPriData)) > 0)
				{
					u32NeedFrameNo[1] = u32RealFrameNo[1] + 1;
					nEncType = ((u32PlayLoad == RTP_PT_ULAW) ||
					            (u32PlayLoad == RTP_PT_ALAW)) ? 0 : 1;
					if(gSendAudio[nEncType](pSess, pSess->stRtpSender.pFrameBuf + u32Offset,
					                        nFrameLen, u32Pts[1], u32Sample) < 0)
					{
						RTSP_PRINT("send data error %d\n", __LINE__);
						break;
					}
				}
			}
		}
#endif
#endif
		if(pSess->u8RequestStreamFlag[0] > 0 && gGetVideoCb != NULL)
		{
			if((nFrameLen = gGetVideoCb(pSess->nChId,
			                            pSess->nStreamId,
			                            &pFrameBuf, &u32Pts[0],
			                            &u32PlayLoad, &u32FrameType, 0,
			                            pSess->lPriData)) <= 0)
			{
				usleep(10000);
				continue ;
			}
			nHaveSndVideo = 1; //已经发送了视频帧
			nEncType = (u32PlayLoad ==  RTP_PT_H264) ? 0 : 1;
			if(gSendVideo[nEncType % 2](pSess, pFrameBuf,
			                            nFrameLen, u32FrameType, u32Pts[0]) < 0)
			{
				break;
			}
		}
	}
	RTSP_PRINT("rtsp_av_transmit_proc close sid %s\n", pSess->szId);

	pSess->eTransType = RTSP_STATE_STOP;
	return ;
}

int send_rtp_data(RTSP_SESSION_S *pSess, char *szRtpData, int nDataLen, int bAudio)
{
	if(pSess->eTransType == RTP_TRANSPORT_TCP)
	{
		pthread_mutex_lock(&pSess->mutex);
		if(net_tcp_noblock_send(pSess->stRtpSender.nRtpTcpSock, szRtpData, nDataLen,
		                        0, 5000) != nDataLen)
		{
			pthread_mutex_unlock(&pSess->mutex);
			return -1;
		}
		pthread_mutex_unlock(&pSess->mutex);
	}
	else
	{
		if(net_udp_send(pSess->stRtpSender.nRtpSock[bAudio], szRtpData, nDataLen,
		                (struct sockaddr *) & (pSess->stRtpSender.addrRemote[bAudio])) != nDataLen)
			return -1;
	}
	return nDataLen;
}


