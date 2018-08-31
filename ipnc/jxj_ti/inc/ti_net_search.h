#ifndef _TI_NET_SEARCH_H
#define _TI_NET_SEARCH_H

#include "ipnc_lib.h"

int Ti_Net_Search_Cfg(IPNC_NET_SEARCH_CFG_REQ_S* pIpncSearchCfg);

int Ti_Net_Search_Do(long lHandle);

int Ti_Net_Search_Done(long lHandle);

int Ti_Net_Get_Search(long lHandle, IPNC_NET_SEARCH_RES_S *pDev);

int Ti_Net_Search_init();

int Ti_Net_Search_uninit();

int Ti_Net_Get_Search_DevNum();

#endif

