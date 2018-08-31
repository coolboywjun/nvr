
#ifndef __JXJ_CU_PTZ_H__
#define __JXJ_CU_PTZ_H__

#include "j_sdk.h"

//发送控制ptz命令码
int jxj_cu_ptz_set(long logonHandle, JPTZControl *cfg);

/* ptz控制 */
int jxj_cu_client_crtl_ptz(long logonHandle,  
						   JPTZAction action, /*PTZ控制命令码*/
						   uint8_t u8Speed);  /*速度*/
/* 设置自动巡航 */
int jxj_cu_client_ptz_cruise(long logonHandle, 
							 uint8_t u8CruisePresetNo, 	   /*预置号*/
							 uint8_t u8CruisePresetEnable, /*使能与否*/
							 uint8_t u8CruisePresetSec,    /*保留时间*/
							 uint8_t u8CruisePresetSpeed); /*速度*/

#endif


