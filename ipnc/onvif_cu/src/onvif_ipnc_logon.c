
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "onvif_api.h"
#include "onvif_ipnc_mng.h"
#include "onvif_ipnc_logon.h"
#include "onvif_ipnc_param.h"


int onvif_ipnc_logon(long handle, long *logonHandle)
{	
	int ret;
	char acPort[8];
	char *profileToken = NULL;
	onvif_logonInfo_t *pLoginInfo = (onvif_logonInfo_t *)malloc(sizeof(onvif_logonInfo_t));
	//printf("@@@@@@@@@@@@@@ onvif_ipnc_logon\r\n");
	ONVIF_IPNC_PROTOCOL_MNG *ipncHandle = (ONVIF_IPNC_PROTOCOL_MNG *)handle;	
	if(NULL == ipncHandle)
		return -1;
	memset(&pLoginInfo->profile, 0, sizeof(onvif_profile_list_t));
	INIT_LIST_HEAD(&pLoginInfo->profile.list);
	sprintf(acPort, "%d", ipncHandle->port);	
	strcpy(pLoginInfo->ip, ipncHandle->ipAddr);
	strcpy(pLoginInfo->port, acPort);
	ret = nvr_onvif_get_profiles(ipncHandle->ipAddr, acPort, &pLoginInfo->profile);
	if (ret < 0)
	{
		printf("get profile failed\r\n");
		return -1;
	}
	profileToken = onvif_get_profile_token(&pLoginInfo->profile, 0);
	memset(&pLoginInfo->presets, 0, sizeof(onvif_ptzpresets_list_t));
	INIT_LIST_HEAD(&pLoginInfo->presets.list);
	ret = nvr_onvif_get_presets(ipncHandle->ipAddr, acPort, profileToken, &pLoginInfo->presets);
	if (ret < 0)
	{
		printf("get presets failed\r\n");
		return -1;
	}
	
	*logonHandle = (long)pLoginInfo;

	return 0;
}

int onvif_ipnc_logout(long logonHandle)
{
	//printf("@@@@@@@@@@@@@@ onvif_ipnc_logout\r\n");	
	onvif_logonInfo_t *pLoginInfo = (onvif_logonInfo_t *)logonHandle;
	if(((long)NULL == logonHandle) || (0 == logonHandle))
		return -1;	
	
	_onvif_list_clear(onvif_ptzpresets_list_t, (&pLoginInfo->profile));		
	_onvif_list_clear(onvif_ptzpresets_list_t, (&pLoginInfo->presets));
	free(pLoginInfo);
	return 0;
}

int onvif_ipnc_get_param(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize)
{	
	ONVIF_NET_CMD_PARAM_S* pCmdParam = NULL;
		
	if(NULL == pParam)
		return -1;
	if(((long)NULL == logonHandle) || (0 == logonHandle))
		return -1;
	
	pCmdParam = (ONVIF_NET_CMD_PARAM_S*)onvif_net_param_cmd(cmd);
	
	if(NULL != pCmdParam)
	{
		//if(!pCmdParam->checkParamProc(logonHandle, 0, pParam, dataSize))
		return pCmdParam->getParamProc(logonHandle, 0, pParam, dataSize);
	}
	else		
		printf("pCmdParam is null!\n");

	printf("pParam check is error!\n");
	return -1;
}
int onvif_ipnc_set_param(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize)
{	
	ONVIF_NET_CMD_PARAM_S* pCmdParam = NULL;
	
	if(NULL == pParam)
		return -1;

	if(((long)NULL == logonHandle) || (0 == logonHandle))
		return -1;
	
	pCmdParam = (ONVIF_NET_CMD_PARAM_S*)onvif_net_param_cmd(cmd);
	if(NULL != pCmdParam)
	{
		if(!pCmdParam->checkParamProc(logonHandle, 0, pParam, dataSize))
			return pCmdParam->setParamProc(logonHandle, 0, pParam, dataSize);
	}
	else		
		printf("pCmdParam is null!\n");

	printf("pParam check is error!\n");
	return -1;
}

int onvif_ipnc_ptz_ctrl(long logonHandle, void *ptzPacket)
{		
	int ret = 0;
	float speed_x = 0, speed_y = 0;
	unsigned char u8Speed = 0;
	char *profileToken = NULL;
	onvif_logonInfo_t *pLogonInfo = NULL;
	IPNC_PTZ_CTRL_S *pPtzCtrl	 = NULL;	
	if(((long)NULL == logonHandle) 
		 || (0 == logonHandle)
		 || (NULL == ptzPacket))
		return -1;
	
	pPtzCtrl = (IPNC_PTZ_CTRL_S *)ptzPacket;
	pLogonInfo	 = (onvif_logonInfo_t *)logonHandle;
	profileToken = onvif_get_profile_token(&pLogonInfo->profile, 0);
	u8Speed = pPtzCtrl->struPreset.u8Speed;
	
	switch(pPtzCtrl->u16PtzCmd)
	{
		case PTZ_CMD_STOP:	// Í£Ö¹
		{
			printf("===ptz_stop===\r\n");
			ret = nvr_onvif_ptz_stop(pLogonInfo->ip, pLogonInfo->port, 
				profileToken);
			break;
		}	
		case PTZ_CMD_SET_AUTOSCAN_START:	// ×Ô¶¯
		{
			break;
		}	
		case PTZ_CMD_LEFT:	// ×ó
		{
			speed_x = (-1.0)*u8Speed/128;
			speed_y = 0;
			printf("===ptz_left===\r\n");
			ret = nvr_onvif_ptz_continuous_move(pLogonInfo->ip, 
				pLogonInfo->port, profileToken, speed_x, speed_y);
			break;
		}	
		case PTZ_CMD_RIGHT:	// ÓÒ
		{
			speed_x = (1.0)*u8Speed/128;
			speed_y = 0;				
			printf("===ptz_right===\r\n");
			ret = nvr_onvif_ptz_continuous_move(pLogonInfo->ip, 
				pLogonInfo->port, profileToken, speed_x, speed_y);
			break;
		}	
		case PTZ_CMD_UP:	// ÉÏ
		{
			speed_x = 0;
			speed_y = (-1.0)*u8Speed/128;			
			printf("===ptz_up===\r\n");
			ret = nvr_onvif_ptz_continuous_move(pLogonInfo->ip, 
				pLogonInfo->port, profileToken, speed_x, speed_y);
			break;
		}
		case PTZ_CMD_DOWN:	// ÏÂ
		{
			speed_x = 0;
			speed_y = (1.0)*u8Speed/128;
			printf("===ptz_down===\r\n");
			ret = nvr_onvif_ptz_continuous_move(pLogonInfo->ip, 
				pLogonInfo->port, profileToken, speed_x, speed_y);
			break;
		}
		case PTZ_CMD_LEFT_UP:	//×óÉÏ
		{
			speed_x = (-1.0)*u8Speed/128;
			speed_y = (-1.0)*u8Speed/128;
			ret = nvr_onvif_ptz_continuous_move(pLogonInfo->ip, 
				pLogonInfo->port, profileToken, speed_x, speed_y);
			break;
		}
		case PTZ_CMD_LEFT_DOWN:	//×óÏÂ
		{
			speed_x = (-1.0)*u8Speed/128;
			speed_y = (1.0)*u8Speed/128;
			ret = nvr_onvif_ptz_continuous_move(pLogonInfo->ip, 
				pLogonInfo->port, profileToken, speed_x, speed_y);
			break;
		}
		case PTZ_CMD_RIGHT_UP:	//ÓÒÉÏ
		{
			speed_x = (1.0)*u8Speed/128;
			speed_y = (-1.0)*u8Speed/128;
			ret = nvr_onvif_ptz_continuous_move(pLogonInfo->ip, 
				pLogonInfo->port, profileToken, speed_x, speed_y);
			break;
		}
		case PTZ_CMD_RIGHT_DOWN:	//ÓÒÏÂ
		{
			speed_x = (1.0)*u8Speed/128;
			speed_y = (1.0)*u8Speed/128;
			ret = nvr_onvif_ptz_continuous_move(pLogonInfo->ip, 
				pLogonInfo->port, profileToken, speed_x, speed_y);
			break;
		}
		case PTZ_CMD_SET_PRESET:	// ÉèÖÃÔ¤ÖÃµã
		{
			char acPresetName[32];			
			printf("===set_preset===\r\n");
			sprintf(acPresetName, "PresetName_%d", pPtzCtrl->struPreset.u8PresetNo);
			ret = nvr_onvif_set_presets(pLogonInfo->ip, 
				pLogonInfo->port, profileToken, acPresetName);
			if (ret < 0)
			{				
				printf("set preset failed\r\n");
			}
			break;
		}
		case PTZ_CMD_GOTO_PRESET:	// µ÷ÓÃÔ¤ÖÃµã
		{
			printf("===goto_preset===\r\n");
			char *PresetToken;
			ret = nvr_onvif_get_presets(pLogonInfo->ip, pLogonInfo->port, profileToken, &pLogonInfo->presets);
			if (ret < 0) 
			{				
				printf("get preset failed\r\n");
				return -1;
			}
			PresetToken = onvif_get_preset_token(&pLogonInfo->presets, pPtzCtrl->struPreset.u8PresetNo);
			if (PresetToken == NULL)
				return -1;
			printf("get preset token %s\r\n", PresetToken);
			ret = nvr_onvif_goto_presets(pLogonInfo->ip, 
				pLogonInfo->port, profileToken, PresetToken);
			if (ret < 0)
			{
				printf("goto preset failed\r\n");
			}
			break;
		}
		case PTZ_CMD_CLR_PRESET:	// É¾³ýÔ¤ÖÃµã
		{
			break;
		}
		case PTZ_CMD_ZOOM_TELE:	// ½¹¾à·Å´ó
		{
			break;
		}
		case PTZ_CMD_ZOOM_WIDE:	// ½¹¾àËõÐ¡
		{
			break;
		}
		case PTZ_CMD_FOCUS_FAR:	// Ô¶½¹µã
		{
			break;
		}
		case PTZ_CMD_FOCUS_NEAR:	// ½ü½¹µã
		{
			break;
		}
		case PTZ_CMD_SET_CRUISE:	//ÉèÖÃÑ²º½
		{
			break;
		}
		case PTZ_CMD_SET_CRUISE_START:	//¿ªÊ¼Ñ²º½
		{
			break;
		}
		case PTZ_CMD_STOP_CRUISE:	//Í£Ö¹Ñ²º½
		{
			break;
		}
		case PTZ_CMD_DEL_CRUISE:	//É¾³ýÑ²º½
		{
			break;
		}
		default:
			break;
	}

	return ret;
}

