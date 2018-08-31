#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <linux/sockios.h>
#include <linux/if.h>  
#include <time.h>
#include <sys/time.h>
#include <malloc.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>

#include "net_api.h"
#include "ti_net_common.h"
#include "ti_net_struct.h"
#include "ti_type.h"
#include "ti_net_interface.h"
#include "ti_struct.h"

//48¸ö×Ö·û
#define ALEX_XOR_ENCRYPT		"*(!@#$%^alexnanshanshenzhenchina20110505!@#$%^)*"

int ti_net_check_netHdr(TI_NET_HDR_S *pNetHdr)
{
	if (pNetHdr->u32MagicFlag != TI_NET_MAGIC_FLAG)
		return TI_ERR_PROTACAL;

	return TI_ERR_SUCCESS;
}

int net_set_net_hdr(TI_NET_HDR_S *pNetHdr, TI_U32 u32PackLen, 
		TI_U32 u32PackCmd, TI_U32 u32PackNo, TI_S32 s32ErrNo)
{
	pNetHdr->u16Version    = TI_NET_VERSION;
	pNetHdr->u16CmdType  = u32PackCmd;
	pNetHdr->u32PacketSize= u32PackLen;
	pNetHdr->u32PacketNo  = u32PackNo;
	pNetHdr->u32MagicFlag = TI_NET_MAGIC_FLAG;
	pNetHdr->s32ErrNo	  = s32ErrNo;
	return TI_NET_ERR_SUCCESS;
}
int ti_connect_server(const char * host, const char* port)
{
	int nSock;
	if(host == NULL || port == NULL)
		return TI_FAILURE;

	nSock = net_tcp_noblock_connect(NULL, NULL, host, port, TCP_TIMEOUT); 
	if(nSock < 0)
	{
		//__ERR("Failed to connect the server: %d\n", nSock);
		return TI_ERR_CONNECT_FAIL;
	}
	net_set_sock_noblock(nSock);

	return nSock;
}

int ti_net_encrypt(char *pContent, int nLen)
{
	char	*enMask;
	char	enChar;
	int		numChars;


	enMask = ALEX_XOR_ENCRYPT;
	numChars = 0;

	while (*pContent && numChars < nLen) {
		enChar = *pContent ^ *enMask;
		/*
		 *		Do not produce encrypted text with embedded linefeeds or tabs.
		 *			Simply use existing character.
		 */
		//if (enChar && !iswspace(enChar)) 
		if (enChar) 
			*pContent = enChar;
		/*
		 *		Increment all pointers.
		 */
		enMask++;
		pContent++;
		numChars++;
		/*
		 *		Wrap encryption mask pointer if at end of length.
		 */
		if (*enMask == '\0') {
			enMask = ALEX_XOR_ENCRYPT;
		}
	}

	return numChars;
}





