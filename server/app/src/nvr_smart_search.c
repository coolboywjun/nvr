
#include <nvr_server_headers.h>
#include <nvr_param_proc.h>
#include <ipnc_lib_client.h>
#include <nvr_net_search.h>
#include <nvr_arp.h>


#define NVR_SMART_SEARCH_PORT	17185
#define NVR_SMART_MAGIC		('S' | ('M'<<8) | ('R'<<16) | ('T'<<24))

typedef struct _NVR_SMART_IPC_INFO_
{
	uint32_t 		u32DevIp;
	uint8_t		szIpcMac[NVR_MAC_ADDR_LEN];
	uint8_t 		u8Res[2];
}NVR_SMART_IPC_INFO;

typedef struct _NVR_SMART_INFO_
{
	uint32_t		u32DevIp;
	uint8_t		szDevId[NVR_SERIALNO_LEN];

	int			nElapseTime;
	GList		*pIpcInfoList;
}NVR_SMART_INFO;

typedef enum _NVR_SMART_CMD_TYPE_
{
	SMART_HEARTBEAT = 0,
	SMART_SHUTDOWN,
	SMART_CHANNEL_INFO,
	SMART_QUERY_INFO,
}NVR_SMART_CMD_TYPE;

typedef struct _NVR_SMART_NET_HEAD_
{
	uint32_t	magic;
	uint8_t 	type;
	uint16_t 	len;
	uint8_t	devid[NVR_SERIALNO_LEN];
	uint8_t 	data[0];
}NVR_SMART_NET_HEAD;

static GList *gNvrInfoList =  NULL;
static uint8_t gMyDevId[NVR_SERIALNO_LEN];
static gint gRecvInfo = 0;
static gint gSmartQuit = 0;
static GThread *gSmartThread;

static gint64 gLastTime = 0;

G_LOCK_DEFINE_STATIC(smart);

static NVR_SMART_INFO *nvr_smart_find_nvr_info(const uint8_t *pDevId)
{
	GList *list;
	NVR_SMART_INFO *pSmartNvrInfo;

	for(list = g_list_first(gNvrInfoList); list != NULL; list = g_list_next(list))
	{
		pSmartNvrInfo = (NVR_SMART_INFO *)list->data;
		if(memcmp(pSmartNvrInfo->szDevId, pDevId, NVR_SERIALNO_LEN) == 0)
			return pSmartNvrInfo;
	}

	return NULL;
}

static NVR_SMART_IPC_INFO *nvr_smart_find_ipc_info(uint32_t u32DevIp)
{
	GList *pNvrList, *pIpcList;
	NVR_SMART_INFO *pSmartInfo;
	NVR_SMART_IPC_INFO *pSmartIpcInfo;

	for(pNvrList = g_list_first(gNvrInfoList); pNvrList != NULL; pNvrList = g_list_next(pNvrList))
	{
		pSmartInfo = (NVR_SMART_INFO *)pNvrList->data;
		for(pIpcList = g_list_first(pSmartInfo->pIpcInfoList); pIpcList != NULL; pIpcList = g_list_next(pIpcList))
		{
			pSmartIpcInfo = (NVR_SMART_IPC_INFO *)pIpcList->data;
			if(pSmartIpcInfo->u32DevIp == u32DevIp)
				return pSmartIpcInfo;
		}
	}

	return NULL;
}

static NVR_SMART_INFO* nvr_smart_add_nvr_info(const uint8_t *pDevId, uint32_t u32DevIp)
{
	NVR_SMART_INFO *pSmartNvrInfo;

	pSmartNvrInfo = g_malloc0(sizeof(NVR_SMART_INFO));
	pSmartNvrInfo->u32DevIp = u32DevIp;
	memcpy(pSmartNvrInfo->szDevId, pDevId, NVR_SERIALNO_LEN);

	gNvrInfoList = g_list_prepend(gNvrInfoList, pSmartNvrInfo);

	return pSmartNvrInfo;
}

static void nvr_smart_del_nvr_info(const uint8_t *pDevId)
{
	NVR_SMART_INFO *pSmartNvrInfo;

	pSmartNvrInfo = nvr_smart_find_nvr_info(pDevId);
	if(pSmartNvrInfo == NULL)
		return;
	g_list_free_full(pSmartNvrInfo->pIpcInfoList, g_free);
	gNvrInfoList = g_list_remove(gNvrInfoList, pSmartNvrInfo);
	g_free(pSmartNvrInfo);
}

static void nvr_smart_add_ipc_info(NVR_SMART_INFO *pSmartNvrInfo, const uint8_t *pIpcMac, uint32_t u32DevIp)
{
	NVR_SMART_IPC_INFO *pSmartIpcInfo;

	pSmartIpcInfo = (NVR_SMART_IPC_INFO *)g_malloc0(sizeof(NVR_SMART_IPC_INFO));
	pSmartIpcInfo->u32DevIp = u32DevIp;
	memcpy(pSmartIpcInfo->szIpcMac, pIpcMac, NVR_MAC_ADDR_LEN);
	pSmartNvrInfo->pIpcInfoList = g_list_prepend(pSmartNvrInfo->pIpcInfoList, pSmartIpcInfo);
}

static int nvr_smart_recv_heartbeat(uint8_t *pRecvData, uint32_t u32DevIp)
{	
	int i;
	NVR_SMART_NET_HEAD *pSmartHead = (NVR_SMART_NET_HEAD *)pRecvData;
	NVR_SMART_INFO *pSmartNvrInfo;
	NVR_SMART_IPC_INFO *pSmartIpcInfo;
		
	G_LOCK(smart);
	pSmartNvrInfo = nvr_smart_find_nvr_info(pSmartHead->devid);
	if(pSmartNvrInfo)
	{
		pSmartNvrInfo->u32DevIp = u32DevIp;
		pSmartNvrInfo->nElapseTime = 0;
		/* clear and add new ipc info */
		g_list_free_full(pSmartNvrInfo->pIpcInfoList, g_free);
		pSmartNvrInfo->pIpcInfoList = NULL;
	}
	else
		pSmartNvrInfo = nvr_smart_add_nvr_info(pSmartHead->devid, u32DevIp);
	if(pSmartNvrInfo == NULL)
	{
		G_UNLOCK(smart);
		return -1;
	}
	
	pSmartIpcInfo = (NVR_SMART_IPC_INFO *)pSmartHead->data;
	//j_debug("recv smart lenght: %d", pSmartHead->len);
	for(i = 0; i <pSmartHead->len/sizeof(NVR_SMART_IPC_INFO); i++)
	{
		nvr_smart_add_ipc_info(pSmartNvrInfo, pSmartIpcInfo->szIpcMac, pSmartIpcInfo->u32DevIp);
		pSmartIpcInfo++;
	}
	G_UNLOCK(smart);

	return 0;
}

static int nvr_smart_recv_shutdown(uint8_t *pRecvData)
{
	NVR_SMART_NET_HEAD *pSmartHead = (NVR_SMART_NET_HEAD *)pRecvData;

	G_LOCK(smart);
	nvr_smart_del_nvr_info(pSmartHead->devid);
	G_UNLOCK(smart);

	return 0;
}

/*
static int nvr_smart_get_mac(const uint32_t u32DevIp, uint8_t *pIpcMac)
{
	int i;
	FILE *fp;
	char szLineBuf[256];
	char szIp[32];
	char szMac[32];
	unsigned int mac[6];

	fp = fopen("/proc/net/arp", "r");
	if(fp == NULL)
	{
		j_warning("Open /proc/net/arp file failed.");
		return -1;
	}
	// ignore first line
	fgets(szLineBuf, sizeof(szLineBuf), fp);
	while(fgets(szLineBuf, sizeof(szLineBuf), fp) != NULL)
	{
		memset(szIp, 0, sizeof(szIp));
		if(sscanf(szLineBuf, "%s %*s %*s %s", szIp, szMac) != 2)
			continue;
		//j_debug("ip: %s, mac: %s", szIp, szMac);
		if(net_ip_a2n(szIp) == u32DevIp)
		{
			fclose(fp);
			if(sscanf(szMac, "%02x:%02x:%02x:%02x:%02x:%02x", 
				&mac[0], &mac[1], &mac[2],
				&mac[3], &mac[4], &mac[5]) != 6)
			{
				j_warning("Get %s mac address failed.", szIp);
				return -1;
			}
			for(i = 0; i < NVR_MAC_ADDR_LEN; i++)
				pIpcMac[i] = mac[i] & 0xff;
			return 0;
		}
	}
	
	fclose(fp);

	return -1;
}
*/
static int nvr_smart_send(void *pSendData, int nSendLen, uint32_t u32DstIp)
{
	int nSock;
	struct sockaddr_in sockAddr;

	nSock = net_create_sock(2);
	if(nSock <0)
	{
		j_warning("Create smart send socket failed.");
		return -1;
	}
		
	bzero(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;  
	sockAddr.sin_port = htons(NVR_SMART_SEARCH_PORT);
	if(u32DstIp == 0)
	{
		net_set_sock_boardcast(nSock);
		sockAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	}
	else
		sockAddr.sin_addr.s_addr = htonl(u32DstIp);
	
	if(net_udp_send(nSock, pSendData, nSendLen, (struct sockaddr *)&sockAddr) < 0)
	{
		j_warning("Send smart command failed.");
		net_close_socket(&nSock);
		return -1;
	}
	net_close_socket(&nSock);

	return 0;
}

static int nvr_smart_send_heartbeat()
{	
	int nCh;
	uint8_t szSendData[4096] = {0};
	uint8_t szIpcMac[NVR_MAC_ADDR_LEN];
	NVR_SMART_NET_HEAD *pSmartHead = (NVR_SMART_NET_HEAD *)szSendData;
	NVR_SMART_IPC_INFO *pSmartIpcInfo = (NVR_SMART_IPC_INFO *)pSmartHead->data;
	NVR_CHANNEL_CFG_S stChInfo;

	pSmartHead->magic = NVR_SMART_MAGIC;
	pSmartHead->type = SMART_HEARTBEAT;
	pSmartHead->len = 0;
	memcpy(pSmartHead->devid, gMyDevId, sizeof(pSmartHead->devid));
	for(nCh = 0; nCh < NVR_MAX_CHANNEL_NUM; nCh++)
	{
		nvr_get_channel_cfg(nCh, &stChInfo);
		if(stChInfo.u8Enable && stChInfo.u32IpAddr != 0)
		{
			// 判断是否被其它nvr所使用，如果被使用不发送这个信息
			G_LOCK(smart);
			if(nvr_smart_find_ipc_info(stChInfo.u32IpAddr))
			{
				G_UNLOCK(smart);
				continue;
			}
			G_UNLOCK(smart);
			
			if(nvr_arp_get_mac(stChInfo.u32IpAddr, szIpcMac) < 0)
				continue;
			pSmartIpcInfo->u32DevIp = stChInfo.u32IpAddr;
			memcpy(pSmartIpcInfo->szIpcMac, szIpcMac, sizeof(pSmartIpcInfo->szIpcMac));
			
			pSmartHead->len += sizeof(NVR_SMART_IPC_INFO);
			pSmartIpcInfo++;
		}
	}
	if(nvr_smart_send(szSendData, sizeof(NVR_SMART_NET_HEAD) + pSmartHead->len, 0))
	{
		j_warning("Send nvr channel info failed.");
		return -1;
	}
	
	return 0;
}

static int nvr_smart_send_shutdown()
{
	NVR_SMART_NET_HEAD stSmartHead = {0};
	
	stSmartHead.magic = NVR_SMART_MAGIC;
	stSmartHead.type = SMART_SHUTDOWN;
	stSmartHead.len = 0;
	memcpy(stSmartHead.devid, gMyDevId, sizeof(stSmartHead.devid));

	return nvr_smart_send(&stSmartHead, sizeof(stSmartHead), 0);
}

static int nvr_smart_send_query_info()
{
	NVR_SMART_NET_HEAD stSmartHead = {0};
	
	stSmartHead.magic = NVR_SMART_MAGIC;
	stSmartHead.type = SMART_QUERY_INFO;
	stSmartHead.len = 0;
	memcpy(stSmartHead.devid, gMyDevId, sizeof(stSmartHead.devid));

	return nvr_smart_send(&stSmartHead, sizeof(stSmartHead), 0);
}

int nvr_smart_send_channel_info()
{
	G_LOCK(smart);
	if(g_get_monotonic_time() - gLastTime < 1000000)
	{
		G_UNLOCK(smart);
		return 0;
	}
	gLastTime = g_get_monotonic_time();
	G_UNLOCK(smart);
	
	return nvr_smart_send_heartbeat();
}

int nvr_smart_is_ipc_remote_used(const uint32_t u32IpcIp)
{
	int ret;
	if(u32IpcIp == 0)
		return -1;
	if(!g_atomic_int_get(&gRecvInfo))
		return -1;
		
	// 查找是否被其它nvr使用
	G_LOCK(smart);
	if(nvr_smart_find_ipc_info(u32IpcIp) != NULL)
		ret = 1;
	else
	{
		if(g_get_monotonic_time() - gLastTime > 5000000)
		{
			G_UNLOCK(smart);
			nvr_smart_send_channel_info();
			g_usleep(G_USEC_PER_SEC * 1);
			G_LOCK(smart);
			if(nvr_smart_find_ipc_info(u32IpcIp) != NULL)
				ret = 1;
			else
				ret = 0;
		}
		else
			ret = 0;
	}
	G_UNLOCK(smart);
	
	return ret;
}

static gboolean nvr_smart_send_timeout(gpointer user_data)
{
	nvr_smart_send_channel_info();
	return G_SOURCE_REMOVE;
}

int nvr_smart_update_channel(int port, NVR_CHANNEL_CFG_S *pChannelCfgInfo)
{
	g_timeout_add_seconds(0, nvr_smart_send_timeout, NULL);
	return 0;
}

void nvr_smart_print_all_info()
{
	GList *pNvrList, *pIpcList;
	NVR_SMART_INFO *pSmartInfo;
	NVR_SMART_IPC_INFO *pSmartIpcInfo;
	char szIp[32] = {0};

	j_debug("============================== smart info.");
	G_LOCK(smart);
	for(pNvrList = g_list_first(gNvrInfoList); pNvrList != NULL; pNvrList = g_list_next(pNvrList))
	{
		pSmartInfo = (NVR_SMART_INFO *)pNvrList->data;
		net_ip_n2a(pSmartInfo->u32DevIp, szIp, sizeof(szIp));
		printf("NVR IP: %-16s  ID: %s\n", szIp, pSmartInfo->szDevId);
		for(pIpcList = g_list_first(pSmartInfo->pIpcInfoList); pIpcList != NULL; pIpcList = g_list_next(pIpcList))
		{
			pSmartIpcInfo = (NVR_SMART_IPC_INFO *)pIpcList->data;
			net_ip_n2a(pSmartIpcInfo->u32DevIp, szIp, sizeof(szIp));
			printf("\t IPC IP: %-16s \t %02x:%02x:%02x:%02x:%02x:%02x\n", szIp,
					pSmartIpcInfo->szIpcMac[0], pSmartIpcInfo->szIpcMac[1], pSmartIpcInfo->szIpcMac[2],
					pSmartIpcInfo->szIpcMac[3], pSmartIpcInfo->szIpcMac[4], pSmartIpcInfo->szIpcMac[5]);
		}
	}
	G_UNLOCK(smart);
}

static gpointer nvr_smart_search_func (gpointer user_data)
{
	int nRet;
	int nSock = 0;
	uint8_t szData[4096];
	struct sockaddr_in sockAddr;
	int nAddrLen;
	NVR_SMART_NET_HEAD *pSmartHead;

	while(!gSmartQuit)
	{
		if(nSock <= 0)
		{
			nSock = net_create_sock(2);
			if(nSock < 0)
			{
				j_warning("Create smart search socket failed.\n");
				sleep(1);
				continue;
			}
			if(net_bind_sock(nSock, INADDR_ANY, NVR_SMART_SEARCH_PORT) < 0)
			{
				j_warning("Smart search bind sock to %d failed.", NVR_SMART_SEARCH_PORT);
				net_close_socket(&nSock);
				sleep(1);
				continue;
			}
		}
		if((nRet = net_select(&nSock, 1, 0x1, 1000)) < 0)
		{
			sleep(1);
			net_close_socket(&nSock);
			continue;
		}
		
		if(nRet == 0)
			continue;
		else if(nRet == (nSock | 0x10000))
		{
			nAddrLen = sizeof(sockAddr);
			if(net_udp_recv(nSock, szData, sizeof(szData), 0, (struct sockaddr *)&sockAddr, &nAddrLen) < 0)
			{
				g_warning("Receive smart search package failed.");
				net_close_socket(&nSock);
				continue;
			}
			pSmartHead = (NVR_SMART_NET_HEAD *)szData;
			if(pSmartHead->magic != NVR_SMART_MAGIC)
				continue;
			// 判断是否为本地id
			if(memcmp(pSmartHead->devid, gMyDevId, sizeof(pSmartHead->devid)) == 0)
				continue;
			//j_debug("receive smart dev id: %s, cmd: %u", pSmartHead->devid, pSmartHead->type);
				
			switch(pSmartHead->type)
			{
				case SMART_HEARTBEAT:
					nvr_smart_recv_heartbeat(szData, ntohl(sockAddr.sin_addr.s_addr));
					g_atomic_int_set(&gRecvInfo, 1);
					break;
				case SMART_SHUTDOWN:
					nvr_smart_recv_shutdown(szData);
					break;
				case SMART_QUERY_INFO:
					nvr_smart_send_heartbeat();
					break;
				default:
					break;
			}
		}
	}

	return NULL;
}

static gboolean nvr_smart_timeout(gpointer user_data)
{
	GList *list, *list_tmp;
	NVR_SMART_INFO *pSmartInfo;
	
	G_LOCK(smart);
	for(list = g_list_first(gNvrInfoList); list != NULL; /* nothing */)
	{
		pSmartInfo = (NVR_SMART_INFO *)list->data;
		list_tmp = list;
		list = g_list_next(list);
		if(++pSmartInfo->nElapseTime > 3)
		{
			g_list_free_full(pSmartInfo->pIpcInfoList, g_free);
			gNvrInfoList = g_list_delete_link(gNvrInfoList, list_tmp);
			g_free(pSmartInfo);
		}
	}
	G_UNLOCK(smart);

	nvr_smart_send_heartbeat();

	nvr_smart_print_all_info();
	
	return G_SOURCE_CONTINUE;
}

int nvr_smart_search_init()
{
	NVR_DEV_STABLE_INFO_S stStableInfo;

	// 判断是否为本地id
	nvr_get_stable_cfg(0, &stStableInfo);
	memcpy(gMyDevId, stStableInfo.szSerialId, NVR_SERIALNO_LEN);
	
	gSmartThread = g_thread_new("Smart", nvr_smart_search_func, NULL);
	nvr_smart_send_query_info();
	g_usleep(G_USEC_PER_SEC * 3);
	nvr_smart_print_all_info();
	
	g_timeout_add_seconds(30, nvr_smart_timeout, NULL);
	
	return 0;
}

void nvr_smart_search_uninit()
{
	nvr_smart_send_shutdown();
	gSmartQuit = 1;
	g_thread_join(gSmartThread);
}

