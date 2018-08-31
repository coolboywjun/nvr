/***************************************************************************
 *   Copyright (C) 2013 by Yexr   *
 *   yexr@vveye.com   *
 ***************************************************************************/
#ifndef _LIB_T2U_SVR_H_
#define _LIB_T2U_SVR_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
初始化，开启P2P服务
svraddr		服务器地址或域名
svrport		服务器端口
svrkey		服务器密钥
uuid			本机UUID
threadnum	线程数

返回值:
0			成功
-1			失败
*/
int	t2u_svr_init(const char* svraddr,unsigned short svrport,const char* svrkey,const char* uuid,int threadnum);


/*
设置密码
(如果设置了密码，客户端必须凭正确的密码才能建立P2P通道)

passwd		密码字符串

返回值:
0			成功
-1			失败
*/
int	t2u_svr_set_passwd(const char* passwd);


/*
设置自定义附加参数

param		自定义参数，如设备型号，通道信息，UPNP端口状态等
len			参数长度，最大不超过1024字节

返回值:
0:			成功
-1:			失败

*/
int	t2u_svr_set_param(const char* param,int len);

/*
关闭P2P服务
*/
void	t2u_svr_exit();

/*
查询状态

返回值:
1:		已注册到服务器，状态正常
0:		尚未注册到服务器
-1:		SDK未初始化
-2:		服务器密钥无效
-3:		此UUID不允许注册
*/
int	t2u_svr_status();

/*
查询P2P连接

outbuff			接收结果的缓存地址
				(输出内容为字符串，每行一个连接，格式为: ip:port|丢包率|估算上行带宽)
buffsize			接收结果的缓存大小


返回值:

int	当前P2P连接数

*/
int	t2u_svr_conn_stats(char* outbuff,int buffsize);

#ifdef __cplusplus
}
#endif

#endif


