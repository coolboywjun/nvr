
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "miniwget.h"
#include "miniupnpc.h"
#include "upnpcommands.h"
#include "upnperrors.h"

#include "miniupnpc.h"
#include "upnp_api.h"


typedef struct _UPNP_HANDLE
{
	struct UPNPDev * devlist;
	struct UPNPUrls* urls;
	struct IGDdatas* data;
}UPNP_HANDLE;

typedef struct _UPNP_MAP_LIST
{
	unsigned int protocal; //协议类型 0 tcp 1 udp
	unsigned int interPort;//内网端口
	unsigned int extPort;  //外网端口
	unsigned int enabled;  //是否有效
	struct _UPNP_MAP_LIST *next;
}UPNP_MAP_LIST;


/* protofix() checks if protocol is "UDP" or "TCP" 
 * returns NULL if not */
const char * protofix(const char * proto)
{
	static const char proto_tcp[4] = { 'T', 'C', 'P', 0};
	static const char proto_udp[4] = { 'U', 'D', 'P', 0};
	int i, b;
	for(i=0, b=1; i<4; i++)
		b = b && (   (proto[i] == proto_tcp[i]) 
		          || (proto[i] == (proto_tcp[i] | 32)) );
	if(b)
		return proto_tcp;
	for(i=0, b=1; i<4; i++)
		b = b && (   (proto[i] == proto_udp[i])
		          || (proto[i] == (proto_udp[i] | 32)) );
	if(b)
		return proto_udp;
	return 0;
}
#if 0
void freeUPNPDevlist(struct UPNPDev * devlist)
{
	struct UPNPDev * next;
	while(devlist)
	{
		next = devlist->pNext;
		free(devlist);
		devlist = next;
	}
}

void FreeUPNPUrls(struct UPNPUrls * urls)
{
	if(!urls)
		return;
	
	if (urls->controlURL)
		free(urls->controlURL);
	urls->controlURL = 0;

	if (urls->ipcondescURL)
		free(urls->ipcondescURL);
	urls->ipcondescURL = 0;

	if (urls->controlURL_CIF)
		free(urls->controlURL_CIF);
	urls->controlURL_CIF = 0;
	
	free(urls);
	urls = 0;
}
#endif
static void free_redirection_list(UPNP_MAP_LIST *mapList)
{
	UPNP_MAP_LIST* next;
	
	while(mapList)
	{
		next = mapList->next;
		free(mapList);
		mapList = next;
	}
}

static int list_all_redirection(unsigned long hUpnpHandle, 
							char           *localHost, 
							UPNP_MAP_LIST **ppList)
{	
	int r;	
	int i = 0;	
	int num=0;
	char index[6];	
	char intClient[16];	
	char intPort[6];	
	char extPort[6];	
	char protocol[4];	
	char desc[80];	
	char enabled[6];	
	char rHost[64];	
	char duration[16];	
	
	UPNP_HANDLE   *pUpnpHandle = (UPNP_HANDLE *)hUpnpHandle;
	struct UPNPUrls  * urls = pUpnpHandle->urls;
	struct IGDdatas  * data = pUpnpHandle->data;
	UPNP_MAP_LIST *pList = NULL;
	
	do {		
		snprintf(index, 6, "%d", i);		
		rHost[0] = '\0'; 
		enabled[0] = '\0';		
		duration[0] = '\0'; 
		desc[0] = '\0';		
		extPort[0] = '\0'; 
		intPort[0] = '\0'; 
		intClient[0] = '\0';		
		r = UPNP_GetGenericPortMappingEntry(urls->controlURL,
			data->first.servicetype,		                               
			index,		                               
			extPort, 
			intClient, 
			intPort,
			protocol, 
			desc, 
			enabled,
			rHost, 
			duration);
		++i;

		if(r==0)		
		{
			if (localHost != NULL && strstr(intClient, localHost) == NULL)
				continue; 

			if ((pList = malloc(sizeof(UPNP_MAP_LIST))) != NULL)
			{
				memset(pList, 0, sizeof(UPNP_MAP_LIST));
				
				pList->enabled   = strtol(enabled, NULL, 0xa);
				pList->interPort = strtol(intPort, NULL, 0xa);
				pList->extPort   = strtol(extPort, NULL, 0xa);
				pList->protocal  = (strstr(protocol, "udp") ||
									strstr(protocol, "UDP")) ? 1 : 0;
				if (*ppList == NULL)
					*ppList = pList;
				else
					(*ppList)->next = pList;
			}
			++num;
		 	//printf("list %2d %s %5s->%s:%-5s '%s' '%s' '%s' %s\n",	
		 	//	i, protocol, extPort, intClient, intPort, desc, rHost, enabled,
		 	//	duration);
		}	
		//else			
			//printf("GetGenericPortMappingEntry() returned %d (%s)\n",
			//	r, strupnperror(r));		
	} while(r==0);
	return num;
}

/* Test function 
 * 1 - get connection type
 * 2 - get extenal ip address
 * 3 - Add port mapping
 * 4 - get this port mapping from the IGD */
static int proce_upnp_map(struct UPNPUrls * urls,
                      struct IGDdatas * data,
					  const char * iaddr,
					  const char * iport,
					  const char * eport,
                      const char * proto,
                      const char * desc,
                      char *szExternalIp)
{
	char intClient[16];
	char intPort[6];
	int r;

	if(!iaddr || !iport || !eport || !proto)
	{
		fprintf(stderr, "Wrong arguments\n");
		return -1;
	}

	if(NULL == (proto = protofix(proto)))
	{
		fprintf(stderr, "invalid protocol\n");
		return -1;
	}

	char externalIPAddress[16];

	UPNP_GetExternalIPAddress(urls->controlURL,
	                          				data->first.servicetype,
							       externalIPAddress);
	if(externalIPAddress[0])
	{
		//printf("ExternalIPAddress = %s\n", externalIPAddress);
		if(szExternalIp)
			strcpy(szExternalIp, externalIPAddress);
	}
	else
	{
		//printf("GetExternalIPAddress failed.\n");
		if(szExternalIp)
			szExternalIp[0] = 0;
		return -1;
	}

	r = UPNP_GetSpecificPortMappingEntry(urls->controlURL,
	                                 data->first.servicetype,
    	                             eport, proto,  intClient,  intPort);
	if(r == UPNPCOMMAND_SUCCESS)
	{
		if (strcmp(intClient, iaddr) != 0 || intPort != iport)
		{
#ifdef DEBUG	
			printf("port have been map %s\n", intClient);
#endif
			return -1;
		}
		else
		{
#ifdef DEBUG		
			printf("port have been map %s\n", intClient);
#endif
			return 0;
		}
		return -1;
	}

	r = UPNP_AddPortMapping(urls->controlURL, data->first.servicetype,
	                        eport, iport, iaddr, desc, proto, 0);

	if(r!=UPNPCOMMAND_SUCCESS)
	{
#ifdef DEBUG
		printf("AddPortMapping(%s, %s, %s) failed with code %d (%s)\n",
		       eport, iport, iaddr, r, strupnperror(r));
#endif
		return -1;
	}
#if 1	
	r = UPNP_GetSpecificPortMappingEntry(urls->controlURL,
	                                 data->first.servicetype,
    	                             eport, proto,  intClient,  intPort);
	if(r != UPNPCOMMAND_SUCCESS)
	{
#ifdef DEBUG	
		printf("GetSpecificPortMappingEntry() failed with code %d (%s)\n",
		       r, strupnperror(r));
#endif
		return -1;
	}

	if(intClient[0]) {
		//printf("InternalIP:Port = %s:%s\n", intClient, intPort);
		printf("external %s:%s %s is redirected to internal %s:%s\n",
		       externalIPAddress, eport, proto, intClient, intPort);
	}
	else
	{
		printf("Get internal ip and port failed.\n");
		return -1;
	}
#endif
	return 0;
}

static int proce_del_map_port(struct UPNPUrls * urls,  struct IGDdatas * data, const char * eport,   const char * proto)
{
	int r;
	if(!proto || !eport)
	{
#ifdef DEBUG	
		fprintf(stderr, "invalid arguments\n");
#endif
		return -1;
	}
	
	if(NULL == (proto = protofix(proto)))
	{
#ifdef DEBUG	
		fprintf(stderr, "protocol invalid\n");
#endif
		return -1;
	}
	r = UPNP_DeletePortMapping(urls->controlURL, data->first.servicetype, eport, proto, 0);
#ifdef DEBUG	
	printf("UPNP_DeletePortMapping() returned : %d\n", r);
#endif
	return r;
}

int upnp_check_port_mapped(unsigned long hUpnpHandle, int externalPort, int protocal)
{
	UPNP_HANDLE *pUpnpHandle = (UPNP_HANDLE *)hUpnpHandle;
	int r;
	char szExternalPort[16];
	char intClient[16];
	char intPort[16];
	char szProto[2][4] = {{"TCP"}, {"UDP"}};
	
	if(pUpnpHandle == NULL)
		return 0;
	if(externalPort == 0)
		return 0;
	if(protocal  != 0 && protocal != 1)
		return 0;
	sprintf(szExternalPort, "%d", externalPort);
	r = UPNP_GetSpecificPortMappingEntry(pUpnpHandle->urls->controlURL,
	                                 pUpnpHandle->data->first.servicetype,
    	                             	szExternalPort, szProto[protocal], intClient,  intPort);
	if(r != UPNPCOMMAND_SUCCESS)
		return 0;
	return 1;
}

int upnp_add_map_port(unsigned long hUpnpHandle,  
					   char * localip, 
					   int    internalPort, 
					   int    externalPort, 
					   int    protocal, 
					   int    autoMap , 
					   char  *szDesc,
					   char  *szExternalIp)
{
	//check the parameter
	UPNP_HANDLE *pUpnpHandle = (UPNP_HANDLE *)hUpnpHandle;
	
	int       max_try_times = 0;
	int      try_times = 0;
	char    szProto[2][4] = {{"TCP"}, {"UDP"}};
	char    szInterPort[8]  = {0};
	char    szExternalPort[8]= {0};
		
	if ((protocal  != 0 && protocal != 1) ||
	    (autoMap != 0 && autoMap != 1))
	{
#ifdef DEBUG	
		perror("upnp_add_map_port parmaeter error!\n");
#endif
		return -1;

	}
	max_try_times = autoMap ? 100 : 1;
	
	for (try_times = 0;  try_times < max_try_times; ++try_times)
	{
		memset(szInterPort,      0, sizeof(szInterPort));
		memset(szExternalPort, 0, sizeof(szExternalPort));

		sprintf(szInterPort,       "%d",  internalPort);
		sprintf(szExternalPort,  "%d",  externalPort);
		
		if (proce_upnp_map(pUpnpHandle->urls, pUpnpHandle->data, localip, 
			szInterPort, szExternalPort, szProto[protocal], szDesc, szExternalIp) == 0)
		{
			return externalPort;
		}
		++externalPort;
	}		

#ifdef DEBUG	
	printf("upnp_add_map_port map failed!\n");
#endif	

	return -1;
	
}

int upnp_del_map_port(unsigned long hUpnpHandle, 
					     int    externalPort, 
					     int    protocal)
{
	char    szProto[2][4] = {{"TCP"}, {"UDP"}};
	char    szExternalPort[8]= {0};
	UPNP_HANDLE *pUpnpHandle = (UPNP_HANDLE *)hUpnpHandle;
	
	sprintf(szExternalPort,  "%d",  externalPort);	
	if (proce_del_map_port(pUpnpHandle->urls, pUpnpHandle->data,  
				szExternalPort, szProto[protocal])  == 0)
	{
		return 0;
	}

#ifdef DEBUG	
		printf("upnp_del_map_port failed!\n");
#endif	

	return -1;
	
}                       

unsigned long upnp_map_init(char * hostip)
{
	UPNP_HANDLE *pUpnpHandle = NULL;
 	char lanaddr[64];	/* my ip address on the LAN */

	if (hostip == NULL)//check the parameter
	{
#ifdef DEBUG	
		perror("upnp_map_init parmaeter error!\n");
#endif
		return 0;
	}

	if ((pUpnpHandle = malloc(sizeof(UPNP_HANDLE))) == NULL)
	{
		printf("upnp_map_init malloc Handle error\n");
		return 0;
	}
	memset(pUpnpHandle, 0, sizeof(UPNP_HANDLE));
	
	if ((pUpnpHandle->urls = malloc(sizeof(struct UPNPUrls))) == NULL)
	{
		printf("upnp_map_init malloc urls error\n");
		free(pUpnpHandle);
		return 0;
	}

	if ((pUpnpHandle->data = malloc(sizeof(struct IGDdatas))) == NULL)
	{
		printf("upnp_map_init malloc urls error\n");
		free(pUpnpHandle->urls);
		free(pUpnpHandle);
		return 0;
	}	
	memset(pUpnpHandle->urls, 0, sizeof(struct UPNPUrls));
	memset(pUpnpHandle->data, 0, sizeof(struct IGDdatas));

	if((pUpnpHandle->devlist = upnpDiscover(2000, NULL, NULL , 0, hostip)) != NULL)
	{
#if 0
		struct UPNPDev * device;

		if(pUpnpHandle->devlist)
		{
			printf("List of UPNP devices found on the network :\n");
			for(device = pUpnpHandle->devlist; device; device = device->pNext)
			{
				printf(" desc: %s\n st: %s\n\n",  device->descURL, device->st);
			}
		}
#endif 

		if (UPNP_GetValidIGD(pUpnpHandle->devlist, pUpnpHandle->urls,
			pUpnpHandle->data, lanaddr, sizeof(lanaddr)) == 0)
		{
#ifdef DEBUG	
			perror("upnp_map_init get valid igd error!\n");
#endif			
			freeUPNPDevlist(pUpnpHandle->devlist);
			free(pUpnpHandle->urls);
			free(pUpnpHandle->data);
			free(pUpnpHandle);
			return 0;
		}
		
		return (unsigned long)(pUpnpHandle);
	}

#ifdef DEBUG	
	printf("upnp_map_init not found any upnp devices on the network!\n");
#endif	
	free(pUpnpHandle->urls);
	free(pUpnpHandle->data);
	free(pUpnpHandle);
	return 0;
}

int upnp_map_destroy(unsigned long hHandle)
{
	UPNP_HANDLE *pUpnpHandle = (UPNP_HANDLE *)hHandle;
	if (pUpnpHandle == NULL)
		return -1;
	
	freeUPNPDevlist(pUpnpHandle->devlist);
	FreeUPNPUrls(pUpnpHandle->urls);
	free(pUpnpHandle->data);
	free(pUpnpHandle);	
	return 0;
}


