#ifndef __IPNC_PROTOCOL_CLIENT_H__
#define __IPNC_PROTOCOL_CLIENT_H__

#include <ipnc_lib.h>

#ifdef __cplusplus
extern "C" {
#endif

const IPNC_PROTOCOL_S *ipnc_client_get_protocols();
int ipnc_client_get_protocols_num();

int ipnc_client_connect_ipnc(int handle, int poolCh, const char *ipAddr, int port, const char *user, const char *passwd, long *ipncHandle);
int ipnc_client_disconnect_ipnc(long handle, long ipncHandle);

int ipnc_client_logon_ipnc(int handle, long ipncHandle, long *logonHandle);
int ipnc_client_logout_ipnc(int handle,long logonHandle);

int ipnc_client_start_search(long handle);
int ipnc_client_stop_search(long handle);
int ipnc_client_get_search(long handle, IPNC_NET_SEARCH_RES_S *pDevReq);
int ipnc_client_set_search_cfg(long handle, IPNC_NET_SEARCH_CFG_REQ_S *pSearCfg);

int ipnc_client_test_alive(long handle, const char *ipAddr, int port, 
								 const char *user, const char *passwd);


int ipnc_client_start_get_stream(long handle, long ipncHandle, int chn, int streamNo, long *streamHandle);
int ipnc_client_stop_get_stream(long handle, long ipncHandle, long streamHandle);
int ipnc_client_request_iframe(long handle, long ipncHandle, long streamHandle);

int ipnc_client_connect_server(const char *protocolServerName, long *handle);
void ipnc_client_disconnect_server(long handle);

//获取参数
int ipnc_client_get_param(int handle, long logonHandle, IPNC_CMD_TYPE_E paramCmd, void* pParam, int dataSize);
//设置参数
int ipnc_client_set_param(int handle, long logonHandle, IPNC_CMD_TYPE_E paramCmd, void* pParam, int dataSize);

int ipnc_client_send_ptz_cmd(int handle, long logonHandle, /*IPNC_NET_CTRL_PTZ_S*/void *ptzPack, int dataSize);

#ifdef __cplusplus
}
#endif

#endif

