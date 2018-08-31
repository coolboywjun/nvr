#include <unistd.h>
#include <sys/select.h>
#include <sys/prctl.h>
#include <errno.h>

#include <av_pool.h>
#include "ti_struct.h"
#include "ti_type.h"
#include "ti_net_common.h"
#include "ti_net_interface.h"
#include "ti_net_logon.h"


static inline int ti_logon_pipe_write(int fd, const void *sendBuf, int sendLen)
{
	int nLen;
	int nTotalSendLen = 0;
	while(nTotalSendLen < sendLen)
	{
		nLen = write(fd, sendBuf + nTotalSendLen, sendLen - nTotalSendLen);
		if(nLen < 0)
		{
			if(errno == EINTR || errno == EAGAIN)
				continue;
			return -1;
		}
		nTotalSendLen += nLen;
	}
	return nTotalSendLen;
}

static inline int ti_logon_pipe_read(int fd, void *recvBuf, int recvLen)
{
	int nLen;
	int nTotalRecvLen = 0;
	while(nTotalRecvLen < recvLen)
	{
		nLen = read(fd, recvBuf + nTotalRecvLen, recvLen - nTotalRecvLen);
		if(nLen < 0)
		{
			if(errno == EINTR || errno == EAGAIN)
				continue;
			return -1;
		}
		nTotalRecvLen += nLen;
	}
	return nTotalRecvLen;
}

static int ti_logon_pipe_send(int fd, const void *sendBuf, int sendLen)
{
	return ti_logon_pipe_write(fd, sendBuf, sendLen);
}

static int ti_logon_pipe_recv(int fd, char *recvBuf, int recvBufLen)
{
	int len;
	TI_NET_HDR_S *pNetHdr = (TI_NET_HDR_S *)recvBuf;
	if((len = ti_logon_pipe_read(fd, pNetHdr, TI_NET_HDR_LEN)) != TI_NET_HDR_LEN)
		return -1;	
	if(pNetHdr->u32PacketSize <= recvBufLen - TI_NET_HDR_LEN)
	{
		if(ti_logon_pipe_read(fd, pNetHdr + 1, pNetHdr->u32PacketSize) != pNetHdr->u32PacketSize)
			return -1;
		len += pNetHdr->u32PacketSize;
	}

	return len;
}

int ti_logon_pipe_session(TI_LOGON_USER_INFO *pLogonInfo, 
							const void *sendBuf, int sendLen, 
							char *recvBuf, int recvBufLen)
{
	int len;
	if(ti_logon_pipe_send(pLogonInfo->sendPipe[1], sendBuf, sendLen) != sendLen)
		return -1;
	if(net_select(&pLogonInfo->recvPipe[0], 0x1, 0x1, 3000) <= 0)
		return -1;
	if((len = ti_logon_pipe_recv(pLogonInfo->recvPipe[0], recvBuf, recvBufLen)) < 0)
		return -1;
	return len;
}

static  int ti_logon_cmd_send(int socket, void *sendBuff, int sendLen)
{
	if(sendBuff == NULL || sendLen < TI_NET_HDR_LEN)
		return -1;
	if(net_tcp_noblock_send(socket, sendBuff, sendLen, NULL, 1000) != sendLen)
	{
		__ERR("(%s:%d:%s)send data to server is failed !\n", __FILE__, __LINE__, __FUNCTION__);
		return TI_ERR_SEND_FAIL;
	}
	return sendLen;
}

static int ti_logon_cmd_recv(int socket, void *recvBuff, int recvBuffLen)
{
	TI_NET_HDR_S *pNetHdr = (TI_NET_HDR_S *)recvBuff;
	void *pRecvData;
	int len;
	if(recvBuff == NULL || recvBuffLen < TI_NET_HDR_LEN)
		return -1;
	pNetHdr = (TI_NET_HDR_S *)recvBuff;
	pRecvData = recvBuff + TI_NET_HDR_LEN;

	len = net_tcp_noblock_recv(socket, pNetHdr, TI_NET_HDR_LEN, TI_NET_HDR_LEN,  1000);
	if(len != TI_NET_HDR_LEN)
	{
		__ERR("(%s:%d:%s)recv data from server is failed !\n", __FILE__, __LINE__, __FUNCTION__);
		return TI_ERR_RECV_FAIL;
	}
	if(ti_net_check_netHdr(pNetHdr) < 0)
	{
		__ERR("(%s:%d:%s)pNetHdr->u32MagicFlag is error !\n", __FILE__, __LINE__, __FUNCTION__);
		return TI_ERR_RECV_FAIL;
	}

	if(pNetHdr->u32PacketSize > 0)
	{
		if(net_tcp_noblock_recv(socket, pRecvData, 
				recvBuffLen - TI_NET_HDR_LEN, pNetHdr->u32PacketSize, 1000) != pNetHdr->u32PacketSize)
		{	
			__ERR("Recv data failed\n");
			return TI_ERR_RECV_FAIL;
		}
		len += pNetHdr->u32PacketSize;
	}

	return len;
}

static int ti_logon_cmd_session(TI_LOGON_USER_INFO *pLogonInfo, void *sendBuff , int sendLen, void *recvBuff, int recvBuffLen)
{
	int len;
	TI_NET_HDR_S *pNetHdr = (TI_NET_HDR_S *)sendBuff;
	//发送数据
	if((sendBuff == NULL) || (sendLen < sizeof(TI_NET_HDR_S)) 
		|| (recvBuff == NULL) || (recvBuffLen < sizeof(TI_NET_HDR_S)))
	{
		__ERR("(%s:%d:%s)sendBuf or sendBufSize is error !\n", __FILE__, __LINE__, __FUNCTION__);
		return TI_FAILURE;
	}
	pNetHdr->u32PacketNo = pLogonInfo->packetNo++;
	if(ti_logon_cmd_send(pLogonInfo->logonSock, sendBuff, sendLen) != sendLen)
	{
		__ERR("logon send command failed\n");
		return TI_FAILURE;
	}
	if((len = ti_logon_cmd_recv(pLogonInfo->logonSock, recvBuff, recvBuffLen)) < 0)
	{
		__ERR("logon recv command failed\n");
		return TI_FAILURE;
	}

	return len;
}

/*发送心跳包*/
static int ti_logon_keep_alive(TI_LOGON_USER_INFO *pLogonInfo)
{
	TI_NET_HDR_S netHdr;

	net_set_net_hdr(&netHdr, 0, TI_CMD_KEEP_ALIVE, pLogonInfo->packetNo++, 0);
	if(ti_logon_cmd_send(pLogonInfo->logonSock, &netHdr, sizeof(netHdr)) < 0)
	{
		__ERR("Send keep alive command failed\n");
		return TI_FAILURE;
	}
	return TI_SUCCESS;
}
/*报警处理默认处理函数*/
static void ti_logon_alarm_ctrl(long logonHandle, void *data, int dataSize)
{
	TI_LOGON_USER_INFO *pLogonInfo = NULL;
	int poolCh = 0;
	TI_ALARM_INFO_S *pAlarmData = NULL;
	
	if((NULL == (TI_LOGON_USER_INFO *)logonHandle) || (NULL == data ))
		return;

	if(sizeof(TI_ALARM_INFO_S) == dataSize)
	{
		pLogonInfo = (TI_LOGON_USER_INFO *)logonHandle;
		poolCh = *((int*)pLogonInfo->pUserData);
		pAlarmData = (TI_ALARM_INFO_S*)data;
		
		if((TI_ALARM_IN_HAPPEN == pAlarmData->u8AlarmType) ||
		   (TI_MD_HAPPEN == pAlarmData->u8AlarmType))//报警发生
		{
			pLogonInfo->alarm = 1;
			if(0 > av_pool_set_alarm_status(poolCh, pLogonInfo->alarm))
				__ERR("av pool set alarm failed.\n");
		}
		else//报警消失
		{
			pLogonInfo->alarm = 0;
			if(0 > av_pool_set_alarm_status(poolCh, pLogonInfo->alarm))
				__ERR("av pool set alarm failed.\n");
		}
		
	}
}

/*登录服务器：
  成功返回TI_SUCCESS*/
static int ti_logon_to_server(TI_LOGON_USER_INFO *pLogonInfo, const char *usr, const char*psw)
{
	char sendBuf[256];
	char recvBuf[2048];
	TI_NET_HDR_S *pNetHdr = (TI_NET_HDR_S *)sendBuf;
	TI_NET_LOGON_S *pNetLogon = (TI_NET_LOGON_S *)(pNetHdr + 1);
	TI_DEV_STABLE_INFO_S* pDev = (TI_DEV_STABLE_INFO_S*)(recvBuf + TI_NET_HDR_LEN);

	net_set_net_hdr((TI_NET_HDR_S *)sendBuf, sizeof(TI_NET_LOGON_S), TI_CMD_LOGON_SERVER, 0, 0);

	strcpy(pNetLogon->szUsr, usr);
	strcpy(pNetLogon->szPsw, psw);

	ti_net_encrypt(pNetLogon->szUsr, strlen(pNetLogon->szUsr));
	ti_net_encrypt(pNetLogon->szPsw, strlen(pNetLogon->szPsw));

	/*发送参数命令*/
	if(ti_logon_cmd_session(pLogonInfo, sendBuf, sizeof(TI_NET_HDR_S)+sizeof(TI_NET_LOGON_S), recvBuf, sizeof(recvBuf)) < 0)
	{
		__ERR("send parameter command failed !\r\n");
		return TI_FAILURE;
	}
	pNetHdr = (TI_NET_HDR_S *)recvBuf;
	if(pNetHdr->s32ErrNo < 0)
	{
		__ERR("The logon response is failed.\n");
		return TI_FAILURE;
	}
	
	if(pDev != NULL)
	{
		__DBUG("u32HwVer:0x%x\r\n", pDev->u32HwVer);
		__DBUG("u32CoreVer:0x%x\r\n", pDev->u32CoreVer);
		__DBUG("u32SoftVer:0x%x\r\n", pDev->u32SoftVer);
	}

	return TI_SUCCESS;
}

/*登录线程处理函数*/
static void* ti_logon_thread_fun(void *data)
{
	TI_LOGON_USER_INFO *pLogonInfo = (TI_LOGON_USER_INFO *)data;
	int retVal  = -1;
	int len;
	int sendPipe = pLogonInfo->sendPipe[0];
	int maxFds;
	int aliveCnt = 0;
	TI_U32 u32SendPacketNo = 0;
	TI_U16 u16SendCmd = 0;
	fd_set rfds;
	struct timeval tv;
	char sendBuff[2048];
	char recvBuff[2048];
	char szPort[16];
	TI_NET_HDR_S *pNetHdr;
	
	prctl(PR_SET_NAME, "Logon");
	while(!pLogonInfo->quit)
	{
		if(pLogonInfo->logonSock <= 0)
		{
			sprintf(szPort, "%d", pLogonInfo->port);
			pLogonInfo->logonSock = ti_connect_server(pLogonInfo->ipAddr, szPort);
			if(pLogonInfo->logonSock < 0)
			{
				//__ERR("socket create failed!\r\n");
				sleep(1);
				continue;
			}
			if(ti_logon_to_server(pLogonInfo, pLogonInfo->userName, pLogonInfo->passWord) != TI_SUCCESS)
			{
				__ERR("Failed to logon!\r\n");
				net_close_socket(&pLogonInfo->logonSock);
				continue;
			}
		}
		if(aliveCnt == 30)
		{
			if(ti_logon_keep_alive(pLogonInfo) < 0)
			{
				__ERR("Send keep alive command failed\n");
				net_close_socket(&pLogonInfo->logonSock);
				continue;
			}
			aliveCnt = 0;
		}
		
		FD_ZERO(&rfds);
		FD_SET(pLogonInfo->logonSock, &rfds);
		FD_SET(sendPipe, &rfds);
		
		tv.tv_sec = 1; 
		tv.tv_usec = 0;
		maxFds = ((sendPipe > pLogonInfo->logonSock) ? sendPipe : pLogonInfo->logonSock) + 1;
		retVal = select(maxFds, &rfds, NULL, NULL, &tv);
		if(retVal < 0 )
		{
			__ERR("Logon select failed\n");
			net_close_socket(&pLogonInfo->logonSock);
			continue;
		}
		else if(retVal == 0)		// 超时
		{
			aliveCnt++;
			continue;
		}
		
		if(FD_ISSET(pLogonInfo->logonSock, &rfds))
		{
			if((len = ti_logon_cmd_recv(pLogonInfo->logonSock, recvBuff, sizeof(recvBuff))) < 0)
			{
				__ERR("Recv logon command failed\n");
				net_close_socket(&pLogonInfo->logonSock);
				continue;
			}
			pNetHdr = (TI_NET_HDR_S *)recvBuff;
			if(pNetHdr->u16CmdType == TI_CMD_KEEP_ALIVE)
			{
				//__DBUG("Recv keep alive respond:%d.\n", pLogonInfo->logonSock);
			}
			else if(pNetHdr->u16CmdType == TI_CMD_ALARM_MSG)
			{
				//__DBUG("CMD: %x\n", TI_CMD_ALARM_MSG);

				//报警处理
				pLogonInfo->cbAlarmFun((long)pLogonInfo, 
									   recvBuff + TI_NET_HDR_LEN,
									   sizeof(TI_ALARM_INFO_S));

			}
			else if(pNetHdr->u16CmdType == TI_CMD_COMM_DATA_MSG)
			{
				//TI_ALARM_INFO_S *pAlarmInfo = (TI_ALARM_INFO_S*)(pNetHdr + 1);
			}
			else if(pNetHdr->u16CmdType == u16SendCmd && (pNetHdr->u32PacketNo == (u32SendPacketNo | 0x80000000)))
			{
				if(ti_logon_pipe_send(pLogonInfo->recvPipe[1], recvBuff, len) < 0)
					__ERR("Send respond to pipe failed.\n");
				u16SendCmd = 0;
				u32SendPacketNo = 0;
			}
			else
			{
				__ERR("Recv command %#x\n", pNetHdr->u16CmdType);
			}
		}
		
		if(FD_ISSET(sendPipe, &rfds))
		{
			if((len = ti_logon_pipe_recv(sendPipe, sendBuff, sizeof(sendBuff))) <= 0)
				break;
			pNetHdr = (TI_NET_HDR_S *)sendBuff;
			pNetHdr->u32PacketNo = pLogonInfo->packetNo++;
			if((len = ti_logon_cmd_send(pLogonInfo->logonSock, sendBuff, len)) < 0)
			{
				__ERR("Send logon cmd failed.\n");
				net_close_socket(&pLogonInfo->logonSock);
				continue;
			}
			u32SendPacketNo = pNetHdr->u32PacketNo;
			u16SendCmd = pNetHdr->u16CmdType;
			aliveCnt = 0;
		}
		
	}//while end
	av_pool_set_alarm_status(*((int *)pLogonInfo->pUserData),  0);
	
	return NULL;
}

/*登录设备: 
  成功返回用户信息句柄*/
long ti_net_logon(const char* DesIP, const char* User, const char* Password, const char* port, 
				TI_U32 keep_alive_time, fAlarmCb cbAlarmFun, void* pUserData)
{
	TI_LOGON_USER_INFO *pLogonInfo;
	
	if(DesIP == NULL || User == NULL || port == NULL)
		return 0;
	pLogonInfo = malloc(sizeof(TI_LOGON_USER_INFO));
	if(pLogonInfo == NULL)
		return 0;
	memset(pLogonInfo, 0, sizeof(TI_LOGON_USER_INFO));
	if(pipe(pLogonInfo->sendPipe) < 0 || pipe(pLogonInfo->recvPipe) < 0)
	{
		__ERR("Create pipe failed\n");
		goto failed;
	}
	pLogonInfo->logonSock = ti_connect_server(DesIP, port);
	if(pLogonInfo->logonSock < 0)
	{
		__ERR("socket create failed: %s %s!\r\n", DesIP, port);
		goto failed;
	}
	pLogonInfo->port = atoi(port);
	strcpy(pLogonInfo->ipAddr, DesIP);
	strcpy(pLogonInfo->userName, User);
	strcpy(pLogonInfo->passWord, Password);
	pLogonInfo->keepAliveTime = 30;		// seconds
	
	//报警回调函数
	if(NULL == cbAlarmFun)
		pLogonInfo->cbAlarmFun = ti_logon_alarm_ctrl;//默认处理方式
	else
		pLogonInfo->cbAlarmFun = cbAlarmFun;
	
	pLogonInfo->pUserData = pUserData;
	
	/*登录服务器*/
	if(ti_logon_to_server(pLogonInfo, User, Password) != TI_SUCCESS)
	{
		__ERR("Failed to logon!\r\n");
		goto failed;
	}

	if(pthread_create(&pLogonInfo->thid, NULL, ti_logon_thread_fun, pLogonInfo) != 0)
	{
		__ERR("Create logon thread failed.\n");
		goto failed;
	}

	return (long)pLogonInfo;
	
failed:
	if(pLogonInfo->sendPipe[0] > 0)
		close(pLogonInfo->sendPipe[0]);
	if(pLogonInfo->sendPipe[1] > 0)
		close(pLogonInfo->sendPipe[1]);
	if(pLogonInfo->recvPipe[0] > 0)
		close(pLogonInfo->recvPipe[0]);
	if(pLogonInfo->recvPipe[1] > 0)
		close(pLogonInfo->recvPipe[1]);
	if(pLogonInfo->logonSock > 0)
		net_close_socket(&pLogonInfo->logonSock);
	free(pLogonInfo);
	
	return 0;
}

/*退出登录,
  成功返回TI_SUCCESS*/
//int ti_net_logout(TI_LOGON_USER_INFO *pLogonInfo)
int ti_net_logout(long handle)
{
	TI_LOGON_USER_INFO *pLogonInfo = (TI_LOGON_USER_INFO *)handle;
	
	if(pLogonInfo == NULL)
		return -1;
	pLogonInfo->quit = 1;
	pthread_join(pLogonInfo->thid, NULL);
	close(pLogonInfo->sendPipe[0]);
	close(pLogonInfo->sendPipe[1]);
	close(pLogonInfo->recvPipe[0]);
	close(pLogonInfo->recvPipe[1]);
	net_close_socket(&pLogonInfo->logonSock);
	free(pLogonInfo);
	return 0;
}



