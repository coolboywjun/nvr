#ifndef _TI_NET_PREVIEW_H
#define _TI_NET_PREVIEW_H

#include "ti_net_struct.h"

typedef void(*fBeginReadCb)(long lReadHandle, void *pUserData);
typedef void(*fEndReadCb)(long lReadHandle, void *pUserData);
typedef int(*fRealDataCb)(long lRealHandle, int dwDataType, const unsigned char *pBuffer, int dwBufSize, void *pUserData);


long TI_NET_Start_RealPlay(const char 	*pSzDevIP, int dwDevPort, const char *pUserName, const char *pPwd, 
									int dwChn, int dwStream,
									fRealDataCb cbFuncReadData, fBeginReadCb cbFuncBeginRead, fEndReadCb cbFuncEndRead,
									void *pUserData, long *plRealHandle);

int TI_NET_Stop_RealPlay(long lRealHandle);

int TI_NET_Request_IFrame(long lRealHandle);


/*test*/
void test_stream(long lRealHandle, int dwDataType, const unsigned char *pBuffer, int dwBufSize, void *pUserData);

#endif

