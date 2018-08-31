

#include "nvr_struct.h"
#include "nvr_headers.h"

#include <av_pool.h>
#include <ipnc_lib_client.h>
#include <nvr_lib_interface.h>
#include <storage_lib.h>
#include <player_interface.h>

#include "nvr_p2p_tutk.h"

#include "JP2PDef.h"


//#define TUTK_SERVER_IP		"113.105.65.25"
#define TUTK_SERVER_IP		"115.29.227.114"
#define TUTK_SERVER_PORT	11180


typedef struct _P2P_CMD_MSG_
{
	long lMsgType;
	
	struct stJP2PCtrlStream stCmd;
}P2P_CMD_MSG;

typedef struct _P2P_CHN_HANDLE_
{
	volatile int nQuit;

	jdt_long lHandle;
	jdt_int iChn;
	jdt_int iCode;
	jdt_l64		m_stStartTime;						// 历史流的开始时间,实时流时忽略
	jdt_l64		m_stStopTime;						// 历史流的结束时间,实时流时忽略
	jdt_dword	m_dwRecType;						// 历史流的类型,由协议自定义,实时流时忽略

	long			lReadId;
	long			lWriteId;
	StorageFragmentInfo stFragInfo;
	
	int nStatus;
	long lMsgType;
	
	pthread_t thid;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
}P2P_CHN_HANDLE;
static P2P_CHN_HANDLE gP2PHandle[NVR_MAX_CHANNEL_NUM][NVR_MAX_STREAM_NUM];

#define P2P_HISTORY_HANDLE_NUM	8
static P2P_CHN_HANDLE gP2PHistoryHandle[P2P_HISTORY_HANDLE_NUM];
static P2P_CHN_HANDLE gP2PTalkHandle;
static int gMsgQid = 0;

#define P2P_REAL_STREAM_MSG_TYPE	 		0x1000
#define P2P_HISTORY_STREAM_MSG_TYPE		0x2000

extern int nvr_jxj_get_device_param(int id, int channel, void *param, void *u);
extern int nvr_jxj_set_device_param(int id, int channel, void *param, void *u);

static inline time_t nvr_p2p_make_time(struct stJP2PTime *pP2PTime)
{
	struct tm stm;

	stm.tm_year = pP2PTime->m_sYear;
	stm.tm_mon = pP2PTime->m_sMonth % 12;
	stm.tm_mday= pP2PTime->m_sDay % 32;
	stm.tm_hour = pP2PTime->m_sHour % 24;			
	stm.tm_min  = pP2PTime->m_sMinute % 60;		
	stm.tm_sec  = pP2PTime->m_sSecond % 60; 

	return mktime(&stm);
}

static inline int nvr_p2p_recv_msg(long lMsgType, P2P_CMD_MSG *pCmdMsg)
{
	return msgrcv(gMsgQid, pCmdMsg, sizeof(P2P_CMD_MSG)-sizeof(long), lMsgType, IPC_NOWAIT);
}

static inline int nvr_p2p_send_msg(long lMsgType, P2P_CMD_MSG *pCmdMsg)
{
	pCmdMsg->lMsgType = lMsgType;
	return msgsnd(gMsgQid, pCmdMsg, sizeof(P2P_CMD_MSG)-sizeof(long), 0);
}

// 日志回调
static int nvr_p2p_log(jdt_lpchar lpszLog)
{
	j_debug("@@@@@@@@p2p log: %s", lpszLog);
	return eJP2PETSuccess;
}

// 登录认证回调
static int nvr_p2p_auth(jdt_long lHandle, jdt_lpchar lpszName,jdt_lpchar lpszPwd)
{
	int i;
	NVR_DEV_USER_CFG_S stUserCfg;

	j_debug("p2p auth, name: %s, pwd: %s.", lpszName, lpszPwd);

	if(lpszName == NULL || lpszPwd == NULL)
		return eJP2PETParam;
	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{
		nvr_logon_get_user_cfg(i, &stUserCfg);
		if(strcmp(lpszName, stUserCfg.szUsrName) == 0
			&& strcmp(lpszPwd, stUserCfg.szPsw) == 0)
			return eJP2PETSuccess;
	}
	return eJP2PETUserNotExist;
}

// 获取设备参数回调
static int nvr_p2p_get_param(jdt_long lHandle, jdt_int iChn, jdt_int iParamID, jdt_lpvoid lpData, jdt_int iDataSize)
{
	j_debug("p2p set param, chn: %d,  id: %d.", iChn, iParamID);
	return nvr_jxj_get_device_param(iParamID, iChn, lpData, NULL);
}

// 设置设备参数回调
static int nvr_p2p_set_param(jdt_long lHandle, jdt_int iChn, jdt_int iParamID, jdt_lpvoid lpData, jdt_int iDataSize)
{
	j_debug("p2p set param, chn: %d,  id: %d.", iChn, iParamID);
	return nvr_jxj_set_device_param(iParamID, iChn, lpData, NULL);
}

static void *nvr_p2p_send_real_frame_func(void *data)
{
	P2P_CHN_HANDLE *pP2PHandle = (P2P_CHN_HANDLE *)data;
	long lReadId = pP2PHandle->lReadId;
	int nFrameSize;
	int nBufLen;
	char *pFrameBuf;
	P2P_CMD_MSG stCmdMsg;
	int nRet = 0;
	PSTJP2PStreamData pStreamData;
	int nCnt = 0;
	char szName[32] = {0};
	
	j_debug("Start send chn%d stream%d.", pP2PHandle->iChn, pP2PHandle->iCode);	
	pthread_detach(pthread_self());
	snprintf(szName, sizeof(szName)-1, "Ch%dSt%dSend", pP2PHandle->iChn, pP2PHandle->iCode);
	prctl(PR_SET_NAME, szName);

	while(!pP2PHandle->nQuit)
	{
		if(nvr_p2p_recv_msg(pP2PHandle->lMsgType, &stCmdMsg) > 0)
		{
			j_debug("Receive cmd message, message cmd: %d", stCmdMsg.stCmd.m_iCtrlCode);
			
		}
		pFrameBuf = NULL;
		nBufLen = 0;
		if((nFrameSize = av_pool_read_frame(lReadId, &pFrameBuf, &nBufLen, 0)) < 0)
		{
			usleep(10000);
			continue;
		}
		if((nCnt++ % 1000) == 0)
			j_message("Ch %d stream %d send %d frames, frame size: %d", pP2PHandle->iChn, pP2PHandle->iCode, nCnt-1, nFrameSize);
		pStreamData = (PSTJP2PStreamData)pFrameBuf;
		if(pStreamData->m_wFrameFlag != 0x3448)
		{
			j_warning("The frame flag is wrong.");
			break;
		}
		if(JP2P_OnSendFrame(pFrameBuf, nFrameSize, pP2PHandle->lHandle) < 0)
		{
			j_warning("Failed to send frame.");
			nRet = -1;
			break;
		}
	}
	
	return (void *)nRet;
}

static int nvr_p2p_open_real_stream(jdt_long lHandle, PSTJP2PReqStream pstReqParam)
{
	int iChn, iCode;
	long lReadId;
	P2P_CHN_HANDLE *pP2PHandle;

	iChn = pstReqParam->m_iChn;
	iCode = pstReqParam->m_iCode;
	j_debug("p2p open real chn %d stream %d.", iChn, iCode);
	if(iChn >= NVR_MAX_CHANNEL_NUM || iCode < 0 || iCode >= NVR_MAX_STREAM_NUM || iCode < 0)
		return eJP2PETParam;
	
	if(av_pool_start_read_frame(iChn, iCode, 1000, &lReadId) < 0)
	{
		j_warning("Start to read chn %d stream %d failed.", iChn, iCode);
		return eJP2PETOperator;
	}
	pP2PHandle = (P2P_CHN_HANDLE *)&gP2PHandle[iChn][iCode];
	if(pP2PHandle->nStatus)
		return eJP2PETSuccess;
		
	pP2PHandle->nQuit = 0;
	pP2PHandle->lHandle = lHandle;
	pP2PHandle->iChn = iChn;
	pP2PHandle->iCode = iCode;
	pP2PHandle->lReadId = lReadId;
	pP2PHandle->nStatus = 1;
	pP2PHandle->lMsgType = P2P_REAL_STREAM_MSG_TYPE + iChn * NVR_MAX_STREAM_NUM + iCode;
	if(pthread_create(&pP2PHandle->thid, NULL, 
				nvr_p2p_send_real_frame_func, pP2PHandle) != 0)
	{
		j_warning("Create p2p send frame thread failed.");
		av_pool_stop_read_frame(lReadId);
		pP2PHandle->nStatus = 0;
		return eJP2PETOperator;
	}
	
	return eJP2PETSuccess;
}

static gboolean nvr_p2p_close_real_stream_timeout(gpointer user_data)
{
	int iRet;
	P2P_CHN_HANDLE *pP2PHandle = (P2P_CHN_HANDLE *)user_data;
	
	iRet = pthread_kill(pP2PHandle->thid, 0);
	if(iRet == ESRCH) // thread already quit.
	{
		//pthread_join(pP2PHandle->thid, NULL);
		av_pool_stop_read_frame(pP2PHandle->lReadId);
		pP2PHandle->lHandle = 0;
		pP2PHandle->nStatus = 0;
		return G_SOURCE_REMOVE;
	}
	return G_SOURCE_CONTINUE;
}

static int nvr_p2p_close_real_stream(jdt_long lHandle)
{
	int iChn;
	int iCode;
	int iRet;
	P2P_CHN_HANDLE *pP2PHandle;
	
	j_debug("p2p close real stream.");
	for(iChn = 0; iChn < NVR_MAX_CHANNEL_NUM; iChn++)
	{
		for(iCode = 0; iCode < NVR_MAX_STREAM_NUM; iCode++)
		{
			pP2PHandle = &gP2PHandle[iChn][iCode];
			if(pP2PHandle->nStatus && pP2PHandle->lHandle == lHandle)
			{
				pP2PHandle->nQuit = 1;
				iRet = pthread_kill(pP2PHandle->thid, 0);
				if(iRet == ESRCH) // thread already quit.
				{
					//pthread_join(pP2PHandle->thid, NULL);
					av_pool_stop_read_frame(pP2PHandle->lReadId);
					pP2PHandle->lHandle = 0;
					pP2PHandle->nStatus = 0;
				}
				else
				{
					g_timeout_add_seconds(1, nvr_p2p_close_real_stream_timeout, pP2PHandle);
				}
				j_debug("p2p close real stream successfully.");
				return eJP2PETSuccess;
			}
		}
	}
	
	return eJP2PETOperator;
}

static int nvr_p2p_ctrl_real_stream(jdt_long lHandle,PSTJP2PCtrlStream pstCtrlStream)
{
	int iChn;
	int iCode;
	P2P_CHN_HANDLE *pP2PHandle;
	P2P_CMD_MSG stCmdMsg;
	
	j_debug("p2p ctrl real stream.");
	for(iChn = 0; iChn < NVR_MAX_CHANNEL_NUM; iChn++)
	{
		for(iCode = 0; iCode < NVR_MAX_STREAM_NUM; iCode++)
		{
			pP2PHandle = &gP2PHandle[iChn][iCode];
			if(pP2PHandle->nStatus && pP2PHandle->lHandle == lHandle)
			{
				memcpy(&stCmdMsg.stCmd, pstCtrlStream, sizeof(stCmdMsg.stCmd));
				if(nvr_p2p_send_msg(pP2PHandle->lMsgType, &stCmdMsg) < 0)
					return eJP2PETOperator;
				j_debug("p2p ctrl real stream successfully.");
				return eJP2PETSuccess;
			}
		}
	}
	
	return eJP2PETOperator;
}

static P2P_CHN_HANDLE* nvr_p2p_alloc_history_handle()
{
	int i;

	for(i = 0; i < P2P_HISTORY_HANDLE_NUM; i++)
	{
		if(gP2PHistoryHandle[i].nStatus == 0)
		{
			memset(&gP2PHistoryHandle[i], 0, sizeof(P2P_CHN_HANDLE));
			gP2PHistoryHandle[i].nStatus = 1;	
			gP2PHistoryHandle[i].lMsgType = P2P_HISTORY_STREAM_MSG_TYPE + i;
			return&gP2PHistoryHandle[i];
		}
	}
	return NULL;
}

static void nvr_p2p_free_history_handle(P2P_CHN_HANDLE *pP2PHandleMap)
{
	if(pP2PHandleMap == NULL)
		return;
	pP2PHandleMap->nStatus = 0;
}


#define P2P_HISTORY_PLAY_START	0
#define P2P_HISTORY_PLAY_PAUSE	1
#define P2P_HISTORY_PLAY_FRAME	2
#define P2P_HISTORY_PLAY_STOP		3
static void *nvr_p2p_send_history_frame_func(void *data)
{
	P2P_CHN_HANDLE *pP2PHandle = (P2P_CHN_HANDLE *)data;
	long lReadId;
	uint8_t *pFrameBuf;
	int nFrameSize;
	AV_FRAME_HEAD_S *pFrameHead;
	P2P_CMD_MSG stCmdMsg;
	uint32_t u32PreMs = 0;
	uint32_t u32FrameMs, u32SleepMs;
	uint32_t u32SleepNs;
	uint8_t u8FrameType;
	uint8_t u8SendFrame = 0;
	int	nPlayStatus = P2P_HISTORY_PLAY_PAUSE;
	eJP2PPlaySpeed	ePlaySpeed = eJP2PPlaySpeed_1X;
	struct timespec tp;
	StorageFrameHdr stStrgFrameHdr;
	StorageReadCmd nReadCmd = READ_NORMAL;
	int nRet = 0;

	j_debug("Start send record %llu - %llu.", pP2PHandle->m_stStartTime, pP2PHandle->m_stStopTime);
	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "P2PHistorySend");
	
	lReadId = pP2PHandle->lReadId;
	while(!pP2PHandle->nQuit)
	{
		if(nvr_p2p_recv_msg(pP2PHandle->lMsgType, &stCmdMsg) > 0)
		{
			j_debug("Receive cmd message, message cmd: %d", stCmdMsg.stCmd.m_iCtrlCode);
			switch(stCmdMsg.stCmd.m_iCtrlCode)
			{
				case eJP2PSCAudioOnly:
					break;
				case eJP2PSCIFrameOnly:
					break;
				case eJP2PSCPBStart:
					nPlayStatus = P2P_HISTORY_PLAY_START;
					ePlaySpeed = eJP2PPlaySpeed_1X;
					nReadCmd = READ_NORMAL;
					break;
				case eJP2PSCPBStop:
					nPlayStatus = P2P_HISTORY_PLAY_STOP;
					break;
				case eJP2PSCPBPause:
					nPlayStatus = P2P_HISTORY_PLAY_PAUSE;
					break;
				case eJP2PSCPBResume:
					nPlayStatus = P2P_HISTORY_PLAY_START;
					u32PreMs = 0;
					break;
				case eJP2PSCPBSetSpeed:
					nPlayStatus = P2P_HISTORY_PLAY_START;
					u32PreMs = 0;
					ePlaySpeed = stCmdMsg.stCmd.m_dwCtrlValue1;
					j_debug("play speed: %d", ePlaySpeed);
					break;
				case eJP2PSCPBGetSpeed:
					////////////////////
					// TBD
					break;
				case eJP2PSCPBFrame:
					nPlayStatus = P2P_HISTORY_PLAY_FRAME;
					u8SendFrame = 1;
					break;
				case eJP2PSCPBSetTime:
					j_debug("start time: %ld, end time: %ld, set time: %ld", 
								pP2PHandle->stFragInfo.begin_time,
								pP2PHandle->stFragInfo.end_time,
								stCmdMsg.stCmd.m_dwCtrlValue1);
					if(stCmdMsg.stCmd.m_dwCtrlValue1 >= pP2PHandle->stFragInfo.begin_time &&
						stCmdMsg.stCmd.m_dwCtrlValue1 <= pP2PHandle->stFragInfo.end_time)
						storage_lib_read_set(lReadId, stCmdMsg.stCmd.m_dwCtrlValue1);
					u32PreMs = 0;
					break;
				case eJP2PSCPBGetTime:
					////////////////////
					// TBD
					break;
				default:
					break;
			}
		}
		if(nPlayStatus == P2P_HISTORY_PLAY_START)
		{
			if(ePlaySpeed >= eJP2PPlaySpeed_8X && ePlaySpeed <= eJP2PPlaySpeed_32X)
				nReadCmd = READ_FORWARD_1_IFRAME;
			else
				nReadCmd = READ_NORMAL;
		}
		else if(nPlayStatus == P2P_HISTORY_PLAY_PAUSE)
		{
			usleep(10000);
			continue;
		}
		else if(nPlayStatus == P2P_HISTORY_PLAY_FRAME)
		{
			if(u8SendFrame == 0)
			{
				usleep(1000);
				continue;
			}
		}
		else if(nPlayStatus == P2P_HISTORY_PLAY_STOP)
			break;
		nFrameSize = storage_lib_read_frame(lReadId, nReadCmd, &stStrgFrameHdr, &pFrameBuf);
		if(nFrameSize < 0)
		{
			j_warning("Storage read frame failed.");
			nRet = -1;
			break;
		}
		else if(nFrameSize == 0)
		{
			j_message("Storage read the fragment end.");
			nRet = 0;
			break;
		}
		//j_debug("Read frame size %d\n", nFrameSize);
		pFrameHead = (AV_FRAME_HEAD_S *)pFrameBuf;
		if(pFrameHead->u16FrameFlag != 0x3448)
		{
			j_warning("The frame flag is wrong.");
			nRet = -1;
			break;
		}
		u8FrameType = pFrameHead->u8FrameType;
		u32FrameMs = pFrameHead->u64TimeStamp / 1000;
		nFrameSize = pFrameHead->u32FrameSize;
		if(u8FrameType == AV_AUDIO_FRAME_A && ePlaySpeed != eJP2PPlaySpeed_1X)
			continue;
		if(u8FrameType != AV_AUDIO_FRAME_A
			&& u32PreMs != 0 
			&& nPlayStatus == P2P_HISTORY_PLAY_START
			&& u32FrameMs - u32PreMs > 0  
			&& u32FrameMs - u32PreMs < 10000)
		{
			u32SleepMs = u32FrameMs  - u32PreMs;
			switch(ePlaySpeed)
			{
				case eJP2PPlaySpeed_1_16X:
					u32SleepMs *= 16;
					break;
				case eJP2PPlaySpeed_1_8X:
					u32SleepMs *= 8;
					break;
				case eJP2PPlaySpeed_1_4X:
					u32SleepMs *= 4;
					break;
				case eJP2PPlaySpeed_1_2X:
					u32SleepMs *= 2;
					break;
				case eJP2PPlaySpeed_1X:
					break;
				case eJP2PPlaySpeed_2X:
					u32SleepMs /= 2;
					break;
				case eJP2PPlaySpeed_4X:
					u32SleepMs /= 4;
					break;
				case eJP2PPlaySpeed_8X:
					u32SleepMs /= 8;
					break;
				case eJP2PPlaySpeed_16X:
					u32SleepMs /= 16;
					break;
				case eJP2PPlaySpeed_32X:
					u32SleepMs /= 32;
					break;
				default:
					break;
			}
			//j_debug("sleep ms: %u", u32SleepMs);
			clock_gettime(CLOCK_MONOTONIC, &tp);
			tp.tv_sec = tp.tv_sec + u32SleepMs / 1000;
			u32SleepNs = (uint32_t)tp.tv_nsec + (u32SleepMs % 1000) * 1000000;
			if(u32SleepNs < 1000000000UL)
				tp.tv_nsec = u32SleepNs;
			else
			{
				tp.tv_sec += 1;
				tp.tv_nsec = u32SleepNs % 1000000000UL;
			}
			pthread_mutex_lock(&pP2PHandle->mutex);
			pthread_cond_timedwait(&pP2PHandle->cond, &pP2PHandle->mutex, &tp);
			pthread_mutex_unlock(&pP2PHandle->mutex);
		}
		pFrameHead->u64TimeStamp = (unsigned long long)stStrgFrameHdr.ts.tv_sec * 1000000 + stStrgFrameHdr.ts.tv_usec;
		pFrameHead->u32Time = stStrgFrameHdr.ts.tv_sec;
		//j_debug("Send record frame, frame size: %d, frame type: %u, frame time: %lu", nFrameSize, u8FrameType, pFrameHead->u32Time);
		if(JP2P_OnSendFrame(pFrameBuf, nFrameSize+sizeof(AV_FRAME_HEAD_S), pP2PHandle->lHandle) < 0)
		{
			j_warning("Failed to send frame.");
			nRet = -1;
			break;
		}
		if(u8FrameType != AV_AUDIO_FRAME_A)
			u32PreMs = u32FrameMs;
		if(nPlayStatus == P2P_HISTORY_PLAY_FRAME)
			u8SendFrame = 0;
	}
	JP2P_OnSendFrame(NULL, 0, pP2PHandle->lHandle);
	j_debug("history thread exit.");
	
	return (void *)nRet;
}

static int nvr_p2p_open_history_stream(jdt_long lHandle, PSTJP2PReqStream pstReqParam)
{
	int iChn, iCode;
	int bFind = 0;
	P2P_CHN_HANDLE *pP2PHandle;
	pthread_condattr_t condattr;
	time_t nStartTime, nStopTime;
	int nShmid;
	int i, nQueryNum;
	RecordQueryChnInfo *pChnInfo;
	RecordQueryInfo *pQueryInfo;
	StorageFragmentInfo stFragInfo;
	long lReadId;
	
	j_debug("################### p2p open history stream.");
	iChn = pstReqParam->m_iChn;
	iCode = pstReqParam->m_iCode;
	if(iChn != 0 || iCode < 0 || iCode > 2)
		return eJP2PETParam;

	j_debug("Play storage begin time: %d-%d-%d %d:%d:%d",
						pstReqParam->m_stStartTime.m_sYear + 1900,
						pstReqParam->m_stStartTime.m_sMonth + 1,
						pstReqParam->m_stStartTime.m_sDay,
						pstReqParam->m_stStartTime.m_sHour,
						pstReqParam->m_stStartTime.m_sMinute,
						pstReqParam->m_stStartTime.m_sSecond);
	j_debug("Play storage end time: %d-%d-%d %d:%d:%d",
						pstReqParam->m_stStopTime.m_sYear + 1900,
						pstReqParam->m_stStopTime.m_sMonth + 1,
						pstReqParam->m_stStopTime.m_sDay,
						pstReqParam->m_stStopTime.m_sHour,
						pstReqParam->m_stStopTime.m_sMinute,
						pstReqParam->m_stStopTime.m_sSecond);
	nStartTime = nvr_p2p_make_time(&pstReqParam->m_stStartTime);
	nStopTime = nvr_p2p_make_time(&pstReqParam->m_stStopTime);
	if((nQueryNum = storage_lib_record_query((1<<iChn), nStartTime, nStopTime, RECORD_ALL_TYPE, 
							&nShmid, &pChnInfo, &pQueryInfo)) < 0)
	{
		j_warning("Start query failed.");
		return eJP2PETParam;
	}
	j_debug("nQueryNum: %d\n", nQueryNum);
	for(i = 0; i < nQueryNum; i++)
	{
		if(pQueryInfo[i].chn == iChn &&
			pQueryInfo[i].begin_time == nStartTime &&
			pQueryInfo[i].end_time == nStopTime)
		{
			bFind = 1;
			break;
		}
	}
	if(bFind == 0)
	{
		j_warning("Don't find the record fragment.");
		storage_lib_record_query_free(nShmid, pChnInfo, pQueryInfo);
		return eJP2PETParam;
	}
	
	if(storage_lib_read_start(&pQueryInfo[i], &stFragInfo, &lReadId) < 0)
	{
		j_warning("Storage read start failed.");
		storage_lib_record_query_free(nShmid, pChnInfo, pQueryInfo);
		return eJP2PETOperator;
	}
	storage_lib_record_query_free(nShmid, pChnInfo, pQueryInfo);
		
	pP2PHandle = nvr_p2p_alloc_history_handle();
	if(pP2PHandle == NULL)
	{
		j_warning("Don't find the idle handle.");
		storage_lib_read_stop(lReadId);
		return eJP2PETOperator;
	}
	
	pP2PHandle->nQuit = 0;
	pP2PHandle->lHandle = lHandle;
	pP2PHandle->iChn = pstReqParam->m_iChn;
	pP2PHandle->iCode = pstReqParam->m_iCode;
	pP2PHandle->m_stStartTime = nStartTime;
	pP2PHandle->m_stStopTime = nStopTime;
	pP2PHandle->m_dwRecType = pstReqParam->m_dwRecType;
	pP2PHandle->lReadId = lReadId;
	memcpy(&pP2PHandle->stFragInfo, &stFragInfo, sizeof(stFragInfo));
	pthread_condattr_init(&condattr);
	pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC);
	pthread_cond_init(&pP2PHandle->cond, &condattr);
	pthread_condattr_destroy(&condattr);
	pthread_mutex_init(&pP2PHandle->mutex, NULL);
	if(pthread_create(&pP2PHandle->thid, NULL, 
				nvr_p2p_send_history_frame_func, pP2PHandle) != 0)
	{
		j_warning("Create p2p send frame thread failed.");
		storage_lib_read_stop(lReadId);
		pthread_mutex_destroy(&pP2PHandle->mutex);
		pthread_cond_destroy(&pP2PHandle->cond);
		nvr_p2p_free_history_handle(pP2PHandle);
		return eJP2PETOperator;
	}
	
	return eJP2PETSuccess;
}

static gboolean nvr_p2p_close_history_stream_timeout(gpointer user_data)
{
	int iRet;
	P2P_CHN_HANDLE *pP2PHandle = (P2P_CHN_HANDLE *)user_data;
	
	iRet = pthread_kill(pP2PHandle->thid, 0);
	if(iRet == ESRCH) // thread already quit.
	{
		//pthread_join(pP2PHandle->thid, NULL);
		storage_lib_read_stop(pP2PHandle->lReadId);
		pthread_mutex_destroy(&pP2PHandle->mutex);
		pthread_cond_destroy(&pP2PHandle->cond);
		nvr_p2p_free_history_handle(pP2PHandle);
		pP2PHandle->lHandle = 0;
		pP2PHandle->nStatus = 0;
		
		return G_SOURCE_REMOVE;
	}
	return G_SOURCE_CONTINUE;
}

static int nvr_p2p_close_history_stream(jdt_long lHandle)
{
	int i;
	int iRet;
	P2P_CHN_HANDLE *pP2PHandle;
	
	j_debug("p2p close history stream.");
	for(i = 0; i < P2P_HISTORY_HANDLE_NUM; i++)
	{
		pP2PHandle = &gP2PHistoryHandle[i];
		if(pP2PHandle->nStatus == 1 && pP2PHandle->lHandle == lHandle)
		{
			pP2PHandle->nQuit = 1;
			pthread_cond_signal(&pP2PHandle->cond);
			iRet = pthread_kill(pP2PHandle->thid, 0);
			if(iRet == ESRCH) // thread already quit.
			{
				//pthread_join(pP2PHandle->thid, NULL);
				storage_lib_read_stop(pP2PHandle->lReadId);
				nvr_p2p_free_history_handle(pP2PHandle);
				pthread_mutex_destroy(&pP2PHandle->mutex);
				pthread_cond_destroy(&pP2PHandle->cond);
				pP2PHandle->lHandle = 0;
				pP2PHandle->nStatus = 0;
			}
			else
			{
				g_timeout_add_seconds(1, nvr_p2p_close_history_stream_timeout, pP2PHandle);
			}
			j_debug("p2p close history stream successfully.");
			return eJP2PETSuccess;
		}
	}
	return eJP2PETOperator;
}

static int nvr_p2p_ctrl_history_stream(jdt_long lHandle,PSTJP2PCtrlStream pstCtrlStream)
{
	int i;
	P2P_CMD_MSG stCmdMsg;
	P2P_CHN_HANDLE *pP2PHandle;
	
	j_debug("p2p ctrl history stream.");
	for(i = 0; i < P2P_HISTORY_HANDLE_NUM; i++)
	{
		pP2PHandle = &gP2PHistoryHandle[i];
		if(pP2PHandle->nStatus == 1 && pP2PHandle->lHandle == lHandle)
		{
			memcpy(&stCmdMsg.stCmd, pstCtrlStream, sizeof(stCmdMsg.stCmd));
			if(nvr_p2p_send_msg(pP2PHandle->lMsgType, &stCmdMsg) < 0)
				return eJP2PETOperator;
			pthread_cond_signal(&pP2PHandle->cond);
			j_debug("p2p ctrl history stream successfully.");
			return eJP2PETSuccess;
		}
	}
	return eJP2PETSuccess;
}

// 请求打开流回调
static int nvr_p2p_open_stream(jdt_long lHandle, PSTJP2PReqStream pstReqParam)
{
	int nRet = eJP2PETOperator;

	j_debug("begin p2p open stream, handle: 0x%lx.", lHandle);
	if(pstReqParam == NULL)
	{
		j_debug("end p2p open stream, handle: 0x%lx, failed.", lHandle);
		return eJP2PETParam;
	}
	switch(pstReqParam->m_iStreamType)
	{
		case eJP2PStreamReal:
			nRet = nvr_p2p_open_real_stream(lHandle, pstReqParam);
			break;
		case eJP2PStreamHistore:
			nRet = nvr_p2p_open_history_stream(lHandle, pstReqParam);
			break;
		case eJP2PStreamTalk:
			break;
		default:
			break;
	}
	
	j_debug("end p2p open stream, handle: 0x%lx, successfully.", lHandle);
	return nRet;
}

// 请求打开流回调
static int nvr_p2p_close_stream(jdt_long lHandle)
{
	int iRet;
	
	j_debug("begin p2p close stream, handle: 0x%lx.", lHandle);
	iRet = nvr_p2p_close_real_stream(lHandle);
	if(iRet >= 0)
	{
		j_debug("end p2p close real stream, handle: 0x%lx, successfully.", lHandle);
		return iRet;
	}
	iRet = nvr_p2p_close_history_stream(lHandle);
	if(iRet >= 0)
	{
		j_debug("end p2p close history stream, handle: 0x%lx, successfully.", lHandle);
		return iRet;	
	}
	j_warning("end p2p close stream, handle: 0x%lx, failed.", lHandle);
	
	return eJP2PETOperator;
}

// 请求控制流回调
static int nvr_p2p_ctrl_stream(jdt_long lHandle, PSTJP2PCtrlStream pstCtrlStream)
{
	int iRet;
	
	j_debug("begin p2p ctrl stream, handle: 0x%lx.", lHandle);
	iRet = nvr_p2p_ctrl_real_stream(lHandle, pstCtrlStream);
	if(iRet >= 0)
	{
		j_debug("end p2p ctrl stream, handle: 0x%lx, successfully.", lHandle);
		return iRet;
	}
	iRet = nvr_p2p_ctrl_history_stream(lHandle, pstCtrlStream);
	if(iRet >= 0)
	{
		j_debug("end p2p ctrl stream, handle: 0x%lx, successfully.", lHandle);
		return iRet;
	}
	j_warning("end p2p ctrl stream, handle: 0x%lx, failed.", lHandle);
	
	return eJP2PETOperator;
}

// 向设备发送帧数据回调
static int nvr_p2p_send_frame(jdt_long lHandle, jdt_lpvoid lpData, jdt_int iDataSize)
{
	int nHdrLen = sizeof(struct stJP2PStreamData);
	struct stJP2PStreamData stHdr;

	j_debug("p2p receive a frame.");
	memcpy(&stHdr, lpData, nHdrLen);
	av_pool_write_frame(gP2PTalkHandle.lWriteId, lpData+nHdrLen, iDataSize-nHdrLen,
							(unsigned char *)lpData, nHdrLen,
							stHdr.m_dwFrameNo, stHdr.m_byFrameType, stHdr.m_dwTimeStamp);
	return eJP2PETSuccess;
}

static int nvr_p2p_open_talk_stream(jdt_long lHandle, PSTJP2PReqTalk pstReqParam)
{
	long lReadId, lWriteId;
	
	j_debug("p2p open talk stream1111.");
	if(av_pool_start_read_frame(AV_POOL_TALK_CHN, AV_POOL_TALK_OUT_STREAM, 1000, &lReadId) < 0)
	{
		j_warning("Start read talk frame failed.");
		return eJP2PETOperator;
	}
	if(av_pool_start_write_frame(AV_POOL_TALK_CHN, AV_POOL_TALK_IN_STREAM, &lWriteId) < 0)
	{
		j_warning("Start write talk frame failed, maybe the talk is busy.");
		av_pool_stop_read_frame(lReadId);
		return eJP2PETOperator;
	}
	player_start_audio_record();

	gP2PTalkHandle.lReadId = lReadId;
	gP2PTalkHandle.lWriteId = lWriteId;
	
	j_debug("p2p open talk stream2222.");
	return eJP2PETSuccess;
}

static int nvr_p2p_close_talk_stream(jdt_long lHandle)
{
	j_debug("p2p close talk stream111.");
	player_stop_audio_record();
	av_pool_stop_read_frame(gP2PTalkHandle.lReadId);
	av_pool_stop_read_frame(gP2PTalkHandle.lWriteId);
	j_debug("p2p close talk stream222");
	return eJP2PETSuccess;
}

int nvr_p2p_tutk_init()
{
	char		szUID[128];
	struct stJP2PInit	stInit;
	NVR_DEV_STABLE_INFO_S stStableInfo;

	gMsgQid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
	if(gMsgQid < 0)
	{
		j_warning("Create p2p msg queue failed.");
		return -1;
	}
	memset(&gP2PHandle, 0, sizeof(gP2PHandle));
	// 打开服务 begin
	stInit.m_fcbLogTrace		= nvr_p2p_log;				// 日志回调
	stInit.m_fcbLoginAuth		= nvr_p2p_auth;				// 登录认证回调
	stInit.m_fcbGetParam		= nvr_p2p_get_param;				// 获取设备参数回调
	stInit.m_fcbSetParam		= nvr_p2p_set_param;				// 设置设备参数回调
	stInit.m_fcbOpenStream	= nvr_p2p_open_stream;				// 请求打开流回调
	stInit.m_fcbCloseStream 	= nvr_p2p_close_stream; 			// 请求打开流回调
	stInit.m_fcbCtrlStream	= nvr_p2p_ctrl_stream;				// 请求控制流回调
	stInit.m_fcbSendFrame	= nvr_p2p_send_frame;				// 向设备发送帧数据回调
	stInit.m_fcbOpenTalk		= nvr_p2p_open_talk_stream;
	stInit.m_fcbCloseTalk		= nvr_p2p_close_talk_stream;
	stInit.m_iMaxChn			= NVR_MAX_CHANNEL_NUM;							// 最大通道数
	stInit.m_iMaxCode		= NVR_MAX_STREAM_NUM;							// 最大码流数
	//strcpy(stInit.m_szP2PHost[0],"50.19.254.134");				// P2P服务器1
	//strcpy(stInit.m_szP2PHost[1],"122.248.234.207");			// P2P服务器2
	//strcpy(stInit.m_szP2PHost[2],"122.226.84.253"); 			// P2P服务器3
	//strcpy(stInit.m_szP2PHost[3],"61.188.37.216");				// P2P服务器4
	
	nvr_logon_get_stable_cfg(0, &stStableInfo);
	j_debug("Device serial ID: %s", (char *)stStableInfo.szSerialId);
	strcpy(szUID, (char *)stStableInfo.szSerialId);
	//strcpy(szUID, "IPNC13511100003");
	//strcpy(szUID, "CNVR0123456789");
	if ( JP2P_StartSvr(&stInit, szUID, TUTK_SERVER_IP, TUTK_SERVER_PORT) != eJP2PETSuccess )
	{
		j_warning("p2p start failed.");
		return -1;
	}

	return 0;
}


int nvr_p2p_tutk_uninit()
{
	JP2P_StopSvr();
	msgctl( gMsgQid, IPC_RMID, NULL);
	
	return 0;
}



