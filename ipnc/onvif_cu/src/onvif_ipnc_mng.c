
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <net_api.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>

#include "ipnc_lib.h"
#include "onvif_ipnc_realplay.h"
#include "onvif_ipnc_search.h"
#include "onvif_ipnc_mng.h"


static ONVIF_IPNC_PROTOCOL_MNG *gMngHead = NULL;
static pthread_mutex_t gListMutex = PTHREAD_MUTEX_INITIALIZER;

static void onvif_ipnc_mng_add_to_list(ONVIF_IPNC_PROTOCOL_MNG *pMng)
{
	pthread_mutex_lock(&gListMutex);
	if(gMngHead == NULL)
	{
		gMngHead = pMng;
		pMng->next = NULL;
	}
	else
	{
		pMng->next = gMngHead;
		gMngHead = pMng;
	}
	pthread_mutex_unlock(&gListMutex);
}

static void onvif_ipnc_mng_del_from_list(ONVIF_IPNC_PROTOCOL_MNG *pMng)
{
	if(pMng == NULL)
			return;
	pthread_mutex_lock(&gListMutex);
	if(pMng == gMngHead)
	{
		gMngHead = gMngHead->next;
	}
	else
	{
		ONVIF_IPNC_PROTOCOL_MNG *node = gMngHead;
		while(node->next != NULL)
		{
			if(node->next == pMng)
			{
				node->next = pMng->next;
				break;
			}
			node = node->next;
		}
	}
	pthread_mutex_unlock(&gListMutex);

}

void onvif_ipnc_mng_print_list()
{
	int i;
	ONVIF_IPNC_PROTOCOL_MNG *node = gMngHead;
	printf("The ti ipnc information ===>\n");
	pthread_mutex_lock(&gListMutex);
	while(node != NULL)
	{
		printf("ipncHandle: %ld(%p)\n", (long)node, node);
		printf("\tIP address: %s\n", node->ipAddr);
		printf("\tPort: %d\n", node->port);
		printf("\tUser: %s, Password: %s\n", node->user, node->passwd);
		printf("\tRefence count: %d\n", node->refCount);
		for(i = 0; i < 2; i++)
		{
			ONVIF_IPNC_STREAM_S *pStream = (ONVIF_IPNC_STREAM_S*)node->stream[i];
			if(pStream != NULL)
			{
				printf("\tStream handle 0: %ld(%p)\n", (long)pStream, pStream);
				printf("\t\tChannel: %d, Stream: %d\n", pStream->chn, pStream->streamNo);
				printf("\t\tPool Channel: %d, Stream: %d\n", pStream->poolChn, pStream->poolStreamNo);
				printf("\t\tReal play handle: %ld\n", pStream->lRealPlayHandle);
				printf("\t\tVideo id: %ld\n", pStream->videoId);
				printf("\t\tRefence count: %d\n", pStream->refCount);
			}
		}
		node = node->next;
	}
	pthread_mutex_unlock(&gListMutex);
}

int onvif_ipnc_mng_check_handle_valid(ONVIF_IPNC_PROTOCOL_MNG *pMng)
{
	ONVIF_IPNC_PROTOCOL_MNG *node = gMngHead;
	if(pMng == NULL)
		return 0;
	pthread_mutex_lock(&gListMutex);
	while(node != NULL)
	{
		if(node == pMng)
		{
			pthread_mutex_unlock(&gListMutex);
			return 1;
		}
		node = node->next;
	}
	pthread_mutex_unlock(&gListMutex);
	return 0;
}

int onvif_ipnc_mng_check_ipnc_handle_valid(ONVIF_IPNC_PROTOCOL_MNG *pMng, ONVIF_IPNC_STREAM_S *pStream)
{
	ONVIF_IPNC_PROTOCOL_MNG *node = gMngHead;
	if(pMng == NULL || pStream == NULL)
		return 0;
	pthread_mutex_lock(&gListMutex);
	while(node != NULL)
	{
		if(node == pMng && (pMng->stream[0] == pStream || pMng->stream[1] == pStream))
		{
			pthread_mutex_unlock(&gListMutex);
			return 1;
		}
		node = node->next;
	}
	pthread_mutex_unlock(&gListMutex);
	return 0;
}

void onvif_ipnc_mng_ref(ONVIF_IPNC_PROTOCOL_MNG *pMng)
{
	pMng->refCount++;
}

void onvif_ipnc_mng_unref(ONVIF_IPNC_PROTOCOL_MNG *pMng)
{
	if(--pMng->refCount == 0)
	{
		if(pMng->stream[0] != NULL || pMng->stream[1] != NULL)
		{
			printf("Warning: We should close resource of stream handle before closing ipnc handle.\n");
			if(pMng->stream[0] != NULL)
				onvif_ipnc_stop_get_stream((long)pMng, (long)pMng->stream[0]);
			if(pMng->stream[1] != NULL)
				onvif_ipnc_stop_get_stream((long)pMng, (long)pMng->stream[1]);
		}
		onvif_ipnc_mng_del_from_list(pMng);
		free(pMng);
	}
}

int onvif_ipnc_connect(int poolCh, const char *ipAddr, int port, const char *user, const char *passwd, long *ipncHandle)
{
	int sock;
	char strPort[32];
	ONVIF_IPNC_PROTOCOL_MNG *pMng;
	ONVIF_IPNC_PROTOCOL_MNG *node = gMngHead;

	if(ipncHandle == NULL)
	{
		printf("The ipnc handle of ipnc connect is null.\n");
		return -1;
	}
	
	// 先判断链表中是否针对ipnc分配了结构
	pthread_mutex_lock(&gListMutex);
	while(node != NULL)
	{
		if(node->poolCh == poolCh
			&& (strcmp(node->ipAddr, ipAddr) == 0)
			&& (node->port == port)
			&& (strcmp(node->user, user) == 0)
			&& (strcmp(node->passwd, passwd) == 0))
		{
			onvif_ipnc_mng_ref(node);
			*ipncHandle = (long)node;
			pthread_mutex_unlock(&gListMutex);
			return 0;
		}
		node = node->next;
	}
	pthread_mutex_unlock(&gListMutex);

	// 连接远方IPNC端口,判断是否可以正常通信
	// 判断完成后需要关闭socket
	sprintf(strPort, "%d", port);
	if((sock = net_tcp_noblock_connect(NULL, NULL, ipAddr, strPort, 100)) < 0)
	{
		printf("Can't connect to the remote ipnc device.\n");
		return -1;
	}
	net_close_socket(&sock);
	// 如果没有分配,则分配结构并初始化
	pMng = (ONVIF_IPNC_PROTOCOL_MNG *)malloc(sizeof(ONVIF_IPNC_PROTOCOL_MNG));
	if(pMng == NULL)
	{
		printf("Malloc for ONVIF_IPNC_PROTOCOL_MNG failed.\n");
		return -1;
	}
	memset(pMng, 0, sizeof(ONVIF_IPNC_PROTOCOL_MNG));
	strcpy(pMng->ipAddr, ipAddr);
	pMng->port = port;
	strcpy(pMng->user, user);
	strcpy(pMng->passwd, passwd);
	pMng->poolCh = poolCh;
	pMng->refCount = 1;
	
	*ipncHandle = (long)pMng;
	onvif_ipnc_mng_add_to_list(pMng);
	return 0;
}

int onvif_ipnc_disconnect(long handle)
{
	ONVIF_IPNC_PROTOCOL_MNG *pMng = (ONVIF_IPNC_PROTOCOL_MNG *)handle;
	if(!onvif_ipnc_mng_check_handle_valid(pMng))
		return -1;
	onvif_ipnc_mng_unref(pMng);
	return 0;
}

int onvif_ipnc_search_cfg(IPNC_NET_SEARCH_CFG_REQ_S* pIpncSearchCfg)
{
	return 0;
}

int onvif_ipnc_start_search(long lHandle)
{
	//printf("onvif search start@@@@@@@@@@@@@\r\n");	
	return Onvif_Net_Search_Do(lHandle);
}

int onvif_ipnc_stop_search(long lHandle)
{	
	//printf("onvif search stop@@@@@@@@@@@@@\r\n");
	return Onvif_Net_Search_Done(lHandle);
}

int onvif_ipnc_get_search(long lHandle, IPNC_NET_SEARCH_RES_S *pDev)
{	
	return Onvif_Net_Get_Search(lHandle, pDev);
}

int onvif_ipnc_test_ipc(const char* DesIP, const char* User, 
					 const char* Password, const char* port)
{
	return 0;
}

