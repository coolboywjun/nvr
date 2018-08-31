#ifndef _ONVIF_IPNC_SEARCH_H
#define _ONVIF_IPNC_SEARCH_H

#include <ipnc_lib.h>

//int Ti_Net_Search_Cfg(IPNC_NET_SEARCH_CFG_REQ_S* pIpncSearchCfg);

int Onvif_Net_Search_Do(long lHandle);

int Onvif_Net_Search_Done(long lHandle);

int Onvif_Net_Get_Search(long lHandle, IPNC_NET_SEARCH_RES_S *pDev);

int Onvif_Net_Search_init();

int Onvif_Net_Search_uninit();

int Onvif_Net_Get_Search_DevNum();

#endif

