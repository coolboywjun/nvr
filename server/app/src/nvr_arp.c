
#include <nvr_server_headers.h>

/* 需要里面的 glibc 版本号 */
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h> /* 链路层（L2）协议 */
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h> /* 链路层协议 */
#endif
#include <netinet/if_ether.h>
#include <pthread.h>
#include <sys/prctl.h>

#include <net_api.h>
#include <nvr_net_config.h>
#include <nvr_arp.h>

// 以太网的头部结构:
/*
   struct ether_header
   {
   u_int8_t ether_dhost[ETH_ALEN];      // destination eth addr
   u_int8_t ether_shost[ETH_ALEN];      // source ether addr   
   u_int16_t ether_type;                // packet type ID field
   } __attribute__ ((__packed__));
   */
//#define ETH_ALEN 		6 	  //以太网地址的长度
//#define ETH_HALEN 		14 	  //以太网头部的总长度  (6+6+2)
//#define ETH_ZLEN 		60 	  //不含CRC校验数据的数据最小长度(46+14)
//#define ETH_DATA_LEN 	1500  //帧内数据的最大长度
//#define ETH_FRAME_LEN 	1514  //不含CRC最大以太网长度(1500+14)
// ARP头部信息:
/*
   struct arphdr{
   __be16 ar_hrd;//硬件类型 1-硬件接口为以太网接口
   __be16 ar_pro;//协议类型-0x0800高层协议为IP协议 
   unsigned char ar_hln;//硬件地址长度-6字节 MAC
   unsigned char ar_pln;//协议地址长度-4字节为IP
   __be16 ar_op;//ARP操作码-1 ARP请求

   }
   */
// ARP协议数据结构:
/*
   struct ether_arp{
   struct arphdr ea_hdr; //ARPfixed-size header(ARP固定大小的报头)
   u_char arp_sha[ETHER_ADDR_LEN]; //sender hardware address(发送端硬件地址)
   u_char arp_spa[4]; //sender protocol address(发送端协议地址)
   u_char arp_tha[ETHER_ADDR_LEN]; // target hardware address(接收端硬件地址)
   u_char arp_tpa[4]; //target protocol address(接收端协议地址)
   };
   */
#define arp_hrd ea_hdr.ar_hrd
#define arp_pro ea_hdr.ar_pro
#define arp_hln ea_hdr.ar_hln
#define arp_pln ea_hdr.ar_pln
#define arp_op  ea_hdr.ar_op

// sockaddr_ll为设备无关的物理层地址结构，描述发送端的地址结构
/*
   struct sockaddr_ll
   {
   unsigned short sll_family;
   unsigned short sll_protocol;
   int sll_ifindex;
   unsigned short sll_hatype;
   unsigned char sll_pkttype;
   unsigned char sll_halen;
   unsigned char sll_addr[8];
   };
   */

#define NVR_ETH_NAME "eth0"
#define NVR_ARP_PADDING_SIZE (512) //不能超过1400

typedef struct _NVR_NET_ARP_PACK_S_
{
	struct ether_header eh;	//以太网的头部  14
	struct ether_arp ea;	//arp包数据结构 28
} NVR_NET_ARP_PACK_S;		//不包含CRC校验的以太网的帧的最小长度为60个字节

typedef struct _NVR_ARP_IP_CONFLICT_S_
{
	uint32_t u32DevIp;
	ArpIpConflictFunc func;
	gpointer userData;
}NVR_ARP_IP_CONFLICT_S;

// 判断是否有IP冲突需要监控的邋IP地址
static GList *gListenIpList;
static GThread *gArpThread = NULL;
static GAsyncQueue *gQueue = NULL;

G_LOCK_DEFINE_STATIC(ip);

static int arp_pack_send(int hSock,void *pbuf,int size, 
					   struct sockaddr * distAddr, int addrSize)
{
    int  block  = 0;	
    int  alllen = size;
    int  sended = 0;

	if (hSock <= 0 || pbuf == NULL || size <= 0 || distAddr == NULL)
		return 0;
	
    while(alllen > 0)
    {	
		sended = sendto(hSock,pbuf,alllen,0, distAddr, addrSize);
		if(0 == sended)
		{
            return NET_RETURN_FAIL;
		}
		else if(sended < 1)
		{
			if(block > NET_MAX_BLOCK_SEND_TIME)
			{
				return NET_RETURN_FAIL;
			}
			if(errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN)
			{			
				if(block++ < NET_MAX_BLOCK_SEND_TIME)
				{
					usleep(1000);
                    continue;
				}
				else
				{
					break;
				}
			}
			return NET_RETURN_FAIL;
		}
		else
		{		
			pbuf += sended;
			alllen -= sended;
		}
    }
    
    if(alllen > 0)
		return NET_RETURN_FAIL;
    return size;
}

static int nvr_arp_pack_send(const char *pEthName, void *pSendData, int nSendLen)
{
	int sendfd	= 0;
	struct sockaddr_ll send_to;

	if(0 > (sendfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_RARP))))
	{
		j_warning("send socket create failed.");
		return -1;
	}
	
	//发送ARP数据包
	bzero(&send_to, sizeof(struct sockaddr_ll));
	send_to.sll_family  = PF_PACKET;
	send_to.sll_ifindex = if_nametoindex(pEthName);//返回对应接口名的编号

	//发送arp请求包
	
	if(arp_pack_send(sendfd, pSendData, nSendLen, (struct sockaddr *)&send_to, sizeof(send_to)) < 0)
	{
		j_warning("Send arp packet failed.");
		net_close_socket(&sendfd);
		return -1;
	}
	
	net_close_socket(&sendfd);
	
	return 0;
}

static int nvr_arp_pack_recv(int sock, char *pbuf, int bufLen)
{
	int len = 0;
	int ret = 0;
	struct sockaddr_ll  recv_to;
	
	if(NULL == pbuf)
		return -1;
		
	if(bufLen < ETH_FRAME_LEN)
		return -1;
		
	//接收ARP响应包
	len = sizeof(recv_to);
	bzero(pbuf, bufLen);
	bzero(&recv_to, sizeof(recv_to));
	
	//接收数据
	if(0 > (ret = net_udp_recv(sock, pbuf, bufLen, 0,
							   (struct sockaddr*)&recv_to, &len)))
		j_warning("recv failed: %d", ret);
	
	return ret;
}


/*arp数据包初始化:
	pEthName: 网卡名,如eth0,eth1....
	arpCtrlCmd: 操作码，
				请求操作 ARPOP_REQUEST, 
				响应操作 ARPOP_REPLY.
	pArpReq: 	arp数据包.
成功返回0, 失败返回-1.
*/
static int nvr_arp_pack_init(const char *pEthName, int arpCtrlCmd, uint32_t u32DstIp, NVR_NET_ARP_PACK_S *pArpReq)
{
	char ethDstMac[ETH_ALEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//目的MAC
	char ethSrcMac[ETH_ALEN] = {0};
	char srcIp[16] = {0};
	struct in_addr srcAddr;
	
	if(NULL == pArpReq || 
	   NULL == pEthName)
		return -1;
	
	if(ARPOP_REQUEST != arpCtrlCmd &&
	   ARPOP_REPLY   != arpCtrlCmd)
	   return -1;
	
	bzero(pArpReq, sizeof(NVR_NET_ARP_PACK_S));
	
	/* 填写以太网头部*/
	if(0 > nvr_netcfg_get_mac_addr((char*)pEthName, ethSrcMac))
	{
		j_warning("get mac failed.");
		return -1;
	}
	
	memcpy(pArpReq->eh.ether_dhost, ethDstMac, ETH_ALEN);	//源MAC
	memcpy(pArpReq->eh.ether_shost, ethSrcMac, ETH_ALEN);	//目的MAC
	pArpReq->eh.ether_type = htons(ETHERTYPE_ARP);			//协议类型ARP协议
	
	/* 填写arp数据 */
	pArpReq->ea.arp_hrd = htons(ARPHRD_ETHER);		//硬件类型，主机字节序转换成网络字节序
	pArpReq->ea.arp_pro = htons(ETHERTYPE_IP);		//协议类型
	pArpReq->ea.arp_hln = ETH_ALEN;					//MAC地址长度6字节
	pArpReq->ea.arp_pln = 4;						//IP地址长度
	pArpReq->ea.arp_op  = htons(arpCtrlCmd);		//操作码，ARP请求包
	memcpy(pArpReq->ea.arp_sha, ethSrcMac, ETH_ALEN);
	
	//源码IP
	if(0 > nvr_netcfg_get_ip_addr((char*)pEthName, srcIp))
	{
		j_warning("get ipaddr failed.");
		return -1;
	}
	inet_pton(AF_INET, srcIp, &srcAddr);
	memcpy(pArpReq->ea.arp_spa, &srcAddr, sizeof(srcAddr));
	
	//目的IP
	//inet_pton(AF_INET, pDstIp, &dstAddr);
	u32DstIp = htonl(u32DstIp);
	memcpy(pArpReq->ea.arp_tpa, &u32DstIp, sizeof(u32DstIp));
	
	return 0;
}

static int nvr_arp_send_request_pack(uint32_t u32DevIp)
{
	NVR_NET_ARP_PACK_S stArpPack;
	
	if(nvr_arp_pack_init("eth0", ARPOP_REQUEST, u32DevIp, &stArpPack) < 0)
	{
		j_warning("Initialize arp packet failed.");
		return -1;
	}
	if(nvr_arp_pack_send("eth0", &stArpPack, sizeof(stArpPack)) < 0)
	{
		j_warning("Send arp packet failed.");
		return -1;
	}

	return 0;
}

int nvr_arp_check_ip_conflict(uint32_t u32DevIp)
{
	int nRet, nSock;
	int bRecv = 0;
	uint8_t szRecvMac[ETH_ALEN];
	gint64 startTime, curTime;
	NVR_NET_ARP_PACK_S *pArpPack;
	uint8_t szRecvData[2048];
	
	nSock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if(nSock < 0)
	{
		j_warning("Create arp socket failed.");
		return -1;
	}

	if(nvr_arp_send_request_pack(u32DevIp) < 0)
	{
		j_warning("Arp send request packet failed.");
		close(nSock);
		return -1;
	}

	u32DevIp = htonl(u32DevIp);
	startTime = g_get_monotonic_time();
	curTime = startTime;

	do
	{
		nRet = net_select(&nSock, 1, 0x1, 1000);
		if(nRet < 0)
		{
			j_warning("Select arp socket failed.");
			close(nSock);
			return -1;
		}
		else if(nRet == (nSock | 0x10000))
		{
			if((nRet = nvr_arp_pack_recv(nSock, (char *)szRecvData, sizeof(szRecvData))) < 0)
			{
				j_warning("Receive arp data failed.");
				close(nSock);
				return -1;
			}
			pArpPack = (NVR_NET_ARP_PACK_S *)szRecvData;
			//j_debug("arp_spa: %u.%u.%u.%u", 
			//		pArpPack->ea.arp_spa[0], pArpPack->ea.arp_spa[1],
			//		pArpPack->ea.arp_spa[2], pArpPack->ea.arp_spa[3]);
			if(ntohs(pArpPack->ea.arp_op) == ARPOP_REPLY &&
				memcmp(pArpPack->ea.arp_spa, &u32DevIp, 4) == 0)
			{
				if(bRecv == 0)
				{
					bRecv = 1;
					memcpy(szRecvMac, pArpPack->ea.arp_sha, ETH_ALEN);
				}
				else if(memcmp(szRecvMac, pArpPack->ea.arp_sha, ETH_ALEN) != 0)
				{
					close(nSock);
					return 1;
				}
			}
		}
		
		curTime = g_get_monotonic_time();
	}while(curTime - startTime < 1000000);
	close(nSock);

	return 0;
}

static gpointer __nvr_arp_conflict_func(gpointer data)
{
	GList *list;
	NVR_ARP_IP_CONFLICT_S *pIpConflict;
	gpointer cmd;

	while(cmd != NULL)
	{
		G_LOCK(ip);
		for(list = g_list_first(gListenIpList); list != NULL; list = g_list_next(list))
		{
			pIpConflict = (NVR_ARP_IP_CONFLICT_S *)list->data;
			if(nvr_arp_check_ip_conflict(pIpConflict->u32DevIp) == 1)	// ip conflict
			{
				if(pIpConflict->func)
					pIpConflict->func(pIpConflict->u32DevIp, pIpConflict->userData);
			}
		}
		G_UNLOCK(ip);
		
		cmd = g_async_queue_timeout_pop(gQueue, G_USEC_PER_SEC * 5);
	}
	
	return NULL;
}

int nvr_arp_get_mac(uint32_t u32DevIp, uint8_t *pDevMac)
{
	int nRet, nSock;
	gint64 startTime, curTime;
	NVR_NET_ARP_PACK_S *pArpPack;
	uint8_t szRecvData[2048];

	if(u32DevIp == 0)
		return -1;
	nSock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if(nSock < 0)
	{
		j_warning("Create arp socket failed.");
		return -1;
	}
	
	if(nvr_arp_send_request_pack(u32DevIp) < 0)
	{
		j_warning("Arp send request packet failed.");
		close(nSock);
		return -1;
	}
	
	u32DevIp = htonl(u32DevIp);
	startTime = g_get_monotonic_time();
	curTime = startTime;
	
	do
	{
		nRet = net_select(&nSock, 1, 0x1, 1000);
		if(nRet < 0)
		{
			j_warning("Select arp socket failed.");
			close(nSock);
			return -1;
		}
		else if(nRet == (nSock | 0x10000))
		{
			if((nRet = nvr_arp_pack_recv(nSock, (char *)szRecvData, sizeof(szRecvData))) < 0)
			{
				j_warning("Receive arp data failed.");
				close(nSock);
				return -1;
			}
			pArpPack = (NVR_NET_ARP_PACK_S *)szRecvData;
			//j_debug("arp_spa: %u.%u.%u.%u", 
			//		pArpPack->ea.arp_spa[0], pArpPack->ea.arp_spa[1],
			//		pArpPack->ea.arp_spa[2], pArpPack->ea.arp_spa[3]);
			if(ntohs(pArpPack->ea.arp_op) == ARPOP_REPLY &&
				memcmp(pArpPack->ea.arp_spa, &u32DevIp, 4) == 0)
			{
				memcpy(pDevMac, pArpPack->ea.arp_sha, ETH_ALEN);
				close(nSock);
				return ETH_ALEN;
			}
		}
		else
		{
			nvr_arp_send_request_pack(u32DevIp);
		}
		
		curTime = g_get_monotonic_time();
	}while(curTime - startTime < 1000000);
	close(nSock);

	return -1;
}

void nvr_arp_add_listen_ip(uint32_t u32DevIp, ArpIpConflictFunc func, gpointer userData)
{
	NVR_ARP_IP_CONFLICT_S *pIpConflict;

	pIpConflict = g_malloc0(sizeof(NVR_ARP_IP_CONFLICT_S));
	pIpConflict->u32DevIp = u32DevIp;
	pIpConflict->func = func;
	pIpConflict->userData = userData;
	
	G_LOCK(ip);
	gListenIpList = g_list_prepend(gListenIpList, pIpConflict);
	G_UNLOCK(ip);
}

void nvr_arp_remove_listen_ip(uint32_t u32DevIp)
{
	GList *list;
	NVR_ARP_IP_CONFLICT_S *pIpConflict;
	
	G_LOCK(ip);
	for(list = g_list_first(gListenIpList); list != NULL; list = g_list_next(list))
	{
		pIpConflict = (NVR_ARP_IP_CONFLICT_S *)list->data;
		if(pIpConflict->u32DevIp == u32DevIp)
		{
			gListenIpList = g_list_delete_link(gListenIpList, list);
			g_free(pIpConflict);
			break;
		}
	}
	G_UNLOCK(ip);
}

int nvr_arp_init()
{
	gQueue = g_async_queue_new ();
	
	// 因为此线程计算一个周期比较消耗时间，
	// 所以不在定时器里面实现这个功能
	gArpThread = g_thread_new("ARP", __nvr_arp_conflict_func, NULL);

	return 0;
}

void nvr_arp_uninit()
{
	g_async_queue_push(gQueue, GINT_TO_POINTER(1));
	g_thread_join(gArpThread);
	g_async_queue_unref(gQueue);
	
	gListenIpList = g_list_remove_all(gListenIpList, g_free);
}


