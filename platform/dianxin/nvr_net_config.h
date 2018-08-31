#ifndef __NVR_NET_CONFIG_H__
#define __NVR_NET_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif
/*
* Copyright (c) 2010
* All rights reserved.
* 
* filename:nvr_net_config.h
* 
* func:本文件实现了linux下 对本地网卡的一些操作的封装
* 
* author : hi
* createdate: 2010-10-10
*
* modify list: 
* 
*/

//============================================================================
// 函数名称: nvr_netconfig_active()
//
// 功能描述: 加载或卸载指定名字的网络
//
// 参数:	 name    网卡名称(eth0 eth1 lo and so on)	
//           action  状态 ("up" 激活网卡 "down" 停止网卡)
//
//
//返回:成功返回0 失败返回 < 0
//============================================================================
int nvr_netcfg_active(char *name, char *action); 

//============================================================================
// 函数名称: nvr_netconfig_netlink_status()
//
// Notice: call this function need root privilege.
// 
// 功能描述: 获取有线网口的物理连接状态
//
// 参数:	 if_name    网卡名称(eth0 eth1 lo and so on)	
//
//
//返回:网卡连接着返回 1 网卡没有连接返回0  失败返回 < 0
//============================================================================
int nvr_netcfg_netlink_status(const char * const if_name);

//============================================================================
// 函数名称: nvr_netconfig_getipaddr()
//
// 功能描述: 得到指定名字的网络ip
//
// 参数:	 name    网卡名称(eth0 eth1 lo and so on)	
//           net_ip (输出) 网卡IP
//
//返回:获取成功返回0 失败返回-1
//============================================================================
int nvr_netcfg_get_ip_addr(char *name,char *net_ip);

//============================================================================
// 函数名称: nvr_netconfig_setipaddr()
//
// 功能描述: 设置指定名字网络的IP地址
//
// 参数:	 name    网卡名称(eth0 eth1 lo and so on)	
//           net_ip  网卡IP
//
//返回:获取成功返回0 失败返回-1
//============================================================================
int nvr_netcfg_set_ip_addr(char *name, char *net_ip);

//============================================================================
// 函数名称: nvr_netconfig_getmacaddr()
//
// 功能描述: 获得网卡物理地址
//
// 参数:	 name    网卡名称(eth0 eth1 lo and so on)	
//           net_mac (输出) 网卡mac地址
//
//返回:获取成功返回0 失败返回-1
//============================================================================
int nvr_netcfg_get_mac_addr(char *name, char *net_mac);

//============================================================================
// 函数名称: nvr_netconfig_setmacaddr()
//
// 功能描述: 设置指定名字网卡的物理地址
//
// 参数:	 name    网卡名称(eth0 eth1 lo and so on)	
//           addr    网卡mac地址
//
//返回:获取成功返回0 失败返回-1
//============================================================================
int nvr_netcfg_set_mac_addr(char *name,char *addr);

//============================================================================
// 函数名称: nvr_netconfig_getgwaddr()
//
// 功能描述: 获取指定名字网卡的网关地址
//
// 参数:	 name            		网卡名称(eth0 eth1 lo and so on)	
//           gateway_addr (输出)    网卡网关地址
//			 nMetric				网卡metric值
//返回:获取成功返回0 失败返回 < 0
//============================================================================
int nvr_netcfg_get_gw_addr(char *name,char *gateway_addr, int *pMetric);

//============================================================================
// 函数名称: nvr_netconfig_setgwaddr()
//
// 功能描述: 设置指定名字网卡的网关地址
//
// 参数:	 name            网卡名称(eth0 eth1 lo and so on)	
//           gateway_addr    网卡网关地址
//			 nMetric		 网卡metric值
//返回:获取成功返回0 失败返回-1
//============================================================================
int nvr_netcfg_set_gw_addr(char *name,char *gateway_addr, int nMetric);

//============================================================================
// 函数名称: nvr_netconfig_getmaskaddr()
//
// 功能描述: 获取指定名字网卡的子网掩码地址
//
// 参数:	 name            网卡名称(eth0 eth1 lo and so on)	
//           net_mask  (输出)  子网掩码地址
//
//返回:获取成功返回0 失败返回 < 0
//============================================================================
int nvr_netcfg_get_mask_addr(char *name,char *net_mask);

//============================================================================
// 函数名称: nvr_netconfig_setmaskaddr()
//
// 功能描述: 设置指定名字网卡的子网掩码
//
// 参数:	 name            网卡名称(eth0 eth1 lo and so on)	
//           gateway_addr    网卡子网掩码地址
//
//返回:获取成功返回0 失败返回-1
//============================================================================
int nvr_netcfg_set_mask_addr(char *name,char *mask_ip); 

//============================================================================
// 函数名称: nvr_netcfg_get_boardcast_addr()
//
// 功能描述: 获取指定名字网卡的网络广播地址
//
// 参数:	 name            网卡名称(eth0 eth1 lo and so on)	
//           net_brdaddr  (输出)  网络广播地址
//
//返回:获取成功返回0 失败返回 < 0
//============================================================================
int nvr_netcfg_get_boardcast_addr(char *name,char *net_brdaddr);

//============================================================================
// 函数名称: nvr_netcfg_del_gw_addr()
//
// 功能描述: 删除指定名字网卡的网关地址
//
// 参数:	 name            网卡名称(eth0 eth1 lo and so on)	
//           gateway_addr    网卡网关地址
//
//返回:获取成功返回0 失败返回-1
//============================================================================
int nvr_netcfg_del_gw_addr(char *name,char *gateway_addr);

//============================================================================
// 函数名称: nvr_netcfg_update_dns()
//
// 功能描述: 更新DNS
//
// 参数:	 dnsaddr     主Dns 填0不设置	
//           dnsaddr2    副Dns 填0不设置	
//
//返回:获取成功返回0 失败返回-1
//============================================================================
int nvr_netcfg_update_dns(char *cfgFile, unsigned int dnsaddr, unsigned int dnsaddr2);

//============================================================================
// 函数名称: nvr_netcfg_get_dns()
//
// 功能描述: 获取DNS
//
// 参数:	 pDns   输出  主Dns 填NULL不获取	
//           pDns2  输出  副Dns 填NULL不获取	
//
//返回:获取成功返回0 失败返回-1
//============================================================================
int nvr_netcfg_get_dns(char *cfgFile, unsigned int *pDns, unsigned int *pDns2);

//============================================================================
// 函数名称: nvr_netconfig_findlistenport()
//
// 功能描述: 检测是否已经创建了监听端口nPort
//
// 参数:	 nPort  端口
//           	
//
//返回:创建成功返回0 失败返回-1
//============================================================================
int nvr_netcfg_find_listen_port(int nPort);

#ifdef __cplusplus
}
#endif 

#endif


