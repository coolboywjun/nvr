
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <netdb.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <net/if.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <semaphore.h>
#include "uemf.h"	//来自goAhead web

#include <nvr_lib_interface.h>
#include <ipnc_lib.h>

#include "nvr_xml_cfg.h"
#include "nvr_web_proc.h"


#undef	G_LOG_DOMAIN
#define	G_LOG_DOMAIN "xml"

#define XMLFILENAME "/tmp/xmlDoc.xml"
//#define XMLPOSTFILE "/tmp/xmlPost.xml"

#define DECLARE_WEB_GET_CFG(CfgName, StructType)	\
	int nvr_web_get_##CfgName##_cfg(int sock)\
{													\
	StructType struCfg;								\
	memset(&struCfg, 0, sizeof(struCfg));			\
	nvr_logon_get_##CfgName##_cfg(-1, &struCfg);	\
	return make_##CfgName##_cfg_xml(sock, &struCfg);\
}

#define DECLARE_WEB_GET_ARRAY_CFG(CfgName, StructType, ArrayLen)\
	int nvr_web_get_##CfgName##_cfg(int sock)		\
{																\
	int i = 0;													\
	StructType struCfg[ArrayLen];								\
	for(i = 0; i < ArrayLen; i++)								\
	{															\
		memset(&struCfg[i], 0, sizeof(struCfg[i]));				\
		nvr_logon_get_##CfgName##_cfg(i, &struCfg[i]);	\
	}															\
	return make_##CfgName##_cfg_xml(sock, struCfg, ArrayLen);	\
}

sem_t g_xmlLock;

#if 1
static unsigned long net_ip_n2a(unsigned long ip, char *ourIp, int len)
{
	if(!ourIp)
		return -1;
	
	memset(ourIp, 0, len);
	ip = htonl(ip);
	inet_ntop(AF_INET, &ip, ourIp, len);	
	return 0;
}

static unsigned long net_ip_a2n(char *pstrIP)
{
	unsigned long ret;
    struct hostent *hostinfo;
    struct sockaddr_in address;

	if (!pstrIP)
		return -1;
	
	inet_pton(AF_INET, pstrIP, &ret);
		
    if(0xFFFFFFFF == ret)
    {
		if(strcmp(pstrIP,"255.255.255.255") == 0)
		{
		    return 0xFFFFFFFF;
		}
	
		if(!(hostinfo = gethostbyname(pstrIP)))
		{
			return 0;
		}

		address.sin_addr = *((struct in_addr *)(*(hostinfo->h_addr_list)));
		ret = address.sin_addr.s_addr;
    }
	
	return htonl(ret);
}

//发送xml数据到http
static int send_xml2http(int sock)
{
	struct stat file_info = {0};
	FILE *fp = NULL;
	int ret = 0;

	fp = fopen(XMLFILENAME, "rb");
	if(NULL == fp)	
		ret = -1;
	else
	{
		if(-1 == stat(XMLFILENAME, &file_info))
			ret = -1;
		else
		{
			int dataSize = file_info.st_size + 64;
			char *pHttpBuf = malloc(dataSize);
			if(NULL != pHttpBuf)
			{
				sprintf(pHttpBuf,"HTTP/1.1\r\nContent-Length:%d\r\n\r\n",(int)file_info.st_size);

				__DBUG("st_size:%d\n", (int)file_info.st_size);
				__DBUG("sock: %d\n", sock);

				ret = socketWrite(sock, pHttpBuf, strlen(pHttpBuf));
				if(0 < ret)
				{
					__DBUG("start send xml data1...\n");
					memset(pHttpBuf, 0, dataSize);
					do {
						ret = fread(pHttpBuf, 1, file_info.st_size, fp);
					} while (ret < 0 && errno == EINTR);

					__DBUG("ret:%d\n", ret);
					if(ret != socketWrite(sock, pHttpBuf, ret))
						ret = -1;
				}
				else
					ret = -1;

				free(pHttpBuf);
			}
		}
		fclose(fp);
		fp = NULL;

		//清空文件
		do{
		  fp = fopen(XMLFILENAME, "wb");
		  }while(!fp);
		  fclose(fp);
	}

	return ret;
}
//保存xml数据
static int save_and_clear_xml(int sock, xmlDocPtr doc)
{
	//存储xml文档
	int ret = 0;

	sem_wait(&g_xmlLock);//-1

	int nRel = xmlSaveFormatFileEnc(XMLFILENAME, doc, "UTF-8", 1);
	//int nRel = xmlSaveFile(XMLFILENAME, doc);
	if (nRel == -1)
		ret = -1;
	else
		ret = send_xml2http(sock);

	sem_post(&g_xmlLock);//+1
	//释放文档内节点动态申请的内存
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();

	return ret;
}
//增加xml节点
static void xml_add_node(xmlNodePtr node,const char* strdata, int intdata, const char* name, char flag)
{
	char strBuf[128] = {0};

	memset(strBuf, 0, sizeof(strBuf));

	if(NULL == strdata)
	{
		if('d' == flag)
			sprintf(strBuf, "%d", intdata);
		else if('x' == flag)
			sprintf(strBuf, "%x", intdata);
		else
			return;
	}
	else
	{	
		if('s' == flag)
			strncpy(strBuf, strdata, sizeof(strBuf));
		else if('c' == flag)
			sprintf(strBuf, "%c", *strdata);
		else
			return;
	}

	xmlNewTextChild(node, NULL, (xmlChar *)name, (xmlChar *)strBuf);
}
static void xml_add_node_ip(xmlNodePtr node, int data, char* name)
{
	char ipAddr[32] = {0};

	memset(ipAddr, 0, sizeof(ipAddr));
	net_ip_n2a(data, ipAddr, 32);
	xmlNewTextChild(node, NULL, (xmlChar *)name, (xmlChar *)ipAddr);
}

//网络结构体信息
static int make_net_cfg_xml(int sock, NVR_DEV_NET_CFG_S *pStruCfg)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	char strBuf[32] = {0};
	int i = 0, j;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_DEV_NET_CFG_S");
	xmlNodePtr node = NULL;

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	//创建一个儿子NVR_DEV_ETHERNET_CFG_S节点
	for(i = 0; i < NVR_ETHERNET_NUM; i++)
	{
		node = xmlNewNode(NULL, BAD_CAST "NVR_DEV_ETHERNET_CFG_S");
		xmlAddChild(root_node,node);

		xml_add_node_ip(node, pStruCfg->struEtherCfg[i].u32IpAddr, "u32IpAddr");
		xml_add_node_ip(node, pStruCfg->struEtherCfg[i].u32SubMask, "u32SubMask");
		xml_add_node_ip(node, pStruCfg->struEtherCfg[i].u32GateWay, "u32GateWay");

		memset(strBuf, 0, sizeof(strBuf));
		//mac地址
		for(j = 0; j < NVR_MAC_ADDR_LEN; j++)
		{
			if(0 <= pStruCfg->struEtherCfg[i].u8MacAddr[j] &&
				    pStruCfg->struEtherCfg[i].u8MacAddr[j] <= 9)
				sprintf(strBuf + j*2,"0%x", pStruCfg->struEtherCfg[i].u8MacAddr[j]);
			else
				sprintf(strBuf + j*2,"%x", pStruCfg->struEtherCfg[i].u8MacAddr[j]);
		}
		xmlNewTextChild(node, NULL, BAD_CAST "u8MacAddr", (xmlChar *)strBuf);

		xml_add_node(node, NULL, pStruCfg->struEtherCfg[i].u8DhcpOn, "u8DhcpOn", 'd');
		xml_add_node(node, NULL, pStruCfg->struEtherCfg[i].u8DhcpStatus, "u8DhcpStatus", 'd');
		xml_add_node(node, NULL, pStruCfg->struEtherCfg[i].u8EtherStatus, "u8EtherStatus", 'd');
		xml_add_node(node, NULL, pStruCfg->struEtherCfg[i].u8NetcardSpeed, "u8NetcardSpeed", 'd');
	}
	//根下节点
	xml_add_node_ip(root_node, pStruCfg->u32AlarmHostIp, "u32AlarmHostIp");
	xml_add_node(root_node, NULL, pStruCfg->u16AlarmHostPort, "u16AlarmHostPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8DnsDhcp, "u8DnsDhcp", 'd');
	//xml_add_node(root_node, NULL, pStruCfg->u8Res1,    "u8Res1",    'd');
	xml_add_node_ip(root_node, pStruCfg->u32DnsIp1, "u32DnsIp1");
	xml_add_node_ip(root_node, pStruCfg->u32DnsIp2, "u32DnsIp2");
	xml_add_node(root_node, NULL, pStruCfg->u16HttpPort, "u16HttpPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16NetPort,  "u16NetPort",  'd');
	xml_add_node_ip(root_node, pStruCfg->u32MultiCastIp, "u32MultiCastIp");
	xml_add_node(root_node, NULL, pStruCfg->u16MultiCastPort, "u16MultiCastPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16RtspPort, "u16RtspPort", 'd');

	//创建一个儿子NVR_DEV_PPPOE_CFG_S节点
	node = xmlNewNode(NULL, BAD_CAST "NVR_DEV_PPPOE_CFG_S");
	xmlAddChild(root_node,node);
	xml_add_node(node, NULL, pStruCfg->struPppoeCfg.u8PppoeOn, "u8PppoeOn", 'd');
	xml_add_node(node, NULL, pStruCfg->struPppoeCfg.u8PppoeStatus, "u8PppoeStatus", 'd');
	xml_add_node(node, pStruCfg->struPppoeCfg.szPppoeUsr, -1, "szPppoeUsr", 's');
	xml_add_node(node, pStruCfg->struPppoeCfg.szPppoePsw, -1, "szPppoePsw", 's');
	xml_add_node_ip(node, pStruCfg->struPppoeCfg.u32PppoeIp, "u32PppoeIp");
	xml_add_node(node, NULL, pStruCfg->struPppoeCfg.u32PppoeTimes, "u32PppoeTimes", 'd');

	xml_add_node(root_node, NULL, pStruCfg->u16JXJDataPort, "u16JXJDataPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16CmdPort,  "u16CmdPort",  'd');
	xml_add_node(root_node, NULL, pStruCfg->u16TalkPort,  "u16TalkPort",  'd');
	
	return save_and_clear_xml(sock, doc);

}

//设备结构体信息
static int make_dev_cfg_xml(int sock, NVR_DEV_INFO_S *pStruCfg)
{
	int i = 0, j;
	
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	char strBuf[32] = {0};
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_DEV_INFO_S");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	//创建一个儿子NVR_DEV_STABLE_INFO_S节点
	xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "NVR_DEV_STABLE_INFO_S");
	xmlAddChild(root_node,node);

	xml_add_node(node, pStruCfg->stStableInfo.szSerialId, -1, "szSerialId", 's');
	xml_add_node(node, pStruCfg->stStableInfo.szHwVer, -1, "szHwVer", 's');
	xml_add_node(node, pStruCfg->stStableInfo.szSoftVer, -1, "szSoftVer", 's');

	for(i = 0; i < NVR_ETHERNET_NUM; i++)
	{
		memset(strBuf, 0, sizeof(strBuf));
		//mac地址
		for(j = 0; j < NVR_MAC_ADDR_LEN; j++)
		{
			if(0 <= pStruCfg->stStableInfo.u8MacAddr[i][j] &&
				    pStruCfg->stStableInfo.u8MacAddr[i][j] <= 9)
				sprintf(strBuf + j*2,"0%x", pStruCfg->stStableInfo.u8MacAddr[i][j]);
			else
				sprintf(strBuf + j*2,"%x", pStruCfg->stStableInfo.u8MacAddr[i][j]);
		}
		xmlNewTextChild(node, NULL, BAD_CAST "u8MacAddr", (xmlChar *)strBuf);
	}	

	for(i = 0; i < NVR_ETHERNET_NUM; i++)
		xml_add_node_ip(node, pStruCfg->stStableInfo.u32IPAddr[i], "u32IPAddr");
	for(i = 0; i < NVR_ETHERNET_NUM; i++)
		xml_add_node_ip(node, pStruCfg->stStableInfo.u32NetMask[i], "u32NetMask");
	for(i = 0; i < NVR_ETHERNET_NUM; i++)
		xml_add_node_ip(node, pStruCfg->stStableInfo.u32GateWay[i], "u32GateWay");

	for(i = 0; i < 4; i++)
		xml_add_node(node, NULL, pStruCfg->stStableInfo.u32DevCap[i], "u32DevCap", 'd');

	xml_add_node(node, NULL, pStruCfg->stStableInfo.u8SupportAudio, "u8SupportAudio", 'd');
	xml_add_node(node, NULL, pStruCfg->stStableInfo.u8SupportHDMI, "u8SupportHDMI", 'd');
	xml_add_node(node, NULL, pStruCfg->stStableInfo.u8supportVGA, "u8supportVGA", 'd');
	xml_add_node(node, NULL, pStruCfg->stStableInfo.u8ChnNum, "u8ChnNum", 'd');
	xml_add_node(node, NULL, pStruCfg->stStableInfo.u8SataNum, "u8SataNum", 'd');
	xml_add_node(node, NULL, pStruCfg->stStableInfo.u8NetNum, "u8NetNum", 'd');
	xml_add_node(node, NULL, pStruCfg->stStableInfo.u8RS232Num, "u8RS232Num", 'd');
	xml_add_node(node, NULL, pStruCfg->stStableInfo.u8RS485Num, "u8RS485Num", 'd');
	xml_add_node(node, NULL, pStruCfg->stStableInfo.u8AlarmInputNum, "u8AlarmInputNum", 'd');
	xml_add_node(node, NULL, pStruCfg->stStableInfo.u8AlarmOutputNum, "u8AlarmOutputNum", 'd');

	//创建一个儿子NVR_DEV_VAR_INFO_S节点
	node = xmlNewNode(NULL, BAD_CAST "NVR_DEV_VAR_INFO_S");
	xmlAddChild(root_node,node);

	xml_add_node(node, pStruCfg->stVarInfo.szDevName, -1, "szDevName", 's');
	xml_add_node(node, NULL, pStruCfg->stVarInfo.u8Language, "u8Language", 'd');
	xml_add_node(node, NULL, pStruCfg->stVarInfo.u8DevNo, "u8DevNo", 'd');
	xml_add_node(node, NULL, pStruCfg->stVarInfo.u8VideoFormat, "u8VideoFormat", 'd');

	return save_and_clear_xml(sock, doc);
}

//ftp结构体信息
static int make_ftp_cfg_xml(int sock, NVR_FTP_CFG_S *pStruCfg)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_FTP_CFG_S");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	xml_add_node(root_node, pStruCfg->szFtpSvr, -1, "szFtpSvr", 's');
	xml_add_node(root_node, pStruCfg->szFtpUsr, -1, "szFtpUsr", 's');
	xml_add_node(root_node, pStruCfg->szFtpPsw, -1, "szFtpPsw", 's');
	xml_add_node(root_node, pStruCfg->szFtpPath, -1, "szFtpPath", 's');
	xml_add_node(root_node, NULL, pStruCfg->u16FtpPort, "u16FtpPort", 'd');

	return save_and_clear_xml(sock, doc);
}

//SMTP结构体信息
static int make_smtp_cfg_xml(int sock, NVR_SMTP_CFG_S *pStruCfg)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_SMTP_CFG_S");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	xml_add_node(root_node, pStruCfg->szMailHub, -1, "szMailHub", 's');
	xml_add_node(root_node, pStruCfg->szMailAddr, -1, "szMailAddr", 's');
	xml_add_node(root_node, pStruCfg->szMailUsr, -1, "szMailUsr", 's');
	xml_add_node(root_node, pStruCfg->szMailPsw, -1, "szMailPsw", 's');
	xml_add_node(root_node, pStruCfg->szMailRctp1, -1, "szMailRctp1", 's');
	xml_add_node(root_node, pStruCfg->szMailRctp2, -1, "szMailRctp2", 's');
	xml_add_node(root_node, pStruCfg->szMailRctp3, -1, "szMailRctp3", 's');

	xml_add_node(root_node, NULL, pStruCfg->u8EnableSSL, "u8EnableSSL", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16MailPort, "u16MailPort", 'd');

	return save_and_clear_xml(sock, doc);
}

//DDNS结构体信息
static int make_ddns_cfg_xml(int sock, NVR_DDNS_CFG_S *pStruCfg)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_DDNS_CFG_S");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	xml_add_node(root_node, NULL, pStruCfg->u8DDNSOpen, "u8DDNSOpen", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8DDNSType, "u8DDNSType", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16DDNSPort, "u16DDNSPort", 'd');

	xml_add_node(root_node, pStruCfg->szDDNSAccount, -1, "szDDNSAccount", 's');
	xml_add_node(root_node, pStruCfg->szDDNSUsr, -1, "szDDNSUsr", 's');
	xml_add_node(root_node, pStruCfg->szDDNSPsw, -1, "szDDNSPsw", 's');

	xml_add_node(root_node, NULL, pStruCfg->u32DDNSTimes, "u32DDNSTimes", 'd');

	return save_and_clear_xml(sock, doc);
}

//UPNP结构体信息
static int make_upnp_cfg_xml(int sock, NVR_UPNP_CFG_S *pStruCfg)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_UPNP_CFG_S");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	xml_add_node(root_node, NULL, pStruCfg->u8UpnpOpen, "u8UpnpOpen", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8UpnpEthNo, "u8UpnpEthNo", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8UpnpModel, "u8UpnpModel", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8UpnpRefTime, "u8UpnpRefTime", 'd');

	xml_add_node(root_node, pStruCfg->szUpnpSvr, -1, "szUpnpSvr", 's');
	//xml_add_node_ip(root_node, pStruCfg->szUpnpSvr, "szUpnpSvr");

	xml_add_node(root_node, NULL, pStruCfg->u16UpnpDataPort, "u16UpnpDataPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16UpnpWebPort, "u16UpnpWebPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16UpnpRtspPort, "u16UpnpRtspPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16UpnpDataPortS, "u16UpnpDataPortS", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16UpnpWebPortS, "u16UpnpWebPortS", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16UpnpRtspPortS, "u16UpnpRtspPortS", 'd');

	xml_add_node(root_node, NULL, pStruCfg->u16UpnpCmdPort, "u16UpnpCmdPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16UpnpTalkPort, "u16UpnpTalkPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16UpnpJXJDataPort, "u16UpnpJXJDataPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16UpnpCmdPortS, "u16UpnpCmdPortS", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16UpnpWebPortS, "u16UpnpWebPortS", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16UpnpTalkPortS, "u16UpnpTalkPortS", 'd');

	return save_and_clear_xml(sock, doc);
}

//UPNP结构体信息
static int make_ntp_cfg_xml(int sock, NVR_NTP_CFG_S *pStruCfg)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_NTP_CFG_S");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	xml_add_node(root_node, NULL, pStruCfg->u8NtpOpen, "u8NtpOpen", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u16NtpPort, "u16NtpPort", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8EuroTime, "u8EuroTime", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8NtpRefTime, "u8NtpRefTime", 'd');
	xml_add_node(root_node, NULL, pStruCfg->nTimeZone, "nTimeZone", 'd');
	xml_add_node(root_node, pStruCfg->szNtpServer, -1, "szNtpServer", 's');

	return save_and_clear_xml(sock, doc);
}

//用户结构体信息
static int make_user_cfg_xml(int sock, NVR_DEV_USER_CFG_S *pStruCfg, int len)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	int i = 0;
	char strBuf[45] = {0};
	NVR_DEV_USER_CFG_S * pStruCfgTmp = pStruCfg;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_DEV_USER_CFG_S");
	xmlNodePtr node = NULL;
		
	//设置根节点
	xmlDocSetRootElement(doc,root_node);
	for(i = 0; i < len; i++)
	{	
		//创建一个儿子节点
		node = xmlNewNode(NULL, BAD_CAST "NVR_DEV_USER_CFG_S");
		xmlAddChild(root_node,node);
		
		pStruCfg = pStruCfgTmp + i;

		xml_add_node(node, pStruCfg->szUsrName, -1, "szUsrName", 's');
		xml_add_node(node, pStruCfg->szPsw, -1, "szPsw", 's');

		memset(strBuf, 0, sizeof(strBuf));
		sprintf(strBuf, "%llu", pStruCfg->u64LocalRight);
		xmlNewTextChild(node, NULL, (xmlChar *)"u64LocalRight", (xmlChar *)strBuf);

		memset(strBuf, 0, sizeof(strBuf));
		sprintf(strBuf, "%llu", pStruCfg->u64RemoteRight);
		xmlNewTextChild(node, NULL, (xmlChar *)"u64RemoteRight", (xmlChar *)strBuf);
	}
	return save_and_clear_xml(sock, doc);
}

//时间结构体信息
static int make_time_cfg_xml(int sock, NVR_TIME_S *pStruCfg)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	//char strBuf[45] = {0};
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
#if 0
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_TIME_CFG_S");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	//创建一个儿子struct timeval节点
	xmlNodePtr node = xmlNewNode(NULL, BAD_CAST "struTv");
	xmlAddChild(root_node,node);

	memset(strBuf, 0, sizeof(strBuf));
	sprintf(strBuf, "%ld", pStruCfg->struTv.tv_sec);
	xmlNewTextChild(node, NULL, (xmlChar *)"tv_sec", (xmlChar *)strBuf);

	memset(strBuf, 0, sizeof(strBuf));
	sprintf(strBuf, "%ld", pStruCfg->struTv.tv_usec);
	xmlNewTextChild(node, NULL, (xmlChar *)"tv_usec", (xmlChar *)strBuf);

	xml_add_node(root_node, NULL, pStruCfg->u8DateFormat, "u8DateFormat", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8TimeFormat, "u8TimeFormat", 'd');

	xml_add_node(root_node, &pStruCfg->cSeparatorChar, -1, "cSeparatorChar", 'c');
#else

	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_TIME_S");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	//创建节点
	xml_add_node(root_node, NULL, pStruCfg->u8Year, "u8Year", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8Month, "u8Month", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8Day, "u8Day", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8WDay, "u8WDay", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8Hour, "u8Hour", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8Minute, "u8Minute", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8Second, "u8Second", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8Millisec, "u8Millisec", 'd');
	
#endif
	return save_and_clear_xml(sock, doc);
}


//自动任务结构体信息
static int make_auto_cfg_xml(int sock, NVR_AUTO_MAINTAIN_CFG_S *pStruCfg)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_AUTO_MAINTAIN_CFG_S");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	xml_add_node(root_node, NULL, pStruCfg->u8WeekDay, "u8WeekDay", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8Hour, "u8Hour", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8Min, "u8Min", 'd');

	return save_and_clear_xml(sock, doc);
}

//通道结构体信息NVR_CHANNEL_CFG_S
static int make_channel_cfg_xml(int sock, NVR_CHANNEL_CFG_S *pStruCfg, int len)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	int i = 0;
	NVR_CHANNEL_CFG_S * pStruCfgTmp = pStruCfg;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_CHANNEL_CFG_S");
	xmlNodePtr node = NULL;
	//设置根节点
	xmlDocSetRootElement(doc,root_node);
	node = root_node;
	for(i = 0; i < len; i++)
	{	
		//创建一个儿子NVR_CHANNEL_CFG_S节点
		node = xmlNewNode(NULL, BAD_CAST "NVR_CHANNEL_CFG_S");
		xmlAddChild(root_node,node);

		pStruCfg = pStruCfgTmp + i;

		xml_add_node(node, pStruCfg->szFactoryName, -1, "szFactoryName", 's');
		xml_add_node(node, pStruCfg->szProtocolName, -1, "szProtocolName", 's');
		xml_add_node(node, pStruCfg->szDevName, -1, "szDevName", 's');
		xml_add_node(node, pStruCfg->szUser, -1, "szUser", 's');
		xml_add_node(node, pStruCfg->szPasswd, -1, "szPasswd", 's');

		xml_add_node_ip(node, pStruCfg->u32IpAddr, "u32IpAddr");

		xml_add_node(node, NULL, pStruCfg->u16DataPort, "u16DataPort", 'd');
		xml_add_node(node, NULL, pStruCfg->u16WebPort, "u16WebPort", 'd');
		xml_add_node(node, NULL, pStruCfg->u16RtspPort, "u16RtspPort", 'd');
		xml_add_node(node, NULL, pStruCfg->u8RemoteChn, "u8RemoteChn", 'd');
		xml_add_node(node, NULL, pStruCfg->u8UseChn, "u8UseChn", 'd');
		xml_add_node(node, NULL, pStruCfg->u8Enable, "u8Enable", 'd');
		xml_add_node(node, NULL, pStruCfg->u8SearchA, "u8SearchA", 'd');
	}
	return save_and_clear_xml(sock, doc);
}

//串口结构体信息
static int make_comm_cfg_xml(int sock, NVR_DEV_COMM_CFG_S *pStruCfg, int len)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	int i = 0;
	NVR_DEV_COMM_CFG_S * pStruCfgTmp = pStruCfg;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_DEV_COMM_CFG_S");
	xmlNodePtr node = NULL;
	//设置根节点
	xmlDocSetRootElement(doc,root_node);
	for(i = 0; i < len; i++)
	{
		//创建一个儿子节点
		node = xmlNewNode(NULL, BAD_CAST "NVR_DEV_COMM_CFG_S");
		xmlAddChild(root_node,node);
		
		pStruCfg = pStruCfgTmp + i;

		xml_add_node(node, NULL, pStruCfg->u32BaudRate, "u32BaudRate", 'd');
		xml_add_node(node, NULL, pStruCfg->u8DataBit, "u8DataBit", 'd');
		xml_add_node(node, NULL, pStruCfg->u8StopBit, "u8StopBit", 'd');
		xml_add_node(node, NULL, pStruCfg->u8Parity, "u8Parity", 'd');
		xml_add_node(node, NULL, pStruCfg->u8Flowcontrol, "u8Flowcontrol", 'd');
		xml_add_node(node, NULL, pStruCfg->u32BaudMask, "u32BaudMask", 'd');
	}
	return save_and_clear_xml(sock, doc);
}

//rec
static int make_rec_cfg_xml(int sock, NVR_DEV_REC_STRC_S *pStruCfg)
{
	int i = 0, j = 0;
	
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_DEV_REC_STRC_S");
	xmlNodePtr node = NULL;
	
	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	xml_add_node(root_node, NULL, pStruCfg->u8OverWrite, "u8OverWrite", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8RecStream, "u8RecStream", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8PreRecSecs, "u8PreRecSecs", 'd');
	xml_add_node(root_node, NULL, pStruCfg->u8SaveMode, "u8SaveMode", 'd');

	for(i = 0; i < NVR_MAX_SEG_NUM; i++)
	{
		//创建一个儿子NVR_DEV_VAR_INFO_S节点
		node = xmlNewNode(NULL, BAD_CAST "struTimerSeg");
		xmlAddChild(root_node,node);
		for(j = 0; j < 8; j++)
		{
			xml_add_node(node, NULL, pStruCfg->struTimerSeg[i][j].u8Open & 0xff,
						 "u8Open", 'd');
			xml_add_node(node, NULL, pStruCfg->struTimerSeg[i][j].beginSec & 0xffffff,
						 "beginSec", 'd');
			xml_add_node(node, NULL, pStruCfg->struTimerSeg[i][j].endSec & 0xffffff,
						 "endSec", 'd');
		}
	}
	return save_and_clear_xml(sock, doc);
}


static int make_search_req_xml(int sock, IPNC_NET_SEARCH_RES_S *pStruCfg, int len)
{	
	int i = 0, j = 0;
	char strBuf[64] = {0};
	
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"IPNC_NET_SEARCH_RES_S");
	xmlNodePtr node = NULL;
	IPNC_NET_SEARCH_RES_S *pStruCfgTmp = pStruCfg;
	
	//设置根节点
	xmlDocSetRootElement(doc,root_node);
	node = root_node;
	for(i = 0; i < len; i++)
	{
		//创建一个儿子IPNC_NET_SEARCH_REQ_S节点
		node = xmlNewNode(NULL, BAD_CAST "IPNC_NET_SEARCH_RES_S");
		xmlAddChild(root_node,node);

		pStruCfg = pStruCfgTmp + i;

		memcpy(strBuf, pStruCfg->u8SerialId, sizeof(pStruCfg->u8SerialId));
		xmlNewTextChild(node, NULL, BAD_CAST "u8SerialId", (xmlChar *)strBuf);
		//xml_add_node(node, pStruCfg->u8SerialId, -1, "u8SerialId", 's');
		xml_add_node(node, NULL, pStruCfg->u32SoftVer, "u32SoftVer", 'd');
		xml_add_node(node, NULL, pStruCfg->u16DevType, "u16DevType", 'd');
		xml_add_node(node, NULL, pStruCfg->u16DevPort, "u16DevPort", 'd');
		xml_add_node_ip(node, pStruCfg->u32DevIp, "u32DevIp");
		
		memset(strBuf, 0, sizeof(strBuf));
		//mac地址
		for(j = 0; j < NVR_MAC_ADDR_LEN; j++)
		{
			if(0 <= pStruCfg->u8MacAddr[j] &&
				    pStruCfg->u8MacAddr[j] <= 9)
				sprintf(strBuf + j*2,"0%x", pStruCfg->u8MacAddr[j]);
			else
				sprintf(strBuf + j*2,"%x", pStruCfg->u8MacAddr[j]);
		}
		xmlNewTextChild(node, NULL, BAD_CAST "u8MacAddr", (xmlChar *)strBuf);
		
		xml_add_node(node, NULL, pStruCfg->u8Dns, "u8Dns", 'd');
		xml_add_node(node, NULL, pStruCfg->u8DHCP, "u8DHCP", 'd');
		xml_add_node_ip(node, pStruCfg->u32DevIpMask, "u32DevIpMask");
		xml_add_node_ip(node, pStruCfg->u32DevGwAddr, "u32DevGwAddr");
		xml_add_node_ip(node, pStruCfg->u32DevDns1, "u32DevDns1");
		xml_add_node_ip(node, pStruCfg->u32DevDns2, "u32DevDns2");
/*
		xml_add_node(node, NULL, pStruCfg->u32Capability0, "u32Capability0", 'd');
		xml_add_node(node, NULL, pStruCfg->u32Capability1, "u32Capability1", 'd');
		xml_add_node(node, NULL, pStruCfg->u32Capability2, "u32Capability2", 'd');
		xml_add_node(node, NULL, pStruCfg->u8MainDevType, "u8MainDevType", 'd');
		xml_add_node(node, NULL, pStruCfg->u8SubDevType, "u8SubDevType", 'd');
		xml_add_node(node, NULL, pStruCfg->u16PlateformCap, "u16PlateformCap", 'd');
		xml_add_node(node, NULL, pStruCfg->u32UpTime, "u32UpTime", 'd');
*/	
	}
	
	return save_and_clear_xml(sock, doc);
}

static int make_dianxin_platform_cfg_xml(int sock, NVR_DEV_DIANXIN_PLATFORM_CFG_S *pStruCfg)
{
	//定义文档和节点指针
	if(NULL == pStruCfg)
		return -1;

	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"NVR_DEV_DIANXIN_PLATFORM_CFG_S");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);

	xml_add_node(root_node, pStruCfg->szGroupUrl, -1, "szGroupUrl", 's');
	xml_add_node(root_node, pStruCfg->szProvincialUrl, -1, "szProvincialUrl", 's');
	xml_add_node(root_node, pStruCfg->szDevId, -1, "szDevId", 's');
	xml_add_node(root_node, pStruCfg->szKey, -1, "szKey", 's');
	xml_add_node(root_node, pStruCfg->szMode, -1, "szMode", 's');

	return save_and_clear_xml(sock, doc);
}


//开始搜索
int nvr_web_get_search_start(int sock)
{
	int ret = -1;
	long lHandle = 0;
	char* devProtocol = "jxj_ti";
	char  httpBuf[128] = {0};
	char  convBuf[128] = {0};
	
	//开始搜索
	lHandle = nvr_lib_search_start(devProtocol);//开始搜索
	__DBUG("lHandle:%lx, %ld\n", lHandle, lHandle);
	__DBUG("sock: %d\n", sock);
	if(0 != lHandle)
	{
		sprintf(convBuf, "%ld", lHandle);
		sprintf(httpBuf,"HTTP/1.1\r\nContent-Length:%d\r\n\r\n",strlen(convBuf));

		ret = socketWrite(sock, httpBuf, strlen(httpBuf));

		__DBUG("sizeof(lHandle): %d\n", sizeof(lHandle));
		__DBUG("ret: %d\n", ret);
		
		if(0 < ret)
		{
			__DBUG("start send lHandle data...\n");
			memset(httpBuf, 0, sizeof(httpBuf));
			sprintf(httpBuf, "%ld", lHandle);

			long lHanledTmp = -1;
			lHanledTmp = strtol(httpBuf, NULL, 10);
			__DBUG("lHanledTmp: %lx\n", lHanledTmp);
			__DBUG("ret:%d\n", ret);
			
			if(ret != socketWrite(sock, httpBuf, ret))
				ret = -1;
		}
	}
	
	return ret;
}

//获得搜索
int nvr_web_get_search_get(int sock, long lHandle)
{
/*
	int ret = -1, devCnt = 0;
	char recvBuf[RECV_BUFF_SIZE] = {0};			
	IPNC_NET_SEARCH_REQ_S *pDev = NULL;
	
	//获得搜索
	memset(recvBuf, 0, sizeof(recvBuf));
	pDev = (IPNC_NET_SEARCH_REQ_S*)recvBuf;
	ret = nvr_lib_search_get(lHandle, recvBuf, sizeof(recvBuf));
	if(0 < ret)//获得搜索
	{
		devCnt = ret/sizeof(IPNC_NET_SEARCH_REQ_S);
		__DBUG("<<<<<<<<<<>>>>>>>>>>devCnt: %d\n", devCnt);
		make_search_req_xml(sock, pDev, devCnt);
	}
	else
	{
		char httpBuf[64] = {0};
		char convBuf[64] = {0};
		sprintf(convBuf, "%d", ret);
		sprintf(httpBuf,"HTTP/1.1\r\nContent-Length:%d\r\n\r\n",strlen(convBuf));
		if(0 < socketWrite(sock, httpBuf, strlen(httpBuf)))
		{
			memset(httpBuf, 0, sizeof(httpBuf));
			sprintf(httpBuf, "%d", ret);
			__DBUG("<<<<<<<<<<>>>>>>>>>>ret: %d\n", ret);
			ret = socketWrite(sock, httpBuf, strlen(httpBuf));
		}
	}
	return ret;
*/	
	return 0;
}

//停止搜索
int nvr_web_get_search_stop(int sock, long lHandle)
{
	//停止搜索
	return nvr_lib_search_stop(lHandle);
}

//获取设备参数
DECLARE_WEB_GET_CFG(dev, NVR_DEV_INFO_S);

//获取网络参数
DECLARE_WEB_GET_CFG(net, NVR_DEV_NET_CFG_S);

//获取ftp参数
DECLARE_WEB_GET_CFG(ftp, NVR_FTP_CFG_S);

//获取smtp参数
DECLARE_WEB_GET_CFG(smtp, NVR_SMTP_CFG_S);

//获取ddns参数
DECLARE_WEB_GET_CFG(ddns, NVR_DDNS_CFG_S);

//获取upnp参数
DECLARE_WEB_GET_CFG(upnp, NVR_UPNP_CFG_S);

//获取NTP参数
DECLARE_WEB_GET_CFG(ntp, NVR_NTP_CFG_S);

//获取用户参数
DECLARE_WEB_GET_ARRAY_CFG(user, NVR_DEV_USER_CFG_S, NVR_MAX_USR_NUM);

//获取NTP参数
DECLARE_WEB_GET_CFG(time, NVR_TIME_S);

//获取AUTO参数
DECLARE_WEB_GET_CFG(auto, NVR_AUTO_MAINTAIN_CFG_S);

//获取通道参数
DECLARE_WEB_GET_ARRAY_CFG(channel, NVR_CHANNEL_CFG_S, NVR_MAX_CHANNEL_NUM);

//获取COMM参数
DECLARE_WEB_GET_ARRAY_CFG(comm, NVR_DEV_COMM_CFG_S, NVR_MAX_SUPPORT_COMM_NO);

//获取REC参数
DECLARE_WEB_GET_CFG(rec, NVR_DEV_REC_STRC_S);

//获取电信平台参数
DECLARE_WEB_GET_CFG(dianxin_platform, NVR_DEV_DIANXIN_PLATFORM_CFG_S);

//设置NET参数
static void nvr_web_set_dev_cfg(xmlNodePtr curNode)
{
	//int i = 0;
	curNode = curNode->xmlChildrenNode;
	NVR_DEV_INFO_S struCfg;
	
	memset(&struCfg, 0, sizeof(struCfg));
	
	nvr_logon_get_dev_cfg(-1, &struCfg);
	while(curNode != NULL)
	{
		if((NULL != (curNode->xmlChildrenNode)) && 
				(XML_TEXT_NODE != curNode->xmlChildrenNode->type))//儿子节点
		{
			xmlNodePtr curNodeTmp = curNode->xmlChildrenNode;
			xmlChar *szKeyTmp = NULL;
			//stable信息不可修改
			
			//var信息
			if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DEV_VAR_INFO_S"))
			{
				while(curNodeTmp != NULL)
				{
					szKeyTmp = xmlNodeGetContent(curNodeTmp);
					
					if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"szDevName"))
						strncpy(struCfg.stVarInfo.szDevName, (char*)szKeyTmp, NVR_DEV_NAME_LEN);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8Language"))
						struCfg.stVarInfo.u8Language = (uint8_t)atoi((char*)szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8DevNo"))
						struCfg.stVarInfo.u8DevNo = (uint8_t)atoi((char*)szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8VideoFormat"))
						struCfg.stVarInfo.u8VideoFormat = (uint8_t)atoi((char*)szKeyTmp);
					
					xmlFree(szKeyTmp); 
					curNodeTmp = curNodeTmp->next;
				}	
			}	
		}
		curNode = curNode->next;
	}
	nvr_logon_set_dev_cfg(-1, &struCfg);
}

//设置NET参数
static void nvr_web_set_net_cfg(xmlNodePtr curNode)
{
	int i = 0;
	xmlChar *szKey = NULL;
	curNode = curNode->xmlChildrenNode;
	NVR_DEV_NET_CFG_S struCfg;

	memset(&struCfg, 0, sizeof(struCfg));
	nvr_logon_get_net_cfg(-1, &struCfg);
	while(curNode != NULL)
	{
		if((NULL != (curNode->xmlChildrenNode)) && 
				(XML_TEXT_NODE != curNode->xmlChildrenNode->type))//儿子节点
		{
			xmlNodePtr curNodeTmp = curNode->xmlChildrenNode;
			xmlChar *szKeyTmp = NULL;
			//网卡信息
			if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DEV_ETHERNET_CFG_S"))
			{	
				printf("NVR_DEV_ETHERNET_CFG_S %d-------------------------\n", i);
				while(curNodeTmp != NULL)
				{
					szKeyTmp = xmlNodeGetContent(curNodeTmp);

					if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u32IpAddr"))
						struCfg.struEtherCfg[i].u32IpAddr = net_ip_a2n((char *) szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u32SubMask"))
						struCfg.struEtherCfg[i].u32SubMask = net_ip_a2n((char *) szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u32GateWay"))
						struCfg.struEtherCfg[i].u32GateWay = net_ip_a2n((char *) szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8MacAddr"))
					{	
						char mac_buf[5] = {'0', 'x'};
						int j = 0;
						for(j = 0; j < 6; j++)
						{
							strncpy(mac_buf+2, (char*)(szKeyTmp+j*2), 2);
							mac_buf[4] = '\0';
							struCfg.struEtherCfg[i].u8MacAddr[j] = (uint8_t)strtoul(mac_buf, NULL, 16);
						}
					}
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8DhcpOn"))
						struCfg.struEtherCfg[i].u8DhcpOn = (uint8_t)strtoul((char*)szKeyTmp, NULL, 10);//(uint8_t)atoi((char*)szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8DhcpStatus"))
						struCfg.struEtherCfg[i].u8DhcpStatus = (uint8_t)strtoul((char*)szKeyTmp, NULL, 10);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8EtherStatus"))
						struCfg.struEtherCfg[i].u8EtherStatus = (uint8_t)strtoul((char*)szKeyTmp, NULL, 10);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8NetcardSpeed"))
						struCfg.struEtherCfg[i].u8NetcardSpeed = (uint8_t)strtoul((char*)szKeyTmp, NULL, 10);
					
					xmlFree(szKeyTmp); 
					curNodeTmp = curNodeTmp->next;
				}
				i++;
			}
			//PPPOE信息
			if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DEV_PPPOE_CFG_S"))
			{	
				printf("NVR_DEV_PPPOE_CFG_S-------------------------\n");
				while(curNodeTmp != NULL)
				{
					szKeyTmp = xmlNodeGetContent(curNodeTmp);

					if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8PppoeOn"))
						struCfg.struPppoeCfg.u8PppoeOn = (uint8_t)atoi((char*)szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8PppoeStatus"))
						struCfg.struPppoeCfg.u8PppoeStatus = (uint8_t)atoi((char*)szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"szPppoeUsr"))
						strncpy(struCfg.struPppoeCfg.szPppoeUsr, (char*)szKeyTmp, NVR_USER_NAME_LEN);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"szPppoePsw"))
						strncpy(struCfg.struPppoeCfg.szPppoePsw, (char*)szKeyTmp, NVR_USER_NAME_LEN);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u32PppoeIp"))
						struCfg.struPppoeCfg.u32PppoeIp = net_ip_a2n((char *) szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u32PppoeTimes"))
						struCfg.struPppoeCfg.u32PppoeTimes = (uint8_t)atoi((char*)szKeyTmp);	

					xmlFree(szKeyTmp); 
					curNodeTmp = curNodeTmp->next;
				}
			}
		}
		else
		{
			szKey = xmlNodeGetContent(curNode);

			if(!xmlStrcmp(curNode->name, BAD_CAST"u32AlarmHostIp"))
				struCfg.u32AlarmHostIp = net_ip_a2n((char *) szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u16AlarmHostPort"))
				struCfg.u16AlarmHostPort = (uint16_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u8DnsDhcp"))
				struCfg.u8DnsDhcp = (uint8_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u32DnsIp1"))
				struCfg.u32DnsIp1 = net_ip_a2n((char *) szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u32DnsIp2"))
				struCfg.u32DnsIp2 = net_ip_a2n((char *) szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u16HttpPort"))
				struCfg.u16HttpPort = (uint16_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u16NetPort"))
				struCfg.u16NetPort = (uint16_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u32MultiCastIp"))
				struCfg.u32MultiCastIp = net_ip_a2n((char *) szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u16MultiCastPort"))
				struCfg.u16MultiCastPort = (uint16_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u16RtspPort"))
				struCfg.u16RtspPort = (uint16_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u16JXJDataPort"))
				struCfg.u16JXJDataPort = (uint16_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u16CmdPort"))
				struCfg.u16CmdPort = (uint16_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u16TalkPort"))
				struCfg.u16TalkPort = (uint16_t)atoi((char*)szKey);

			xmlFree(szKey);
		}	
		curNode = curNode->next;
	}//while
	nvr_logon_set_net_cfg(-1, &struCfg);
}

//设置FTP参数
static void nvr_web_set_ftp_cfg(xmlNodePtr curNode)
{
	//int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_FTP_CFG_S struCfg;
	
	memset(&struCfg, 0, sizeof(struCfg));
	
	nvr_logon_get_ftp_cfg(-1, &struCfg);
	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);

		if(!xmlStrcmp(curNode->name, BAD_CAST"szFtpSvr"))
			strncpy(struCfg.szFtpSvr, (char *) szKey, NVR_IP_ADDR_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szFtpUsr"))
			strncpy(struCfg.szFtpUsr, (char *) szKey, NVR_USER_NAME_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szFtpPsw"))
			strncpy(struCfg.szFtpPsw, (char *) szKey, NVR_PASS_WORD_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szFtpPath"))
			strncpy(struCfg.szFtpPath, (char *) szKey, NVR_PATH_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16FtpPort"))
				struCfg.u16FtpPort = (uint16_t)atoi((char*)szKey);
									
		xmlFree(szKey);
		curNode = curNode->next;
	}
	nvr_logon_set_ftp_cfg(-1, &struCfg);
}

//设置SMTP参数
static void nvr_web_set_smtp_cfg(xmlNodePtr curNode)
{
	//int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_SMTP_CFG_S struCfg;
	
	memset(&struCfg, 0, sizeof(struCfg));
	
	nvr_logon_get_smtp_cfg(-1, &struCfg);
	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);
		
		if(!xmlStrcmp(curNode->name, BAD_CAST"szMailHub"))
			strncpy(struCfg.szMailHub, (char *) szKey, NVR_IP_ADDR_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szMailAddr"))
			strncpy(struCfg.szMailAddr, (char *) szKey, NVR_MAX_EMAIL_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szMailUsr"))
			strncpy(struCfg.szMailUsr, (char *) szKey, NVR_MAX_EMAIL_LEN);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szMailPsw"))
			strncpy(struCfg.szMailPsw, (char *) szKey, NVR_PASS_WORD_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szMailRctp1"))
			strncpy(struCfg.szMailRctp1, (char *) szKey, NVR_MAX_EMAIL_LEN);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szMailRctp2"))
			strncpy(struCfg.szMailRctp2, (char *) szKey, NVR_MAX_EMAIL_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szMailRctp3"))
			strncpy(struCfg.szMailRctp3, (char *) szKey, NVR_MAX_EMAIL_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8EnableSSL"))
			struCfg.u8EnableSSL = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16MailPort"))
			struCfg.u16MailPort = (uint16_t)atoi((char*)szKey);	
			
		xmlFree(szKey);	
		curNode = curNode->next;
	}
	nvr_logon_set_smtp_cfg(-1, &struCfg);
}
//设置DDNS参数
static void nvr_web_set_ddns_cfg(xmlNodePtr curNode)
{
	//int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_DDNS_CFG_S struCfg;
	
	memset(&struCfg, 0, sizeof(struCfg));
	
	nvr_logon_get_ddns_cfg(-1, &struCfg);
	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);
		
		if(!xmlStrcmp(curNode->name, BAD_CAST"u8DDNSOpen"))
			struCfg.u8DDNSOpen = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8DDNSType"))
			struCfg.u8DDNSType = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16DDNSPort"))
			struCfg.u16DDNSPort = (uint16_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szDDNSAccount"))
			strncpy(struCfg.szDDNSAccount, (char *) szKey, NVR_NAME_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szDDNSUsr"))
			strncpy(struCfg.szDDNSUsr, (char *) szKey, NVR_USER_NAME_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szDDNSPsw"))
			strncpy(struCfg.szDDNSPsw, (char *) szKey, NVR_PASS_WORD_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32DDNSTimes"))
			struCfg.u32DDNSTimes = (uint32_t)atoi((char*)szKey);
			
		xmlFree(szKey);
		curNode = curNode->next;
	}
	nvr_logon_set_ddns_cfg(-1, &struCfg);
}

//设置UPNP参数
static void nvr_web_set_upnp_cfg(xmlNodePtr curNode)
{
	//int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_UPNP_CFG_S struCfg;
	
	memset(&struCfg, 0, sizeof(struCfg));
	
	nvr_logon_get_upnp_cfg(-1, &struCfg);
	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);
		
		if(!xmlStrcmp(curNode->name, BAD_CAST"u8UpnpOpen"))
			struCfg.u8UpnpOpen = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8UpnpEthNo"))
			struCfg.u8UpnpEthNo = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8UpnpModel"))
			struCfg.u8UpnpModel = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8UpnpRefTime"))
			struCfg.u8UpnpRefTime = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szUpnpSvr"))
			strncpy(struCfg.szUpnpSvr, (char *) szKey, NVR_IP_ADDR_LEN);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpDataPort"))
			struCfg.u16UpnpDataPort = (uint16_t)atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpWebPort"))
			struCfg.u16UpnpWebPort = (uint16_t)atoi((char*)szKey);		
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpRtspPort"))
			struCfg.u16UpnpRtspPort = (uint16_t)atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpDataPortS"))
			struCfg.u16UpnpDataPortS = (uint16_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpWebPortS"))
			struCfg.u16UpnpWebPortS = (uint16_t)atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpRtspPortS"))
			struCfg.u16UpnpRtspPortS = (uint16_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpCmdPort"))
			struCfg.u16UpnpCmdPort = (uint16_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpTalkPort"))
			struCfg.u16UpnpTalkPort = (uint16_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpJXJDataPort"))
			struCfg.u16UpnpJXJDataPort = (uint16_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpCmdPortS"))
			struCfg.u16UpnpCmdPortS = (uint16_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpTalkPortS"))
			struCfg.u16UpnpTalkPortS = (uint16_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16UpnpJXJDataPortS"))
			struCfg.u16UpnpJXJDataPortS = (uint16_t)atoi((char*)szKey);
		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	nvr_logon_set_upnp_cfg(-1, &struCfg);
}

//设置NTP参数
static void nvr_web_set_ntp_cfg(xmlNodePtr curNode)
{
	//int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_NTP_CFG_S struCfg;
	
	memset(&struCfg, 0, sizeof(struCfg));
	
	nvr_logon_get_ntp_cfg(-1, &struCfg);
	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);
		
		if(!xmlStrcmp(curNode->name, BAD_CAST"u8NtpOpen"))
			struCfg.u8NtpOpen = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16NtpPort"))
			struCfg.u16NtpPort = (uint16_t)atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8EuroTime"))
			struCfg.u8EuroTime = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8NtpRefTime"))
			struCfg.u8NtpRefTime = (uint8_t)atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"nTimeZone"))
			struCfg.nTimeZone = atoi((char*)szKey);		
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szNtpServer"))
			strncpy(struCfg.szNtpServer, (char *) szKey, NVR_IP_ADDR_LEN);
		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	nvr_logon_set_ntp_cfg(-1, &struCfg);
}

//设置USER参数
static void nvr_web_set_user_cfg(xmlNodePtr curNode)
{
	int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_DEV_USER_CFG_S struCfg[NVR_MAX_USR_NUM];
	
	for(i = 0; i < NVR_MAX_USR_NUM;i++)
	{
		memset(&struCfg[i], 0, sizeof(struCfg[i]));
		nvr_logon_get_user_cfg(i, &struCfg[i]);
	}
	
	i = 0;
	while(curNode != NULL)
	{	
		if(NVR_MAX_USR_NUM <= i)
			break;

		if((NULL != (curNode->xmlChildrenNode)) && 
				(XML_TEXT_NODE != curNode->xmlChildrenNode->type))//儿子节点
		{
			xmlNodePtr curNodeTmp = curNode->xmlChildrenNode;

			if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DEV_USER_CFG_S"))
			{
				__DBUG("------------------------------------>>>>NVR_DEV_USER_CFG_S %d\n", i);
				while(NULL != curNodeTmp)
				{
					szKey = xmlNodeGetContent(curNodeTmp);
					
					if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"szUsrName"))
						strncpy(struCfg[i].szUsrName, (char *) szKey, NVR_USER_NAME_LEN);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"szPsw"))
						strncpy(struCfg[i].szPsw, (char *) szKey, NVR_PASS_WORD_LEN);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u64LocalRight"))
						struCfg[i].u64LocalRight = (uint64_t)strtoul((char*)szKey, NULL, 10);	
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u64RemoteRight"))
						struCfg[i].u64RemoteRight = (uint64_t)strtoul((char*)szKey, NULL, 10);

					xmlFree(szKey);
					curNodeTmp = curNodeTmp->next;
				}
				i++;
			}
		}

		curNode = curNode->next;
	}
	
	for(i = 0; i < NVR_MAX_USR_NUM;i++)
		nvr_logon_set_user_cfg(i, &struCfg[i]);
}

//设置时间参数
static void nvr_web_set_time_cfg(xmlNodePtr curNode)
{
	//int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;	
	//NVR_TIME_CFG_S struCfg;
	NVR_TIME_S struCfg;
	
	memset(&struCfg, 0, sizeof(struCfg));
	
	nvr_logon_get_time_cfg(-1, &struCfg);
#if 0	
	while(curNode != NULL)
	{
		if((NULL != (curNode->xmlChildrenNode)) && 
				(XML_TEXT_NODE != curNode->xmlChildrenNode->type))//儿子节点
		{
			xmlNodePtr curNodeTmp = curNode->xmlChildrenNode;
			xmlChar *szKeyTmp = NULL;
			
			if(!xmlStrcmp(curNode->name, BAD_CAST"struTv"))
			{
				while(curNodeTmp != NULL)
				{
					szKeyTmp = xmlNodeGetContent(curNodeTmp);
					
					if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"tv_sec"))
						struCfg.struTv.tv_sec = strtoul((char*)szKeyTmp, NULL, 10);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"tv_usec"))
						struCfg.struTv.tv_usec = strtoul((char*)szKeyTmp, NULL, 10);

					xmlFree(szKeyTmp); 
					curNodeTmp = curNodeTmp->next;
				}
			}
		}
		else
		{		
		
			szKey = xmlNodeGetContent(curNode);
			
			if(!xmlStrcmp(curNode->name, BAD_CAST"u8DateFormat"))
				struCfg.u8DateFormat = (uint8_t)atoi((char*)szKey);	
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u8TimeFormat"))
				struCfg.u8TimeFormat = (uint8_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"cSeparatorChar"))
				struCfg.cSeparatorChar = *((char*)szKey);	
				
			xmlFree(szKey);
		}	
		curNode = curNode->next;
	}
#else 
	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);
		
		if(!xmlStrcmp(curNode->name, BAD_CAST"u8Year"))
			struCfg.u8Year = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8Month"))
			struCfg.u8Month = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8Day"))
			struCfg.u8Day = (uint8_t)atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8WDay"))
			struCfg.u8WDay = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8Hour"))
			struCfg.u8Hour = (uint8_t)atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8Minute"))
			struCfg.u8Minute = (uint8_t)atoi((char*)szKey);		
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8Second"))
			struCfg.u8Second = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8Millisec"))
			struCfg.u8Millisec = (uint8_t)atoi((char*)szKey);
		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	
#endif
	nvr_logon_set_time_cfg(-1, &struCfg);
}

//设置自动任务参数
static void nvr_web_set_auto_cfg(xmlNodePtr curNode)
{
	//int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_AUTO_MAINTAIN_CFG_S struCfg;
	
	memset(&struCfg, 0, sizeof(struCfg));
	
	nvr_logon_get_auto_cfg(-1, &struCfg);
	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);
		
		if(!xmlStrcmp(curNode->name, BAD_CAST"u8WeekDay"))
			struCfg.u8WeekDay = (uint8_t)atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8Hour"))
			struCfg.u8Hour = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8Min"))
			struCfg.u8Min = (uint8_t)atoi((char*)szKey);
		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	nvr_logon_set_auto_cfg(-1, &struCfg);
}

//设置CHANNEl参数
static void nvr_web_set_channel_cfg(xmlNodePtr curNode)
{
	int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_CHANNEL_CFG_S struCfg[NVR_MAX_CHANNEL_NUM];
	
	for(i = 0; i < NVR_MAX_CHANNEL_NUM;i++)
	{
		memset(&struCfg[i], 0, sizeof(struCfg[i]));
		nvr_logon_get_channel_cfg(i, &struCfg[i]);
	}
	
	i = 0;
	while(curNode != NULL)
	{	
		if(NVR_MAX_CHANNEL_NUM <= i)
			break;

		if((NULL != (curNode->xmlChildrenNode)) && 
				(XML_TEXT_NODE != curNode->xmlChildrenNode->type))//儿子节点
		{
			xmlNodePtr curNodeTmp = curNode->xmlChildrenNode;

			if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_CHANNEL_CFG_S"))
			{
				__DBUG("------------------------------------>>>>NVR_CHANNEL_CFG_S %d\n", i);
				while(NULL != curNodeTmp)
				{
					szKey = xmlNodeGetContent(curNodeTmp);
					
					if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"szFactoryName"))
						strncpy(struCfg[i].szFactoryName, (char *)szKey, NVR_NAME_LEN);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"szProtocolName"))
						strncpy(struCfg[i].szProtocolName, (char *)szKey, NVR_NAME_LEN);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"szDevName"))
						strncpy(struCfg[i].szDevName, (char *)szKey, NVR_NAME_LEN);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"szUser"))
						strncpy(struCfg[i].szUser, (char *)szKey, NVR_NAME_LEN);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"szPasswd"))
						strncpy(struCfg[i].szPasswd, (char *)szKey, NVR_NAME_LEN);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u32IpAddr"))
						struCfg[i].u32IpAddr = net_ip_a2n((char *)szKey);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u16DataPort"))
						struCfg[i].u16DataPort = (uint16_t)atoi((char*)szKey);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u16WebPort"))
						struCfg[i].u16WebPort = (uint16_t)atoi((char*)szKey);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u16RtspPort"))
						struCfg[i].u16RtspPort = (uint16_t)atoi((char*)szKey);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8RemoteChn"))
						struCfg[i].u8RemoteChn = (uint8_t)atoi((char*)szKey);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8UseChn"))
						struCfg[i].u8UseChn = (uint8_t)atoi((char*)szKey);	
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8Enable"))
						struCfg[i].u8Enable = (uint8_t)atoi((char*)szKey);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8SearchA"))
						struCfg[i].u8SearchA = (uint8_t)atoi((char*)szKey);

					xmlFree(szKey);
					curNodeTmp = curNodeTmp->next;
				}
				i++;
			}
		}

		curNode = curNode->next;
	}
	for(i = 0; i < NVR_MAX_CHANNEL_NUM;i++)
		nvr_logon_set_channel_cfg(i, &struCfg[i]);
}

//设置COMM信息
static void nvr_web_set_comm_cfg(xmlNodePtr curNode)
{
	int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_DEV_COMM_CFG_S struCfg[NVR_MAX_SUPPORT_COMM_NO];
	
	for(i = 0; i < NVR_MAX_SUPPORT_COMM_NO; i++)
	{
		memset(&struCfg[i], 0, sizeof(struCfg[i]));
		nvr_logon_get_comm_cfg(i, &struCfg[i]);
	}
	i = 0;
	while(curNode != NULL)
	{	
		if(NVR_MAX_SUPPORT_COMM_NO <= i)
			break;

		if((NULL != (curNode->xmlChildrenNode)) && 
				(XML_TEXT_NODE != curNode->xmlChildrenNode->type))//儿子节点
		{
			xmlNodePtr curNodeTmp = curNode->xmlChildrenNode;

			if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DEV_COMM_CFG_S"))
			{
				__DBUG("------------------------------------>>>>NVR_DEV_COMM_CFG_S %d\n", i);
				while(NULL != curNodeTmp)
				{
					szKey = xmlNodeGetContent(curNodeTmp);
					
					if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u32BaudRate"))
						struCfg[i].u32BaudRate = strtoul((char*)szKey, NULL, 10);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8DataBit"))
						struCfg[i].u8DataBit = (uint8_t)atoi((char*)szKey);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8StopBit"))
						struCfg[i].u8StopBit = (uint8_t)atoi((char*)szKey);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8Parity"))
						struCfg[i].u8Parity = (uint8_t)atoi((char*)szKey);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8Flowcontrol"))
						struCfg[i].u8Flowcontrol = (uint8_t)atoi((char*)szKey);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u32BaudMask"))
						struCfg[i].u32BaudMask = atoi((char*)szKey);

					xmlFree(szKey);
					curNodeTmp = curNodeTmp->next;
				}
				i++;
			}
		}

		curNode = curNode->next;
	}
		
	for(i = 0; i < NVR_MAX_SUPPORT_COMM_NO; i++)
		nvr_logon_set_comm_cfg(i, &struCfg[i]);
}

//设置rec配置
static void nvr_web_set_rec_cfg(xmlNodePtr curNode)
{
	int i = 0, j = 0;
	xmlChar *szKey = NULL;
	NVR_DEV_REC_STRC_S struCfg;
	curNode = curNode->xmlChildrenNode;
	memset(&struCfg, 0, sizeof(struCfg));
	
	nvr_logon_get_rec_cfg(-1, &struCfg);
	printf("-----------------------------------------nvr_web_set_rec_cfg\n");
	while(curNode != NULL)
	{
		if((NULL != (curNode->xmlChildrenNode)) && 
				(XML_TEXT_NODE != curNode->xmlChildrenNode->type))//儿子节点
		{
			xmlNodePtr curNodeTmp = curNode->xmlChildrenNode;
			xmlChar *szKeyTmp = NULL;
			//网卡信息
			if(!xmlStrcmp(curNode->name, BAD_CAST"struTimerSeg"))
			{	
				printf("NVR_DEV_ETHERNET_CFG_S %d-------------------------\n", i);
				j = 0;
				while(curNodeTmp != NULL)
				{
					szKeyTmp = xmlNodeGetContent(curNodeTmp);

					 if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"u8Open"))
						struCfg.struTimerSeg[i][j].u8Open = (uint8_t)atoi((char*)szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"beginSec"))
						struCfg.struTimerSeg[i][j].beginSec = atoi((char*)szKeyTmp);
					else if(!xmlStrcmp(curNodeTmp->name, BAD_CAST"endSec"))
						struCfg.struTimerSeg[i][j++].endSec = atoi((char*)szKeyTmp);//j++ 顺序不能乱
					
					xmlFree(szKeyTmp); 
					curNodeTmp = curNodeTmp->next;
				}
				i++;
			}	
		}
		else
		{
			szKey = xmlNodeGetContent(curNode);
			
			if(!xmlStrcmp(curNode->name, BAD_CAST"u8OverWrite"))
				struCfg.u8OverWrite = (uint8_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u8RecStream"))
				struCfg.u8RecStream = (uint8_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u8PreRecSecs"))
				struCfg.u8PreRecSecs = (uint8_t)atoi((char*)szKey);
			else if(!xmlStrcmp(curNode->name, BAD_CAST"u8SaveMode"))
				struCfg.u8SaveMode = (uint8_t)atoi((char*)szKey);

			xmlFree(szKey);
		}
							
		curNode = curNode->next;
	}
	nvr_logon_set_rec_cfg(-1, &struCfg);
}

static void nvr_web_set_search_cfg(xmlNodePtr curNode, long lHandle)
{
	int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	IPNC_NET_SEARCH_CFG_REQ_S struCfg;
	
	memset(&struCfg, 0, sizeof(struCfg));	
	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);
		
		if(!xmlStrcmp(curNode->name, BAD_CAST"u32CliMultiIp"))
			;//struCfg.u32CliMultiIp = net_ip_a2n((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16CliMultiPort"))
			;//struCfg.u16CliMultiPort = (uint16_t)atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szUsrName"))
			strncpy(struCfg.szUsrName, (char*)szKey, sizeof(struCfg.szUsrName));
		else if(!xmlStrcmp(curNode->name, BAD_CAST"szPsw"))
			strncpy(struCfg.szPsw, (char*)szKey, sizeof(struCfg.szPsw));	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8SerialId"))
		{
			//memcpy(&struCfg.u8SerialId, szKey, sizeof(struCfg.u8SerialId));		
			//序列号
			for(i = 0; i < strlen((char*)szKey) && i < IPNC_SERIALNO_LEN; i++)
			{
				struCfg.u8SerialId[i] = szKey[i];
			}
			__DBUG("------------------------------------>>>>%d\n", i);
		}
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32SoftVer"))
			struCfg.u32SoftVer = atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16DevType"))
			struCfg.u16DevType = (uint16_t)atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16DevPort"))
			struCfg.u16DevPort = (uint16_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32DevIp"))
			struCfg.u32DevIp = net_ip_a2n((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8MacAddr"))
		{
			char mac_buf[5] = {'0', 'x'};
			int i = 0;
			for(i = 0; i < IPNC_MAC_ADDR_LEN; i++)
			{
				strncpy(mac_buf+2, (char*)(szKey+i*2), 2);
				mac_buf[4] = '\0';
				struCfg.u8MacAddr[i] = (uint8_t)strtoul(mac_buf, NULL, 16);
			}
		}
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8Dns"))
			struCfg.u8Dns = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8DHCP"))
			struCfg.u8DHCP = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32DevIpMask"))
			struCfg.u32DevIpMask = net_ip_a2n((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32DevGwAddr"))
			struCfg.u32DevGwAddr = net_ip_a2n((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32DevDns1"))
			struCfg.u32DevDns1 = net_ip_a2n((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32DevDns2"))
			struCfg.u32DevDns2 = net_ip_a2n((char*)szKey);
/*			
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32Capability0"))
			struCfg.u32Capability0 = atoi((char*)szKey);	
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32Capability1"))
			struCfg.u32Capability1 = atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32Capability2"))
			struCfg.u32Capability2 = atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8MainDevType"))
			struCfg.u8MainDevType = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u8SubDevType"))
			struCfg.u8SubDevType = (uint8_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u16PlateformCap"))
			struCfg.u16PlateformCap = (uint16_t)atoi((char*)szKey);
		else if(!xmlStrcmp(curNode->name, BAD_CAST"u32UpTime"))
			struCfg.u32UpTime = atoi((char*)szKey);
*/		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	
	__DBUG("<<<<<<<<<<<<<<<>>>>>>>>>>>>>>u8SerialId: %s\n", struCfg.u8SerialId);						
	//发送搜索配置包
	nvr_lib_search_cfg(lHandle, &struCfg, sizeof(IPNC_NET_SEARCH_CFG_REQ_S));
}


/*
static void nvr_web_set_comm_cfg(xmlNodePtr curNode)
{
	int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_DEV_COMM_CFG_S struCfg[NVR_MAX_SUPPORT_COMM_NO];
	
	for(i = 0; i < NVR_MAX_SUPPORT_COMM_NO; i++)
	{
		memset(&struCfg[i], 0, sizeof(struCfg[i]));
		nvr_logon_get_comm_cfg(i, &struCfg[i]);
	}
	i = 0;
	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);
		
		if(i < NVR_MAX_SUPPORT_COMM_NO)
		{
			
		}	
		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	for(i = 0; i < NVR_MAX_SUPPORT_COMM_NO; i++)
		nvr_logon_set_comm_cfg(i, &struCfg[i]);
}
static void nvr_web_set_ftp_cfg(xmlNodePtr curNode)
{
	//int i = 0;
	curNode = curNode->xmlChildrenNode;
	xmlChar *szKey = NULL;
	NVR_FTP_CFG_S struCfg;
	
	memset(&struCfg, 0, sizeof(struCfg));
	
	nvr_logon_get_ftp_cfg(-1, &struCfg);
	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);
		
		xmlFree(szKey);
		curNode = curNode->next;
	}
	nvr_logon_set_smtp_cfg(-1, &struCfg);
}*/

//读xml文件
int nvr_web_read_xml(const char* xmlData, long lHandle)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr curNode = NULL;
	int ret = 0;
	
	if(NULL != xmlData)
	{
		//解析xml文件
		xmlKeepBlanksDefault(0);
		//doc = xmlReadFile(XMLPOSTFILE,NULL, XML_PARSE_NOBLANKS | XML_PARSE_NODICT);
		doc = xmlReadMemory(xmlData, strlen(xmlData), 
		                    NULL, NULL, XML_PARSE_NOBLANKS | XML_PARSE_NODICT);
		if(NULL != doc)
		{
			curNode = xmlDocGetRootElement(doc);
			if(NULL != curNode)
			{
				if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DEV_NET_CFG_S"))
					nvr_web_set_net_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DEV_INFO_S"))
					nvr_web_set_dev_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_FTP_CFG_S"))
					nvr_web_set_ftp_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_SMTP_CFG_S"))
					nvr_web_set_smtp_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DDNS_CFG_S"))
					nvr_web_set_ddns_cfg(curNode);	
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_UPNP_CFG_S"))
					nvr_web_set_upnp_cfg(curNode);	
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_NTP_CFG_S"))
					nvr_web_set_ntp_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DEV_USER_CFG_S"))
					nvr_web_set_user_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_TIME_S"))
					nvr_web_set_time_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_AUTO_MAINTAIN_CFG_S"))
					nvr_web_set_auto_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_CHANNEL_CFG_S"))
					nvr_web_set_channel_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DEV_COMM_CFG_S"))
					nvr_web_set_comm_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"NVR_DEV_REC_STRC_S"))
					nvr_web_set_rec_cfg(curNode);
				else if(!xmlStrcmp(curNode->name, BAD_CAST"IPNC_NET_SEARCH_CFG_REQ_S"))
				{
					nvr_web_set_search_cfg(curNode, lHandle);
				}	
			}
		}
		else
			ret = -1;
	}
	else
		ret = -1;

	return ret;
}

#endif
