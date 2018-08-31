
#include <dhcp.h>

#include "nvr_net_man.h"
#include "nvr_net_config.h"
#include "nvr_param_proc.h"


#define DNS_CFG_FILE	"/tmp/resolv.conf"

#define ETH0_METRIC 5
#define WIFI_METRIC 10

typedef struct _NVR_NET_MAN_INFO_S_
{
	uint8_t	u8PppoeStatus;	// 0:未拨号，1:已经拨号
	
	uint8_t	u8PppoeOpen;
	gint64	s64PppoeTimes;
	uint32_t	u32PppoeIp;
	uint32_t	u32Change;	
	NVR_DEV_PPPOE_CFG_S stPppoeCfg;
	
	gint		pppoeTimeId;
}NVR_NET_MAN_INFO_S, *LPNVR_NET_MAN_INFO_S;

static NVR_NET_MAN_INFO_S gNetManInfo;

#define	PPPOE_CHECK_INTERVAL		5000

int nvr_net_pppoe_start(NVR_DEV_PPPOE_CFG_S *pPppoeCfg);
void nvr_net_pppoe_stop();


static int nvr_net_is_nfs_boot()
{
	FILE *fp;
	char cmdline[1024];
	int nfs = 0;

	fp = fopen("/proc/cmdline", "r");
	if(fp != NULL)
	{
		fgets(cmdline, sizeof(cmdline), fp);
		j_debug("cmdline: %s", cmdline);
		if(strstr(cmdline, "root=/dev/nfs"))
			nfs = 1;
		fclose(fp);
	}
	return nfs;
}

static int nvr_net_set_mac_addr(char *pName, uint8_t *pMacAddr)
{
	char szMacAddr[32] = {0};
	int  nRet = 0;
	int  nCount = 0;

	if (pName == NULL || pMacAddr == NULL)
		return NVR_FAILURE;
	memset(szMacAddr, 0, sizeof(szMacAddr));

	sprintf(szMacAddr, "%02x:%02x:%02x:%02x:%02x:%02x",
						pMacAddr[0], pMacAddr[1], pMacAddr[2],
						pMacAddr[3], pMacAddr[4], pMacAddr[5]);
	j_message("set %s mac addr %s", pName, szMacAddr);
	do
	{
		nRet = 0;
		nRet |= nvr_netcfg_active(pName, (char *)"down");
		nRet |= nvr_netcfg_set_mac_addr(pName, szMacAddr);
		nRet |= nvr_netcfg_active(pName, (char *)"up");
	}while((nRet != 0) && (++nCount < 10));	

	j_message("set %s macAddr %s", pName, (nRet == 0) ? "ok" : "failed");
	return nRet;
}

static int	nvr_net_man_update_dns(uint32_t u32Dns1, uint32_t u32Dns2)
{
	j_message( "config main ddns %x sub ddns %x",u32Dns1, u32Dns2);
	return nvr_netcfg_update_dns(DNS_CFG_FILE, u32Dns1, u32Dns2);
}

static int nvr_net_man_update_pppoe(NVR_DEV_PPPOE_CFG_S *pPppoeCfg)
{	
	if(gNetManInfo.u8PppoeStatus == 0)
	{
		if(pPppoeCfg->u8PppoeOn)
		{
			nvr_net_pppoe_start(pPppoeCfg);
		}
	}
	else
	{
		if(pPppoeCfg->u8PppoeOn == 0)
		{
			nvr_net_pppoe_stop();
		}
		else
		{
			if(g_strcmp0(pPppoeCfg->szPppoeUsr, gNetManInfo.stPppoeCfg.szPppoeUsr) != 0
				|| g_strcmp0(pPppoeCfg->szPppoePsw, gNetManInfo.stPppoeCfg.szPppoePsw) != 0
				|| pPppoeCfg->u8PppoeEth != gNetManInfo.stPppoeCfg.u8PppoeEth)
			{
				nvr_net_pppoe_stop();
				nvr_net_pppoe_start(pPppoeCfg);
			}
		}
	}
	return 0;
}

static gboolean nvr_net_dhcp_timeout(gpointer user_data)
{
	int nErr = 0;
	static dhcpHandle *eth0DhcpHandle = NULL;
	unsigned long ipAddr, netmask, gateway, dns1, dns2;
	char szIfName[32];
	char szIp[32] = {0}, szMask[32] = {0}, szGw[32] = {0};
	char *p;
	NVR_DEV_NET_CFG_S stNetCfg;
	
	if(eth0DhcpHandle != NULL)
		dhcpLibRelease(eth0DhcpHandle);
	eth0DhcpHandle = dhcpLibRequest(NULL, &ipAddr, &netmask, &gateway, &dns1, &dns2);
	if(eth0DhcpHandle == NULL)
		return G_SOURCE_CONTINUE;
	net_ip_n2a(ipAddr, szIp, sizeof(szIp));
	net_ip_n2a(netmask, szMask, sizeof(szMask));
	net_ip_n2a(gateway, szGw, sizeof(szGw));
	g_strlcpy(szIfName, (const char *)user_data, sizeof(szIfName));
	j_message( "config local net %s ip %s szMask %s gw %s ", szIfName, szIp, szMask, szGw);
	if ((nErr = nvr_netcfg_set_ip_addr(szIfName, szIp)) != 0 ||
		(nErr = nvr_netcfg_set_mask_addr(szIfName, szMask)) != 0)
	{
		j_warning( "config local net interface %s error", szIfName);
            dhcpLibRelease(eth0DhcpHandle);
            eth0DhcpHandle = NULL;
		return G_SOURCE_CONTINUE;
	}
	if((p = strchr(szIfName, ':')) != NULL)
		*p = 0;
	if((nErr = nvr_netcfg_set_gw_addr(szIfName, szGw, ETH0_METRIC)) != 0)
	{
		j_warning("config local gateway interface %s error", szIfName);
            dhcpLibRelease(eth0DhcpHandle);
            eth0DhcpHandle = NULL;
		return G_SOURCE_CONTINUE;
	}
	// pppoe打开了则不使用dhcp的dns
	nvr_get_net_cfg(0, &stNetCfg);
	if(!stNetCfg.struPppoeCfg.u8PppoeOn)
		nvr_net_man_update_dns(dns1, dns2);
	
	return G_SOURCE_REMOVE;
}

static int nvr_net_config_lan_net(NVR_DEV_NET_CFG_S *pNetCfg, int *pChange)
{
	int nErr = 0;
	char szIp[32] = {0}, szMask[32] = {0}, szGw[32] = {0};
	int nMetric;

#ifdef ARCH_X86
	nvr_netcfg_get_ip_addr((char *)"eth2", szIp);
	pNetCfg->struEtherCfg[0].u32IpAddr = net_ip_a2n(szIp);
	
	nvr_netcfg_get_mask_addr((char *)"eth2", szMask);
	pNetCfg->struEtherCfg[0].u32SubMask = net_ip_a2n(szMask);
	
	nvr_netcfg_get_gw_addr((char *)"eth2", szGw, &nMetric);
	pNetCfg->struEtherCfg[0].u32GateWay = net_ip_a2n(szGw);
	*pChange = 1;
	
	return 0;
#endif

	if(nvr_net_is_nfs_boot())
	{
		nvr_netcfg_get_ip_addr((char *)"eth0", szIp);
		pNetCfg->struEtherCfg[0].u32IpAddr = net_ip_a2n(szIp);
		
		nvr_netcfg_get_mask_addr((char *)"eth0", szMask);
		pNetCfg->struEtherCfg[0].u32SubMask = net_ip_a2n(szMask);
		
		nvr_netcfg_get_gw_addr((char *)"eth0", szGw, &nMetric);
		pNetCfg->struEtherCfg[0].u32GateWay = net_ip_a2n(szGw);
		
		nvr_netcfg_get_mac_addr((char *)"eth0", (char *)&pNetCfg->struEtherCfg[0].u8MacAddr[0]);
		
		nvr_net_man_update_dns(pNetCfg->u32DnsIp1, pNetCfg->u32DnsIp2);

		g_timeout_add_seconds(10, nvr_net_dhcp_timeout, "eth0:1");
			
		*pChange = 1;
		
		nvr_net_man_update_pppoe(&pNetCfg->struPppoeCfg);
		return 0;
	}
	
	nvr_net_set_mac_addr("eth0", &pNetCfg->struEtherCfg[0].u8MacAddr[0]);
	if (pNetCfg->struEtherCfg[0].u8DhcpOn)
	{
		/*
		if (nvr_net_run_dhcp("eth0") == NVR_SUCCESS)
		{
			nvr_netcfg_get_ip_addr((char *)"eth0", szIp);
			pNetCfg->struEtherCfg[0].u32IpAddr = net_ip_a2n(szIp);
			
			nvr_netcfg_get_mask_addr((char *)"eth0", szMask);
			pNetCfg->struEtherCfg[0].u32SubMask = net_ip_a2n(szMask);
			
			nvr_netcfg_get_gw_addr((char *)"eth0", szGw, &nMetric);
			pNetCfg->struEtherCfg[0].u32GateWay = net_ip_a2n(szGw);

			j_message( "run dhcp success ip %s submask %s gw %s", szIp, szMask, szGw);

			if (pNetCfg->u8DnsDhcp)//开启了自动获取DNS则读取系统的DNS					
				nvr_netcfg_get_dns(DNS_CFG_FILE, (unsigned int*)(&pNetCfg->u32DnsIp1), 
									 (unsigned int*)(&pNetCfg->u32DnsIp2));
			else
				nvr_netcfg_update_dns(DNS_CFG_FILE, pNetCfg->u32DnsIp1, pNetCfg->u32DnsIp2);
			*pChange = 1;
			return 0;
		}
		*/
		g_timeout_add_seconds(5, nvr_net_dhcp_timeout, "eth0");
	}
	else
		g_timeout_add_seconds(5, nvr_net_dhcp_timeout, "eth0:1");
	net_ip_n2a(pNetCfg->struEtherCfg[0].u32IpAddr,    szIp, sizeof(szIp));
	net_ip_n2a(pNetCfg->struEtherCfg[0].u32SubMask, szMask, sizeof(szMask));
	net_ip_n2a(pNetCfg->struEtherCfg[0].u32GateWay, szGw, sizeof(szGw));
	j_message( "config local net ip %s szMask %s gw %s ", szIp, szMask, szGw);
	if ((nErr = nvr_netcfg_set_ip_addr((char *)"eth0", szIp)) != 0 ||
		(nErr = nvr_netcfg_set_mask_addr((char *)"eth0", szMask)) != 0 ||
		(nErr = nvr_netcfg_set_gw_addr((char *)"eth0", szGw, ETH0_METRIC)) != 0)
	{
		j_warning( "config local net error");
		return -1;
	}
	nvr_net_man_update_dns(pNetCfg->u32DnsIp1, pNetCfg->u32DnsIp2);
	nvr_net_man_update_pppoe(&pNetCfg->struPppoeCfg);
	
	return 0;
}

static gboolean nvr_net_update_cfg_timeout(gpointer user_data)
{
	NVR_DEV_NET_CFG_S stDevNetCfg;
	int nChange = 0;

	nvr_get_net_cfg(0, &stDevNetCfg);
	nvr_net_config_lan_net(&stDevNetCfg, &nChange);
	if (nChange != 0)
		nvr_update_net_cfg(0, &stDevNetCfg);
	return G_SOURCE_REMOVE;
}

int nvr_net_update_cfg()
{
	g_timeout_add_seconds(3, nvr_net_update_cfg_timeout, NULL);
	return 0;
}

int nvr_net_get_net_info(NVR_DEV_NET_CFG_S *pNetCfg)
{
	char szIp[32] = {0}, szMask[32] = {0}, szGw[32] = {0};
	int nMetric;

	if(pNetCfg == NULL)
		return -1;
	if(pNetCfg->struEtherCfg[0].u8DhcpOn)
	{
		nvr_netcfg_get_ip_addr((char *)"eth0", szIp);
		pNetCfg->struEtherCfg[0].u32IpAddr = net_ip_a2n(szIp);
		
		nvr_netcfg_get_mask_addr((char *)"eth0", szMask);
		pNetCfg->struEtherCfg[0].u32SubMask = net_ip_a2n(szMask);
		
		nvr_netcfg_get_gw_addr((char *)"eth0", szGw, &nMetric);
		pNetCfg->struEtherCfg[0].u32GateWay = net_ip_a2n(szGw);
		
		if (pNetCfg->u8DnsDhcp)//开启了自动获取DNS则读取系统的DNS					
			nvr_netcfg_get_dns(DNS_CFG_FILE, (unsigned int*)(&pNetCfg->u32DnsIp1), 
								 (unsigned int*)(&pNetCfg->u32DnsIp2));

		if(nvr_netcfg_get_ip_addr((char*)"eth0:1", szIp) == 0)
			pNetCfg->struEtherCfg[0].u32IpAddr1 = net_ip_a2n(szIp);
		else
			pNetCfg->struEtherCfg[0].u32IpAddr1 = 0;
		
		if(nvr_netcfg_get_mask_addr((char *)"eth0:1", szMask) == 0)
			pNetCfg->struEtherCfg[0].u32SubMask1 = net_ip_a2n(szMask);
		else
			pNetCfg->struEtherCfg[0].u32SubMask1 = 0;
	}
	
	if(pNetCfg->struPppoeCfg.u8PppoeOn)
	{
		if(nvr_netcfg_get_ip_addr((char*)"ppp0", szIp) < 0)
		{
			pNetCfg->struPppoeCfg.u32PppoeIp = 0;
			if(gNetManInfo.u8PppoeStatus == 0)		// 未拨号
				pNetCfg->struPppoeCfg.u8PppoeStatus = 0;
			else
				pNetCfg->struPppoeCfg.u8PppoeStatus = 1;
			pNetCfg->struPppoeCfg.u32PppoeTimes = 0;
		}
		else
		{
			pNetCfg->struPppoeCfg.u32PppoeIp = net_ip_a2n(szIp);
			pNetCfg->struPppoeCfg.u8PppoeStatus = 2;
			if(gNetManInfo.s64PppoeTimes == 0)
				pNetCfg->struPppoeCfg.u32PppoeTimes = 0;
			else
				pNetCfg->struPppoeCfg.u32PppoeTimes = (g_get_monotonic_time() 
														- gNetManInfo.s64PppoeTimes) / 1000000;
			nvr_netcfg_get_dns(DNS_CFG_FILE, (unsigned int*)(&pNetCfg->u32DnsIp1), 
								 (unsigned int*)(&pNetCfg->u32DnsIp2));
		}
	}
	else
	{
		pNetCfg->struPppoeCfg.u32PppoeIp = 0;
		pNetCfg->struPppoeCfg.u8PppoeStatus = 0;
		pNetCfg->struPppoeCfg.u32PppoeTimes = 0;
	}

	return 0;
}

int	nvr_net_update_pppoe_conf(char * pIfname, char *pUsr)
{
	FILE *pSrcFd, *pDstFd;
	char  szStr[512] = {0};
	
	if ((pSrcFd = fopen("/etc/ppp/pppoe.conf-bak", "r")) == NULL)
	{
		j_warning("open pppoe-bak.conf-bak file error");
		return -1;
	}

	if ((pDstFd = fopen("/tmp/pppoe.conf", "w")) == NULL)
	{
		fclose(pSrcFd);
		j_warning("open pppoe.conf file error");
		return -1;
	}

	while(!feof(pSrcFd))
	{
		fgets(szStr, 512, pSrcFd);
		
		if(strncasecmp(szStr, "USER=", 5) == 0)
		{
			sprintf(szStr, "USER='%s'", pUsr);
		}
		if(strncasecmp(szStr, "ETH=", 4) == 0)
		{
			sprintf(szStr, "ETH='%s'", pIfname);
		}

		fputs(szStr, pDstFd);
		fflush(pDstFd);
		memset(szStr, 0, 512);
	}
	
	fclose(pSrcFd);
	fclose(pDstFd);	
	return 0;
}

int nvr_net_update_pppoe_secrets(char *pUsr, char *pPsw)
{
	FILE *pSrcChapFd, *pDstChapFd, *pSrcPapFd, *pDstPapFd;
	char  szStr[512] = {0};
	if ((pSrcChapFd = fopen("/etc/ppp/chap-secrets-bak", "r")) == NULL)
	{
		j_warning("open chap-secrets-bak file error");
		return -1;
	}
	if ((pDstChapFd = fopen("/tmp/chap-secrets", "w")) == NULL)
	{
		fclose(pSrcChapFd);
		j_warning("openchap-secrets file error");
		return -1;
	}

	if ((pSrcPapFd = fopen("/etc/ppp/pap-secrets-bak", "r")) == NULL)
	{
		fclose(pSrcChapFd);
		fclose(pDstChapFd);
		j_warning("open pap-secrets-bak file error");
		return -1;
	}

	if ((pDstPapFd = fopen("/tmp/pap-secrets", "w")) == NULL)
	{
		fclose(pSrcChapFd);
		fclose(pDstChapFd);
		fclose(pSrcPapFd);
		j_warning("openpap-secrets file error");
		return -1;
	}
	
	while(!feof(pSrcChapFd))
	{
		fgets(szStr, 512, pSrcChapFd);
		
		if(strncasecmp(szStr, "\"", 1) == 0)
		{
			sprintf(szStr, "\"%s\"%s\"%s\"", pUsr, "     *       ", pPsw);
		}

		fputs(szStr, pDstChapFd);
		fflush(pDstChapFd);
		memset(szStr, 0, 512);
	}

	while(!feof(pSrcPapFd))
	{
		fgets(szStr, 512, pSrcPapFd);
		
		if(strncasecmp(szStr, "\"", 1) == 0)
		{
			sprintf(szStr, "\"%s\"%s\"%s\"", pUsr, "     *       ", pPsw);
		}

		fputs(szStr, pDstPapFd);
		fflush(pDstPapFd);
		memset(szStr, 0, 512);
	}
	
	fclose(pSrcChapFd);
	fclose(pDstChapFd);
	fclose(pSrcPapFd);
	fclose(pDstPapFd);	
	return 0;
}

static gboolean nvr_net_check_pppoe_timeout(gpointer user_data)
{
	char szIp[16];
	if(nvr_netcfg_get_ip_addr("ppp0", szIp) >= 0)
	{
		gNetManInfo.s64PppoeTimes = g_get_monotonic_time();
		return G_SOURCE_REMOVE;
	}
	return G_SOURCE_CONTINUE;
}

int nvr_net_pppoe_start(NVR_DEV_PPPOE_CFG_S *pPppoeCfg)
{
	if(pPppoeCfg->u8PppoeEth == 0)
	{
		nvr_net_update_pppoe_conf("eth0", pPppoeCfg->szPppoeUsr);
	}else if(pPppoeCfg->u8PppoeEth == 1)
	{
		nvr_net_update_pppoe_conf("ra0", pPppoeCfg->szPppoeUsr);
	}else
	{
		nvr_net_update_pppoe_conf("eth0", pPppoeCfg->szPppoeUsr);
	}
	nvr_net_update_pppoe_secrets(pPppoeCfg->szPppoeUsr, 
											pPppoeCfg->szPppoePsw);
	gNetManInfo.stPppoeCfg.u8PppoeOn = pPppoeCfg->u8PppoeOn;
	gNetManInfo.stPppoeCfg.u8PppoeEth = pPppoeCfg->u8PppoeEth;
	strcpy(gNetManInfo.stPppoeCfg.szPppoeUsr, pPppoeCfg->szPppoeUsr);
	strcpy(gNetManInfo.stPppoeCfg.szPppoePsw, pPppoeCfg->szPppoePsw);
	gNetManInfo.s64PppoeTimes = 0;
	gNetManInfo.u8PppoeStatus = 1;
	g_timeout_add_seconds(1, nvr_net_check_pppoe_timeout, NULL);
	j_message("Start pppoe.");
	return system("pppoe-start");
}

void nvr_net_pppoe_stop()
{
	if(gNetManInfo.u8PppoeStatus == 0)
		return;
	j_message("Stop pppoe.");
	system("pppoe-stop");
	memset(&gNetManInfo.stPppoeCfg, 0, sizeof(gNetManInfo.stPppoeCfg));
	gNetManInfo.u8PppoeStatus = 0;
	gNetManInfo.s64PppoeTimes = 0;
}

int	nvr_net_man_init()
{
	NVR_DEV_NET_CFG_S	struNetCfg;
	int	nChange = 0;
	
	dhcpLibInit("eth0");
	memset(&struNetCfg, 0, sizeof(NVR_DEV_NET_CFG_S));
	
	nvr_get_net_cfg(0, &struNetCfg);
	nvr_net_config_lan_net(&struNetCfg, &nChange);
	if (nChange != 0)
	{
		nvr_update_net_cfg(0, &struNetCfg);
	}
	
	j_message("nvr net man init ok.");
	return NVR_SUCCESS;
}

void nvr_net_man_uninit()
{
	nvr_net_pppoe_stop();
	//system("killall udhcpc");
	dhcpLibUninit();
	j_message( "nvr net man uninit ok.");
}



