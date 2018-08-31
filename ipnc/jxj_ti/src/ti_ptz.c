#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <error.h>
#include <linux/sockios.h>
#include <linux/if.h>  
#include <time.h>
#include <malloc.h>
#include <sys/unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>

#include "ti_net_struct.h"
#include "ti_ptz.h"
#include "net_api.h"
#include "ti_net_interface.h"
#include "ti_ptz_cmd.h"
#include "ti_net_common.h"
#include "ti_net_search.h"
#include "ti_net_preview.h"


/*发送云控制台控制命令*/
int ti_send_ptz_cmd(long handle, void* pPtz)
{	
	if(pPtz== NULL)
		return TI_FAILURE;

	if(0 > ti_send_set_param_cmd(handle, pPtz, sizeof(TI_NET_CTRL_PTZ_S), TI_CMD_CTRL_PTZ))
	{
		__ERR("parameter set is Failed !\r\n");
		return TI_FAILURE;
	}
	return TI_SUCCESS;
}
/* 设置位置方向 */
int Ti_Client_Crtl_Ptz(long handle, TI_U16 u16PtzCmd, TI_U8 u8Chn, TI_U8 u8Speed, TI_U8 u8PresetNo)
{
	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;

	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = (TI_U8)u16PtzCmd;
	ptz.struPreset.u8Chn = (TI_U8)u8Chn;
	ptz.struPreset.u8PresetNo = (TI_U8)u8PresetNo;
	ptz.struPreset.u8Speed = (TI_U8)u8Speed;

	return ti_send_ptz_cmd(handle, &ptz);

}
/* 设置自动巡航 */
int ti_client_Ctrl_Ptz_Cruise(long handle, TI_U8 u8Chn, TI_U8 u8CruisePresetNo, TI_BOOL u8CruisePresetEnable, 
		TI_U8 u8CruisePresetSec, TI_U8 u8CruisePresetSpeed)
{
	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = PTZ_CMD_SET_CRUISE;
	ptz.struCruise.u8CruisePresetEnable = (TI_U8)u8CruisePresetEnable;
	ptz.struCruise.u8CruisePresetNo = (TI_U8)u8CruisePresetNo;
	ptz.struCruise.u8CruisePresetSec = (TI_U8)u8CruisePresetSec;
	ptz.struCruise.u8CruisePresetSpeed = (TI_U8)u8CruisePresetSpeed;

	return ti_send_ptz_cmd(handle, &ptz);

}

/* 设置自动扫描 */
int Ti_Client_Ctrl_Ptz_AutoScan(long handle, TI_U8 u8Chn, TI_U8 u8SetVerticalPos, TI_U8 u8Speed)
{

	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = PTZ_CMD_SET_AUTOSCAN_SETTING;
	ptz.struAutoScan.u8SetVerticalPos = (TI_U8)u8SetVerticalPos;
	ptz.struAutoScan.u8Speed = (TI_U8)u8Speed;

	return ti_send_ptz_cmd(handle, &ptz);

}

/* 设置AB两点扫描 */
int Ti_Client_Ctrl_Ptz_ABScan(long handle, TI_U8 u8Chn, TI_U8 u8SetVerticalPos, TI_U8 u8Speed)
{

	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = PTZ_CMD_SET_ABSCAN_SETTING;
	ptz.struABScan.u8SetVerticalPos = (TI_U8)u8SetVerticalPos;
	ptz.struABScan.u8Speed = (TI_U8)u8Speed;

	return ti_send_ptz_cmd(handle, &ptz);

}
int Ti_Client_Ctrl_Ptz_ABScan_Left_Pos(long handle, TI_U8 u8Chn)
{

	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = PTZ_CMD_SET_ABSCAN_LEFT_POS;

	return ti_send_ptz_cmd(handle,&ptz);	
}
int Ti_Client_Ctrl_Ptz_ABScan_Right_Pos(long handle, TI_U8 u8Chn)
{

	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = PTZ_CMD_SET_ABSCAN_RIGHT_POS;

	return ti_send_ptz_cmd(handle,&ptz);
}

/* 设置闲置动作 */
int Ti_Client_Ctrl_Ptz_SetIdle(long handle, TI_U8 u8Chn, TI_U8 u8IdleActionIndex, TI_U8 u8IdleSec)
{

	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = PTZ_CMD_SET_IDLE_SETTING;
	ptz.struIdleAction.u8IdleActionIndex = (TI_U8)u8IdleActionIndex;
	ptz.struIdleAction.u8IdleSec = (TI_U8)u8IdleSec;

	return ti_send_ptz_cmd(handle, &ptz);

}

/* 设置定时动作 */
int Ti_Client_Ctrl_Ptz_SetOntime(long handle,TI_U8 u8Chn, TI_U8 u8OntimeIndex, TI_U8 u8OntimeActionIndex,
								TI_U8 u8BeginHour, TI_U8 u8BeginMin, TI_U8 u8EndHour, TI_U8 u8EndMin)
{
	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = PTZ_CMD_SET_ONTIME_SETTING;
	ptz.struOntimeAction.u8OntimeIndex = (TI_U8)u8OntimeIndex;
	ptz.struOntimeAction.u8OntimeActionIndex = (TI_U8)u8OntimeActionIndex;
	ptz.struOntimeAction.u8BeginHour = (TI_U8)u8BeginHour;
	ptz.struOntimeAction.u8BeginMin = (TI_U8)u8BeginMin;
	ptz.struOntimeAction.u8EndHour = (TI_U8)u8EndHour;
	ptz.struOntimeAction.u8EndMin = (TI_U8)u8EndMin;
	return ti_send_ptz_cmd(handle,&ptz);
}


/* 设置报警动作 */
int Ti_Client_Ctrl_Ptz_SetAlarm(long handle ,TI_U8 u8Chn, TI_U8 u8AlarmTypeIndex, TI_U8 u8AlarmActionIndex,
									TI_U8 u8AlarmDurationSec)
{
		TI_NET_CTRL_PTZ_S ptz;
		
		if (u8Chn >= TI_MAX_SUPPORT_CHN)
			return TI_FAILURE;
		memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	
		
		ptz.u16PtzCmd = PTZ_CMD_SET_ALARM_SETTING;
		ptz.struAlarm.u8AlarmTypeIndex = (TI_U8)u8AlarmTypeIndex;
		ptz.struAlarm.u8AlarmActionIndex = (TI_U8)u8AlarmActionIndex;
		ptz.struAlarm.u8AlarmDurationSec =(TI_U8)u8AlarmDurationSec;
		
		return ti_send_ptz_cmd(handle,&ptz);

}

/* 红外灯远近开关*/ 
/*u8IRMode: 0:远近灯自动切换,1:远近灯同时开启 */
int Ti_Client_Ctrl_Ptz_SetIRMode(long handle, TI_U8 u8Chn, TI_U8 u8IRMode)
{
	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = PTZ_CMD_SET_IR_MODE;
	ptz.u8IRMode = (TI_U8)u8IRMode;

	return ti_send_ptz_cmd(handle, &ptz);

}
/*  设置日夜模式 */
/* 0:白天模式,1:夜晚模式,2:自动模式 3:红外定时开启模式 */
int Ti_Client_Ctrl_Ptz_SetDayNightMode(long handle, TI_U8 u8Chn, TI_U8 u8DayNightMode)
{
	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = PTZ_CMD_SET_DAY_NIGHT_MODE;
	ptz.u8DayNightMode = (TI_U8)u8DayNightMode;

	return ti_send_ptz_cmd(handle, &ptz);

}

/*  设置变倍速度 */
/*  0 快速, 1 中速, 2 低速*/
int Ti_Client_Ctrl_Ptz_SetZoomSpeed(long handle, TI_U8 u8Chn, TI_U8 u8ZoomSpeed)
{

	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = (TI_U8)PTZ_CMD_ZOOM_SPEED;
	ptz.u8ZoomSpeed = (TI_U8)u8ZoomSpeed;

	return ti_send_ptz_cmd(handle, &ptz);

}

/*  设置白平衡工作模式 */
/* 0 室外,1 室内 ,2 自动 ,3 手动 */
int Ti_Client_Ctrl_Ptz_SetWBMode(long handle, TI_U8 u8Chn, TI_U8 u8WBMode)
{
	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = PTZ_CMD_SET_WB_MODE;
	ptz.u8WBMode = (TI_U8)u8WBMode;

	return ti_send_ptz_cmd(handle, &ptz);

}

/* 设置某些选项的使能
 * 包括:
 * 	180度自动翻转
 *	电子放大
 *	背光补偿
 *	变倍显示
 *	帧降噪
 *	宽动态
 *	移动侦测
 *	数字慢快门
 */
/*	0:close, 1:open */

int Ti_Client_Ctrl_Ptz_SetEnable(long handle, TI_U8 u8Chn, TI_U16 u8PtzCmd, TI_U8 u8Enable)
{
	TI_NET_CTRL_PTZ_S ptz;

	if (u8Chn >= TI_MAX_SUPPORT_CHN)
		return TI_FAILURE;
	memset(&ptz, 0, sizeof(TI_NET_CTRL_PTZ_S));	

	ptz.u16PtzCmd = u8PtzCmd;
	ptz.u8OpenOrClose = (TI_U8)u8Enable;

	return ti_send_ptz_cmd(handle, &ptz);

}






