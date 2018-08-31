/******************************************************************************

                  版权所有 (C), 2011-2015, 佳信捷电子

 ******************************************************************************
  文 件 名   : hi_rtsp_transfer.c
  版 本 号   : v2.0
  作    者   : 梁锦明
  生成日期   : 2011年9月8日
  最近修改   :
  功能描述   : rtsp 发送实现
  函数列表   :
              create_rtsp_av_transmit
              rtsp_av_transmit_proc
              rtsp_send_reply
              send_rtp_data
  修改历史   :
  1.日    期   : 2011年9月8日
    作    者   : 梁锦明
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/prctl.h>
#include "hi_rtsp_debug.h"
#include "hi_rtsp_transfer.h"
#include "hi_rtsp_global.h"
#include "net_api.h"
#include "hi_rtsp_session.h"
#include "hi_rtsp_h264.h"
#include "hi_rtsp_mjpeg.h"
#include "hi_rtsp_g7xx.h"
#include "hi_rtsp_aac.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
void rtsp_av_transmit_proc(void *arg);
 
/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
SEND_VIDEO_FUN gSendVideo[2] = {rtsp_h264_video_send, 
								rtsp_mjpeg_video_send};

SEND_AUDIO_FUN gSendAudio[2] = {rtsp_g7xx_audio_send,
								rtsp_aac_audio_send};

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 函数实现                                     *
 *----------------------------------------------*/
int rtsp_send_reply(RTSP_SESSION_S* pSess, int err, char *addon , int simple)
{
    char* pTmp = pSess->szBuffSend;

    if (simple == 1)
    {
        pTmp += rtsp_make_resp_header(pSess, err);
    }
    
    if (addon != NULL)
    {
        pTmp += sprintf(pTmp, "%s", addon );
    }

    if (simple)
        strcat(pSess->szBuffSend, RTSP_LRLF);
    pthread_mutex_lock(&pSess->mutex);
    if ((net_tcp_noblock_send(pSess->nRtspSocket, pSess->szBuffSend, strlen(pSess->szBuffSend),
				NULL, 5000)) != strlen(pSess->szBuffSend))
    {	
    	 pthread_mutex_unlock(&pSess->mutex);
         RTSP_PRINT("rtsp to send %d byte error\n",strlen(pSess->szBuffSend));
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
	usleep(10000);	
    	pthread_create(&(pSess->sessThd), NULL, (void *)rtsp_av_transmit_proc, pSess);
	RTSP_PRINT("create_rtsp_av_transmit %d\n", (int)pSess->sessThd);
	usleep(10000);	
	return 0;
}


void rtsp_av_transmit_proc(void *arg)
{
	RTSP_SESSION_S	  *pSess = (RTSP_SESSION_S *)arg;
	int   nFrameLen = 0, nEncType = 0;
	unsigned long u32Pts[2] = {0};
	unsigned long u32PlayLoad = 0;
	unsigned long u32FrameType = 0;
	int nHaveSndVideo = 0;
	char *pFrameBuf = NULL;
	prctl(PR_SET_NAME, "RTSPTransmit");

	if(!pSess->isHistoryStream)
	{		
		gRequestIdrCb(pSess->nChId, pSess->nStreamId, pSess->lPriData);
		while(pSess->eState != RTSP_STATE_STOP)
		{
			if(pSess->u8RequestStreamFlag[0] > 0 && gGetFrameCb != NULL)
			{
				if((nFrameLen = gGetFrameCb(pSess->nChId,
				                            pSess->nStreamId,
				                            &pFrameBuf, &u32Pts[0],
				                            &u32PlayLoad, &u32FrameType, 0,
				                            pSess->lPriData)) <= 0)
				{
					usleep(10000);
					continue ;
				}
				if(u32PlayLoad == RTP_PT_H264 || u32PlayLoad == RTP_PT_JPEG || u32PlayLoad == RTP_PT_JPEG)
				{
					nHaveSndVideo = 1; //已经发送了视频帧
					nEncType = (u32PlayLoad ==  RTP_PT_H264) ? 0 : 1;
					
					if(gSendVideo[nEncType % 2](pSess, pFrameBuf,
					                            nFrameLen, u32FrameType, u32Pts[0]) < 0)
					{
						break;
					}	
				}
				else
				{
					if(nHaveSndVideo == 0)
						continue;					
					nEncType = ((u32PlayLoad == RTP_PT_ULAW) ||
										     (u32PlayLoad == RTP_PT_ALAW)) ? 0 : 1;
					if (gSendAudio[nEncType](pSess, pFrameBuf, 
						nFrameLen, u32Pts[1], pSess->audioSample) < 0)
					{
						RTSP_PRINT("send data error %d\n", __LINE__);
						break;	
					}
				}
			}
		}
	}
	else
	{
		unsigned long u32PreVPts = ~0UL;
		unsigned long u32PreAPts = ~0Ul;
		unsigned long u32Sample, u32Bits, u32Offset;
		while (pSess->eState != RTSP_STATE_STOP)
		{
			if(pSess->eState == RTSP_STATE_PAUSE)
			{
				usleep(100000);
				u32PreVPts = ~0UL;
				u32PreAPts = ~0Ul;
				continue;
			}
			if(pSess->seekSec != 0)
			{
				gSetVodSeek(pSess->vodHandle, pSess->seekSec);
				pSess->seekSec = 0;
			}
			if((nFrameLen = gGetVodFrameCb(pSess->vodHandle, 
								&pFrameBuf, RTSP_MAX_FRAME_LEN,
								&u32FrameType, 
								&u32Pts[0], &u32PlayLoad,
								&u32Sample, &u32Bits, &u32Offset)) < 0)
			{
				printf("Read record data failed.\n");
				break;
			}

			if(u32FrameType == 1)		// video
			{
				int us;	
				
				nHaveSndVideo = 1; //已经发送了视频帧
				nEncType = (u32PlayLoad ==  RTP_PT_H264) ? 0 : 1;
				if (gSendVideo[nEncType%2](pSess, pFrameBuf + u32Offset,
						nFrameLen, u32FrameType, u32Pts[0]) < 0)
				{
					break;	
				}
				if(u32Pts[0] - u32PreVPts > 0 && u32Pts[0] - u32PreVPts < 100)
					us = (u32Pts[0] - u32PreVPts) * 1000;
				else
					us = 25000;
				if(pSess->scale == 1000)
				{
					usleep(us);
					//printf("scale: %d, us: %d\n", pSess->scale, us);
				}
				else if(pSess->scale == 2000)
				{
					usleep(us>>1);
					//printf("scale: %d, us: %d\n", pSess->scale, us>>1);
				}
				else
				{
					usleep(us<<1);
					//printf("scale: %d, us: %d\n", pSess->scale, us<<1);
				}
				//printf("video interval ms: %lu\n", u32Pts[0] - u32PreVPts);
				u32PreVPts = u32Pts[0];
			}
			else		// audio
			{
				printf("vod get a audio.\n");
				if(pSess->scale == 1000)
				{
					if(nHaveSndVideo == 0)
						continue;	
					printf("vod send audio frame\n");
					nEncType = ((u32PlayLoad == RTP_PT_ULAW) ||
										     (u32PlayLoad == RTP_PT_ALAW)) ? 0 : 1;
					if (gSendAudio[nEncType](pSess, pFrameBuf + u32Offset, 
											nFrameLen, u32Pts[0], u32Sample) < 0)
					{
						break;	
					}
				}
			}
		}
		gGetVodStopCb(pSess->vodHandle);
	}
	RTSP_PRINT("rtsp_av_transmit_proc close sid %s\n", pSess->szId);	
	pSess->eTransType = RTSP_STATE_STOP;
	return ;
}

int send_rtp_data(RTSP_SESSION_S* pSess, char *szRtpData, int nDataLen, int bAudio)
{
	if (pSess->eTransType == RTP_TRANSPORT_TCP)
	{
		pthread_mutex_lock(&pSess->mutex);
		if (net_tcp_noblock_send(pSess->stRtpSender.nRtpTcpSock, szRtpData, nDataLen,
			0, 5000) != nDataLen)	
		{
    			pthread_mutex_unlock(&pSess->mutex);
			return -1;
		}
		pthread_mutex_unlock(&pSess->mutex);
	}
	else
	{
 		if (net_udp_send(pSess->stRtpSender.nRtpSock[bAudio], szRtpData, nDataLen,
			( struct sockaddr *)&(pSess->stRtpSender.addrRemote[bAudio])) != nDataLen)	
			return -1;		
	}
	return nDataLen;
}


