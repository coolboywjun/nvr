#ifndef __TI_NET_LOGON_H_
#define __TI_NET_LOGON_H_

#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

#include "net_api.h"
#include "ti_type.h"

#define TI_NET_LOGON_FLAG					0x12ab34cd
#define TI_NET_SERVER_PORT 					"6080"
#define TI_NET_KEEP_ALIVE_TIME				30
#define TI_NET_IPADDR_LEN					128
/*登录最大数*/
#define TI_NET_MAX_SUPPORT_LOGONE			256
#define TI_NET_MAX_SUPPORT_PREVIEW			256

//报警回调函数
typedef void (*fAlarmCb)(long logonHandle, void* data, int dataSize);

/*登录用户信息结构体*/
typedef struct _TI_LOGON_USER_INFO
{
	int 		logonSock;	//套接字
	int 		port;			//端口号
	char 	ipAddr[TI_NET_IPADDR_LEN];//ip地址
	char 	userName[32];	//用户名
	char 	passWord[32];	//密码
	TI_U32 	keepAliveTime;	//心跳包间隔
	TI_U32	packetNo;
	
	int		sendPipe[2];
	int		recvPipe[2];

	int		alarm;
	
	volatile int 		quit;
	pthread_t thid;
	fAlarmCb cbAlarmFun;	//报警回调函数
	void *pUserData;		//用户数据
}TI_LOGON_USER_INFO;



int ti_logon_pipe_session(TI_LOGON_USER_INFO *pLogonInfo, 
							const void *sendBuf, int sendLen, 
							char *recvBuf, int recvBufLen);

/*登录设备
	DesIP:目标IP地址;
	User:用户名;
	Password:密码;
	port:端口;
	keep_alive_time:心跳包间隔.
	alarm_funcb:警报回调处理函数
	成功返回用户信息句柄
	*/
long ti_net_logon(const char* DesIP, const char* User, const char* Password, const char* port, 
				  TI_U32 keep_alive_time, fAlarmCb cbAlarmFun, void *pUserData);
/*退出登录
	handle:用户信息句柄
	成功返回TI_SUCCESS
	*/
int ti_net_logout(long handle);

#endif

