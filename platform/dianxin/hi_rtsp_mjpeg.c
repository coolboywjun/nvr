/******************************************************************************

                  版权所有 (C), 2011-2015, 佳信捷电子

 ******************************************************************************
  文 件 名   : hi_rtsp_mjpeg.c
  版 本 号   : v2.0
  作    者   : 梁锦明
  生成日期   : 2011年9月8日
  最近修改   :
  功能描述   : rtsp 发送mjpeg实现
  函数列表   :
              get_jpeg_attr
              jpeg_find_marker
              rtsp_mjpeg_video_send
              rtsp_send_mjpeg
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
#include "hi_rtsp_transfer.h"
#include "hi_rtsp_rtp.h"
#include "hi_rtsp_global.h"
#include "hi_rtsp_mjpeg.h"

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
int jpeg_find_marker(unsigned char **pbuf_ptr, unsigned char *buf_end)
{
	const unsigned char *buf_ptr;
	unsigned int v, v2;
	int val;
	
	buf_ptr = *pbuf_ptr;
	while (buf_ptr < buf_end) 
	{
		v = *buf_ptr++;
		v2 = *buf_ptr;
		if ((v == 0xff) && (v2 >= 0xc0) && (v2 <= 0xfe) && buf_ptr < buf_end) {
			val = *buf_ptr++;
			goto found;
		}
	}
	val = -1;
found:
	*pbuf_ptr = (unsigned char *)buf_ptr;
	return val;
}


int get_jpeg_attr(const char *frameBuf,int frameLen, unsigned char **jpeg_data, 
				  int *len, unsigned char *type, unsigned char *typespec,
				  int *width, int *height, int *dri,
                  unsigned char *q, unsigned char *lqt, unsigned char *cqt)
{
	unsigned char *buf_ptr,*buf_end;
	unsigned char h,l;
	int start_code = 0;
	
	buf_ptr = (unsigned char *)frameBuf;
	buf_end = (unsigned char *)(frameBuf+frameLen);
	*typespec= 0;
	
	while (buf_ptr < buf_end)
	{
		start_code = jpeg_find_marker(&buf_ptr, buf_end);
		switch (start_code) 
		{
			case SOI:
				break;
			case DQT:
				*q = 255;
				buf_ptr += 3;
				memcpy(lqt, buf_ptr, 64);
				buf_ptr += 65;
				memcpy(cqt, buf_ptr, 64);
				buf_ptr += 64;
				break;
			case DHT:
				break;
			case SOF0:
				buf_ptr += 3;
				*type = 1;
				h = *buf_ptr++;
				l = *buf_ptr++;
				*height = h << 8 | l;
				h = *buf_ptr++;
				l = *buf_ptr++;
				*width = h << 8 | l;
			case SOF1:
				break;
			case SOF2:
				break;
			case SOF3:
				break;
			case SOF48:
				break;
			case LSE:
				break;
			case EOI:
				return 0;
			case SOS:
				*jpeg_data = buf_ptr + 12;
				*len = (buf_end - buf_ptr)-12;
                return 1;
            case DRI:
				*dri = 1;
                break;
            case SOF5:
            case SOF6:
            case SOF7:
            case SOF9:
            case SOF10:
            case SOF11:
            case SOF13:
            case SOF14:
            case SOF15:
            case JPG:
                break;
		}
	}
	return 1;
}

int rtsp_send_mjpeg(RTSP_SESSION_S *pSess,unsigned int nPts,
               unsigned char *pJpegData, int nLen, unsigned char u8Type,
               unsigned char typespec, int nWidth, int nHeight, int nDri,
               unsigned char u8Q, unsigned char *lqt, unsigned char *cqt) 
{
	RTP_HDR_S 			rtpHdr;
	RTP_OVER_TCP_HEADER rtpOverTcpHdr;
	int nDollarLen = sizeof(RTP_OVER_TCP_HEADER);
    
    struct jpeghdr jpgHdr;
    struct jpeghdr_rst rstHdr;

    struct jpeghdr_qtable qTblHdr;
    unsigned char szPacketBuf[1500] = {0};
    unsigned char *ptr = NULL;
    int nByteLeft = nLen, nDataLen = 0;
	
    /* Initialize RTP header
     */
	rtpOverTcpHdr.u8Dollar= '$';
	rtpOverTcpHdr.u8ChnId = pSess->nChId;
	
	memset(&rtpHdr,  0, sizeof(rtpHdr));
    memset(&jpgHdr,  0, sizeof(jpgHdr));
	memset(&rstHdr,  0, sizeof(rstHdr)); 
	memset(&qTblHdr, 0, sizeof(qTblHdr)); 
	
	rtpHdr.version   = RTP_VERSION;
	rtpHdr.cc        = 0;
	rtpHdr.x         = 0;
	rtpHdr.p         = 0;
	rtpHdr.ts		 = htonl(nPts);
	rtpHdr.pt        = RTP_PT_JPEG;
	rtpHdr.ssrc      = htonl(pSess->stRtpSender.nVideoSsrc);
	
    /* Initialize JPEG header
     */
    jpgHdr.tspec  = typespec;
    jpgHdr.off    = 0;
    jpgHdr.type   = u8Type | ((nDri != 0) ? RTP_JPEG_RESTART : 0);
    jpgHdr.q      = u8Q;
    jpgHdr.width  = nWidth / 8;
    jpgHdr.height = nHeight / 8;

    /* Initialize DRI header
     */
   
    if (nDri != 0) {
        rstHdr.dri   = nDri;
        rstHdr.f     = 1;        /* This code does not align Ris */
        rstHdr.l     = 1;
        rstHdr.count = 0x3fff;
    }

    /* Initialize quantization table header
     */
    if (u8Q >= 128) {
        qTblHdr.mbz = 0;
        qTblHdr.precision = 0; /* This code uses 8 bit tables only */
        qTblHdr.length = htons(128);  /* 2 64-byte tables */
    }

	if (pSess->eTransType != RTP_TRANSPORT_TCP)
		nDollarLen = 0;
		
    while (nByteLeft > 0) 
	{
		rtpHdr.marker = 0;
        ptr = szPacketBuf + nDollarLen + RTP_HDR_SZ;
        memcpy(ptr, &jpgHdr, sizeof(jpgHdr));
	
		ptr += sizeof(jpgHdr);
		if (nDri != 0) {
			memcpy(ptr, &rstHdr, sizeof(rstHdr));
			ptr += sizeof(rstHdr);
		}
		
		if (u8Q >= 128 && jpgHdr.off == 0) {
			memcpy(ptr, &qTblHdr, sizeof(qTblHdr));
			ptr += sizeof(qTblHdr);
			memcpy(ptr, lqt, 64);
			ptr += 64;
			memcpy(ptr, cqt, 64);
			ptr += 64;
		}

		nDataLen = RTP_MAX_PACKET_BUFF - (ptr - szPacketBuf);
        if ((nDataLen) >= nByteLeft) 
		{
            nDataLen      = nByteLeft;
            rtpHdr.marker = 1;
        }
		
		rtpHdr.seqno = htons(++pSess->stRtpSender.u16LastSndSeq[0]);
		if (nDollarLen != 0)
		{
			rtpOverTcpHdr.u16PacketSize = htons((ptr - szPacketBuf) + nDataLen - nDollarLen);
			memcpy(szPacketBuf, &rtpOverTcpHdr, nDollarLen);
		}	
		
        memcpy(szPacketBuf+nDollarLen, &rtpHdr, RTP_HDR_SZ);
        memcpy(ptr, pJpegData + jpgHdr.off, nDataLen);

		if (send_rtp_data(pSess, (char*)szPacketBuf, (ptr - szPacketBuf) + nDataLen, 0) < 0)
			return -1;
		
		jpgHdr.off += nDataLen;
		nByteLeft  -= nDataLen;
    }/*end while*/

	return 1;
}

int rtsp_mjpeg_video_send(RTSP_SESSION_S *pSess, const char *szBuff, int nFrameLen, int nFrameType,
                          unsigned long u32TimeStamp)
{
	unsigned long nPts = 0;
	unsigned char *data= NULL;
	unsigned char u8Type = 0, u8TypeSpec = 0, u8Q = 0;
	int nWidth = 0, nHeight = 0, nDri = 0, nLen = 0;
	unsigned char lqt[64]= {0};
	unsigned char cqt[64]= {0};
	
	get_jpeg_attr(szBuff, nFrameLen, &data, &nLen, &u8Type, &u8TypeSpec,
			&nWidth, &nHeight, &nDri, &u8Q, lqt, cqt);
	
	if (pSess->stRtpSender.u32LastSndTs[0] == 0)
		nPts = 3600;
	else
		nPts = pSess->stRtpSender.u32LastSndPts[0]+90*(u32TimeStamp-
						pSess->stRtpSender.u32LastSndTs[0]);
	nDri = 0;

	rtsp_send_mjpeg(pSess, nPts, data, nLen, u8Type, u8TypeSpec,
		nWidth, nHeight, nDri, u8Q, lqt, cqt);	
			
	pSess->stRtpSender.u32LastSndTs[0]  = u32TimeStamp;
	pSess->stRtpSender.u32LastSndPts[0] = nPts;
	return 0;	
}

