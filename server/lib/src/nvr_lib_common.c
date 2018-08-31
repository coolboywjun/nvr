#include <nvr_headers.h>

#include <nvr_lib_common.h>
#include <ipnc_lib.h>


static int nvr_check_net_hdr(NVR_NET_HDR_S *hdr)
{
	if(hdr->u32MagicFlag != NVR_NET_MAGIC_FLAG)
		return -1;
	return 0;
}

static int nvr_recv_net_hdr(int sock, NVR_NET_HDR_S *hdr)
{
	if(net_tcp_noblock_recv(sock, hdr, sizeof(*hdr), sizeof(*hdr), 1000) < 0)
		return -1;
	return nvr_check_net_hdr(hdr);
}

void nvr_make_net_hdr(NVR_NET_HDR_S *hdr, int chn, int cmdType, int packSize, int errNo)
{
	hdr->u32MagicFlag = NVR_NET_MAGIC_FLAG;
	hdr->u16CmdType = cmdType;
	hdr->u32PacketSize = packSize;
	hdr->s8Chn = chn;
	hdr->s8ErrNo = errNo;
}

int nvr_recv_net_packet(int sock, NVR_NET_HDR_S *hdr, void * recvBuf, int bufLen)
{
	if(nvr_recv_net_hdr(sock, hdr) < 0)
		return -1;
	if(hdr->u32PacketSize < bufLen || hdr->s8ErrNo < 0)
		return -1;
	if(hdr->u32PacketSize > 0)
	{
		if(net_tcp_noblock_recv(sock, recvBuf, bufLen, hdr->u32PacketSize, NVR_NET_RECV_TIMEOUT)
			!= hdr->u32PacketSize)
			return -1;
	}
	return hdr->u32PacketSize;
}

int nvr_send_net_packet(int sock, NVR_NET_HDR_S *hdr, void * sendBuf, int sendLen)
{	
	if(net_tcp_noblock_send(sock, hdr, NVR_NET_HDR_LEN, 
			NULL, NVR_NET_SEND_TIMEOUT) != NVR_NET_HDR_LEN)
		return -1;
	if(hdr->u32PacketSize != sendLen)
		__ERR("The send data size is not equal the packet size of the net header.\n");
	if(sendBuf != NULL && sendLen > 0)
	{
		if(net_tcp_noblock_send(sock, sendBuf, sendLen, 
			NULL, NVR_NET_SEND_TIMEOUT) != sendLen)
			return -1;
	}
	return sendLen;
}


extern int g_nvrsock;
static pthread_mutex_t g_nvrmutex = PTHREAD_MUTEX_INITIALIZER;

/*发送命令:
socket:套接字
recvBuf:接收缓冲区
recvBufSize:接收缓冲区大小
sendBuf:发送缓冲区
sendBufSize:发送缓冲区大小
成功返回TI_SUCCESS*/
int nvr_send_cmd(void *recvBuf, int recvBufSize, void *sendBuf , int sendBufSize)
{
	int ret = 0;
	NVR_NET_HDR_S netHdr;
	char recvBufTmp[RECV_BUFF_SIZE] = {0};
	
	//发送数据
	if((NULL == sendBuf) || (sendBufSize < sizeof(NVR_NET_HDR_S)) )
	{
		__ERR("sendBuf or sendBufSize is error!!\r\n");
		return NVR_FAILURE;
	}
	
	pthread_mutex_lock(&g_nvrmutex);
	if(g_nvrsock <= 0)
	{
		g_nvrsock = net_local_tcp_connect("/tmp/server");
		if(g_nvrsock < 0)
		{
			__ERR("Connect to /tmp/server failed.\n");
			pthread_mutex_unlock(&g_nvrmutex);
			return NVR_FAILURE;
		}
	}
	if(net_tcp_noblock_send(g_nvrsock, sendBuf, sendBufSize, NULL, 1000) < 0)
	{
		__ERR("Failed to send packet to the server. g_nvrsock:%d\r\n", g_nvrsock);
		net_close_socket(&g_nvrsock);
		pthread_mutex_unlock(&g_nvrmutex);
		return NVR_NET_ERR_SEND_FAIL;
	}

	//接收数据
	memset(&netHdr, 0, sizeof(NVR_NET_HDR_S));
	if(0 > net_tcp_noblock_recv(g_nvrsock, &netHdr, 
								sizeof(netHdr), sizeof(netHdr),1000))
	{
		__ERR("nvr lib recv head packet failed. g_nvrsock %d!\n", g_nvrsock);
		net_close_socket(&g_nvrsock);
		pthread_mutex_unlock(&g_nvrmutex);
		return NVR_FAILURE;
	}
	//__DBUG("netHdr.u16CmdType: %#x\n", netHdr.u16CmdType);
	//__DBUG("netHdr.u32PacketSize: %d\n", netHdr.u32PacketSize);
		
	if(0 > netHdr.s8ErrNo)
	{
		__ERR("nvr lib recv head error:%d \n", netHdr.s8ErrNo);
		net_close_socket(&g_nvrsock);
		pthread_mutex_unlock(&g_nvrmutex);
		return NVR_FAILURE;
	}
		
	if(0 < netHdr.u32PacketSize)
	{
		ret = net_tcp_noblock_recv(g_nvrsock, recvBufTmp, sizeof(recvBufTmp), 
									   netHdr.u32PacketSize, NVR_NET_RECV_TIMEOUT);
		/*if(netHdr.u32PacketSize != net_tcp_noblock_recv(g_nvrsock, recvBufTmp, sizeof(recvBufTmp), 
													   netHdr.u32PacketSize, NVR_NET_RECV_TIMEOUT))*/
		//__DBUG("ret : %d\n", ret );
		if(0 > ret)											   
		{
			net_close_socket(&g_nvrsock);
			pthread_mutex_unlock(&g_nvrmutex);
			return NVR_FAILURE;
		}

		//__DBUG("recvBufSize: %d\n", recvBufSize);
		if(NULL != recvBuf)
		{	
			if(netHdr.u32PacketSize > recvBufSize)
			{
				__ERR("nvr lib recv buf size is too small !\n");
				net_close_socket(&g_nvrsock);
				pthread_mutex_unlock(&g_nvrmutex);
				return NVR_FAILURE;
			}
			else
				memcpy(recvBuf, recvBufTmp, netHdr.u32PacketSize);
		}
	}
	pthread_mutex_unlock(&g_nvrmutex);
	
	return netHdr.u32PacketSize;
}

