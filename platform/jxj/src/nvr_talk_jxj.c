

#include <sys/prctl.h>

#include <nvr_headers.h>
#include <nvr_lib_interface.h>
#include <av_pool.h>
#include <storage_lib.h>
#include <player_interface.h>
#include <audio_params.h>

#include "nvr_platform_jxj.h"
#include "nvr_param_jxj.h"
#include "nvr_talk_jxj.h"

#include "j_sdk.h"
#include "media_structs.h"
#include "j_sdk_inters.h"


typedef struct NVR_TALK_JXJ_CLIENT_INFO_S
{
	volatile int nQuit;
	int nSock;
	gint bRecvHead;
	GIOChannel *pIoChannel;
	guint watchId;
	long lReadId;
	long lWriteId;
	GThread *thread;

	J_TALK_REQ stTalkRecvReq;
	J_TALK_REQ stTalkSendReq;
}NVR_TALK_JXJ_CLIENT_INFO;

typedef struct NVR_TALK_JXJ_INFO_S
{
	int s32TalkSocket;	//对讲监听

	GIOChannel *pIoChannel;
	NVR_TALK_JXJ_CLIENT_INFO *pClientInfo;
}NVR_TALK_JXJ_INFO;

static NVR_TALK_JXJ_INFO stTalkJxjInfo;


#define TIME_OUT 10000


static int nvr_jxj_talk_disconnector_proc(NVR_TALK_JXJ_CLIENT_INFO *pClientInfo);

static int nvr_jxj_talk_send_data(int s32TalkSockfd, char *pFrameBuf, int s32FrameLen)
{
	int s32DataLen = 0;
	J_TALK_FRMAE_HDR *pstTalkFrameHdr = (J_TALK_FRMAE_HDR *)pFrameBuf;
	AV_FRAME_HEAD_S stFrameHdr;

	memcpy(&stFrameHdr, pFrameBuf, AV_FRAME_HEAD_LEN);

	s32DataLen = s32FrameLen - AV_FRAME_HEAD_LEN;
	pstTalkFrameHdr->u32Magic = J_TALK_MAGIC;
	pstTalkFrameHdr->u32FrameNo = stFrameHdr.u32FrameNo;
	pstTalkFrameHdr->u32Pts = stFrameHdr.u64TimeStamp / 1000;
	pstTalkFrameHdr->u32Len = s32DataLen;
	memcpy(pstTalkFrameHdr->u8Data, pFrameBuf + AV_FRAME_HEAD_LEN, s32DataLen);
	
	if(net_tcp_noblock_send(s32TalkSockfd, pFrameBuf, sizeof(J_TALK_FRMAE_HDR)+s32DataLen, NULL, TIME_OUT) < 0)
	{
		j_warning("Send stream error.");
		return -1;
	}
	
	return 0;
}

static int nvr_jxj_talk_recv_data(int s32TalkSockfd, char *pBuf, int s32BufLen)
{
	J_TALK_FRMAE_HDR *pTalkFrameHdr = (J_TALK_FRMAE_HDR *)pBuf;
	
	memset(pBuf, 0, sizeof(s32BufLen));
	if(net_tcp_noblock_recv(s32TalkSockfd, pTalkFrameHdr, s32BufLen, sizeof(J_TALK_FRMAE_HDR), TIME_OUT) <= 0)
	{
		j_warning("Recv talk header error.");
		return -1;
	}
	if(pTalkFrameHdr->u32Magic != J_TALK_MAGIC)
	{
		j_warning("Recv talk header magic error.");
		return -1;
	}
	//j_debug("package len: %u", pTalkFrameHdr->u32Len);
	if(net_tcp_noblock_recv(s32TalkSockfd, pBuf+sizeof(J_TALK_FRMAE_HDR), 
					s32BufLen-sizeof(J_TALK_FRMAE_HDR), pTalkFrameHdr->u32Len, TIME_OUT) <= 0)
	{
		j_warning("Recv talk data error.");
		return -1;
	}

	return 0;
}

static int nvr_jxj_talk_req_reponse(int s32TalkSockfd, J_TALK_REQ *pTalkReq)
{	
	if(net_tcp_noblock_send(s32TalkSockfd, (char *)pTalkReq, sizeof(J_TALK_REQ), NULL, TIME_OUT) != sizeof(J_TALK_REQ))
	{
		j_warning("The reponse send error.");
		return -1;
	}
	
	return 0;
}

static int nvr_jxj_talk_req_proc(NVR_TALK_JXJ_CLIENT_INFO *pClientInfo, J_TALK_REQ *pTalkReq)
{
	int user;
	NVR_DEV_USER_CFG_S stUserCfg = {0};
	int nClientSock = pClientInfo->nSock;
	J_TALK_REQ *pTalkSendReq = &pClientInfo->stTalkSendReq;
	AV_AUDIO_INFO_S stAudioInfo = {0};
	//NVR_DEV_AUDIO_CFG_S stAudioCfg = {0};
	
	if(pTalkReq->u32Magic != J_TALK_MAGIC)
	{
		j_warning("The req packet magic error.");
		return -1;
	}

	// check the user and password
	pTalkReq->szUsr[sizeof(pTalkReq->szUsr) - 1] = '\0';
	pTalkReq->szPsw[sizeof(pTalkReq->szPsw) - 1] = '\0';
	j_debug("Receive user: %s, password: %s",  pTalkReq->szUsr, pTalkReq->szPsw);
	for(user = 0; user < NVR_MAX_USR_NUM; user++)
	{
		nvr_logon_get_user_cfg(user, &stUserCfg);
		if(strcmp(pTalkReq->szUsr, stUserCfg.szUsrName) == 0
			&& strcmp(pTalkReq->szPsw, stUserCfg.szPsw) == 0)
			break;
	}
	if(user == NVR_MAX_USR_NUM)
	{
		j_warning("The user or psw error.");
		return -1;
	}
	j_debug("Receive audio sample %d, encode type %d channels %d bits %d.",
			pTalkReq->stAttr.u8AudioSamples, pTalkReq->stAttr.u8EncodeType,
			pTalkReq->stAttr.u8AudioChannels, pTalkReq->stAttr.u8AudioBits);
	memcpy(pTalkSendReq, pTalkReq, sizeof(J_TALK_REQ));
	//pTalkSendReq->stAttr.u8AudioSamples = 0;
	//pTalkSendReq->stAttr.u8EncodeType = 1;
	//pTalkSendReq->stAttr.u8AudioChannels = 1;
	//pTalkSendReq->stAttr.u8AudioBits = 16;
	pTalkSendReq->s32Res = 0;
	if(pClientInfo->lWriteId == 0)
	{
		j_warning("The talk is busy.");
		pTalkSendReq->s32Res = 1;
		nvr_jxj_talk_req_reponse(nClientSock, pTalkSendReq);
		return -1;
	}
	
	stAudioInfo.u8AudioSamples = pTalkSendReq->stAttr.u8AudioSamples;
	stAudioInfo.u16EncodeType = pTalkSendReq->stAttr.u8EncodeType;
	stAudioInfo.u8AudioChannels = pTalkSendReq->stAttr.u8AudioChannels;
	stAudioInfo.u8AudioBits = pTalkSendReq->stAttr.u8AudioBits;
	player_set_record_par(&stAudioInfo);
	
	if(nvr_jxj_talk_req_reponse(nClientSock, pTalkSendReq) < 0)
	{
		j_warning("The reponse error.");
		return -1;
	}

	///////////////////////////////////////////////
	// set the audio decode type, TBD
	//nvr_get_audio_cfg(0, &stAudioCfg);
	//stAudioCfg.u8AEncType = pTalkReq->stAttr.u8EncodeType;	//其它暂时只有一个选项
	//nvr_set_audio_cfg(0, &stAudioCfg);
	
	return 0;
}

static gpointer nvr_jxj_talk_client_send_func (gpointer data)
{
	NVR_TALK_JXJ_CLIENT_INFO *pClientInfo = (NVR_TALK_JXJ_CLIENT_INFO *)data;
	int nClientSock = pClientInfo->nSock;
	long lReadId = pClientInfo->lReadId;
	char *pTalkBuf;
	int nBufLen;
	int nFrameLen;

	while(!pClientInfo->nQuit)
	{
		if(g_atomic_int_get(&pClientInfo->bRecvHead))
		{
			pTalkBuf = NULL;
			nBufLen = 0;
			if((nFrameLen = av_pool_read_frame(lReadId, &pTalkBuf, &nBufLen, 100)) < 0)
			{
				g_usleep(G_USEC_PER_SEC * 1);
				continue;
			}
			if(nvr_jxj_talk_send_data(nClientSock, pTalkBuf, nFrameLen) < 0)
			{
				j_warning("Send talk data to client failed.");
				break;
			}
		}
		else
			usleep(100000);
	}
	
	return NULL;
}

static gboolean nvr_jxj_talk_client_recv_func (GIOChannel *channel, 
							GIOCondition  condition, gpointer      data)
{
	int nRet = TRUE;
	int nClientSock;
	char buff[1024];
	NVR_TALK_JXJ_CLIENT_INFO *pClientInfo = (NVR_TALK_JXJ_CLIENT_INFO *)data;
	J_TALK_REQ *pTalkRecvReq = &pClientInfo->stTalkRecvReq;
	J_TALK_FRMAE_HDR *pTalkFrameHdr = (J_TALK_FRMAE_HDR *)buff;
	
	nClientSock = g_io_channel_unix_get_fd (channel);
	if(condition | G_IO_IN)
	{
		if(!g_atomic_int_get(&pClientInfo->bRecvHead))
		{
			if(net_tcp_noblock_recv(nClientSock, (char *)pTalkRecvReq, sizeof(J_TALK_REQ), sizeof(J_TALK_REQ), TIME_OUT) < 0)
			{
				j_warning("The Req packet recv error.");
				nRet = FALSE;
				goto eexit;
			}
			if(nvr_jxj_talk_req_proc(pClientInfo, pTalkRecvReq) < 0)
			{
				j_warning("The req packet proc error.");
				nRet = FALSE;
				goto eexit;
			}
			g_atomic_int_set(&pClientInfo->bRecvHead, 1);
		}
		else
		{
			if(nvr_jxj_talk_recv_data(nClientSock, buff, sizeof(buff)) < 0)
			{
				j_warning("Recv talk data failed.");
				nRet = FALSE;
				goto eexit;
			}

			if(pClientInfo->lWriteId != 0)
			{
				AV_FRAME_HEAD_S stFrameHdr;

				memset(&stFrameHdr, 0, sizeof(stFrameHdr));
				stFrameHdr.u16FrameFlag = 0x3448;
				stFrameHdr.u8FrameType = AV_AUDIO_FRAME_A;
				stFrameHdr.u32FrameNo = pTalkFrameHdr->u32FrameNo;
				stFrameHdr.u32Time = pTalkFrameHdr->u32Pts / 1000;
				stFrameHdr.u32FrameSize = pTalkFrameHdr->u32Len;
				stFrameHdr.u64TimeStamp = pTalkFrameHdr->u32Pts * 1000;
				stFrameHdr.unAudioInfo.u8AudioSamples = pTalkRecvReq->stAttr.u8AudioSamples;
				stFrameHdr.unAudioInfo.u16EncodeType = pTalkRecvReq->stAttr.u8EncodeType;
				stFrameHdr.unAudioInfo.u8AudioChannels = pTalkRecvReq->stAttr.u8AudioChannels;
				stFrameHdr.unAudioInfo.u8AudioBits = pTalkRecvReq->stAttr.u8AudioBits;
				//j_debug("sample: %u, encode: %u, channel: %u, bits: %u", 
				//				stFrameHdr.unAudioInfo.u8AudioSamples,
				//				stFrameHdr.unAudioInfo.u16EncodeType,
				//				stFrameHdr.unAudioInfo.u8AudioChannels,
				//				stFrameHdr.unAudioInfo.u8AudioBits);
				av_pool_write_frame(pClientInfo->lWriteId, 
									pTalkFrameHdr->u8Data, pTalkFrameHdr->u32Len,
									(unsigned char *)&stFrameHdr, sizeof(stFrameHdr),
									stFrameHdr.u32FrameNo, stFrameHdr.u8FrameType, 
									stFrameHdr.u64TimeStamp);
				/*
				audio_head head = {0};
				head.audio_para.u8AudioBits=pTalkRecvReq->stAttr.u8AudioBits;
				head.audio_para.u8AudioChannels=pTalkRecvReq->stAttr.u8AudioChannels;
				head.audio_para.u16EncodeType=pTalkRecvReq->stAttr.u8EncodeType;
				head.audio_para.u8AudioSamples=pTalkRecvReq->stAttr.u8AudioSamples;
				head.len=pTalkFrameHdr->u32Len;
				head.data_format=ALSA_PCM_FORMAT_S16_LE;
				*/
				//j_debug("audio bits: %u", head.audio_para.u8AudioBits);
				//j_debug("audio channels: %u", head.audio_para.u8AudioChannels);
				//j_debug("audio encode type: %u", head.audio_para.u16EncodeType);
				//j_debug("audio samples: %u", head.audio_para.u8AudioSamples);
			}
		}
	}
	else if((condition | G_IO_ERR) || (condition | G_IO_HUP))
	{
		nRet = FALSE;
	}
	
eexit:
	if(nRet == FALSE)
	{
		nvr_jxj_talk_disconnector_proc(pClientInfo);
	}
	
	return nRet;
}

static int nvr_jxj_talk_new_connector_proc(int nClientSock)
{
	NVR_TALK_JXJ_CLIENT_INFO *pClientInfo;
	
	pClientInfo = g_new0(NVR_TALK_JXJ_CLIENT_INFO, 1);
	if(av_pool_start_write_frame(AV_POOL_TALK_CHN, AV_POOL_TALK_IN_STREAM, &pClientInfo->lWriteId) < 0)
	{
		j_warning("Start write talk frame failed, maybe the talk is busy.");
		g_free(pClientInfo);
		return -1;
	}
	
	//  Read the audio date and send to client
	if(av_pool_start_read_frame(AV_POOL_TALK_CHN, AV_POOL_TALK_OUT_STREAM, 1000, &pClientInfo->lReadId) < 0)
	{
		j_warning("Start read talk frame failed.");
		av_pool_stop_write_frame(pClientInfo->lWriteId);
		g_free(pClientInfo);
		return -1;
	}
	player_start_phone_audio();
	player_start_audio_record();

	net_set_sock_noblock(nClientSock);
	net_set_sock_attr(nClientSock, 1, 0,0,0,0);

	j_debug("client sock: %d", nClientSock);
	pClientInfo->nSock = nClientSock;
	g_atomic_int_set(&pClientInfo->bRecvHead, 0);
	pClientInfo->pIoChannel = g_io_channel_unix_new (nClientSock);
	pClientInfo->watchId = g_io_add_watch(pClientInfo->pIoChannel, 
						G_IO_IN | G_IO_ERR | G_IO_HUP,
						nvr_jxj_talk_client_recv_func, pClientInfo);
						
	pClientInfo->thread = g_thread_new("Talk", nvr_jxj_talk_client_send_func, pClientInfo);

	g_atomic_pointer_set(&stTalkJxjInfo.pClientInfo, pClientInfo);
	
	return 0;
}

static int nvr_jxj_talk_disconnector_proc(NVR_TALK_JXJ_CLIENT_INFO *pClientInfo)
{
	/* wait the thread exit */
	pClientInfo->nQuit = 1;
	g_thread_join(pClientInfo->thread);
	pClientInfo->thread = NULL;
	
	player_stop_phone_audio();
	player_stop_audio_record();

	/* clear the resource */
	av_pool_stop_read_frame(pClientInfo->lReadId);
	av_pool_stop_write_frame(pClientInfo->lWriteId);
	g_source_remove(pClientInfo->watchId);
	g_io_channel_unref(pClientInfo->pIoChannel);
	net_close_socket(&pClientInfo->nSock);
	g_free(pClientInfo);
	g_atomic_pointer_set(&stTalkJxjInfo.pClientInfo, NULL);

	return 0;
}

static gboolean nvr_jxj_talk_server_func (GIOChannel *channel, 
							GIOCondition  condition, gpointer      data)
{
	int nSock;
	int nServerSock = g_io_channel_unix_get_fd (channel);
	struct sockaddr_in addr;
	int nAddrLen = sizeof(addr);
	
	if(condition | G_IO_IN)
	{
		/* accept new connect */
		nSock = net_tcp_block_accept(nServerSock, (struct sockaddr *)&addr, &nAddrLen);
		if(nSock < 0)
		{
			j_warning("Accept new jxj talk connection.");
			return FALSE;
		}
		if(nvr_jxj_talk_new_connector_proc(nSock) < 0)
		{
			j_warning("Proc new connector failed.");
			net_close_socket(&nSock);
		}
	}
	else if((condition | G_IO_ERR) || (condition | G_IO_HUP))
	{
		/* sock error, clear the resource */
		return FALSE;
	}

	return TRUE;
}

int nvr_jxj_talk_init()
{
	char port[16];

	memset(&stTalkJxjInfo, 0, sizeof(stTalkJxjInfo));
	
	player_msg_init();
	
	g_sprintf(port, "%d", J_TALK_PORT);
	stTalkJxjInfo.s32TalkSocket = net_tcp_listen(NULL, port, NULL);
	if(stTalkJxjInfo.s32TalkSocket < 0)
	{
		j_warning("Listen port %d failed.", J_TALK_PORT);
		return -1;
	}
	stTalkJxjInfo.pIoChannel = g_io_channel_unix_new (stTalkJxjInfo.s32TalkSocket);
	g_io_add_watch(stTalkJxjInfo.pIoChannel, G_IO_IN | G_IO_ERR | G_IO_HUP,
						nvr_jxj_talk_server_func, NULL);
						
	return 0;
}

int nvr_jxj_talk_uninit()
{
	g_io_channel_unref(stTalkJxjInfo.pIoChannel);
	if(g_atomic_pointer_get(&stTalkJxjInfo.pClientInfo) != NULL)
		nvr_jxj_talk_disconnector_proc(stTalkJxjInfo.pClientInfo);
	net_close_socket(&stTalkJxjInfo.s32TalkSocket);

	return 0;
}


