#include <stdio.h>
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
#include "ti_net_search.h"
#include "ti_net_common.h"
#include "ti_net_interface.h"

#define	TI_NET_SEARCH_MULTI_ADDR	 "224.240.240.240"
#define	TI_NET_SEARCH_MULTI_PORT	 47865
#define TI_NET_SEARCH_MAX_CLIENT_NUM 50

typedef struct _TI_NET_SEARCH_NODE_S_
{
	TI_NET_SEARCH_RES_S searchRes;
	struct _TI_NET_SEARCH_NODE_S_ *next;
}TI_NET_SEARCH_NODE_S;

typedef struct _TI_NET_SEARCH_USER_TABLE_S_
{
	long lHandle;
	TI_U32 lastPos;//上次获取结束位置
	TI_NET_SEARCH_NODE_S *pLastNode;//记录结束访问的节点
}TI_NET_SEARCH_USER_TABLE_S;

typedef struct _TI_NET_SEARCH_STRUCT_
{
	TI_BOOL Searching;
	volatile TI_BOOL RecvQuit;
	int nRecvSock;
	volatile TI_U32 u32PacketNo;
	volatile int u32DevNum;//设备数
	//TI_U32 lastPos;//上次获取结束位置
	//TI_NET_SEARCH_NODE_S *pLastNode;//记录结束访问的节点
	TI_NET_SEARCH_NODE_S *pSearchNodeHead;
	TI_NET_SEARCH_NODE_S *pSearchNodeEnd;//尾节点
	
	volatile int userCnt;
	pthread_mutex_t userlock;
	TI_NET_SEARCH_USER_TABLE_S userCT[TI_NET_SEARCH_MAX_CLIENT_NUM];
	
	pthread_mutex_t lock;
	pthread_t RecvthId;
	
}TI_NET_SEARCH_STRUCT;

static TI_NET_SEARCH_STRUCT gSearch;

static int Ti_Net_Add_User_Table(long lHandle)
{
	int i = 0;
	pthread_mutex_lock(&gSearch.userlock);
	if(TI_NET_SEARCH_MAX_CLIENT_NUM <= gSearch.userCnt)
	{
		pthread_mutex_unlock(&gSearch.userlock);
		return TI_FAILURE;
	}
	for(; i < TI_NET_SEARCH_MAX_CLIENT_NUM; i++)
	{
		if(0 == gSearch.userCT[i].lHandle)
		{
			gSearch.userCT[i].lHandle = lHandle;
			gSearch.userCnt++;

			pthread_mutex_unlock(&gSearch.userlock);
			return TI_SUCCESS;
		}
	}
	
	pthread_mutex_unlock(&gSearch.userlock);
	return TI_FAILURE;
}

static TI_NET_SEARCH_USER_TABLE_S* Ti_Net_Get_User_Table(long lHandle)
{
	int i = 0;
	pthread_mutex_lock(&gSearch.userlock);
	if(gSearch.userCnt == 0)
	{
		pthread_mutex_unlock(&gSearch.userlock);
		return NULL;
	}
	for(; i < TI_NET_SEARCH_MAX_CLIENT_NUM; i++)
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

static void Ti_Net_Del_User_Table(long lHandle)
{
	int i = 0;
	pthread_mutex_lock(&gSearch.userlock);
	for(; i < TI_NET_SEARCH_MAX_CLIENT_NUM; i++)
	{
		if(lHandle == gSearch.userCT[i].lHandle)
		{
			gSearch.userCT[i].lHandle = 0;
			gSearch.userCT[i].lastPos = 0;
			gSearch.userCT[i].pLastNode = NULL;
			gSearch.userCnt--;

			pthread_mutex_unlock(&gSearch.userlock);
			return;
		}
	}
	
	pthread_mutex_unlock(&gSearch.userlock);
}

static int Ti_Net_Get_User_Cnt()
{
	int nUserCnt;
	pthread_mutex_lock(&gSearch.userlock);
	nUserCnt = gSearch.userCnt;
	pthread_mutex_unlock(&gSearch.userlock);
	return nUserCnt;
}

static int Ti_Net_Search_List_Find(TI_NET_SEARCH_RES_S *pResNode)
{
	TI_NET_SEARCH_NODE_S *pResNodeTmp = NULL;
	
	if(gSearch.pSearchNodeHead == NULL)
		return -1;

	pResNodeTmp = gSearch.pSearchNodeHead;

	while(NULL != pResNodeTmp)
	{
		//ip与序列号相同
		if((pResNode->u32DevIp ==  pResNodeTmp->searchRes.u32DevIp)
			&& (0 == strncmp((char *)pResNode->u8SerialId,
			                 (char *)pResNodeTmp->searchRes.u8SerialId,
			                 TI_SERIALNO_LEN)))
			return 0;

		pResNodeTmp = pResNodeTmp->next;
	}

	return -1;
}

static void Ti_Net_Search_Add_To_List(TI_NET_SEARCH_NODE_S *pResNode)
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

static void Ti_Net_Search_List_Free()
{
	TI_NET_SEARCH_NODE_S *pResNode;
	TI_NET_SEARCH_NODE_S *pTmpResNode;
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
/*
static void Ti_net_Search_Print_List()
{
	int nCnt = 0;
	TI_NET_SEARCH_NODE_S *pResNode;
	pthread_mutex_lock(&gSearch.lock);
	pResNode = gSearch.pSearchNodeHead;
	while(pResNode != NULL)
	{
		int addr[4];
		addr[0] = (pResNode->searchRes.u32DevIp>>24)&0xff;
		addr[1] = (pResNode->searchRes.u32DevIp>>16)&0xff;
		addr[2] = (pResNode->searchRes.u32DevIp>>8)&0xff;
		addr[3] = (pResNode->searchRes.u32DevIp>>0)&0xff;
		printf("%d===>%d.%d.%d.%d:%d\n", ++nCnt, addr[0], addr[1], addr[2], addr[3],
					pResNode->searchRes.u16DevPort);
		pResNode = pResNode->next;
	}
	pthread_mutex_unlock(&gSearch.lock);
}
*/

/*
	发送搜索包,
	pPack: 搜索包数据(头+数据).
	cmdVal: 搜索命令码(
	        搜索命令:TI_CMD_SEARCH_SERVER_MSG, 
	        搜索配置命令: TI_CMD_SEARCH_CONFIG_SERVER).
	dataSize: 数据包大小.
*/
static int Ti_Net_Search_Send_Pack(void* pPack, int nPackSize)
{
	int  nMultiSock = 0;
	struct sockaddr_in LocalAddr;
	struct sockaddr_in DesAddr;
	
	memset(&LocalAddr, 0, sizeof(struct sockaddr_in));
	memset(&DesAddr, 0, sizeof(struct sockaddr_in));

	if ((nMultiSock = net_create_sock(2)) < 0)
		return nMultiSock;

	LocalAddr.sin_family = AF_INET;
	LocalAddr.sin_addr.s_addr = inet_addr(TI_NET_SEARCH_MULTI_ADDR );

	DesAddr.sin_family = AF_INET;
	DesAddr.sin_addr.s_addr = inet_addr(TI_NET_SEARCH_MULTI_ADDR );
	DesAddr.sin_port = htons(TI_NET_SEARCH_MULTI_PORT);

	net_set_sock_attr(nMultiSock, 1, 0, 0, 0, 0);  
	net_set_sock_multicast_noloop(nMultiSock);

	if (bind(nMultiSock,(struct sockaddr*)&LocalAddr, sizeof(struct sockaddr_in)) < 0)
		return NET_RETURN_FAIL;
	if(net_udp_send(nMultiSock, pPack, nPackSize,
							(struct sockaddr *)&DesAddr) < 0)
	{
		__ERR("Faild to send search msg!\r\n");
		net_close_socket(&nMultiSock);
		return TI_ERR_SEND_FAIL;
	}
	net_close_socket(&nMultiSock);

	return TI_ERR_SUCCESS;
}
//解析为通用属性
static void ti_get_search_info_ti2ipnc(IPNC_NET_SEARCH_RES_S *pIpncDev, TI_NET_SEARCH_RES_S *pTiDev)
{
	if(NULL == pIpncDev || NULL == pTiDev)
		return;

	//序列号
	memcpy(pIpncDev->u8SerialId, pTiDev->u8SerialId, sizeof(pIpncDev->u8SerialId));
	//软件版本
	pIpncDev->u32SoftVer = pTiDev->u32SoftVer;
	//设备类型
	pIpncDev->u16DevType = pTiDev->u16DevType;
	//设备端口
	pIpncDev->u16DevPort = pTiDev->u16DevPort;
	//ip地址
	pIpncDev->u32DevIp = pTiDev->u32DevIp;
	//mac地址
	memcpy(pIpncDev->u8MacAddr, pTiDev->u8MacAddr, sizeof(pIpncDev->u8MacAddr));
	// dns enable
	pIpncDev->u8Dns = pIpncDev->u8Dns;
	// dhcp
	pIpncDev->u8DHCP = pIpncDev->u8DHCP;
	//掩码
	pIpncDev->u32DevIpMask = pTiDev->u32DevIpMask;
	//网关
	pIpncDev->u32DevGwAddr = pTiDev->u32DevGwAddr;
	//dns adddress
	pIpncDev->u32DevDns1 = pTiDev->u32DevDns1;
	pIpncDev->u32DevDns2 = pTiDev->u32DevDns2;

	memcpy(pIpncDev->szPriData, pTiDev, sizeof(TI_NET_SEARCH_RES_S));
}

//解析为通用属性
static void ti_get_search_info_ipnc2ti(TI_NET_SEARCH_CFG_REQ_S *pTiDev, IPNC_NET_SEARCH_CFG_REQ_S *pIpncDev)
{
	TI_NET_SEARCH_CFG_REQ_S *pPriTiDev;
	
	if(NULL == pIpncDev || NULL == pTiDev)
		return;
	pPriTiDev = (TI_NET_SEARCH_CFG_REQ_S *)pIpncDev->szPriData;
	
	//用户名密码
	strncpy(pTiDev->szUsrName, pIpncDev->szUsrName, 
			strlen(pIpncDev->szUsrName));
	strncpy(pTiDev->szPsw, pIpncDev->szPsw, 
			strlen(pIpncDev->szPsw));
			
	//序列号
	memcpy(pTiDev->u8SerialId, pIpncDev->u8SerialId, sizeof(pTiDev->u8SerialId));
	//软件版本
	pTiDev->u32SoftVer = pIpncDev->u32SoftVer;
	//设备类型
	pTiDev->u16DevType = pIpncDev->u16DevType;
	//设备端口
	pTiDev->u16DevPort = pIpncDev->u16DevPort;
	//mac地址
	memcpy(pTiDev->u8MacAddr, pIpncDev->u8MacAddr, sizeof(pTiDev->u8MacAddr));
	// dns enable
	pTiDev->u8Dns = pIpncDev->u8Dns;
	// dhcp
	pTiDev->u8DHCP = pIpncDev->u8DHCP;
	//ip地址
	pTiDev->u32DevIp = pIpncDev->u32DevIp;
	//掩码
	pTiDev->u32DevIpMask = pIpncDev->u32DevIpMask;
	//网关
	pTiDev->u32DevGwAddr = pIpncDev->u32DevGwAddr;
	//dns
	pTiDev->u32DevDns1 = pIpncDev->u32DevDns1;
	pTiDev->u32DevDns2 = pIpncDev->u32DevDns2;

	pTiDev->u32Capability0 = pPriTiDev->u32Capability0;
	pTiDev->u32Capability1 = pPriTiDev->u32Capability1;
	pTiDev->u32Capability2 = pPriTiDev->u32Capability2;
	pTiDev->u8MainDevType = pPriTiDev->u8MainDevType;
	pTiDev->u8SubDevType = pPriTiDev->u8SubDevType;
	pTiDev->u16PlateformCap = pPriTiDev->u16PlateformCap;
	pTiDev->u32UpTime = pPriTiDev->u32UpTime;
}

int Ti_Net_Get_Search(long lHandle, IPNC_NET_SEARCH_RES_S *pDev)
{
	TI_NET_SEARCH_NODE_S *pResNode;
	TI_NET_SEARCH_USER_TABLE_S* pUserTb = NULL;

	pthread_mutex_lock(&gSearch.lock);
	//获得用户信息
	pUserTb = Ti_Net_Get_User_Table(lHandle);
	if(NULL == pUserTb)
	{
		__ERR("user table is null. \n");
		pthread_mutex_unlock(&gSearch.lock);
		return TI_FAILURE;
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
	//memcpy(pDev, &pResNode->searchRes, sizeof(pResNode->searchRes));
	ti_get_search_info_ti2ipnc(pDev, &pResNode->searchRes);
	pUserTb->pLastNode = pResNode;
	pthread_mutex_unlock(&gSearch.lock);
	
	return 1;
}


int Ti_Net_Search_Cfg(IPNC_NET_SEARCH_CFG_REQ_S* pIpncSearchCfg)
{
	char szMsg[512] = {0};
	TI_U32 u32PacketNo;
	struct timeval tv;
	TI_NET_SEARCH_CFG_REQ_S searCfg;

	if(NULL == pIpncSearchCfg)
		return TI_ERR_PARAMETER;

	gettimeofday(&tv, NULL);
	u32PacketNo = tv.tv_usec;
	
	ti_net_encrypt(pIpncSearchCfg->szUsrName,
	               strlen(pIpncSearchCfg->szUsrName));
	ti_net_encrypt(pIpncSearchCfg->szPsw, 
				   strlen(pIpncSearchCfg->szPsw));
	
	memset(&searCfg, 0 , sizeof(searCfg));
	ti_get_search_info_ipnc2ti(&searCfg, pIpncSearchCfg);
	
	searCfg.u32CliMultiIp = htonl(inet_addr(TI_NET_SEARCH_MULTI_ADDR));
	searCfg.u16CliMultiPort= TI_NET_SEARCH_MULTI_PORT -1;
	memcpy(szMsg + TI_NET_HDR_LEN, &searCfg, sizeof(TI_NET_SEARCH_CFG_REQ_S));
	net_set_net_hdr((TI_NET_HDR_S *)szMsg, sizeof(TI_NET_SEARCH_CFG_REQ_S), TI_CMD_SEARCH_CONFIG_SERVER, u32PacketNo, 0);
	if(Ti_Net_Search_Send_Pack(szMsg, TI_NET_HDR_LEN + sizeof(TI_NET_SEARCH_CFG_REQ_S)) < 0)
		return TI_ERR_SEND_FAIL;
	/*
	pSearchCfg = szMsg + TI_NET_HDR_LEN;
	__DBUG("end---------------------------------------------pSearchCfg: %p\n", pSearchCfg);
	__DBUG("end---------------------------------------------pSearchCfg->u8SerialId: %s\n", pSearchCfg->u8SerialId);
	__DBUG("end---------------------------------------------pSearchCfg->szUsrName: %s\n", pSearchCfg->szUsrName);
	__DBUG("end---------------------------------------------pSearchCfg->szPsw: %s\n", pSearchCfg->szPsw);
	__DBUG("end---------------------------------------------pSearchCfg->u16DevPort: %d\n", pSearchCfg->u16DevPort);
	__DBUG("end---------------------------------------------ip: %d.%d.%d.%d\n", 
																	(pSearchCfg->u32DevIp >> 24) & 0xff, 
																	(pSearchCfg->u32DevIp >> 16) & 0xff,
																    (pSearchCfg->u32DevIp >> 8 ) & 0xff,  
																     pSearchCfg->u32DevIp & 0xff);
	__DBUG("---------------------------------------------u32PacketNo: %d\n", u32PacketNo);
	*/

	return TI_ERR_SUCCESS;
}


int Ti_Net_Search_Do(long lHandle)
{
	char szMsg[512] = {0};
	struct timeval stTv;
	TI_NET_SEARCH_REQ_S*struSearch = (TI_NET_SEARCH_REQ_S*)(szMsg + TI_NET_HDR_LEN);

	pthread_mutex_lock(&gSearch.lock);
	if(Ti_Net_Get_User_Cnt() > 0)
	{
		if(Ti_Net_Add_User_Table(lHandle) < 0)
		{
			pthread_mutex_unlock(&gSearch.lock);
			return TI_FAILURE;
		}
		pthread_mutex_unlock(&gSearch.lock);
		return TI_SUCCESS;
	}
	else
	{
		Ti_Net_Search_List_Free();
	}
	
	if(0 > Ti_Net_Add_User_Table(lHandle))
	{
		pthread_mutex_unlock(&gSearch.lock);
		return TI_FAILURE;
	}
	
	struSearch->u32CliMultiIp = htonl(inet_addr(TI_NET_SEARCH_MULTI_ADDR));
	struSearch->u16CliMultiPort= TI_NET_SEARCH_MULTI_PORT -1;
	
	gettimeofday(&stTv, NULL);
	gSearch.u32PacketNo = stTv.tv_usec;
	net_set_net_hdr((TI_NET_HDR_S *)szMsg, sizeof(TI_NET_SEARCH_REQ_S), TI_CMD_SEARCH_SERVER_MSG, gSearch.u32PacketNo, 0);
	if(Ti_Net_Search_Send_Pack(szMsg, TI_NET_HDR_LEN + sizeof(TI_NET_SEARCH_REQ_S)) < 0)
	{
		__ERR("Search send package failed.\n");
		Ti_Net_Del_User_Table(lHandle);
		pthread_mutex_unlock(&gSearch.lock);
		return TI_FAILURE;
	}
	
	pthread_mutex_unlock(&gSearch.lock);
	
	return TI_SUCCESS;
}

int Ti_Net_Search_Done(long lHandle)
{
	//删除用户信息
	pthread_mutex_lock(&gSearch.lock);
	Ti_Net_Del_User_Table(lHandle);
	if(Ti_Net_Get_User_Cnt() <= 0)
	{
		Ti_Net_Search_List_Free();
		gSearch.u32PacketNo = ~0;
	}
	pthread_mutex_unlock(&gSearch.lock);
	
	return 0;
}

int Ti_Net_Get_Search_DevNum()
{
	int nDevNum;
	pthread_mutex_unlock(&gSearch.lock);
	nDevNum = gSearch.u32DevNum;
	pthread_mutex_unlock(&gSearch.lock);
	
	return nDevNum;
}

/*
int Ti_Net_Get_Search(IpncSearch *search, int cnt, int *searchCnt)
{
	int i = 0;
	TI_NET_SEARCH_NODE_S *pResNode;

	if(gSearch.Searching != TI_TRUE)
		return TI_FAILURE;
	//Ti_net_Search_Print_List();
	pthread_mutex_lock(&gSearch.lock);
	pResNode = gSearch.pSearchNodeHead;
	while(pResNode != NULL)
	{
		if(i >= cnt)
			break;
		search[i].ipAddr = pResNode->searchRes.u32DevIp;
		search[i].port = pResNode->searchRes.u16DevPort;
		i++;
		pResNode = pResNode->next;
	}
	*searchCnt = i;
	pthread_mutex_unlock(&gSearch.lock);
	__DBUG("Search Count: %d\n", *searchCnt);
	return TI_SUCCESS;
}
*/

static int	Ti_Net_Search_Create_Multi_Sock()
{
	int		nMultiSock = 0;

	if ((nMultiSock = net_create_sock(2)) < 0)
		return nMultiSock;

	if (net_bind_sock(nMultiSock, INADDR_ANY, TI_NET_SEARCH_MULTI_PORT-1) < 0 ||
		net_set_sock_attr(nMultiSock, 1, 0, 0, 0, 0) < 0 ||
		net_set_sock_multicast_noloop(nMultiSock) < 0)
		goto TI_NET_CREATE_SERVER_SOCK_ERR;
	
	if (net_set_sock_multi_membership(nMultiSock, TI_NET_SEARCH_MULTI_ADDR, net_get_sock_ip(nMultiSock)))
	{
		goto TI_NET_CREATE_SERVER_SOCK_ERR;
	}
	
	return nMultiSock;

TI_NET_CREATE_SERVER_SOCK_ERR:
	net_close_socket(&nMultiSock);
	return TI_FAILURE;
}
	

static int Ti_Net_Search_Set_Dev(TI_NET_SEARCH_RES_S* pDev)
{
	TI_NET_SEARCH_NODE_S *pSearchNode;
	if(pDev == NULL)
		return TI_FAILURE;

	if(0 == Ti_Net_Search_List_Find(pDev))
		return 0;
	
	pSearchNode = (TI_NET_SEARCH_NODE_S*)malloc(sizeof(TI_NET_SEARCH_NODE_S));
	if(pSearchNode == NULL)
		return TI_FAILURE;
	memcpy(&pSearchNode->searchRes, pDev, sizeof(*pDev));
	pSearchNode->next = NULL;
	pthread_mutex_lock(&gSearch.lock);
	Ti_Net_Search_Add_To_List(pSearchNode);
	pthread_mutex_unlock(&gSearch.lock);
#if 0
	char IP[32] = {0};
	net_ip_n2a(pSearchNode->searchRes.u32DevIp, IP, 32);
	__DBUG("IP:%-16s, %s\r\n", IP, (char*)pSearchNode->searchRes.u8SerialId);
#endif
	
	return 0;
}

static int Ti_Net_Search_Recv_Pro(int nSock)
{
	int nRet = 0;
	int nAddrLen = sizeof(struct sockaddr_in);;
	char RecvBuff[2048] = {0};
	//char IP[32] = {0};
	TI_NET_HDR_S *pHdr = (TI_NET_HDR_S *)RecvBuff;
	TI_NET_SEARCH_RES_S*pBody = (TI_NET_SEARCH_RES_S*)(RecvBuff + TI_NET_HDR_LEN);

	struct sockaddr_in	addrNet;
	if ((nRet = net_udp_recv(nSock, RecvBuff, sizeof(RecvBuff), 0, 
						(struct sockaddr *)&addrNet, &nAddrLen)) < 0)
	{
		return TI_NET_ERR_RECV_FAIL;
	}
	
	if(Ti_Net_Get_User_Cnt() <= 0)
		return TI_FAILURE;
		
	if (ti_net_check_netHdr(pHdr) != TI_NET_ERR_SUCCESS)
		return TI_NET_ERR_DATA_FORMAT;
	if(pHdr->u32PacketNo != (gSearch.u32PacketNo | 0x80000000UL))
		return TI_FAILURE;
		
	switch(pHdr->u16CmdType)
	{
		case TI_CMD_SEARCH_SERVER_MSG://搜索
		{
			//net_ip_n2a(pBody->u32DevIp, IP, 32);
			//__DBUG("IP:%-16s, %s\r\n", IP, pBody->u8SerialId);
			Ti_Net_Search_Set_Dev(pBody);
			break;	
		}
		case TI_CMD_SEARCH_CONFIG_SERVER://配置
		{
			//__DBUG("TI_CMD_SEARCH_CONFIG_SERVER: %#x\n", TI_CMD_SEARCH_CONFIG_SERVER);
			break;
		}
	}

	return TI_NET_ERR_SUCCESS;
}

static void* Ti_Net_Search_Recv_Server(void * data)
{
	int nSock = 0;
	int nRet = 0;
	void* ret = NULL;

	prctl(PR_SET_NAME, "TiSearch");
	while(!gSearch.RecvQuit)
	{
		if(nSock <= 0 && (nSock = Ti_Net_Search_Create_Multi_Sock()) < 0)
		{
			__ERR("Failed to Create Socket! \r\n");
			sleep(1);
			continue;
		}
		if ((nRet = net_select(&nSock, 0x1, 0x1, 1000)) < 0)
		{
			__ERR("Faild to select search socket!\r\n");
			net_close_socket(&nSock);
			sleep(1);
			continue ;
		}
		else if (nRet == (nSock | 0x10000))
		{
			Ti_Net_Search_Recv_Pro(nSock);
		}
	}
	net_close_socket(&nSock);
	
	return ret;
}

/* 搜索初始化 */
int Ti_Net_Search_init()
{
	memset(&gSearch, 0, sizeof(TI_NET_SEARCH_STRUCT));
	pthread_mutex_init(&gSearch.lock, NULL);
	pthread_mutex_init(&gSearch.userlock, NULL);
	
	if(pthread_create(&gSearch.RecvthId, NULL, Ti_Net_Search_Recv_Server, NULL) != 0)
	{
		__ERR("Failed to create Ti_Net_Recv_Search_Pro thread.\n");
		return TI_FAILURE;
	}
	gSearch.u32PacketNo = ~0;
	
	return TI_SUCCESS;
}

int Ti_Net_Search_uninit()
{
	gSearch.RecvQuit = 1;
	pthread_join(gSearch.RecvthId, NULL);
	Ti_Net_Search_List_Free();
	pthread_mutex_destroy(&gSearch.lock);
	pthread_mutex_destroy(&gSearch.userlock);
	return TI_SUCCESS;
}

