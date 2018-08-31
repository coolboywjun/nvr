#ifndef __NVR_NET_COMMON_H__
#define __NVR_NET_COMMON_H__

#include <nvr_server_headers.h>
#include <ipnc_lib.h>

/*BUG调试*/
#define DEBUG
#ifdef DEBUG
	#define __DBUG(fmt, args...) fprintf(stderr, fmt, args)
#else
	#define __DBUG(fmt, args...) 
#endif

void nvr_make_net_hdr(NVR_NET_HDR_S *hdr, int chn, int cmdType, int packSize, int errNo);


int nvr_recv_net_packet(int sock, NVR_NET_HDR_S *hdr, void * recvBuf, int bufLen);
	
int nvr_send_net_packet(int sock, NVR_NET_HDR_S *hdr, void * sendBuf, int sendLen);

//获取参数
int nvr_net_get_param(int chnNo, IPNC_CMD_TYPE_E cmd, void* packet, int dataSize);
//设置参数
int nvr_net_set_param(int chnNo, IPNC_CMD_TYPE_E cmd, void* packet, int dataSize);

//唤醒线程
void nvr_pthread_signal(pthread_cond_t *pThrcond, pthread_mutex_t *pThrLock);
//线程阻塞
int nvr_pthread_timeout_wait(pthread_cond_t *pThrcond, pthread_mutex_t *pThrLock, int timeOut);

#endif

