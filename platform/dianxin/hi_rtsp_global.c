/******************************************************************************

                  版权所有 (C), 2011-2015, 佳信捷电子

 ******************************************************************************
  文 件 名   : hi_rtsp_global.c
  版 本 号   : v2.0
  作    者   : 梁锦明
  生成日期   : 2011年9月8日
  最近修改   :
  功能描述   : rtsp 服务器实现
  函数列表   :
              hi_rtsp_set_cb
              hi_rtsp_start
              hi_rtsp_stop
              rtsp_get_audio_media_info
              rtsp_get_video_media_info
              rtsp_new_conn_proc
              rtsp_socket_build
              rtsp_svr_proc
  修改历史   :
  1.日    期   : 2011年9月8日
    作    者   : 梁锦明
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include "hi_rtsp_debug.h"
#include "hi_rtsp_global.h"
#include "hi_rtsp_session.h"
#include "hi_rtsp_h264.h"
#include "hi_rtsp_mjpeg.h"
#include "hi_rtsp_transfer.h"
#include "hi_rtsp_parse.h"
#include "net_api.h"

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
HI_RTSP_SVR	         gRtspSvr;
pthread_mutex_t		 gRtspMutex;



CheckUserCallback    gCheckUserCb = NULL;
RequestIDRCallback	 gRequestIdrCb= NULL;
GetVideoCallback     gGetVideoCb  = NULL;
GetAudioCallback     gGetAudioCb  = NULL;
GetFrameCallback    gGetFrameCb = NULL;

MsgCallback		    gMsgCb        = NULL;

GetVodStart gGetVodStartCb = NULL;
GetVodStop gGetVodStopCb = NULL;
GetVodFrame gGetVodFrameCb = NULL;

SetVodSeek gSetVodSeek = NULL;
GetVodProperty gGetVodProperty = NULL;

GetVideoVarify gGetVideoVarify = NULL;
GetVodVarify gGetVodVarify = NULL;
GetVideoFlag gGetVideoFlag = NULL;
GetVodFlag gGetVodFlag = NULL;

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
int     hi_rtsp_set_cb(CheckUserCallback checkUserCb,
					   RequestIDRCallback reqIdrCb,
					   GetVideoCallback  getVideoCb,
					   GetAudioCallback  getAudioCb,
					   GetFrameCallback getFrameCb,
					   MsgCallback	  msgCb)
{
	gCheckUserCb = checkUserCb;
	gRequestIdrCb= reqIdrCb;	
	gGetVideoCb  = getVideoCb;
	gGetAudioCb  = getAudioCb;
	gGetFrameCb = getFrameCb;
	gMsgCb	 	=  msgCb;
	return 0;
}

int	hi_rtsp_set_vod_cb(GetVodStart getVodStartCb,
							GetVodStop getVodStopCb,
							GetVodFrame getVodFrameCb,
							SetVodSeek setVodSeek,
							GetVodProperty getVodProperty)
{
	gGetVodStartCb = getVodStartCb;
	gGetVodStopCb = getVodStopCb;
	gGetVodFrameCb = getVodFrameCb;
	gSetVodSeek = setVodSeek;
	gGetVodProperty = getVodProperty;
	return 0;
}

int	hi_rtsp_set_varify_cb(GetVideoVarify getVideoVarifyCb,
							GetVodVarify getVodVarifyCb,
							GetVideoFlag getVideoFlagCb,
							GetVodFlag getVodFlagCb)
{
	gGetVideoVarify = getVideoVarifyCb;
	gGetVodVarify = getVodVarifyCb;
	gGetVideoFlag = getVideoFlagCb;
	gGetVodFlag = getVodFlagCb;
	return 0;
}


void   rtsp_socket_build(int *pSockArray, int *pSockNum)
{
    struct list_head *pos, *q;
	RTSP_SESSION_S *pSess = NULL;
	
	*pSockNum     = 1;
	pSockArray[0] = gRtspSvr.nSvrSocket;

	list_for_each_safe(pos, q, &(gRtspSvr.sessList.list) )
	{
		pSess = list_entry(pos, RTSP_SESSION_S , list);
        if (NULL != pSess && pSess->nRtspSocket > 0)
        {
        	pSockArray[*pSockNum] = pSess->nRtspSocket;
			*pSockNum += 1;
        }
    } 	
}

int    rtsp_new_conn_proc()
{
	int  nLen = 0, nClientSock = 0, nRecvLen = 0;
	struct sockaddr_in adrrClient;
	char szBuff[RTSP_MAX_PROTOCOL_BUFFER] = {0};
	RTSP_SESSION_S *pSess = NULL;

	nLen = sizeof(adrrClient);
	if ((nClientSock = net_tcp_block_accept(gRtspSvr.nSvrSocket, 
				(struct sockaddr *)&adrrClient, &nLen)) < 0)
	{
		RTSP_PRINT("rtsp_new_conn_proc accept error\n");
		return -1 ;
	}

	RTSP_PRINT("recv a new rtsp connect \n");
	net_set_sock_noblock(nClientSock);
	net_set_sock_attr(nClientSock, 1, 0,0,0,0);
	net_sock_set_linger(nClientSock);
	net_set_sock_nodelay(nClientSock);
/*
	if (hi_select(&nClientSock, 0x1, 0x1, 15000) != (0x10000 | nClientSock))
	{
		RTSP_PRINT("rtsp_new_conn_proc client select error\n");
		hi_close_socket(&nClientSock);
		return -1 ;
	}

	if ((nRecvLen = hi_tcp_noblock_recv(nClientSock, szBuff, sizeof(szBuff),
				0, 5000)) <= 0)
	{
		RTSP_PRINT("rtsp_new_conn_proc client recv error\n");
		hi_close_socket(&nClientSock);
		return -1 ;	
	}
	RTSP_PRINT("rtsp_new_conn_proc recv:%s\n", 
			szBuff);
*/
	/*创建rtsp会话，并把它加入rtsp会话链表之中*/
	if ((pSess = rtsp_session_create(nClientSock)) == NULL)
	{/*创建新的rtsp session 失败*/
		net_close_socket(&nClientSock);
		return -1 ;	
	}
#if 0
	memcpy(pSess->szBuffRecv, szBuff, RTSP_MAX_PROTOCOL_BUFFER);
	pSess->nMsgSize = nRecvLen;
	/*处理rtsp会话消息*/
	if((nRecvLen = rtsp_session_process(pSess)) != 0)
	{
		RTSP_PRINT("close rtsp sid %s  chn %d \n", pSess->szId, pSess->nChId);
		rtsp_session_close(pSess);
		rtsp_list_client();
		return -1 ;
	}
#endif
	return 0;
}

void * rtsp_svr_proc(void *arg)
{
	char szPort[32] = {0};
	int  nSocket[32]= {0};
	int  nAddrLen = 0, nSocketNum = 0, nRet = 0, nRecvLen = 0;
	int nParseResult;

    int	hdr_len, body_len;
    struct list_head *pSessPoss = NULL, *pSessN = NULL;
	RTSP_SESSION_S   *pSess = NULL;
	
	prctl(PR_SET_NAME, "RTSPSVR");
	sprintf(szPort, "%d", gRtspSvr.nSvrPort);
	if ((gRtspSvr.nSvrSocket = net_tcp_listen(NULL, szPort, &nAddrLen)) < 0)
	{
		RTSP_PRINT("rtsp_svr_proc  net_tcp_listen %s error %s\n", szPort, strerror(errno));
		return (void *)NULL;
	}

	gRtspSvr.nSvrExits = 0;
	while (gRtspSvr.nSvrExits == 0)
	{	/*检查rtsp会话状态*/
		check_rtsp_session_state();
		/**/
		rtsp_socket_build(nSocket, &nSocketNum);
		if ((nRet = net_select(nSocket, nSocketNum, 0x1, 5000/*15000*/)) < 0)
		{
			sleep(1);
			RTSP_PRINT("rtsp_svr_proc select error\n");
			continue ;
		}
		/*time out*/
		if (nRet == 0)
			continue;

		if (nRet == (gRtspSvr.nSvrSocket | 0x10000))
		{/* Check if a connect has occured  */
			rtsp_new_conn_proc();
			continue;
		}

		list_for_each_safe(pSessPoss, pSessN, &(gRtspSvr.sessList.list))
		{
			pSess = list_entry(pSessPoss, RTSP_SESSION_S, list);
			if (NULL != pSess && nRet == (pSess->nRtspSocket | 0x10000))
			{
				/*接收rtsp会话消息*/
				//memset(pSess->szBuffRecv, 0, RTSP_MAX_PROTOCOL_BUFFER);
				if((nRecvLen = net_tcp_noblock_recv(pSess->nRtspSocket,
									pSess->szBuffData+pSess->nDataSize, RTSP_MAX_PROTOCOL_BUFFER-pSess->nDataSize-1,
									0, 5000)) <= 0)
					goto __rtsp_session_error_handle;
	
				//RTSP_PRINT("Recive C(%s) --> S >>>>>>\n %s\n <<<<<<" ,pSess->szRemoteIP, pSess->szBuffRecv);
				/*处理rtsp会话消息*/
				pSess->nDataSize += nRecvLen;
				pSess->szBuffData[pSess->nDataSize] = '\0';
				//pSess->nMsgSize = nRecvLen;
				
				do
				{
					nRet = 0;
					/*RTSP消息合法性分析*/
					nParseResult = rtsp_full_msg_parse(pSess, &hdr_len, &body_len);
					//printf("@@@@@@@@@@@@@@@@@@@ ret: %d, hdr_len : %d, body_len: %d\n", nParseResult, hdr_len, body_len);
				
					 if(RTSP_NOT_FULL == nParseResult)
								
					 {
						//RTSP_PRINT("The receive message is not full\n");		
						break;
					 }
					else if(RTSP_INTERLVD_RCVD == nParseResult)
					{
						pSess->nMsgSize = hdr_len + body_len;
						pSess->nDataSize -= pSess->nMsgSize;
						if(pSess->nDataSize > 0)
							memcpy(pSess->szBuffData, pSess->szBuffData + pSess->nMsgSize, pSess->nDataSize);
						continue;
					}
					pSess->nMsgSize = hdr_len + body_len;
					pSess->nDataSize -= pSess->nMsgSize;

					memset(pSess->szBuffRecv, 0, RTSP_MAX_PROTOCOL_BUFFER);
					memcpy(pSess->szBuffRecv, pSess->szBuffData, pSess->nMsgSize);
					if(pSess->nDataSize > 0)
						memcpy(pSess->szBuffData, pSess->szBuffData + pSess->nMsgSize, pSess->nDataSize);
					
					nRet = rtsp_session_process(pSess); 
				}while(nRet == 0);
				
__rtsp_session_error_handle:          
				if (nRet != 0) 
				{                  
					RTSP_PRINT("close rtsp sid %s  chn %d \n",pSess->szId, pSess->nChId);
					rtsp_session_close(pSess);
					rtsp_list_client();	
				} 
				break;
			} /* end of if (NULL != pSess) */
		} /* end of list_for_each_safe */
	}
	list_for_each_safe(pSessPoss, pSessN, &(gRtspSvr.sessList.list))
	{
		if (NULL != (pSess = list_entry(pSessPoss, RTSP_SESSION_S, list)))
		{
			rtsp_session_close(pSess);
		}
	}	
	
	net_close_socket(&gRtspSvr.nSvrSocket);
	RTSP_PRINT("rtsp_svr_proc exits\n");
	gRtspSvr.thd = 0;
	return NULL;
}

//============================================================================
//函数:hi_rtsp_start 
//
//功能:启动rtsp 服务
//
//nMaxCh:设备支持的视频最大通道数
//
//nStreamNum:设备一个通道支持的码流数量 (1.单码流 2.双码流 3.三码流 以此类推)
//
//nRtspPort:rtsp 服务端口默认为554
//
//bUseAuth: 是否启用校验 0 不启用 1启用
//
//bRtspMode: rtsp工作模式 0 被动上传视频 1 主动上传视频
//
//=============================================================================
int	hi_rtsp_start(int nMaxCh, int nStreamNum, int nRtspPort, int bUseAuth, int bRtspMode)
{
	int nRet = 0;
	
	memset(&gRtspSvr, 0, sizeof(HI_RTSP_SVR));
	gRtspSvr.eState  = VOD_SVR_STATE_INIT;
	gRtspSvr.nSvrPort= nRtspPort;
	gRtspSvr.nUseAuth= bUseAuth;
	
    /*判断输入的最大通道号， 如果大于设定的最大值， 复位为最大值*/
    if (nMaxCh <= 0 || nMaxCh > RTSP_MAX_CHN)
    {
        nMaxCh = RTSP_MAX_CHN;
    }	
    	
	gRtspSvr.nMaxCh = nMaxCh;
	
	pthread_mutex_init (&gRtspMutex, NULL);	
	INIT_LIST_HEAD(&(gRtspSvr.sessList.list));
	
	if ((nRet = pthread_create(&(gRtspSvr.thd), NULL, rtsp_svr_proc, NULL)) != 0)
	{
		return -1;
	}
	gRtspSvr.nInitOk = 1;
	return 0;
}

int hi_rtsp_stop()
{
	pthread_t thid = gRtspSvr.thd;
	int nTimes = 0;
	if(gRtspSvr.nInitOk)
	{
		gRtspSvr.nSvrExits = 1;
		while (gRtspSvr.thd != 0 && nTimes++ < 500)
		{
			usleep(10000);
			continue ;
		}
		pthread_join(thid, NULL);
	}
	pthread_mutex_destroy(&gRtspMutex);
	return 0;
}

int rtsp_get_video_media_info(int nCh, int nStream, int *pPt, char *profileLevelId, char *sps_pps, long lPriData)
{
	int     nFrameLen = 0, nTryTime = 0;
	unsigned long u32PlayLoad = 0;
	unsigned long u32Pts = 0, u32FrameType = 0;
	char   *pFrameBuf = NULL;

	printf("rtsp_get_video_media_info ##############\r\n");
	while(nTryTime++ < 200)
	{
		if((nFrameLen = gGetVideoCb(nCh, nStream,
		                            &pFrameBuf, &u32Pts,
		                            &u32PlayLoad, &u32FrameType, -1,
		                            lPriData)) > 0)
			break;
		usleep(10000);
	}
	printf("rtsp_get_video_media_info nFrameLen = %d\r\n##############\r\n", nFrameLen);

	if(nFrameLen > 0)
	{
		*pPt = u32PlayLoad;
		if(RTP_PT_H264 == u32PlayLoad)  //VENC_H264
		{
			rtsp_h264_get_media_info(pFrameBuf, nFrameLen, profileLevelId, sps_pps);
		}
	}
	
	return 0;
}

int rtsp_get_audio_media_info(int nCh, int *pPt, int *pBits, int *pSample, char *pInfo, long lPriData)
{	
	int     nFrameLen = 0, nTryTime = 0;
	unsigned long u32PlayLoad = 0;
	unsigned long u32Pts = 0;
	unsigned long u32Bits, u32Sample;
	char   *pFrameBuf = NULL;
	
	while(nTryTime++ < 200)
	{
		if((nFrameLen = gGetAudioCb(nCh,
		                            &pFrameBuf, &u32Pts,
		                            &u32PlayLoad, &u32Bits, &u32Sample,
		                            lPriData)) > 0)
			break;
		usleep(10000);
	}
	
	if (nFrameLen > 0)
	{
		*pPt = u32PlayLoad;
		switch(u32PlayLoad)
		{
			case RTP_PT_ADPCM:
				strcpy(pInfo, " PCM");
				break;

			case RTP_PT_G726:
				strcpy(pInfo, "G726-16");
				break;

			case RTP_PT_ULAW:
				strcpy(pInfo, " PCMU");
				break;

			case RTP_PT_ALAW:
				strcpy(pInfo, " PCMA");
				break;
			
			case RTP_PT_AAC:
				strcpy(pInfo, "MPEG4-GENERIC");
				break;
		}
		
		*pBits   	  = u32Bits;
		*pSample = u32Sample;	

		return 0;
	}		
	return -1;
}

int rtsp_get_history_video_media_info(int nCh, int nStream, int *pPt, char *profileLevelId, char *sps_pps, void *handle)
{
	int     nFrameLen = 0, nTryTime = 0;
	unsigned long u32PlayLoad = 0;
	unsigned long u32Pts = 0, u32FrameType = 0;
	char   *pFrameBuf = NULL;
	unsigned long u32Sample, u32Bits, u32Offset;

	while(nTryTime++ < 200)
	{
		if((nFrameLen = gGetVodFrameCb(handle, 
					&pFrameBuf, RTSP_MAX_FRAME_LEN,
					&u32FrameType, 
					&u32Pts, &u32PlayLoad,
					&u32Sample, &u32Bits, &u32Offset)) > 0)
		{
			if(u32FrameType)	// video
				break;
		}
		usleep(10000);
	}
	
	if(nFrameLen > 0)
	{
		*pPt = u32PlayLoad;
		if(RTP_PT_H264 == u32PlayLoad)  //VENC_H264
		{
			rtsp_h264_get_media_info(pFrameBuf, nFrameLen, profileLevelId, sps_pps);
		}
	}
	
	printf("rtsp_get_video_media_info u32PlayLoad = %d,  nFrameLen = %d\r\n##############\r\n",u32PlayLoad, nFrameLen);
	return 0;
}

int rtsp_get_history_audio_media_info(int nCh, int *pPt, int *pBits, int *pSample, char *pInfo, void *handle)
{	
	int     nFrameLen = 0, nTryTime = 0;
	unsigned long u32PlayLoad = 0;
	unsigned long u32Pts = 0, u32FrameType = 0;
	char   *pFrameBuf = NULL;
	unsigned long u32Sample, u32Bits, u32Offset;

	while(nTryTime++ < 200)
	{
		if((nFrameLen = gGetVodFrameCb(handle, 
					&pFrameBuf, RTSP_MAX_FRAME_LEN,
					&u32FrameType, 
					&u32Pts, &u32PlayLoad,
					&u32Sample, &u32Bits, &u32Offset)) > 0)
		{
			if(u32FrameType == 0)	// audio
				break;
		}
		usleep(10000);
	}

	if (nFrameLen > 0)
	{
		*pPt = u32PlayLoad;
		switch(u32PlayLoad)
		{
			case RTP_PT_ADPCM:
				strcpy(pInfo, " PCM");
				break;

			case RTP_PT_G726:
				strcpy(pInfo, "G726-16");
				break;

			case RTP_PT_ULAW:
				strcpy(pInfo, " PCMU");
				break;

			case RTP_PT_ALAW:
				strcpy(pInfo, " PCMA");
				break;
			
			case RTP_PT_AAC:
				strcpy(pInfo, "MPEG4-GENERIC");
				break;
		}
		
		*pBits   	  = u32Bits;
		*pSample = u32Sample;	

		return 0;
	}		
	return -1;
}
int rtsp_get_client_cnt()
{
	return gRtspSvr.clientCnt[0] + gRtspSvr.clientCnt[1];
}


