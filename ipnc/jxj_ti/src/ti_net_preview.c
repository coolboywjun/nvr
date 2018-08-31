#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <error.h>
#include <linux/sockios.h>
#include <linux/if.h>  
#include <time.h>
#include <malloc.h>
#include <sys/unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/prctl.h>

#include "ti_net_common.h"
#include "net_api.h"
#include "ti_net_interface.h"
#include "ti_net_preview.h"
#include "ti_net_struct.h"
#include "ti_struct.h"

#define TI_NET_PREV_BUF_SIZE	2048

typedef struct _TI_NET_REAL_PLAY_S_
{
	int 		nQuit;
	int 		nSock;

	char		ipAddr[32];
	int		port;
	
	pthread_t thid;
	
	TI_U32		u32FrameNo;
	TI_U8		u8FrameType;//VIDEO_FRAME_I = 0x7,
							//VIDEO_FRAME_P = 0x8,
							//VIDEO_FRAME_B = 0xB,
							//AUDIO_FRAME_A = 0xA, 
	TI_U32		u32FrameLen;//帧长度

	char*		pPreBuf;
	TI_U32		u32PreBufSize;

	TI_NET_PREV_REQ_S ReqHdr;
	TI_S32 		nTimeout;

	int			nStatus;
	int 		nPacketNo;
	
	fRealDataCb	cbFuncReadData;
	fBeginReadCb cbFuncBeginRead;
	fEndReadCb cbFuncEndRead;
	
	void *pUserData;

	TI_U32	u32PreFrameNo;
	TI_U32	u32PreRecvFrameNo;
	TI_U64	u64PreTs;
}TI_NET_REAL_PLAY_S;

static int TI_NET_RealPlay_Check_Buffer(TI_NET_REAL_PLAY_S* pRealPlay)
{
	int nRecvLen = 0;
	char RecvBuf[TI_NET_PREV_BUF_SIZE] = {0};
	char *pRecvData = NULL;
	TI_NET_STREAM_HDR_S *pStreamHdr = (TI_NET_STREAM_HDR_S *)RecvBuf;
	
	pRecvData = (char *)pStreamHdr;
	nRecvLen = sizeof(TI_NET_STREAM_HDR_S); /* 先接收TI_NET_STREAM_HDR_S头*/

	if(net_tcp_noblock_recv(pRealPlay->nSock, pRecvData, TI_NET_PREV_BUF_SIZE, nRecvLen, pRealPlay->nTimeout) != nRecvLen)
	{
		//__ERR("Failed to recv stream hdr packet from the server.\n");
		return TI_NET_ERR_RECV_FAIL;
	}

	if(pStreamHdr->u32MagicFlag == TI_NET_MAGIC_FLAG)
	{
		pRealPlay->u32FrameNo = pStreamHdr->u32FrameNo;
		pRealPlay->u8FrameType = pStreamHdr->u8FrameType;
		pRealPlay->u32FrameLen = pStreamHdr->u32FrameLen;
		if(pRealPlay->pPreBuf == NULL || pRealPlay->u32PreBufSize < pRealPlay->u32FrameLen)
		{
			if(pRealPlay->pPreBuf != NULL)
				free(pRealPlay->pPreBuf);
			// 256K对齐
			pRealPlay->u32PreBufSize = (pStreamHdr->u32FrameLen + 0x40000) & (~(0x40000 - 1));
			pRealPlay->pPreBuf = (char*)malloc(pRealPlay->u32PreBufSize);
			if(pRealPlay->pPreBuf == NULL)
			{
				__ERR("Failed to malloc for frame buffer.\n");
				pRealPlay->u32PreBufSize = 0;
				return TI_NET_ERR_ALLOC_RESOURCE;
			}
		}
	}
	else
	{
		__ERR("Stream MaicFlag error\r\n");
		return TI_NET_ERR_RECV_FAIL;	
	}

	return pStreamHdr->u32FrameLen;
}

static int TI_NET_RealPlay_Recv_Frame(TI_NET_REAL_PLAY_S* pRealPlay)
{
	int nTotalRecv = 0;
	TI_NET_STREAM_PIS_HDR_S pisHdr;
	TI_FRAME_HEAD_S *pFrameHdr;
	
	if(TI_NET_RealPlay_Check_Buffer(pRealPlay) < 0)
	{
		//__ERR("Failed to check buffer.\n");
		return TI_FAILURE;
	}
	
	/* 接收音视频数据包*/
	while(nTotalRecv < pRealPlay->u32FrameLen)
	{
		if(net_tcp_noblock_recv(pRealPlay->nSock, (char *)&pisHdr, sizeof(pisHdr), sizeof(pisHdr), pRealPlay->nTimeout) != sizeof(pisHdr))
		{
			__ERR("Failed to recv packet header from the server.\n");
			return TI_FAILURE;
		}
		
		if(pisHdr.u32MagicFlag != TI_NET_MAGIC_FLAG)
		{
			__ERR("The packet magic flag is wrong, flag is : 0x%08x\n", pisHdr.u32MagicFlag);
			return TI_FAILURE;
		}
		if(net_tcp_noblock_recv(pRealPlay->nSock, &pRealPlay->pPreBuf[nTotalRecv], TI_NET_PREV_BUF_SIZE, pisHdr.u16PacketLen, pRealPlay->nTimeout) != pisHdr.u16PacketLen)
		{
			__ERR("Failed to recv packet from the server.\n");
			return TI_FAILURE;
		}
		nTotalRecv += pisHdr.u16PacketLen;
	}
	pFrameHdr = (TI_FRAME_HEAD_S *)pRealPlay->pPreBuf;
	if(pFrameHdr->u8FrameType != 0xa)
	{
		if(pRealPlay->u32PreFrameNo != 0 && pRealPlay->u32PreFrameNo + 1 != pFrameHdr->u32FrameNo)
		{
			__ERR("@@@@@@@@@@@@@@ jxj ti pre frame no: %u, cur frame no %u, pre recv no: %u, cur recv no: %d, pre ts: %llu, cur ts: %llu, frame type: %u\n", 
				pRealPlay->u32PreFrameNo, pFrameHdr->u32FrameNo,
				pRealPlay->u32PreRecvFrameNo, pisHdr.u32FrameNo,
				pRealPlay->u64PreTs/1000, pFrameHdr->u64TimeStamp/1000,
				pFrameHdr->u8FrameType);
		}
		pRealPlay->u32PreFrameNo = pFrameHdr->u32FrameNo;
		pRealPlay->u32PreRecvFrameNo = pisHdr.u32FrameNo;
		pRealPlay->u64PreTs = pFrameHdr->u64TimeStamp;
	}
	return nTotalRecv;
}


/*################################*/
static int TI_NET_RealPlay_Send_ReqCmd(TI_NET_REAL_PLAY_S *pRealPlay)
{
	int nRecvSize;
	char sendBuf[1024] = {0};
	TI_NET_HDR_S *pNetHdr = (TI_NET_HDR_S *)sendBuf;

	net_set_net_hdr(pNetHdr, sizeof(TI_NET_LOGON_S), TI_CMD_OPEN_PREVIEW,
				pRealPlay->nPacketNo++,  0);

	ti_net_encrypt(pRealPlay->ReqHdr.szUsr, strlen(pRealPlay->ReqHdr.szUsr));
	ti_net_encrypt(pRealPlay->ReqHdr.szPsw, strlen(pRealPlay->ReqHdr.szPsw));
	
	memcpy((TI_NET_PREV_REQ_S *)(sendBuf + TI_NET_HDR_LEN), &pRealPlay->ReqHdr, sizeof(TI_NET_PREV_REQ_S));

	if(net_tcp_noblock_send(pRealPlay->nSock, sendBuf, sizeof(TI_NET_HDR_S)+sizeof(TI_NET_LOGON_S), NULL, 1000) < 0)
	{
		__ERR("Failed to send packet to the server.\n");
		return TI_NET_ERR_SEND_FAIL;
	}
	nRecvSize = net_tcp_noblock_recv(pRealPlay->nSock, sendBuf, sizeof(TI_NET_HDR_S), sizeof(sendBuf), 1000);

	if(nRecvSize < 0)
	{
		//__ERR("Failed to recv net header packet from the server.\n");
		return TI_NET_ERR_RECV_FAIL;
	}
	if(pNetHdr->u16CmdType != TI_CMD_OPEN_PREVIEW)
	{
		__ERR("Check the net header packet error!\r\n");
		return TI_FAILURE;
	}
	
	return TI_SUCCESS;
}

static void* TI_NET_RealPlay_Func(void *pData)
{
	TI_NET_REAL_PLAY_S *pRealPlay = (TI_NET_REAL_PLAY_S *)pData;
	char szPort[16];

	prctl(PR_SET_NAME, "RealPlay");
	sprintf(szPort, "%d", pRealPlay->port);
	while(!pRealPlay->nQuit)
	{
		if((pRealPlay->nSock = ti_connect_server(pRealPlay->ipAddr, szPort)) < 0)
		{
			pRealPlay->nStatus = 0;
			sleep(3);
			continue;
		}
		if(TI_NET_RealPlay_Send_ReqCmd(pRealPlay) < 0)
		{
			pRealPlay->nStatus = 0;
			net_close_socket(&pRealPlay->nSock);
			sleep(3);
			continue;
		}
		if(pRealPlay->cbFuncBeginRead)
			pRealPlay->cbFuncBeginRead((long)pRealPlay, pRealPlay->pUserData);
		while(!pRealPlay->nQuit)
		{
			if(TI_NET_RealPlay_Recv_Frame(pRealPlay) < 0)
			{
				__ERR("Recv frame error!\r\n");
				pRealPlay->nStatus = 0;
				sleep(1);
				break;
			}
			if(pRealPlay->cbFuncReadData((long)pRealPlay, pRealPlay->u8FrameType, 
					(unsigned char *)pRealPlay->pPreBuf, pRealPlay->u32FrameLen, pRealPlay->pUserData) < 0)
			{
				__ERR("Write frame error.\n");
				pRealPlay->nStatus = 0;
				sleep(1);
				break;
			}
			pRealPlay->nStatus = 1;
		}
		if(pRealPlay->cbFuncEndRead)
			pRealPlay->cbFuncEndRead((long)pRealPlay, pRealPlay->pUserData);
		net_close_socket(&pRealPlay->nSock);
	}
	return (void*)1;
}

long	TI_NET_Start_RealPlay(const char 	*pSzDevIP, int dwDevPort, const char *pUserName, const char *pPwd, 
									int dwChn, int dwStream,
									fRealDataCb cbFuncReadData, fBeginReadCb cbFuncBeginRead, fEndReadCb cbFuncEndRead,
									void *pUserData, long *plRealHandle)
{
	TI_NET_REAL_PLAY_S *pRealPlay;

	if(dwChn != 0 && (dwStream < 0 || dwStream > 2) && cbFuncReadData == NULL)
		return TI_FAILURE;
		
	pRealPlay = (TI_NET_REAL_PLAY_S*)malloc(sizeof(TI_NET_REAL_PLAY_S));
	if(pRealPlay == NULL)
		return TI_FAILURE;

	memset(pRealPlay, 0, sizeof(*pRealPlay));

	strncpy(pRealPlay->ipAddr, pSzDevIP, sizeof(pRealPlay->ipAddr)-1);
	pRealPlay->ipAddr[sizeof(pRealPlay->ipAddr)-1] = '\0';
	pRealPlay->port = dwDevPort;
	strcpy(pRealPlay->ReqHdr.szUsr, pUserName);
	strcpy(pRealPlay->ReqHdr.szPsw, pPwd);
	pRealPlay->ReqHdr.u8Ch = dwChn;
	pRealPlay->ReqHdr.u8StreamType = dwStream;
	//pRealPlay->ReqHdr.u32DstIp  = pSzDevIP;
	//pRealPlay->ReqHdr.u16DstPort = dwDevPort;
	pRealPlay->nTimeout = 3000;

	pRealPlay->cbFuncReadData = cbFuncReadData;
	pRealPlay->cbFuncBeginRead = cbFuncBeginRead;
	pRealPlay->cbFuncEndRead = cbFuncEndRead;
	pRealPlay->pUserData = pUserData;

	if(pthread_create(&pRealPlay->thid, NULL, TI_NET_RealPlay_Func, pRealPlay) != 0)
	{
		free(pRealPlay);
		return TI_FAILURE;
	}

	*plRealHandle = (long)pRealPlay;
	return TI_SUCCESS;	
}

int TI_NET_Stop_RealPlay(long lRealHandle)
{
	TI_NET_REAL_PLAY_S *pRealPlay = (TI_NET_REAL_PLAY_S *)lRealHandle;

	if(pRealPlay == NULL)
		return TI_FAILURE;

	pRealPlay->nQuit = 1;
	pthread_join(pRealPlay->thid, NULL);

	if(pRealPlay->pPreBuf != NULL)
	{
		free(pRealPlay->pPreBuf);
		pRealPlay->pPreBuf = NULL;
	}
	pRealPlay->u32PreBufSize = 0;
	
	free(pRealPlay);
	
	return TI_SUCCESS;
}


int TI_NET_Request_IFrame(long lRealHandle)
{
	TI_NET_REAL_PLAY_S *pRealPlay = (TI_NET_REAL_PLAY_S *)lRealHandle;
	TI_NET_HDR_S stNetHdr;

	if(pRealPlay == NULL)
		return TI_FAILURE;
	if(pRealPlay->nStatus != 1)	// 没有发送流
		return TI_FAILURE;
	memset(&stNetHdr, 0, sizeof(stNetHdr));
	net_set_net_hdr(&stNetHdr, 0, TI_CMD_REQUEST_I_FRAME, pRealPlay->nPacketNo++, 0);
	if (net_tcp_noblock_send(pRealPlay->nSock, (char *)&stNetHdr, sizeof(TI_NET_HDR_S), NULL, 1000) != sizeof(TI_NET_HDR_S))
	{
		
		return TI_ERR_SEND_FAIL;
	}
	return TI_SUCCESS;
}

/*test*/
static int cnt;
void test_stream(long lRealHandle, int dwDataType, const unsigned char *pBuffer, int dwBufSize, void *pUserData)
{
	FILE *fp = NULL;
	FILE *fphr = NULL;
	int ret = 0;
	int frameSize = 0;
	
	if(pBuffer == NULL)
		return;
	
	fp = fopen("./video.h264", "ab+");
	if(NULL== fp)
	{
		fp = fopen("./video.h264", "wb+");
		if(NULL== fp)
		{	
			__ERR("open failed!\n");
			return;
		}	
	}
	fphr = fopen("./video.h264.hdr", "ab+");
	if(NULL== fphr)
	{
		fphr= fopen("./video.hdr", "wb+");
		if(NULL== fphr)
		{	
			__ERR("open failed!\n");
			return;
		}	
	}
	
	//写文件
	do{
		ret = fwrite(pBuffer+sizeof(TI_FRAME_HEAD_S), 1, dwBufSize - sizeof(TI_FRAME_HEAD_S), fp);//去掉头
		//ret = fwrite(pBuffer, 1, dwBufSize, fp);
	}while(ret < 0 && errno == EINTR);
	if(-1 == ret)//出错
	{	
		__ERR("write failed!\n");
		goto _OUT;
	}
	else
	{
		frameSize = dwBufSize - sizeof(TI_FRAME_HEAD_S);//去掉头
		//frameSize = dwBufSize;
		do{
			ret = fwrite(&frameSize, 1, sizeof(frameSize), fphr);
		}while(ret < 0 && errno == EINTR);
	}
	cnt++;
	__DBUG("frameSize:%d, cnt:%d\n", frameSize, cnt);
_OUT:	
	fclose(fp);
	fclose(fphr);
	return;

}

