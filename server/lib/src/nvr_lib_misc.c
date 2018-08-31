
#include <nvr_headers.h>
#include <nvr_lib_common.h>
#include <nvr_lib_interface.h>


int g_nvrsock = 0;

int nvr_lib_init()
{
	int try_num;

	for(try_num = 0; try_num < 30; try_num++)
	{
		g_nvrsock = net_local_tcp_connect("/tmp/server");
		if(g_nvrsock < 0)
		{
			sleep(1);
			continue;
		}
		break;
	}
/*	
	if(g_nvrsock < 0)
	{	
		__ERR("connect to server failed.");
		return NVR_FAILURE;
	}
*/	
	return NVR_SUCCESS;
}

void nvr_lib_uninit()
{
	if(g_nvrsock > 0)
		net_close_socket(&g_nvrsock);
}

int nvr_restart()
{
	NVR_NET_HDR_S netHdr;
	int ret = -1;
	nvr_make_net_hdr(&netHdr, -1, NVR_CMD_RESTART_SERVER, 0, 0);
	if(0 > (ret = nvr_send_cmd(NULL, 0, &netHdr, sizeof(netHdr))))
	{
		__ERR("nvr restart failed :%d\n", ret);
		return NVR_FAILURE;
	}
	
	return NVR_SUCCESS;
}

int nvr_shutdown()
{
	NVR_NET_HDR_S netHdr;
	int ret = -1;

	nvr_make_net_hdr(&netHdr, -1, NVR_CMD_SHUTDOWN_SERVER, 0, 0);
	if(0 > (ret = nvr_send_cmd(NULL, 0, &netHdr, sizeof(netHdr))))
	{
		__ERR("nvr shutdown failed :%d\n", ret);
		return NVR_FAILURE;
	}

	return NVR_SUCCESS;
}

int nvr_sw_reset()
{
	NVR_NET_HDR_S netHdr;
	int ret = -1;

	nvr_make_net_hdr(&netHdr, -1, NVR_CMD_SW_RESET_SERVER, 0, 0);
	if(0 > (ret = nvr_send_cmd(NULL, 0, &netHdr, sizeof(netHdr))))
	{
		__ERR("nvr reset failed :%d\n", ret);
		return NVR_FAILURE;
	}

	return NVR_SUCCESS;
}

int nvr_set_hwgpio_check(int open)
{
	unsigned char szSendBuff[sizeof(NVR_NET_HDR_S) + 1];
	NVR_NET_HDR_S *pNetHdr = (NVR_NET_HDR_S*)szSendBuff;
	int ret = -1;

	nvr_make_net_hdr(pNetHdr, -1, NVR_CMD_SET_HWGPIO_CHECK, 1, 0);
	szSendBuff[sizeof(NVR_NET_HDR_S)] = open;
	if(0 > (ret = nvr_send_cmd(NULL, 0, szSendBuff, sizeof(szSendBuff))))
	{
		__ERR("nvr reset failed :%d\n", ret);
		return NVR_FAILURE;
	}

	return NVR_SUCCESS;
}

/*获得支持协议表:
	ipncProtocol: 存放协议的空间
	protocolNum	: 空间能存放的协议条数数澹(空间大小)
返回支持协议总条数.
*/
int nvr_lib_get_pro_table(IPNC_PROTOCOL_S* ipncProtocol, int protocolNum)
{
	char sendBuf[512] = {0};
	int  headSize   = sizeof(NVR_NET_HDR_S);
	int  dataSize 	= 0;
	int  cmd		= NVR_CMD_GET_PROT_TAB_INFO;
	int  sendLen	= headSize + dataSize;
	int  ret = 0;
	
	if(NULL == ipncProtocol || protocolNum <= 0)
		return NVR_FAILURE;
	
	nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, 0, cmd, dataSize, 0);
	if(0 > (ret = nvr_send_cmd((void *)ipncProtocol, sizeof(IPNC_PROTOCOL_S) * protocolNum, sendBuf, sendLen)))
		return NVR_FAILURE;
	
	return ret/sizeof(IPNC_PROTOCOL_S);

}

/*获得支持协议表数
 返回支持协议总条数.
*/
int nvr_lib_get_pro_table_num(void)
{
	char sendBuf[512] = {0};
	int  headSize   = sizeof(NVR_NET_HDR_S);
	int  dataSize 	= 0;
	int  cmd		= NVR_CMD_GET_PROT_TAB_NUM;
	int  sendLen	= headSize + dataSize;
	int  ret = 0;
	int  proNum = 0;

	nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, 0, cmd, dataSize, 0);
	if(0 > (ret = nvr_send_cmd(&proNum, sizeof(proNum), sendBuf, sendLen)))
		return NVR_FAILURE;
	
	return proNum;

}

int nvr_lib_start_manual_record(int chn)
{
	NVR_NET_HDR_S netHdr;
	int ret = -1;

	nvr_make_net_hdr(&netHdr, chn, NVR_CMD_START_MANUAL_REC, 0, 0);
	if(0 > (ret = nvr_send_cmd(NULL, 0, &netHdr, sizeof(netHdr))))
	{
		__ERR("Start manual record failed :%d\n", ret);
		return NVR_FAILURE;
	}

	return NVR_SUCCESS;
}

int nvr_lib_stop_manual_record(int chn)
{
	NVR_NET_HDR_S netHdr;
	int ret = -1;

	nvr_make_net_hdr(&netHdr, chn, NVR_CMD_STOP_MANUAL_REC, 0, 0);
	if(0 > (ret = nvr_send_cmd(NULL, 0, &netHdr, sizeof(netHdr))))
	{
		__ERR("Stop manual record failed :%d\n", ret);
		return NVR_FAILURE;
	}

	return NVR_SUCCESS;
}


