/******************************************************************************

                  版权所有 (C), 2011-2015, 佳信捷电子

 ******************************************************************************
  文 件 名   : hi_rtsp_g7xx.c
  版 本 号   : v2.0
  作    者   : 梁锦明
  生成日期   : 2011年9月8日
  最近修改   :
  功能描述   : rtsp g711a g711u音频数据发送实现
  函数列表   :
              rtsp_g7xx_audio_send
              rtsp_g7xx_rtp_send
  修改历史   :
  1.日    期   : 2011年9月8日
    作    者   : 梁锦明
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <string.h>
#include "hi_rtsp_debug.h"
#include "hi_rtsp_g7xx.h"
#include "hi_rtsp_rtp.h"
#include "hi_rtsp_transfer.h"

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


int rtsp_g7xx_rtp_send(RTSP_SESSION_S* pSess, const char *szBuff, int nFrameLen, 
						 unsigned long u32TimeStamp)
{
	RTP_HDR_S 			rtpHdr;
	RTP_OVER_TCP_HEADER rtpOverTcpHdr;
	char *				pSendBuf = (char *)pSess->stRtpSender.szBuff;
	
	int    nRtpOverTcpLen = sizeof(RTP_OVER_TCP_HEADER);
	int    nRtpHdrLen     = sizeof(RTP_HDR_S);
	int    nPlayLoadLen   = 0, nDataLen = 0;
	
	memset(&rtpHdr,        0, sizeof(RTP_HDR_S));
	memset(&rtpOverTcpHdr, 0, sizeof(RTP_OVER_TCP_HEADER));

	rtpOverTcpHdr.u8Dollar     = '$';
	rtpOverTcpHdr.u8ChnId      = pSess->nAudioChId;
	
	rtpHdr.version   = RTP_VERSION;
	rtpHdr.cc        = 0;
	rtpHdr.x         = 0;
	rtpHdr.p         = 0;
	rtpHdr.ts		 = htonl(u32TimeStamp);
	rtpHdr.pt        = pSess->stRtpSender.nPayLoad[1];
	rtpHdr.ssrc      = htonl(pSess->stRtpSender.nAudioSsrc);

	if (pSess->eTransType != RTP_TRANSPORT_TCP)
	{
		nRtpOverTcpLen = 0;
	}
	
	nPlayLoadLen = RTP_MAX_PACKET_BUFF - nRtpOverTcpLen - nRtpHdrLen;
	if (nFrameLen + nRtpOverTcpLen + nRtpHdrLen <= RTP_MAX_PACKET_BUFF)
	{
		rtpHdr.seqno = htons(++pSess->stRtpSender.u16LastSndSeq[1]);
		rtpHdr.marker= 1;
		
		if (nRtpOverTcpLen != 0)
		{
			rtpOverTcpHdr.u16PacketSize = htons(nFrameLen+nRtpHdrLen);
			memcpy(pSendBuf, &rtpOverTcpHdr, nRtpOverTcpLen);
		}

		memcpy(&pSendBuf[nRtpOverTcpLen], &rtpHdr, nRtpHdrLen);
		pSess->stRtpSender.nBuffLen = nRtpHdrLen+nRtpOverTcpLen;
		memcpy(&pSendBuf[pSess->stRtpSender.nBuffLen], szBuff, nFrameLen);	
		pSess->stRtpSender.nBuffLen += nFrameLen;
		
		if (send_rtp_data(pSess, pSendBuf, pSess->stRtpSender.nBuffLen, 1) < 0)
			return -1;		
	}
	else
	{
		while(nFrameLen > 0)
		{
			pSess->stRtpSender.nBuffLen = 0;	
			rtpHdr.seqno = htons(++pSess->stRtpSender.u16LastSndSeq[1]);
		
			if (nFrameLen + nRtpOverTcpLen + nRtpHdrLen <= RTP_MAX_PACKET_BUFF)
			{
				rtpHdr.marker       = 1; ////bLast
				nDataLen            = nFrameLen;
			}	
			else
			{
				rtpHdr.marker       = 0;
				nDataLen            = nPlayLoadLen;
			}

			if (nRtpOverTcpLen != 0)
			{
				rtpOverTcpHdr.u16PacketSize = htons(nDataLen+nRtpHdrLen);
				memcpy(pSendBuf, &rtpOverTcpHdr, nRtpOverTcpLen);
			}	
			
			memcpy(&pSendBuf[nRtpOverTcpLen], &rtpHdr, nRtpHdrLen);
			pSess->stRtpSender.nBuffLen = nRtpHdrLen+nRtpOverTcpLen;
			
			memcpy(&pSendBuf[pSess->stRtpSender.nBuffLen], szBuff, nDataLen);
			pSess->stRtpSender.nBuffLen += nDataLen;
			nFrameLen  -= nDataLen;
			szBuff     += nDataLen;

			if (send_rtp_data(pSess, pSendBuf, pSess->stRtpSender.nBuffLen, 1) < 0)
			{
				return -1;
			}	
		}/*end while*/
			
	}/*end else*/	
	return 0;
}

int rtsp_g7xx_audio_send(RTSP_SESSION_S* pSess, const char *szBuff, int nFrameLen, 
						 unsigned long u32TimeStamp, unsigned long u32SampleRate)
{
	unsigned long nPts = 0;
	int nRet = 0;

	if (pSess->stRtpSender.u32LastSndTs[1] == 0 || 
		u32TimeStamp < pSess->stRtpSender.u32LastSndTs[1])
		nPts = 1600;
	else
 		nPts = pSess->stRtpSender.u32LastSndPts[1] + 
 			  (u32SampleRate/1000)*(u32TimeStamp - pSess->stRtpSender.u32LastSndTs[1]);	

	nRet = rtsp_g7xx_rtp_send(pSess, szBuff, nFrameLen, nPts);
	pSess->stRtpSender.u32LastSndTs[1]  = u32TimeStamp;
	pSess->stRtpSender.u32LastSndPts[1] = nPts;	
	return nRet;
}

int rtsp_g7xx_lsit_audio_send(struct list_head* pSessHead, const char *szBuff, int nFrameLen, 
						 unsigned long u32TimeStamp, unsigned long u32SampleRate)
{
	unsigned long nPts = 0;
	int nRet = 0;
	RTSP_SESSION_S* pSess;

	list_for_each_entry(pSess, pSessHead, transList)
	{
		if (pSess->stRtpSender.u32LastSndTs[1] == 0 || 
			u32TimeStamp < pSess->stRtpSender.u32LastSndTs[1])
			nPts = 1600;
		else
	 		nPts = pSess->stRtpSender.u32LastSndPts[1] + 
	 			  (u32SampleRate/1000)*(u32TimeStamp - pSess->stRtpSender.u32LastSndTs[1]);	

		nRet = rtsp_g7xx_rtp_send(pSess, szBuff, nFrameLen, nPts);
		pSess->stRtpSender.u32LastSndTs[1]  = u32TimeStamp;
		pSess->stRtpSender.u32LastSndPts[1] = nPts;	
	}
	return nRet;
}


