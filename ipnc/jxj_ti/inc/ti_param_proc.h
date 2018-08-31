#ifndef _TI_PARAM_PROC_H
#define _TI_PARAM_PROC_H

#include "ti_type.h"
#include <ipnc_lib.h>

typedef int (*funcGetParam)(long logonHandle, int nPort, void *pData, uint32_t dataSize);
typedef int (*funcSetParam)(long logonHandle, int nPort, void *pData, uint32_t dataSize);
typedef int (*funcCheckParam)(long logonHandle, int nPort, void *pData, uint32_t dataSize);

typedef struct _TI_NET_CMD_PARAM_S_
{
	IPNC_CMD_TYPE_E paramCmd;
	TI_S32			s32MaxSuppPort;	
	funcGetParam	getParamProc;
	funcSetParam	setParamProc;
	funcCheckParam	checkParamProc;		
}TI_NET_CMD_PARAM_S;

/* 获取设备参数 */
/* pParam 传入参数结构体 */
/* u32PackLen 参数结构体长度*/
/* 获取参数命令 */
int ti_send_get_param_cmd(long handle, void* pParam, TI_U32 u32PackLen, TI_U32 u32PackCmd);

/*发送设置参数命令*/
int ti_send_set_param_cmd(long handle, void* pParam, TI_U32 u32PackLen, TI_U32 u32PackCmd);

/*根据参数命令,查找响应结构操作函数集.
成功返回操作函数集句柄,
失败返回空.
*/
long ti_net_param_cmd(IPNC_CMD_TYPE_E paramCmd);

//test
int ti_param_get_net(long handle);
int ti_param_get_net_ball(long handle);

#endif

