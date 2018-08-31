#include <sys/prctl.h>
#include "net_api.h"
#include "nvr_net_config.h"
#include "stdsoap2.h"
#include "soapH.h"
#include "hi_onvif.h"
#include "hi_platform_onvif.h"
#include "hi_onvif_server.h"
#include "hi_onvif_proc.h"
#include "soapStub.h"
#include "nvr_headers.h"
#include "nvr_lib_interface.h"

#include   <net/if_arp.h>
#include   <arpa/inet.h>
#include   <errno.h>
#include   <net/if.h>
#include  <linux/sockios.h>
#include  <linux/ethtool.h>




#define NUM_BROADCAST (4)        //number of times hello is broadcasted
#define NUM_TIMEOUT (5)
typedef struct _HI_ONVIF_MNG_S_
{
	int quit;

	pthread_t helloThread;
	pthread_t probeThread;
	pthread_t serverThread;
	pthread_t helloWifiThread;
	pthread_t probeWifiThread;

	pthread_mutex_t mutex;
}HI_ONVIF_MNG_S;

static HI_ONVIF_MNG_S gOnvifMng;;


static int hi_onvif_get_netstatus()
{
	#define ETHTOOL_GLINK       0x0000000a /* Get link status (ethtool_value) */
	struct ethtool_value 
	{
		unsigned int   cmd;
		unsigned int   data; 
	};	

	int skfd;
	struct ifreq ifr;
	struct ethtool_value edata;
	edata.cmd = ETHTOOL_GLINK;
	edata.data = 0;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, "eth0", sizeof(ifr.ifr_name) - 1);
	ifr.ifr_data = (char *)&edata;
	if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
	   return -1;
	if (ioctl(skfd, SIOCETHTOOL, &ifr) == -1)
	{
	   close(skfd);
	   return -1;
	}
	close(skfd);
	return edata.data;

}



static void *hi_onvif_hello_check(void *data)
{	
	int StatusFlag = 0;
	int ChangFlag = 0;

	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "ONVIFHello");
	ChangFlag = hi_onvif_get_netstatus();

	while(!gOnvifMng.quit)
	{
		StatusFlag = hi_onvif_get_netstatus();
			
		if(ChangFlag != StatusFlag)
		{
			ChangFlag = StatusFlag;
			if(StatusFlag)
			{
				hi_onvif_hello(HI_CARD_ETH0);
			}
		}
		usleep(500000);
	}
	
	return NULL;
}

#if 0 
static void *hi_onvif_wifi_hello_check(void *data)
{	
	int StatusFlag = 0;
	int ChangFlag = 0;

	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "ONVIFWIFIHello");

	ChangFlag = hi_onvif_get_netstatus();

	while(!gOnvifMng.quit)
	{
		StatusFlag = hi_wifi_get_iw_netstat();
			
		if(ChangFlag != StatusFlag)
		{
			ChangFlag = StatusFlag;
			if(StatusFlag)
			{
				if(hi_wifi_get_flag() == 2)
					hi_onvif_hello(HI_CARD_RA0);
			}
		}
		usleep(500000);
	}
	
	return NULL;
}
#endif

static unsigned long hi_get_ip_by_interface(char *pInterfaceName)
{
	int sockfd;
	struct sockaddr_in stAddr;
	struct ifreq ifr;
	if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return -1;
	}
	strncpy(ifr.ifr_name, pInterfaceName, IF_NAMESIZE);
	ifr.ifr_name[IF_NAMESIZE - 1] = '\0';
	if(ioctl(sockfd, SIOCGIFADDR, &ifr) < 0)
	{
		return -1;
	}
	close(sockfd);
	memcpy(&stAddr, &ifr.ifr_addr, sizeof(stAddr));
	return stAddr.sin_addr.s_addr;
}

static void *hi_onvif_probe(void *data)
{
	int nSocket = 0;
//	int quit = 0;
	struct soap *soap_udp;
	struct sockaddr_in servaddr;
	struct ip_mreq mip;

	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "ONVIFProbe");	
	soap_udp=soap_new();
	soap_init1(soap_udp, SOAP_IO_UDP);

	if (!soap_valid_socket((nSocket = soap_bind(soap_udp, NULL, DEFAULT_ONVIF_WSD_PORT, 5000))))
	{	 
		soap_print_fault(soap_udp, stderr);
		__E("soap_bind err\r\n");	
		soap_destroy (soap_udp);	
		soap_end(soap_udp);
		soap_free(soap_udp);
		return NULL;
	}
	
	 /* Enable Multicast Reception */
    bzero (&servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton (AF_INET, ONVIF_WSD_MC_ADDR, &servaddr.sin_addr);
    servaddr.sin_port = htons (DEFAULT_ONVIF_WSD_PORT);
    mip.imr_multiaddr = servaddr.sin_addr;
    mip.imr_interface.s_addr = hi_get_ip_by_interface("eth0");
    setsockopt (nSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mip, sizeof(mip));
	net_sock_bind_interface(nSocket, "eth0");
	net_set_sock_multicast_noloop(nSocket);
	while(!gOnvifMng.quit)
	{	
		if(soap_accept(soap_udp) < 0)
		{
			soap_print_fault(soap_udp, stderr);
			__E("soap_accept err\r\n");	
			soap_destroy (soap_udp);
			soap_end(soap_udp);
			break;
		}
		stOnvifSocket.eth0Socket = soap_udp->master;
		soap_serve(soap_udp, NULL);
		soap_destroy (soap_udp);
		soap_end(soap_udp);
	}	
	
	soap_done(soap_udp);		
	soap_free(soap_udp);

    return NULL;
}

#if 0
static void *hi_onvif_wifi_probe(void *data)
{
	int nSocket = 0;
	struct soap *soap_udp;
	struct sockaddr_in servaddr;
	struct ip_mreq mip;
	//char szIp[128];
	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "ONVIFWIFIProbe");
	while(!gOnvifMng.quit)
	{
		if(hi_wifi_get_iw_netstat() != 1)
		{
		sleep(2);
		continue;
		}
		
		soap_udp=soap_new();
		soap_init1(soap_udp, SOAP_IO_UDP);
		if (!soap_valid_socket((nSocket = soap_bind(soap_udp, NULL, DEFAULT_ONVIF_WSD_PORT, 5000))))
		{	 
			soap_print_fault(soap_udp, stderr);
			__E("soap_bind err\r\n");	
			soap_destroy (soap_udp);	
			soap_end(soap_udp);
			soap_free(soap_udp);
			sleep(3);
			continue;
		}
	
		bzero (&servaddr, sizeof (servaddr));
		servaddr.sin_family = AF_INET;
		inet_pton (AF_INET, ONVIF_WSD_MC_ADDR, &servaddr.sin_addr);
		servaddr.sin_port = htons (DEFAULT_ONVIF_WSD_PORT);
		mip.imr_multiaddr = servaddr.sin_addr;
		mip.imr_interface.s_addr = hi_get_ip_by_interface("ra0");
		if( mip.imr_interface.s_addr < 0)
		{
			soap_destroy (soap_udp);	
			soap_end(soap_udp);
			soap_free(soap_udp);
			continue;
		}
		setsockopt (nSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mip, sizeof(mip));

		/* Enable Multicast Reception */
		hi_set_sock_multicast_noloop(nSocket);
		hi_sock_bind_interface(nSocket, "ra0");

		while(!gOnvifMng.quit)
		{	
			if(hi_wifi_get_iw_netstat() != 1)
			{
				soap_destroy (soap_udp);	
				soap_end(soap_udp);
				break;
			}
			if(soap_accept(soap_udp) < 0)
			{
				soap_print_fault(soap_udp, stderr);
				__E("soap_accept err\r\n");	
				soap_destroy (soap_udp);
				soap_end(soap_udp);
				break;
			}
			stOnvifSocket.wifiSocket = soap_udp->master;
			soap_serve(soap_udp, NULL);

			soap_destroy (soap_udp);
			soap_end(soap_udp);
		}	

		soap_done(soap_udp);		
		soap_free(soap_udp);
	}

    return NULL;
}
#endif

static void *hi_onvif_server(void *data)
{
	
	SOAP_SOCKET sock;
	struct soap *psoap;

	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "ONVIFServer");

	
	psoap = soap_new();
	soap_init (psoap);
	sock = soap_bind (psoap, NULL, /*DEFAULT_ONVIF_SERVER_PORT*/8000, 5000);

	if(!soap_valid_socket(sock))
	{
		__E(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>create onvif server sock err\r\n");
		soap_print_fault(psoap, stderr);
		soap_destroy (psoap);
		soap_end (psoap);
		soap_done(psoap);
		soap_free(psoap);
		return (void*)0;
	}

	/* Run Server Loop */
	while (!gOnvifMng.quit)
	{
		/* Wait for Request */
		sock = soap_accept (psoap);
		if (!soap_valid_socket(sock))
		{
			/* TODO: Log Error! */
			/* soap_print_fault (&soap, stderr); */
			printf("Failed to accept onvif server socket.\n");
			soap_print_fault(psoap, stderr);
			soap_destroy (psoap);
			soap_end (psoap);
			break;
		}

		/* Serve Request */
		soap_serve(psoap, NULL);

		soap_destroy (psoap);
		soap_end (psoap); /* clear soap */
	}

	/* End */
	soap_done(psoap);
	soap_free(psoap);
	printf("out onvif server\r\n");
	return (void *)0;
}

void hi_onvif_hello(int nType)
{
	int times;
	time_t time_n;
	struct tm *tm_t;
	struct soap *soaph;
	struct SOAP_ENV__Header header;
	struct wsa__EndpointReferenceType wsa__EndpointReference;
	char macaddr[MACH_ADDR_LENGTH];
	char msgid[LARGE_INFO_LENGTH];
	NVR_DEV_NET_CFG_S stNetCfg;
	nvr_logon_get_net_cfg(0, &stNetCfg);
	soaph = soap_new();		/*creating  soap object*/	 
	if(!soaph)
	{
		__E("Faild to creat hello soaph.\r\n");
		return;
	}
	
	soap_init1(soaph,SOAP_IO_UDP | SO_BROADCAST);						 /*initializing soap packet*/
	soap_default_wsa__EndpointReferenceType(soaph, &wsa__EndpointReference);
	soap_default_SOAP_ENV__Header(soaph, &header); 					 /*setting default values for header*/
	soaph->header = &header;

	if(nType)
		memcpy(macaddr, stNetCfg.struEtherCfg[1].u8MacAddr, 6);
	else
		memcpy(macaddr, stNetCfg.struEtherCfg[0].u8MacAddr, 6);
	time_n = time(NULL);
	tm_t=localtime(&time_n);
	sprintf(msgid,"uuid:1319d68a-%d%d%d-%d%d-%d%d-%02X%02X%02X%02X%02X%02X",
			tm_t->tm_wday,tm_t->tm_mday,tm_t->tm_mon,
			tm_t->tm_year,tm_t->tm_hour,tm_t->tm_min,
			tm_t->tm_sec,macaddr[0], macaddr[1], macaddr[2], 
			macaddr[3], macaddr[4], macaddr[5]);
	header.wsa__MessageID =(char *)soap_malloc(soaph, sizeof(char) * INFO_LENGTH);
	if(header.wsa__MessageID == NULL)
	{
		__E("Failed to malloc for message id.\n");
		return;
	}
	strncpy(header.wsa__MessageID, msgid,INFO_LENGTH - 1);
	header.wsa__MessageID[INFO_LENGTH - 1] = 0;
	header.wsa__To = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
	header.wsa__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Hello";
	
	for(times = 0; times < NUM_BROADCAST; times++)			
	{
	
		if (soap_send___dnrd__sendHello(soaph, NULL, NULL, NULL))
		{
			soap_print_fault(soaph, stderr);
		}
		usleep(50000);
	}
	soap_destroy (soaph);
	soap_end(soaph);
	soap_done(soaph);
	soap_free(soaph);	
}

void hi_onvif_bye(int nType)
{
	char msgid[LARGE_INFO_LENGTH];
	char macaddr[MACH_ADDR_LENGTH];
	time_t curTime;
	struct tm *ptm;
	struct tm tmTime;
	struct soap *pSoap;
	struct SOAP_ENV__Header header;
	struct wsa__EndpointReferenceType wsa__EndpointReference;

	pSoap=soap_new();													 /* creating  soap object */   
	soap_init1(pSoap,SOAP_IO_UDP | SO_BROADCAST);						 /* initializing soap packet */
	soap_set_omode(pSoap, SOAP_ENC_ZLIB);
	soap_default_wsa__EndpointReferenceType(pSoap, &wsa__EndpointReference);
	soap_default_SOAP_ENV__Header(pSoap, &header);						/* setting default values for header */
	pSoap->header = &header;

	unsigned long ip;
	ip = onvif_get_ipaddr(pSoap);
	onvif_get_mac(ip, macaddr);
	curTime = time(NULL);
	ptm = localtime_r(&curTime, &tmTime);
	sprintf(msgid,"uuid:1319d68a-%d%d%d-%d%d-%d%d-%02X%02X%02X%02X%02X%02X",
			ptm->tm_wday,ptm->tm_mday,ptm->tm_mon,
			ptm->tm_year,ptm->tm_hour,ptm->tm_min,
			ptm->tm_sec,macaddr[0], macaddr[1], macaddr[2], 
			macaddr[3], macaddr[4], macaddr[5]);
	
	header.wsa__MessageID =(char *)soap_malloc(pSoap, sizeof(char) * INFO_LENGTH);
	if(header.wsa__MessageID == NULL)
	{
		__E("Failed to malloc for message id.\n");
		return;
	}
	strcpy(header.wsa__MessageID, msgid);
	header.wsa__To = "urn:schemas-xmlsoap-org:ws:2005:04:discovery";
	header.wsa__Action = "http://schemas.xmlsoap.org/ws/2005/04/discovery/Bye";
#if 1
	if (soap_send___dnrd__sendBye(pSoap,NULL,NULL,NULL))
	{
		soap_print_fault(pSoap, stderr);
	}
#endif	
	soap_destroy (pSoap);
	soap_end(pSoap);
	soap_done(pSoap);
	soap_free(pSoap);

}

int hi_onvif_init()
{	
	__D("onvif initialize...\n");
	memset(&gOnvifMng, 0, sizeof(gOnvifMng));
	pthread_mutex_init(&gOnvifMng.mutex, NULL);
	
	onvif_server_init(DEFAULT_ONVIF_SERVER_PORT);
	
	hi_onvif_hello(HI_CARD_ETH0); /* Send Hello */
	
	pthread_create(&gOnvifMng.helloThread, NULL, hi_onvif_hello_check, NULL);
	pthread_create(&gOnvifMng.probeThread, NULL, hi_onvif_probe, NULL);
	//pthread_create(&gOnvifMng.serverThread, NULL, hi_onvif_server, NULL);
	//pthread_create(&gOnvifMng.helloWifiThread, NULL, hi_onvif_wifi_hello_check, NULL);
	//pthread_create(&gOnvifMng.probeWifiThread, NULL, hi_onvif_wifi_probe, NULL);
	
	return NVR_SUCCESS;
}

int hi_onvif_uninit()
{
	gOnvifMng.quit = 1;
	//hi_onvif_bye(HI_CARD_ETH0);
	//if(hi_wifi_get_iw_netstat())
	//{
	//hi_onvif_bye(HI_CARD_RA0);		
	//}
	
	//pthread_kill(gOnvifMng.helloThread, SIGKILL);
	//pthread_kill(gOnvifMng.probeThread, SIGKILL);
	//pthread_kill(gOnvifMng.serverThread, SIGKILL);
	//pthread_kill(gOnvifMng.helloWifiThread, SIGKILL);
	//pthread_kill(gOnvifMng.probeWifiThread, SIGKILL);

	return NVR_SUCCESS;
}

