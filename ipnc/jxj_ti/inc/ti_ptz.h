#ifndef _TI_CLIENT_H
#define _TI_CLIENT_H

#include "ti_param_proc.h"
#include "ti_net_interface.h"
#include "ti_type.h"



/*###############################################  API  ############################################### */


/* 设置位置方向 */
int Ti_Client_Crtl_Ptz(long handle, TI_U16 u16PtzCmd, TI_U8 u8Chn, TI_U8 u8Speed, TI_U8 u8PresetNo);

/* 设置自动巡航 */
int ti_client_Ctrl_Ptz_Cruise(long handle, TI_U8 u8Chn, TI_U8 u8CruisePresetNo, TI_BOOL u8CruisePresetEnable, 
								TI_U8 u8CruisePresetSec, TI_U8 u8CruisePresetSpeed);
/* 设置自动扫描 */
int Ti_Client_Ctrl_Ptz_AutoScan(long handle, TI_U8 u8Chn, TI_U8 u8SetVerticalPos, TI_U8 u8Speed);

/* 设置AB两点扫描 */
int Ti_Client_Ctrl_Ptz_ABScan(long handle, TI_U8 u8Chn, TI_U8 u8SetVerticalPos, TI_U8 u8Speed);
int Ti_Client_Ctrl_Ptz_ABScan_Left_Pos(long handle, TI_U8 u8Chn);
int Ti_Client_Ctrl_Ptz_ABScan_Right_Pos(long handle, TI_U8 u8Chn);

/* 设置闲置动作 */
int Ti_Client_Ctrl_Ptz_SetIdle(long handle, TI_U8 u8Chn, TI_U8 u8IdleActionIndex, TI_U8 u8IdleSec);

/* 设置定时动作 */
int Ti_Client_Ctrl_Ptz_SetOntime(long handle,TI_U8 u8Chn, TI_U8 u8OntimeIndex, TI_U8 u8OntimeActionIndex,
								TI_U8 u8BeginHour, TI_U8 u8BeginMin, TI_U8 u8EndHour, TI_U8 u8EndMin);

/* 设置报警动作 */
int Ti_Client_Ctrl_Ptz_SetAlarm(long handle ,TI_U8 u8Chn, TI_U8 u8AlarmTypeIndex, TI_U8 u8AlarmActionIndex,
									TI_U8 u8AlarmDurationSec);
/* 红外灯远近开关*/ 
/*u8IRMode: 0:远近灯自动切换,1:远近灯同时开启 */
int Ti_Client_Ctrl_Ptz_SetIRMode(long handle, TI_U8 u8Chn, TI_U8 u8IRMode);

/*  设置日夜模式 */
/* 0:白天模式,1:夜晚模式,2:自动模式 3:红外定时开启模式 */
int Ti_Client_Ctrl_Ptz_SetDayNightMode(long handle, TI_U8 u8Chn, TI_U8 u8DayNightMode);

/*  设置变倍速度 */
/*  0 快速, 1 中速, 2 低速*/
int Ti_Client_Ctrl_Ptz_SetZoomSpeed(long handle, TI_U8 u8Chn, TI_U8 u8ZoomSpeed);

/*  设置白平衡工作模式 */
/* 0 室外,1 室内 ,2 自动 ,3 手动 */
int Ti_Client_Ctrl_Ptz_SetWBMode(long handle, TI_U8 u8Chn, TI_U8 u8WBMode);

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
int Ti_Client_Ctrl_Ptz_SetEnable(long handle, TI_U8 u8Chn, TI_U16 u8PtzCmd, TI_U8 u8Enable);

/* 云台命令发送 */ 
int ti_send_ptz_cmd(long handle, void* pPtz); 


/*###############################################  API  ############################################### */

#endif

