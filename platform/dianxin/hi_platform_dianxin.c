/////////////////////////////////////////////////////////////////////////////////
////////////////////////// hi_platform_dianxin.c ////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


/*-------------------------------------------------*
 *包含头文件                                       *
 *-------------------------------------------------*/
#include "hi_platform_dianxin.h"
#include "net_api.h"
#include "hi_dianxin_proc.h"
#include "hi_dianxin_turn.h"
//#include "hi_upnp.h"
#include "nvr_lib_interface.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/prctl.h>
#include "hi_ftp_api.h"
#include "hi_ftp.h"


/*-------------------------------------------------*
 *外部变量说明                                     *
 *-------------------------------------------------*/

/*-------------------------------------------------*
 *外部函数原型说明                                 *
 *-------------------------------------------------*/

/*-------------------------------------------------*
 *内部函数原型说明                                 *
 *-------------------------------------------------*/

/*-------------------------------------------------*
 *全局变量                                         *
 *-------------------------------------------------*/

/*-------------------------------------------------*
 *模块级变量                                       *
 *-------------------------------------------------*/
DIANXIN_CTRL stDianxinCtrl;

/*-------------------------------------------------*
 *常量定义                                         *
 *-------------------------------------------------*/

/*-------------------------------------------------*
 *宏定义                                           *
 *-------------------------------------------------*/



/*-------------------------------------------------*
 *函数实现                                         *
 *-------------------------------------------------*/



int hi_dianxin_register()
{
	int ret = -1;
	HI_DIANXIN_PLATRORM_CFG *pPlatformCfg = &stDianxinCtrl.stPlatformCfg;
	__D(">>>>>>>>>>hi_group_register begin!\r\n");
	if(strlen(pPlatformCfg->szProvincialUrl) == 0)
	{
		if(strlen(pPlatformCfg->szGroupUrl) == 0)
		{
			__E("the GROUP URL is NULL!\r\n");
			return -1;
		}
		hi_dianxin_group_register(&stDianxinCtrl);
	}else 
	{
		ret = hi_dianxin_lcms_register(&stDianxinCtrl);
	}

	return ret;
	
}

int dianxin_create_message_sock(HI_DIANXIN_REGISTER_RES *pDinxinRegisterRes)
{
	int nFd;
	if((nFd = net_tcp_noblock_connect(NULL, NULL, pDinxinRegisterRes->szNatIP, pDinxinRegisterRes->szNatPort, 3000)) < 0)
	{
		if((nFd = net_tcp_noblock_connect(NULL, NULL, pDinxinRegisterRes->szBackupNatIP, pDinxinRegisterRes->szBackupNatPort, 3000)) < 0)
		{
			return -1;
		}
	}
	return nFd;
}

DIANXIN_CTRL *hi_dianxin_get_ctrl()
{
	return &stDianxinCtrl;
}

int hi_dianxin_check_shoot()
{
#if 0	
	HI_DIANXIN_SHOOT_FTP_S stDianxinShootCfg;
	hi_platform_get_dianxin_shoot_cfg(0, &stDianxinShootCfg);
	
	int i;
	time_t curTime;
	struct tm tmTime;
	curTime = time(NULL);
	localtime_r(&curTime, &tmTime);
	for(i = 0; i < 3; i++)
	{
		if((stDianxinShootCfg.struSeg[i].nOpen == 1) &&(stDianxinShootCfg.struSeg[i].nHour == tmTime.tm_hour)
			&& (stDianxinShootCfg.struSeg[i].nMinute == tmTime.tm_min))
		{
			hi_shoot_pic(0, 1, 0, 0x1, 0);
		}
	}
#endif	
	return 0;
}

#if 0
int hi_dianxin_send_alarm(LPHI_ALARM_INFO_S pAlarmInfo)
{
	DIANXIN_CTRL *pDianxinCtrl = hi_dianxin_get_ctrl();
	hi_dianxin_alarm_notify_req(pDianxinCtrl, pAlarmInfo);
	return 0;
}
#endif



int dianxin_video_varify_proc(char *pDevId, char *pChannelNo, 
			char *pStreamType, char *pHashToken)
{
	DIANXIN_VIDEO_PLAY stVideoPlay;
	strcpy(stVideoPlay.szDevId, pDevId);
	strcpy(stVideoPlay.szChannelNo, pChannelNo);
	strcpy(stVideoPlay.szStreamType, pStreamType);
	strcpy(stVideoPlay.szHashToken, pHashToken);
	return hi_dianxin_video_play_req(&stVideoPlay);
}

int dianxin_get_video_flag()
{
	return stDianxinCtrl.nVideoRes;
}

int dianxin_vod_varify_proc(char *pDevId, char *pChannelNo, 
			char *pFileName, char *pHashToken)
{
	DIANXIN_VOD_PLAY stVodPlay;
	strcpy(stVodPlay.szDevId, pDevId);
	strcpy(stVodPlay.szChannelNo, pChannelNo);
	strcpy(stVodPlay.szFileName, pFileName);
	strcpy(stVodPlay.szHashToken, pHashToken);
	return hi_dianxin_record_play_req(&stVodPlay);
}

int dianxin_get_vod_flag()
{
	return stDianxinCtrl.nVodRes;
}

void *dianxin_message_proc(void *args)
{
	int ret;
	int nRandom, curtime;
	char szRelayIp[NVR_IP_ADDR_LEN] = {0};
	int nRelayPort = 0;		
	struct timespec tp;	

	prctl(PR_SET_NAME, "DianXinMsgProc");
	while(stDianxinCtrl.nExit == 0)
	{
		if(stDianxinCtrl.nRegisterFlag == 0)
		{
			ret = hi_dianxin_register();
			if(ret < 0)
			{
				if(ret == -2)
				{
					nRandom = random() % 10;
					sleep(nRandom);
					sleep(5);
					continue;
				}else
				{
					nRandom = random() % 60;
					sleep(nRandom);
					sleep(300);
					continue;
				}
			}else 
			{
				stDianxinCtrl.nRegisterFlag = 1;
				hi_turn_start_client(stDianxinCtrl.stRegister.szTurnIP, atoi(stDianxinCtrl.stRegister.szTurnPort),
										stDianxinCtrl.stPlatformCfg.szDevId, stDianxinCtrl.stPlatformCfg.szKey);
			}
			__D(">>>>>register success!\r\n");
		}
		if(stDianxinCtrl.nSocketFd <= 0)
		{
			stDianxinCtrl.nLoginStatus = DIANXIN_NO_LOGIN;
			stDianxinCtrl.nSocketFd = dianxin_create_message_sock(&stDianxinCtrl.stRegister);
			__D("the nSocketFd is %d!\r\n", stDianxinCtrl.nSocketFd);
			if(stDianxinCtrl.nSocketFd <= 0)
			{
				nRandom = random() % 10;
				sleep(nRandom);
				sleep(5);
		//		stDianxinCtrl.nRegisterFlag = 0;
		//		hi_turn_stop_client();
				continue;
			}
		}
		if(stDianxinCtrl.nLoginStatus == DIANXIN_NO_LOGIN || 
			stDianxinCtrl.nLoginStatus == DIANXIN_LOGIN_ERR)
		{
			if(stDianxinCtrl.nLoginStatus == DIANXIN_LOGIN_ERR)
			{
				nRandom = random() % 10;
				sleep(nRandom);
			}
			stDianxinCtrl.u32LastActiveTime = 0;
			ret = hi_dianxin_login_req(&stDianxinCtrl);
			if(ret < 0)
			{
				net_close_socket(&stDianxinCtrl.nSocketFd);
				stDianxinCtrl.nLoginStatus = DIANXIN_NO_LOGIN;
				nRandom = random() % 10;
				sleep(nRandom);
				continue;
			}
		}else if(stDianxinCtrl.nLoginStatus == DIANXIN_FIRST_LOGIN)
		{
			if(hi_dianxin_abilityup_req(&stDianxinCtrl) < 0)
			{
				net_close_socket(&stDianxinCtrl.nSocketFd);
				stDianxinCtrl.nLoginStatus = DIANXIN_NO_LOGIN;
				continue;
			}
		}else if(stDianxinCtrl.nLoginStatus == DIANXIN_LOGIN)
		{
			clock_gettime(CLOCK_MONOTONIC, &tp);
			curtime = tp.tv_sec;
			if(stDianxinCtrl.u32LastActiveTime == 0 || (curtime - stDianxinCtrl.u32LastActiveTime >= 120)
				|| ((stDianxinCtrl.u32NoActiveTime > 0)&&(curtime - stDianxinCtrl.u32LastActiveTime >= 10)))
			{
				if(hi_dianxin_heart_beat_req(&stDianxinCtrl) < 0)
				{
					net_close_socket(&stDianxinCtrl.nSocketFd);
					stDianxinCtrl.nLoginStatus = DIANXIN_NO_LOGIN;
					continue;
				}

				stDianxinCtrl.u32LastActiveTime = curtime;
				stDianxinCtrl.u32NoActiveTime += 1;
			}
			else if(hi_turn_get_relay_info(szRelayIp, sizeof(szRelayIp), &nRelayPort) == 0
				&& (strcmp(szRelayIp, stDianxinCtrl.szRelayIp) != 0 || nRelayPort != stDianxinCtrl.nRelayPort))
			{
				if(hi_dianxin_heart_beat_req(&stDianxinCtrl) < 0)
				{
					stDianxinCtrl.nLoginStatus = DIANXIN_NO_LOGIN;
					net_close_socket(&stDianxinCtrl.nSocketFd);
					continue;
				}
				stDianxinCtrl.u32LastActiveTime = curtime;
				stDianxinCtrl.u32NoActiveTime += 1;
			}
			
		}
		if(stDianxinCtrl.u32NoActiveTime > 3)
		{
			__E("the heart beat no res %d times!\r\n", stDianxinCtrl.u32NoActiveTime);
			close(stDianxinCtrl.nSocketFd);
			stDianxinCtrl.nSocketFd = -1;
			stDianxinCtrl.u32NoActiveTime = 0;
			continue;
		}
		if(stDianxinCtrl.nExceptionNeedSend == 1 && stDianxinCtrl.nLoginStatus != DIANXIN_NO_LOGIN)
		{
			usleep(100);
			if(hi_dianxin_exception_notify_proc(&stDianxinCtrl) < 0)
			{
				stDianxinCtrl.nLoginStatus = DIANXIN_NO_LOGIN;
				net_close_socket(&stDianxinCtrl.nSocketFd);
				continue;
			}
		}
		
		if(hi_dianxin_recv(&stDianxinCtrl) < 0)
		{
			stDianxinCtrl.nLoginStatus = DIANXIN_NO_LOGIN;
			continue;
		}
		
	}
	return NULL;
}
int hi_dianxin_update_exception_notify_cb()
{
	DIANXIN_CTRL *pDianxinCtrl = hi_dianxin_get_ctrl();
	pDianxinCtrl->nExceptionNeedSend = 1;
	pDianxinCtrl->nExceptionType = 11;
	return 0;
}

int hi_dianxin_pf_init(NVR_DEV_DIANXIN_PLATFORM_CFG_S *pDevPlatformCfg)
{
	HI_DIANXIN_PLATRORM_CFG *pPlatformCfg;
	NVR_UPNP_CFG_S stUpnpCfg;
	
	memset(&stDianxinCtrl, 0, sizeof(stDianxinCtrl));
	//stDevPlatformCfg.u32Size = sizeof(NVR_DEV_DIANXIN_PLATFORM_CFG_S);
	//strcpy(stDevPlatformCfg.szProvincialUrl, LCMSURL);
	//strcpy(stDevPlatformCfg.szDevId, DEVICEID);
	//strcpy(stDevPlatformCfg.szKey, KEY);
	//strcpy(stDevPlatformCfg.szMode, MODE);
	
	printf("Group Url: %s\n", pDevPlatformCfg->szGroupUrl);
	printf("Provincial Url: %s\n", pDevPlatformCfg->szProvincialUrl);
	printf("Key: %s\n", pDevPlatformCfg->szKey);
	printf("Mode: %s\n", pDevPlatformCfg->szMode);
	printf("DevID: %s\n", pDevPlatformCfg->szDevId);
	printf("local rtsp port: %u\n", pDevPlatformCfg->localRtspPort);
	printf("upnp rtsp port: %u\n", pDevPlatformCfg->upnpRtspPort);
	if((strlen(pDevPlatformCfg->szGroupUrl) == 0 && strlen(pDevPlatformCfg->szProvincialUrl) == 0)
		|| strlen(pDevPlatformCfg->szDevId) == 0
		|| strlen(pDevPlatformCfg->szKey) == 0
		|| strlen(pDevPlatformCfg->szMode) == 0)
	{
		__E("the dianxin platform param is NULL!\r\n");
		return -1;
	}
	hi_turn_client_init();
	
	pPlatformCfg = &stDianxinCtrl.stPlatformCfg;

	strcpy(pPlatformCfg->szGroupUrl, pDevPlatformCfg->szGroupUrl);
	strcpy(pPlatformCfg->szProvincialUrl, pDevPlatformCfg->szProvincialUrl);
	strcpy(pPlatformCfg->szDevId, pDevPlatformCfg->szDevId);
	strcpy(pPlatformCfg->szKey, pDevPlatformCfg->szKey);
	strcpy(pPlatformCfg->szMode, pDevPlatformCfg->szMode);
	if(pDevPlatformCfg->localRtspPort == 0)
		pPlatformCfg->localRtspPort = LOCAL_RTSP_PORT;
	else
		pPlatformCfg->localRtspPort = pDevPlatformCfg->localRtspPort;
	nvr_logon_get_upnp_cfg(0, &stUpnpCfg);
	if(stUpnpCfg.u8UpnpOpen == 0 || stUpnpCfg.u16UpnpRtspPort == 0)
	{
		stUpnpCfg.u8UpnpOpen = 1;
		stUpnpCfg.u16UpnpRtspPort = pPlatformCfg->localRtspPort;
		nvr_logon_set_upnp_cfg(0, &stUpnpCfg);
	}
	//暂时屏蔽
	hi_ftp_set_cb(hi_dianxin_update_exception_notify_cb);

	if(pthread_create(&stDianxinCtrl.pthreadId, NULL, dianxin_message_proc, NULL) != 0)
	{
		__D("dianxin pthread creat err!\r\n");
		return -1;
	}
	
	__D(">>>>>>>the dianxin init!\r\n");
	return 0;
}


int hi_dianxin_pf_uninit()
{
	DIANXIN_CTRL *pDianxinCtrl = hi_dianxin_get_ctrl();
	stDianxinCtrl.nExit = 1;
	pthread_join(stDianxinCtrl.pthreadId, NULL);
	hi_turn_client_uninit();
	
	return 0;
}

