#ifndef _NVR_NET_SEARCH_H
#define _NVR_NET_SEARCH_H

#include <ipnc_lib_client.h>
#include <nvr_server_headers.h>

//获得支持协议表
const IPNC_PROTOCOL_S *nvr_get_protcol_table(void);
int nvr_get_protcol_table_num(void);
int  nvr_search_start(char* pProtocolName, long* plHandle);
void nvr_search_stop(long lHandle);
int  nvr_search_get(long lHandle, IPNC_NET_SEARCH_RES_S *pDevReq);
int nvr_search_test_ipc_status(long lHandle, char *pIpAddr, int port,
							   const char* pUsr, const char* pPwd);
int  nvr_search_cfg(long lHandle, IPNC_NET_SEARCH_CFG_REQ_S *pSearchCfg);

//初始化函数
int nvr_search_update_channel(int port, NVR_CHANNEL_CFG_S *pChannelCfgInfo);
int  nvr_search_init(void);
void nvr_search_uninit(void);


#endif

