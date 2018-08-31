/////////////////////////////////////////////////////////////////////////////////
////////////////////////// hi_dianxin_common.c ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


/*-------------------------------------------------*
 *包含头文件                                       *
 *-------------------------------------------------*/
#include "hi_dianxin_common.h"
#include "hi_platform_dianxin.h"
#include "hi_character_encode.h"
#include <arpa/inet.h>
#include "net_api.h"

#include <netdb.h>
#include <sys/socket.h>



/*-------------------------------------------------*
 *外部变量说明                                     *
 *-------------------------------------------------*/

/*-------------------------------------------------*
 *外部函数原型说明                                 *
 *-------------------------------------------------*/

/*-------------------------------------------------*
 *内部函数原型说明                                 *
 *-------------------------------------------------*/

/*-------------------------------------------------*
 *全局变量                                         *
 *-------------------------------------------------*/

/*-------------------------------------------------*
 *模块级变量                                       *
 *-------------------------------------------------*/

/*-------------------------------------------------*
 *常量定义                                         *
 *-------------------------------------------------*/
#define TIMEOUT 3000

/*-------------------------------------------------*
 *宏定义                                           *
 *-------------------------------------------------*/
typedef struct _DIANXIN_HTTP_RESP_S_
{
	char	res_num[4];
	char	res_ch[4];
}DIANXIN_HTTP_RESP;


/*-------------------------------------------------*
 *函数实现                                         *
 *-------------------------------------------------*/


int dainxin_mxml_string_copy(char *pBuf, mxml_node_t *pNode)
{
	if(mxmlGetText(pNode, 0) != NULL)
		strcpy(pBuf, mxmlGetText(pNode, 0));
	else 
		return -1;
	return 0;
}




mxml_node_t *dianxin_mxml_node_get(mxml_node_t *pParent, char *pNodeName)
{
	mxml_node_t *pNode;
	if((pNode = mxmlFindElement(pParent, pParent, pNodeName, NULL, NULL, MXML_DESCEND)) == NULL)
	{
		__E("Can not find element :%s!\r\n", pNodeName);
		return NULL;
	}
	return pNode;
}



/*	解析URL	*
*	输入	pGroupUrl格式:http://xxx.xxx.xxx.xxx:XXXX/xxxx
*	输出	pIp	:url包含的IP
			nPort : url包含的端口
			pUrl : url包含的地址
*			*/
int hi_dianxin_parse_url(char *pGroupUrl, char *pIp, char *pPort, char *pUrl)
{
	char *nPColn;
	char *nPSlash;
	int nLen;
	int ret;
	struct in_addr s;
	char **pptr;
	char szStr[128];
	struct hostent *pHostent;
	nLen = strlen("http://");
	printf("the url is %s\r\n", pGroupUrl);
	if(strncmp(pGroupUrl, "http://", nLen) != 0)
	{
		__E("the url has not 'http://'!\r\n");
		return -1;
	}
	if((nPSlash = strchr(pGroupUrl + nLen, '/')) == NULL)
	{
		__D("the url has not '/'!\r\n");
		nPSlash = strchr(pGroupUrl + nLen, '\0');
	}
	if((nPColn = strchr(pGroupUrl + nLen, ':')) == NULL)
	{
		strcpy(pPort, "80");
		strncpy(szStr, pGroupUrl + nLen, nPSlash - pGroupUrl - nLen);
		szStr[nPSlash - pGroupUrl - nLen] = '\0';
		strcpy(pUrl, nPSlash);
	}else
	{
		memset(szStr, 0, sizeof(szStr));
		strncpy(pPort, nPColn + 1, nPSlash - nPColn - 1);
		pPort[nPSlash - nPColn -1] = '\0';
		strncpy(szStr, pGroupUrl + nLen, nPColn - pGroupUrl - nLen);
		szStr[nPColn - pGroupUrl - nLen] = '\0';
		strcpy(pUrl, nPSlash);
	}
	
	__D("the ip is %s!\r\n", szStr);
	if((ret =inet_pton(AF_INET, szStr, &s.s_addr)) <= 0)
	{
		if((pHostent = gethostbyname((const char *)szStr)) == NULL)
		{
			printf("gethostbyname error by %s\r\n", szStr);
			return -1;
		}else
		{
			pptr=pHostent->h_addr_list;
			for(; *pptr != NULL; pptr++)
			{
				printf("the ip is %s\r\n", inet_ntop(pHostent->h_addrtype, *pptr, pIp, 32));
				printf("the ip is %s\r\n", pIp);
			}
		}
	}else
	{
		strcpy(pIp, szStr);
	}
	
	__D("the ip is %s!\r\n", pIp);
	__D("the port is %s!\r\n", pPort);
	__D("the pUrl is %s!\r\n", pUrl);
	return 0;
}



int hi_dianxin_http_write(int fd, const char * body, int bodysize,
          const char * headers, int headerssize)
{
	int n = 0;
	char * p;

	p = malloc(headerssize+bodysize);
	if(!p)
	  return -1;
	memcpy(p, headers, headerssize);
	memcpy(p+headerssize, body, bodysize);
	n = net_tcp_noblock_send(fd, p, headerssize+bodysize, 0, TIMEOUT);
	if(n<0) {
	  __E("POST SUBMIT SEND ERR!\r\n");
	}
	__D(">>>>>>>>>the send buf is %s!\r\n", p);
	free(p);
	return n;
}

int hi_dianxin_register_recv(int nFd, char *pRecvBuf, int nRecvLen)
{
	DIANXIN_HTTP_RESP stHttpResp;
	if(net_tcp_noblock_recv(nFd, pRecvBuf, nRecvLen, 0, TIMEOUT) < 0)
	{
		__E("The group recv err!\r\n");
		return -1;
	}
	if(pRecvBuf == NULL)
		return -1;
	__D(">>>>>>>>>>>>>>>the recvbuf is %s!\r\n", pRecvBuf);
	if(strncmp(pRecvBuf, "HTTP", 4) == 0)
	{
		sscanf(pRecvBuf, "%*s%s%s", stHttpResp.res_num, stHttpResp.res_ch);
		__D("the res_num is %s\r\n", stHttpResp.res_num);
		__D("the res_ch is %s\r\n", stHttpResp.res_ch);
	}

	if(!strcmp(stHttpResp.res_num, "200") && !strcmp(stHttpResp.res_ch, "OK"))
	{
		return 0;
	}
	return -1;
}


int hi_dianxin_message_send(int fd, int nCmd, int nSeq, char *pBodyBuf, int nBodyLen)
{
	int nSendLen;
	int nLen = 0;
	char *p;
//	char p[1024];
	char szMd5Buf[16];
	HI_DIANXIN_MSG_HEAD stMsgHead;
	int nHeadLen = sizeof(HI_DIANXIN_MSG_HEAD);
	memset(&stMsgHead, 0, sizeof(stMsgHead));
	stMsgHead.u16Head = htons(0xFAF5);
	stMsgHead.u8Cmd = nCmd;
	stMsgHead.u16Seq = htons(nSeq);
	stMsgHead.u8Remark = 0;
	stMsgHead.u16Len = htons(nBodyLen);
	p = malloc(nHeadLen + nBodyLen + sizeof(szMd5Buf));
	if(!p)
		return -1;
	memcpy(p, &stMsgHead, nHeadLen);
	nLen = nHeadLen;
	memcpy(&p[nLen], pBodyBuf, nBodyLen);
	//printf("the pBodyBuf is %s\r\n", pBodyBuf);
	nLen += nBodyLen;
	hi_md5_encrypt_bin((unsigned char *)szMd5Buf, (unsigned char *)p, nLen);
	memcpy(&p[nLen], szMd5Buf, sizeof(szMd5Buf));
	nLen += sizeof(szMd5Buf);
	nSendLen = net_tcp_noblock_send(fd, p, nLen, 0, TIMEOUT);
	if(nSendLen < 0)
	{
		__E("the nCmd[%x] message send err!\r\n", nCmd);
	}
	free(p);
	return nSendLen;
}


