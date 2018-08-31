/******************************************************************************

                  版权所有 (C), 2011-2015, 佳信捷电子

 ******************************************************************************
  文 件 名   : nvr_ipc_api.c
  版 本 号   : v2.0
  作    者   : 梁锦明
  生成日期   : 2011年9月8日
  最近修改   :
  功能描述   : 进程间通讯 消息和fifo的封装
  函数列表   :
              nvr_del_fifo
              nvr_fifo_init
              nvr_ipc_clear_msg
              nvr_ipc_del_msq
              nvr_ipc_msg_init
              nvr_ipc_msg_init2
              nvr_ipc_msg_recv
              nvr_ipc_msg_send
              nvr_pthread_wait
  修改历史   :
  1.日    期   : 2011年9月8日
    作    者   : 梁锦明
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "nvr_ipc_api.h"
#include <sys/time.h>
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 函数实现                                     *
 *----------------------------------------------*/
int nvr_ipc_msg_init2(char *pName, int nId)
{
	int	    nMsqId  = 0;
	key_t	nKeyMsg = 0;

	/* 消息队列初始化 */
	if ((nKeyMsg = ftok(pName, nId)) == -1)
	{
		return NVR_IPC_INIT_FTOK_ERR;
	}

	if ((nMsqId  = msgget(nKeyMsg, IPC_CREAT | 0666)) == -1)
	{
		return NVR_IPC_MSG_Get_ERR;
	}
	return nMsqId;	 
}


int nvr_ipc_msg_init(int nId)
{
	int	    nMsqId  = 0;
	key_t	nKeyMsg = 0;

	/* 消息队列初始化 */
	if ((nKeyMsg = ftok(".", nId)) == -1)
	{
		return NVR_IPC_INIT_FTOK_ERR;
	}

	if ((nMsqId  = msgget(nKeyMsg, IPC_CREAT | 0666)) == -1)
	{
		return NVR_IPC_MSG_Get_ERR;
	}
	return nMsqId;	 
}

int nvr_ipc_msg_send(int nMsqId, LPNVR_MSG_S lpStMsg, int nMsgBodyLen, int nWait)
{
	int nRet = 0;

	nRet = msgsnd(nMsqId, lpStMsg, nMsgBodyLen,  nWait ? 0 : IPC_NOWAIT);
	return nRet;	
}

int nvr_ipc_msg_recv(int nMsqId, LPNVR_MSG_S lpStMsg, int nMsgBodyLen, int nWait)
{
	int nRet = 0;

	nRet = msgrcv(nMsqId, lpStMsg, nMsgBodyLen, 0,  nWait ? 0 : IPC_NOWAIT);
	return nRet;
}

int nvr_ipc_clear_msg(int nMsqId, LPNVR_MSG_S lpStMsg, int nMsgBodyLen)
{
	int nCount = 0;
	int nRet   = 0;
	
    for (nCount = 0; nCount < 1024; nCount++)
    {
        if ((nRet = msgrcv(nMsqId, lpStMsg, nMsgBodyLen, 0, IPC_NOWAIT)) < 1)
        {
            break;
        }
    }
	return 0;
}

int nvr_ipc_del_msq(int nMsqId)
{
	if (msgctl(nMsqId, IPC_RMID, 0) == -1)
	{
		return(-1);
	}
	return(0);
}

int nvr_fifo_init(char *name)
{
	return mkfifo(name, 0777);
}

int nvr_del_fifo(char *name)
{
	return unlink(name);
}

int nvr_pthread_wait(pthread_mutex_t *pMutex, pthread_cond_t *pCond, int nTimeOut)
{
	struct timeval now;
	struct timespec timeout;

	if (nTimeOut != 0)
	{
		gettimeofday(&now, NULL);
		timeout.tv_sec  = now.tv_sec + nTimeOut/1000;
		timeout.tv_nsec= (now.tv_usec + (nTimeOut%1000) * 1000) * 1000;
	}	

    	pthread_mutex_lock(pMutex);
	if (nTimeOut == 0)
		pthread_cond_wait(pCond, pMutex);
	else
    		pthread_cond_timedwait(pCond, pMutex, &timeout);
    	pthread_mutex_unlock(pMutex);

	return NVR_IPC_SUCCESS;
}

