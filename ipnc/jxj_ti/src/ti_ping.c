#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include "net_api.h"
#include "ti_ping.h"


// ICMP header  
typedef struct _ihdr
{
	unsigned char   i_type; 
	unsigned char   i_code;	// type sub code
	unsigned short  i_cksum; 
	unsigned short  i_id; 
	unsigned short  i_seq; 
	// This is not the std header, but we reserve space for time
	unsigned long   timestamp; 
}IcmpHeader;

typedef struct stJMsgHeader
{
	unsigned int    uMagic;
	unsigned int    uType;
	unsigned int    ret;
	unsigned char   dat;
}PSTJMsgHeader;

#define PSTJ_PING_CMD 1000
#define IPC_PING_PORT 33080

/*
主要是查询ipc是否在线.
	成功返回0
*/
int ti_ipc_status(char *ip)
{
	int ret = 0;
	fd_set readfds;
	struct timeval time_out;
	unsigned int sock;
	PSTJMsgHeader recvBuf;
	PSTJMsgHeader sendBuf;
   	
	//套接字地址结构体
	struct sockaddr_in client_addr = {0};
	
	//1.创建套接字
	sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(0  >  sock)
	{
		perror("socket failed:");	
		return -1;	
	}
	
	//2.建立套接字地址	
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(IPC_PING_PORT);
	client_addr.sin_addr.s_addr = inet_addr(ip);
	
	//3.发送数据
	memset(&sendBuf, 0, sizeof(PSTJMsgHeader));
	sendBuf.uMagic = IPC_PING_PORT;
	sendBuf.uType = PSTJ_PING_CMD;

	//printf("ip: %s ", ip);
	ret = net_udp_send(sock, &sendBuf,sizeof(PSTJMsgHeader),(struct sockaddr *)&client_addr);
	if (sizeof(PSTJMsgHeader) != ret){
		perror("sendto failed:");
		goto _FAILED;
	}
	
	memset(&recvBuf, 0, sizeof(PSTJMsgHeader));
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);
	time_out.tv_sec=0;
	time_out.tv_usec=400*1000;
	if (select(sock + 1, &readfds, NULL, NULL, &time_out) > 0)
	{
		//printf("net_udp_recv start...\n");
		if(ret != net_udp_recv(sock, &recvBuf,sizeof(PSTJMsgHeader), ret, NULL, NULL))//出错
		{
			perror("recvfrom failed:");
			goto _FAILED;
		}
	}
	else
	{
		//printf("net_udp_recv timeout\n");
		goto _FAILED;
	}

	//4.关闭套接字
	close(sock);
	return 0;

_FAILED:
	close(sock);
    return -1;
}

/*int main(int argc, char *argv[])
{
	char *ip = NULL;
	if(argv[1] == NULL)
		ip = "172.16.4.250";
	else
		ip = argv[1];
	printf("ip:%s\n", ip);
	
	if(0 != ti_ipc_status(ip))
		printf("failed...\n");
	else
		printf("OK\n");
	return 0;
}*/
