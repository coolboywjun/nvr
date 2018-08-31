/*
 * main.c -- Main program for the GoAhead WebServer (LINUX version)
 *
 * Copyright (c) GoAhead Software Inc., 1995-2000. All Rights Reserved.
 *
 * See the file "license.txt" for usage and redistribution license requirements
 *
 * $Id: main.c,v 1.5 2003/09/11 14:03:46 bporter Exp $
 */

/******************************** Description *********************************/

/*
 *	Main program for for the GoAhead WebServer. This is a demonstration
 *	main program to initialize and configure the web server.
 */

/********************************* Includes ***********************************/
#include	"uemf.h"
#include	"wsIntrn.h"
#include	<signal.h>
#include	<unistd.h> 
#include	<sys/types.h>
#include	<sys/wait.h>
#include 	<sys/time.h>
#include 	<sys/socket.h>
#include 	<net/if.h>
#include 	<sys/ioctl.h>
#include    <stdlib.h>

#include <av_pool.h>
#include <storage_lib.h>
#include <syscfg.h>
#include "hi_platform_onvif.h"

#define ONVIF_OPEN

#ifdef WEBS_SSL_SUPPORT
#include	"websSSL.h"
#endif

#ifdef USER_MANAGEMENT_SUPPORT
#include	"um.h"
void	formDefineUserMgmt(void);
#endif

#include "nvr_lib_interface.h"
#include "nvr_xml_cfg.h"
#include "nvr_web_proc.h"
#include "stdsoap2.h"

/*********************************** Locals ***********************************/
/*
 *	Change configuration here
 */
//static char_t		*rootWeb = T("/webs");			/* Root web directory */
static char_t		*rootWeb = T("/var/www");			/* Root web directory */
static char_t		*password = T("");				/* Security password */
static int			port = 80;						/* Server port */
static int			retries = 5;					/* Server port retries */
static int			finished;						/* Finished flag */

/****************************** Forward Declarations **************************/

static int 	initWebs();
static int  websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
		int arg, char_t *url, char_t *path, char_t *query);
extern void defaultErrorHandler(int etype, char_t *msg);
extern void defaultTraceHandler(int level, char_t *buf);
#ifdef B_STATS
static void printMemStats(int handle, char_t *fmt, ...);
static void memLeaks();
#endif

/*********************************** Code *************************************/
/*
 *	Main -- entry point from LINUX
 */

static int websOnvifServer(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
	char_t *url, char_t *path, char_t *query)
{
	socket_t	*sp;
	struct soap gsoap;
	DEV_SystemCfg *pSystemCfg;
	if(websValid(wp)) {}
	else    return 1;
	pSystemCfg = get_system_cfg();
	if(!(pSystemCfg->platformServer & PLATFORM_ONVIF_SERVER))
		return 0;
	soap_init (&gsoap);		
	sp = socketPtr(wp->sid);
	gsoap.socket = sp->sock;
	sp->sock = -1;
	
	soap_serve(&gsoap, (char *)sp->inBuf.buf);
	soap_destroy (&gsoap);
	soap_end (&gsoap); /* clear soap */
	soap_done(&gsoap);
	websDone(wp, 200);

	return 0;
}


int main(void)
{
	int bStartOnvif = 0;
	DEV_SystemCfg *pSystemCfg = NULL;
	/*
	 *	Initialize the memory allocator. Allow use of malloc and start 
	 *	with a 60K heap.  For each page request approx 8KB is allocated.
	 *	60KB allows for several concurrent page requests.  If more space
	 *	is required, malloc will be used for the overflow.
	 */
	bopen(NULL, (60 * 1024), B_USE_MALLOC);
	signal(SIGPIPE, SIG_IGN);
	
	//初始化信号量值1
	sem_init(&g_xmlLock, 0, 1);	
#ifdef ONVIF_OPEN	
	if(av_pool_init() < 0)
	{
		printf("av pool init failed.\n");
		return -1;
	}
#endif
	//init web
	__DBUG("init web ...\n");
	if(0 > nvr_lib_init())
	{
		__ERR("logon server failed ...\n");
		return -1;
	}
	storage_lib_init();
	pSystemCfg = get_system_cfg();
	if(pSystemCfg->platformServer & PLATFORM_ONVIF_SERVER)
	{
		hi_onvif_init();
		bStartOnvif = 1;
	}
	
	if (initWebs() < 0) {
		__ERR("web server init failed ...\n");
		return -1;
	}

#ifdef WEBS_SSL_SUPPORT
	websSSLOpen();
#endif

	/*
	 *	Basic event loop. SocketReady returns true when a socket is ready for
	 *	service. SocketSelect will block until an event occurs. SocketProcess
	 *	will actually do the servicing.
	 */
	while (!finished) {
		if (socketReady(-1) || socketSelect(-1, 1000)) {
			socketProcess(-1);
		}
		websCgiCleanup();
		emfSchedProcess();
	}

#ifdef WEBS_SSL_SUPPORT
	websSSLClose();
#endif

#ifdef USER_MANAGEMENT_SUPPORT
	umClose();
#endif

	/*
	 *	Close the socket module, report memory leaks and close the memory allocator
	 */
	websCloseServer();
	socketClose();
#ifdef B_STATS
	memLeaks();
#endif
	bclose();
	sem_destroy(&g_xmlLock);
#ifdef ONVIF_OPEN	
	if(bStartOnvif)
		hi_onvif_uninit();
	storage_lib_uninit();
	nvr_lib_uninit();
#endif	
	return 0;
}

/******************************************************************************/
/*
 *	Initialize the web server.
 */
//============================================================================
// 函数名称: net_NetConfig_GetIpAddr()
// 功能描述: 得到指定名字的网络ip
// 其他说明:
//============================================================================
static int get_local_ip_addr(char *name,char *net_ip)
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
		strcpy(net_ip, inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr));

	close(fd);

	return	ret;
}

static int getParam(webs_t wp, char_t *urlPrefix, char_t *webDir, int arg, 
		char_t *url, char_t *path, char_t *query)
{
	if(NULL == query)
		return -1;

	//__DBUG("url:%s\n", url);
	//__DBUG("path:%s\n", path);
	//__DBUG("flags:%#x\n", wp->flags);
	//__DBUG("state:%#x\n", wp->state);
	//__DBUG("query:%s, %d\n", query, strlen(query));

	if (wp->flags & WEBS_POST_REQUEST) 
	{
		int i = 0;
		
		__DBUG(">>>>post cmp: %s\n", query);
		if((('-' == query[0]) && ('0' < query[1] && query[1] <= '9')) ||
			    ('0' < query[0] && query[0] <= '9'))
		{
			long lHandle = 0;
			char buf[64] = {0};
			
			for(i = 0; i < sizeof(buf); i++)
			{
				if('&' == query[i])
				{
					buf[i] = '\0';
					lHandle = strtol(buf, NULL, 10);
					__DBUG("lHandle: %lx\n", lHandle);
					nvr_web_read_xml(query + i + 1, lHandle);
				}
				else
					buf[i] = query[i];
			}
		}
		else
		{
			if( 0 > nvr_web_read_xml(query, 0))
				__ERR("nvr_web_read_xml failed !\n");
		}
			
	
	}
	else//get
	{
		__DBUG(">>>>get cmp: %s\n", query);
		if(0 == strcasecmp(query, "DEV"))
		{
			if(0 > nvr_web_get_dev_cfg(wp->sid))
				__ERR("get_dev_cfg failed !");
		}
		else if(0 == strcasecmp(query, "NET"))
		{
			if(0 > nvr_web_get_net_cfg(wp->sid))
				__ERR("get_net_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "FTP"))
		{
			if(0 > nvr_web_get_ftp_cfg(wp->sid))
				__ERR("get_ftp_cfg failed !");
		}
		else if(0 == strcasecmp(query, "SMTP"))
		{
			if(0 > nvr_web_get_smtp_cfg(wp->sid))
				__ERR("get_smtp_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "DDNS"))
		{
			if(0 > nvr_web_get_ddns_cfg(wp->sid))
				__ERR("get_ddns_cfg failed !");
		}
		else if(0 == strcasecmp(query, "UPNP"))
		{
			if(0 > nvr_web_get_upnp_cfg(wp->sid))
				__ERR("get_upnp_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "NTP"))
		{
			if(0 > nvr_web_get_ntp_cfg(wp->sid))
				__ERR("get_ntp_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "USER"))
		{
			if(0 > nvr_web_get_user_cfg(wp->sid))
				__ERR("get_user_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "TIME"))
		{
			if(0 > nvr_web_get_time_cfg(wp->sid))
				__ERR("get_time_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "AUTO"))
		{
			if(0 > nvr_web_get_auto_cfg(wp->sid))
				__ERR("get_auto_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "CHANNEL"))
		{
			if(0 > nvr_web_get_channel_cfg(wp->sid))
				__ERR("get_channel_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "COMM"))
		{
			if(0 > nvr_web_get_comm_cfg(wp->sid))
				__ERR("get_channel_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "REC"))
		{
			if(0 > nvr_web_get_rec_cfg(wp->sid))
				__ERR("get_rec_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "DIANXIN"))
		{
			if(0 > nvr_web_get_dianxin_platform_cfg(wp->sid))
				__ERR("get_rec_cfg failed !\n");
		}
		else if(0 == strcasecmp(query, "SEARCHSTART"))
		{
			if(0 > nvr_web_get_search_start(wp->sid))
				__ERR("nvr_web_get_search_start failed !\n");
		}
		else if(0 == strncmp(query, "SEARCHGET", strlen("SEARCHGET")))//SEARCHGET?lHandle值
		{
			
			long lHandle = 0;
			
			__DBUG("query: %s\n", query);
			lHandle = strtol(query + strlen("SEARCHGET?"), NULL, 10);
			__DBUG("lHandle: %lx\n", lHandle);
			
			if(0 > nvr_web_get_search_get(wp->sid, lHandle))
				__ERR("nvr_web_get_search_get failed !\n");
		}
		else if(0 == strncmp(query, "SEARCHSTOP", strlen("SEARCHSTOP")))//SEARCHSTOP?lHandle值
		{
			long lHandle = 0;
			
			lHandle = strtol(query + strlen("SEARCHSTOP?"), NULL, 10);
			__DBUG("lHandle: %lx\n", lHandle);
			
			if(0 > nvr_web_get_search_stop(wp->sid, lHandle))
				__ERR("nvr_web_get_search_stop failed !\n");
		}
		else if(0 == strncmp(query, "wdt", strlen("wdt")))
		{
			int value;
			DEV_SystemCfg *pCfg = get_system_cfg();
			if(sscanf(query, "wdt=%d", &value) != 1)
			{
				if(!pCfg->u8NoWdt)
					websResponse(wp, 200, T("WDT status: enable"), NULL);
				else
					websResponse(wp, 200, T("WDT status: disable"), NULL);
				return 0;
			}
			else
			{
				if(pCfg->u8NoWdt != !value)
				{
					pCfg->u8NoWdt = !value;
					save_system_cfg(pCfg);
				}
				if(value)
					websResponse(wp, 200, T("Enable WDT OK!"), NULL);
				else
					websResponse(wp, 200, T("Disable WDT OK!"), NULL);
				return 0;
			}
		}
		else if(0 == strncmp(query, "check", strlen("check")))
		{
			int value;
			DEV_SystemCfg *pCfg = get_system_cfg();
			if(sscanf(query, "check=%d", &value) != 1)
			{
				if(!pCfg->u8FactoryChecked)
					websResponse(wp, 200, T("Factory check flag: 0"), NULL);
				else
					websResponse(wp, 200, T("Factory check flag: 1"), NULL);
				return 0;
			}
			else
			{
				if(pCfg->u8FactoryChecked != value)
				{
					pCfg->u8FactoryChecked = value;
					save_system_cfg(pCfg);
				}
				if(value)
					websResponse(wp, 200, T("Set factory checked flag OK!"), NULL);
				else
					websResponse(wp, 200, T("Clear factory checked flag OK!"), NULL);
				return 0;
			}
		}
		else if(0 == strncmp(query, "openserver", strlen("openserver")))
		{
			char value[256];
			DEV_SystemCfg *pCfg = get_system_cfg();
			if(sscanf(query, "openserver=%s", value) != 1)
			{
				char response[256];
				char *pStr = response;
				pStr += sprintf(pStr, "Support platform server: JXJ ");
				if(pCfg->platformServer & PLATFORM_ONVIF_SERVER)
					pStr += sprintf(pStr, "ONVIF ");
				if(pCfg->platformServer & PLATFORM_TUTK_SERVER)
					pStr += sprintf(pStr, "TUTK ");
				if(pCfg->platformServer & PLATFORM_VIVI_SERVER)
					pStr += sprintf(pStr, "VIVI ");
				if(pCfg->platformServer & PLATFORM_KANDIAN_SERVER)
					pStr += sprintf(pStr, "KANDIAN ");
				pStr += sprintf(pStr, "     ");
				
				pStr += sprintf(pStr, "Don't support platform server: ");
				if(!(pCfg->platformServer & PLATFORM_ONVIF_SERVER))
					pStr += sprintf(pStr, "ONVIF ");
				if(!(pCfg->platformServer & PLATFORM_TUTK_SERVER))
					pStr += sprintf(pStr, "TUTK ");
				if(!(pCfg->platformServer & PLATFORM_VIVI_SERVER))
					pStr += sprintf(pStr, "VIVI ");
				if(!(pCfg->platformServer & PLATFORM_KANDIAN_SERVER))
					pStr += sprintf(pStr, "KANDIAN ");
				websResponse(wp, 200, response, NULL);
				return 0;
			}
			else
			{
				if(strcasecmp(value, "ONVIF") == 0)
					pCfg->platformServer |= PLATFORM_ONVIF_SERVER;
				else if(strcasecmp(value, "TUTK") == 0)
					pCfg->platformServer |= PLATFORM_TUTK_SERVER;
				else if(strcasecmp(value, "VIVI") == 0)
					pCfg->platformServer |= PLATFORM_VIVI_SERVER;
				else if(strcasecmp(value, "KANDIAN") == 0)
					pCfg->platformServer |= PLATFORM_KANDIAN_SERVER;
				save_system_cfg(pCfg);
				websResponse(wp, 200, T("Open platform server flag OK!"), NULL);
				return 0;
			}
		}
		else if(0 == strncmp(query, "closeserver", strlen("closeserver")))
		{
			char value[256];
			DEV_SystemCfg *pCfg = get_system_cfg();
			if(sscanf(query, "closeserver=%s", value) != 1)
			{
				websResponse(wp, 200, T("The input paramete error."), NULL);
				return 0;
			}
			else
			{
				if(strcasecmp(value, "ONVIF") == 0)
					pCfg->platformServer &= ~PLATFORM_ONVIF_SERVER;
				else if(strcasecmp(value, "TUTK") == 0)
					pCfg->platformServer &= ~PLATFORM_TUTK_SERVER;
				else if(strcasecmp(value, "VIVI") == 0)
					pCfg->platformServer &= ~PLATFORM_VIVI_SERVER;
				else if(strcasecmp(value, "KANDIAN") == 0)
					pCfg->platformServer &= ~PLATFORM_KANDIAN_SERVER;
				save_system_cfg(pCfg);
				websResponse(wp, 200, T("Close platform server flag OK!"), NULL);
				return 0;
			}
		}		
		else if(0 == strncmp(query, "openclient", strlen("openclient")))
		{
			char value[256];
			DEV_SystemCfg *pCfg = get_system_cfg();
			if(sscanf(query, "openclient=%s", value) != 1)
			{
				char response[256];
				char *pStr = response;
				pStr += sprintf(pStr, "Support platform client: JXJV1 JXJV2 ");
				if(pCfg->platformClient & PLATFORM_ONVIF_CLIENT)
					pStr += sprintf(pStr, "ONVIF ");
				pStr += sprintf(pStr, "      ");
				
				pStr += sprintf(pStr, "Don't support platform client: JXJV1 JXJV2 ");
				if(!(pCfg->platformClient & PLATFORM_ONVIF_CLIENT))
					pStr += sprintf(pStr, "ONVIF ");
				websResponse(wp, 200, response, NULL);
				return 0;
			}
			else
			{
				if(strcasecmp(value, "ONVIF") == 0)
					pCfg->platformClient |= PLATFORM_ONVIF_CLIENT;
				save_system_cfg(pCfg);
				websResponse(wp, 200, T("Open platform client flag OK!"), NULL);
				return 0;
			}
		}
		else if(0 == strncmp(query, "closeclient", strlen("closeclient")))
		{
			char value[256];
			DEV_SystemCfg *pCfg = get_system_cfg();
			if(sscanf(query, "closeclient=%s", value) != 1)
			{
				websResponse(wp, 200, T("The input paramete error."), NULL);
				return 0;
			}
			else
			{
				if(strcasecmp(value, "ONVIF") == 0)
					pCfg->platformClient &= ~PLATFORM_ONVIF_CLIENT;
				save_system_cfg(pCfg);
				websResponse(wp, 200, T("Close platform client flag OK!"), NULL);
				return 0;
			}
		}	
		else if(0 == strncmp(query, "setlang", strlen("setlang")))
		{
			char value[256];
			DEV_SystemCfg *pCfg = get_system_cfg();
			if(sscanf(query, "setlang=%s", value) != 1)
			{
				char response[256];
				char *pStr = response;
				pStr += sprintf(pStr, "Support language: ");
				if(pCfg->supportLang & (1<<LANG_SIMPLIFIED_CHINESE))
					pStr += sprintf(pStr, "simplified chinese, ");
				if(pCfg->supportLang & (1<<LANG_ENGLISH))
					pStr += sprintf(pStr, "english, ");
				if(pCfg->supportLang & (1<<LANG_TRADITIONAL_CHINESE))
					pStr += sprintf(pStr, "traditional chinese, ");
				pStr += sprintf(pStr, "      ");
				
				pStr += sprintf(pStr, "Don't support platform lang: ");
				if(!(pCfg->supportLang & (1<<LANG_SIMPLIFIED_CHINESE)))
					pStr += sprintf(pStr, "simplified chinese, ");
				if(!(pCfg->supportLang & (1<<LANG_ENGLISH)))
					pStr += sprintf(pStr, "english, ");
				if(!(pCfg->supportLang & (1<<LANG_TRADITIONAL_CHINESE)))
					pStr += sprintf(pStr, "traditional chinese, ");
				websResponse(wp, 200, response, NULL);
				return 0;
			}
			else
			{
				if(strcasecmp(value, "simp") == 0)
					pCfg->supportLang |= 1<<LANG_SIMPLIFIED_CHINESE;
				else if(strcasecmp(value, "english") == 0)
					pCfg->supportLang |= 1<<LANG_ENGLISH;
				else if(strcasecmp(value, "trad") == 0)
					pCfg->supportLang |= 1<<LANG_TRADITIONAL_CHINESE;
				save_system_cfg(pCfg);
				websResponse(wp, 200, T("Set language flag OK!"), NULL);
				return 0;
			}
		}
		else if(0 == strncmp(query, "clrlang", strlen("clrlang")))
		{
			char value[256];
			DEV_SystemCfg *pCfg = get_system_cfg();
			if(sscanf(query, "clrlang=%s", value) != 1)
			{
				websResponse(wp, 200, T("The input paramete error."), NULL);
				return 0;
			}
			else
			{
				if(strcasecmp(value, "simp") == 0)
					pCfg->supportLang &= ~(1<<LANG_SIMPLIFIED_CHINESE);
				else if(strcasecmp(value, "english") == 0)
					pCfg->supportLang &= ~(1<<LANG_ENGLISH);
				else if(strcasecmp(value, "trad") == 0)
					pCfg->supportLang &= ~(1<<LANG_TRADITIONAL_CHINESE);
				save_system_cfg(pCfg);
				websResponse(wp, 200, T("Clear language flag OK!"), NULL);
				return 0;
			}
		}
		else if(0 == strncmp(query, "deflang", strlen("deflang")))
		{
			char value[256];
			int defLang = 0;
			DEV_SystemCfg *pCfg = get_system_cfg();
			if(sscanf(query, "deflang=%s", value) != 1)
			{
				websResponse(wp, 200, T("The input paramete error."), NULL);
				return 0;
			}
			else
			{
				if(strcasecmp(value, "simp") == 0)
					defLang = 0;
				else if(strcasecmp(value, "english") == 0)
					defLang = 1;
				else if(strcasecmp(value, "trad") == 0)
					defLang = 2;
				if(((1<<defLang) & pCfg->supportLang) == 0)
				{
					websResponse(wp, 200, T("The input default language don't support!!!"), NULL);
					return 0;
				}
				pCfg->u8DefLang = defLang;
				save_system_cfg(pCfg);
				websResponse(wp, 200, T("Set default language flag OK!"), NULL);
				return 0;
			}
		}
		else
		{
			__DBUG("no cmd!!!!query: %s\n", query);
			websResponse(wp, 200, T("Paramete name: dev net ftp smtp ddns upnp ntp user time auto channel comm rec dianxin wdt check"), NULL);
			return 0;
		}
	}

	return 0;
}

#define LOCAL_ETH_NAME "eth0"
static int initWebs()
{
	struct in_addr	intaddr;
	char			host[128],/* dir[128],*/ webdir[128];
	char			*cp;
	char_t			wbuf[128];
	char ip[32] = {0};
	NVR_DEV_NET_CFG_S struCfg;

	/*
	 *	Initialize the socket subsystem
	 */
	socketOpen();

#ifdef USER_MANAGEMENT_SUPPORT
	/*
	 *	Initialize the User Management database
	 */
	umOpen();
	umRestore(T("umconfig.txt"));
#endif

	/*
	 *	Define the local Ip address, host name, default home page and the 
	 *	root web directory.
	 */
	if (gethostname(host, sizeof(host)) < 0) {
		error(E_L, E_LOG, T("Can't get hostname"));
		return -1;
	}

	get_local_ip_addr(LOCAL_ETH_NAME, ip);
	intaddr.s_addr = inet_addr(ip); // 服务器IP
	__DBUG("webip:%s\n",ip);

	/*
	 *	Set ../web as the root web. Modify this to suit your needs
	 */
	strcpy(webdir, rootWeb);
	__DBUG("webdir:%s\n", webdir);

	/*
	 *	Configure the web server options before opening the web server
	 */
	websSetDefaultDir(webdir);
	cp = inet_ntoa(intaddr);
	ascToUni(wbuf, cp, min(strlen(cp) + 1, sizeof(wbuf)));
	websSetIpaddr(wbuf);
	ascToUni(wbuf, host, min(strlen(host) + 1, sizeof(wbuf)));
	websSetHost(wbuf);

	/*
	 *	Configure the web server options before opening the web server
	 */
	websSetDefaultPage(T("default.asp"));
	websSetPassword(password);

	/* 
	 *	Open the web server on the given port. If that port is taken, try
	 *	the next sequential port for up to "retries" attempts.
	 */

	memset(&struCfg, 0, sizeof(struCfg));
	nvr_logon_get_net_cfg(-1, &struCfg);
	port = struCfg.u16HttpPort;
	__DBUG("struCfg.u16HttpPort:%d\n", struCfg.u16HttpPort);
	if(0 >= port)
		port = 80;
	__DBUG("webPort:%d\n", port);
	websOpenServer(port, retries);

	/*
	 * 	First create the URL handlers. Note: handlers are called in sorted order
	 *	with the longest path handler examined first. Here we define the security 
	 *	handler, forms handler and the default web page handler.
	 */
	websUrlHandlerDefine(T(""), NULL, 0, websSecurityHandler, 
			WEBS_HANDLER_FIRST);
	//websUrlHandlerDefine(T("/webs"), NULL, 0, websFormHandler, 0);
	websUrlHandlerDefine(T("/webs"), "/var/www", 0, websFormHandler, 0);
	websUrlHandlerDefine(T("/cgi-bin"), NULL, 0, websCgiHandler, 0);
	websUrlHandlerDefine(T(""), NULL, 0, websDefaultHandler, 
			WEBS_HANDLER_LAST); 
	
	websUrlHandlerDefine((char_t  *)T("/onvif"), NULL, 0, websOnvifServer, 0);	  //by zhx. 10-1-2014 16:56:38
	websUrlHandlerDefine(T("/nvr.html"), NULL, 0, getParam, 0);

	/*
	 *	Now define two test procedures. Replace these with your application
	 *	relevant ASP script procedures and form functions.
	 */
	websAspDefine((char *)T("getLoginInfo"), getLoginInfo);
	websFormDefine((char *)T("login"), loginInfo);
	
	websAspDefine((char *)T("getMainInfo"), getMainInfo);
	
	/*
	 *	Create the Form handlers for the User Management pages
	 */
#ifdef USER_MANAGEMENT_SUPPORT
	formDefineUserMgmt();
#endif

	/*
	 *	Create a handler for the default home page
	 */
	websUrlHandlerDefine(T("/"), NULL, 0, websHomePageHandler, 0); 
	return 0;
}

/******************************************************************************/
/*
 *	Home page handler
 */

static int websHomePageHandler(webs_t wp, char_t *urlPrefix, char_t *webDir,
		int arg, char_t *url, char_t *path, char_t *query)
{
	/*
	 *	If the empty or "/" URL is invoked, redirect default URLs to the home page
	 */
	if (*url == '\0' || gstrcmp(url, T("/")) == 0) {
		websRedirect(wp, T("login.asp"));
		return 1;
	}
	return 0;
}

/******************************************************************************/
/*
 *	Default error handler.  The developer should insert code to handle
 *	error messages in the desired manner.
 */

void defaultErrorHandler(int etype, char_t *msg)
{
#if 0
	write(1, msg, gstrlen(msg));
#endif
}

/******************************************************************************/
/*
 *	Trace log. Customize this function to log trace output
 */

void defaultTraceHandler(int level, char_t *buf)
{
	/*
	 *	The following code would write all trace regardless of level
	 *	to stdout.
	 */
#if 0
	if (buf) {
		write(1, buf, gstrlen(buf));
	}
#endif
}

/******************************************************************************/
/*
 *	Returns a pointer to an allocated qualified unique temporary file name.
 *	This filename must eventually be deleted with bfree();
 */

char_t *websGetCgiCommName()
{
	char_t	*pname1, *pname2;

	//pname1 = tempnam(NULL, T("cgi"));
	pname1 = (char_t *)mkstemp(T("cgi")); 
	pname2 = bstrdup(B_L, pname1);
	free(pname1);
	return pname2;
}

/******************************************************************************/
/*
 *	Launch the CGI process and return a handle to it.
 */

int websLaunchCgiProc(char_t *cgiPath, char_t **argp, char_t **envp,
		char_t *stdIn, char_t *stdOut)
{
	int	pid, fdin, fdout, hstdin, hstdout, rc;

	fdin = fdout = hstdin = hstdout = rc = -1; 
	if ((fdin = open(stdIn, O_RDWR | O_CREAT, 0666)) < 0 ||
			(fdout = open(stdOut, O_RDWR | O_CREAT, 0666)) < 0 ||
			(hstdin = dup(0)) == -1 ||
			(hstdout = dup(1)) == -1 ||
			dup2(fdin, 0) == -1 ||
			dup2(fdout, 1) == -1) {
		goto DONE;
	}

	rc = pid = fork();
	if (pid == 0) {
		/*
		 *		if pid == 0, then we are in the child process
		 */
		if (execve(cgiPath, argp, envp) == -1) {
			printf("content-type: text/html\n\n"
					"Execution of cgi process failed\n");
		}
		exit (0);
	} 

DONE:
	if (hstdout >= 0) {
		dup2(hstdout, 1);
		close(hstdout);
	}
	if (hstdin >= 0) {
		dup2(hstdin, 0);
		close(hstdin);
	}
	if (fdout >= 0) {
		close(fdout);
	}
	if (fdin >= 0) {
		close(fdin);
	}
	return rc;
}

/******************************************************************************/
/*
 *	Check the CGI process.  Return 0 if it does not exist; non 0 if it does.
 */

int websCheckCgiProc(int handle)
{
	/*
	 *	Check to see if the CGI child process has terminated or not yet.  
	 */
	if (waitpid(handle, NULL, WNOHANG) == handle) {
		return 0;
	} else {
		return 1;
	}
}

/******************************************************************************/

#ifdef B_STATS
static void memLeaks() 
{
	int		fd;

	if ((fd = gopen(T("leak.txt"), O_CREAT | O_TRUNC | O_WRONLY, 0666)) >= 0) {
		bstats(fd, printMemStats);
		close(fd);
	}
}

/******************************************************************************/
/*
 *	Print memory usage / leaks
 */

static void printMemStats(int handle, char_t *fmt, ...)
{
	va_list		args;
	char_t		buf[256];

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	write(handle, buf, strlen(buf));
}
#endif

/******************************************************************************/
