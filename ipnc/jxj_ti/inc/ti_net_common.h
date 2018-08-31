#ifndef _TI_NET_COMMON_H
#define _TI_NET_COMMON_H

#include "ti_net_struct.h"
#include "ti_type.h"
#include "ti_net_logon.h"


#define TI_NET_VERSION	 		0x0100
#define TI_NET_MAGIC_FLAG	 	0x30404891
#define TI_NET_MAX_MSG_LEN 		0x1000

#define TCP_TIMEOUT 			1000		// 单位:毫秒

/*BUG调试*/
//#define DEBUG_CLIENT
#ifdef DEBUG_CLIENT
	#define __DBUG(fmt, args...) fprintf(stderr, "CLIENT DEBUG(%s:%d):" fmt, __func__, __LINE__, ## args)
#else
	#define __DBUG(fmt, args...) 
#endif

#define __ERR(fmt, args...) fprintf(stderr, "CLIENT ERROR(%s:%d):" fmt, __func__, __LINE__, ## args)



/* 登陆加密字符 */
int ti_net_encrypt(char *pContent, int nLen);
/* 检查包头是否正确 */
int ti_net_check_netHdr(TI_NET_HDR_S *pNetHdr);
/* 填充网络包头 */
int net_set_net_hdr(TI_NET_HDR_S *pNetHdr, TI_U32 u32PackLen, 
				TI_U32 u32PackCmd, TI_U32 u32PackNo, TI_S32 s32ErrNo);

/* 连接设备 */
/* 返回值:套接字描述符 */
int ti_connect_server(const char * host, const char* port);

#endif

