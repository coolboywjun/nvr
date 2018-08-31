#ifndef _NVR_RTSP_RTP_H_
#define _NVR_RTSP_RTP_H_

#ifdef __cplusplus
extern "C" {
#endif

	/*
	* Copyright (c) 2010
	* All rights reserved.
	*
	* filename:nvr_rtsp_rtp.h
	*
	* func:本文件根据rfc3350实现了rtp封装
	*
	* author : nvr
	* createdate: 2011-01-27
	*
	* modify list:
	*
		*/

#define RTP_VERSION		    2

#ifndef PACKED
#define PACKED	__attribute__((packed, aligned(1)))
#endif

	//========================================================
	//RTP 相关结构

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

	/* total 12Bytes */
	typedef struct _RTP_HDR_S
	{
#if (BYTE_ORDER == LITTLE_ENDIAN)
		/* byte 0 */
		unsigned short cc      : 4;  /* CSRC count */
		unsigned short x       : 1;  /* header extension flag */
		unsigned short p       : 1;  /* padding flag */
		unsigned short version : 2;  /* protocol version */

		/* byte 1 */
		unsigned short pt      : 7;  /* payload type */
		unsigned short marker  : 1;  /* marker bit */
#elif (BYTE_ORDER == BIG_ENDIAN)
		/* byte 0 */
		unsigned short version : 2;  /* protocol version */
		unsigned short p       : 1;  /* padding flag */
		unsigned short x       : 1;  /* header extension flag */
		unsigned short cc      : 4;  /* CSRC count */
		/*byte 1*/
		unsigned short marker  : 1;  /* marker bit */
		unsigned short pt      : 7;  /* payload type */
#else
#error YOU MUST DEFINE BYTE_ORDER == LITTLE_ENDIAN OR BIG_ENDIAN !  
#endif


		/* bytes 2, 3 */
		unsigned short seqno  : 16;  /* sequence number */

		/* bytes 4-7 */
		unsigned long ts;            /* timestamp in ms */

		/* bytes 8-11 */
		unsigned long ssrc;          /* synchronization source */
	} RTP_HDR_S;


	typedef struct _RTP_OVER_TCP_HEADER
	{
		unsigned char  u8Dollar;
		unsigned char  u8ChnId;
		unsigned short u16PacketSize;
	} RTP_OVER_TCP_HEADER;

#ifdef __cplusplus
}
#endif

#endif

