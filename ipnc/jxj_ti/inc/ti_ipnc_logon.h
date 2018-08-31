
#ifndef __TI_IPNC_LOGON_H__
#define __TI_IPNC_LOGON_H__

#include <ipnc_lib.h>

int ti_ipnc_logon(long handle, long *logonHandle);

int ti_ipnc_logout(long logonHandle);


int ti_ipnc_get_param(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize);
int ti_ipnc_set_param(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize);

int ti_ipnc_ptz_ctrl(long logonHandle, void *pztPacket);

#endif


