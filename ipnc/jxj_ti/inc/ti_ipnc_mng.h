
#ifndef __TI_IPNC_MNG_H__
#define __TI_IPNC_MNG_H__

#include <ipnc_lib.h>

#include "ti_ipnc_realplay.h"

typedef struct _TI_IPNC_PROTOCOL_MNG_
{
	int sock;
	
	char ipAddr[16];
	int port;
	char user[32];
	char passwd[32];

	int status;
	int isLogon;
	
	int refCount;

	void *stream[2];	// 主码流, 次码流

	int poolCh;			//通道号

	struct _TI_IPNC_PROTOCOL_MNG_ *next;
}TI_IPNC_PROTOCOL_MNG;


void ti_ipnc_mng_print_list();

int ti_ipnc_mng_check_handle_valid(TI_IPNC_PROTOCOL_MNG *pMng);

int ti_ipnc_mng_check_ipnc_handle_valid(TI_IPNC_PROTOCOL_MNG *pMng, TI_IPNC_STREAM_S *pStream);

void ti_ipnc_mng_ref(TI_IPNC_PROTOCOL_MNG *pMng);

void ti_ipnc_mng_unref(TI_IPNC_PROTOCOL_MNG *pMng);

int ti_ipnc_connect(int poolCh, const char *ipAddr, int port, const char *user, const char *passwd, long *ipncHandle);

int ti_ipnc_disconnect(long handle);

int ti_ipnc_search_cfg(IPNC_NET_SEARCH_CFG_REQ_S* pIpncSearchCfg);

int ti_ipnc_start_search(long lHandle);

int ti_ipnc_stop_search(long lHandle);

int ti_ipnc_get_search(long lHandle, IPNC_NET_SEARCH_RES_S *pDev);

int ti_ipnc_test_ipc(const char* DesIP, const char* User, 
					 const char* Password, const char* port);


#endif

