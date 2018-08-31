#ifndef _NVR_CLIENT_H
#define _NVR_CLIENT_H

/*###############################################  API  ############################################### */


/* 设置位置方向 */
int Nvr_Client_Crtl_Ptz(uint16_t u16PtzCmd, uint8_t u8Chn, uint8_t u8Speed, uint8_t u8PresetNo);

/* 设置自动巡航 */
int Nvr_client_Ctrl_Ptz_Cruise(uint8_t u8Chn, uint8_t u8CruisePresetNo, NVR_BOOL u8CruisePresetEnable, 
								uint8_t u8CruisePresetSec, uint8_t u8CruisePresetSpeed);
/* 设置自动扫描 */
int Nvr_Client_Ctrl_Ptz_AutoScan(uint8_t u8Chn, uint8_t u8SetVerticalPos, uint8_t u8Speed);

/* 设置AB两点扫描 */
int Nvr_Client_Ctrl_Ptz_ABScan(uint8_t u8Chn, uint8_t u8SetVerticalPos, uint8_t u8Speed);
int Nvr_Client_Ctrl_Ptz_ABScan_Left_Pos(uint8_t u8Chn);
int Nvr_Client_Ctrl_Ptz_ABScan_Right_Pos(uint8_t u8Chn);

/* 设置闲置动作 */
int Nvr_Client_Ctrl_Ptz_SetIdle(uint8_t u8Chn, uint8_t u8IdleActionIndex, uint8_t u8IdleSec);

/* 设置定时动作 */
int Nvr_Client_Ctrl_Ptz_SetOntime(uint8_t u8Chn, uint8_t u8OntimeIndex, uint8_t u8OntimeActionIndex,
								uint8_t u8BeginHour, uint8_t u8BeginMin, uint8_t u8EndHour, uint8_t u8EndMin);

/* 设置报警动作 */
int Nvr_Client_Ctrl_Ptz_SetAlarm(uint8_t u8Chn, uint8_t u8AlarmTypeIndex, uint8_t u8AlarmActionIndex,
									uint8_t u8AlarmDurationSec);
/* 红外灯远近开关*/ 
/*u8IRMode: 0:远近灯自动切换,1:远近灯同时开启 */
int Nvr_Client_Ctrl_Ptz_SetIRMode(uint8_t u8Chn, uint8_t u8IRMode);

/*  设置日夜模式 */
/* 0:白天模式,1:夜晚模式,2:自动模式 3:红外定时开启模式 */
int Nvr_Client_Ctrl_Ptz_SetDayNightMode(uint8_t u8Chn, uint8_t u8DayNightMode);

/*  设置变倍速度 */
/*  0 快速, 1 中速, 2 低速*/
int Nvr_Client_Ctrl_Ptz_SetZoomSpeed(uint8_t u8Chn, uint8_t u8ZoomSpeed);

/*  设置白平衡工作模式 */
/* 0 室外,1 室内 ,2 自动 ,3 手动 */
int Nvr_Client_Ctrl_Ptz_SetWBMode(uint8_t u8Chn, uint8_t u8WBMode);

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
int Nvr_Client_Ctrl_Ptz_SetEnable(uint8_t u8Chn, uint16_t u8PtzCmd, uint8_t u8Enable);

/* 云台命令发送 */ 
int nvr_send_ptz_cmd(int chnNum, void* pPtz); 


/*###############################################  API  ############################################### */

#endif

