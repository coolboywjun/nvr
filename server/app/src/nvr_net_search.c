
#include <stdio.h>
#include <dhcp.h>
#include <net_api.h>
#include <ipnc_lib_client.h>

#include "nvr_net_search.h"
#include "nvr_param_proc.h"
#include "nvr_channel.h"
#include "nvr_arp.h"
#include "nvr_smart_search.h"

/*
开始搜索设备:
	pProtocolName:设备使用的协议名
	plHandle:存放返回句柄
成功返回搜索到的设备数量。

typedef struct _NVR_NET_SEARCH_CHNN_S_
{
	int maxChn;
	int chnArr[NVR_MAX_CHANNEL_NUM];
	//char protocolNameArr[NVR_MAX_CHANNEL_NUM];
	
}NVR_NET_SEARCH_CHNN_S;*/

typedef struct _NVR_NET_SEARCH_MNG_S_
{	
	int sock;
	//NVR_NET_SEARCH_CHNN_S chnCfg;
	GThread *searchServerTh;	
}NVR_NET_SEARCH_MNG_S;

typedef enum _NVR_SEARCH_CTRL_CMD_E_
{
	NVR_SEARCH_INTELLIGENT_CHECK 	= 0x01,//通道连接检测
}NVR_SEARCH_CTRL_CMD_E;

typedef struct _NVR_SEARCH_NODE_S_
{
	char protocolName[32];
	char user[NVR_USER_NAME_LEN];
	char passwd[NVR_PASS_WORD_LEN];
	
	IPNC_NET_SEARCH_RES_S search;
}NVR_SEARCH_NODE_S;

NVR_NET_SEARCH_MNG_S gSearchServerMng = {0};

static GThreadPool *g_searchpool;

const IPNC_PROTOCOL_S *nvr_get_protcol_table(void)
{
	return ipnc_client_get_protocols();
}

int nvr_get_protcol_table_num(void)
{
	return ipnc_client_get_protocols_num();
}

int nvr_search_start(char* pProtocolName, long* plHandle)
{
	long lHandle = 0;
	if(ipnc_client_connect_server(pProtocolName, &lHandle) < 0)
	{
		j_warning("nvr_start_search failed ...");
		return -1;
	}

	if(0 != ipnc_client_start_search(lHandle))
	{
		ipnc_client_disconnect_server(lHandle);
		j_warning("ipnc_client_start_search failed ...");
		return -1;
	}
	else
		*plHandle = lHandle;
	
	return 0;
}

/*
清除搜索缓存。
*/
void nvr_search_stop(long lHandle)
{
	if(0 != lHandle)
	{
		ipnc_client_stop_search(lHandle);
		ipnc_client_disconnect_server(lHandle);
	}	
}

/*
获得搜索结果:
 lHandle:连接句柄
 pDev:存放搜索结果的空间;
 devBufSize:空间大小
 成功返回0.
*/
int nvr_search_get(long lHandle, IPNC_NET_SEARCH_RES_S *pDevReq)
{
	if(NULL == pDevReq || (lHandle == 0))
		return -1;

	return ipnc_client_get_search(lHandle, pDevReq);
}

/*
获得ipc状态:
lHandle:连接句柄
pIpAddr:ip地址
port:端口
pUsr:用户名字
pPwd:密码
*/
int nvr_search_test_ipc_status(long lHandle, char *pIpAddr, int port,
							   const char* pUsr, const char* pPwd)
{
	if(NULL == pIpAddr || (lHandle == 0))
		return -1;

	return ipnc_client_test_alive(lHandle, pIpAddr, port, pUsr, pPwd);
}

/*
搜索配置:
lHandle:连接句柄
pSearchCfg:搜索配置包
dataSize  :配置包大小
*/

int nvr_search_cfg(long lHandle, IPNC_NET_SEARCH_CFG_REQ_S *pSearchCfg)
{
	if(NULL == pSearchCfg || (lHandle == 0))
		return -1;
	
	return ipnc_client_set_search_cfg(lHandle, pSearchCfg);
}

GList* nvr_search_all_ipc()
{
	int ret, flag;
	const IPNC_PROTOCOL_S *protocol;
	IPNC_NET_SEARCH_RES_S search;
	long lHandle;
	NVR_SEARCH_NODE_S *searchNode;
	GList *searchIpcList = NULL;

	j_debug("Search all ipc.");
	for(protocol = ipnc_client_get_protocols(); protocol != NULL; protocol++)
	{
		if(strlen(protocol->name) == 0)
			break;
		if(ipnc_client_connect_server(protocol->name, &lHandle) < 0)
		{
			j_warning("ipnc_client_connect_server failed ...");
			continue;
		}
		if(ipnc_client_start_search(lHandle) < 0)
		{
			j_warning("%s start_search failed.", protocol->name);
			ipnc_client_disconnect_server(lHandle);
			continue;
		}
		g_usleep(1000000);
		flag = 0;
		while((ret = ipnc_client_get_search(lHandle, &search)) >= 0)
		{
			// 如果没有获取到 search数据，再获取一次
			if(ret == 0)
			{
				if(flag < 3)
				{
					flag++;
					g_usleep(100000);
					continue;
				}
				else
					break;
			}
			else if(ret < 0)
				break;
			flag = 0;
			searchNode = (NVR_SEARCH_NODE_S *)g_malloc(sizeof(NVR_SEARCH_NODE_S));
			g_strlcpy(searchNode->protocolName, protocol->name, sizeof(searchNode->protocolName));
			g_strlcpy(searchNode->user, protocol->usrPwd[0].usr, sizeof(searchNode->user));
			g_strlcpy(searchNode->passwd, protocol->usrPwd[0].pwd, sizeof(searchNode->passwd));
			memcpy(&searchNode->search, &search, sizeof(search));
			searchIpcList = g_list_append(searchIpcList, searchNode);
		}
		ipnc_client_stop_search(lHandle);
		ipnc_client_disconnect_server(lHandle);
	}
	return searchIpcList;
}

// nvr自动分配IP
static int nvr_search_get_random_ip(unsigned long *newip, unsigned long *newnetmask, 
                        unsigned long *newgatewayip, unsigned long *newdns1, unsigned long *newdns2)
{
	unsigned long ip, ip1;
	unsigned long netmask;
	unsigned long hostnet;
	unsigned long randomip;
	NVR_DEV_NET_CFG_S stNetCfg;
	uint8_t macAddr[16];
	nvr_get_net_cfg(0, &stNetCfg);
	ip = stNetCfg.struEtherCfg[0].u32IpAddr;
	ip1 = stNetCfg.struEtherCfg[0].u32IpAddr1;
	netmask = stNetCfg.struEtherCfg[0].u32SubMask;
	do
	{
		srandom((unsigned)time(NULL));
		hostnet = random() & (~netmask);
		if(hostnet == 0 || hostnet == 1 || hostnet == (~netmask))
			continue;
		randomip = (ip & netmask) | hostnet;
		if(randomip == ip || randomip == ip1)
			continue;
		// 如果获取不到这个ip地址说明局域网内没有设备使用
		// 这个IP地址,然后我们使用这个IP地址
		if(nvr_arp_get_mac(randomip, macAddr) < 0)
			break;
	}while(1);
	if(newip) 			*newip = randomip;
	if(newnetmask) 	*newnetmask = netmask;
	if(newgatewayip)	*newgatewayip = stNetCfg.struEtherCfg[0].u32GateWay;
	if(newdns1)		*newdns1 = stNetCfg.u32DnsIp1;
	if(newdns2)		*newdns2 = stNetCfg.u32DnsIp2;
	return 0;
}

static int nvr_search_set_ipc_ip(NVR_SEARCH_NODE_S *searchNode)
{
	IPNC_NET_SEARCH_RES_S *search = &searchNode->search;
	IPNC_NET_SEARCH_CFG_REQ_S ipncSearchCfg;
	unsigned long ipAddr, netmask, gateway, dns1, dns2;
	dhcpHandle handle;
	long lHandle;

	if(nvr_smart_is_ipc_remote_used(search->u32DevIp) == 1)
		return 0;
	if(ipnc_client_connect_server(searchNode->protocolName, &lHandle) < 0)
		return -1;
	if((handle = dhcpLibRequest(search->u8MacAddr, &ipAddr, &netmask, &gateway, &dns1, &dns2)) == NULL)
	{
		j_message("Request dhcp ip address failed, we create a ip address.");
		nvr_search_get_random_ip(&ipAddr, &netmask, &gateway, &dns1, &dns2);
	}
	j_message("new ip: %lu.%lu.%lu.%lu", (ipAddr>>24) & 0xff, (ipAddr>>16)&0xff, (ipAddr>>8)&0xff, ipAddr&0xff);
	j_message("protocol: %s, serial: %s, usr: %s, passwd: %s\n", searchNode->protocolName, search->u8SerialId, searchNode->user, searchNode->passwd);
	memset(&ipncSearchCfg, 0, sizeof(ipncSearchCfg));
	g_strlcpy(ipncSearchCfg.szUsrName, searchNode->user, sizeof(ipncSearchCfg.szUsrName));
	g_strlcpy(ipncSearchCfg.szPsw, searchNode->passwd, sizeof(ipncSearchCfg.szPsw));
	//g_strlcpy((char *)ipncSearchCfg.u8SerialId, (char*)search->u8SerialId, sizeof(ipncSearchCfg.u8SerialId));
	memcpy(ipncSearchCfg.u8SerialId, search->u8SerialId, sizeof(ipncSearchCfg.u8SerialId));
	ipncSearchCfg.u32SoftVer = search->u32SoftVer;
	ipncSearchCfg.u16DevType = search->u16DevType;
	ipncSearchCfg.u16DevPort = search->u16DevPort;
	ipncSearchCfg.u32DevIp = ipAddr;
	memcpy(ipncSearchCfg.u8MacAddr, search->u8MacAddr, IPNC_MAC_ADDR_LEN);
	ipncSearchCfg.u8Dns = search->u8Dns;
	ipncSearchCfg.u8DHCP = search->u8DHCP;
	ipncSearchCfg.u32DevIpMask = netmask;
	ipncSearchCfg.u32DevGwAddr = gateway;
	ipncSearchCfg.u32DevDns1 = dns1;
	ipncSearchCfg.u32DevDns2 = dns2;
	memcpy(ipncSearchCfg.szPriData, search->szPriData, sizeof(ipncSearchCfg.szPriData));
	if(ipnc_client_set_search_cfg(lHandle, &ipncSearchCfg) < 0)
	{
		j_warning("Set remote ipnc ip address failed.");
		dhcpLibRelease(handle);
		handle = NULL;
	}
	ipnc_client_disconnect_server(lHandle);
	
	return 0;
}

//智能检测通道连通性
static void nvr_search_intelligent_check_task()
{
	int chn;
	long lHandle;
	char ipaddr[32];
	NVR_CHANNEL_CFG_S channelCfg[NVR_MAX_CHANNEL_NUM];
	NVR_DEV_AUTO_CTRL_CFG_S autoCtrlCfg;
	GList *list = NULL, *list_noalive = NULL;
	NVR_SEARCH_NODE_S *searchNode;
	IPNC_NET_SEARCH_RES_S *search;
	static gint64 lastTime = 0;
	GList *searchIpcList;

	if(nvr_get_filesystem_is_default())
		return;
	if(g_get_monotonic_time() - lastTime < 1000000)	// 在1s内不再马上处理
		return;
	lastTime = g_get_monotonic_time();
		
	j_debug("Check the no alive channel.");
	for(chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
	{
		nvr_get_channel_cfg(chn, &channelCfg[chn]);
		if(!channelCfg[chn].u8Enable || !channelCfg[chn].u8SearchA)
			continue;
		//判断是否被其它nvr加入 
		if(nvr_smart_is_ipc_remote_used(channelCfg[chn].u32IpAddr) == 1)
		{
			channelCfg[chn].u32IpAddr = 0;
			nvr_set_channel_cfg(chn, &channelCfg[chn]);
			list_noalive = g_list_append(list_noalive, GINT_TO_POINTER(chn));
			continue;
		}

		if(channelCfg[chn].u32IpAddr == 0)
		{
			list_noalive = g_list_append(list_noalive, GINT_TO_POINTER(chn));
			continue;
		}
		//查看ipc连通性
		if(ipnc_client_connect_server(channelCfg[chn].szProtocolName, &lHandle) < 0)
		{
			j_warning("ipnc_client_connect_server failed ...");
			list_noalive = g_list_append(list_noalive, GINT_TO_POINTER(chn));
			continue;
		}
		net_ip_n2a(channelCfg[chn].u32IpAddr, ipaddr, sizeof(ipaddr));
		if(ipnc_client_test_alive(lHandle, ipaddr, channelCfg[chn].u16DataPort, 
								channelCfg[chn].szUser, channelCfg[chn].szPasswd) < 0)
		{
			list_noalive = g_list_append(list_noalive, GINT_TO_POINTER(chn));
		}
		ipnc_client_disconnect_server(lHandle);
	}

	nvr_get_auto_ctrl_cfg(0, &autoCtrlCfg);
	if(g_list_length(list_noalive) == 0 && autoCtrlCfg.u8Enable == 0)
		return;
		
	searchIpcList = nvr_search_all_ipc();
	if(searchIpcList == NULL)
		return;
	for(list = g_list_first(searchIpcList); list != NULL; list = g_list_next(list))
	{
		if(g_list_length(list_noalive) == 0)
			break;
		searchNode = (NVR_SEARCH_NODE_S *)list->data;
		search = &searchNode->search;
		// 测试是否有相同的IP
		for(chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
		{
			if(channelCfg[chn].u8Enable && search->u32DevIp == channelCfg[chn].u32IpAddr)
				break;
		}
		if(chn != NVR_MAX_CHANNEL_NUM)
			continue;
		// 测试获取到的ipnc是否可以正常连接
		net_ip_n2a(search->u32DevIp, ipaddr, sizeof(ipaddr));
		if(nvr_smart_is_ipc_remote_used(search->u32DevIp) == 1)
			continue;
		//查看ipc连通性
		if(ipnc_client_connect_server(searchNode->protocolName, &lHandle) < 0)
			continue;
		if(ipnc_client_test_alive(lHandle, ipaddr, search->u16DevPort,
								searchNode->user, searchNode->passwd) < 0)
		{
			ipnc_client_disconnect_server(lHandle);
			continue;
		}
		ipnc_client_disconnect_server(lHandle);
			
		list = g_list_first(list_noalive);
		chn = GPOINTER_TO_INT(list->data);
		list_noalive = g_list_delete_link(list_noalive, list);
		j_debug("Add ipnc ==> protocol:%s chn: %d, ip:%s, usr: %s, passwd: %s", 
				searchNode->protocolName, chn, ipaddr, 
				searchNode->user, searchNode->passwd);
		channelCfg[chn].u32Size = sizeof(channelCfg[chn]);
		g_strlcpy(channelCfg[chn].szFactoryName, searchNode->protocolName, NVR_NAME_LEN);
		g_strlcpy(channelCfg[chn].szProtocolName, searchNode->protocolName, NVR_NAME_LEN);
		g_strlcpy(channelCfg[chn].szDevName, (gchar *)search->u8SerialId, NVR_NAME_LEN);
		g_strlcpy(channelCfg[chn].szUser, searchNode->user, NVR_NAME_LEN);
		g_strlcpy(channelCfg[chn].szPasswd, searchNode->passwd, NVR_PASS_WORD_LEN);
		channelCfg[chn].u32IpAddr = search->u32DevIp;
		channelCfg[chn].u16DataPort = search->u16DevPort;
		channelCfg[chn].u16WebPort = 80;
		channelCfg[chn].u16RtspPort = 554;
		channelCfg[chn].u8RemoteChn = 1;
		channelCfg[chn].u8UseChn = 0;
		channelCfg[chn].u8Enable = 1;
		channelCfg[chn].u8SearchA = 1;
		
		nvr_set_channel_cfg(chn, &channelCfg[chn]);
	}
	g_list_free(list_noalive);
	
	/////////////////////////////////////////////
	/// 注意:启用以下功能会对局域网内的ipc造成影响,
	/// 请警惕使用
	//g_list_free_full(searchIpcList, g_free);
	//return;
	////////////////////////////////////////////
	// 只有全自动控制打开后才执行下面的动作
	if(!autoCtrlCfg.u8Enable || g_list_length(searchIpcList) > 8 || nvr_get_dianxin_is_used())
	{
		g_list_free_full(searchIpcList, g_free);
		return;
	}
#if 1
	// 判断search中ipc是否有相同的ip
	// 如果有相同的ip,则修ipc ip地址
	for(list = g_list_first(searchIpcList); list != NULL; list = g_list_next(list))
	{
		GList *list1;
		if(list->data == NULL)
			continue;
		searchNode = (NVR_SEARCH_NODE_S *)list->data;
		search = &searchNode->search;
		// 如果ipc打开自动dhcp,则不设置ipc的ip地址
		if(search->u8DHCP)
			continue;
		for(list1 = g_list_next(list); list1 != NULL; list1 = g_list_next(list1))
		{
			NVR_SEARCH_NODE_S *searchNode1 = (NVR_SEARCH_NODE_S *)list1->data;
			IPNC_NET_SEARCH_RES_S *search1 = &searchNode1->search;
			if(list1->data == NULL)
				continue;
			if(search->u32DevIp != search1->u32DevIp)
				continue;
			if(g_strcmp0((char *)search->u8SerialId, (char *)search1->u8SerialId) == 0)
				continue;
			if(nvr_search_set_ipc_ip(searchNode) < 0)
				continue;
			// searchIpcList中删除list->data
			// 之后不再处理链接表中这个节点的数据
			g_free(list->data);
			list->data = NULL;
			break;
		}
	}
#endif

#if 1
	// 判断是否有ipc不能与nvr正常通信
	// 如果不能正常通信，则修改ipc ip地址
	for(list = g_list_first(searchIpcList); list != NULL; list = g_list_next(list))
	{
		char cmd[128];
		uint32_t devIp;
		if(list->data == NULL)
			continue;
		searchNode = (NVR_SEARCH_NODE_S *)list->data;
		search = &searchNode->search;
		devIp = search->u32DevIp;
		g_snprintf(cmd, sizeof(cmd), "ping %u.%u.%u.%u -c 1 -W 1 1>/dev/null 2>&1",
					(devIp >> 24) & 0xff, (devIp >> 16) & 0xff, 
					(devIp >> 8) & 0xff, devIp & 0xff);
		//j_debug("%s", cmd);
		if(system(cmd) == 0)		// 可以正常通信
			continue;
		if(nvr_search_set_ipc_ip(searchNode) < 0)
			continue;
		g_free(list->data);
		list->data = NULL;
	}
#endif

#if 1
	// 判断ipc是否存在ip冲突
	// 如果冲突则修改ipc ip地址
	for(list = g_list_first(searchIpcList); list != NULL; list = g_list_next(list))
	{
		if(list->data == NULL)
			continue;
		searchNode = (NVR_SEARCH_NODE_S *)list->data;
		search = &searchNode->search;
		if(nvr_arp_check_ip_conflict(search->u32DevIp) == 1)
			nvr_search_set_ipc_ip(searchNode);
	}
#endif
	g_list_free_full(searchIpcList, g_free);
}

//开始智能检测
void nvr_search_intelligent_check(void)
{
	g_thread_pool_push(g_searchpool, GINT_TO_POINTER(NVR_SEARCH_INTELLIGENT_CHECK), NULL);
}

/*定时更新时间控制*/
static gboolean nvr_search_intelligent_check_timeout(gpointer user_data)
{
	nvr_search_intelligent_check();
	if(GPOINTER_TO_INT(user_data))
		return G_SOURCE_REMOVE;
	return G_SOURCE_CONTINUE;
}

//线程处理函数
static void nvr_search_server_func(gpointer data, gpointer user_data)
{
	NVR_SEARCH_CTRL_CMD_E cmdVal = GPOINTER_TO_INT(data);

	switch(cmdVal)
	{
		case NVR_SEARCH_INTELLIGENT_CHECK:
			nvr_search_intelligent_check_task();
			break;
		default:
			g_assert_not_reached();
			break;
	}
}

int nvr_search_update_channel(int port, NVR_CHANNEL_CFG_S *pChannelCfgInfo)
{
	if(pChannelCfgInfo->u8Enable)
		g_timeout_add_seconds(0, nvr_search_intelligent_check_timeout, GINT_TO_POINTER(1));
	return 0;
}

//初始化函数
int nvr_search_init(void)
{
	g_searchpool = g_thread_pool_new(nvr_search_server_func, NULL, 1, FALSE, NULL);
	
	//智能检测
	//nvr_search_intelligent_check();

	//开启定时检测
	g_timeout_add_seconds(60, nvr_search_intelligent_check_timeout, NULL);

	return 0;
}

void nvr_search_uninit(void)
{
	j_warning("search uninit.");
	g_thread_pool_free(g_searchpool, TRUE, FALSE);
	j_warning("search uninit finish");
}


