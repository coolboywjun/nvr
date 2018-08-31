#ifndef _NVR_IPC_API_H
#define _NVR_IPC_API_H
/*
* Copyright (c) 2010
* All rights reserved.
* 
* filename:nvr_ipc_api.h
* 
* func:本文件实现了对linux 下的Posix消息队列的封装
* 
* author : hi
* createdate: 2010-10-10
*
* modify list: 
* 
*/
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct _NVR_MSG_S
{
	unsigned long ulMsgType; /* 消息类型，必须 > 0 */
	char          szMsg[1024];/* 消息文本 */
}NVR_MSG_S, *LPNVR_MSG_S;

typedef enum _IPC_ERR_E
{
	NVR_IPC_SUCCESS       = 0,
	NVR_IPC_INIT_FTOK_ERR = -1,
	NVR_IPC_MSG_Get_ERR   = -2,
}IPC_ERR_E;
//=================================================================
//函数: nvr_ipc_msg_init 
//
//功能: 初始化一个消息队列
//
//参数:nId 输入:初始化消息队列时用的一个子序号
//				(不同的消息对列应该用不同的子序号)
//
//
//返回: 成功返回返回消息队列标识值 > 0 失败返回 _IPC_ERR_E中的错误码			
//=================================================================
int nvr_ipc_msg_init(int nId);

int nvr_ipc_msg_init2(char *pName, int nId);

//=================================================================
//函数: nvr_ipc_msg_init 
//
//功能: 发送一个消息
//
//参数:nMsqId  输入:消息队列标识值
//	   lpStMsg 输入:消息体 		   
//	   nWait   输入:0 消息队列满时不需要阻塞 1 消息队列满时阻塞 
//
//返回: 成功返回NVR_IPC_SUCCESS	失败返回-1		
//=================================================================
int nvr_ipc_msg_send(int nMsqId, LPNVR_MSG_S lpStMsg, int nMsgBodyLen,int nWait);

//=================================================================
//函数: nvr_ipc_msg_recv 
//
//功能: 发送一个消息
//
//参数:nMsqId  输入:消息队列标识值
//	   lpStMsg 输入:消息体 		   
//	   nWait   输入:0 消息队列呈空时，不做等待马上返回-1
//					1 在队列呈空的情形时，采取阻塞等待
//
//返回: 成功返回szMsg中实际数据长度 失败返回-1	
//=================================================================
int nvr_ipc_msg_recv(int nMsgId, LPNVR_MSG_S lpStMsg, int nMsgBodyLen, int nWait);

//=================================================================
//函数: nvr_ipc_clear_msg 
//
//功能: 清空消息队列
//
//参数:nMsqId  输入:消息队列标识值
//	   lpStMsg 输入:只需填进一个NVR_MSG_S缓存
//
//返回: 成功返回NVR_IPC_SUCCESS 失败返回-1	
//=================================================================
int nvr_ipc_clear_msg(int nMsgId, LPNVR_MSG_S lpStMsg, int nMsgBodyLen);

//=================================================================
//函数: nvr_ipc_del_msq 
//
//功能: 删除消息队列
//
//参数:nMsqId  输入:消息队列标识值
//
//返回: 成功返回NVR_IPC_SUCCESS 失败返回-1	
//=================================================================
int nvr_ipc_del_msq(int nMsqId);

//=================================================================
//函数: nvr_fifo_init 
//
//功能: 建立命名管道
//
//参数:name  命名管道路径
//
//返回: 成功返回NVR_IPC_SUCCESS 失败返回-1	
//=================================================================
int nvr_fifo_init(char *name);

//=================================================================
//函数: nvr_fifo_init 
//
//功能: 删除命名管道
//
//参数:name  命名管道路径
//
//返回: 成功返回NVR_IPC_SUCCESS 失败返回-1	
//=================================================================
int nvr_del_fifo(char *name);
//=================================================================
//函数: nvr_fifo_init 
//
//功能: 通过pthread_mutex_t pthread_cond_t 来模拟事件等待
//
//参数:nTimeOut  :等待超时时间
//
//返回: 成功返回NVR_IPC_SUCCESS 失败返回-1	
//=================================================================
int nvr_pthread_wait(pthread_mutex_t *pMutex, pthread_cond_t *pCond, int nTimeOut);

#ifdef __cplusplus
}
#endif 

#endif

