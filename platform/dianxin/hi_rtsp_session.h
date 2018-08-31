#ifndef _HI_RTSP_SESSION_H_
#define _HI_RTSP_SESSION_H_

#include "hi_rtsp_struct.h"
/*
* Copyright (c) 2010
* All rights reserved.
* 
* filename:hi_rtsp_session.h
* 
* func:本文件根据实现了rtsp 会话处理
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


#define ERR_RTSP_SOCKET          1

#define ERR_RTSP_MALLOC          2

#define ERR_RTSP_SOCKET_CLOSE 	 3

#define ERR_VOD_SVR_THREAD       4

#define ERR_RTSP_PARSE_ERROR     5

#define ERR_RTSP_SEQ  			 6

#define ERR_RTSP_NOT_SUPPORT_CMD 7



#define RTSP_BAD_STATUS_BEGIN 202

/*Setup 消息中的track 信息*/
#define RTSP_TRACKID_VIDEO 0
#define RTSP_TRACKID_AUDIO 1


RTSP_SESSION_S *rtsp_session_create(int nSock);

void 			rtsp_session_lists_add(RTSP_SESSION_S *pSess);
	
unsigned long   rtsp_session_close(RTSP_SESSION_S* pSess);

int 			rtsp_session_process(RTSP_SESSION_S *pSess);

unsigned long   rtsp_handle_event(RTSP_SESSION_S *pSess, int event, 
								  int status, char *buf);

void 			check_rtsp_session_state(void);
	
void 			rtsp_list_client();

int 			rtsp_check_chn(int chn);
	
void 			check_rtsp_sock(int fd, RTSP_SESSION_S *pSess);

unsigned long   rtsp_make_resp_header(RTSP_SESSION_S *pSess, int err);

#ifdef __cplusplus
}
#endif 

#endif

