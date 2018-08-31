#ifndef __NVR_LIB_COMMON_H__
#define __NVR_LIB_COMMON_H__

#include <nvr_net_interface.h>
#include <nvr_net_struct.h>


#define SERVER_LIB_DEBUG
#if defined(__DEBUG__) && defined(SERVER_LIB_DEBUG)
#define __DBUG(fmt, args...)		\
do{	\
printf(fmt, ##args);		\
syslog(LOG_DEBUG | LOG_USER, fmt, ## args);		\
}while(0)
#else
#define __DBUG(fmt, args...)
#endif

#define __ERR(fmt, args...)	\
do{	\
printf(fmt, ##args);		\
syslog(LOG_ERR | LOG_USER, fmt, ## args);		\
}while(0)

#define __LOG(priority, fmt, args...)	\
do{	\
printf(fmt, ##args);		\
syslog(priority | LOG_USER, fmt, ## args);		\
}while(0)


#ifdef __cplusplus
extern "C" {
#endif

void nvr_make_net_hdr(NVR_NET_HDR_S *hdr, int chn, int cmdType, int packSize, int errNo);

int nvr_recv_net_packet(int sock, NVR_NET_HDR_S *hdr, void* recvBuf, int bufLen);
	
int nvr_send_net_packet(int sock, NVR_NET_HDR_S *hdr, void* sendBuf, int sendLen);

int nvr_send_cmd(void *recvBuf, int recvBufSize, void *sendBuf , int sendBufSize);


#ifdef __cplusplus
}
#endif

#endif

