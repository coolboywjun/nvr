#ifndef _HI_FTP_H_
#define _HI_FTP_H_

#include "nvr_headers.h"
#include "nvr_net_interface.h"

typedef int (*ExceptionNotify)();

typedef struct _HI_FTP_MAN_S_
{
	unsigned char		u8Init;
	unsigned char		u8Exits;
	unsigned char		u8ThreadRun;
	unsigned char		u8NeedReSend;
	int					s32MsgId;
	int					s32MsgNum;
	
	pthread_t			thdId;
	pthread_mutex_t 	mutexTask;
	pthread_cond_t   	condTask;	
	UpdateCb	funcUpdateCb;
	ExceptionNotify exceptionNotifyCb;
}HI_FTP_MAN_S, *LPHI_FTP_MAN_S;

typedef struct _HI_FTP_MSG_S_
{
	char				szSrcName[128];
	char				szDistName[128];
}HI_FTP_MSG_S, *LPHI_FTP_MSG_S;

//////////////////////////////////////add by hjl

int hi_ftp_set_cb(ExceptionNotify exceptionNotifyCb);



int	hi_ftp_init(UpdateCb funcUpdateCb);

int	hi_ftp_uninit();

int	hi_ftp_send(char *pSrcFileName, char *pDstFileName, int bDownLoad);
#endif

