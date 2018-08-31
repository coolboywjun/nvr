#ifndef _TI_NET_API_H
#define _TI_NET_API_H

#include <sys/time.h>
#include <sys/socket.h>

/*
* Copyright (c) 2010
* All rights reserved.
* 
* filename:net_net_api.h
* 
* func:本文件实现了linux下 socket封装
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

#define LISTENQ 1024
#define NET_MAX_BLOCK_SEND_TIME 3
#define NET_MAX_BLOCK_RECV_TIME 3

#define NET_DEFAULT_RECV_TIMEOUT 500
#define NET_DEFAULT_SEND_TIMEOUT 1000

#define NET_DEFAULT_LOCAL_TIME	2

#define NET_RETURN_FAIL -1
#define NET_RETURN_OK    0


//////////////////////////////////////////////////////////////////////////
//
//
//
//
//socket 操作函数
//
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////

//=================================================================
//函数: net_create_sock 
//
//功能:创建一个socket
//
//iType :创建的socket的类型 1:tcp 2:udp 3:本地通讯tcp sock
//		  4.本地通讯udp sock 使用af_unix协议族
//
//返回值：成功返回socket ,出错返回NET_RETURN_FAIL
//==================================================================
int net_create_sock(int iType);

//=================================================================
//函数: net_close_socket 
//
//功能:关闭指定的socket
//
//HId : 所要关闭的socket 	
//返回值：超时返回NET_RETURN_OK,出错返回NET_RETURN_FAIL
//=================================================================
int net_close_socket(int *fd);

//=================================================================
//函数: net_bind_sock 
//
//功能:给socket绑定一个制定的端口
//
//sockfd : 需绑定的一个socket
//
//ip   :   绑定的ip(默认填0)
//
//nport:   绑定的端口
//
//返回值：成功返回socket ,出错返回NET_RETURN_FAIL
//=================================================================
int net_bind_sock(int sockfd, int ip, int nPort);
//=================================================================
//函数: net_get_remote_addr
//
//功能: 取得socket绑定的远程IP
//
//sock:套接字
//pAddr: 远程IP
//返回值：成功返回NET_RETURN_OK ,出错返回NET_RETURN_FAIL
//=================================================================
int net_get_remote_addr(int sock, struct sockaddr *pAddr);

//=================================================================
//函数: net_get_sock_ip 
//
//功能: 取得socket绑定ip
//
//sock:套接字
//
//返回值：成功返回ip ,出错返回NET_RETURN_FAIL
//=================================================================
int net_get_sock_ip(int sock);

//=================================================================
//函数: net_get_sock_port 
//
//功能:取得socket绑定port
//
//sock:套接字
//
//返回值：成功返回port ,出错返回NET_RETURN_FAIL
//=================================================================
int net_get_sock_port(int sock);

//=================================================================
//函数: net_set_sock_attr 
//
//功能:设置sock属性
//
//参数:
//  fd:所要设置的socket
//  bReuseAddr:设置重新绑定
//  nSndTimeO:设置socket的发送超时时间(单位毫秒), 填0不设置该项
//  nRcvTimeO:设置socket的发送超时时间(单位毫秒), 填0不设置该项
//  nSndBuf:设置socket的发送缓冲区的大小, 填0不设置该项
//  nRcvBuf:设置socket的接收缓冲区的大小, 填0不设置该项     
//
//返回: 成功返回 FTP_SUCCESS 失败返回 _FTP_ERR_E中的错误码			
//=================================================================
int net_set_sock_attr(int fd, int bReuseAddr, int nSndTimeO, int nRcvTimeO, 
							 int nSndBuf, int nRcvBuf);
//=================================================================
//函数: net_set_sock_nodelay 
//
//功能:指定流Socket 是使用Nagle 算法
//
//参数:
//  fd:所要设置的socket
//		
//  返回值 :所用设置成功返回NET_RETURN_OK, 否则返回NET_RETURN_FAILE
//=================================================================
int net_set_sock_nodelay(int fd);

//=================================================================
//函数: net_set_sock_block 
//
//功能:设置socket为阻塞socket
//
// nSock: 所要设置的socket
//  
//返回值: 成功时返回NET_RETURN_OK， 否则返回NET_RETURN_FAIL
//=================================================================
int net_set_sock_block(int nSock);


//=================================================================
//函数: net_set_sock_noblock 
//
//功能:设置socket为非阻塞socket
//
// nSock: 所要设置的socket
//  
//返回值: 成功时返回NET_RETURN_OK， 否则返回NET_RETURN_FAIL
//=================================================================
int net_set_sock_noblock(int nSock);


//=================================================================
//函数: net_set_sock_keepalive 
//
//功能:  指定Socket keepalive 属性
//		
//参数:
//  fd:所要设置的socket
//
// 返回值 :所用设置成功返回NET_RETURN_OK, 否则返回NET_RETURN_FAILE
//==============================================================
int net_set_sock_keepalive(int fd);

//=================================================================
//函数: net_set_sock_boardcast 
//
//功能: 增加Socket广播属性
//		
//参数:
//  fd:所要设置的socket
//	
//返回值 :所用设置成功返回NET_RETURN_OK, 否则返回NET_RETURN_FAILE
//=================================================================
int  net_set_sock_boardcast(int fd);

//=================================================================
//函数: net_set_sock_multicast 
//
//功能: 指定Socket不接收本机发出的多播包
//		
//参数:
//  fd:所要设置的socket
//	
// 返回值 :所用设置成功返回NET_RETURN_OK, 否则返回NET_RETURN_FAILE
//=================================================================
int  net_set_sock_multicast_noloop(int fd);

//=================================================================
//函数: net_set_sock_multi_membership 
//
//功能: 指定Socket加入多播组
//		
//参数:
//  fd:         所要设置的socket
//	multiAddr:  多播组地址
//  interfaceIp:网卡ip(sock绑定的网卡ip)
//
//返回值 :所用设置成功返回NET_RETURN_OK, 否则返回NET_RETURN_FAILE
//=================================================================
int  net_set_sock_multi_membership(int fd, char *multiAddr, int interfaceIp);

//=================================================================
//函数: net_set_sock_rm_multi_membership 
//
//功能: 退出多播组
//		
//参数:
//  fd:         所要设置的socket
//	multiAddr:  多播组地址
//
//返回值 :所用设置成功返回NET_RETURN_OK, 否则返回NET_RETURN_FAILE
//=================================================================
int net_set_sock_rm_multi_membership(int fd, char *multiAddr);

//=================================================================
//函数: net_sock_bind_interface 
//
//功能: socket绑定到指定网卡
//		
//参数:
//  sockfd:     所要设置的socket
//	interfaceName: 网卡名称
//
//返回值 :所用设置成功返回NET_RETURN_OK, 否则返回NET_RETURN_FAILE
//=================================================================
int net_sock_bind_interface(int sockfd, char *interfaceName);



//////////////////////////////////////////////////////////////////////////
//
//
//
//                              网络处理 函数
//
//
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////

//=================================================================
//函数: net_tcp_listen 
//
//功能: 创建一个tcp监听服务器
//		
//参数:
//  host    输入: 本地主机名(默认填NULL)
//	serv    输入: 本地服务名(可填服务名称或者端口的字符串)
//  addrlenp输入/输出: 套接字长度
//
//返回值 :所用设置成功返回NET_RETURN_OK, 否则返回NET_RETURN_FAILE
//=================================================================
int net_tcp_listen(const char *host, const char *serv, int *addrlenp);

//=================================================================
//函数: net_tcp_block_connect 
//
//功能:tcp阻塞方式连接服务端
//
//参数:
//  localHost:本地端的域名或者ip地址
//  localServ:本地端的服务名称或者是端口
// 
//	host: 服务端的域名或者ip地址
//	serv: 服务端的服务名称或者是端口
//	
//返回值：连接成功返回连接使用的socket, 否则返回NET_RETURN_FAIL
//=================================================================
int net_tcp_block_connect (const char *localHost, const char *localServ, 
						  const char *dstHost, const char *dstServ);

//=================================================================
//函数: net_tcp_block_recv 
//
//功能: tcp阻塞socket接收函数
//
//参数:
//  sockfd:接收的socket
//  rcvBuf:接收的缓冲区
//  bufSize:接收的缓冲区大小
//  rcvSize:指定接收的字节数, 填0不指定接收的字节数
//  
//	返回值：连接成功返回接收到的字节数,否则返回NET_RETURN_FAIL
//=================================================================
int net_tcp_block_recv(int sockfd, void *rcvBuf, int bufSize, int rcvSize);

//=================================================================
//函数: net_tcp_block_send 
//
//功能:tcp阻塞方式发送
//
//参数:
//
//fd:所要发送数据的socket
//vptr:所要发送的数据
//n: 所要发送的数据的字节数
//
//返回值:返回发送的字节数,否则返回NET_RETURN_FAIL
//=================================================================
int net_tcp_block_send(int fd, const void *vptr, int n);

//=================================================================
//函数: net_tcp_block_accept 
//
//功能:tcp 阻塞方式accept
//
//参数:
//
//fd 输入          : 监听的socket
//sa 输入          : 对端的socket地址结构
//salenptr输入/输出:输入为struct sockaddr 的结构大小, 输出为对端的socket地址结构
//
//返回值:成功返回所用的socket, 否则返回NET_RETURN_FAIL
//=================================================================
int net_tcp_block_accept(int fd, struct sockaddr *sa, int *salenptr);

//=================================================================
//函数: net_tcp_noblock_connect 
//
//功能:tcp非阻塞方式连接服务端
//
//参数:
//localHost:本地端的域名或者ip地址
//localServ:本地端的服务名称或者是端口
//
//dstHost: 服务端的域名或者ip地址
//dstServ: 服务端的服务名称或者是端口
//timeout_ms: 连接服务器超时时间(单位毫秒)
//
//返回值：连接成功返回连接使用的socket, 否则返回NET_RETURN_FAIL
//=================================================================	
int net_tcp_noblock_connect(const char *localHost, const char *localServ, 
					const char *dstHost, const char *dstServ, int timeout_ms);

//=================================================================
//函数: net_tcp_noblock_recv 
//
//功能:tcp非阻塞socket接收函数
//
//参数:
//  sockfd:接收的socket
//  rcvBuf:接收的缓冲区
//  bufSize:接收的缓冲区大小
//  rcvSize:制定接收的字节数, 填0不指定接收的字节数
//  timeOut:接收超时时间(单位毫秒)		
//
//返回值:成功返回接收到的字节数, 否则返回NET_RETURN_FAIL
//=================================================================
int net_tcp_noblock_recv(int sockfd, void *rcvBuf, int bufSize, int rcvSize, 
						int timeOut);
//=================================================================
//函数: net_tcp_noblock_send 
//
//功能:tcp非阻塞方式发送
//
//参数:
// hSock:所要发送数据的socket
// pbuf:所要发送的数据
// size: 所要发送的数据的字节数
// pBlock: 判读网路是不是断开 (连续超过timeout毫秒次不成功则认为网络断开)
// timeOut: 发送超时时间(单位毫秒)
//
//返回值:成功返回发送的字符数, 否则返回NET_RETURN_FAIL
//=================================================================
int net_tcp_noblock_send(int hSock,void *pbuf,int size, int *pBlock, int timeOut);

//=================================================================
//函数: net_tcp_noblock_mtu_send 
//
//功能:tcp非阻塞方式发送每次发送mtu字节数据
//
//参数:
// hSock:所要发送数据的socket
// pbuf: 所要发送的数据
// size: 所要发送的数据的字节数
// mtu:  每次发送的最大字节数
//返回值:成功返回发送的字符数, 否则返回NET_RETURN_FAIL
//=================================================================
int net_tcp_noblock_mtu_send(int hSock,void *pbuf,int size, int mtu);

//=================================================================
//函数: net_tcp_noblock_accept 
//
//功能:tcp 非阻塞方式accept
//
//参数:
//fd[in]: 监听的socket
//sa[out]: 对端的socket地址结构
//salenptr[in/out]:输入为struct sockaddr 的结构大小, 输出为对端的socket地址结构
//to:      accept超时结构
//
//返回值:成功返回连接所用的socket，否则返回NET_RETURN_FAIL
//=================================================================
int net_tcp_noblock_accept(int fd, struct sockaddr *sa, int *salenptr, struct timeval * to);

//=================================================================
//函数: net_udp_recv 
//
//功能:udp socket接收函数
//
//参数:
//sockfd:接收的socket
//rcvBuf:接收的缓冲区
//bufSize:接收的缓冲区大小
//rcvSize:制定接收的字节数, 填0不指定接收的字节数
//from:获取发送端的socket结构
//fromlen:获取发送端的socket结构长度
//		
//返回值:成功返回接收到的字节数, 否则返回NET_RETURN_FAIL
//=================================================================
int net_udp_recv(int sockfd, void *rcvBuf, int bufSize, int rcvSize, struct sockaddr *from, int *fromlen);

//=================================================================
//函数: net_udp_send 
//
//功能:udp方式发送
//
//参数:
//hSock:所要发送数据的socket
//pbuf:所要发送的数据
//size: 所要发送的数据的字节数
//distAddr:目的地址
//		 
//返回值:成功返回发送的字符数, 否则返回NET_RETURN_FAIL
//=================================================================
int net_udp_send(int hSock,void *pbuf,int size, struct sockaddr * distAddr);

//=================================================================
//函数: net_ping 
//
//功能:ping 功能实现函数 
//
//参数:
//ips: 远端服务器ip
//timeout:发送ping包超时时间
//max_times:发送失败尝试次数
//
//		 
//返回值:成功返回NET_RETURN_OK, 否则返回NET_RETURN_FAIL
//=================================================================	
int net_ping(char *ips, int timeout, int max_times);

//=================================================================
//函数: net_get_host_ip 
//
//功能:取得给出的host的ip地址(返回的第一个地址)
//
//af: host 使用的协议族
//host:host 名
//	 
//返回值：成功返回host的ip值(网络序),否则返回NET_RETURN_FAIL
//=================================================================
int net_get_host_ip(int af, char *host);

//=================================================================
//函数: net_select 
//
//功能:监听fd_array数组中的socket
//
//fd_array: socket 数组
//fd_num:   socket 的个数
//fd_type:  bit 1 为 1 代表监听socket 是否可读
//	 	  bit 2 为 1 代表监听socket 是否可写
//		  bit 3 为 1 代表监听socket 是否有异常
//	
//返回值：超时返回NET_RETURN_OK,出错返回NET_RETURN_FAIL，否则
//		如果有可读的返回该socket值|0x10000
//		有可写返回该socket值|0x20000
//		有异常返回该socket值|0x40000
//=================================================================
int net_select(int *fd_array, int fd_num, int fd_type, int time_out);

//=================================================================
//函数: net_ip_n2a 
//
//功能:整数IP 转成 点分十进制 IP
//
//ip    输入: 整数IP(网络序)
//ourIp 输出: 点分十进制 IP缓存
//len   输入: 点分十进制 IP缓存大小
//	
//返回值:成功返回NET_RETURN_OK,否则返回NET_RETURN_FAIL
//=================================================================	
unsigned long net_ip_n2a(unsigned long ip, char *ourIp, int len);
//=================================================================
//函数: net_ip_a2n 
//
//功能:点分十进制 IP(或者域名) 转成 整数IP  
//
//ip    输入: 点分十进制 IP缓存
//	
//返回值:成功返回整数IP(网络序),否则返回NET_RETURN_FAIL
//=================================================================	
unsigned long net_ip_a2n(const char *szIp);

//=================================================================
//函数: net_Ip_N2A 
//
//功能:net_itoa 跟windows 上itoa一样 数字转成字符串
//
//n:数值
//
//s:字串
//
//返回值:无
//=================================================================
void   net_itoa(int   n,   char   s[]);

/*
SO_LINGER

   此选项指定函数close对面向连接的协议如何操作（如TCP）。
   缺省close操作是立即返回，如果有数据残留在套接口缓冲区中
   则系统将试着将这些数据发送给对方。
SO_LINGER选项用来改变此缺省设置。使用如下结构：
struct linger {
     int l_onoff; // 0 = off, nozero = on 
     int l_linger; // linger time 
};

有下列三种情况：

l_onoff为0，则该选项关闭，l_linger的值被忽略，等于缺省情况，close立即返回； 

l_onoff为非0，l_linger为0，则套接口关闭时TCP夭折连接，
TCP将丢弃保留在套接口发送缓冲区中的任何数据并发送一个RST给对方，
而不是通常的四分组终止序列，这避免了TIME_WAIT状态； 

l_onoff 为非0，l_linger为非0，当套接口关闭时内核将拖延一段时间（由l_linger决定）。
如果套接口缓冲区中仍残留数据，进程将处于睡眠状态，
直 到（a）所有数据发送完且被对方确认，
之后进行正常的终止序列（描述字访问计数为0）或（b）延迟时间到。
此种情况下，应用程序检查close的返回值是非常重要的，
如果在数据发送完并被确认前时间到，
close将返回EWOULDBLOCK错误且套接口发送缓冲区中的任何数据都丢失。
close的成功返回仅告诉我们发送的数据（和FIN）已由对方TCP确认，
它并不能告诉我们对方应用进程是否已读了数据。
如果套接口设为非阻塞的，它将不等待close完 成。 


l_linger的单位依赖于实现，4.4BSD假设其单位是时钟滴答（百分之一秒），
但Posix.1g规定单位为秒。

*/
int net_sock_set_linger(int sockfd);

int net_sock_unset_linger(int sockfd);

//////////////////////////////////////////////////////////////////////////
//
//
//
//                              本地通讯函数
//
//
//
//
//
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////

//=================================================================
//函数: net_create_local_tcp_server 
//
//功能: 创建本地通讯面向连接服务器
//
//参数:
//
//svrPath: 本地通讯服务器命名节点
//
//	
//返回值:成功返回服务器sock,否则返回NET_RETURN_FAIL
//=================================================================	
int net_create_local_tcp_server(char *svrPath);

//=================================================================
//函数: net_local_tcp_connect 
//
//功能: 连接本地通讯服务器
//
//参数:
//
//svrPath: 本地通讯服务器命名节点
//
//	
//返回值:成功返回sock节点,否则返回NET_RETURN_FAIL
//=================================================================	
int net_local_tcp_connect(char *svrPath);

//=================================================================
//函数: net_local_tcp_send 
//
//功能:本地通讯Tcp方式发送
//
//参数:
// fd:所要发送数据的socket
// data:所要发送的数据
// len: 所要发送的数据的字节数
//
//返回值:成功返回发送的字符数, 否则返回NET_RETURN_FAIL
//=================================================================
int net_local_tcp_send(int fd, void *data, int len);

//=================================================================
//函数: net_local_tcp_recv 
//
//功能:本地通讯面向连接接收函数
//
//参数:
//  fd:接收的socket
//  data:接收的缓冲区
//  bufSize:接收的缓冲区大小
//
//返回值:成功返回接收到的字节数, 否则返回NET_RETURN_FAIL
//=================================================================
int net_local_tcp_recv(int fd, void *data, int bufSize);

//=================================================================
//函数: net_create_local_udp_server 
//
//功能: 创建本地通讯非面向连接服务器
//
//参数:
//
//svrPath: 本地通讯服务器命名节点
//
//	
//返回值:成功返回服务器sock,否则返回NET_RETURN_FAIL
//=================================================================	
int net_create_local_udp_server(char *svrPath);

//=================================================================
//函数: net_local_udp_send 
//
//功能:本地通讯Udp方式发送
//
//参数:
// fd:所要发送数据的socket
//svrPath: 接收服务器命名节点
// data:所要发送的数据
// len: 所要发送的数据的字节数
//
//返回值:成功返回发送的字符数, 否则返回NET_RETURN_FAIL
//=================================================================
int net_local_udp_send(int fd, char * svrPath, void * data, int nLen);

//=================================================================
//函数: net_local_udp_recv 
//
//功能:本地通讯非面向连接接收函数
//
//参数:
//  fd:接收的socket
//  data:接收的缓冲区
//  bufSize:接收的缓冲区大小
//
//返回值:成功返回接收到的字节数, 否则返回NET_RETURN_FAIL
//=================================================================
int net_local_udp_recv(int fd, void * data, int nLen);

//=================================================================
//函数: net_get_wanip 
//
//功能:获取外网Ip地址
//
//参数:
//  pIp 输出:获取成功返回外网Ip
//
//返回值:成功返回NET_RETURN 否则返回NET_RETURN_FAIL
//=================================================================
int net_get_wan_ip(char *pIp);

//============================================================================
// 函数名称: Net_GetIpAddr()
// 功能描述: 得到指定名字的网络ip
// 其他说明:
//============================================================================
int net_get_ip_addr(char *name,char *net_ip);


//============================================================================
//获得网卡物理地址
//============================================================================
int net_get_mac_addr(char *name, char *net_mac);


//============================================================================
// 函数名称: Net_GetMaskAddr()
// 功能描述: 得到指定名字的子网掩码
// 其他说明:
//============================================================================
int net_get_mask_addr(char *name,char *net_mask);


//============================================================================
// 函数名称: Net_GetBrdcastAddr()
// 功能描述: 得到指定名字的网络广播地址
// 其他说明:
//============================================================================
int net_get_boardcast_addr(char *name,char *net_brdaddr);


//============================================================================
// 函数名称: Net_SetIpAddr()
// 功能描述: 设置指定名字网络的IP地址
// 其他说明:
//============================================================================
int net_set_ip_addr(char *name, char *net_ip);


int net_set_mac_addr(char *name, char *mac);


//============================================================================
// 函数名称: Net_SetMaskAddr()
// 功能描述: 设置指定名字网络的子网掩码
// 其他说明:
//============================================================================
int net_set_mask_addr(char *name,char *mask_ip); /* name : eth0 eth1 lo and so on */


//============================================================================
// 函数名称: Net_GetGwAddr()
// 功能描述: 得到默认网关
// 其他说明:
//============================================================================
int net_get_gw_addr(char *name,char *gateway_addr);


//============================================================================
// 函数名称: Net_SetGwAddr()
// 功能描述: 设置默认网关(gateway)
// 其他说明:
//============================================================================
int net_set_gw_addr(char *name,char *gateway_addr);



//============================================================================
// 函数名称: Net_DelGwAddr()
// 功能描述: 删除默认网关(gateway) 
// 其他说明:
//============================================================================
int net_del_gw_addr(char *name,char *gateway_addr);



#ifdef __cplusplus
}
#endif 

#endif





