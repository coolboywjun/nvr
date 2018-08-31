#ifndef __IPNC_PROTOCOL_SERVER_H__
#define __IPNC_PROTOCOL_SERVER_H__
#include <ipnc_lib.h>

#ifdef __cplusplus
extern "C" {
#endif


#define JXJ_TI_PROTOCOL_NAME	"jxj_ti"
#define JXJ_CU_PROTOCOL_NAME	"jxj_cu"
#define JXJ_ONVIF_PROTOCOL_NAME "onvif_cu"

typedef int (*ipnc_connect)(int poolCh, const char *ipAddr, int port, const char *user, const char *passwd, long *ipncHandle);
typedef int (*ipnc_disconnect)(long handle);

typedef int (*ipnc_search_cfg)(IPNC_NET_SEARCH_CFG_REQ_S* pIpncSearchCfg);
typedef int (*ipnc_start_search)(long lHandle);
typedef int (*ipnc_stop_search)(long lHandle);
//typedef int (*ipnc_get_search)(IpncSearch *search, int cnt, int *searchCnt);
typedef int (*ipnc_get_search)(long lHandle, IPNC_NET_SEARCH_RES_S *pDev);

typedef int (*ipnc_logon)(long handle, long *logonHandle);
typedef int (*ipnc_logout)(long logonHandle);

typedef int (*ipnc_start_get_stream)(long handle, int chn, int streamNo, long *streamHandle);
typedef int (*ipnc_stop_get_stream)(long handle, long streamHandle);
typedef int (*ipnc_request_iframe)(long handle, long streamHandle);

typedef int (*ipnc_get_param)(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize);
typedef int (*ipnc_set_param)(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize);

typedef int (*ipnc_ptz_ctrl)(long logonHandle, /*IPNC_NET_CTRL_PTZ_S*/void *pztPacket);

typedef int (*ipnc_test_ipc_status)(const char* DesIP, const char* User, 
					 				const char* Password, const char* port);

typedef struct
{
	ipnc_connect connectCb;
	ipnc_disconnect disconnectCb;

	ipnc_search_cfg	 searchCfgCb;
	ipnc_start_search startSearchCb;
	ipnc_stop_search stopSearchCb;
	ipnc_get_search getSearchCb;

	ipnc_logon logonCb;
	ipnc_logout logoutCb;
	
	ipnc_start_get_stream startGetStreamCb;
	ipnc_stop_get_stream stopGetStreamCb;
	ipnc_request_iframe requestIFrameCb;

	ipnc_get_param getParamCb;
	ipnc_set_param setParamCb;

	ipnc_ptz_ctrl  ptzControl;

	ipnc_test_ipc_status testIpcStatus;
	
}IpncProtocolCbTable;

int ipnc_server_init(const char *protocolName, IpncProtocolCbTable *cbTable);

void ipnc_server_uninit();

#ifdef __cplusplus
}
#endif

#endif

