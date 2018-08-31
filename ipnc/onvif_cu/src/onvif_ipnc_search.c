#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <string.h>
#include <error.h>
#include <linux/sockios.h>
#include <linux/if.h>  
#include <time.h>
#include <malloc.h>
#include <sys/unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/prctl.h>

#include "net_api.h"
#include "onvif_ipnc_search.h"
#include "onvif_api.h"

#define  ONVIF_NET_SEARCH_MAX_CLIENT_NUM 	20

typedef discover_info_t ONVIF_NET_SEARCH_RES_S;

typedef struct _ONVIF_NET_SEARCH_NODE_S_
{
	ONVIF_NET_SEARCH_RES_S searchRes;
	struct _ONVIF_NET_SEARCH_NODE_S_ *next;
}ONVIF_NET_SEARCH_NODE_S;

typedef struct _ONVIF_NET_SEARCH_USER_TABLE_S_
{
	long lHandle;
	unsigned int lastPos;//上次获取结束位置
	ONVIF_NET_SEARCH_NODE_S *pLastNode;//记录结束访问的节点
}ONVIF_NET_SEARCH_USER_TABLE_S;

typedef struct _ONVIF_NET_SEARCH_STRUCT_
{
	int Searching;
	volatile int RecvQuit;
	int nRecvSock;
	volatile unsigned int u32PacketNo;
	volatile int u32DevNum;//设备数
	
	ONVIF_NET_SEARCH_NODE_S *pSearchNodeHead;
	ONVIF_NET_SEARCH_NODE_S *pSearchNodeEnd;//尾节点
	
	volatile int userCnt;
	pthread_mutex_t userlock;
	ONVIF_NET_SEARCH_USER_TABLE_S userCT[ONVIF_NET_SEARCH_MAX_CLIENT_NUM];
	pthread_mutex_t lock;
	
}ONVIF_NET_SEARCH_STRUCT;

static ONVIF_NET_SEARCH_STRUCT gSearch = {0};

static int Onvif_Net_Add_User_Table(long lHandle)
{
	int i = 0;	
	//printf("add handle @@@@@@@@@\r\n");
	pthread_mutex_lock(&gSearch.userlock);
	if(ONVIF_NET_SEARCH_MAX_CLIENT_NUM <= gSearch.userCnt)
	{
		pthread_mutex_unlock(&gSearch.userlock);
		return -1;
	}
	for(; i < ONVIF_NET_SEARCH_MAX_CLIENT_NUM; i++)
	{
		if(0 == gSearch.userCT[i].lHandle)
		{
			//printf("add handle i=%d, 0x%0x\r\n", i, lHandle);
			gSearch.userCT[i].lHandle = lHandle;
			gSearch.userCnt++;

			pthread_mutex_unlock(&gSearch.userlock);
			return 0;
		}
	}
	
	pthread_mutex_unlock(&gSearch.userlock);
	return -1;
}

static ONVIF_NET_SEARCH_USER_TABLE_S* Onvif_Net_Get_User_Table(long lHandle)
{
	int i = 0;
	pthread_mutex_lock(&gSearch.userlock);
	if(gSearch.userCnt == 0)
	{
		pthread_mutex_unlock(&gSearch.userlock);
		return NULL;
	}
	for(; i < ONVIF_NET_SEARCH_MAX_CLIENT_NUM; i++)
	{
		if(lHandle == gSearch.userCT[i].lHandle)
		{
			pthread_mutex_unlock(&gSearch.userlock);
			return &gSearch.userCT[i];
		}
	}
	
	pthread_mutex_unlock(&gSearch.userlock);
	return NULL;
}

static void Onvif_Net_Del_User_Table(long lHandle)
{
	int i = 0;
	
	//printf("delete handle @@@@@@@@@\r\n");
	pthread_mutex_lock(&gSearch.userlock);
	for(; i < ONVIF_NET_SEARCH_MAX_CLIENT_NUM; i++)
	{
		if(lHandle == gSearch.userCT[i].lHandle)
		{
			gSearch.userCT[i].lHandle = 0;
			gSearch.userCT[i].lastPos = 0;
			gSearch.userCT[i].pLastNode = NULL;
			gSearch.userCnt--;
			
			//printf("delete handle i=%d, 0x%0x\r\n", i, lHandle);
			pthread_mutex_unlock(&gSearch.userlock);
			return;
		}
	}
	
	pthread_mutex_unlock(&gSearch.userlock);
}

static int Onvif_Net_Get_User_Cnt()
{
	int nUserCnt;
	pthread_mutex_lock(&gSearch.userlock);
	nUserCnt = gSearch.userCnt;
	pthread_mutex_unlock(&gSearch.userlock);
	return nUserCnt;
}

static int Onvif_Net_Search_List_Find(ONVIF_NET_SEARCH_RES_S *pResNode)
{
	ONVIF_NET_SEARCH_NODE_S *pResNodeTmp = NULL;
	
	if(gSearch.pSearchNodeHead == NULL)
		return -1;

	pResNodeTmp = gSearch.pSearchNodeHead;
	while(NULL != pResNodeTmp)
	{
		//ip与uuid 相同
		if ((strcmp(pResNode->devIP, pResNodeTmp->searchRes.devIP) == 0)
			&&(strcmp(pResNode->EPAddr, pResNodeTmp->searchRes.EPAddr) == 0))
			return 0;
		pResNodeTmp = pResNodeTmp->next;
	}

	return -1;
}

static void Onvif_Net_Search_Add_To_List(ONVIF_NET_SEARCH_NODE_S *pResNode)
{
	//尾部增加
	if(gSearch.pSearchNodeHead == NULL)
	{
		gSearch.pSearchNodeHead = pResNode;
		pResNode->next = NULL;
		gSearch.pSearchNodeEnd = gSearch.pSearchNodeHead;//尾节点
	}
	else
	{
		gSearch.pSearchNodeEnd->next = pResNode;
		pResNode->next = NULL;
		gSearch.pSearchNodeEnd = pResNode;//尾节点
	}
	gSearch.u32DevNum++;
}

static void Onvif_Net_Search_List_Free()
{
	ONVIF_NET_SEARCH_NODE_S *pResNode;
	ONVIF_NET_SEARCH_NODE_S *pTmpResNode;
	pResNode = gSearch.pSearchNodeHead;
	while(pResNode != NULL)
	{
		pTmpResNode = pResNode->next;
		free(pResNode);
		pResNode = pTmpResNode;
	}
	gSearch.pSearchNodeHead = NULL;
	gSearch.pSearchNodeEnd = NULL;
	gSearch.u32DevNum = 0;
}

int Onvif_Net_Get_Search(long lHandle, IPNC_NET_SEARCH_RES_S *pDev)
{
	ONVIF_NET_SEARCH_NODE_S *pResNode;
	ONVIF_NET_SEARCH_USER_TABLE_S* pUserTb = NULL;
	char *pId = NULL;
	
	pthread_mutex_lock(&gSearch.lock);
	//获得用户信息
	pUserTb = Onvif_Net_Get_User_Table(lHandle);
	if(NULL == pUserTb)
	{
		//printf("onvif user table is null. \n");
		pthread_mutex_unlock(&gSearch.lock);
		return -1;
	}
	if(gSearch.pSearchNodeHead == NULL)
	{
		pthread_mutex_unlock(&gSearch.lock);
		return 0;
	}
	if(pUserTb->pLastNode == NULL)
		pResNode = gSearch.pSearchNodeHead;
	else
		pResNode = pUserTb->pLastNode->next;
	if(pResNode == NULL)
	{
		pthread_mutex_unlock(&gSearch.lock);
		return 0;
	}

	memset(pDev, 0, sizeof(IPNC_NET_SEARCH_RES_S));
	pDev->u32DevIp = net_ip_a2n(pResNode->searchRes.devIP);
	pDev->u32SoftVer = 0;
	pDev->u16DevPort = atoi(pResNode->searchRes.port);
	pDev->u32DevGwAddr = net_ip_a2n("0.0.0.0");
	pId = strrchr(pResNode->searchRes.EPAddr, '-');
	if (pId == NULL)
		pId = pResNode->searchRes.EPAddr;
	else
		pId++;
	strncpy((char *)pDev->u8SerialId, pId, sizeof(pDev->u8SerialId) - 1);
	pUserTb->pLastNode = pResNode;
	pthread_mutex_unlock(&gSearch.lock);
	
	return 1;
}

static void cbSearchFunc(int nIndex, discover_info_t *pDev)
{	
	ONVIF_NET_SEARCH_NODE_S *pSearchNode;
	if (pDev == NULL) 
		return;
	
	if(Onvif_Net_Get_User_Cnt() <= 0)
		return;
	
	if(0 == Onvif_Net_Search_List_Find(pDev))
		return;

	pSearchNode = (ONVIF_NET_SEARCH_NODE_S*)malloc(sizeof(ONVIF_NET_SEARCH_NODE_S));
	if(pSearchNode == NULL)
		return;
	
	memcpy(&pSearchNode->searchRes, pDev, sizeof(*pDev));
	pSearchNode->next = NULL;
	pthread_mutex_lock(&gSearch.lock);
	Onvif_Net_Search_Add_To_List(pSearchNode);
	pthread_mutex_unlock(&gSearch.lock);
	
	return;
	
}

static void *__SearchThreadFunc(void *data)
{	
	prctl(PR_SET_NAME, "OnvifSearch");
	nvr_onvif_discover(4, cbSearchFunc);	
	gSearch.Searching = 0;	
	//printf("@@@@@@@@@@@@@@@@@search stop\r\n");
	return NULL;
}

int Onvif_Net_Search_Do(long lHandle)
{
	if (gSearch.Searching == 0)
	{		
		pthread_t searchThrId;
		pthread_mutex_lock(&gSearch.lock);		
		if(Onvif_Net_Get_User_Cnt() > 0)
		{
			if(Onvif_Net_Add_User_Table(lHandle) < 0)
			{
				pthread_mutex_unlock(&gSearch.lock);
				return -1;
			}
			pthread_mutex_unlock(&gSearch.lock);
			return 0;
		}
		else
		{
			Onvif_Net_Search_List_Free();
		}

		if(0 > Onvif_Net_Add_User_Table(lHandle))
		{
			pthread_mutex_unlock(&gSearch.lock);
			return -1;
		}
		pthread_mutex_unlock(&gSearch.lock);		
		gSearch.Searching = 1;		
		pthread_create(&searchThrId, NULL, __SearchThreadFunc, NULL);
	}
	return 0;
	
}

int Onvif_Net_Search_Done(long lHandle)
{
	//删除用户信息
	pthread_mutex_lock(&gSearch.lock);
	Onvif_Net_Del_User_Table(lHandle);
	if(Onvif_Net_Get_User_Cnt() <= 0)
	{
		Onvif_Net_Search_List_Free();
		gSearch.u32PacketNo = ~0;
	}
	pthread_mutex_unlock(&gSearch.lock);
	
	return 0;
}

int Onvif_Net_Get_Search_DevNum()
{
	int nDevNum;
	pthread_mutex_unlock(&gSearch.lock);
	nDevNum = gSearch.u32DevNum;
	pthread_mutex_unlock(&gSearch.lock);
	
	return nDevNum;
}


/* 搜索初始化 */
int Onvif_Net_Search_init()
{
	memset(&gSearch, 0, sizeof(ONVIF_NET_SEARCH_STRUCT));
	pthread_mutex_init(&gSearch.lock, NULL);
	pthread_mutex_init(&gSearch.userlock, NULL);
	gSearch.u32PacketNo = ~0;
	
	return 0;
}

int Onvif_Net_Search_uninit()
{
	gSearch.RecvQuit = 1;
	Onvif_Net_Search_List_Free();
	pthread_mutex_destroy(&gSearch.lock);
	pthread_mutex_destroy(&gSearch.userlock);
	return 0;
}

