#ifndef _HI_RTSP_H264_H_
#define _HI_RTSP_H264_H_

#include "hi_rtsp_struct.h"
/*
* Copyright (c) 2010
* All rights reserved.
* 
* filename:hi_rtsp_h264.h
* 
* func:本文件根据rfc3984实现了对h264视频流的rtp封装
* 
* author : hi
* createdate: 2011-01-27
*
* modify list: 
* 
*/
#ifdef __cplusplus
extern "C" {
#endif 


#ifndef PACKED
#define PACKED	__attribute__((packed, aligned(1)))
#endif

//================================================================
//类型:结构体
//名称:FUIndicatorDef
//作用:FU类型第二字节的指示
//================================================================
typedef struct tagFUIndicator
{
	char TYPE : 5;		/* set to 28 or 29 */
	char NRI : 2;		/* the same to nal */
	char F : 1; 		/* set to 0 */
}PACKED FUIndicatorDef;


/**************************************************************************
类型:结构体
名称:FUHeaderDef
作用:FU第一字节头结构
***************************************************************************/
typedef struct tagFUHeader
{
	char Type : 5;		/* set to nal type */
	char R : 1; 		/* reserve */
	char E : 1; 		/* end flag */
	char S : 1; 		/* start flag */
}PACKED  FUHeaderDef;

/**************************************************************************
类型:结构体
名称:FU_ADef
作用:FU头结构
***************************************************************************/
typedef struct tagFU_A
{
/*根据RFC3984  FU-A的RTP荷载格式*/
/*+---------------+
  |0|1|2|3|4|5|6|7|
  +-+-+-+-+-+-+-+-+
  |F|NRI|Type = 1c|
  +---------------+*/
	FUIndicatorDef stFUIndicator;
/*+---------------+
  |0|1|2|3|4|5|6|7|
  +-+-+-+-+-+-+-+-+
  |S|E|R| NalType |
  +---------------+*/
	FUHeaderDef stFUHeader;
}PACKED FU_ADef;

	int rtsp_h264_video_send(RTSP_SESSION_S *pSess, const char *szBuff, int nFrameLen, int nFrameType,
	                         unsigned long u32TimeStamp);

int rtsp_h264_get_media_info(char *pBuffData, int nFrameLen, char *proLevelId, char *szSpsPps);

int rtsp_h264_list_video_send(struct list_head* pSessHead, const char *szBuff, int nFrameLen, 
						 unsigned long u32TimeStamp);


#ifdef __cplusplus
}
#endif 

#endif
