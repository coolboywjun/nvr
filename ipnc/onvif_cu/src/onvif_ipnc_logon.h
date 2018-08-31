
#ifndef __ONVIF_IPNC_LOGON_H__
#define __ONVIF_IPNC_LOGON_H__

#include <ipnc_lib.h>

typedef struct _onvif_logonInfo
{
	char ip[32];
	char port[8];
	onvif_profile_list_t profile;
	onvif_ptzpresets_list_t presets;
}onvif_logonInfo_t;


int onvif_ipnc_logon(long handle, long *logonHandle);

int onvif_ipnc_logout(long logonHandle);


int onvif_ipnc_get_param(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize);
int onvif_ipnc_set_param(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize);

int onvif_ipnc_ptz_ctrl(long logonHandle, void *pztPacket);

#endif


