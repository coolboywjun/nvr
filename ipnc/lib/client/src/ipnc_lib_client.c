
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <errno.h>

#include <net_api.h>
#include <ipnc_lib.h>

#include "ipnc_lib_client.h"

//#define IPNC_LIB_CLIENT_DEBUG
#if defined(__DEBUG__) && defined(IPNC_LIB_CLIENT_DEBUG)
#define DBG(fmt, args...)		\
do{	\
printf(fmt, ##args);		\
syslog(LOG_DEBUG | LOG_USER, fmt, ## args);		\
}while(0)
#else
#define DBG(args...)
#endif

#define ERR(fmt, args...)	\
do{	\
printf(fmt, ##args);		\
syslog(LOG_ERR | LOG_USER, fmt, ## args);		\
}while(0)

#define LOG(priority, fmt, args...)	\
do{	\
printf(fmt, ##args);		\
syslog(priority | LOG_USER, fmt, ## args);		\
}while(0)

#define ARRAY_SIZE(array)	(sizeof(array)/sizeof(array[0]))

typedef struct
{
	int sock;
	
	char protocolServerName[32];
}IpncProtocolClient;

static const IPNC_PROTOCOL_S g_protocol_names[] = 
{
	{"jxj_ti", "v1", "JXJ", {{"admin", "admin"}}, 6080, 80, 554},
	{"jxj_cu", "v2", "JXJ", {{"admin", "admin"}}, 3321, 80, 7554},
	{"onvif_cu", "ONVIF", "ONVIF", {{"admin", "admin"}}, 80, 80, 554},
	{"\0", "\0", "\0", {{"\0"}, {"\0"}}}
};

#define IPNC_SUPPORT_PROT_NUM (ARRAY_SIZE(g_protocol_names)-1)//支持的协议数, 减去末尾'\0';

static int send_cmd_head(int sock, long ipncHandle, long logonHandle,
						 int cmd, IPNC_CMD_TYPE_E paramCmd, int response, int cmdDataLen)
{
	CmdHead cmdHdr;

	cmdHdr.magic = PROCOTOL_REQUEST_MAGIC;
	cmdHdr.ipncHandle = ipncHandle;
	cmdHdr.logonHandle = logonHandle;
	cmdHdr.cmd = cmd;
	cmdHdr.paramCmd = paramCmd;
	cmdHdr.response = response;
	cmdHdr.result = 0;
	cmdHdr.cmdDataLen = cmdDataLen;
#if 0	
	printf("======================================================\n");
	printf("%s: ipncHandle: 0x%08lx\n", __func__, cmdHdr.ipncHandle);
	printf("%s: logonHandle: 0x%08lx\n", __func__, cmdHdr.logonHandle);
	printf("%s: cmd: 0x%08lx\n", __func__, cmdHdr.cmd);
	printf("%s: paramCmd: 0x%08lx\n", __func__, cmdHdr.paramCmd);
	printf("%s: cmdDataLen: 0x%08lx\n", __func__, cmdHdr.cmdDataLen);
	printf("======================================================\n");
#endif
	return net_tcp_block_send(sock, &cmdHdr, sizeof(cmdHdr));
}

static int send_cmd_data(int sock, void *cmdData, int cmdDataLen)
{
	int ret = 0;
	if(cmdDataLen > 0 && cmdData != NULL)
		ret = net_tcp_block_send(sock, cmdData, cmdDataLen);
	return ret;
}

static int recv_cmd_response(int sock, CmdHead *cmdHdr, unsigned char *cmdData, int dataLen)
{
	if(net_tcp_block_recv(sock, cmdHdr, sizeof(CmdHead), sizeof(CmdHead)) < 0)
		return -1;
	if(cmdHdr->magic != PROTOCOL_RESPONSE_MAGIC)
	{
		ERR("The response magic is wrong.\n");
		return -1;
	}
	if((dataLen >= cmdHdr->cmdDataLen) && (cmdHdr->cmdDataLen > 0))
	{
		if(net_tcp_block_recv(sock, cmdData, dataLen, cmdHdr->cmdDataLen) < 0)
			return -1;
	}
	else
	{
		//接收多余包
		if(cmdHdr->cmdDataLen > 0)
		{
			char* tmpRecvBuf = (char*)malloc(cmdHdr->cmdDataLen);
			ERR("The cmdData is too small.\n");
			
			net_tcp_block_recv(sock, tmpRecvBuf, cmdHdr->cmdDataLen, cmdHdr->cmdDataLen);
			free(tmpRecvBuf);
			return -1;
		}
	}
	return cmdHdr->result;
}

static int recv_cmd_response_result(int sock)
{
	CmdHead cmdHdr;
	unsigned char szRecvBuff[RECV_BUFF_SIZE];
	if(recv_cmd_response(sock, &cmdHdr, szRecvBuff, sizeof(szRecvBuff)) < 0)
		return -1;
	return cmdHdr.result;
}
// 获取ipnc所有协议名
const IPNC_PROTOCOL_S *ipnc_client_get_protocols()
{
	return g_protocol_names;
}

// 获取ipnc所有协议总数
int ipnc_client_get_protocols_num()
{
	return IPNC_SUPPORT_PROT_NUM;
}

//连接ipnc
int ipnc_client_connect_ipnc(int handle, int poolCh, const char *ipAddr, int port, const char *user, const char *passwd, long *ipncHandle)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	CmdHead cmdHdr;
	unsigned char szRecvBuff[RECV_BUFF_SIZE];
	IpncConnect cmd;
	if(pClient == NULL)
		return -1;
	if(ipncHandle == NULL)
		return -1;
	strncpy(cmd.ipAddr, ipAddr, sizeof(cmd.ipAddr)-1);
	cmd.port = port;
	strncpy(cmd.user, user, sizeof(cmd.user)-1);
	strncpy(cmd.passwd, passwd, sizeof(cmd.passwd)-1);
	cmd.poolCh = poolCh;
	//printf("%s: poolCh: %d\n", __func__, poolCh);
	if(send_cmd_head(pClient->sock, 0, 0, IPNC_CONNECT, 0, 1, sizeof(cmd)) < 0)
		return -1;
	if(send_cmd_data(pClient->sock, &cmd, sizeof(cmd)) < 0)
		return -1;
	if(recv_cmd_response(pClient->sock, &cmdHdr, szRecvBuff, sizeof(szRecvBuff)) < 0)
		return -1;
	if(cmdHdr.result >= 0)
	{
		//printf("%s: ipncHandle: 0x%08lx\n", __func__, cmdHdr.ipncHandle);
		*ipncHandle = cmdHdr.ipncHandle;
	}
	return cmdHdr.result;
}
//退出ipnc
int ipnc_client_disconnect_ipnc(long handle, long ipncHandle)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	if(pClient == NULL)
		return -1;
	if(send_cmd_head(pClient->sock, ipncHandle, 0, IPNC_DISCONNECT, 0, 1, 0) < 0)
		return -1;
	return recv_cmd_response_result(pClient->sock);
}
//登录
int ipnc_client_logon_ipnc(int handle, long ipncHandle,long *logonHandle)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	CmdHead cmdHdr;

	if(pClient == NULL)
		return -1;
	if(logonHandle == NULL)
		return -1;

	if(send_cmd_head(pClient->sock, ipncHandle, 0, IPNC_LOGON, 0, 1, 0) < 0)
		return -1;
			
	if(recv_cmd_response(pClient->sock, &cmdHdr, NULL, 0) < 0)
		return -1;
		
	if(cmdHdr.result >= 0)
	{
		//printf("%s: logonHandle: 0x%08lx\n", __func__, cmdHdr.logonHandle);
		*logonHandle = cmdHdr.logonHandle;
	}
	return cmdHdr.result;
}
//退出登录
int ipnc_client_logout_ipnc(int handle, long logonHandle)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	CmdHead cmdHdr;
	
	if(pClient == NULL)
		return -1;

	if(send_cmd_head(pClient->sock, 0, logonHandle, IPNC_LOGOUT, 0, 1, 0) < 0)
		return -1;
			
	if(recv_cmd_response(pClient->sock, &cmdHdr, NULL, 0) < 0)
		return -1;
		
	return cmdHdr.result;
}
//获取参数
int ipnc_client_get_param(int handle, long logonHandle, IPNC_CMD_TYPE_E paramCmd, void* pParam, int dataSize)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	CmdHead cmdHdr;
	
	if(pClient == NULL)
		return -1;

	if(NULL == pParam)
		return -1;

	if(send_cmd_head(pClient->sock, 0, logonHandle, IPNC_GET_PARAM, paramCmd, 1, 0) < 0)
		return -1;
	
	if(recv_cmd_response(pClient->sock, &cmdHdr, pParam, dataSize) < 0)
		return -1;

	return cmdHdr.result;
}
//设置参数
int ipnc_client_set_param(int handle, long logonHandle, IPNC_CMD_TYPE_E paramCmd, void* pParam, int dataSize)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	CmdHead cmdHdr;
	
	if(pClient == NULL)
		return -1;

	if(send_cmd_head(pClient->sock, 0, logonHandle, IPNC_SET_PARAM, paramCmd, 1, dataSize) < 0)
		return -1;

	if(send_cmd_data(pClient->sock, pParam, dataSize) < 0)
		return -1;

	if(recv_cmd_response(pClient->sock, &cmdHdr, NULL, 0) < 0)
		return -1;

	return cmdHdr.result;
}

//发送PTZ命令
int ipnc_client_send_ptz_cmd(int handle, long logonHandle, void *ptzPack, int dataSize)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	CmdHead cmdHdr;
	
	if(pClient == NULL)
		return -1;
	
	if(send_cmd_head(pClient->sock, 0, logonHandle, IPNC_PTZ_CTRL, 0, 1, dataSize) < 0)
		return -1;
	if(send_cmd_data(pClient->sock, ptzPack, dataSize) < 0)
		return -1;
	if(recv_cmd_response(pClient->sock, &cmdHdr, NULL, 0) < 0)
		return -1;

	return cmdHdr.result;
}
//搜索配置
int ipnc_client_set_search_cfg(long handle, IPNC_NET_SEARCH_CFG_REQ_S *pSearCfg)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	if(pClient == NULL || pSearCfg == NULL)
		return -1;
	if(send_cmd_head(pClient->sock, 0, 0, IPNC_SEARCH_CFG, 0, 1, sizeof(IPNC_NET_SEARCH_CFG_REQ_S)) < 0)
		return -1;
	if(send_cmd_data(pClient->sock, pSearCfg, sizeof(IPNC_NET_SEARCH_CFG_REQ_S)) < 0)
		return -1;
	return recv_cmd_response_result(pClient->sock);
}

//搜索
int ipnc_client_start_search(long handle)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	if(pClient == NULL)
		return -1;
	if(send_cmd_head(pClient->sock, 0, 0, IPNC_START_SEARCH, 0, 1, sizeof(handle)) < 0)
		return -1;
	if(send_cmd_data(pClient->sock, &handle, sizeof(handle)) < 0)
		return -1;
	return recv_cmd_response_result(pClient->sock);
}
//停止搜索
int ipnc_client_stop_search(long handle)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	if(pClient == NULL)
		return -1;
	if(send_cmd_head(pClient->sock, 0, 0, IPNC_STOP_SEARCH, 0, 1, sizeof(handle)) < 0)
		return -1;
	if(send_cmd_data(pClient->sock, &handle, sizeof(handle)) < 0)
		return -1;
	return recv_cmd_response_result(pClient->sock);
}

//获得搜索
int ipnc_client_get_search(long handle, IPNC_NET_SEARCH_RES_S *pDevReq)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	CmdHead cmdHdr;

	if(pClient == NULL)
		return -1;

	if(pDevReq == NULL)
		return -1;

	if(send_cmd_head(pClient->sock, 0, 0, IPNC_GET_SEARCH, 0, 1, sizeof(handle)) < 0)
		return -1;
	if(send_cmd_data(pClient->sock, &handle, sizeof(handle)) < 0)
		return -1;
	if(recv_cmd_response(pClient->sock, &cmdHdr, (void*)pDevReq, sizeof(IPNC_NET_SEARCH_RES_S)) < 0)
		return -1;
	//printf("cmdHdr.result: %d\n", cmdHdr.result);
	
	return cmdHdr.result;
}
//获得IPC状态
int ipnc_client_test_alive(long handle, const char *ipAddr, int port, 
								 const char *user, const char *passwd)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	IpncConnect cmd;
	
	if(pClient == NULL)
		return -1;
	if(ipAddr == NULL)
		return -1;
	
	memset(&cmd, 0, sizeof(cmd));
	strncpy(cmd.ipAddr, ipAddr, sizeof(cmd.ipAddr)-1);
	cmd.port = port;
	if(user != NULL)
		strncpy(cmd.user, user, sizeof(cmd.user));
	if(passwd != NULL)
		strncpy(cmd.passwd, passwd, sizeof(cmd.passwd)-1);
	cmd.poolCh = 0;
	if(send_cmd_head(pClient->sock, 0, 0, IPNC_TEST_IPC_STATUS, 0, 1, sizeof(cmd)) < 0)
		return -1;
	if(send_cmd_data(pClient->sock, &cmd, sizeof(cmd)) < 0)
		return -1;
	return recv_cmd_response_result(pClient->sock);
}

//获取流
int ipnc_client_start_get_stream(long handle, long ipncHandle, int chn, int streamNo, long *streamHandle)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	IpncChannel channel = {0};
	CmdHead cmdHdr;
	unsigned char szRecvBuff[RECV_BUFF_SIZE];
	if(pClient == NULL)
		return -1;
	if(streamHandle == NULL)
		return -1;
	channel.chn = chn;
	channel.streamNo = streamNo;
	//channel.poolCh = poolCh;
	channel.poolStreamNo = streamNo;
	if(send_cmd_head(pClient->sock, ipncHandle, 0, IPNC_START_GET_STREAM, 0, 1, sizeof(channel)) < 0)
		return -1;
	if(send_cmd_data(pClient->sock, &channel, sizeof(channel)) < 0)
		return -1;
	if(recv_cmd_response(pClient->sock, &cmdHdr, szRecvBuff, sizeof(szRecvBuff)) < 0)
		return -1;
	*streamHandle = *((long *)szRecvBuff);
	return 0;
}
//停止获取流
int ipnc_client_stop_get_stream(long handle, long ipncHandle, long streamHandle)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	if(pClient == NULL)
		return -1;
	if(send_cmd_head(pClient->sock, ipncHandle, 0, IPNC_STOP_GET_STREAM, 0, 1, sizeof(streamHandle)) < 0)
		return -1;
	if(send_cmd_data(pClient->sock, &streamHandle, sizeof(streamHandle)) < 0)
		return -1;
	return recv_cmd_response_result(pClient->sock);
}
//请求i帧
int ipnc_client_request_iframe(long handle, long ipncHandle, long streamHandle)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	if(pClient == NULL)
		return -1;
	if(send_cmd_head(pClient->sock, ipncHandle, 0, IPNC_REQUEST_IFRAME, 0, 1, sizeof(streamHandle)) < 0)
		return -1;
	if(send_cmd_data(pClient->sock, &streamHandle, sizeof(streamHandle)) < 0)
		return -1;
	return recv_cmd_response_result(pClient->sock);
}
//连接服务器
int ipnc_client_connect_server(const char *protocolServerName, long *handle)
{
	int tryNum = 0;
	char path[128];
	IpncProtocolClient *pClient;
	if(handle == NULL)
		return -1;
	if(protocolServerName == NULL)
		return -1;
	if(strlen(protocolServerName) == 0)
		return -1;
	pClient = (IpncProtocolClient*)malloc(sizeof(IpncProtocolClient));
	if(pClient == NULL)
		return -1;
	memset(pClient, 0, sizeof(IpncProtocolClient));
	strncpy(pClient->protocolServerName, protocolServerName, sizeof(pClient->protocolServerName)-1);

	sprintf(path, "/tmp/%s", protocolServerName);
	while(tryNum++ < 3)
	{
		pClient->sock = net_local_tcp_connect(path);
		if(pClient->sock < 0)
		{
			sleep(1);
			continue;
		}
		break;
	}
	if(pClient->sock < 0)
	{
		ERR("Connet to the protocol server %s failed.\n", protocolServerName);
		free(pClient);
		return -1;	
	}
	*handle = (long)pClient;
	//printf("%s: *handle: 0x%08lx\n", __func__, *handle);
	return 0;
}
//断开连接
void ipnc_client_disconnect_server(long handle)
{
	IpncProtocolClient *pClient = (IpncProtocolClient*)handle;
	if(pClient == NULL)
		return;
	if(pClient->sock > 0)
		net_close_socket(&pClient->sock);
	free(pClient);
}

