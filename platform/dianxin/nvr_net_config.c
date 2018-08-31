

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/route.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>



static const char WFNAME[32] = "eth0"; 

typedef unsigned long long u64;
typedef unsigned int	u32;
typedef unsigned short	u16;
typedef unsigned char	u8;

typedef unsigned long long __u64;
typedef int				__s32;
typedef short			__s16;
typedef char			_s8;
typedef unsigned int	__u32;
typedef unsigned short	__u16;
typedef unsigned char	__u8;

#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <linux/if_ether.h>
#include <stddef.h>		/* offsetof */

#include "nvr_net_config.h"

#define ifreq_offsetof(x)  offsetof(struct ifreq, x)

//#define IFF_DYNAMIC     0x8000  /* dialup device with changing addresses */

#define N_CLR            0x01
#define M_CLR            0x02
#define N_SET            0x04
#define M_SET            0x08
#define N_ARG            0x10
#define M_ARG            0x20

#define M_MASK           (M_CLR | M_SET | M_ARG)
#define N_MASK           (N_CLR | N_SET | N_ARG)
#define SET_MASK         (N_SET | M_SET)
#define CLR_MASK         (N_CLR | M_CLR)
#define SET_CLR_MASK     (SET_MASK | CLR_MASK)
#define ARG_MASK         (M_ARG | N_ARG)

/*
 * Here are the bit masks for the "arg_flags" member of struct options below.
 */

/*
 * cast type:
 *   00 int
 *   01 char *
 *   02 HOST_COPY in_ether
 *   03 HOST_COPY INET_resolve
 */
#define A_CAST_TYPE      0x03
/*
 * map type:
 *   00 not a map type (mem_start, io_addr, irq)
 *   04 memstart (unsigned long)
 *   08 io_addr  (unsigned short)
 *   0C irq      (unsigned char)
 */
#define A_MAP_TYPE       0x0C
#define A_ARG_REQ        0x10	/* Set if an arg is required. */
#define A_NETMASK        0x20	/* Set if netmask (check for multiple sets). */
#define A_SET_AFTER      0x40	/* Set a flag at the end. */
#define A_COLON_CHK      0x80	/* Is this needed?  See below. */

/*
 * These defines are for dealing with the A_CAST_TYPE field.
 */
#define A_CAST_CHAR_PTR  0x01
#define A_CAST_RESOLVE   0x01
#define A_CAST_HOST_COPY 0x02
#define A_CAST_HOST_COPY_IN_ETHER    A_CAST_HOST_COPY
#define A_CAST_HOST_COPY_RESOLVE     (A_CAST_HOST_COPY | A_CAST_RESOLVE)

/*
 * These defines are for dealing with the A_MAP_TYPE field.
 */
#define A_MAP_ULONG      0x04	/* memstart */
#define A_MAP_USHORT     0x08	/* io_addr */
#define A_MAP_UCHAR      0x0C	/* irq */

/*
 * Define the bit masks signifying which operations to perform for each arg.
 */

#define ARG_METRIC       (A_ARG_REQ /*| A_CAST_INT*/)
#define ARG_MTU          (A_ARG_REQ /*| A_CAST_INT*/)
#define ARG_TXQUEUELEN   (A_ARG_REQ /*| A_CAST_INT*/)
#define ARG_MEM_START    (A_ARG_REQ | A_MAP_ULONG)
#define ARG_IO_ADDR      (A_ARG_REQ | A_MAP_ULONG)
#define ARG_IRQ          (A_ARG_REQ | A_MAP_UCHAR)
#define ARG_DSTADDR      (A_ARG_REQ | A_CAST_HOST_COPY_RESOLVE)
#define ARG_NETMASK      (A_ARG_REQ | A_CAST_HOST_COPY_RESOLVE | A_NETMASK)
#define ARG_BROADCAST    (A_ARG_REQ | A_CAST_HOST_COPY_RESOLVE | A_SET_AFTER)
#define ARG_HW           (A_ARG_REQ | A_CAST_HOST_COPY_IN_ETHER)
#define ARG_POINTOPOINT  (A_CAST_HOST_COPY_RESOLVE | A_SET_AFTER)
#define ARG_KEEPALIVE    (A_ARG_REQ | A_CAST_CHAR_PTR)
#define ARG_OUTFILL      (A_ARG_REQ | A_CAST_CHAR_PTR)
#define ARG_HOSTNAME     (A_CAST_HOST_COPY_RESOLVE | A_SET_AFTER | A_COLON_CHK)
/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
//拷贝自:busybox(ifconfig.c)
//增加设置网卡物理地址及ifconfig_up_down需要
struct arg1opt 
{
	const char *name;
	unsigned short selector;
	unsigned short ifr_offset;
};

struct options 
{
	const char *name;
	const unsigned char  flags;
	const unsigned char  arg_flags;
	const unsigned short selector;
};

static const struct arg1opt Arg1Opt[] = {
	{"SIOCSIFMETRIC",  SIOCSIFMETRIC,  ifreq_offsetof(ifr_metric)},
	{"SIOCSIFMTU",     SIOCSIFMTU,     ifreq_offsetof(ifr_mtu)},
	{"SIOCSIFTXQLEN",  SIOCSIFTXQLEN,  ifreq_offsetof(ifr_qlen)},
	{"SIOCSIFDSTADDR", SIOCSIFDSTADDR, ifreq_offsetof(ifr_dstaddr)},
	{"SIOCSIFNETMASK", SIOCSIFNETMASK, ifreq_offsetof(ifr_netmask)},
	{"SIOCSIFBRDADDR", SIOCSIFBRDADDR, ifreq_offsetof(ifr_broadaddr)},
	{"SIOCSIFHWADDR",  SIOCSIFHWADDR,  ifreq_offsetof(ifr_hwaddr)},
	{"SIOCSIFDSTADDR", SIOCSIFDSTADDR, ifreq_offsetof(ifr_dstaddr)},
	{"SIOCSIFMAP",     SIOCSIFMAP,     ifreq_offsetof(ifr_map.mem_start)},
	{"SIOCSIFMAP",     SIOCSIFMAP,     ifreq_offsetof(ifr_map.base_addr)},
	{"SIOCSIFMAP",     SIOCSIFMAP,     ifreq_offsetof(ifr_map.irq)},
	/* Last entry if for unmatched (possibly hostname) arg. */
	{"SIOCSIFADDR",    SIOCSIFADDR,    ifreq_offsetof(ifr_addr)},
};


static const struct options OptArray[] = {
	{"metric",       N_ARG,         ARG_METRIC,      0},
    {"mtu",          N_ARG,         ARG_MTU,         0},
	{"txqueuelen",   N_ARG,         ARG_TXQUEUELEN,  0},
	{"dstaddr",      N_ARG,         ARG_DSTADDR,     0},
	{"netmask",      N_ARG,         ARG_NETMASK,     0},
	{"broadcast",    N_ARG | M_CLR, ARG_BROADCAST,   IFF_BROADCAST},
	{"hw",           N_ARG,         ARG_HW,          0},
	{"pointopoint",  N_ARG | M_CLR, ARG_POINTOPOINT, IFF_POINTOPOINT},
	{"mem_start",    N_ARG,         ARG_MEM_START,   0},
	{"io_addr",      N_ARG,         ARG_IO_ADDR,     0},
	{"irq",          N_ARG,         ARG_IRQ,         0},
	{"arp",          N_CLR | M_SET, 0,               IFF_NOARP},
	{"trailers",     N_CLR | M_SET, 0,               IFF_NOTRAILERS},
	{"promisc",      N_SET | M_CLR, 0,               IFF_PROMISC},
	{"multicast",    N_SET | M_CLR, 0,               IFF_MULTICAST},
	{"allmulti",     N_SET | M_CLR, 0,               IFF_ALLMULTI},
	{"dynamic",      N_SET | M_CLR, 0,               IFF_DYNAMIC},
	{"up",           N_SET        , 0,               (IFF_UP | IFF_RUNNING)},
	{"down",         N_CLR        , 0,               IFF_UP},
	{ NULL,          0,             ARG_HOSTNAME,    (IFF_UP | IFF_RUNNING)}
};
/*----------------------------------------------*
 * 函数实现                                     *
 *----------------------------------------------*/
//============================================================================
//* 函数名称: in_ether()
//* 功能描述: 地址转换
//* 其他说明:
//============================================================================
static int in_ether(char *bufp, struct sockaddr *sap)
{
	unsigned char *ptr;
	int i, j;
	unsigned char val;
	unsigned char c;

	sap->sa_family = ARPHRD_ETHER;
	ptr = (unsigned char *)sap->sa_data;

	i = 0;
	do {
		j = val = 0;

		/* We might get a semicolon here - not required. */
		if (i && (*bufp == ':')) {
			bufp++;
		}

		do {
			c = *bufp;
			if (((unsigned char)(c - '0')) <= 9) {
				c -= '0';
			} else if (((unsigned char)((c|0x20) - 'a')) <= 5) {
				c = (c|0x20) - ('a'-10);
			} else if (j && (c == ':' || c == 0)) {
				break;
			} else {
				return -1;
			}
			++bufp;
			val <<= 4;
			val += c;
		} while (++j < 2);
		*ptr++ = val;
	} while (++i < ETH_ALEN);

	return (int) (*bufp);	/* Error if we don't end at end of string. */
}

//============================================================================
//
//============================================================================
char *safe_strncpy(char *dst, const char *src, size_t size)
{
    dst[size-1] = '\0';
    return strncpy(dst, src, size-1);
}

//============================================================================
// 函数名称: nvr_NetConfig_SetPhyAddr()
// 功能描述: 设置指定名字网卡的物理地址
// 其他说明:
//============================================================================
int nvr_netcfg_set_mac_addr(char *name,char *addr)  // name : eth0 eth1 lo and so on
{												  // addr : 12:13:14:15:16:17
	const struct arg1opt *a1op;
	const struct options *op;
	int 	sockfd;
	struct 	ifreq ifr;
	struct 	sockaddr sa;
	unsigned char mask;
	char 	*p = "hw";
	char 	host[128];
	
	/* Create a channel to the NET kernel. */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}
	
	/* get interface name */
	safe_strncpy(ifr.ifr_name, name, IFNAMSIZ);
	mask = N_MASK;
	
	for (op = OptArray ; op->name ; op++)
	{		
		if (strcmp("hw",op->name) == 0) 
		{
			if ((mask &= op->flags)) 
			{ 
				goto SET_NET_PHYADDR_FOUND_ARG;
			}
		}
	}
	close(sockfd);
	return -4;
	
SET_NET_PHYADDR_FOUND_ARG:
	
	a1op = Arg1Opt + 6;
	
	safe_strncpy(host, addr, (sizeof host));
	
	if (in_ether(host, &sa)) 
	{
		close(sockfd);
		return -2;
	}
	p = (char *) &sa;
	
	memcpy((((char *) (&ifr)) + a1op->ifr_offset), p, sizeof(struct sockaddr));
	
	if (ioctl(sockfd, a1op->selector, &ifr) < 0)
	{
		close(sockfd);
		return -3;
	}
	
	close(sockfd);

	return  0;
}
 
int nvr_netcfg_active(char *name, char *action) // name :  eth0 eth1 lo and so on
{											   // action: down up
	const struct options *op;
	int 	sockfd;
	int 	selector;
	struct 	ifreq ifr;
	unsigned char mask;

	/* Create a channel to the NET kernel. */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}

	/* get interface name */
	safe_strncpy(ifr.ifr_name, name, IFNAMSIZ);
	mask = N_MASK;

	for (op = OptArray ; op->name ; op++)
	{
			if (strcmp(action,op->name) == 0)
			{
				if ((mask &= op->flags))
				{
				    goto IFCONFIG_UP_DOWN_FOUND_ARG;
				}
			}
	}
	close(sockfd);
	return -4;

IFCONFIG_UP_DOWN_FOUND_ARG:

	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
	{
		close(sockfd);
		return -2;
	}
	else
	{
		selector = op->selector;
		if (mask & SET_MASK)
		{
			ifr.ifr_flags |= selector;
		}
		else
		{
			ifr.ifr_flags &= ~selector;
		}
		if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) < 0)
		{
			close(sockfd);
			return -3;
		}
	}

	close(sockfd);
	return  0;
}

//============================================================================
//
//============================================================================
int nvr_netcfg_status(char *name,unsigned int *flag)
{
	int 	sockfd;
	struct 	ifreq ifr;

	*flag = 0;
	/* Create a channel to the NET kernel. */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}

	/* get interface name */
	safe_strncpy(ifr.ifr_name, name, IFNAMSIZ);

	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
	{
		close(sockfd);
		return -2;
	}

	*flag = ifr.ifr_flags;

	return 0;
}

// if_name like "ath0", "eth0". Notice: call this function
// need root privilege.
// return value:
// -1 -- error , details can check errno
// 1 -- interface link up
// 0 -- interface link down.int get_netlink_status(const char * const if_name)
//只能获取到有线网口的物理连接状态
int nvr_netcfg_netlink_status(const char * const if_name)
{
    int skfd;
    struct ifreq ifr;
    struct ethtool_value edata;

    edata.cmd = ETHTOOL_GLINK;
    edata.data = 0;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name) - 1);
    ifr.ifr_data = (char *) &edata;

    if (( skfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0)
        return -1;

    if(ioctl( skfd, SIOCETHTOOL, &ifr ) == -1)
    {
        close(skfd);
        return -1;
    }

    close(skfd);
    return edata.data;
}

//============================================================================
// 函数名称: nvr_NetConfig_GetIpAddr()
// 功能描述: 得到指定名字的网络ip
// 其他说明:
//============================================================================
int nvr_netcfg_get_ip_addr(char *name,char *net_ip)
{
	struct ifreq ifr;
	int ret = 0;
	int fd;
	
	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) 
	{
		ret = -1;
	}
	else
		strcpy(net_ip,inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);

	return	ret;
}

//============================================================================
//获得网卡物理地址
//============================================================================
int nvr_netcfg_get_mac_addr(char *name, char *net_mac)
{
	struct ifreq ifr;
	int ret = 0;
	int fd;

	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;

	ret = ioctl(fd, SIOCGIFHWADDR,(char *)&ifr,sizeof(ifr));
    if(ret == 0)
	{
        memcpy(net_mac,ifr.ifr_hwaddr.sa_data,6);
	}

	close(fd);

	return	ret;
}

//============================================================================
// 函数名称: nvr_NetConfig_GetMaskAddr()
// 功能描述: 得到指定名字的子网掩码
// 其他说明:
//============================================================================
int nvr_netcfg_get_mask_addr(char *name,char *net_mask)
{
	struct ifreq ifr;
	int ret = 0;
	int fd;	
	
	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	if (ioctl(fd, SIOCGIFNETMASK, &ifr) < 0) 
	{
		ret = -1;
	}
	
	strcpy(net_mask,inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);

	return	ret;
}

//============================================================================
// 函数名称: nvr_NetConfig_GetBrdcastAddr()
// 功能描述: 得到指定名字的网络广播地址
// 其他说明:
//============================================================================
int nvr_netcfg_get_boardcast_addr(char *name,char *net_brdaddr)
{
	struct ifreq ifr;
	int ret = 0;
	int fd;	
	
	strcpy(ifr.ifr_name, name);
	ifr.ifr_addr.sa_family = AF_INET;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	if (ioctl(fd, SIOCGIFBRDADDR, &ifr) < 0) 
	{
		ret = -1;
	}
	
	strcpy(net_brdaddr,inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);


	return	ret;
}

//============================================================================
// 函数名称: nvr_NetConfig_SetIpAddr()
// 功能描述: 设置指定名字网络的IP地址
// 其他说明:
//============================================================================
int nvr_netcfg_set_ip_addr(char *name, char *net_ip)
{	
	struct sockaddr	addr;
	struct ifreq ifr;
	char gateway_addr[32] = {0};
	int ret = 0;
	int fd;	
	int nMetric;
	
	((struct sockaddr_in *)&(addr))->sin_family = PF_INET;
	((struct sockaddr_in *)&(addr))->sin_addr.s_addr = inet_addr(net_ip);

	ifr.ifr_addr = addr;
	strcpy(ifr.ifr_name,name);

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;

	nvr_netcfg_get_gw_addr(name,gateway_addr, &nMetric);		/* 在设置IP时会清除网络,所以先保存好网络设置 */

	if (ioctl(fd, SIOCSIFADDR, &ifr) != 0) 
	{
		ret = -1;
	}

	close(fd);

	nvr_netcfg_set_gw_addr(name, gateway_addr, nMetric);		/*恢复网络的网关设置 */

	return	ret;
}

//============================================================================
// 函数名称: nvr_NetConfig_SetMaskAddr()
// 功能描述: 设置指定名字网络的子网掩码
// 其他说明:
//============================================================================
int nvr_netcfg_set_mask_addr(char *name,char *mask_ip) /* name : eth0 eth1 lo and so on */
{	
	struct sockaddr	addr;
	struct ifreq ifr;
	char gateway_addr[32];
	int ret = 0;
	int fd;	
	int nMetric;
	
	((struct sockaddr_in *)&(addr))->sin_family = PF_INET;
	((struct sockaddr_in *)&(addr))->sin_addr.s_addr = inet_addr(mask_ip);
	ifr.ifr_netmask = addr;
	strcpy(ifr.ifr_name,name);

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
		
	nvr_netcfg_get_gw_addr(name,gateway_addr, &nMetric); /* 在设置IP时会清除网络,所以先保存好网络设置 */

	if (ioctl(fd, SIOCSIFNETMASK, &ifr) != 0) 
	{
		ret = -1;
	}

	close(fd);

	nvr_netcfg_set_gw_addr(name, gateway_addr, nMetric); /*恢复网络的网关设置 */

	return	ret;
}

//============================================================================
// 函数名称: nvr_NetConfig_GetGwAddr()
// 功能描述: 得到默认网关
// 其他说明:
//============================================================================
int nvr_netcfg_get_gw_addr(char *name,char *gateway_addr, int *pMetric)
{
	char buff[256];
	char ifname[32] = {0};
	int  nl = 0 ;
	struct in_addr gw;
	int flgs, ref, use, metric;
	unsigned  long d,g,m;

	FILE	*fp;
	
	if((fp = fopen("/proc/net/route", "r")) == NULL)
		return -1;	
		
	nl = 0 ;
	while( fgets(buff, sizeof(buff), fp) != NULL ) 
	{
		if(nl) 
		{
			if(sscanf(buff, "%s%lx%lx%X%d%d%d%lx",
				   ifname,&d, &g, &flgs, &ref, &use, &metric, &m)!=8) 
			{
				//continue;
				fclose(fp);
				return	-2;
			}

			if(flgs&RTF_UP && (strcmp(name,ifname)== 0)) 
			{			
				gw.s_addr   = g;
					
				if(d==0)
				{
					strcpy(gateway_addr,inet_ntoa(gw));
					*pMetric = metric;
					fclose(fp);
					return 0;
				}				
			}

		}
		nl++;
	}	
	
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
	
	return	-1;
}

//============================================================================
// 函数名称: nvr_NetConfig_SetGwAddr()
// 功能描述: 设置默认网关(gateway)
// 其他说明:
//============================================================================
int nvr_netcfg_set_gw_addr(char *name,char *gateway_addr, int nMetric)
{
	char old_gateway_addr[32];
	struct rtentry rt;
	unsigned long gw;
	int fd;
	int ret = 0;
	int nOldMetric;
	
	nvr_netcfg_get_gw_addr(name, old_gateway_addr, &nOldMetric);

	nvr_netcfg_del_gw_addr(name,old_gateway_addr);
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		ret = -1;
	}	

	gw = inet_addr(gateway_addr);
	memset((char *) &rt, 0, sizeof(struct rtentry));

	((struct sockaddr_in *)&(rt.rt_dst))->sin_addr.s_addr = 0;

	rt.rt_flags = RTF_UP | RTF_GATEWAY ;
	//rt.rt_flags = 0x03;

	((struct sockaddr_in *)&(rt.rt_dst))->sin_family = PF_INET;
	
	((struct sockaddr_in *)&(rt.rt_gateway))->sin_addr.s_addr = gw;	
	((struct sockaddr_in *)&(rt.rt_gateway))->sin_family = PF_INET;
	

	((struct sockaddr_in *)&(rt.rt_genmask))->sin_addr.s_addr = 0;
	((struct sockaddr_in *)&(rt.rt_genmask))->sin_family = PF_INET;
	rt.rt_dev = name;
	rt.rt_metric = nMetric + 1;

	if (ioctl(fd, SIOCADDRT, &rt) < 0)
	{
		ret = -1;
	}
	close(fd);
	return	ret;
}


//============================================================================
// 函数名称: nvr_NetConfig_DelGwAddr()
// 功能描述: 删除默认网关(gateway) 
// 其他说明:
//============================================================================
int nvr_netcfg_del_gw_addr(char *name,char *gateway_addr)
{
	struct rtentry rt;
	unsigned long gw;
	int ret = 0;
	int fd;
	
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0)
		ret = -1;
	
	gw = inet_addr(gateway_addr);
	memset((char *) &rt, 0, sizeof(struct rtentry));

	rt.rt_flags = RTF_UP | RTF_GATEWAY ;
	
	((struct sockaddr_in *)&(rt.rt_dst))->sin_family = PF_INET;
	
	((struct sockaddr_in *)&(rt.rt_gateway))->sin_addr.s_addr = gw;	
	((struct sockaddr_in *)&(rt.rt_gateway))->sin_family = PF_INET;

	
	((struct sockaddr_in *)&(rt.rt_genmask))->sin_addr.s_addr = 0;
	((struct sockaddr_in *)&(rt.rt_genmask))->sin_family = PF_INET;
	
	rt.rt_dev = name;
		
	if (ioctl(fd, SIOCDELRT, &rt) < 0) 	
	{
		ret = -1;
	}

	close(fd);

	return	ret;
}

//============================================================================
// 函数名称: HH_NETTOOL_CheckListenPort()
//============================================================================
#define _PATH_PROCNET_UDP  "/proc/net/udp"
#define _PATH_PROCNET_UDP6 "/proc/net/udp6"
#define _PATH_PROCNET_TCP  "/proc/net/tcp"
#define _PATH_PROCNET_TCP6 "/proc/net/tcp6"
#define _PATH_PROCNET_RAW  "/proc/net/raw"
#define _PATH_PROCNET_RAW6 "/proc/net/raw6"
#define _PATH_PROCNET_UNIX "/proc/net/unix"

static const char * const tcp_state[] =
{
    "",
    "ESTABLISHED",
    "SYN_SENT",
    "SYN_RECV",
    "FIN_WAIT1",
    "FIN_WAIT2",
    "TIME_WAIT",
    "CLOSE",
    "CLOSE_WAIT",
    "LAST_ACK",
    "LISTEN",
    "CLOSING"
};

#define NETSTAT_NONE			0  
#define NETSTAT_ESTABLISHED		1
#define NETSTAT_SYN_SENT		2
#define NETSTAT_SYN_RECV		3
#define NETSTAT_FIN_WAIT1		4
#define NETSTAT_FIN_WAIT2		5
#define NETSTAT_TIME_WAIT		6
#define NETSTAT_CLOSE			7
#define NETSTAT_CLOSE_WAIT		8
#define NETSTAT_LAST_ACK		9
#define NETSTAT_LISTEN			10 
#define NETSTAT_CLOSING			11

int find_listen_port(int lnr, const char *line, int nPort)
{
	char more[512];
	int           num, local_port, rem_port, index, state, timer_run, uid, timeout;
	unsigned long rxq, txq, time_len, retr, inode;

	unsigned int local_addr, rem_addr;
	if (lnr == 0)
		return -1;

	more[0] = '\0'; 

	num = sscanf(line,
				 "%d: %X:%X %X:%X %X %lX:%lX %X:%lX %lX %d %d %ld %512s\n",
				 &index, 
				 &local_addr, &local_port,
				 &rem_addr,   &rem_port, 
				 &state,
				 &txq, &rxq, 
				 &timer_run, &time_len, &retr, &uid, &timeout, &inode, more);
	if (num < 10) 
	{
		return -1;
	}

	if( state == NETSTAT_LISTEN  && local_port == nPort)
	{
		return 1;
	}

	return 0;
}


int nvr_netcfg_find_listen_port(int nPort)
{
	char  buffer[1024];
	FILE *procinfo;
	int   nFind = -1;
	int   lnr = 0;

	if ((procinfo = fopen(_PATH_PROCNET_TCP, "r")) == NULL) 
	{
		return -1;
	}	

	do 
	{
		if (fgets(buffer, sizeof(buffer), procinfo) )
		{
			buffer[sizeof(buffer) - 1] = 0;
			if (find_listen_port(lnr, buffer, nPort) > 0)
			{
				nFind = 0;
			}
			lnr += 1;
		}
	} while (!feof(procinfo));

	fclose(procinfo);

	return nFind;
}

//-----------------------------------------------------------------------------
//将DNS参数写到文件
//-----------------------------------------------------------------------------
int nvr_netcfg_update_dns(char *cfgFile, unsigned int dnsaddr, unsigned int dnsaddr2)
{
	char str[128] = {0};
	FILE *pFile;

	
	if((pFile = fopen(cfgFile, "w+b")) ==NULL)
	{
		return -1;
	}
	
	memset(str, 0, 128);
	sprintf(str, "search localdomain\n");
	fputs(str, pFile);

	if (dnsaddr != 0)
	{
		memset(str, 0, 128);
		sprintf(str, "nameserver %d.%d.%d.%d\n",  (dnsaddr & 0xff000000) >> 24,	
											      (dnsaddr & 0xff0000) >> 16, 
											      (dnsaddr & 0xff00) >> 8,
											      (dnsaddr & 0xff) >> 0);
		fputs(str, pFile);
	}

	if (dnsaddr2 != 0)
	{
		memset(str, 0, 128);
		sprintf(str, "nameserver %d.%d.%d.%d\n",  (dnsaddr2 & 0xff000000) >> 24,	
											      (dnsaddr2 & 0xff0000) >> 16, 
											      (dnsaddr2 & 0xff00) >> 8,
											      (dnsaddr2 & 0xff) >> 0);	
		fputs(str, pFile);
	}	

	fclose(pFile);
	return 0;
}

//-----------------------------------------------------------------------------
//从文件读回DNS
//-----------------------------------------------------------------------------
int nvr_netcfg_get_dns(char *cfgFile, unsigned int *pDns, unsigned int *pDns2)
{
	char szStr[128] = {0};
	char ss[128]  = {0};
	FILE *pFile;
	int  nArray[4]   = {0};
	unsigned int mIP = 0;
	int i = 0;
		
	if((pFile = fopen(cfgFile, "r+b")) == NULL)
		return -1;
	

	while(fgets(szStr, 128, pFile) != NULL)
	{
		if(strstr(szStr,"nameserver") == NULL)
			continue ;
		
		if(sscanf(szStr, "%s %d.%d.%d.%d", ss, &(nArray[0]), &(nArray[1]),
			&(nArray[2]), &(nArray[3])) == 5)
		{
			mIP = (nArray[0] << 24) | (nArray[1] << 16) |
				  (nArray[2] << 8)  | (nArray[3] << 0);
			if(i == 0 && pDns != NULL)
			{
				*pDns = mIP;
			}
			else if (pDns2 != NULL)
			{
				*pDns2 = mIP;
			}
			i++;				
		}
		memset(szStr, 0, 128);
	}
	fclose(pFile);
	return 0;
}

