
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "ti_type.h"
#include "ti_struct.h"
#include "ti_net_logon.h"
#include "ti_ipnc_mng.h"
#include "ti_net_common.h"
#include "ti_net_interface.h"
#include "ti_ptz.h"
#include "ti_param_proc.h"

int ti_ipnc_logon(long handle, long *logonHandle)
{
	TI_IPNC_PROTOCOL_MNG *ipncHandle = (TI_IPNC_PROTOCOL_MNG *)handle;
	TI_LOGON_USER_INFO * pUserInfo = NULL;
	
	if(NULL == ipncHandle)
		return -1;

	char port[10] = {0};
	sprintf(port, "%d", ipncHandle->port);
	
	*logonHandle = ti_net_logon(ipncHandle->ipAddr, ipncHandle->user, 
								ipncHandle->passwd, port, 20, 
								NULL, &ipncHandle->poolCh);

	if((long)NULL == *logonHandle)
		return -1;

	pUserInfo = (TI_LOGON_USER_INFO *)(*logonHandle);
	return pUserInfo->logonSock;
}

int ti_ipnc_logout(long logonHandle)
{
	if(((long)NULL == logonHandle) || (0 == logonHandle))
		return -1;
	
	return ti_net_logout(logonHandle);
}

int ti_ipnc_get_param(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize)
{
	TI_NET_CMD_PARAM_S* pCmdParam = NULL;
	
	//__DBUG("ti_ipnc_get_param...\n");
	if(NULL == pParam)
		return -1;
	if(((long)NULL == logonHandle) || (0 == logonHandle))
		return -1;
	
	pCmdParam = (TI_NET_CMD_PARAM_S*)ti_net_param_cmd(cmd);
	//__DBUG("dataSize:%d\n", dataSize);
	
	if(NULL != pCmdParam)
	{
		//if(!pCmdParam->checkParamProc(logonHandle, 0, pParam, dataSize))
		return pCmdParam->getParamProc(logonHandle, 0, pParam, dataSize);
	}
	else		
		__ERR("pCmdParam is null!\n");

	__ERR("pParam check is error!\n");
	return -1;
}
int ti_ipnc_set_param(long logonHandle, IPNC_CMD_TYPE_E cmd, void *pParam, int dataSize)
{
	TI_NET_CMD_PARAM_S* pCmdParam = NULL;

	//__DBUG("ti_ipnc_set_param...\n");
	//if(NULL == pParam)
	//	return -1;

	if(((long)NULL == logonHandle) || (0 == logonHandle))
		return -1;
	
	pCmdParam = (TI_NET_CMD_PARAM_S*)ti_net_param_cmd(cmd);
	//__DBUG("dataSize:%d\n", dataSize);
	if(NULL != pCmdParam)
	{
		if(!pCmdParam->checkParamProc(logonHandle, 0, pParam, dataSize))
			return pCmdParam->setParamProc(logonHandle, 0, pParam, dataSize);
	}
	else		
		__ERR("pCmdParam is null!\n");

	__ERR("pParam check is error!\n");
	return -1;
}

int ti_ipnc_ptz_ctrl(long logonHandle, void *pztPacket)
{
	TI_LOGON_USER_INFO * pUserInfo = (TI_LOGON_USER_INFO *)logonHandle;
	char sendBuf[2048];
	char recvBuf[2048];
	
	if(((long)NULL == logonHandle) || (0 == logonHandle))
		return TI_FAILURE;
		
	net_set_net_hdr((TI_NET_HDR_S *)sendBuf, sizeof(TI_NET_CTRL_PTZ_S), TI_CMD_CTRL_PTZ, 0, 0);
	memcpy(sendBuf + sizeof(TI_NET_HDR_S), pztPacket, sizeof(TI_NET_CTRL_PTZ_S));
	if(ti_logon_pipe_session(pUserInfo, sendBuf, sizeof(TI_NET_HDR_S)+sizeof(TI_NET_CTRL_PTZ_S),
								recvBuf, sizeof(recvBuf)) < 0)
	{
		__ERR("Failed to send ptz packet to the ipc client. sock:%d\n", pUserInfo->logonSock);
		return TI_FAILURE;
	}

	return TI_SUCCESS;
}

