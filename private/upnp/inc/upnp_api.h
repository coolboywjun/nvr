#ifndef HI_UPNP_API_H
#define  HI_UPNP_API_H
/*
* Copyright (c) 2010
* All rights reserved.
* 
* filename:upnp_api.h
* 
* func:本文件实现了对利用upnp进行端口映射的封装
* 
* author : hi
* createdate: 2010-10-10
*
* modify list: 
* 
*/
#ifdef __cplusplus
extern "C" {
#endif


//******************************************************************/
//function:do the upnp init(after call this function must call upnp_map_destroy)
//
//hostip: upnp server ip
//
//return:success return the handle, else return 0
//******************************************************************/
unsigned long upnp_map_init(char * hostip);

//******************************************************************/
//function:release the 
//
//hHandle: upnp handle
//
//return:success return the handle, else return -1
//******************************************************************/
int upnp_map_destroy(unsigned long hHandle);

//******************************************************************/
//function:check upnp is mapped
//
//hUpnpHandle: upnp handle
//
//externalPort: external port which want to be maped as
//
//protocal: 0 tcp 1 udp
//
//return : return 1 if mapped or 0
//		 
/********************************************************************/
int upnp_check_port_mapped(unsigned long hUpnpHandle, int externalPort, int protocal);

//******************************************************************/
//function:do the upnp map
//
//hUpnpHandle: upnp handle
//
//localip: the dev's ip
//
//internalPort: internal port which want to map to external
//
//externalPort: external port which want to be maped as
//
//protocal: 0 tcp 1 udp
//
//autoMap: 0 just try map the internalPort to externalPort
//              1 when map the internalPort to externalPort failed, will try map the internalPort
//		   to externalPort + 1.......externalPort+100 untill success or map externalPort+100 failed.
//
//szDes: the descript name show in the upnp server.
//
//return : return the realmap port if success -1 fail
//		 
/********************************************************************/
int upnp_add_map_port(unsigned long hUpnpHandle,  
					   char * localip, 
					   int    internalPort, 
					   int    externalPort, 
					   int    protocal, 
					   int    autoMap , 
					   char  *szDesc,
					   char  *szExternalIp);
//******************************************************************/
//function:delete the upnp map
//
//hUpnpHandle: upnp handle
//
//externalPort: external port which want to be delete
//
//protocal: 0 tcp 1 udp
//
//
//return : success return 0, else return -1
//		 
/********************************************************************/
int upnp_del_map_port(unsigned long hUpnpHandle,
						 int    externalPort, 
					     int    protocal);


#ifdef __cplusplus
}
#endif 

#endif
