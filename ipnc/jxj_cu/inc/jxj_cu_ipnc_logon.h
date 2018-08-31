
#ifndef __JXJ_CU_IPNC_H__
#define __JXJ_CU_IPNC_H__

#include "ipnc_lib.h"

int jxj_cu_ipnc_test_logon(const char* DesIP, const char* User, 
					 const char* Password, const char* port);

int jxj_cu_ipnc_logon(long handle, long *logonHandle);
int jxj_cu_ipnc_logout(long logonHandle);

int jxj_cu_ipnc_get_param(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize);
int jxj_cu_ipnc_set_param(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize);

int jxj_cu_ipnc_ptz_ctrl(long logonHandle, void *ptzPacket);


#endif




