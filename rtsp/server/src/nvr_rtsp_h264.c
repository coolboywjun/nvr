
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "nvr_rtsp_debug.h"
#include "nvr_rtsp_h264.h"
#include "nvr_rtsp_global.h"
#include <net_api.h>
#include "nvr_rtsp_transfer.h"
#include "nvr_rtsp_rtp.h"

#include "base64.h"


int rtsp_h264_send_nal(RTSP_SESSION_S *pSess, const char *szNalData, int nNalLen,
                       int bLast, unsigned int nPts)
{
	RTP_HDR_S 			rtpHdr;
	RTP_OVER_TCP_HEADER rtpOverTcpHdr;
	FU_ADef 			stFU_A;
	char 				*pSendBuf = (char *)pSess->stRtpSender.szBuff;
	int    nRtpOverTcpLen = sizeof(RTP_OVER_TCP_HEADER);
	int    nRtpHdrLen     = sizeof(RTP_HDR_S);
	int    nFuHdrLen      = sizeof(FU_ADef);
	int    nPlayLoadLen   = 0, nFuNum = 0,  nDataLen = 0;
	memset(&rtpHdr,        0, sizeof(RTP_HDR_S));
	memset(&rtpOverTcpHdr, 0, sizeof(RTP_OVER_TCP_HEADER));
	memset(&stFU_A,        0, sizeof(FU_ADef));
	rtpOverTcpHdr.u8Dollar     = '$';
	//rtpOverTcpHdr.u8ChnId      = pSess->nChId;
	rtpOverTcpHdr.u8ChnId = 0;
	rtpHdr.version   = RTP_VERSION;
	rtpHdr.cc        = 0;
	rtpHdr.x         = 0;
	rtpHdr.p         = 0;
	rtpHdr.ts		 = htonl(nPts);
	rtpHdr.pt        = RTP_PT_H264;
	rtpHdr.ssrc      = htonl(pSess->stRtpSender.nVideoSsrc);
	if(pSess->eTransType != RTP_TRANSPORT_TCP)
	{
		nRtpOverTcpLen = 0;
	}
	nPlayLoadLen = RTP_MAX_PACKET_BUFF - nRtpOverTcpLen - nRtpHdrLen - nFuHdrLen;
	if(nNalLen + nRtpOverTcpLen + nRtpHdrLen <= RTP_MAX_PACKET_BUFF)
	{
		rtpHdr.seqno = htons(++pSess->stRtpSender.u16LastSndSeq[0]);
		rtpHdr.marker = 1; //bLast
		if(nRtpOverTcpLen != 0)
		{
			rtpOverTcpHdr.u16PacketSize = htons(nNalLen + nRtpHdrLen);
			memcpy(pSendBuf, &rtpOverTcpHdr, nRtpOverTcpLen);
		}
		memcpy(&pSendBuf[nRtpOverTcpLen], &rtpHdr, nRtpHdrLen);
		pSess->stRtpSender.nBuffLen = nRtpHdrLen + nRtpOverTcpLen;
		memcpy(&pSendBuf[pSess->stRtpSender.nBuffLen], szNalData, nNalLen);
		pSess->stRtpSender.nBuffLen += nNalLen;
		if(send_rtp_data(pSess, pSendBuf, pSess->stRtpSender.nBuffLen, 0) < 0)
			return -1;
	}
	else
	{
		stFU_A.stFUIndicator.F   = (szNalData[0] & 0x80) >> 7;
		stFU_A.stFUIndicator.NRI = (szNalData[0] & 0x60) >> 5;
		stFU_A.stFUIndicator.TYPE = 28;
		stFU_A.stFUHeader.Type   = szNalData[0] & 0x1f;
		szNalData++;   //the first packet skip the first byte
		nNalLen -= 1;
		while(nNalLen > 0)
		{
			pSess->stRtpSender.nBuffLen = 0;
			rtpHdr.seqno = htons(++pSess->stRtpSender.u16LastSndSeq[0]);
			stFU_A.stFUHeader.S = (nFuNum == 0) ? 1 : 0;
			if(nNalLen + nRtpOverTcpLen + nRtpHdrLen + nFuHdrLen <= RTP_MAX_PACKET_BUFF)
			{
				rtpHdr.marker       = 1; ////bLast
				stFU_A.stFUHeader.E = 1;
				nDataLen            = nNalLen;
			}
			else
			{
				stFU_A.stFUHeader.E = 0;
				rtpHdr.marker       = 0;
				nDataLen            = nPlayLoadLen;
			}
			if(nRtpOverTcpLen != 0)
			{
				rtpOverTcpHdr.u16PacketSize = htons(nDataLen + nRtpHdrLen + nFuHdrLen);
				memcpy(pSendBuf, &rtpOverTcpHdr, nRtpOverTcpLen);
			}
			memcpy(&pSendBuf[nRtpOverTcpLen], &rtpHdr, nRtpHdrLen);
			pSess->stRtpSender.nBuffLen = nRtpHdrLen + nRtpOverTcpLen;
			memcpy(&pSendBuf[pSess->stRtpSender.nBuffLen], &stFU_A, nFuHdrLen);
			pSess->stRtpSender.nBuffLen += nFuHdrLen;
			memcpy(&pSendBuf[pSess->stRtpSender.nBuffLen], szNalData, nDataLen);
			pSess->stRtpSender.nBuffLen += nDataLen;
			nNalLen  -= nDataLen;
			szNalData += nDataLen;
			nFuNum++;
			if(send_rtp_data(pSess, pSendBuf, pSess->stRtpSender.nBuffLen, 0) < 0)
				return -1;
		}/*end while*/
	}/*end else*/
	return 0;
}

int rtsp_h264_find_nal(const char *pData, int nDataLen)
{
	int pos = 0;
	while(pos < nDataLen - 4)
	{
		if(pData[pos] == 0x00 && pData[pos + 1] == 0x00
		        && pData[pos + 2] == 0x00 && pData[pos + 3] == 0x01)
		{
			return pos;
		}
		pos++;
	}
	return -1;
}

int rtsp_h264_get_media_info(char *pBuffData, int nFrameLen, char *proLevelId, char *szSpsPps)
{
	unsigned char szSps[128] = {0}, szPps[128] = {0};
	unsigned char szBase64sps[256] = {0}, szBase64pps[256] = {0};
	int nPpsLen = 128, nSpsLen = 0, nPos = 0;
	int nNalUnitType = 0, nSps = 0, nPps = 0, nCurPos = 0;
	while(nCurPos < nFrameLen)
	{
		if((nPos = rtsp_h264_find_nal(&pBuffData[nCurPos], nFrameLen - nCurPos)) < 0)
		{
			return -1;
		}
		if(nNalUnitType == 7)
		{
			nSps++;
			if(nPos > 0)
				memcpy(szSps, &pBuffData[nCurPos], nPos);
			nSpsLen = nPos;
		}
		if(nNalUnitType == 8)
		{
			nPps++;
			if(nPos > 0)
				memcpy(szPps, &pBuffData[nCurPos], nPos);
			nPpsLen = nPos;
		}
		if(nSps > 0 && nPps > 0)
		{
			break;
		}
		nCurPos     += (nPos + 4);
		nNalUnitType = pBuffData[nCurPos] & 0x1f;
	}
	if(nSps == 0 || nPps == 0)
	{
		return -1;
	}
	sprintf(proLevelId, "%02x%02x%02x", szSps[1], szSps[2], szSps[3]);
	to64frombits((unsigned char *)szBase64sps, szSps, nSpsLen);
	to64frombits((unsigned char *)szBase64pps, szPps,  nPpsLen);
	sprintf(szSpsPps, "%s,%s", szBase64sps, szBase64pps);
	return 0;
}

static int rtsp_h264_video_send_sei(RTSP_SESSION_S *pSess,  unsigned long nPts)
{
	unsigned char szSeiData[25] = {0};
	time_t curTime;
	szSeiData[0] = 0x06;
	szSeiData[1] = 0xf0;
	szSeiData[2] = 0x10;
	szSeiData[3] = 0x00;
	curTime = time(NULL);
	szSeiData[4] = curTime & 0xff;
	szSeiData[5] = (curTime >> 8) & 0xff;
	szSeiData[6] = (curTime >> 16) & 0xff;
	szSeiData[7] = (curTime >> 24) & 0xff;
	szSeiData[10] = 0x03;
	szSeiData[13] = 0x03;
	szSeiData[16] = 0x03;
	szSeiData[19] = 0x03;
	szSeiData[22] = 0x03;
	szSeiData[24] = 0x80;
	return rtsp_h264_send_nal(pSess, (char *)szSeiData, sizeof(szSeiData), 0, nPts);
}

extern int nvr_find_nal_unit(char *pData, int nDataLen);

int rtsp_h264_video_send(RTSP_SESSION_S *pSess, const char *szBuff, int nFrameLen, int nFrameType,
                         unsigned long u32TimeStamp)
{
	int nCurPos = 0, nNalLen = 0, nNalUnitType = 0;
	unsigned long nPts = 0;
	time_t curTime;

	if (pSess->stRtpSender.u32LastSndTs[0] == 0 || u32TimeStamp < pSess->stRtpSender.u32LastSndTs[0])
		nPts = 3600;
	else
		nPts = pSess->stRtpSender.u32LastSndPts[0]+90*(u32TimeStamp-
						pSess->stRtpSender.u32LastSndTs[0]);

	if (u32TimeStamp < pSess->stRtpSender.u32LastSndTs[0])
		printf("rtsp_h264_video_send pts %lu last pts %lu\n", u32TimeStamp, 
		pSess->stRtpSender.u32LastSndTs[0]);
/*	
	if (nPts - pSess->stRtpSender.u32LastSndPts[0] != 3600)
		printf("rtsp pts error cur pts %d old pts%d\n", nPts, 
			 pSess->stRtpSender.u32LastSndPts[0]);
*/	
	curTime = time(NULL);
	if(curTime - pSess->preTime >= 1)
	{
		pSess->preTime = curTime;
		rtsp_h264_video_send_sei(pSess, nPts);
	}
	while(nCurPos < nFrameLen && nFrameLen > 0)
	{
		if((nNalLen = rtsp_h264_find_nal(&szBuff[nCurPos], 
										nFrameLen - nCurPos)) < 0)
		{
			if(rtsp_h264_send_nal(pSess, &szBuff[nCurPos], nFrameLen - nCurPos,
					1, nPts) < 0)
			{
					return -1;
			}
			
			break;
		}
		
		if(nNalUnitType != 0/*nal_unit_type == 1 || nal_unit_type == 5*/)
		{
			if(rtsp_h264_send_nal(pSess, &szBuff[nCurPos], nNalLen, 0, nPts) < 0)
			{
					return -1;
			}
		}

		nCurPos += nNalLen + 4;
		nNalUnitType = szBuff[nCurPos] & 0x1f;
		
	}	

	pSess->stRtpSender.u32LastSndTs[0]  = u32TimeStamp;
	pSess->stRtpSender.u32LastSndPts[0] = nPts;
	return 0;
}

