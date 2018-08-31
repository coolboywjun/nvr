
#include <glib.h>
#include <sys/select.h>
#include <nvr_net_common.h>
#include <ipnc_lib_client.h>
#include "nvr_param_sys.h"
#include "nvr_channel.h"
#include <time.h>
#include <sys/time.h>

static inline int nvr_check_net_hdr(NVR_NET_HDR_S *hdr)
{
	return (hdr->u32MagicFlag == NVR_NET_MAGIC_FLAG) ? 0 : -1;
}

static inline int nvr_recv_net_hdr(int sock, NVR_NET_HDR_S *hdr)
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
	if(hdr->u32PacketSize > bufLen)
	{
		j_warning("The receive buffer is too small, the packet size is %d and buffer length is %d.\n",
			hdr->u32PacketSize, bufLen);	
		return -1;
	}
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
		j_warning("The send data size is not equal the packet size of the net header.\n");
	if(sendBuf != NULL && sendLen > 0)
	{
		if(net_tcp_noblock_send(sock, sendBuf, sendLen, 
				NULL, NVR_NET_SEND_TIMEOUT) != sendLen)
			return -1;
	}
	return sendLen;
}

//获取参数
int nvr_net_get_param(int chnNo, IPNC_CMD_TYPE_E cmd, void* packet, int dataSize)
{
	g_return_val_if_fail(packet != NULL && dataSize > 0, -1);
	return nvr_channel_get_param(chnNo, cmd, packet, dataSize);
}

//设置参数
int nvr_net_set_param(int chnNo, IPNC_CMD_TYPE_E cmd, void* packet, int dataSize)
{
	g_return_val_if_fail(packet != NULL && dataSize > 0, -1);
	return nvr_channel_set_param(chnNo, cmd, packet, dataSize);
}

//唤醒线程
void nvr_pthread_signal(pthread_cond_t *pThrcond, pthread_mutex_t *pThrLock)
{
	pthread_cond_signal(pThrcond);
	//pthread_mutex_unlock(pThrLock);
	
}
//线程阻塞
int nvr_pthread_timeout_wait(pthread_cond_t *pThrcond, pthread_mutex_t *pThrLock, int timeOut)
{
	struct timespec timer;
	clock_gettime(CLOCK_MONOTONIC, &timer);
	if(0 >= timeOut)
   		timer.tv_sec += 5;//超时
   	else
		timer.tv_sec += timeOut;//超时
		
   	if(ETIMEDOUT == pthread_cond_timedwait(pThrcond,pThrLock,&timer))
          return ETIMEDOUT;

	return 0;
}


