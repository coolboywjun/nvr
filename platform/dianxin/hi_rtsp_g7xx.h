#ifndef _HI_RTSP_G7XX_H_
#define _HI_RTSP_G7XX_H_

#include "hi_rtsp_struct.h"
/*
* Copyright (c) 2010
* All rights reserved.
* 
* filename:hi_rtsp_g7XX.h
* 
* func:本文件实现了对rtsp g726 g711a g711u音频的封装
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

int rtsp_g7xx_audio_send(RTSP_SESSION_S* pSess, const char *szBuff, int nFrameLen, 
						 unsigned long u32TimeStamp, unsigned long u32SampleRate);
int rtsp_g7xx_lsit_audio_send(struct list_head* pSessHead, const char *szBuff, int nFrameLen, 
						 unsigned long u32TimeStamp, unsigned long u32SampleRate);

#ifdef __cplusplus
}
#endif 


#endif
