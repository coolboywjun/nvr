#include <nvr_server_headers.h>
#include <nvr_ddns_api.h>

#define DYNDNS_DEFAULT_SERVER "members.dyndns.org"
#define DYNDNS_DEFAULT_PORT   "80"
#define DYNDNS_REQUEST 		  "/nic/update"

#define DDNS3322_DEFAULT_SERVER "members.3322.org"
#define DDNS3322_DEFAULT_PORT   "80"
#define DDNS3322_REQUEST 		"/dyndns/update"

#define NVR_DDNS_MAX_CONNECT_TO  15000
#define NVR_DDNS_MAX_RESPONSE_TO 10000

struct service_t
{
  char szServerName[32];
  char szServerPort[8];
  char szServerRequest[32];  
};

struct service_t nvr_ddns_conf[] = 
{
	{
		DYNDNS_DEFAULT_SERVER,
		DYNDNS_DEFAULT_PORT,
		DYNDNS_REQUEST
	},
	{
		DDNS3322_DEFAULT_SERVER,
		DDNS3322_DEFAULT_PORT,
		DDNS3322_REQUEST
	},
};

int nvr_handle_dns_pack (char *server,
						char *request,
						char *szSndMsg, 
						int confIndex,
						const char *host,
						const char *userName,
						const char *passWord,
						const char *ip,
						int   ddnsFun)
{
	char szMsg[256]  = {0};
	char szUser[256] = {0};
	char *pAuth;
		
	memset(szMsg, 0, sizeof(szMsg));
	sprintf(szMsg, "GET %s?", request);
	strcat(szSndMsg, szMsg);

	memset(szMsg, 0, sizeof(szMsg));
	sprintf(szMsg, "hostname=%s&", host);
	strcat(szSndMsg, szMsg);

	if (ip != NULL)
	{
		memset(szMsg, 0, sizeof(szMsg));
		sprintf(szMsg, "myip=%s&", ip);
		strcat(szSndMsg, szMsg);
	}
	
	memset(szMsg, 0, sizeof(szMsg));
	sprintf(szMsg, "wildcard=%s&", (ddnsFun & NVR_DDNS_SUPPORT_WILDCARD)?"ON":"OFF");
	strcat(szSndMsg, szMsg);

	if (ddnsFun & NVR_DDNS_SUPPORT_OFFLINE)
	{
		memset(szMsg, 0, sizeof(szMsg));
		sprintf(szMsg, "offline=yes&");
		strcat(szSndMsg, szMsg);
	}
	strcat(szSndMsg, " HTTP/1.0\015\012");
	
 	sprintf(szUser, "%s:%s", userName, passWord);
	pAuth = g_base64_encode((guchar *)szUser, strlen(szUser));
	
	memset(szMsg, 0, sizeof(szMsg));
	sprintf(szMsg, "Authorization: Basic %s\015\012", pAuth);
	strcat(szSndMsg, szMsg);
	g_free(pAuth);
	
	memset(szMsg, 0, sizeof(szMsg));
	sprintf(szMsg, "User-Agent: %s %s [%s]\015\012", 
      	"ddns-update", "last modify 2010-09-18",  "by Alex");
	strcat(szSndMsg, szMsg);

	memset(szMsg, 0, sizeof(szMsg));
	sprintf(szMsg, "Host: %s\015\012", server);
	strcat(szSndMsg, szMsg);
	
  	strcat(szSndMsg, "\015\012");
	return strlen(szSndMsg);
}

int nvr_parse_ddns_response(char *buf)
{
	int ret = 0;
	int retval = 0;
	
	if(sscanf(buf, " HTTP/1.%*c %3d", &ret) != 1)
	{
		ret = -1;
	}

	switch(ret)
	{
	case -1:
	  retval = NVR_UPDATERES_SHUTDOWN;
	  break;

	case 200:
	  if(strstr(buf, "\ngood ") != NULL)
	  {
		retval = NVR_UPDATERES_OK;//success
	  }
	  else
	  {
	    if(strstr(buf, "\nnohost") != NULL)
	    {
	      retval = NVR_UPDATERES_SHUTDOWN; //invalid hostname
	    }
	    else if(strstr(buf, "\nnotfqdn") != NULL)
	    {
	      retval = NVR_UPDATERES_NOT_FQND; //malformed hostname
	    }
	    else if(strstr(buf, "\n!yours") != NULL)
	    {
	      retval = NVR_UPDATERES_NOT_YOURS; //host is not under your control
	    }
	    else if(strstr(buf, "\nabuse") != NULL)
	    {
	      retval = NVR_UPDATERES_ABUSE; //host has been blocked for abuse
	    }
	    else if(strstr(buf, "\nnochg") != NULL)
	    {
	      // lets say that this counts as a successful update
	      // but we'll roll back the last update time to max_interval/2
	      retval = NVR_UPDATERES_OK;
	    }
	    else if(strstr(buf, "\nbadauth") != NULL)
	    {
	      retval = NVR_UPDATERES_AUTHENTICATION_FAILURE;//authentication failure
	    }
	    else if(strstr(buf, "\nbadsys") != NULL)
	    {
	      retval = NVR_UPDATERES_INVALID_PARAMETER; //invalid system parameter
	    }
	    else if(strstr(buf, "\nbadagent") != NULL)
	    {
	      retval = NVR_UPDATERES_BAD_AGENT; //this useragent has been blocked
	    }
	    else if(strstr(buf, "\nnumhost") != NULL)
	    {
	      retval = NVR_UPDATERES_ERR_NUMBER_HOST; //Too many or too few hosts found
	    }
	    else if(strstr(buf, "\ndnserr") != NULL)
	    {
	      retval = NVR_UPDATERES_DNS_ERR; //dyndns internal error
	    }
	    else if(strstr(buf, "\n911") != NULL)
	    {
	      retval = NVR_UPDATERES_SHUTDOWN;
	    }
	    else if(strstr(buf, "\n999") != NULL)
	    {
	      retval = NVR_UPDATERES_SHUTDOWN;
	    }
	    else if(strstr(buf, "\n!donator") != NULL)
	    {
	      retval = NVR_UPDATERES_DONATOR;
	    }
	    // this one should be last as it is a stupid string to signify waits
	    // with as it is so short
	    else if(strstr(buf, "\nw") != NULL)
	    {
	      retval = NVR_UPDATERES_SHUTDOWN;
	    }
	    else
	    {
	      retval = NVR_UPDATERES_SHUTDOWN;
	    }
	  }
	  break;

	case 401:
	  retval = NVR_UPDATERES_SHUTDOWN;
	  break;

	default:
	  retval = NVR_UPDATERES_SHUTDOWN;
	  break;
	}

	return(retval);
}

int nvr_ddns_update_entry(const char *server,
						 const char *host,
						 const char *userName,
						 const char *passWord,
						 const char *ip,
						 unsigned long u32Port,
						 const char *request,
						 int   ddnsFun)
{
	int confIndex = -1;
	char szSndMsg[2048] = {0};
	int fd = 0;
	int sndLen = 0;
	char szPort[8] = {0};

	sprintf(szPort, "%lu", u32Port);
	
	if (server == NULL || host == NULL || userName == NULL || passWord == NULL)
		return NVR_UPDATERES_PARAM_ERR;

	if ((fd = net_tcp_noblock_connect(NULL, NULL, server, szPort,
						   NVR_DDNS_MAX_CONNECT_TO)) < 0)
	{
#ifdef DEBUG	
		printf("nvr_ddns_update_entry connect server %s %s failed %s\n",
  				server, szPort, strerror(errno));
#endif
		return NVR_UPDATERES_CONNECT_SVR_ERR;
	}

	sndLen = nvr_handle_dns_pack((char *)server, (char *)request, 
								szSndMsg, confIndex, host, userName,
								passWord, ip, ddnsFun);
#ifdef DEBUG
	printf("nvr_ddns_update_entry send msg:\n%s\n", szSndMsg);
#endif	
  	if (sndLen != net_tcp_noblock_send(fd, szSndMsg, sndLen, NULL, 5000))
  	{
#ifdef DEBUG	
		printf("nvr_ddns_update_entry send to server %s %s failed %s\n",
				nvr_ddns_conf[confIndex].szServerName,
				nvr_ddns_conf[confIndex].szServerPort,
				strerror(errno));
#endif
		return NVR_UPDATERES_NET_ERR;
	}

	if ((fd | 0x10000) == net_select(&fd, 0x1, 0x1, NVR_DDNS_MAX_RESPONSE_TO))
	{
		memset(szSndMsg, 0, sizeof(szSndMsg));
		if ((sndLen = net_tcp_noblock_recv(fd, szSndMsg, sizeof(szSndMsg), 
						0, NVR_DDNS_MAX_RESPONSE_TO)) <= 0)
		{
#ifdef DEBUG	
			printf("nvr_ddns_update_entry recv data failed %s\n",
				strerror(errno));
#endif
			return NVR_UPDATERES_NET_ERR;
		}
#ifdef DEBUG
	printf("nvr_ddns_update_entry recv response:\n%s\n", szSndMsg);
#endif		
		return nvr_parse_ddns_response(szSndMsg);
	}
	return NVR_UPDATERES_SVR_NO_RESPONSE;
}

int nvr_ddns_get_wan_ip(char *pWanIp, int nLen)
{
	int nSock;
	char szBuff[1024];
	char *pSendBuff = szBuff;
	char *pStr, *pStr1;
	
	nSock = net_tcp_noblock_connect(NULL, NULL, "ip.3322.net", "80",
						   NVR_DDNS_MAX_CONNECT_TO);
	if(nSock < 0)
		return NVR_UPDATERES_NET_ERR;
	pSendBuff += sprintf(pSendBuff, "GET / HTTP/1.0\r\n");
	pSendBuff += sprintf(pSendBuff, "Host: ip.3322.net\r\n");
	pSendBuff += sprintf(pSendBuff, "Connection: keep-alive\r\n");
	pSendBuff += sprintf(pSendBuff, "User-Agent: %s %s [%s]\r\n", 
      							"ddns-update", "last modify 2010-09-18",  "by Alex");
      	pSendBuff += sprintf(pSendBuff, "\r\n\r\n");
      	if(net_tcp_noblock_send(nSock, szBuff, strlen(szBuff), NULL, 5000) != strlen(szBuff))
      	{
		net_close_socket(&nSock);
		return -1;
      	}
      	if ((nSock | 0x10000) != net_select(&nSock, 0x1, 0x1, NVR_DDNS_MAX_RESPONSE_TO))
      	{
		net_close_socket(&nSock);
		return -1;
      	}
      	memset(szBuff, 0, sizeof(szBuff));
      	if(net_tcp_noblock_recv(nSock, szBuff, sizeof(szBuff), 0, NVR_DDNS_MAX_RESPONSE_TO) < 0)
      	{
		net_close_socket(&nSock);
		return -1;
      	}
	net_close_socket(&nSock);
	do
	{
		pStr = strstr(szBuff, "\r\n\r\n");
		if(pStr == NULL)
			break;
		pStr += 4;
		pStr1 = strchr(pStr, '\r');
		if(pStr1 != NULL)
			*pStr1 = '\0';
		pStr1 = strchr(pStr, '\n');
		if(pStr1 != NULL)
			*pStr1 = '\0';
		g_strlcpy(pWanIp, pStr, nLen);
		return 0;
	}while(0);
	
	return -1;
}

