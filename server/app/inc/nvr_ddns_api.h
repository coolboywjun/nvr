#ifndef __NVR_DDNS_API__
#define __NVR_DDNS_API__

/*
* Copyright (c) 2010
* All rights reserved.
* 
* filename:nvr__ddns_api.h
* 
* func:本文件实现了对ddns更新的封装
* 
* author : nvr_
* createdate: 2010-10-10
*
* modify list: 
* 
*/
#ifdef __cplusplus
extern "C" {
#endif 

#define DYNDNS_DEFAULT_SERVER "members.dyndns.org"
#define DYNDNS_DEFAULT_PORT   "80"
#define DYNDNS_REQUEST 		  "/nic/update"

#define DDNS3322_DEFAULT_SERVER "members.3322.org"
#define DDNS3322_DEFAULT_PORT   "80"
#define DDNS3322_REQUEST 		"/dyndns/update"

enum NVR_DDNS_FUNC
{
	NVR_DDNS_SUPPORT_NO,
	NVR_DDNS_SUPPORT_WILDCARD,
	NVR_DDNS_SUPPORT_MX,
	NVR_DDNS_SUPPORT_OFFLINE
};

enum NVR_DDNS_ERR_CODE
{
  NVR_UPDATERES_OK = 0,
  NVR_UPDATERES_PARAM_ERR,
  NVR_UPDATERES_CONNECT_SVR_ERR,
  NVR_UPDATERES_NET_ERR,
  NVR_UPDATERES_SVR_NO_RESPONSE,
  NVR_UPDATERES_INVALID_HOSTNAME,
  NVR_UPDATERES_NOT_FQND,
  NVR_UPDATERES_NOT_YOURS,
  NVR_UPDATERES_ABUSE,
  NVR_UPDATERES_AUTHENTICATION_FAILURE,
  NVR_UPDATERES_INVALID_PARAMETER,
  NVR_UPDATERES_BAD_AGENT,
  NVR_UPDATERES_ERR_NUMBER_HOST,
  NVR_UPDATERES_DNS_ERR,
  NVR_UPDATERES_DONATOR,
  NVR_UPDATERES_SHUTDOWN
};
//=================================================================
//函数:nvr__ddns_update_entry 
//
//功能:动态域名更新
//
//参数:server(输入)  :   ddns服务器domain或者ip
//	   host(输入)    :   需要更新的ddns域名	
//     username(输入):   ddns帐号的用户名
//     password(输入):   ddns帐号的密码
//	   ip(输入)      :   主机ip(dyndns 可以不填,3322 填eth0地址就可以)
//	   u32Port(输入) :   ddns服务器端口
//	   request       :   ddns请求域(值请参考本文件开始的宏定义)	
//	   ddnsFun       :   ddns附加功能(参考NVR_DDNS_FUNC)	
//
//返回:成功返回UPDATERES_OK,失败返回NVR_DDNS_ERR_CODE中的错误码
//
//=================================================================

int nvr_ddns_update_entry(const char *server,
						 const char *host,
						 const char *userName,
						 const char *passWord,
						 const char *ip,
						 unsigned long u32Port,
						 const char *request,
						 int   ddnsFun);
//sample 
/*************************************************
int main(int argv, char **argc)
{
	int ret = nvr__ddns_update_entry(DYNDNS_DEFAULT_SERVER, 
						"xx.dyndns.org",
						"xx",
						"19840403",
						NULL,
						80,
						NVR_DDNS_SUPPORT_NO);

	printf("xlddns return %d\n", ret);

	ret = nvr__ddns_update_entry(DDNS3322_DEFAULT_SERVER, 
						"xx.3322.org",
						"xx",
						"865557",
						"192.168.1.78",
						80,
						NVR_DDNS_SUPPORT_WILDCARD|
						NVR_DDNS_SUPPORT_MX);

	return 0;
}
********************************************************/

int nvr_ddns_get_wan_ip(char *pWanIp, int nLen);

#ifdef __cplusplus
}
#endif 

#endif


