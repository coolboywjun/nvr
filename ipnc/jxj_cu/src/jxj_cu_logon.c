
#include "jxj_cu_heads.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <semaphore.h>

#include <av_pool.h>
#include "jxj_cu_base64.h"
#include "jxj_cu_logon.h"
#include "jxj_cu_ipnc_mng.h"


typedef struct _PipeNode
{
	const void *data;
	int 	nDataLen;	
}PipeNode;

#define BUF_SIZE	1024*2*2
	
//管道控制
static inline int jxj_cu_pipe_write(int fd, const void *sendBuf, int sendLen)
{
	int nLen;
	int nTotalSendLen = 0;
	while(nTotalSendLen < sendLen)
	{
		nLen = write(fd, sendBuf + nTotalSendLen, sendLen - nTotalSendLen);
		if(nLen < 0)
		{
			if(errno == EINTR || errno == EAGAIN)
				continue;
			return -1;
		}
		nTotalSendLen += nLen;
	}
	return nTotalSendLen;
}

static inline int jxj_cu_pipe_read(int fd, void *recvBuf, int recvLen)
{
	int nLen;
	int nTotalRecvLen = 0;
	while(nTotalRecvLen < recvLen)
	{
		nLen = read(fd, recvBuf + nTotalRecvLen, recvLen - nTotalRecvLen);
		if(nLen < 0)
		{
			if(errno == EINTR || errno == EAGAIN)
				continue;
			return -1;
		}
		nTotalRecvLen += nLen;
	}
	return nTotalRecvLen;
}

static int jxj_cu_pipe_send(int fd, const void *sendBuf, int sendLen)
{
	PipeNode node;
	node.data = sendBuf;
	node.nDataLen = sendLen;
	return jxj_cu_pipe_write(fd, &node, sizeof(node));
}

static int jxj_cu_pipe_recv(int fd, char **recvBuf)
{
	PipeNode node;
	
	if(jxj_cu_pipe_read(fd, &node, sizeof(node)) != sizeof(node))
	{
		__ERR("jxj_cu_pipe_read failed\n");	
		return -1;	
	}
	*recvBuf = (char *)node.data;
	
	return node.nDataLen;
}

static int jxj_cu_pipe_session(JXJ_CU_LOGON_S *pLogonHdl, 
							void *sendBuf, int sendLen, 
							char **recvBuf)
{
	int len = 0;
	if((len = jxj_cu_pipe_send(pLogonHdl->sendPipe[1], sendBuf, sendLen)) < 0)
	{
		__ERR("sendLen: %d, len: %d\n", sendLen, len);
		free(sendBuf);
		return -1;
	}
	if(net_select(&pLogonHdl->recvPipe[0], 0x1, 0x1, 10000) <= 0)
		return -1;
	if((len = jxj_cu_pipe_recv(pLogonHdl->recvPipe[0], recvBuf)) < 0)
		return -1;
	return len;
}

static int jxj_cu_connect_server(const char * host, const char* port)
{
	int nSock;
	if(host == NULL || port == NULL)
		return -1;

	nSock = net_tcp_noblock_connect(NULL, NULL, host, port, 1000); 
	if(nSock < 0)
	{
		//__ERR("Failed to connect the server: %d\n", nSock);
		return -1;
	}
	net_set_sock_noblock(nSock);

	return nSock;
}

static  int jxj_cu_send(int socket, void *sendBuff, int sendLen)
{
	if(sendBuff == NULL || sendLen <= 0)
		return -1;
	if(net_tcp_noblock_send(socket, sendBuff, sendLen, NULL, 1000) != sendLen)
	{
		__ERR("(%s:%d:%s)send data to server is failed !\n", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}
	return sendLen;
}

//接收数据
static int jxj_cu_recv(int socket, void *recvBuff, int recvBuffLen)
{
	int len;
	if(recvBuff == NULL || recvBuffLen <= 0)
		return -1;

	len = net_tcp_noblock_recv(socket, recvBuff, recvBuffLen, recvBuffLen,  1000);
	if(len < 0)
	{
		__ERR("(%s:%d:%s)recv data from server is failed !\n", __FILE__, __LINE__, __FUNCTION__);
		return -1;
	}

	return len;
}
static int jxj_cu_recv_ctrl(int sock, char **recvBuf)
{
	jpf_proto_head_t netHdr;
	int packetLen;
	char *pRecvBuf;
	
	//接收cu 包头
	if(jxj_cu_recv(sock, &netHdr, sizeof(netHdr)) != sizeof(netHdr))
	{
		__ERR("Receive net header failed.\n");
		return -1;
	}
	if(!VALID_PROTO_HEAD(&netHdr))
	{
		__ERR("The net magic is wrong.\n");
		return -1;
	}
	packetLen =  sizeof(netHdr) + GET_PROTO_HEAD_L(&netHdr);
	pRecvBuf = malloc(packetLen + 16);
	memcpy(pRecvBuf, &netHdr, sizeof(netHdr));
	if(jxj_cu_recv(sock, pRecvBuf+sizeof(netHdr), GET_PROTO_HEAD_L(&netHdr)) 
			!= GET_PROTO_HEAD_L(&netHdr))
	{
		__ERR("jxj_cu_recv failed.\n");
		free(pRecvBuf);
		return -1;
	}
	pRecvBuf[packetLen] = '\0';
	*recvBuf = pRecvBuf;
	
	return packetLen;
}

//CU协议头封装
static void jxj_cu_make_net_head(jpf_proto_head_t *pHd, uint32_t payload_len)
{
	static uint32_t  seq = 2112;
	//uint8_t	  tot_packets;
	//uint8_t	  packet_no;
	
	SET_PROTO_HEAD_M(pHd);
	SET_PROTO_HEAD_S(pHd, seq);
	SET_PROTO_HEAD_L(pHd, payload_len);
	seq++;
	//SET_PROTO_HEAD_P(head, packets);
	//SET_PROTO_HEAD_N(head, no);
}

static int jxj_cu_xml_data_send(JXJ_CU_LOGON_S *pLogonHdl, xmlDocPtr doc, 
								char **pRecvBuf)
{
	int ret = 0;
	int xmlDataSize = 0;
	xmlChar *pXmlDataBuf = NULL;
	
	jpf_proto_head_t jpfHead = {0};
	int jpfHdLen = sizeof(jpf_proto_head_t);
	
	int sendLen = 0;
	unsigned char *pSendBuf;
	
	if(doc == NULL)
		return -1;

	//将xml转换到缓存
	//xmlDocDumpMemory(doc, &pXmlDataBuf, &xmlDataSize);
	xmlDocDumpMemoryEnc(doc, &pXmlDataBuf, &xmlDataSize, "UTF-8");
	if(pXmlDataBuf == NULL)
		return -1;
	
	//__DBUG("xmlDataSize: %d\n", xmlDataSize);
	//__DBUG("pXmlDataBuf: %s\n", (char*)pXmlDataBuf);

	jxj_cu_make_net_head(&jpfHead, xmlDataSize);
	sendLen = jpfHdLen + xmlDataSize;
	pSendBuf = malloc(sendLen);
	memcpy(pSendBuf, &jpfHead, jpfHdLen);
	memcpy(pSendBuf + jpfHdLen, pXmlDataBuf, xmlDataSize);
	
	xmlFree(pXmlDataBuf);
	
	ret = jxj_cu_pipe_session(pLogonHdl, pSendBuf, 
								  sendLen, pRecvBuf);
	if(ret < 0)
	{
		__ERR("jxj_cu_pipe_session Failed: %d.\n", ret);
		ret = -1;
	}
	
	return ret;
}


//增加xml节点
static void xml_add_node(xmlNodePtr node,const char* strdata, int intdata, const char* name, char flag)
{
	char strBuf[1024] = {0};

	memset(strBuf, 0, sizeof(strBuf));

	if(NULL == strdata)
	{
		if('d' == flag)
			sprintf(strBuf, "%d", intdata);
		else if('x' == flag)
			sprintf(strBuf, "%x", intdata);
		else
			return;
	}
	else
	{	
		if('s' == flag)
			strncpy(strBuf, strdata, sizeof(strBuf));
		else if('c' == flag)
			sprintf(strBuf, "%c", *strdata);
		else
			return;
	}
	
	xmlNewTextChild(node, NULL, (xmlChar *)name, (xmlChar *)strBuf);
}

//保活
static int jxj_cu_keep_alive(int sock)
{
	jpf_proto_head_t jpfHead = {0};
	int jpfHdLen = sizeof(jpf_proto_head_t);
	
	char sendBuf[BUF_SIZE] = {0};
	int sendLen = 0;
	
	int xmlDataSize = 0;
	xmlChar *pXmlDataBuf = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr root_node = NULL;

	/*封装cu 保活xml数据格式*/
	doc = xmlNewDoc(BAD_CAST"1.0");
	if(NULL == doc)
		return -1;

	//设置根节点
	root_node = xmlNewNode(NULL,BAD_CAST"message");
	xmlNewProp (root_node, BAD_CAST "type", BAD_CAST "UserHeart");
	xmlDocSetRootElement(doc,root_node);
	
	xmlDocDumpMemory(doc, &pXmlDataBuf, &xmlDataSize);
	if(pXmlDataBuf == NULL)
	{
		xmlFreeDoc(doc);
		return -1;
	}

	jxj_cu_make_net_head(&jpfHead, xmlDataSize);
	
	memcpy(sendBuf, &jpfHead, jpfHdLen);
	memcpy(sendBuf + jpfHdLen, pXmlDataBuf, xmlDataSize);
	sendLen = jpfHdLen + xmlDataSize;
	
	xmlFree(pXmlDataBuf);
	xmlFreeDoc(doc);
	
	if(0 > jxj_cu_send(sock, sendBuf, sendLen))
	{
		return -1;
	}

	return 0;
}

//解析登录响应包
static int jxj_cu_userlogin_res(char* pXmlDataBuf)
{
	int ret = 0;
	jpf_proto_head_t *pNetHdr = NULL;
	char *xmlData = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr curNode = NULL;
	xmlChar *szKey = NULL;
	
	if(NULL == pXmlDataBuf)
		return -1;
	
	pNetHdr = (jpf_proto_head_t*)pXmlDataBuf;
	if(!VALID_PROTO_HEAD(pNetHdr))
		return -1;
	
	xmlData = (char*)(pNetHdr + 1);

	//__DBUG(">>>>>>>>>>>>>>xmlData: %s\n", xmlData);
	doc = xmlReadMemory(xmlData, strlen(xmlData), 
		                NULL, "UTF-8", 0);
	if(doc == NULL)
		return -1;
		
	//获得根节点
	curNode = xmlDocGetRootElement(doc);
	curNode = curNode->xmlChildrenNode;

	while(curNode != NULL)
	{
		szKey = xmlNodeGetContent(curNode);
		//__DBUG(">>>>>>>>>curNode->name: %s\n", curNode->name);
		//__DBUG(">>>>>>>>>szKey: %s\n", szKey);
		
		if(!xmlStrcmp(curNode->name, BAD_CAST"resultCode"))
		{
			ret = atoi((char*)szKey);
			xmlFree(szKey);
			xmlFreeDoc(doc);
			if(0 != ret)
				return -1;
			else
				return 0;
		}							
		xmlFree(szKey);
		curNode = curNode->next;
	}
	xmlFreeDoc(doc);
	
	return -1;                
}

//登录
static int jxj_cu_logon_to_server(JXJ_CU_LOGON_S *pLogonHdl,
									  const char* userName,
									  const char* passWord)
{
	int ret = 0;
	int xmlDataSize = 0;
	xmlChar *pXmlDataBuf = NULL;
	
	jpf_proto_head_t jpfHead = {0};
	int jpfHdLen = sizeof(jpf_proto_head_t);
	
	char sendBuf[BUF_SIZE] = {0};
	int sendLen = 0;
	char *pRecvBuf;

	xmlDocPtr doc = NULL;
	
	if(NULL == pLogonHdl || NULL == userName || NULL == passWord)
		return -1;


	/*封装cu 登录xml数据格式*/
	doc = xmlNewDoc(BAD_CAST"1.0");
	if(doc == NULL)
		return -1;
	//doc->encoding = "UTF-8";
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"message");
	xmlNewProp (root_node, BAD_CAST "type", BAD_CAST "UserLogin");

	//设置根节点
	xmlDocSetRootElement(doc,root_node);
	
	xmlNewTextChild(root_node, NULL, (xmlChar *)"userName", (xmlChar *)userName);
	xmlNewTextChild(root_node, NULL, (xmlChar *)"password", (xmlChar *)passWord);

	xmlDocDumpMemory(doc, &pXmlDataBuf, &xmlDataSize);
	if(pXmlDataBuf == NULL)
	{
		xmlFreeDoc(doc);
		return -1;
	}
	//__DBUG("xmlDataSize: %d\n", xmlDataSize);
	//__DBUG("pXmlDataBuf: %s\n", (char*)pXmlDataBuf);

	jxj_cu_make_net_head(&jpfHead, xmlDataSize);
	memcpy(sendBuf, &jpfHead, jpfHdLen);
	memcpy(sendBuf + jpfHdLen, pXmlDataBuf, xmlDataSize);
	sendLen = jpfHdLen + xmlDataSize;
	
	xmlFree(pXmlDataBuf);
	xmlFreeDoc(doc);
	if((sendLen = jxj_cu_send(pLogonHdl->sock, sendBuf, sendLen)) < 0)
	{
		__ERR("jxj_cu_logon_to_server:send failed.\n");
		return -1;
	}
	if(jxj_cu_recv_ctrl(pLogonHdl->sock, &pRecvBuf) < 0)
	{
		__ERR("jxj_cu_logon_to_server:recv failed.\n");
		return -1;
	}

	if(0 > jxj_cu_userlogin_res(pRecvBuf))
	{
		__ERR("User login response error.\n");
		ret = -1;
	}
	free(pRecvBuf);

	return ret;
}

//响应xml数据解析
static int jxj_cu_get_xml_parse(int chn, JParamId type, 
						 char *xmlData, void * pCfg, int cfgSize)
{
	int enDataLen = 0;
	int deDataLen = 0;
	int len 	  = cfgSize;
	
	char *pBuf        = NULL;
	char *pBase64Data = NULL;

	xmlDocPtr  doc       = NULL;
	xmlNodePtr node 	 = NULL;
	xmlChar   *szKeyTmp  = NULL;
	
	if(NULL == xmlData || NULL == pCfg)
		return -1;

	//__DBUG("xmlData: %s\n", xmlData);	
	doc = xmlReadMemory(xmlData, strlen(xmlData), 
			                NULL, NULL, 0);
	if(doc == NULL)
		return -1;

	//获得根节点
	node = xmlDocGetRootElement(doc);
	//指向子节点
	node = node->xmlChildrenNode;
	//__DBUG(">>>>>>>>>>>>>>jxj_cu_ntp_xml_parse\n");
	while(node != NULL)
	{
		//获取属性值
		szKeyTmp = xmlNodeGetContent(node);
		//__DBUG(">>>>>>>>>>>>>>name: %s\n", (char*)node->name);
		//__DBUG(">>>>>>>>>>>>>>szKeyTmp: %s\n", (char*)szKeyTmp);
		if(!xmlStrcmp(node->name, BAD_CAST"resultCode"))//返回值
		{
			if(0 != atoi((char*)szKeyTmp))
			{
				__ERR("resultCode failed: %d\n", atoi((char*)szKeyTmp));
				goto __FAILED;
			}
		}
		else if(!xmlStrcmp(node->name, BAD_CAST"type"))//命令码
		{
			if(type != atoi((char*)szKeyTmp))
			{
				__ERR("type failed: srcType %d, dstType %d\n", 
					type, atoi((char*)szKeyTmp));
				goto __FAILED;
			}
		}
		else if(!xmlStrcmp(node->name, BAD_CAST"channel"))//通道号
		{
			if(chn != atoi((char*)szKeyTmp))
			{
				__ERR("channel failed: srcChn %d, dstChn %d\n", 
					 chn, atoi((char*)szKeyTmp));
				goto __FAILED;
			}
		}
		else if(!xmlStrcmp(node->name, BAD_CAST"length"))//base64数据长度
		{
			if(0 >= (enDataLen = atoi((char*)szKeyTmp)))
			{
				__ERR("enDataLen failed: %d\n", enDataLen);
				goto __FAILED;
			}
		}
		else if(!xmlStrcmp(node->name, BAD_CAST"data"))//base64数据
		{
			pBase64Data = (char*)szKeyTmp;
			if(enDataLen != strlen(pBase64Data))
				goto __FAILED;
			
			pBuf = (char*)g_base64_decode(pBase64Data, (unsigned int*)&deDataLen);
			
			//__DBUG(">>>>>> deDataLen: %d\n", deDataLen);
			//__DBUG(">>>>>> len: %d\n", len);
			
			if(deDataLen != len)
			{
				__ERR("data failed: deLen %d, cfgSize %d\n", 
					 deDataLen, len);
				free(pBuf);
				goto __FAILED;
			}	
			
			memcpy(pCfg, pBuf, deDataLen);
			free(pBuf);
		}
							
		xmlFree(szKeyTmp);
		szKeyTmp = NULL;
		node = node->next;
	}
	xmlFreeDoc(doc);
	doc = NULL;
	
	return 0;

__FAILED:
	if(NULL != szKeyTmp)
		xmlFree(szKeyTmp);
	if(NULL != doc)
		xmlFreeDoc(doc);

	return -1;
}

/*报警处理默认处理函数*/
static void jxj_cu_logon_alarm_ctrl(JXJ_CU_LOGON_S *pLogonHdl, char *xmlData)
{
	int poolCh = 0;
	JSubmitAlarm jAlarm;
	int chn = 0;
	int len = 0;
	
	if(NULL == pLogonHdl || NULL == xmlData )
		return;

	memset(&jAlarm, 0 , sizeof(jAlarm));
	poolCh = *((int *)pLogonHdl->pUserData);
	len = sizeof(JSubmitAlarm);
	if(0 == jxj_cu_get_xml_parse(chn, 
								 PARAM_SUBMINT_ALARM,
								 xmlData,
								 &jAlarm,
								 len))
	{

		//__DBUG("jAlarm.action_type:%d, poolch: %d\n", jAlarm.action_type, poolCh);
		if(!jAlarm.action_type)//报警发生
		{
			pLogonHdl->alarm = 1;
			if(0 > av_pool_set_alarm_status(poolCh, pLogonHdl->alarm))
				__ERR("av pool set alarm failed.\n");
		}
		else//报警消失
		{
			pLogonHdl->alarm = 0;
			if(0 > av_pool_set_alarm_status(poolCh, pLogonHdl->alarm))
				__ERR("av pool set alarm failed.\n");
		}
		
	}
}


//获取配置
int jxj_cu_cfg_get(long logonHandle, JParamId type, void * pCfg, int cfgSize)
{
	JXJ_CU_LOGON_S *pLongHdl = (JXJ_CU_LOGON_S *)logonHandle;
	int chn = 0;
	int ret = 0;
	char buf[32];
	char *pBase64Data  = NULL;
	char *pRecvBuf;
	int len            = cfgSize;	

	xmlDocPtr  doc       = NULL;
	xmlNodePtr root_node = NULL;

	if(NULL == pLongHdl || NULL == pLongHdl->pUserData)
		return -1;

	//chn = *((int *)pLongHdl->pUserData);
	chn = 0;
	
	/*封装cu 参数获取xml数据格式*/
	//创建文档
	doc = xmlNewDoc(BAD_CAST"1.0");
	if(NULL == doc)
		return -1;

	//设置根节点
	root_node = xmlNewNode(NULL,BAD_CAST"message");
	xmlNewProp (root_node, BAD_CAST "type", BAD_CAST "GetTransparentParam");
	xmlDocSetRootElement(doc,root_node);

	//添加节点
	xml_add_node(root_node, NULL, type, "type", 'd');
	xml_add_node(root_node, NULL, chn, "channel", 'd');
	
	pBase64Data = g_base64_encode((unsigned char*)pCfg, (unsigned int)len);
	sprintf(buf, "%d", strlen(pBase64Data));
	xmlNewTextChild(root_node, NULL, (xmlChar *)"length", (xmlChar *)buf);
	xmlNewTextChild(root_node, NULL, (xmlChar *)"data", (xmlChar *)pBase64Data);
	free(pBase64Data);
	
	if(0 < jxj_cu_xml_data_send(pLongHdl, doc, &pRecvBuf))
	{
		ret = jxj_cu_get_xml_parse(chn, type,
  									 pRecvBuf + sizeof(jpf_proto_head_t),
  									 pCfg, len);
  		free(pRecvBuf);
	}
	else 
		ret = -1;
	xmlFreeDoc(doc);
	
	return ret;
}

//参数设置
int jxj_cu_cfg_set(long logonHandle, JParamId type, void * pCfg, int cfgSize)
{
	JXJ_CU_LOGON_S *pLongHdl = (JXJ_CU_LOGON_S *)logonHandle;
	char buf[BUF_SIZE] = {0};
	char *pBase64Data  = NULL;
	char *pRecvBuf;
	int ret 		= 0;
	int len            = cfgSize;	
	int chn 		   = 0;
	xmlDocPtr  doc       = NULL;
	xmlNodePtr root_node = NULL;

	if(NULL == pLongHdl || NULL == pLongHdl->pUserData)
		return -1;

	//chn = *((int *)pLongHdl->pUserData);
	chn = 0;
	
	/*封装cu 参数设置xml数据格式*/
	//创建文档
	doc = xmlNewDoc(BAD_CAST"1.0");
	if(NULL == doc)
		return -1;
	
	//设置根节点
	root_node = xmlNewNode(NULL,BAD_CAST"message");
	xmlNewProp (root_node, BAD_CAST "type", BAD_CAST "SetTransparentParam");
	xmlDocSetRootElement(doc,root_node);

	//添加节点
	xml_add_node(root_node, NULL, type, "type", 'd');
	xml_add_node(root_node, NULL, chn, "channel", 'd');
	
	//memset(buf, 0, sizeof(buf));
	//memcpy(buf, pCfg, len);
	pBase64Data = g_base64_encode((unsigned char*)pCfg, len);
	//memset(buf, 0, sizeof(buf));
	sprintf(buf, "%d", strlen(pBase64Data));
	xmlNewTextChild(root_node, NULL, (xmlChar *)"length", (xmlChar *)buf);
	xmlNewTextChild(root_node, NULL, (xmlChar *)"data", (xmlChar *)pBase64Data);
	free(pBase64Data);
	
	ret = jxj_cu_xml_data_send(pLongHdl, doc, &pRecvBuf);
	if(ret >= 0)
		free(pRecvBuf);
	xmlFreeDoc(doc);

	return ret;
}

//接收发送线程处理函数
static void* jxj_cu_thread_fun(void *data)
{
	JXJ_CU_LOGON_S *pLogonHdl = (JXJ_CU_LOGON_S *)data;
	int retVal  = -1;
	int len;
	int sendPipe = pLogonHdl->sendPipe[0];
	int sock = pLogonHdl->sock;
	int maxFds;
	int aliveCnt = 0;
	fd_set rfds;
	struct timeval tv;
	char *pRecvBuf;
	jpf_proto_head_t *pNetHdr = NULL;
	
	xmlChar *pType = NULL;
	char* xmlData = NULL;
	
	xmlDocPtr doc = NULL;
	xmlNodePtr curNode = NULL;

	char strPort[8] = {0};
	
	prctl(PR_SET_NAME, "JXJLogon");
	maxFds = ((sendPipe > sock) ? sendPipe : sock) + 1;
		
	while(!pLogonHdl->quit)
	{
		//重新登录
		if(pLogonHdl->sock < 0)
		{
			memset(strPort, 0, sizeof(strPort));
			sprintf(strPort, "%d", pLogonHdl->port);
			//__DBUG("retry Logon :%s, %s.\n",pLogonHdl->ipAddr, strPort);
			pLogonHdl->sock = jxj_cu_connect_server(pLogonHdl->ipAddr, strPort);
			if(0 > pLogonHdl->sock)
			{
				__ERR("retry connect server failed: %s, %s.\n",pLogonHdl->ipAddr, strPort);
				pLogonHdl->sock = -1;
				sleep(1);
				continue;
			}
			//登录
			if(0 > jxj_cu_logon_to_server(pLogonHdl, 
										  pLogonHdl->userName, 
										  pLogonHdl->passWord))
			{
				__ERR("retry logon server failed: %s, %s.\n",pLogonHdl->ipAddr, strPort);
				net_close_socket(&pLogonHdl->sock);
				pLogonHdl->sock = -1;
				sleep(1);
				continue;
			}

			sock = pLogonHdl->sock;
		}
		
		if(aliveCnt >= 15)
		{
			if(jxj_cu_keep_alive(sock) < 0)
			{
				__ERR("Send keep alive command failed\n");
				net_close_socket(&sock);
				pLogonHdl->sock = -1;
				continue;
			}
			aliveCnt = 0;
		}
		
		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		FD_SET(sendPipe, &rfds);
		
		tv.tv_sec = 1; 
		tv.tv_usec = 0;
		retVal = select(maxFds, &rfds, NULL, NULL, &tv);
		if(retVal < 0 )
		{
			__ERR("Logon select failed\n");
			break;
		}
		else if(retVal == 0)// 超时
		{
			aliveCnt++;
			continue;
		}
		
		if(FD_ISSET(sock, &rfds))
		{
			if((len = jxj_cu_recv_ctrl(sock, &pRecvBuf)) < 0)
			{
				__ERR("Recv logon command failed\n");
				net_close_socket(&pLogonHdl->sock);
				pLogonHdl->sock = -1;
				continue;
			}
			
			pNetHdr = (jpf_proto_head_t*)pRecvBuf;
			xmlData = (char*)(pNetHdr + 1);
			//__DBUG(">>>>>>>>>>>>>>len: %d\n", len);
			//__DBUG(">>>>>>>>>>>>>>xmlData: %s\n", xmlData);

			//将xml数据转换成xml文档
			doc = xmlReadMemory(xmlData, strlen(xmlData), 
				                NULL, "UTF-8", 0);
			if(NULL != doc)
			{
				//获得根节点
				curNode = xmlDocGetRootElement(doc);
				pType = xmlGetProp(curNode, BAD_CAST "type");
				//__DBUG("pType: %s\n", pType);
				if(!xmlStrcmp(pType, BAD_CAST"UserHeartResponse"))//心跳包响应
				{
					//__DBUG(">>>>>>>>>>>>>>UserHeartResponse\n");
					free(pRecvBuf);
				}
				else if(!xmlStrcmp(pType, BAD_CAST"GetTransparentParamResponse"))//获取参数响应
				{
					//__DBUG(">>>>>>>>>>>>>>GetTransparentParamResponse, pRecvBuf:%p\n", pRecvBuf);
					if(jxj_cu_pipe_send(pLogonHdl->recvPipe[1], 
											pRecvBuf, len) < 0)
					{
						__ERR("Send respond to pipe failed.\n");
						free(pRecvBuf);
					}
				}
				else if(!xmlStrcmp(pType, BAD_CAST"SetTransparentParamResponse"))//设置参数响应
				{	
					//__DBUG(">>>>>>>>>>>>>>SetTransparentParamResponse, pRecvBuf: %p\n", pRecvBuf);
					if(jxj_cu_pipe_send(pLogonHdl->recvPipe[1], 
											pRecvBuf, len) < 0)
					{
						__ERR("Send respond to pipe failed.\n");
						free(pRecvBuf);
					}
				}
				else if(!xmlStrcmp(pType, BAD_CAST"NotifyEvent"))
				{
					//__DBUG(">>>>>>>>>>>>>>NotifyEvent\n");
					jxj_cu_logon_alarm_ctrl(pLogonHdl, xmlData);
					free(pRecvBuf);
				}
				else 
				{
					free(pRecvBuf);
				}

				if(pType)    
					xmlFree(pType);
				//释放文档内节点动态申请的内存
				xmlFreeDoc(doc);
			}
		}
		
		if(FD_ISSET(sendPipe, &rfds))
		{
			if((len = jxj_cu_pipe_recv(sendPipe, &pRecvBuf)) <= 0)
			{
				__ERR("sendPipe jxj_cu_pipe_recv failed.\n");
				break;
			}

			//__DBUG(">>>>>>>>>>>>>>sendPipe len :%d.\n", len);
			//__DBUG(">>>>>>>>>>>>>> send xml: %s\n", buf + sizeof(jpf_proto_head_t));
			if((len = jxj_cu_send(sock, pRecvBuf, len)) < 0)
			{
				__ERR(">>>>>>>>>>>>>>sendPipe jxj_cu_send failed.\n");
				free(pRecvBuf);
				break;
			}
			free(pRecvBuf);
			aliveCnt = 0;
		}
		
	}//while end

	return NULL;
}

long jxj_cu_logon(const char* pIpAddr, const char* pUsr, const char* pPwd, const char* pPort, 
				     fAlarmCb cbAlarmFun, void* pUserData)
{
	JXJ_CU_LOGON_S *pLogonHdl = NULL;
	
	if(NULL == pIpAddr ||
	   NULL == pUsr	   ||
	   NULL == pPwd	   ||
	   NULL == pPort)
	   return 0;

	pLogonHdl = malloc(sizeof(JXJ_CU_LOGON_S));
	if(NULL == pLogonHdl)
		return 0;

	memset(pLogonHdl, 0, sizeof(JXJ_CU_LOGON_S));

	//管道创建
	if(pipe(pLogonHdl->sendPipe) < 0 || pipe(pLogonHdl->recvPipe) < 0)
	{
		__ERR("Create pipe failed.\n");
		goto _FAILED;
	}

	strcpy(pLogonHdl->ipAddr, pIpAddr);
	strcpy(pLogonHdl->userName, pUsr);
	strcpy(pLogonHdl->passWord, pPwd);
	pLogonHdl->port = atoi(pPort);
	pLogonHdl->pUserData = pUserData;
	
	//连接
	pLogonHdl->sock = jxj_cu_connect_server(pIpAddr, pPort);
	if(0 > pLogonHdl->sock)
	{
		__ERR("connect server failed.\n");
		goto _FAILED;
	}
	//登录
	if(0 > jxj_cu_logon_to_server(pLogonHdl, pUsr, pPwd))
	{
		__ERR("logon server failed.\n");
		goto _FAILED;
	}
	//创建接收线程
	if(0 != pthread_create(&pLogonHdl->pid, NULL, (void*)jxj_cu_thread_fun, pLogonHdl))	
	{
		__ERR("pthread create failed\n");
		goto _FAILED;
	}
		
	return (long)pLogonHdl;

_FAILED:
	if(0 < pLogonHdl->sendPipe[0])
		close(pLogonHdl->sendPipe[0]);
	if(0 < pLogonHdl->sendPipe[1])
		close(pLogonHdl->sendPipe[1]);
	if(0 < pLogonHdl->recvPipe[0])
		close(pLogonHdl->recvPipe[0]);
	if(0 < pLogonHdl->recvPipe[1] )
		close(pLogonHdl->recvPipe[1]);
	if(0 < pLogonHdl->sock)
		net_close_socket(&pLogonHdl->sock);
	if(NULL != pLogonHdl)
		free(pLogonHdl);

	return 0;
}

int jxj_cu_logout(long logonHandle)
{
	JXJ_CU_LOGON_S *pLogonHdl = (JXJ_CU_LOGON_S *)logonHandle;
	
	if(pLogonHdl == NULL)
		return -1;
	pLogonHdl->quit = 1;
	pthread_join(pLogonHdl->pid, NULL);
	close(pLogonHdl->sendPipe[0]);
	close(pLogonHdl->sendPipe[1]);
	close(pLogonHdl->recvPipe[0]);
	close(pLogonHdl->recvPipe[1]);
	net_close_socket(&pLogonHdl->sock);
	free(pLogonHdl);
	return 0;
}

int jxj_cu_logon_test(const char* pDstIP, const char* pUsr, 
					  const char* pPwd, const char* pPrt)
{	
	int sck = 0;
	int xmlDataSize = 0;
	
	xmlChar *pXmlDataBuf = NULL;
	xmlDocPtr sendDoc = NULL;

	char buf[BUF_SIZE] = {0};
	int  sendLen = 0;
	char *pRecvBuf;

	jpf_proto_head_t jpfHead = {0};
	int jpfHdLen = sizeof(jpf_proto_head_t);
	
	if(NULL == pDstIP || NULL == pUsr || 
	   NULL == pPwd || NULL == pPrt)
		return -1;

	//连接
	sck = net_tcp_noblock_connect(NULL, NULL, pDstIP, pPrt, 100); 
	if(sck < 0)
	{
		//__ERR("Failed to connect the server: %d\n", nSock);
		return -1;
	}
	net_set_sock_noblock(sck);

	/*封装cu 登录xml数据格式*/
	sendDoc = xmlNewDoc(BAD_CAST"1.0");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"message");
	xmlNewProp (root_node, BAD_CAST "type", BAD_CAST "UserLogin");

	//设置根节点
	xmlDocSetRootElement(sendDoc,root_node);
	
	xmlNewTextChild(root_node, NULL, (xmlChar *)"userName", (xmlChar *)pUsr);
	xmlNewTextChild(root_node, NULL, (xmlChar *)"password", (xmlChar *)pPwd);
	
	xmlDocDumpMemory(sendDoc, &pXmlDataBuf, &xmlDataSize);
	if(pXmlDataBuf == NULL)
	{
		__ERR("Dump xml to memory failed.\n");
		xmlFreeDoc(sendDoc);
		net_close_socket(&sck);
		return -1;
	}
	//__DBUG("xmlDataSize: %d\n", xmlDataSize);
	//__DBUG("pXmlDataBuf: %s\n", (char*)pXmlDataBuf);
	//cu头
	jxj_cu_make_net_head(&jpfHead, xmlDataSize);
	memcpy(buf, &jpfHead, jpfHdLen);
	memcpy(buf + jpfHdLen, pXmlDataBuf, xmlDataSize);
	sendLen = jpfHdLen + xmlDataSize;
	
	free(pXmlDataBuf);
	xmlFreeDoc(sendDoc);
	
	//发送登录数据包
	if((sendLen = jxj_cu_send(sck, buf, sendLen)) < 0)
	{
		__ERR("jxj cu logon test failed.\n");
		net_close_socket(&sck);
		return -1;
	}
	//接收响应登录数据包
	if(jxj_cu_recv_ctrl(sck, &pRecvBuf) < 0)
	{
		__ERR("jxj cu logon test failed.\n");
		net_close_socket(&sck);
		return -1;
	}

	if(0 > jxj_cu_userlogin_res(pRecvBuf))
	{
		free(pRecvBuf);
		net_close_socket(&sck);
		return -1;
	}
	free(pRecvBuf);
	
	net_close_socket(&sck);
	
	return 0;
}

