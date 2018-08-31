
#include <nvr_headers.h>
#include <nvr_lib_common.h>
#include <nvr_lib_interface.h>
#include <ipnc_lib.h>

//返回不等0,代表成功
long nvr_lib_search_start(const char* pProtocolName)
{
	char sendBuf[2048] = {0};
	long lHandle = 0;
	int headSize = sizeof(NVR_NET_HDR_S);
	int dataSize = strlen(pProtocolName)+1;
	int cmd 	 = NVR_CMD_START_SEARCH;
	int sendLen	 = headSize + dataSize;
	
	nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, 0, cmd, dataSize, 0);
	memcpy(sendBuf + headSize, pProtocolName, dataSize);
	
	if( 0 > nvr_send_cmd(&lHandle, sizeof(lHandle), 
						  sendBuf, sendLen))
		return 0;

	if(0 != lHandle)	
		return lHandle;

	return 0;
}

int nvr_lib_search_get(long lHandle, IPNC_NET_SEARCH_RES_S *pRecvBuf)
{
	int ret = -1;
	char sendBuf[2048] = {0};
	int  headSize   = sizeof(NVR_NET_HDR_S);
	int  dataSize 	= sizeof(lHandle);
	int	 recvSize	= sizeof(IPNC_NET_SEARCH_RES_S);
	int  cmd		= NVR_CMD_GET_SEARCH;
	int  sendLen	= headSize + dataSize;

	if(0 == lHandle)
		return NVR_FAILURE;
	
	nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, 0, cmd, dataSize, 0);
	memcpy(sendBuf + headSize, &lHandle, dataSize);
	if(0 > (ret = nvr_send_cmd(pRecvBuf, recvSize, 
							   sendBuf, sendLen)))
		return NVR_FAILURE;
	
	return ret;

}

int nvr_lib_search_get_ipc_status(long lHandle, char* ipAddr, int ipSize)
{
	char sendBuf[2048] = {0};
	int  headSize   = sizeof(NVR_NET_HDR_S);
	int  dataSize 	= sizeof(lHandle)+ipSize;
	int  cmd		= NVR_CMD_TEST_IPC_STATUS;
	int  sendLen	= headSize + dataSize;
	int  ret = -1;

	if(0 == lHandle)
		return NVR_FAILURE;
	
	nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, 0, cmd, dataSize, 0);
	memcpy(sendBuf + headSize, &lHandle, sizeof(lHandle));
	memcpy(sendBuf + headSize + sizeof(lHandle), ipAddr, ipSize);
	if(0 > nvr_send_cmd(&ret, sizeof(ret), 
						 sendBuf, sendLen))
		return NVR_FAILURE;
	
	return ret;

}

int nvr_lib_search_req_cpy_cfg(IPNC_NET_SEARCH_CFG_REQ_S *pSearchCfg, IPNC_NET_SEARCH_RES_S *pDev)
{
	if(NULL == pDev || NULL == pSearchCfg)
		return -1;
	
	//序列号
	memcpy(pSearchCfg->u8SerialId, pDev->u8SerialId, sizeof(pSearchCfg->u8SerialId));
	//软件版本
	pSearchCfg->u32SoftVer = pDev->u32SoftVer;
	//设备类型
	pSearchCfg->u16DevType = pDev->u16DevType;
	//设备端口
	pSearchCfg->u16DevPort = pDev->u16DevPort;
	//mac地址
	memcpy(pSearchCfg->u8MacAddr, pDev->u8MacAddr, sizeof(pSearchCfg->u8MacAddr));
	//ip地址
	pSearchCfg->u32DevIp = pDev->u32DevIp;
	//掩码
	pSearchCfg->u32DevIpMask = pDev->u32DevIpMask;
	//网关
	pSearchCfg->u32DevGwAddr = pDev->u32DevGwAddr;
	//dns
	pSearchCfg->u32DevDns1 = pDev->u32DevDns1;
	pSearchCfg->u32DevDns2 = pDev->u32DevDns2;
	//DNS DHCP
	pSearchCfg->u8Dns = pDev->u8Dns;
	pSearchCfg->u8DHCP = pDev->u8DHCP;

	memcpy(pSearchCfg->szPriData, pDev->szPriData, sizeof(pSearchCfg->szPriData));
	
	return 0;
}

int nvr_lib_search_cfg(long lHandle, IPNC_NET_SEARCH_CFG_REQ_S *pSearchCfg, int size)
{
	char sendBuf[2048] = {0};
	int  headSize   = sizeof(NVR_NET_HDR_S);
	int  dataSize 	= sizeof(lHandle) + size;
	int  cmd		= NVR_CMD_SEARCH_CFG;
	int  sendLen	= headSize + dataSize;

	if(0 == lHandle)
		return NVR_FAILURE;
	
	memcpy(sendBuf + headSize, &lHandle, sizeof(lHandle));
	memcpy(sendBuf + headSize + sizeof(lHandle), pSearchCfg, size);
	
	nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, 0, cmd, dataSize, 0);
	if(0 > nvr_send_cmd(NULL, 0, sendBuf, sendLen))
		return NVR_FAILURE;
	
	return NVR_SUCCESS;

}


int nvr_lib_search_stop(long lHandle)
{
	char sendBuf[2048] = {0};
	int  headSize   = sizeof(NVR_NET_HDR_S);
	int  dataSize 	= sizeof(lHandle);
	int  cmd		= NVR_CMD_STOP_SEARCH;
	int  sendLen	= headSize + dataSize;

	if(0 == lHandle)
		return NVR_FAILURE;
	
	nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, 0, cmd, dataSize, 0);
	memcpy(sendBuf + headSize, &lHandle, dataSize);
	if(0 > nvr_send_cmd(NULL, 0, sendBuf, sendLen))
		return NVR_FAILURE;
	
	return NVR_SUCCESS;

}



