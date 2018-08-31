#include <nvr_lib_interface.h>
#include "hi_ftp_api.h"
#include "hi_ftp.h"
#include "nvr_ipc_api.h"


static HI_FTP_MAN_S	gFtpMan;

int hi_ftp_set_cb(ExceptionNotify exceptionNotifyCb)
{
	gFtpMan.exceptionNotifyCb = exceptionNotifyCb;
	return 0;
}

int hi_ftp_upload(char *pSrcFile, char *pDstFile)
{
	HI_FTP_INFO  stFtpInfo;
	NVR_FTP_CFG_S struFtp;
	int          nErr = 0;
	
	memset(&stFtpInfo, 0, sizeof(HI_FTP_INFO));
	nvr_logon_get_ftp_cfg(0, &struFtp);	
	strncpy(stFtpInfo.serverIp, struFtp.szFtpSvr, NVR_IP_ADDR_LEN - 1);
	strncpy(stFtpInfo.userName, struFtp.szFtpUsr, NVR_USER_NAME_LEN-1);
	strncpy(stFtpInfo.passWord, struFtp.szFtpPsw, NVR_PASS_WORD_LEN-1);
	
	stFtpInfo.serverPort = struFtp.u16FtpPort;
	stFtpInfo.startPort  = 10080;
	stFtpInfo.endPort    = 10090;
	
	if ((nErr = hi_ftp_connect(&stFtpInfo)) <= 0 ||
		(nErr = hi_ftp_login(&stFtpInfo)) < 0 ||
		(nErr = hi_ftp_upload_file(&stFtpInfo, pSrcFile, pDstFile)) < 0)
	{
		nErr = -1;
	}
	else
		printf("Ftp Upload file %s success !\n", pSrcFile);
	hi_ftp_disconnect(&stFtpInfo);

	return nErr;
}

int hi_ftp_download(char *pSrcFile, UpdateCb funcUpdateCb)
{
	HI_FTP_INFO  stFtpInfo;
	NVR_FTP_CFG_S struFtp;
	int          nErr = 0;
	int			 resetFlag = 0;
	
	memset(&stFtpInfo, 0, sizeof(HI_FTP_INFO));

	nvr_logon_get_ftp_cfg(0, &struFtp);	
	strncpy(stFtpInfo.serverIp, struFtp.szFtpSvr, NVR_IP_ADDR_LEN - 1);
	strncpy(stFtpInfo.userName, struFtp.szFtpUsr, NVR_USER_NAME_LEN-1);
	strncpy(stFtpInfo.passWord, struFtp.szFtpPsw, NVR_PASS_WORD_LEN-1);	
	stFtpInfo.serverPort = struFtp.u16FtpPort;
	stFtpInfo.startPort  = 10080;
	stFtpInfo.endPort    = 10090;
	
	if ((nErr = hi_ftp_connect(&stFtpInfo)) < 0 ||
		(nErr = hi_ftp_login(&stFtpInfo)) < 0 ||
		(nErr = hi_ftp_download_file(&stFtpInfo,  pSrcFile, funcUpdateCb)) < 0)
	{
		printf("Ftp Download file %s failed %d\n", pSrcFile, nErr);
		gFtpMan.exceptionNotifyCb();
		nErr = -1;
	}
	else
	{
		printf("Ftp Download file %s success !\n", pSrcFile);
		resetFlag = 1;
	}
	
	hi_ftp_disconnect(&stFtpInfo);
	if(resetFlag)
		nvr_restart();
	return 0;
}

int	nvr_ftp_create_msg_list()
{
	if ((gFtpMan.s32MsgId = nvr_ipc_msg_init(NVR_FTP_KEY)) < 0)
	{
		printf("ftp init msg err [%s %d]\n", __FILE__, __LINE__);
		return -1;
	}
	return gFtpMan.s32MsgId;
}


void hi_ftp_task(void *arg)
{
	NVR_MSG_S stMsg;
	HI_FTP_MSG_S *pFtpMsg = (HI_FTP_MSG_S *)(&stMsg.szMsg[0]);
	int nMsgLen = 0;

	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "ftp");
	
	gFtpMan.u8ThreadRun = 1;
	pthread_cond_signal(&gFtpMan.condTask);

	while (gFtpMan.u8Exits == 0)
	{
		if (gFtpMan.s32MsgId < 0 && nvr_ftp_create_msg_list() < 0)
		{
			sleep(5);
			continue;
		}

		if ((nMsgLen = nvr_ipc_msg_recv(gFtpMan.s32MsgId, &stMsg, sizeof(HI_FTP_MSG_S), 1)) < 1)
		{
			printf("ftp recv ipc msg err %d\n", nMsgLen);
			nvr_ipc_del_msq(gFtpMan.s32MsgId);
			gFtpMan.s32MsgId = -1;
			continue ;
		}
		if (stMsg.ulMsgType != NVR_FTP_UP_MSG && stMsg.ulMsgType != NVR_FTP_DOWN_MSG)
		{
			printf("ftp recv err msg type%d\n", stMsg.ulMsgType);
			continue;
		}
		gFtpMan.s32MsgNum -= 1;
		
		if (stMsg.ulMsgType == NVR_FTP_UP_MSG)
			hi_ftp_upload(pFtpMsg->szSrcName, pFtpMsg->szDistName);	
		else
			hi_ftp_download(pFtpMsg->szSrcName, gFtpMan.funcUpdateCb);
	}

	gFtpMan.u8ThreadRun = 0;
	printf("hi ftp task exits\n");
	pthread_cond_signal(&gFtpMan.condTask);
}

int	hi_ftp_init(UpdateCb funcUpdateCb)
{
	memset(&gFtpMan, 0, sizeof(HI_FTP_MAN_S));

	pthread_mutex_init(&gFtpMan.mutexTask, NULL);
	pthread_cond_init(&gFtpMan.condTask, NULL);
	gFtpMan.s32MsgId = -1;
	gFtpMan.funcUpdateCb = funcUpdateCb;
	pthread_create(&gFtpMan.thdId, NULL, (void *)&hi_ftp_task, NULL);

	nvr_pthread_wait(&gFtpMan.mutexTask, &gFtpMan.condTask, 100);
	gFtpMan.u8Init = 1;
	return NVR_SUCCESS;
}

int	hi_ftp_uninit()
{
	NVR_MSG_S stMsg = {0};
	
	if (gFtpMan.u8Init == 0)
		return NVR_SUCCESS;
	
	if (gFtpMan.u8ThreadRun == 1) //
	{
		gFtpMan.u8Exits = 1;
		if (gFtpMan.s32MsgId >= 0)
			nvr_ipc_msg_send(gFtpMan.s32MsgId, &stMsg, sizeof(HI_FTP_MSG_S), 0);

		nvr_pthread_wait(&gFtpMan.mutexTask, &gFtpMan.condTask, 100);
	}

	if (gFtpMan.s32MsgId >= 0) 
	{
		nvr_ipc_clear_msg(gFtpMan.s32MsgId, &stMsg, sizeof(HI_FTP_MSG_S));
		nvr_ipc_del_msq(gFtpMan.s32MsgId);
	}		
	pthread_mutex_destroy(&gFtpMan.mutexTask);
	pthread_cond_destroy(&gFtpMan.condTask);			
	printf("hi ftp uninit ok .......[%s %d]\n", __FILE__, __LINE__);
	return NVR_SUCCESS;
}

int	hi_ftp_send(char *pSrcFileName, char *pDstFileName, int bDownLoad)
{
	int      nRet = 0;
	NVR_MSG_S stMsg = {0};
	HI_FTP_MSG_S *pStMsg = (HI_FTP_MSG_S *)&stMsg.szMsg[0];

	if (gFtpMan.u8Init != 1)
		return NVR_SUCCESS;
	
	if (gFtpMan.s32MsgId <= 0)
	{
		return NVR_FAILURE;
	}

	if (gFtpMan.s32MsgNum > NVR_MAX_SYS_IPC_MSG) 
	{
		printf("ftp too more ipc msg, Cleaning...\n");
		nvr_ipc_clear_msg(gFtpMan.s32MsgId, &stMsg, sizeof(HI_FTP_MSG_S));
		gFtpMan.s32MsgNum = 0;
	}

	memset(pStMsg, 0, sizeof(HI_FTP_MSG_S));
	
	stMsg.ulMsgType = (bDownLoad == 0) ? NVR_FTP_UP_MSG : NVR_FTP_DOWN_MSG;
	if (pSrcFileName && strlen(pSrcFileName) > 0)
		strncpy(pStMsg->szSrcName, pSrcFileName, strlen(pSrcFileName));
	
	if (pDstFileName && strlen(pDstFileName) > 0)
		strncpy(pStMsg->szDistName, pDstFileName, strlen(pDstFileName));

	if ((nRet = nvr_ipc_msg_send(gFtpMan.s32MsgId, &stMsg, sizeof(HI_FTP_MSG_S), 0)) != 0)
	{
		printf("ftp send ipc msg err %d\n", nRet);
		return NVR_FAILURE;
	}	
	++gFtpMan.s32MsgNum;
	return NVR_SUCCESS;	
}

