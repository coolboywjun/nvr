
#ifndef __JXJ_CU_LOGON_H__
#define __JXJ_CU_LOGON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipnc_lib.h"
#include "j_sdk.h"

// 协议头
#define JPF_PROTO_HEAD_MAGIC		0x6a786a2d		/*"jxj-"*/

#define SET_PROTO_HEAD_M(head) \
	((head)->magic = htonl(JPF_PROTO_HEAD_MAGIC))
#define SET_PROTO_HEAD_S(head, seq) \
	(head)->seq = htonl(seq)
#define SET_PROTO_HEAD_L(head, len) \
	(head)->payload_len = htonl(len)
#define SET_PROTO_HEAD_P(head, packets) \
	(head)->tot_packets = packets
#define SET_PROTO_HEAD_N(head, no) \
	(head)->packet_no = no

#define VALID_PROTO_HEAD(head)	\
	(ntohl((head)->magic) == JPF_PROTO_HEAD_MAGIC)
#define GET_PROTO_HEAD_S(head) \
	ntohl((head)->seq)
#define GET_PROTO_HEAD_L(head) \
	ntohl((head)->payload_len)
#define GET_PROTO_PACKETS(head) \
	((head)->tot_packets)
#define GET_PROTO_PACKNO(head) \
	((head)->packet_no)

typedef struct jpf_proto_head
{
	uint32_t        magic;
	uint32_t   		seq;
	uint32_t        payload_len;
	uint8_t			tot_packets;
	uint8_t			packet_no;
	uint16_t		reserve;
}jpf_proto_head_t;

//报警回调函数
typedef void (*fAlarmCb)(long logonHandle, void* data, int dataSize);
typedef struct _JXJ_CU_LOGON_S_
{
	volatile int  quit;		//线程退出标记
	
	int 	sock;			//套接字
	int		sendPipe[2];	//发送管道
	int		recvPipe[2];	//接收管道

	int 	port;			//端口号
	char 	userName[32];	//用户名
	char 	passWord[32];	//密码
	int 	keepAliveTime;	//心跳包间隔
	
	char 	ipAddr[64];		//ip地址
	
	pthread_t pid;			//线程ID
	pthread_mutex_t lock;	//操作锁
	int		alarm;
	fAlarmCb cbAlarmFun;	//报警回调函数
	void *pUserData;		//用户数据 

	JIrcutControl ircutCfg;
	int recvIrcutCfg;
	JPictureInfo picCfg;
	int recvPicCfg;
	JPictureInfo defPicCfg;
	int recvDefPicCfg;
}JXJ_CU_LOGON_S;


/*BUG调试*/
#define DEBUG_CLIENT
#ifdef DEBUG_CLIENT
	#define __DBUG(fmt, args...) fprintf(stderr, "DEBUG(%s:%d):" fmt, __func__, __LINE__, ## args)
#else
	#define __DBUG(fmt, args...) 
#endif

#define __ERR(fmt, args...) fprintf(stderr, "ERROR(%s:%d):" fmt, __func__, __LINE__, ## args)

/*
	pIpAddr: ip地址.
	pUsr   : 用户名.
	pPwd   : 密码.
	pPort  : 端口.
	cbAlarmFun: 报警回调函数.
	pUserData:  用户参数.
	
成功返回分配空间句柄, 失败返回零.
*/
long jxj_cu_logon(const char* pIpAddr, const char* pUsr, 
					  const char* pPwd, const char* pPort, 
				      fAlarmCb cbAlarmFun, void* pUserData);

/*
	logonHandle: 登录句柄.
*/
int jxj_cu_logout(long logonHandle);

/*响应xml数据解析:
	chn: 通道号.
	type: 命令码.
	xmlData: xml数据.
	pCfg: 存放响应参数配置.
	cfgSize: 配置参数大小.
成功返回0, 失败返回-1.
*/
/*int jxj_cu_get_xml_parse(int chn, JParamId type, 
							 char *xmlData, void * pCfg, int cfgSize);*/
/*获取参数信息:
	logonHandle: 登录句柄.
	type: 命令码.
	pCfg: 存放响应参数配置.
	cfgSize: 配置参数大小.
成功返回0, 失败返回-1.
*/
int jxj_cu_cfg_get(long logonHandle, JParamId type, 
					   void * pCfg, int cfgSize);
/*设置参数信息:
	logonHandle: 登录句柄.
	type: 命令码.
	pCfg: 设置参数配置.
	cfgSize: 配置参数大小.
成功返回0, 失败返回-1.
*/
int jxj_cu_cfg_set(long logonHandle, JParamId type, 
					   void * pCfg, int cfgSize);

/*测试登录: 成功返回0, 失败返回-1;
*/
int jxj_cu_logon_test(const char* pDstIP, const char* pUsr, 
					  const char* pPwd, const char* pPrt);

#endif


