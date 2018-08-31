/******************************************************************************                  版权所有 (C), 2011-2015, 佳信捷电子 ******************************************************************************  文 件 名   : hi_rtsp_session.c  版 本 号   : v2.0  作    者   : 梁锦明  生成日期   : 2011年9月8日  最近修改   :  功能描述   : rtsp 会话管理  函数列表   :              check_rtsp_session_state              check_rtsp_sock              random_id              rtsp_check_chn              rtsp_check_usr              rtsp_fill_audio_sdp              rtsp_fill_video_sdp              rtsp_get_chn              rtsp_handle_describe              rtsp_handle_event              rtsp_handle_options              rtsp_handle_play              rtsp_handle_setup              rtsp_handle_teardown              rtsp_list_client              rtsp_make_resp_header              rtsp_session_close              rtsp_session_create              rtsp_session_lists_add              rtsp_session_lists_remove              rtsp_session_process  修改历史   :  1.日    期   : 2011年9月8日    作    者   : 梁锦明    修改内容   : 创建文件******************************************************************************//*----------------------------------------------* * 包含头文件                                   * *----------------------------------------------*/#include <stdlib.h>#include <stdio.h>#include <string.h>#include <pthread.h>#include <unistd.h>#include <ctype.h>#include "hi_rtsp_debug.h"#include "hi_rtsp_global.h"#include "hi_rtsp_session.h"#include "hi_rtsp_parse.h"#include "hi_rtsp_transfer.h"#include "hi_rtsp_rtp.h"#include "net_api.h"#include "hi_character_encode.h"/*----------------------------------------------* * 外部变量说明                                 * *----------------------------------------------*//*----------------------------------------------* * 外部函数原型说明                             * *----------------------------------------------*//*----------------------------------------------* * 内部函数原型说明                             * *----------------------------------------------*//*----------------------------------------------* * 全局变量                                     * *----------------------------------------------*//*----------------------------------------------* * 模块级变量                                   * *----------------------------------------------*//*----------------------------------------------* * 常量定义                                     * *----------------------------------------------*//*----------------------------------------------* * 宏定义                                       * *----------------------------------------------*//*----------------------------------------------* * 函数实现                                     * *----------------------------------------------*/
/***********************************************************************************
*Function Name   :random_id
*Description     :产生随机rtsp会话编号
*Input           :
*Output          :
*Return          :
				 :
*Other           :
*Author          :hi
***********************************************************************************/

void random_id( char *dest, int len )
{
	int i;

	for( i = 0; i < len ; ++i )
	{
        dest[i] =  (char)( ( random() % 10 ) + '0');
	}
	dest[len] = 0;
}

/*return chn number*/
int rtsp_get_chn(const char *pChn, int *nCh, int *nStream)
{	int nChNo = 0;	int nStreamNo = 0;	if(pChn == NULL)		return(-1);	if(sscanf(pChn, "ch%d_%d.h264", &nChNo, &nStreamNo) != 2)	{		return -1;	}	if(nChNo < 0 || nChNo >= RTSP_MAX_CHN || nStreamNo < 0 || nStreamNo >= RTSP_MAX_STREAM_TYPE)	{		return(-1);	}	*nCh = nChNo;	*nStream = nStreamNo;	return 0;}

/***********************************************************************************
*Function Name   :rtsp_check_chn()
*Description     :检查编码通道号 合法范围0~RTSP_MAX_CHN
*Input           :
*Output          :
*Return          :
*Athor           :hi
*Other           :
***********************************************************************************/
int rtsp_check_chn(int chn)
{
	int nSessCnt = 0,i = 0;
	
	for (i = 0; i < RTSP_MAX_CHN; ++i)
	{
		nSessCnt += gRtspSvr.clientCnt[i];
	}

	if((chn >= 0 && chn < RTSP_MAX_CHN) && (nSessCnt < RTSP_MAX_CLIENT))
	{
		return(1);
	}

	return 0;
}
   int rtsp_get_dianxin_chn(RTSP_SESSION_S *pSess, const char* pChn)   {	char url[128];	char channelno[64];	char streamtype[64];	char devid[64];	char hashtoken[64];	char *p1, *p2;	int nChNo, nStreamNo = 0;	int i;			if(pChn == NULL)		return -1;	printf("real rtsp path string: %s\n", pChn);	if(strstr(pChn, "realplay") == NULL		|| strstr(pChn, "channelno") == NULL		|| strstr(pChn, "streamtype") == NULL)		return -1;	strncpy(url, pChn, sizeof(url)-1);	p1 = strstr(url, "devid=");	if(p1 == NULL)		return -1;	p2 = strchr(p1, '&');	if(p2 == NULL)		return -1;	*p2++ = 0;	strcpy(devid, p1 + strlen("devid="));	if(strlen(devid) == 0)		return -1;	p1 = strchr(p2, '=');	p2 = strchr(p1, '&');	*p2++ = 0;	strcpy(channelno, p1 + 1);	if(strlen(channelno) == 0)		return -1;
	p1 = strchr(p2, '=');	p2 = strchr(p1, '&');	*p2++ = 0;	strcpy(streamtype, p1 + 1);	if(strlen(streamtype) == 0)		return -1;	p1 = strchr(p2, '=');//	p2 = strchr(p1, '&');//	*p2++ = 0;	strcpy(hashtoken, p1 + 1);	if(strlen(hashtoken) == 0)		return -1;	nChNo = atoi(channelno);	nStreamNo = atoi(streamtype);	printf(">>>>>>>>>the devid is %s!!\r\n", devid);	printf(">>>>>>>>>the channelno is %s!!\r\n", channelno);	printf(">>>>>>>>>the streamtype is %s!!\r\n", streamtype);	printf(">>>>>>>>>the hashtoken is %s!!\r\n", hashtoken);	nStreamNo = ((--nStreamNo) <= 0) ? 0 : 1;		if (nChNo <= 0 || nChNo > 4)		//return -1;		nChNo = 1;		if(rtsp_check_chn(nStreamNo) != 1)		return -1;			gGetVideoVarify(devid, channelno, streamtype, hashtoken);		for(i= 0; i< 200; i++)	{		usleep(20000);		if(gGetVideoFlag() > 0)			break;	}		if(gGetVideoFlag() != 1)	{		return -2;	}		return nChNo - 1;   }  /*return chn filename*/int rtsp_get_vod_filename(const char* pChn, char *filename){	char url[128] = {0};	char channelno[64];	char name[64];	char devid[64];	char hashtoken[64];	char *p1, *p2;	int i;		if(pChn == NULL || filename == NULL)		return -1;	printf("filename rtsp path string: %s\n", pChn);	if(strstr(pChn, "realplay") == NULL		|| strstr(pChn, "devid") == NULL		|| strstr(pChn, "channelno") == NULL		|| strstr(pChn, "filename") == NULL		|| strstr(pChn, "hashtoken") == NULL)		return -1;	//if(sscanf(pChn, "realplay?devid=%s&channelno=%d&filename=%s&hashtoken=%s",	//		devid, &channelno, filename, hashtoken) != 4)	//	return -1;	strncpy(url, pChn, sizeof(url)-1);	p1 = strstr(url, "devid=");	if(p1 == NULL)		return -1;	p2 = strchr(p1, '&');	if(p2 == NULL)		return -1;	*p2++ = 0;	strcpy(devid, p1 + strlen("devid="));	p1 = strchr(p2, '=');	p2 = strchr(p1, '&');	*p2++ = 0;	strcpy(channelno, p1 + 1);	if(strlen(channelno) == 0)		return -1;	p1 = strchr(p2, '=');	p2 = strchr(p1, '&');	*p2++ = 0;	strcpy(name, p1 + 1);	if(strlen(name) == 0)		return -1;	p1 = strchr(p2, '=');//	p2 = strchr(p1, '&');//	*p2++ = 0;	strcpy(hashtoken, p1 + 1);	if(strlen(hashtoken) == 0)		return -1;		printf("channelno: %s\n", channelno);	printf("filename: %s\n", name);	if(rtsp_check_chn(atoi(channelno)) != 1)		return -1;	gGetVodVarify(devid, channelno, name, hashtoken);	printf(">>>>>>>>>the devid is %s!!\r\n", devid);	printf(">>>>>>>>>the channelno is %s!!\r\n", channelno);	printf(">>>>>>>>>the name is %s!!\r\n", name);	printf(">>>>>>>>>the hashtoken is %s!!\r\n", hashtoken);	for(i= 0; i< 100; i++)	{		usleep(20000);		if(gGetVodFlag() > 0)			break;	}	if(gGetVodFlag() != 1)	{		return -2;	}	strcpy(filename, name);	return atoi(channelno);}/*   int rtsp_get_begin_time(const char *pFileName, const char *pBeginTime)   {	int nBeginTime, nEndTime;	int year, mon, day, hour, min, sec;	int bFind = 0;	struct tm stm = {0};	char szFileName[128] = {0};	char szBeginTime[64];	char *p1, *p2;	   	if(pFileName == NULL || pBeginTime == NULL)   		return -1;	printf("begin time: %s\n", pFileName);	if(strstr(pFileName, "mp4") == NULL)		return -1;	if(pFileName == NULL || strlen(pFileName) < 14)		return -1;	strncpy(szFileName, pFileName, sizeof(szFileName)-1);	p1 = strrchr(szFileName, '_');	if(p1 == NULL)		return -1;	p2 = strrchr(szFileName, '.');	if(p2 == NULL)		return -1;	*p2 = 0;	strcpy(szBeginTime, p1+1);	for(i = 0; i < 14; i++)	{		if(!isdigit(szBeginTime[i]))			return -1;	}	if(sscanf(szBeginTime, "%04d%02d%02d%02d%02d%02d", 			&year, &mon, &day, &hour, &min, &sec) != 6)	{		printf("The record fragment for rtsp is wrong.\n");		return -1;	}	printf("###################### Begin time:  %d-%d-%d %d:%d:%d\n", year, mon, day, hour, min, sec);	stm.tm_year = year - 1900;	stm.tm_mon = (mon - 1) % 12;	stm.tm_mday = day % 32;	stm.tm_hour = hour % 24;	stm.tm_min = min % 60;	stm.tm_sec = sec % 60;	nBeginTime = (int)mktime(&stm);	nEndTime = nBeginTime + 3600;	// 搜索一个小时内的录像		   	return 0;   }*/    int rtsp_get_play_scale(const char *pRecvBuf)  {  	const char *p;  	float scale = 0.0;	if(pRecvBuf == NULL)		return -1;	printf("get play scale: %s\n", pRecvBuf);  	p = strstr(pRecvBuf, "Scale");  	if(p == NULL)  		return -1;  	if(sscanf(p, "Scale: %f\r\n", &scale) != 1)  	{		printf("get scale faileddddddddddddddddddddd\n");  		return -1;	}	printf("################## get scale: %f\n", scale);  	return (int)(scale * 1000);  }  /* Range: npt=37.640- */  int rtsp_get_seek_time(const char *pRecvBuf)  {  	const char *p;  	float t = 0.0;	if(pRecvBuf == NULL)		return -1;	printf("get play scale: %s\n", pRecvBuf);  	p = strstr(pRecvBuf, "Range");  	if(p == NULL)  		return -1;  	if(sscanf(p, "Range: npt=%f-\r\n", &t) != 1)  	{		printf("get time faileddddddddddddddddddddd\n");  		return -1;	}	return (int)t;  }  /***********************************************************************************
*Function Name   :rtsp_session_create()
*Description     :创建一个rtsp session
*Input           :
*Output          :
*Return          :
*Athor           :hi
*Other           :
***********************************************************************************/
RTSP_SESSION_S *rtsp_session_create(int nSock)
{
    RTSP_SESSION_S *pSess = NULL;
	 static int nBasePort  = 19840;	 int        nRtpSock[4]= {0};	 int        nTryTimes  = 0;	 int        nIndex     = 0;	 
    if (0 == nSock)
    {
        RTSP_PRINT("Cann't create new rtsp session.\n");
        return NULL;
    }
    
    if (NULL == (pSess = (RTSP_SESSION_S *)malloc(sizeof(RTSP_SESSION_S ))) )
    {
        RTSP_PRINT("Create session failed");
        return NULL;
    }

    memset(pSess, 0 , sizeof(RTSP_SESSION_S));	pthread_mutex_init(&pSess->mutex, NULL);
    pSess->nRtspSocket = nSock;
    pSess->nChId       = -1;
	 pSess->eState      = RTSP_STATE_INIT;
	 check_rtsp_sock(nSock, pSess);
	 while (nTryTimes++ < 10)	 {	 	for (nIndex = 0; nIndex < 4; ++nIndex)	 	{	 					if ((nRtpSock[nIndex] = net_create_sock(2)) < 0 ||				net_bind_sock(nRtpSock[nIndex], INADDR_ANY, nBasePort+nIndex) < 0)			{				net_close_socket(&nRtpSock[nIndex]);				net_close_socket(&pSess->stRtpSender.nRtpSock[0]);				net_close_socket(&pSess->stRtpSender.nRtcpSock[0]);				net_close_socket(&pSess->stRtpSender.nRtpSock[1]);				net_close_socket(&pSess->stRtpSender.nRtcpSock[1]);								break;			}			else			{				RTSP_PRINT("create udp port index %d src port %d\n",					nIndex, nBasePort+nIndex);				if (nIndex % 2 == 0)				{					pSess->stRtpSender.nRtpPort[nIndex/2]  = nBasePort+nIndex;					pSess->stRtpSender.nRtpSock[nIndex/2]  = nRtpSock[nIndex];				}				else				{					pSess->stRtpSender.nRtcpPort[nIndex/2]  = nBasePort+nIndex;					pSess->stRtpSender.nRtcpSock[nIndex/2]  = nRtpSock[nIndex];				}			}	 	} 		nBasePort += 4; 		break;	 }		 	 	 pSess->stRtpSender.nAudioSsrc   = RTP_DEFAULT_SSRC + nBasePort - 19840;	 pSess->stRtpSender.nVideoSsrc   = RTP_DEFAULT_SSRC + 1024 + nBasePort - 19840;
	 random_id(pSess->szId, 8);
    RTSP_PRINT("create new rtsp session. remote host: %s:%d, session id: %s, socket:%d\n",
                pSess->szRemoteIP, pSess->u16RemotePort, pSess->szId, nSock);
    rtsp_session_lists_add(pSess);

    return pSess;
}

/***********************************************************************************
*Function Name   :RTSP_SessLists_Add()
*Description     :加入rtsp session 到rtsp会话链表
*Input           :
*Output          :
*Return          :
*Athor           :hi
*Other           :
***********************************************************************************/
void rtsp_session_lists_add(RTSP_SESSION_S *pSess)
{
    if (NULL != pSess)
    {
		/*进入临界区，预防其他线程正在用rtsp session的的时候，增加rtsp session*/
		pthread_mutex_lock (&gRtspMutex);
        list_add_tail(&pSess->list, &gRtspSvr.sessList.list);
		pthread_mutex_unlock (&gRtspMutex);
    }
}

/***********************************************************************************
*Function Name   :rtsp_session_lists_remove()
*Description     :从rtsp会话链表删除rtsp session 
*Input           :
*Output          :
*Return          :
*Athor           :hi
*Other           :
***********************************************************************************/
void rtsp_session_lists_remove(RTSP_SESSION_S* pSess)
{
    if (NULL != pSess)
    {
		/*进入临界区，预防其他线程正在用rtsp session的的时候，释放rtsp session*/
		pthread_mutex_lock (&gRtspMutex);
		RTSP_PRINT("rtsp remove sid %s chn:%d,client:%s\n", pSess->szId, pSess->nChId, pSess->szRemoteIP);
		/*关闭socket套接字*/
		net_close_socket(&pSess->nRtspSocket);		net_close_socket(&pSess->stRtpSender.nRtpSock[0]);		net_close_socket(&pSess->stRtpSender.nRtcpSock[0]);		net_close_socket(&pSess->stRtpSender.nRtpSock[1]);		net_close_socket(&pSess->stRtpSender.nRtcpSock[1]);				if (pSess->stRtpSender.pFrameBuf != NULL)		{			free(pSess->stRtpSender.pFrameBuf);			pSess->stRtpSender.pFrameBuf = NULL;		}		
		list_del(&pSess->list);		pthread_mutex_destroy(&pSess->mutex);
		free(pSess);
		pSess = NULL;

		pthread_mutex_unlock(&gRtspMutex);
    }
}

/***********************************************************************************
*Function Name   :rtsp_session_close()
*Description     :关闭指定的 rtsp session
*Input           :
*Output          :
*Return          :
*Athor           :hi
*Other           :
***********************************************************************************/
unsigned long rtsp_session_close(RTSP_SESSION_S* pSess)
{	int chno;	if(pSess == NULL)		return 0;	RTSP_PRINT("rtsp sid %s  close\n", pSess->szId);	if(pSess->eState < RTSP_STATE_PLAY)	{		/*rtsp会话在stup之前失败，此时并没有加入rtp传输会话，直接从session列表中删除*/		rtsp_session_lists_remove(pSess);		return 0;	}	/*rtsp 会话计数减一*/	gRtspSvr.clientCnt[pSess->nChId]--;	chno = pSess->nChId;	RTSP_PRINT("rtsp sid %s  TCP close\n", pSess->szId);	/*通知rtp_tcp传输线程, 结束*/	pSess->eState = RTSP_STATE_STOP;	/*等待rtp_tcp 线程结束之后，才释放pSess*/	/*	while(0 != pSess->sessThd)	{		RTSP_PRINT("rtsp sid %s  wait tcp thread close\n", pSess->szId);		usleep(10000);	}	*/	RTSP_PRINT("%d\n", (int)pSess->sessThd);	pthread_join(pSess->sessThd, NULL);	if(gMsgCb && pSess->nLink != 0)	{		pSess->nLink = 0;		gMsgCb(RTSP_MSG_PREVIEW_UNLINK, pSess->nChId, pSess->nStreamId, &pSess->lPriData);	}	/*安全地将session从列表中删除,释放pSess所有资源*/	rtsp_session_lists_remove(pSess);	(void)chno;	return 0;
}

/***********************************************************************************
*Function Name   :rtsp_list_client()
*Description     :显示rtsp session状态
*Input           :
*Output          :
*Return          :
*Athor           :hi
*Other           :
***********************************************************************************/
void rtsp_list_client()
{
	RTSP_SESSION_S *pSess;
	struct list_head *pSessPoss, *pSessN;
    int iCnt = 0;

    RTSP_PRINT("RTSP Client Infomation\n");
    for(iCnt = 0; iCnt < RTSP_MAX_CHN ; iCnt ++)
    {
        RTSP_PRINT("chn:%d RTSP Client Count: %ld\n", iCnt, gRtspSvr.clientCnt[iCnt]);
    }

    list_for_each_safe(pSessPoss, pSessN, &(gRtspSvr.sessList.list))
    {
        pSess = list_entry(pSessPoss, RTSP_SESSION_S, list);
        if (pSess == NULL)
        {
            RTSP_PRINT("get rtsp session entry error\n");
            continue;
        }
		else
		{
			RTSP_PRINT("sessid %s status %d\n", pSess->szId, pSess->eState);
		}
    }

    return;
}

/***********************************************************************************
*Function Name   :check_rtsp_session_state()
*Description     :检查rtsp会话状态,开心跳异常、teardown等情况
*Input           :
*Output          :
*Return          :
*Athor           :hi
*Other           :
***********************************************************************************/
void check_rtsp_session_state(void)
{
    struct list_head *pos, *q;
	RTSP_SESSION_S *pSess = NULL;

	list_for_each_safe(pos, q, &(gRtspSvr.sessList.list))
	{
		pSess = list_entry(pos, RTSP_SESSION_S , list);
        	if (NULL != pSess)
        	{
			/*心跳检查线程 将异常的pSess->sess_state赋值RTSP_STATE_STOP*/
			if(RTSP_STATE_STOP == pSess->eState)
			{
				rtsp_session_close(pSess);
				rtsp_list_client();	
			}
        	}
    } 
}


/***********************************************************************************
*Function Name   :check_rtsp_session_state()
*Description     :检查rtsp会话状态,开心跳异常、teardown等情况
*Input           :
*Output          :
*Return          :
*Athor           :hi
*Other           :
***********************************************************************************/
void check_rtsp_sock(int fd, RTSP_SESSION_S *pSess)
{
    struct sockaddr_in	cli_addr; 
    struct sockaddr_in	host_addr;
	int					len_inet;
	unsigned  long		l_subnet = 0;
	unsigned  long		r_subnet = 0;
    unsigned  char		*p = NULL;

	/*获取远端IP*/
	len_inet = sizeof(cli_addr);   /*len_inet务必赋于值sizeof(cli_addr)*/
	if( getpeername(fd, (struct sockaddr*)&cli_addr, (socklen_t *)&len_inet) < 0 )
	{
	    RTSP_PRINT("getpeername error\n");
		return ;
	}

	r_subnet = cli_addr.sin_addr.s_addr;
	p = (unsigned char *)&(r_subnet);
	RTSP_PRINT("Remote IP:%d.%d.%d.%d\n", p[0],p[1],p[2],p[3]);

    strncpy(pSess->szRemoteIP, inet_ntoa(cli_addr.sin_addr), 64);
	pSess->u16RemotePort = (unsigned short)ntohs(cli_addr.sin_port);

	/*获取本地IP*/
	len_inet = sizeof(host_addr);	/*len_inet务必赋于值sizeof(host_addr)*/
	if( getsockname(fd, (struct sockaddr*)&host_addr, (socklen_t *)&len_inet) < 0 )
	{
	    RTSP_PRINT("getsockname error\n");
		return ;
	}
	l_subnet = host_addr.sin_addr.s_addr;
	p = (unsigned char *)&(l_subnet);
	RTSP_PRINT("Host IP:%d.%d.%d.%d\n",p[0],p[1],p[2],p[3]);
    strncpy(pSess->szHostIP, inet_ntoa(host_addr.sin_addr), 64);
	pSess->nHostPort = htons(host_addr.sin_port);
	/*检查rtsp session是否为同一个网段*/
	if ((l_subnet & 0x0000ffff) & (r_subnet & 0x0000ffff))
	{
		RTSP_PRINT("client is in the same lan\n");
		pSess->nLan = 1;
	}

	if (l_subnet == r_subnet)	{		printf("@@@@@@@@@@@ client is in the same device.\n");		RTSP_PRINT("client is in the same device.\n");		pSess->isInner = 1;	}	return ;
}
/***********************************************************************************
*Function Name   :rtsp_session_process()
*Description     :rtsp Session 处理的主函数
*Input           :
*Output          :
*Return          :
*Athor           :liaoshiliang
*Other           :
***********************************************************************************/
int rtsp_session_process(RTSP_SESSION_S *pSess)
{
    unsigned long opcode = 0, rep = 0;
	 unsigned short seq_num, status;
    //int			hdr_len, body_len;
	//int ret;
    if (pSess == NULL)
    {
        RTSP_PRINT("process rtsp session, by input session is null\n");
        return 0;
    }#if 0		RTSP_PRINT("\n\n==================== RECV =================\n");	RTSP_PRINT("len: %d ===> %s\n", pSess->nDataSize, pSess->szBuffData);	RTSP_PRINT("\n==================== RECV END =================\n\n");	
    /*RTSP消息合法性分析*/
	ret = rtsp_full_msg_parse(pSess, &hdr_len, &body_len);
	printf("@@@@@@@@@@@@@@@@@@@ ret: %d, hdr_len : %d, body_len: %d\n", ret, hdr_len, body_len);	 if(RTSP_NOT_FULL == ret)	 {
		RTSP_PRINT("The receive message is not full\n");		 return 0;
	 }
	else if(RTSP_INTERLVD_RCVD == ret)	{		pSess->nMsgSize = hdr_len + body_len;		pSess->nDataSize -= pSess->nMsgSize;		return 0;	}	pSess->nMsgSize = hdr_len + body_len;	pSess->nDataSize -= pSess->nMsgSize;	memset(pSess->szBuffRecv, 0, RTSP_MAX_PROTOCOL_BUFFER);	memcpy(pSess->szBuffRecv, pSess->szBuffData, pSess->nMsgSize);	if(pSess->nDataSize > 0)		memcpy(pSess->szBuffData, pSess->szBuffData + pSess->nMsgSize, pSess->nDataSize);#endif	
	RTSP_PRINT("\n\n==================== MSG =================\n");	RTSP_PRINT("len: %d ===> %s\n", pSess->nMsgSize, pSess->szBuffRecv);	RTSP_PRINT("\n==================== MSG END =================\n\n");	    /* check for response message. */
    rep = rtsp_valid_resp_msg( pSess->szBuffRecv, &seq_num, &status);
    if ( rep != RTSP_PARSE_IS_RESP )
    {  
        /* not a response message, check for method request. */
        opcode = rtsp_valid_req( pSess->szBuffRecv );
        if ( opcode == RTSP_PARSE_INVALID_OPCODE)
        {
            RTSP_PRINT("Method requested was invalid.  Message discarded.  "
                   "Method = %s\n", pSess->szBuffRecv );
            return ERR_RTSP_PARSE_ERROR;
        }
        else if ( opcode == RTSP_PARSE_INVALID)
        {
             RTSP_PRINT("Bad request line encountered.  Expected 4 valid "
                   "tokens.  Message discarded" );
            return ERR_RTSP_PARSE_ERROR;
        }
        int iCseq = rtsp_get_cseq(pSess->szBuffRecv);
        if (iCseq > 0 )
        {
            pSess->u32LastRecvSeq = iCseq;
            RTSP_PRINT("Cseq: %d\n", iCseq);
        }
        else
        {
            RTSP_PRINT("invalid cseq\n");
        }
        
        status = 0;
    }
    else if ( rep == RTSP_PARSE_IS_RESP )
    {  
        /* response message was valid.  Derive opcode from request- */
        /* response pairing to determine what actions to take. */
        if (seq_num != (pSess->u32LastSndSeq + 1))
        {
           RTSP_PRINT("Last requse send by me sn is: %ld, but resp seq is %d\n", 					pSess->u32LastSndSeq, seq_num );
        }
        
        opcode = RTSP_MAKE_RESP_CMD(pSess->u32LastSndReq);
        
        if ( status > RTSP_BAD_STATUS_BEGIN )
        {
            RTSP_PRINT("Response had status = %d\n", status );
        }
    }
    else
    {
         /* response message was not valid, ignore it. */
    }
    return  rtsp_handle_event(pSess,  opcode, status, pSess->szBuffRecv );
}

//=====================================================================
unsigned long rtsp_make_resp_header(RTSP_SESSION_S *pSess, int err)
{
    memset(pSess->szBuffSend, 0, RTSP_MAX_PROTOCOL_BUFFER);
    
    char *pTmp = pSess->szBuffSend;

    pTmp += sprintf(pTmp, "%s %d %s\r\n", RTSP_VER_STR, err, rtsp_get_status_str( err ));
    pTmp += sprintf(pTmp,"CSeq: %ld\r\n", pSess->u32LastRecvSeq);
    pTmp += sprintf(pTmp,"Server: XiLian Rtsp Server \r\n");

    return (strlen(pSess->szBuffSend)) ;
}


/*
C -- > S
OPTIONS rtsp://192.168.86.252:554/3 RTSP/1.0
CSeq: 141
User-Agent: VLC media player (LIVE555 Streaming Media v2008.07.24)

S --> C
RTSP/1.0 200 OK
CSeq: 141
Server: XiLian Rtsp Server V100R001
Public: DESCRIBE, SETUP, TEARDOWN, PLAY
*/
unsigned long rtsp_handle_options(RTSP_SESSION_S *pSess )
{	int i;	//char szUrl[256] = {0}, szSvr[128] = {0}, szChn[16] = {0};	//int     nCh   = 0, nPort = 0;	RTSP_PRINT("RTSP_Handle_Options\n");	if(strstr(pSess->szBuffRecv, "rtsp://localhost") != NULL)	{		char *p;		int newSock;				p = strstr(pSess->szBuffRecv, "Socket");		if(p == NULL)		{			rtsp_send_reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);			return -1;		}				if(sscanf(p, "Socket=%d;TurnServerIp=%[0-9.];TurnServerPort=%d;RemoteIp=%[0-9.];RemotePort=%u",			&newSock, pSess->turnServerIp, &pSess->turnServerPort, pSess->szRemoteIP, &pSess->u16RemotePort) != 5)		{			printf("Get turn server info and remote info failed.\n");			rtsp_send_reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);			return -1;		}		printf("socket: %d turnserverip %s:%d, remote ip %s:%d\n", newSock, pSess->turnServerIp, pSess->turnServerPort,					pSess->szRemoteIP, pSess->u16RemotePort);		i = rtsp_make_resp_header(pSess, 200);		rtsp_send_reply(pSess, 200, NULL, 0);					net_close_socket(&pSess->nRtspSocket);		pSess->nRtspSocket = newSock;		check_rtsp_sock(newSock, pSess);		printf(">>>>>>>>>>111111111>>>the ip is %s\r\n", pSess->remoteIp);		return 0;	}	else	{	///////////////////////////////////////////////////	// dian xin 	///////////////////////////////////////////////////////		i = rtsp_make_resp_header(pSess, 200);		sprintf(pSess->szBuffSend+ i , "Public: DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n");		rtsp_send_reply(pSess, 200, NULL, 0);	}		return 0;
}

/*
C -- > S
DESCRIBE rtsp:admin:admin@192.168.86.252:554/3 RTSP/1.0
CSeq: 142
Accept: application/sdp
User-Agent: VLC media player (LIVE555 Streaming Media v2008.11.13)


S -- > C

RTSP/1.0 401 Unauthorized
Server: XiLian Rtsp Server 
V100R001
CSeq: 142

WWW-Authenticate: Digest realm="Surveillance Server", nonce="4e405a1c"


C -- > S

DESCRIBE rtsp:admin:admin@192.168.86.252:554 RTSP/1.0

CSeq: 143

Accept: application/sdp

Authorization: Digest username="admin", realm="Surveillance Server", nonce="4e405a1c", uri="rtsp://192.168.86.103:554", response="9e6bc65e2367f64036c8f05d8ec5f35b"

User-Agent: VLC media player (LIVE555 Streaming Media v2008.11.13)

S -- > C
RTSP/1.0 200 OK
CSeq: 143
Server: XiLian Rtsp Server V100R001
Content-Type: application/sdp
Content-length: 364
Content-Base: rtsp://192.168.86.252/3

v=0
o=StreamingServer 3331435948 1116907222000 IN IP4 192.168.86.252
s=h264.mp4
c=IN IP4 0.0.0.0
t=0 0
a=control:*
m=video 0 RTP/AVP 96
a=control:trackID=0
a=rtpmap:96 H264/90000
a=fmtp:96 packetization-mode=1; sprop-parameter-sets=Z0LgFNoFglE=,aM4wpIA=
m=audio 0 RTP/AVP 97
a=control:trackID=1
a=rtpmap:97 G726/8000/0
a=fmtp:97 packetization-mode=1

1) 根据用户的URI, 服务器获取媒体信息， 
2) 组sdp消息包
3) 发送回复
*/
/*
	S -- > C
			
		RTSP/1.0 401 Unauthorized
			
		Server: GrandStream Rtsp Server V100R001
			
		CSeq: 142
			
		WWW-Authenticate: Digest realm="Surveillance Server", nonce="4e405a1c"
			
		*/int rtsp_check_usr(RTSP_SESSION_S *pSess, char *szUrl){
   char*   pTemp     = NULL; 	char*	pParseTmp = NULL;		char szName[32] = {0}, szLocalPwd[64] = {0}, szPwd[64] = {0}, szUri[128] = {0};
	unsigned char szMd5Pwd[64] = {0};		char szRealM[64]= {0};		strncpy(szRealM, "Surveillance Server", 20);	
	if((pParseTmp = strstr(pSess->szBuffRecv, RTSP_HDR_AUTHORIZATION)) == NULL)
	{	
			
		random_id(pSess->nonce, 8);
		pTemp = pSess->szBuffSend + rtsp_make_resp_header(pSess, 401);
 			
		pTemp += sprintf(pTemp,"WWW-Authenticate: Digest realm=\"Surveillance Server\", nonce=\"%s\"\r\n\r\n", pSess->nonce);
		strcpy(pSess->szUri, szUrl);
		rtsp_send_reply(pSess, 401, NULL, 0);
		return -1;
	}
	else
	{		if(0 == rtsp_prase_user_info(pSess->szBuffRecv, szName, szPwd, szUri))
		{
			if(strcmp(szName, "guest") != 0)
			{
				if(gCheckUserCb(szName, szLocalPwd, 1))
				{
					hi_md5_auth_build_resonse((char *)szMd5Pwd,
											RTSP_MAX_USERPASSWD_LEN,
											0,
											szName,
											szRealM,
											szLocalPwd,
											pSess->nonce,
											NULL,NULL,NULL,
											"DESCRIBE",
											szUri);
					if(0 != strcmp(szPwd, (char *)szMd5Pwd))
					{/*这里是否要弹出对话框 密码不匹配，回复401*/
						pTemp = pSess->szBuffSend + rtsp_make_resp_header(pSess, 401);
						random_id(pSess->nonce, 8);
	 					pTemp += sprintf(pTemp,"WWW-Authenticate: Digest realm=\"Surveillance Server\", nonce=\"%s\"\r\n\r\n",pSess->nonce);
						strcpy(pSess->szUri, szUrl);
						rtsp_send_reply(pSess, 401, NULL, 0);
						return -1;
					} 
				}
				else
				{ /*这里是否要弹出对话框 不存在该用户或数据库操作失败,回复401*/
					pTemp = pSess->szBuffSend + rtsp_make_resp_header(pSess, 401);	
					random_id(pSess->nonce,8);
					pTemp += sprintf(pTemp,"WWW-Authenticate: Digest realm=\"Surveillance Server\", nonce=\"%s\"\r\n\r\n",pSess->nonce);
					strcpy(pSess->szUri, szUrl);
					rtsp_send_reply(pSess, 401, NULL, 0);						
					return -1;
				
				}
			}
		}
		else
		{	/*这里是否要弹出对话框 用户或密码为空 回复401*/
			pTemp = pSess->szBuffSend + rtsp_make_resp_header(pSess, 401);
			random_id(pSess->nonce,8);
 			pTemp += sprintf(pTemp,"WWW-Authenticate: Digest realm=\"Surveillance Server\", nonce=\"%s\"\r\n\r\n",pSess->nonce);
			strcpy(pSess->szUri, szUrl);
				
			rtsp_send_reply(pSess, 401, NULL, 0);
			return -1;
		}
	}	return 0;}static int rtsp_get_map_address(RTSP_SESSION_S* pSess, int sock, int port){	RTSP_MAP_ADDRESS stMapAddr;		if(gMsgCb == NULL)		return -1;	strcpy(stMapAddr.szTurnServerIp, pSess->turnServerIp);	stMapAddr.nTurnServerPort = pSess->turnServerPort;	stMapAddr.sock = sock;	stMapAddr.innerPort = port;	if(gMsgCb(RTSP_MSG_GET_MAP_ADDRESS, pSess->nChId%gRtspSvr.nMaxCh, 			pSess->nChId/gRtspSvr.nMaxCh, (void *)&stMapAddr) < 0)	{		RTSP_PRINT("Get map address failed.\n");		return -1;	}	if(strlen(pSess->szNatIP) == 0)		strcpy(pSess->szNatIP, stMapAddr.natIp);			return stMapAddr.natPort;}extern int rtsp_get_video_media_info(int nCh, int nStream, int *pPt, char *profileLevelId, char *sps_pps, long lPriData);extern int rtsp_get_history_media_info(int nCh, int nStream, int *pPt, char *profileLevelId, char *sps_pps, void *handle);extern int rtsp_get_history_video_media_info(int nCh, int nStream, int *pPt, char *profileLevelId, char *sps_pps, void *handle);extern int rtsp_get_history_audio_media_info(int nCh, int *pPt, int *pBits, int *pSample, char *pInfo, void *handle);int rtsp_fill_video_sdp(RTSP_SESSION_S* pSess, char *pSzBuff, int nCh, int nStream, void *vodHandle){	char szProfileLevelId[8] = {0}, szSpsPps[256] = {0};	int  nPt = 0;	
    /*视频媒体描述*/
    /*H264 TrackID=0 RTP_PT 96*/#if 1	//rtsp_get_video_media_info(nCh, &nPt, szProfileLevelId, szSpsPps);#endif	if (pSess->isHistoryStream)			rtsp_get_history_video_media_info(nCh, nStream, &nPt, szProfileLevelId, szSpsPps, vodHandle);	else		rtsp_get_video_media_info(nCh, nStream, &nPt, szProfileLevelId, szSpsPps, pSess->lPriData);	printf("nPt = %d\r\n", nPt);		//printf("szProfileLevelId = %s\r\n", szProfileLevelId);	//printf("szSpsPps = %s\r\n", szSpsPps);
   pSzBuff += sprintf(pSzBuff,"m=video 0 RTP/AVP %d\r\n", nPt);	pSzBuff += sprintf(pSzBuff,"b=AS:70\r\n");
   pSzBuff += sprintf(pSzBuff,"a=control:trackID=0\r\n");
   pSzBuff += sprintf(pSzBuff,"a=rtpmap:%d %s/90000\r\n", nPt, 						(nPt == RTP_PT_H264) ? "H264" : "JPEG");	pSess->stRtpSender.nPayLoad[0] = nPt;	if (nPt == RTP_PT_H264) // ,
  		pSzBuff += sprintf(pSzBuff,"a=fmtp:96 profile-level-id=%s;packetization-mode=1; sprop-parameter-sets=%s\r\n", 
						 szProfileLevelId, szSpsPps);	printf("pSzBuff = %s\r\n", pSzBuff);	return 0;}extern int rtsp_get_audio_media_info(int nCh, int *pPt, int *pBits,                                     int *pSample, char *pInfo, long lPriData);int rtsp_fill_audio_sdp(RTSP_SESSION_S* pSess, char *pSzBuff, int nCh, void *vodHandle){	int nPt = -1, nBits = 0, nSample = 0;	char szAudioInfo[32] = {0};	if (pSess->isHistoryStream) 		rtsp_get_history_audio_media_info(nCh, &nPt, &nBits, &nSample, szAudioInfo, vodHandle);	else		rtsp_get_audio_media_info(nCh, &nPt, &nBits, &nSample, szAudioInfo, pSess->lPriData);	if (nPt != -1)	{		pSess->audioSample = nSample;		pSess->stRtpSender.nPayLoad[1] = nPt;	
	/*主码流音频媒体描述*/	
	pSzBuff += sprintf(pSzBuff,"m=audio 0 RTP/AVP %d\r\n", nPt);		pSzBuff += sprintf(pSzBuff,"b=AS:48\r\n");	
	pSzBuff += sprintf(pSzBuff,"a=control:trackID=1\r\n");	
	pSzBuff += sprintf(pSzBuff,"a=rtpmap:%d %s/%d\r\n", nPt, szAudioInfo, nSample); 	
		if (nPt == RTP_PT_AAC)			pSzBuff += sprintf(pSzBuff,"a=fmtp:%d streamtype=5;profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3;config=1290\r\n", nPt); 	}		return 0;}unsigned long rtsp_handle_describe(RTSP_SESSION_S *pSess){
   char szUrl[256] = {0}, szSvr[128] = {0}, szChn[16] = {0}, szIp[32];
	char szSdp[8192]= {0}, szVideoSdp[2048]= {0}, szAudioSdp[1024] = {0};
	int     nCh   = 0, nStream = 0, nPort = 0;
   char*   pTemp     = NULL; 	char*	pParseTmp = NULL;	
	/* sscanf之后 url = "rtsp://192.168.86.252:554/0" */
	if (!sscanf(pSess->szBuffRecv, " %*s %254s ", szUrl)) 
   {
		rtsp_send_reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);	/* bad request */
		return 0;
	}
   if (rtsp_parse_url(szUrl, szSvr, szIp, &nPort, szChn) == -1)
   {
   	RTSP_PRINT("url: %s . not found\n", szUrl);
   	rtsp_send_reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
   	return 0;
   }
	strcpy(pSess->szHostIP, szIp);	//电信回应的本地ip要求为服务器ip	pSess->nHostPort = nPort;	//printf(">>>>>>>>>>111111 the szSvr is %s\r\n", szSvr);	//printf(">>>>>>>>>>111111 the nPort is %d\r\n", nPort);	/*非法通道号*/
	if(rtsp_get_chn(szChn, &nCh, &nStream) < 0)
	{
		if((nCh = rtsp_get_dianxin_chn(pSess, szChn)) < 0)		{		  	if(nCh == -2)		 	{				rtsp_send_reply(pSess, 401, NULL, 1);		        return 0;			}	       	if((nCh = rtsp_get_vod_filename(szChn, pSess->szVodFileName)) < 0)	       	{			        rtsp_send_reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);
		        return 0;
	      	}	      	else	      	{							           	pSess->isHistoryStream = 1;	      	}	    }	}	
	/*登记通道号*/
	///////////////////////////////////    pSess->nChId = nCh;		pSess->nStreamId = nStream;		///////////////////////////////////    /*获取客户端请求媒体的描述方法 application/sdp*/
	if (strstr(pSess->szBuffRecv, RTSP_HDR_ACCEPT) != NULL) 
   {
		if (strstr(pSess->szBuffRecv, "application/sdp") == NULL) 
        {
           RTSP_PRINT("Only accept require\n");
			rtsp_send_reply(pSess, 551, NULL, 1);	/* Option not supported */
			return 0;
		}
	}
    /*先定位到User-Agent, 再定位到第一个空格, 就是: 后面, 然后已知搜索到行尾*/
    /*User-Agent: VLC media player (LIVE555 Streaming Media v2008.07.24)*/
   if ((pParseTmp = strstr(pSess->szBuffRecv, RTSP_HDR_USER_AGENT)) != NULL)
   {
        char* p = pParseTmp;
        while( (char)(*p) != 0 )
        {/*碰到结尾, 就跳出*/
            if ( (char)(*p) == 10 || (char)(*p) == 13)
            {
                break;
            }
            if ( (char)*p == ':')
            {
                pParseTmp = p+2; /*jump to "VLC...."*/
            }
            p++;
        }
        if (p != pParseTmp)
        {
            strncpy(pSess->szUserAgent, pParseTmp, p - pParseTmp);      
        }
   }
			if (gRtspSvr.nUseAuth && rtsp_check_usr(pSess, szUrl) < 0)		return 0;	if (!pSess->isHistoryStream)	{		if (gMsgCb)		{			if(gMsgCb(RTSP_MSG_PREVIEW_LINK, nCh, nStream, &pSess->lPriData) < 0)			{								rtsp_send_reply(pSess, 404, NULL, 1);	/* Option not supported */				return 0;			}		}	}	
	memset(pSess->szBuffSend, 0, sizeof(pSess->szBuffSend));
	pTemp = pSess->szBuffSend;
   pTemp += sprintf(pTemp, "%s %d %s\r\n", RTSP_VER_STR, 200, rtsp_get_status_str( 200 ));
   pTemp += sprintf(pTemp,"CSeq: %ld\r\n", pSess->u32LastRecvSeq);	
   pTemp += sprintf(pTemp,"Content-Type: application/sdp\r\n");

    //TODO 把一些固定值改为动态值
   char *pTemp2 = szSdp;
   pTemp2 += sprintf(pTemp2,"v=0\r\n");
   pTemp2 += sprintf(pTemp2,"o=StreamingServer 3331435948 1116907222000 IN IP4 %s\r\n", pSess->szHostIP);
   pTemp2 += sprintf(pTemp2,"s=h264.mp4\r\n");
   pTemp2 += sprintf(pTemp2,"c=IN IP4 %s\r\n", pSess->szHostIP);	// modify by xq
   pTemp2 += sprintf(pTemp2,"t=0 0\r\n");
   pTemp2 += sprintf(pTemp2,"a=control:*\r\n");	if(pSess->isHistoryStream)	{				void	*vodHandle;		int sec = 0;		/// 发送文件播放时间		if(gGetVodStartCb(pSess->szVodFileName, &vodHandle) >= 0)		{			sec = gGetVodProperty(vodHandle);						rtsp_fill_video_sdp(pSess, szVideoSdp, nCh, nStream, vodHandle);	  		pTemp2 += sprintf(pTemp2,"a=range:npt=0-%d\r\n", sec);	  		pTemp2 += sprintf(pTemp2,"a=recvonly\r\n");			strcat(pTemp2, szVideoSdp); 			rtsp_fill_audio_sdp(pSess, szAudioSdp, nCh, vodHandle);			strcat(pTemp2, szAudioSdp);			gGetVodStopCb(vodHandle);		}		else		{	  		pTemp2 += sprintf(pTemp2,"a=range:npt=0-%d\r\n", sec);	  		pTemp2 += sprintf(pTemp2,"a=recvonly\r\n");			strcat(pTemp2, szVideoSdp); 		}		pSess->totalSec = sec;	}	else	{		rtsp_fill_video_sdp(pSess, szVideoSdp, nCh, nStream, NULL);				strcat(pTemp2, szVideoSdp); 		rtsp_fill_audio_sdp(pSess, szAudioSdp, nCh, NULL);		strcat(pTemp2, szAudioSdp);	}	if (!pSess->isHistoryStream)	{		if(gMsgCb)			gMsgCb(RTSP_MSG_PREVIEW_UNLINK, nCh, nStream, &pSess->lPriData);		if(gMsgCb)		{			if(gMsgCb(RTSP_MSG_PREVIEW_LINK, nCh, nStream, &pSess->lPriData) < 0)			{				rtsp_send_reply(pSess, 404, NULL, 1);	/* Option not supported */				return 0;			}			pSess->nLink = 1;		}	}
   pTemp += sprintf(pTemp,"Content-length: %d\r\n", strlen(szSdp));  
	if(pSess->remotePort == 554)   	pTemp += sprintf(pTemp,"Content-Base: rtsp://%s/%d\r\n\r\n", pSess->szHostIP, nCh);
	else		pTemp += sprintf(pTemp,"Content-Base: rtsp://%s:%d/%d\r\n\r\n", pSess->szHostIP, pSess->nHostPort, nCh);   strcat(pTemp, szSdp);
   strcpy(pSess->szUri, szUrl);
   rtsp_send_reply(pSess, 200, NULL, 0);
   return 0;
}int check_ip_is_valid(char *ip){	int a = 0, b = 0, c = 0, d = 0;	char t;	if(sscanf(ip, "%d.%d.%d.%d%c", &a, &b, &c, &d, &t) == 4)	{		if(a > 0 && a <= 255 && b >=0 && b <= 255			&& c >=0 && c <= 255 && d >= 0 && d <= 255)		{			return 0;		}else		{			return -1;		}	}	return -1;}

/*
C -- > S
SETUP rtsp://192.168.86.252/3/trackID=0 RTSP/1.0
CSeq: 143
Transport: RTP/AVP;unicast;client_port=4654-4655
User-Agent: VLC media player (LIVE555 Streaming Media v2008.07.24)

S -- > C
RTSP/1.0 200 OK
CSeq: 143
Server: XiLian Rtsp Server V100R001
Session: 65693745
Transport: RTP/AVP;unicast;client_port=4654-4655;source=192.168.86.252;server_port=32781-0;ssrc=00004E87
*/
int rtsp_handle_setup(RTSP_SESSION_S *pSess )
{
    char* pTemp;
    char* p = NULL; 
    char object[255] = {0}, server[255] = {0}, trash[255] = {0}, szIp[32];
    char line[255] = {0}, url[255] = {0};
	char destip[32];	char hostip[32];    int svrport = 0, rtpport = 0, rtcpport = 0, trackid = 0;	 int chid = 0;	/* Get the URL */
	if (!sscanf(pSess->szBuffRecv, " %*s %254s ", url)) 
    {
		rtsp_send_reply(pSess, 400, 0, 1);	/* bad request */
		return -1;
	}
    url[254] = '\0';
	/* Validate the URL */
	if (rtsp_parse_url(url, server, szIp, &svrport, object) != 0) 
    {   
		rtsp_send_reply(pSess, 400, 0, 1);	/* bad request */
		return -1;
	}	RTSP_PRINT("object: %s\n", object);
    /*获取trackID*/
	if(strstr(object, "trackID")== NULL)		p=strstr(object, "track"); //上海迅垒使用的格式是track1	else		p=strstr(object, "trackID"); //标准格式是trackID=1	    if (p == NULL)
	{
		RTSP_PRINT("No track info\n");
       /*ToDo 发什么回复*/
       rtsp_send_reply(pSess, 406, "Require: Transport settings of rtp/udp;port=nnnn. ", 1);	/* Not Acceptable */
       return -1;
	}
	sscanf(p, "%*[^0-9]%i",&trackid);	sscanf(p, "%[^0-9]",trash);	//printf("trash = %s trackid = %d\r\n", trash, trackid);	if(strcmp(trash, "track") == 0)//迅垒NVR  track1	{		if(trackid == 1)			trackid = 0;  //迅垒NVR trackID = 1表示视频刚好与标准情况相反
	}	RTSP_PRINT("trash: %s, trackid: %d\n", trash, trackid);
    /*获取Transport Token*/
	if ((p = strstr(pSess->szBuffRecv, RTSP_HDR_TRANSPORT)) == NULL) 
    {
		rtsp_send_reply(pSess, 406, "Require: Transport settings of rtp/udp;port=nnnn. ", 1);	/* Not Acceptable */
		return -1;
	}
	
   chid = rtsp_parse_chid(pSess->szBuffRecv);		if (trackid == RTSP_TRACKID_VIDEO )
	{
		pSess->u8RequestStreamFlag[0] = 1;
		//pSess->nChId = chid	}
	else if (trackid == RTSP_TRACKID_AUDIO)
	{
		pSess->u8RequestStreamFlag[1] = 1;		if(chid != -1)
			pSess->nAudioChId = chid;	}
	else
	{
		RTSP_PRINT("rtsp setup trackid %d error\n", trackid);
		rtsp_send_reply(pSess, 400, NULL, 1);
		return -1;
	}

    /*    
    Transport: RTP/AVP;unicast;client_port=6972-6973;source=10.71.147.222;server_port=6970-6971;ssrc=00003654 
    trash = "Transport:"
    line = "RTP/AVP;unicast;client_port=6972-6973;source=10.71.147.222;server_port=6970-6971;ssrc=00003654"
    */
	if (sscanf(p, "%10s%255s", trash, line) != 2) 
    {
		RTSP_PRINT("SETUP request malformed\n");
		rtsp_send_reply(pSess, 400, 0, 1);	/* Bad Request */
		return -1;
	}

	/* RTP 使用UDP作传输*/
	if( (p = strstr(line, "client_port") )!= NULL)
    {
		p = strstr(p, "=");
		sscanf(p + 1, "%d", &(rtpport));
		p = strstr(p, "-");
		if(p != NULL)			sscanf(p + 1, "%d", &(rtcpport));  
		else			rtcpport = rtpport + 1;		if((p = strstr(line, "destination")) != NULL)		{
			p = strstr(p, "=");			if(sscanf(p + 1, "%[0-9.]", destip) == 1)			{				RTSP_PRINT("destip: %s\n", destip);				printf("@@@@@@@@@@ distip : %s\n", destip);				if(check_ip_is_valid(destip) == 0)					strcpy(pSess->szRemoteIP, destip);				else				{					printf("The destip format is wrong.\n");					RTSP_PRINT("the destination ip is err.\n");					rtsp_send_reply(pSess, 400, 0, 1);					return -1;				}				////////////////////////////////////////////////				//strcpy(pSess->szRemoteIP, "113.116.60.105");				//strcpy(pSess->szRemoteIP, "121.34.147.66");				///////////////////////////////////////////////			}else			{				printf(">>>>>>the destination ip is err!\r\n");				RTSP_PRINT("the destination ip is err.\n");				rtsp_send_reply(pSess, 400, 0, 1);				return -1;			}		}		//else		//{		//	// set destination ip address.		//	strcpy(pSess->szRemoteIP, "192.168.123.100");		//}				pSess->u16RemoteRTPPort[trackid]  = rtpport;
		pSess->u16RemoteRTCPPort[trackid] = rtcpport;
		pSess->eTransType = RTP_TRANSPORT_UDP;
		pTemp  = pSess->szBuffSend;
		pTemp += rtsp_make_resp_header(pSess, 200);
		//pTemp += sprintf(pTemp,"Date: 23 Jan 1997 15:35:06 GMT\r\n");
		pTemp += sprintf(pTemp,"Session: %s\r\n", pSess->szId);
		if (trackid == RTSP_TRACKID_VIDEO )
		{			if(pSess->isInner)			{				pSess->stRtpSender.nRtpNatPort[0] = rtsp_get_map_address(pSess, 													pSess->stRtpSender.nRtpSock[0],													pSess->stRtpSender.nRtpPort[0]);				if(pSess->stRtpSender.nRtpNatPort[0] < 0)				{					RTSP_PRINT("Get RTP mapping port failed.\n");					rtsp_send_reply(pSess, 400, 0, 1);					return -1;				}				pSess->stRtpSender.nRtcpNatPort[0] = rtsp_get_map_address(pSess, 													pSess->stRtpSender.nRtcpSock[0],													pSess->stRtpSender.nRtcpPort[0]);				if(pSess->stRtpSender.nRtcpNatPort[0] < 0)				{					RTSP_PRINT("Get RTCP mapping port failed.\n");					rtsp_send_reply(pSess, 400, 0, 1);					return -1;				}				rtpport = pSess->stRtpSender.nRtpNatPort[0];				rtcpport = pSess->stRtpSender.nRtcpNatPort[0];				strcpy(hostip, pSess->szNatIP);				printf(">>>>>>>the szNatIP is %s\r\n", hostip);			}			else			{				rtpport = pSess->stRtpSender.nRtpPort[0];				rtcpport = pSess->stRtpSender.nRtcpPort[0];				strcpy(hostip, pSess->szHostIP);				printf(">>>>>>>the szHostIP is %s\r\n", hostip);			}			pSess->u8RequestStreamFlag[RTSP_TRACKID_VIDEO] = 1;
			pTemp += sprintf(pTemp,"Transport: RTP/AVP;unicast;destination=%s;client_port=%d-%d;source=%s;"
								   "server_port=%d-%d;ssrc=00004E87;mode=\"play\"\r\n\r\n",
								   pSess->szRemoteIP,								   pSess->u16RemoteRTPPort[trackid], 								   pSess->u16RemoteRTCPPort[trackid], 								   hostip,								   rtpport,
								   rtcpport);
		}		else
		{			if(pSess->isInner)			{				pSess->stRtpSender.nRtpNatPort[1] = rtsp_get_map_address(pSess, 													pSess->stRtpSender.nRtpSock[1],													pSess->stRtpSender.nRtpPort[1]);
				if(pSess->stRtpSender.nRtpNatPort[1] < 0)				{					RTSP_PRINT("Get RTP mapping port failed.\n");					rtsp_send_reply(pSess, 400, 0, 1);					return -1;				}				pSess->stRtpSender.nRtcpNatPort[1] = rtsp_get_map_address(pSess, 													pSess->stRtpSender.nRtcpSock[1],													pSess->stRtpSender.nRtcpPort[1]);
				if(pSess->stRtpSender.nRtcpNatPort[1] < 0)				{					RTSP_PRINT("Get RTCP mapping port failed.\n");					rtsp_send_reply(pSess, 400, 0, 1);					return -1;				}
				rtpport = pSess->stRtpSender.nRtpNatPort[1];				rtcpport = pSess->stRtpSender.nRtcpNatPort[1];				strcpy(hostip, pSess->szNatIP);			}			else
			{				rtpport = pSess->stRtpSender.nRtpPort[1];				rtcpport = pSess->stRtpSender.nRtcpPort[1];				strcpy(hostip, pSess->szHostIP);			}			pSess->u8RequestStreamFlag[RTSP_TRACKID_AUDIO] = 1;
			pTemp += sprintf(pTemp,"Transport: RTP/AVP;unicast;destination=%s;client_port=%d-%d;source=%s;"
									   "server_port=%d-%d;ssrc=00004E87\r\n\r\n",
									   pSess->szRemoteIP,									   pSess->u16RemoteRTPPort[trackid], 								  	   pSess->u16RemoteRTCPPort[trackid], 									   hostip,									   rtpport,
	
								   rtcpport);
		}
	}
	/* RTP 使用TCP作传输*/
	else if( (p = strstr(line, "/TCP") )!= NULL)
	{
		pSess->eTransType = RTP_TRANSPORT_TCP;
		pTemp = pSess->szBuffSend;
		pTemp += rtsp_make_resp_header(pSess, 200);
		pTemp += sprintf(pTemp,"Session: %s;timeout=120\r\n", pSess->szId);

		if( (p = strstr(line, "interleaved")) != NULL)
		{
			if (trackid == RTSP_TRACKID_VIDEO )
			{
				p = strstr(p, "=");
				sscanf(p + 1, "%d", &(pSess->stInterleaved[0].RTP));
				if ( (p = strstr(p, "-")) != NULL )
				{
					sscanf(p + 1, "%d", &(pSess->stInterleaved[0].RTCP));
				}
				else
				{
					pSess->stInterleaved[0].RTCP = pSess->stInterleaved[0].RTP + 1;
				}

				pTemp += sprintf(pTemp,"Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d\r\n\r\n",\
								pSess->stInterleaved[0].RTP, \
								pSess->stInterleaved[0].RTCP);
			}
			else if(trackid == RTSP_TRACKID_AUDIO )
			{
				p = strstr(p, "=");
				sscanf(p + 1, "%d", &(pSess->stInterleaved[1].RTP));
				if ( (p = strstr(p, "-")) != NULL )
				{
					sscanf(p + 1, "%d", &(pSess->stInterleaved[1].RTCP));
				}
				else
				{
					pSess->stInterleaved[1].RTCP = pSess->stInterleaved[1].RTP + 1;
				}
				pTemp += sprintf(pTemp,"Transport: RTP/AVP/TCP;unicast;interleaved=%d-%d\r\n\r\n",\
								pSess->stInterleaved[1].RTP, \
								pSess->stInterleaved[1].RTCP);
			}
			else
			{
				RTSP_PRINT("rtsp setup trackid %d  Unsupported Transport\n", trackid);
				rtsp_send_reply(pSess, 461,  "Unsupported Transport", 1);
				return -1;
			}
		}
	}
	else
	{
		RTSP_PRINT("rtsp setup trackid %d  Unsupported Transport\n", trackid);
		rtsp_send_reply(pSess, 461,  "Unsupported Transport", 1);
		return -1;
	}

    rtsp_send_reply(pSess, 200, NULL, 0);
    return 0;
}
/*
C --> S
PLAY rtsp://192.168.86.252/3 RTSP/1.0
CSeq: 179
Session: 06261879
Range: npt=0.000-
User-Agent: VLC media player (LIVE555 Streaming Media v2008.07.24)

S --> C
RTSP/1.0 200 OK
CSeq: 179
Server: XiLian Rtsp Server V100R001

1) 准备ok
2) rtp可以开始发送数据
3) 发送回复
*/

int rtsp_handle_play(RTSP_SESSION_S *pSess)
{
	char *pTemp = NULL;	char  szUrl[256] = {0}, szSvr[128] = {0}, szChn[32] ={0}, szIp[32] = {0};	int   nCh = 0, nStream = 0, nPort = 0;	/* sscanf之后 url = "rtsp://192.168.86.252:554/0" */	if (!sscanf(pSess->szBuffRecv, " %*s %254s ", szUrl))    {		rtsp_send_reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);	/* bad request */		return 0;	}   if (-1 == rtsp_parse_url(szUrl, szSvr, szIp, &nPort, szChn))   {	   	RTSP_PRINT("url: %s . not found\n", szUrl);	   	rtsp_send_reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);	   	return 0;   }    /* 判断当前工作状态*/	if(pSess->eState == RTSP_STATE_PAUSE || pSess->eState == RTSP_STATE_PLAY)	{		int nPlayTime;		int nScale;		pTemp  = pSess->szBuffSend;		pTemp += rtsp_make_resp_header(pSess, 200);		if((nPlayTime = rtsp_get_seek_time(pSess->szBuffRecv)) > 0)		{			unsigned long u32Pts;			printf("################## seek time: %d\n", nPlayTime);			pSess->seekSec = nPlayTime;			/*			if(pSess->vodHandle != NULL && gSetVodSeek != NULL)			{			if(gSetVodSeek(pSess->vodHandle, nPlayTime) < 0)				printf("@@@@@@@@@@@@@@@@ seek failedddddddddddd\n");			else				printf("@@@@@@@@@@@@@@@@@ seek successfully.\n");			}			*/			/*			Range: npt=55-1854			RTP-Info: url=rtsp://218.3.208.30:5504/realplay?devid=811435133080&channelno=0&filename=ch01_00000000017000000&hashtoken=172ac98f32daf27f14f5a9a30c58fb32/trackID=1;seq=3103;rtptime=1620000			*/			u32Pts = pSess->stRtpSender.u32FirstSndPts[0]+90*nPlayTime*1000;			pTemp += sprintf(pTemp,"Range: npt=%d-%d\r\n", nPlayTime, pSess->totalSec);			pTemp += sprintf(pTemp,"RTP-Info: url=%s/trackID=1;seq=%u;rtptime=%lu\r\n", szUrl, 					pSess->stRtpSender.u16LastSndSeq[0], u32Pts);		}				if((nScale = rtsp_get_play_scale(pSess->szBuffRecv)) > 0)		{			printf("################## nScale:  %d\n", nScale);			pSess->scale = nScale;			if(pSess->scale > 1500)				pSess->scale = 2000;			else if(pSess->scale > 750)				pSess->scale = 1000;			else				pSess->scale = 500;		}		pTemp += sprintf(pTemp,"Session: %s;timeout=120\r\n\r\n", pSess->szId);		rtsp_send_reply(pSess, 200, NULL, 0);		pSess->eState = RTSP_STATE_PLAY;		return 0;		}    /*检查rtsp会话已经经过stup过程*/
   if(RTSP_STATE_READY != pSess->eState)
	{
		return -1;
	}	/* 如果是历史流，则打开相应的文件*/	if(pSess->isHistoryStream)	{		if(gGetVodStartCb(pSess->szVodFileName, &pSess->vodHandle) < 0)		{			rtsp_send_reply(pSess, RTSP_STATUS_BAD_REQUEST, NULL, 1);			return 0;		}	}
    
   /*增加RTP TCP传输通道*/
   if(RTP_TRANSPORT_TCP == pSess->eTransType)
	{		pSess->stRtpSender.stInterleaved[0].RTP = pSess->stInterleaved[0].RTP;
		pSess->stRtpSender.stInterleaved[0].RTCP = pSess->stInterleaved[0].RTCP;
		pSess->stRtpSender.stInterleaved[1].RTP = pSess->stInterleaved[1].RTP;
		pSess->stRtpSender.stInterleaved[1].RTCP = pSess->stInterleaved[1].RTCP;
		pSess->stRtpSender.nRtpTcpSock = pSess->nRtspSocket;
		pSess->stRtpSender.u16LastTs[0] = 3600;		/*初始的ts*/		pSess->stRtpSender.u16LastTs[1] = 3600;
		pSess->stRtpSender.nAudioSsrc = RTP_DEFAULT_SSRC + pSess->nChId;		/*音频同步源*/
		pSess->stRtpSender.nVideoSsrc = RTP_DEFAULT_SSRC + pSess->nChId + 128;	/*视频同步源*/
	}
   else
	{
	   /*增加RTP UDP传输通道*/		pSess->stRtpSender.addrRemote[0].sin_family  = AF_INET;		pSess->stRtpSender.addrRemote[1].sin_family  = AF_INET;		pSess->stRtpSender.addrRemote[0].sin_port    = htons(pSess->u16RemoteRTPPort[0]);		pSess->stRtpSender.addrRemote[1].sin_port    = htons(pSess->u16RemoteRTPPort[1]);				pSess->stRtpSender.addrRemote[0].sin_addr.s_addr = inet_addr(pSess->szRemoteIP); 		pSess->stRtpSender.addrRemote[1].sin_addr.s_addr = inet_addr(pSess->szRemoteIP); 
	}
   pSess->eState = RTSP_STATE_PLAY;
	pSess->scale = 1000;		/* 正常速度播放*/	pTemp  = pSess->szBuffSend;
	pTemp += rtsp_make_resp_header(pSess, 200);
	pTemp += sprintf(pTemp,"Session: %s;timeout=120\r\n", pSess->szId);	if(pSess->isHistoryStream)	{		int sec = 0;		/// 发送文件播放时间		printf("play vodFilename: %s\n", pSess->szVodFileName);		sec = pSess->totalSec;		printf("play the vod length sec %d\n", sec);				pTemp += sprintf(pTemp,"Range: npt=0-%d\r\n", sec);		pTemp += sprintf(pTemp,"RTP-Info: url=%s/trackID=1;seq=0;rtptime=0\r\n", szUrl);	}	pTemp += sprintf(pTemp,"\r\n");	rtsp_send_reply(pSess, 200, NULL, 0);#if 0
// 不再等待client的穿透包	if(RTP_TRANSPORT_UDP == pSess->eTransType)	{		char buff[16];		struct sockaddr_in addr;		int nAddrLen = sizeof(addr);		//printf("Rtp send to %s:%d\n", inet_ntoa(pSess->stRtpSender.addrRemote[0].sin_addr), 		//						ntohs(pSess->stRtpSender.addrRemote[0].sin_port));		//hi_udp_send(pSess->stRtpSender.nRtpSock[0], buff, sizeof(buff), 		//		( struct sockaddr *)&(pSess->stRtpSender.addrRemote[0]));		net_udp_send(pSess->stRtpSender.nRtcpSock[0], buff, sizeof(buff), 				( struct sockaddr *)&(pSess->stRtpSender.addrRemote[0]));				if(net_select(&pSess->stRtpSender.nRtpSock[0], 1, 0x1, 1500) > 0)		{			if(net_udp_recv(pSess->stRtpSender.nRtpSock[0], buff, sizeof(buff), 0, (struct sockaddr *)&addr, &nAddrLen) > 0)			{				printf("@@@@@@@@@@@@@Receive udp package successfully.\n");				printf("@@@@@@@@@@remote ip %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));				//pSess->stRtpSender.addrRemote[0].sin_port    = htons();				//pSess->stRtpSender.addrRemote[1].sin_port    = htons(pSess->u16RemoteRTPPort[1]);				memcpy(&pSess->stRtpSender.addrRemote[0], &addr, sizeof(addr));			}			else			{				printf("@@@@@@@@@@@@@ Receive udp package failed.\n");			}		}		else			printf("@@@@@@@@@@@ select udp package failed.\n");	}#endif		create_rtsp_av_transmit(pSess);
	gRtspSvr.clientCnt[pSess->nChId]++;	   return 0;
}

/*
C --> S
    TEARDOWN rtsp://10.71.147.222/1.mp4 RTSP/1.0
    CSeq: 13
    Session: 117381456211096
    User-Agent: QuickTime/7.0.2 (qtver=7.0.2;os=Windows NT 5.1Service Pack 1)


S --> C
    RTSP/1.0 200 OK
    Server: DSS/5.0.1.1 (Build/464.1.1; Platform/Win32; Release/5;)
    Cseq: 13
    Session: 117381456211096
    Connection: Close

    
*/
int rtsp_handle_teardown(RTSP_SESSION_S *pSess)
{
	char* pTemp;
	RTSP_PRINT("sid %s teardown", pSess->szId);
	pTemp = pSess->szBuffSend;
	pTemp += rtsp_make_resp_header(pSess, 200);
	pTemp += sprintf(pTemp,"Session: %s\r\n", pSess->szId);
	pTemp += sprintf(pTemp,"Connection: Close\r\n\r\n");
	rtsp_send_reply(pSess, 200, NULL, 0);

	RTSP_PRINT("teardown rtsp sid %s  chn %d \n",pSess->szId, pSess->nChId);
	rtsp_session_close(pSess);
	rtsp_list_client();	

    return 0;
}int rtsp_handle_param(RTSP_SESSION_S *pSess){
	char* pTemp = NULL;	pTemp  = pSess->szBuffSend;
	pTemp += rtsp_make_resp_header(pSess, 200);
	pTemp += sprintf(pTemp,"Session: %s;timeout=120\r\n\r\n", pSess->szId);
	rtsp_send_reply(pSess, 200, NULL, 0);	return 0;}
/*
rfc2326bis06-Page42 SETUP can be used in all three states; INIT,and READY
InitState:
    Describe: --> init_state
    Setup   : --> *ready_state
    Teardown: --> init_state
    Options : --> init_state
    Play    : --> init_state
    Pause   : --> init_state

ReadyState:
    Play    : --> *play_state
    Setup   : --> ready
    Teardown: --> ??
    Options : --> 
    pause   : -->
    describe: --> 

*/
int rtsp_handle_pause(RTSP_SESSION_S *pSess)  {
	char* pTemp = NULL;	pTemp  = pSess->szBuffSend;	pTemp += rtsp_make_resp_header(pSess, 200);	pTemp += sprintf(pTemp,"Session: %s;timeout=120\r\n\r\n", pSess->szId);	rtsp_send_reply(pSess, 200, NULL, 0);	pSess->eState = RTSP_STATE_PAUSE;		return 0;  }unsigned long rtsp_handle_event(RTSP_SESSION_S *pSess, int event, int status, char *buf)
{
    unsigned long u32Ret = 0;
    switch (event) 
    {
        case RTSP_OPTIONS_METHOD:
            u32Ret = rtsp_handle_options(pSess);
            break;
        case RTSP_DESCRIBE_METHOD:
            u32Ret = rtsp_handle_describe(pSess);
            break;
        case RTSP_SETUP_METHOD:
			if(0 == (u32Ret = rtsp_handle_setup(pSess)))
			{
				pSess->eState = RTSP_STATE_READY;
			}
            break;
        case RTSP_PLAY_METHOD:
            if (0 == (u32Ret = rtsp_handle_play(pSess)))
            {			//pSess->eState = RTSP_STATE_PLAY;			//gRtspSvr.clientCnt[pSess->nChId]++;			rtsp_list_client();
            }
            break;
        case RTSP_TEARDOWN_METHOD:
            u32Ret = rtsp_handle_teardown(pSess);
            break;
        case RTSP_PAUSE_METHOD:
            //u32Ret = rtsp_send_reply(pSess, 455, "Accept: OPTIONS, SETUP, PLAY, TEARDOWN\r\n", 1);
		u32Ret = rtsp_handle_pause(pSess);            break;
        	    case RTSP_GET_PARAM_METHOD:	    	u32Ret = rtsp_handle_param(pSess);	    	break;	    	
    	default:
    	u32Ret = rtsp_send_reply(pSess, 501, "Accept: OPTIONS, SETUP, PLAY, TEARDOWN\n", 1);
           break;
    }
    return u32Ret;
}

