
#ifndef __JXJ_CU_NET_PARAM_H__
#define __JXJ_CU_NET_PARAM_H__

#include <ipnc_lib.h>

typedef int (*funcGetParam)(long logonHandle, int nPort, void *pData, uint32_t dataSize);
typedef int (*funcSetParam)(long logonHandle, int nPort, void *pData, uint32_t dataSize);
typedef int (*funcCheckParam)(long logonHandle, int nPort, void *pData, uint32_t dataSize);

typedef struct _JXJ_CU_NET_CMD_PARAM_S_
{
	IPNC_CMD_TYPE_E paramCmd;
	int				s32MaxSuppPort;	
	funcGetParam	getParamProc;
	funcSetParam	setParamProc;
	funcCheckParam	checkParamProc;		
}JXJ_CU_NET_CMD_PARAM_S;


long jxj_cu_net_param_cmd(IPNC_CMD_TYPE_E paramCmd);


#endif





