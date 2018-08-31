
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
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <net/if.h>

#include <errno.h>

#include <net_api.h>

#include <ipnc_lib.h>

#include <ipnc_lib_server.h>

//#define IPNC_LIB_SERVER_DEBUG
#if defined(__DEBUG__) && defined(IPNC_LIB_SERVER_DEBUG)
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
	int quit;
	int sock;
	int epoll_fd;
	char protocolName[64];
	char path[64];
	IpncProtocolCbTable cbTable;

	pthread_t thread;
}IpncProtocolMng;

IpncProtocolMng gProtocol;

#define COMPARE_SIZE(strct_type, arglen)	\
	if(arglen != sizeof(strct_type))	\
		return -EINVAL;
		
// CBTABLE, 简写CT
#define CT	(gProtocol.cbTable)

int ipnc_server_control(CmdHead *cmdHdr, void *arg, int argLen, void *outArg, int *outArgLen)
{
	//DBG("server receive command: %x\n", cmdHdr->cmd);
	int outOrgLen = *outArgLen;
	*outArgLen = 0;
	switch(cmdHdr->cmd)
	{
		case IPNC_CONNECT://建立连接
		{
			IpncConnect *cmdArg = (IpncConnect*)arg;
			DBG("IPNC_CONNECT:%x %s %d\n", IPNC_CONNECT, cmdArg->ipAddr, cmdArg->port);
			
			
			COMPARE_SIZE(IpncConnect, argLen);
			
			if(CT.connectCb)
				return CT.connectCb(cmdArg->poolCh, cmdArg->ipAddr, cmdArg->port, cmdArg->user, cmdArg->passwd, &cmdHdr->ipncHandle);
			break;
		}
		case IPNC_DISCONNECT://断开连接
		{
			DBG("IPNC_DISCONNECT:%x\n", IPNC_DISCONNECT);
			
			if(CT.disconnectCb)
				return CT.disconnectCb(cmdHdr->ipncHandle);
			break;
		}
		case IPNC_START_SEARCH://开始搜索
		{
			long lHandle = 0;
			
			memcpy(&lHandle, arg, sizeof(lHandle));
			//printf("IPNC_START_SEARCH:%x\n", IPNC_START_SEARCH);
			//printf("lHandle:%lx\n", lHandle);
			
			if(CT.startSearchCb)
			{
				*outArgLen = 0;
				return CT.startSearchCb(lHandle);
			}
			break;
		}
		case IPNC_STOP_SEARCH://停止搜索
		{
			long lHandle = 0;
			
			memcpy(&lHandle, arg, sizeof(lHandle));
			
			//printf("IPNC_STOP_SEARCH:%x\n", IPNC_STOP_SEARCH);
			//printf("lHandle:%lx\n", lHandle);
			
			if(CT.stopSearchCb)
			{
				*outArgLen = 0;
				return CT.stopSearchCb(lHandle);
			}
			break;
		}
		case IPNC_GET_SEARCH://获得搜索
		{
			long lHandle = 0;
			
			memcpy(&lHandle, arg, sizeof(lHandle));
			
			//printf("IPNC_GET_SEARCH:%x\n", IPNC_GET_SEARCH);
			//printf("lHandle:%lx\n", lHandle);
			
			if(CT.getSearchCb)
			{
				int ret;
				ret = CT.getSearchCb(lHandle, (IPNC_NET_SEARCH_RES_S*)outArg);
				if(ret <= 0)
					*outArgLen = 0;
				else
					*outArgLen = sizeof(IPNC_NET_SEARCH_RES_S);
				return ret;
			}
			
			break;
		}
		case IPNC_SEARCH_CFG://搜索配置
		{
			if(CT.searchCfgCb)
			{
				//IPNC_NET_SEARCH_CFG_REQ_S *pIpncSearchCfg = (IPNC_NET_SEARCH_CFG_REQ_S *)arg;
				//printf("server---------pIpncSearchCfg->u8SerialId: %s\n", pIpncSearchCfg->u8SerialId);
				//printf("server---------pIpncSearchCfg->u16DevPort: %d\n", pIpncSearchCfg->u16DevPort);
				return CT.searchCfgCb(arg);
			}
			break;
		}
		case IPNC_LOGON://登录
		{	
			DBG("IPNC_LOGON:%x\n", IPNC_LOGON);
			
			if(CT.logonCb)
			{
				*outArgLen = 0;
				return CT.logonCb(cmdHdr->ipncHandle, &cmdHdr->logonHandle);
			}
			break;
		}
		case IPNC_LOGOUT://注销登录
		{
			DBG("IPNC_LOGOUT:%x\n", IPNC_LOGOUT);
			
			if(CT.logoutCb)
			{
				*outArgLen = 0;
				return CT.logoutCb(cmdHdr->logonHandle);
			}
			break;
		}
		case IPNC_GET_PARAM://获取参数
		{
			DBG("IPNC_GET_PARAM:%x\n", IPNC_GET_PARAM);
			DBG("cmdHdr->paramCmd:%x\n",  cmdHdr->paramCmd);
			DBG("outOrgLen:%d\n",  outOrgLen);
			
			if(CT.getParamCb)
			{
				int len;
				len = CT.getParamCb(cmdHdr->logonHandle, cmdHdr->paramCmd, outArg, outOrgLen);
				DBG("get param len: %d\n", len);
				if(len > 0)
					*outArgLen = len;
				return len;
			}
			break;
		}
		case IPNC_SET_PARAM://设置参数
		{
			DBG("IPNC_SET_PARAM:%x\n", IPNC_SET_PARAM);
			
			if(CT.setParamCb)
			{
				*outArgLen = 0;
				return CT.setParamCb(cmdHdr->logonHandle, cmdHdr->paramCmd, arg, argLen);
			}
			break;
		}
		case IPNC_START_GET_STREAM://开始获取流
		{
			DBG("IPNC_START_GET_STREAM:%x\n", IPNC_START_GET_STREAM);
			
			IpncChannel *cmdArg = (IpncChannel*)arg;
			long streamHandle;
			COMPARE_SIZE(IpncChannel, argLen);
			if(CT.startGetStreamCb)
			{
				if(CT.startGetStreamCb(cmdHdr->ipncHandle, cmdArg->chn, 
									   cmdArg->streamNo, &streamHandle) >= 0)
				{
					memcpy(outArg, &streamHandle, sizeof(streamHandle));
					*outArgLen = sizeof(streamHandle);
					return 0;
				}
			}
			break;
		}
		case IPNC_STOP_GET_STREAM://停止获取流
		{
			DBG("IPNC_STOP_GET_STREAM:%x\n", IPNC_STOP_GET_STREAM);
			
			long streamHandle = *((int *)arg);
			COMPARE_SIZE(long, argLen);
			if(CT.stopGetStreamCb)
				return CT.stopGetStreamCb(cmdHdr->ipncHandle, streamHandle);
			break;
		}
		case IPNC_REQUEST_IFRAME://获取I帧
		{
			DBG("IPNC_REQUEST_IFRAME:%x\n", IPNC_REQUEST_IFRAME);
			
			long streamHandle = *((int *)arg);
			COMPARE_SIZE(long, argLen);
			if(CT.requestIFrameCb)
				return CT.requestIFrameCb(cmdHdr->ipncHandle, streamHandle);
			break;
		}
		case IPNC_PTZ_CTRL://pzt控制
		{	
			DBG("IPNC_PTZ_CTRL:%x\n", IPNC_PTZ_CTRL);
			
			if(CT.ptzControl)
			{
				*outArgLen = 0;
				return CT.ptzControl(cmdHdr->logonHandle, arg);
			}
			break;
		}
		case IPNC_TEST_IPC_STATUS://获得ipc状态
		{
			//DBG("IPNC_GET_IPC_STATUS:%x\n", IPNC_TEST_IPC_STATUS);
			//printf("argLen:%d\n", argLen);
			if(CT.testIpcStatus)
			{
				IpncConnect *cmdArg = (IpncConnect*)arg;
				char port[10] = {0};
				sprintf(port, "%d", cmdArg->port);
				return CT.testIpcStatus(cmdArg->ipAddr, cmdArg->user, cmdArg->passwd, port);
			}
			
			break;
		}
		default:
			DBG("cmdHdr->cmd:%x\n", cmdHdr->cmd);
			break;
	}
	*outArgLen = 0;
	return -1;
}

static int ipnc_server_create()
{
	int sock;
	sock = net_create_local_tcp_server(gProtocol.path);
	if(sock < 0)
		return sock;
	net_set_sock_noblock(sock);
	return sock;
}

static int ipnc_server_read_package(int sock)
{
	int nRet;
	CmdHead cmdHdr;
	unsigned char szRecvBuff[RECV_BUFF_SIZE] = {0};
	unsigned char szSendBuff[RECV_BUFF_SIZE] = {0};
	int nSendLen;
	
	if(net_tcp_noblock_recv(sock, &cmdHdr, sizeof(cmdHdr), sizeof(cmdHdr), 1000) != sizeof(cmdHdr))
	{
		net_close_socket(&sock);
		return -1;
	}
	if(cmdHdr.magic != PROCOTOL_REQUEST_MAGIC)
	{
		ERR("Cmd header magic is wrong, magic is: 0x%08x\n", cmdHdr.magic);
		net_close_socket(&sock);
		return -1;
	}
	//DBG("Recv cmd %x, length: %d\n", cmdHdr.cmd, cmdHdr.cmdDataLen);
	if(cmdHdr.cmdDataLen > 0)
	{
		if(net_tcp_noblock_recv(sock, szRecvBuff, sizeof(szRecvBuff), cmdHdr.cmdDataLen, 1000) != cmdHdr.cmdDataLen)
		{
			net_close_socket(&sock);
			return -1;
		}
	}
	nSendLen = sizeof(szSendBuff);
	cmdHdr.result = ipnc_server_control(&cmdHdr, szRecvBuff, cmdHdr.cmdDataLen, szSendBuff, &nSendLen);
	if(cmdHdr.response)
	{
		cmdHdr.magic = PROTOCOL_RESPONSE_MAGIC;
		cmdHdr.response = 0;
		cmdHdr.cmdDataLen = nSendLen;
		//DBG("cmdHdr.cmdDataLen: %d, nSendLen: %d\n", cmdHdr.cmdDataLen, nSendLen);
		nRet = net_tcp_noblock_send(sock, &cmdHdr, sizeof(cmdHdr), NULL, 1000);
		if(cmdHdr.cmdDataLen > 0)
			nRet |= net_tcp_noblock_send(sock, szSendBuff, cmdHdr.cmdDataLen, NULL, 1000);
		if(nRet < 0)
		{
			DBG("net_tcp_noblock_send failed:cmd %d, length: %d\n", cmdHdr.cmd, cmdHdr.cmdDataLen);
			net_close_socket(&sock);
			return -1;
		}
	}
	return 0;
}


static void* ipnc_server_fxn(void *data)
{
	int i, nfds, sock;
	struct epoll_event events[16];
	struct epoll_event ev;
	struct sockaddr_in  addr;
	int addr_len;
	
	while(!gProtocol.quit)
	{
		nfds = epoll_wait(gProtocol.epoll_fd, events, 16, 1000);
		if(nfds == 0)
			continue;
		else if(nfds < 0)
		{
			ERR("epool wait for ipnc lib server failed.\n");
			break;
		}
		else
		{
			for(i = 0; i < nfds; i++)
			{
				if(events[i].data.fd == gProtocol.sock)
				{
					addr_len = sizeof(addr);
					sock = net_tcp_block_accept(gProtocol.sock, 
												(struct sockaddr*)&addr, 
												&addr_len);
					if(sock > 0)
					{
						net_set_sock_noblock(sock);
						ev.events = EPOLLIN;
						ev.data.fd = sock;
						if(epoll_ctl(gProtocol.epoll_fd, EPOLL_CTL_ADD, sock, &ev) < 0)
							net_close_socket(&sock);
					}
				}
				else
				{
					if(events[i].events & EPOLLIN)
					{
						if(ipnc_server_read_package(events[i].data.fd) < 0)
						{
							sock = events[i].data.fd;
							epoll_ctl(gProtocol.epoll_fd, EPOLL_CTL_DEL, sock, NULL);
							net_close_socket(&sock);
						}
					}
				}
			}
		}
	}
	return NULL;
}

int ipnc_server_init(const char *protocolName, IpncProtocolCbTable *cbTable)
{
	struct epoll_event ev;
	memset(&gProtocol, 0, sizeof(gProtocol));

	signal(SIGPIPE, SIG_IGN);

	sprintf(gProtocol.path, "/tmp/%s", protocolName);
	memcpy(&gProtocol.cbTable, cbTable, sizeof(IpncProtocolCbTable));
	
	gProtocol.sock = ipnc_server_create();
	if(gProtocol.sock < 0)
	{
		ERR("Create the procotol %s local tcp server %s failed.\n", gProtocol.protocolName, gProtocol.path);
		return -1;
	}
	gProtocol.epoll_fd = epoll_create(1024);
	if(gProtocol.epoll_fd < 0)
	{
		ERR("Create epoll failed.");
		net_close_socket(&gProtocol.sock);
		return -1;
	}
	ev.events = EPOLLIN;
	ev.data.fd = gProtocol.sock;
	epoll_ctl(gProtocol.epoll_fd, EPOLL_CTL_ADD, gProtocol.sock, &ev);
	if(pthread_create(&gProtocol.thread, NULL, ipnc_server_fxn, NULL) != 0)
	{
		ERR("Failed to create protocol thread.\n");
		net_close_socket(&gProtocol.epoll_fd);
		net_close_socket(&gProtocol.sock);
		return -1;
	}
	
	return 0;
}

void ipnc_server_uninit()
{
	gProtocol.quit = 1;
	pthread_join(gProtocol.thread, NULL);
	if(gProtocol.epoll_fd > 0)
		close(gProtocol.epoll_fd);
	net_close_socket(&gProtocol.sock);
}

