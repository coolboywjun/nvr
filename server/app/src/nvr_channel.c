
#include <nvr_server_headers.h>
#include <nvr_param_proc.h>
#include <ipnc_lib_client.h>
#include <nvr_net_search.h>

#include <av_pool.h>


typedef struct _NVR_CHANNEL_INFO_S_
{
	NVR_CHANNEL_CFG_S	struChannelCfg;
	long 	lHandle;
	long 	lIpncHandle;
	long		lLongonHandle;
	long 	lStreamHandle[NVR_MAX_STREAM_NUM];
	gint		nOpenStreamCnt[NVR_MAX_STREAM_NUM];
	GMutex	mutex;
}NVR_CHANNEL_INFO_S;

typedef struct _NVR_CHANNEL_PARAM_S_
{
	int nChn;
	IPNC_CMD_TYPE_E cmd;
	void *data;
	int dataSize;
}NVR_CHANNEL_PARAM_S;

typedef enum _NVR_CHANNEL_CTRL_CMD_E_
{
	NVR_CHANNEL_STATUS_CHECK 	= 0x01,//通道状态检测
	NVR_CHANNEL_SET_PARAM,
	NVR_CHANNEL_GET_PARAM,
	NVR_CHANNEL_START_READ,
	NVR_CHANNEL_STOP_READ,
}NVR_CHANNEL_CTRL_CMD_E;

static NVR_CHANNEL_INFO_S gChInfo[NVR_MAX_CHANNEL_NUM];
static GThreadPool *gChPool;
static GThread *gAvMsgThread;

static int nvr_auto_set_ipnc_params(int nChn, NVR_CHANNEL_INFO_S *pChInfo, NVR_CHANNEL_CFG_S *pChCfg)
{
	NVR_TIME_S stTimeCfg;
	IPNC_DEV_MD_CFG_S stMdCfg;
	IPNC_DEV_VENC_CFG_S stMainStreamCfg;

	// 如果使用了看店平台，不自动修改ipc的这些参数
	if(!nvr_get_dianxin_is_used())
	{
		// 设置ipnc时间
		nvr_get_time_cfg(0, &stTimeCfg);
		ipnc_client_set_param(pChInfo->lHandle, pChInfo->lLongonHandle, 
							IPNC_CMD_TIME_CFG, &stTimeCfg, sizeof(IPNC_TIME_S));
		// 设置md
		if(ipnc_client_get_param(pChInfo->lHandle, pChInfo->lLongonHandle, 
						IPNC_CMD_VMD_CFG, &stMdCfg, sizeof(stMdCfg)) >= 0)
		{
			stMdCfg.u8Enable = 1;
			stMdCfg.u8Level = 1;
			stMdCfg.struRect[0].s32X = 0;
			stMdCfg.struRect[0].s32Y = 0;
			stMdCfg.struRect[0].u32Width = 704;
			stMdCfg.struRect[0].u32Height = 576;
			stMdCfg.struSeg[0][0].u8Open = 1;
			stMdCfg.struSeg[0][0].beginSec = 0;
			stMdCfg.struSeg[0][0].endSec = 24 * 60 * 60 - 1;
			stMdCfg.u32DetectInterval = 1;
			ipnc_client_set_param(pChInfo->lHandle, pChInfo->lLongonHandle, 
							IPNC_CMD_VMD_CFG, &stMdCfg, sizeof(stMdCfg));
		}
	}	
	// 强制帧率不超过25fps
	if(ipnc_client_get_param(pChInfo->lHandle, pChInfo->lLongonHandle,
						IPNC_CMD_VMAIN_CFG, &stMainStreamCfg, sizeof(stMainStreamCfg)) >= 0)
	{
		int bChange = 0;
		if(stMainStreamCfg.u8FrameRate > 25)
		{
			stMainStreamCfg.u8FrameRate = 25;
			bChange = 1;
		}
		if(stMainStreamCfg.u16Width != 1280 || stMainStreamCfg.u16Height != 720)
		{
			stMainStreamCfg.u16Width = 1280;
			stMainStreamCfg.u16Height = 720;
			bChange = 1;
		}
		if(stMainStreamCfg.u8VideoEncType != 0)
		{
			stMainStreamCfg.u8VideoEncType = 0;
			bChange = 1;
		}
		if(stMainStreamCfg.u32BitrateRate > 4 * 1024 * 1024)
		{
			stMainStreamCfg.u32BitrateRate = 4 * 1024 * 1024;
			bChange = 1;
		}
		if(stMainStreamCfg.u8Gop < 10)
		{
			stMainStreamCfg.u8Gop = 10;
			bChange = 1;
		}
		if(bChange)
		{
			ipnc_client_set_param(pChInfo->lHandle, pChInfo->lLongonHandle,
						IPNC_CMD_VMAIN_CFG, &stMainStreamCfg, sizeof(stMainStreamCfg));
		}
	}

	return 0;
}

// 检查此通道的远程ipnc协议是否仍然是这个协议
static int nvr_check_handle(NVR_CHANNEL_INFO_S *pChInfo, NVR_CHANNEL_CFG_S *pChCfg)
{
	NVR_CHANNEL_CFG_S *pCfg = &pChInfo->struChannelCfg;
	long 	lHandle = pChInfo->lHandle;
	long 	lIpncHandle = pChInfo->lIpncHandle;
	long		lLononHandle = pChInfo->lLongonHandle;
	long 	lStreamHandle[2];

	// 判断是否关闭此通道的所有流后重启这个通道
	if((lHandle == 0) || 
		(lHandle != 0 && 
		((strcmp(pCfg->szProtocolName, pChCfg->szProtocolName) != 0) ||
		(strcmp(pCfg->szUser, pChCfg->szUser) != 0) ||
		(strcmp(pCfg->szPasswd, pChCfg->szPasswd) != 0))))
	{
		if(lHandle != 0)
		{
			lStreamHandle[0] = pChInfo->lStreamHandle[0];
			lStreamHandle[1] = pChInfo->lStreamHandle[1];
			if(lStreamHandle[0] != 0)
				ipnc_client_stop_get_stream(lHandle, lIpncHandle, lStreamHandle[0]);
			if(lStreamHandle[1] != 0)
				ipnc_client_stop_get_stream(lHandle, lIpncHandle, lStreamHandle[1]);
			if(lLononHandle != 0)
				ipnc_client_logout_ipnc(lHandle, lLononHandle);
			if(lIpncHandle != 0)
				ipnc_client_disconnect_ipnc(lHandle, lIpncHandle);
			ipnc_client_disconnect_server(lHandle);
			pChInfo->lHandle = 0;
			pChInfo->lIpncHandle = 0;
			pChInfo->lLongonHandle = 0;
			pChInfo->lStreamHandle[0] = 0;
			pChInfo->lStreamHandle[1] = 0;
		}
		if(ipnc_client_connect_server(pChCfg->szProtocolName, &pChInfo->lHandle) < 0)
			return -1;
	}

	return 0;
}

// 检查是否连接的仍然是这个摄像头,如果不是则关闭所有的流后重新拉流
static int nvr_check_ipnc_handle(int nPoolCh, NVR_CHANNEL_INFO_S *pChInfo, NVR_CHANNEL_CFG_S *pChCfg)
{
	NVR_CHANNEL_CFG_S *pCfg = &pChInfo->struChannelCfg;
	long 	lHandle = pChInfo->lHandle;
	long 	lIpncHandle = pChInfo->lIpncHandle;
	long	lLononHandle = pChInfo->lLongonHandle;
	long 	lStreamHandle[2];
	char		szIpAddr[32];

	if(lHandle == 0)
		return -1;
	if((lIpncHandle == 0) ||
		((lIpncHandle != 0) && 
		(pCfg->u32IpAddr != pChCfg->u32IpAddr ||
		pCfg->u16DataPort != pChCfg->u16DataPort ||
		pCfg->u16RtspPort != pChCfg->u16RtspPort ||
		pCfg->u8UseChn != pChCfg->u8UseChn)))
	{
		if(lIpncHandle != 0)
		{
			lStreamHandle[0] = pChInfo->lStreamHandle[0];
			lStreamHandle[1] = pChInfo->lStreamHandle[1];
			if(lStreamHandle[0] != 0)
				ipnc_client_stop_get_stream(lHandle, lIpncHandle, lStreamHandle[0]);
			if(lStreamHandle[1] != 0)
				ipnc_client_stop_get_stream(lHandle, lIpncHandle, lStreamHandle[1]);
			if(lLononHandle != 0)
				ipnc_client_logout_ipnc(lHandle, lLononHandle);
			ipnc_client_disconnect_ipnc(lHandle, lIpncHandle);
			pChInfo->lIpncHandle = 0;
			pChInfo->lLongonHandle = 0;
			pChInfo->lStreamHandle[0] = 0;
			pChInfo->lStreamHandle[1] = 0;
		}
		sprintf(szIpAddr, "%d.%d.%d.%d", 
				(pChCfg->u32IpAddr >> 24) & 0xff,
				(pChCfg->u32IpAddr >> 16) & 0xff,
				(pChCfg->u32IpAddr >> 8) & 0xff,
				(pChCfg->u32IpAddr >> 0) & 0xff);
		if(ipnc_client_connect_ipnc(lHandle, nPoolCh, szIpAddr,
									pChCfg->u16DataPort, pChCfg->szUser,
									pChCfg->szPasswd, &pChInfo->lIpncHandle) < 0)
			return -1;
	}

	return 0;
}

static int nvr_check_logon_handle(int nChn, NVR_CHANNEL_INFO_S *pChInfo, NVR_CHANNEL_CFG_S *pChCfg)
{
	long 	lHandle = pChInfo->lHandle;
	long 	lIpncHandle = pChInfo->lIpncHandle;
	long 	lLongonHandle = pChInfo->lLongonHandle;

	if(lLongonHandle == 0 && lHandle != 0 && lIpncHandle != 0)
	{
		int ret;
		ret = ipnc_client_logon_ipnc(lHandle, lIpncHandle, &pChInfo->lLongonHandle);
		if(ret >= 0)
		{
			nvr_auto_set_ipnc_params(nChn, pChInfo, pChCfg);
		}
		return ret;
	}
	
	return 0;
}

static int nvr_check_stream_handle(NVR_CHANNEL_INFO_S *pChInfo, NVR_CHANNEL_CFG_S *pChCfg)
{
	int 	i, nRet = 0;
	long 	lHandle = pChInfo->lHandle;
	long 	lIpncHandle = pChInfo->lIpncHandle;
	long 	lStreamHandle;

	for(i = 0; i < NVR_MAX_STREAM_NUM; i++)
	{
		lStreamHandle = pChInfo->lStreamHandle[i];
		if(lHandle != 0 && lIpncHandle != 0)
		{
			if(lStreamHandle == 0 && g_atomic_int_get(&pChInfo->nOpenStreamCnt[i]) > 0)
			{
				nRet = ipnc_client_start_get_stream(lHandle, lIpncHandle, pChCfg->u8UseChn, 
													i, &pChInfo->lStreamHandle[i]);
			}
			else if(lStreamHandle != 0 && g_atomic_int_get(&pChInfo->nOpenStreamCnt[i]) == 0)
			{
				nRet = ipnc_client_stop_get_stream(lHandle, lIpncHandle, lStreamHandle);
				pChInfo->lStreamHandle[i] = 0;
			}
		}
	}
	
	return nRet;
}

static void nvr_channel_close_all_handle(NVR_CHANNEL_INFO_S *pChnInfo)
{
	if(pChnInfo->lStreamHandle[0] != 0)
	{
		ipnc_client_stop_get_stream(pChnInfo->lHandle, 
								pChnInfo->lIpncHandle, 
								pChnInfo->lStreamHandle[0]);	
		pChnInfo->lStreamHandle[0] = 0;
	}
	if(pChnInfo->lStreamHandle[1] != 0)
	{
		ipnc_client_stop_get_stream(pChnInfo->lHandle, 
								pChnInfo->lIpncHandle, 
								pChnInfo->lStreamHandle[1]);
		pChnInfo->lStreamHandle[1] = 0;
	}
	if(pChnInfo->lLongonHandle != 0)
	{
		ipnc_client_logout_ipnc(pChnInfo->lHandle, 
								pChnInfo->lLongonHandle);	
		pChnInfo->lLongonHandle = 0;				
	}
	if(pChnInfo->lIpncHandle != 0)
	{
		ipnc_client_disconnect_ipnc(pChnInfo->lHandle, 
								pChnInfo->lIpncHandle);
		pChnInfo->lIpncHandle = 0;
	}
	if(pChnInfo->lHandle != 0)
	{
		ipnc_client_disconnect_server(pChnInfo->lHandle);
		pChnInfo->lHandle = 0;
	}
	memset(&pChnInfo->struChannelCfg, 0, sizeof(pChnInfo->struChannelCfg));
}

static int nvr_channel_check_status(int nChn)
{
	int i;
	NVR_CHANNEL_INFO_S 		*pChInfo;
	NVR_CHANNEL_CFG_S		struChannelCfg;
	//static gint64 lastTime = 0;

	// 如果是默认文件系统不取流
	if(nvr_get_filesystem_is_default())
		return 0;
	//if(g_get_monotonic_time() - lastTime < 1000000)	// 在1s内不再马上处理
	//	return 0;
	//lastTime = g_get_monotonic_time();
	for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{
		if(nChn >=0 && nChn != i)
			continue;
		pChInfo = &gChInfo[i];
		nvr_get_channel_cfg(i, &struChannelCfg);
		/*
		if(struChannelCfg.u8Enable && struChannelCfg.u8SearchA)
		{
			if(nvr_smart_is_ipc_remote_used(struChannelCfg.u32IpAddr) == 1)
			{
				struChannelCfg.u32IpAddr == 0;
				nvr_set_channel_cfg(i, &struChannelCfg);
			}
		}
		*/
#if 0		
		j_debug("===========================================");
		j_debug("%d struChannelCfg.szProtocolName:%s", i, struChannelCfg.szProtocolName);
		j_debug("%d struChannelCfg.szUser:%s", i, struChannelCfg.szUser);
		j_debug("%d struChannelCfg.szPasswd:%s", i, struChannelCfg.szPasswd);
		j_debug("%d struChannelCfg.u32IpAddr:%d.%d.%d.%d", i, ((struChannelCfg.u32IpAddr >> 24) & 0xff),
															  ((struChannelCfg.u32IpAddr >> 16) & 0xff),
															  ((struChannelCfg.u32IpAddr >>  8) & 0xff),
															  ( struChannelCfg.u32IpAddr & 0xff));
		j_debug("===========================================");
#endif		
		g_mutex_lock(&pChInfo->mutex);
		if(struChannelCfg.u8Enable == 0 || struChannelCfg.u32IpAddr == 0)
		{
			nvr_channel_close_all_handle(pChInfo);
			g_mutex_unlock(&pChInfo->mutex);
			continue;
		}
		if(nvr_check_handle(pChInfo, &struChannelCfg) < 0)
		{
			//j_warning("Check channel %d handle failed.", i);
			g_mutex_unlock(&pChInfo->mutex);
			continue;
		}
		if(nvr_check_ipnc_handle(i, pChInfo, &struChannelCfg) < 0)
		{
			//j_warning("Check channel %d ipnc handle failed.", i);
			g_mutex_unlock(&pChInfo->mutex);
			continue;
		}
		if(nvr_check_logon_handle(i, pChInfo, &struChannelCfg) < 0)
		{
			//j_warning("Check channel %d logon failed.", i);
			g_mutex_unlock(&pChInfo->mutex);
			continue;
		}
		if(nvr_check_stream_handle(pChInfo, &struChannelCfg) < 0)
		{
			//j_warning("Check channel %d stream failed.", i);
			g_mutex_unlock(&pChInfo->mutex);
			continue;
		}
		memcpy(&pChInfo->struChannelCfg, &struChannelCfg, sizeof(struChannelCfg));
		g_mutex_unlock(&pChInfo->mutex);
	}
	return 0;
}

static void nvr_channel_check_status_func(gpointer data, gpointer user_data)
{
	NVR_CHANNEL_CTRL_CMD_E cmdVal = GPOINTER_TO_INT(data) & 0xffff;
	int nChn = GPOINTER_TO_INT(data) >> 16;
	switch(cmdVal)
	{
		case NVR_CHANNEL_STATUS_CHECK:
			nvr_channel_check_status(nChn);
			break;
		default:
			g_assert_not_reached();
			break;
	}
}

static void nvr_channel_check(int nChn)
{
	g_thread_pool_push(gChPool, GINT_TO_POINTER((nChn<<16) | NVR_CHANNEL_STATUS_CHECK), NULL);
}

static gboolean nvr_channel_check_status_timeout(gpointer user_data)
{
	int nChn = GPOINTER_TO_INT(user_data);
	nvr_channel_check(nChn);
	if(nChn >= 0)
		return G_SOURCE_REMOVE;
	return G_SOURCE_CONTINUE;
}

static gboolean nvr_channel_set_param_timeout(gpointer user_data)
{
	NVR_CHANNEL_PARAM_S *pChnParam = (NVR_CHANNEL_PARAM_S *)user_data;
	g_assert(pChnParam != NULL);

	NVR_CHANNEL_INFO_S 		*pChInfo;
	int ret = -1;
	
	pChInfo = &gChInfo[pChnParam->nChn];
	g_mutex_lock(&pChInfo->mutex);
	if(pChInfo->lHandle == 0 || pChInfo->lIpncHandle == 0 || pChInfo->lLongonHandle == 0)
	{
		g_mutex_unlock(&pChInfo->mutex);
		g_free(pChnParam->data);
		g_slice_free(NVR_CHANNEL_PARAM_S, pChnParam);
		return G_SOURCE_REMOVE;
	}
	if((ret = ipnc_client_set_param(pChInfo->lHandle, 
								pChInfo->lLongonHandle, 
								pChnParam->cmd,
								pChnParam->data, 
								pChnParam->dataSize)) < 0)
	{
		j_warning("Set ipnc param failed.");
		ipnc_client_logout_ipnc(pChInfo->lHandle, pChInfo->lLongonHandle);
		pChInfo->lLongonHandle = 0;
	}
	g_mutex_unlock(&pChInfo->mutex);

	if(pChnParam->dataSize > 0)
		g_free(pChnParam->data);
	g_slice_free(NVR_CHANNEL_PARAM_S, pChnParam);

	return G_SOURCE_REMOVE;
}

int nvr_channel_get_param(int nChn, IPNC_CMD_TYPE_E cmd, void *data, int dataSize)
{
	g_return_val_if_fail(nChn < NVR_MAX_CHANNEL_NUM, -1);
	
	NVR_CHANNEL_INFO_S 		*pChInfo;
	int ret = -1;
	
	pChInfo = &gChInfo[nChn];
	g_mutex_lock(&pChInfo->mutex);
	if(pChInfo->lHandle == 0 || pChInfo->lIpncHandle == 0 || pChInfo->lLongonHandle == 0)
	{
		g_mutex_unlock(&pChInfo->mutex);
		return ret;
	}
	if((ret = ipnc_client_get_param(pChInfo->lHandle,
								pChInfo->lLongonHandle,
								cmd,
								data, 
								dataSize)) < 0)
	{
		j_warning("Get ipnc param failed.");
		ipnc_client_logout_ipnc(pChInfo->lHandle, pChInfo->lLongonHandle);
		pChInfo->lLongonHandle = 0;
	}
	g_mutex_unlock(&pChInfo->mutex);

	return ret;
}

int nvr_channel_set_param(int nChn, IPNC_CMD_TYPE_E cmd, void *data, int dataSize)
{
	g_return_val_if_fail(nChn < NVR_MAX_CHANNEL_NUM, -1);

	NVR_CHANNEL_PARAM_S *pChnParam;

	pChnParam = g_slice_new0(NVR_CHANNEL_PARAM_S);
	pChnParam->nChn = nChn;
	pChnParam->cmd = cmd;
	if(dataSize > 0)
	{
		pChnParam->data = g_malloc(dataSize);
		pChnParam->dataSize = dataSize;
		memcpy(pChnParam->data, data, dataSize);
	}
	g_timeout_add_seconds(0, nvr_channel_set_param_timeout, pChnParam);

	return 0;
}

static gboolean nvr_channel_stop_stream_timeout(gpointer user_data)
{
	int nChn = (GPOINTER_TO_INT(user_data) >> 16) & 0xffff;	
	int nStream = GPOINTER_TO_INT(user_data) & 0xffff;
	NVR_CHANNEL_INFO_S 		*pChInfo;
	
	pChInfo = &gChInfo[nChn];
	if(g_atomic_int_dec_and_test(&pChInfo->nOpenStreamCnt[nStream]))
		nvr_channel_check(nChn);
	return G_SOURCE_REMOVE;
}

int nvr_channel_start_stream(int nChn, int nStream)
{
	NVR_CHANNEL_INFO_S 		*pChInfo;
	if(nChn >= NVR_MAX_CHANNEL_NUM || nStream >= NVR_MAX_STREAM_NUM)
		return -1;
		
	pChInfo = &gChInfo[nChn];
	g_mutex_lock(&pChInfo->mutex);
	if(pChInfo->lHandle == 0 || pChInfo->lIpncHandle == 0 || pChInfo->lLongonHandle == 0)
	{
		g_mutex_unlock(&pChInfo->mutex);
		return -1;
	}
	if(pChInfo->lStreamHandle[nStream] != 0)
	{
		ipnc_client_request_iframe(pChInfo->lHandle, pChInfo->lIpncHandle, pChInfo->lStreamHandle[nStream]);
	}
	g_mutex_unlock(&pChInfo->mutex);
	
	if(g_atomic_int_add(&pChInfo->nOpenStreamCnt[nStream], 1) == 0)
	{
		nvr_channel_check(nChn);
	}
	return 0;
}

int nvr_channel_stop_stream(int nChn, int nStream)
{
	if(nChn >= NVR_MAX_CHANNEL_NUM || nStream >= NVR_MAX_STREAM_NUM)
		return -1;
		
	g_timeout_add_seconds(30, nvr_channel_stop_stream_timeout, 
					GINT_TO_POINTER((nChn<<16) | nStream));
	return 0;
}

int nvr_channel_request_iframe(int nChn, int nStream)
{
	NVR_CHANNEL_INFO_S 		*pChInfo;
	if(nChn >= NVR_MAX_CHANNEL_NUM || nStream >= NVR_MAX_STREAM_NUM)
		return -1;

	pChInfo = &gChInfo[nChn];
	g_mutex_lock(&pChInfo->mutex);
	if(pChInfo->lHandle != 0 && pChInfo->lIpncHandle != 0 && pChInfo->lStreamHandle[nStream] != 0)
	{
		ipnc_client_request_iframe(pChInfo->lHandle, pChInfo->lIpncHandle, pChInfo->lStreamHandle[nStream]);
	}
	g_mutex_unlock(&pChInfo->mutex);
	
	return 0;
}

int nvr_channel_ptz(int nChn, void *pPtz)
{
	g_return_val_if_fail(nChn < NVR_MAX_CHANNEL_NUM, -1);
	
	NVR_CHANNEL_INFO_S		*pChInfo;
	int 	ret = -1;
	
	pChInfo = &gChInfo[nChn];
	g_mutex_lock(&pChInfo->mutex);
	if(pChInfo->lHandle == 0     || 
	   pChInfo->lIpncHandle == 0 || 
	   pChInfo->lLongonHandle == 0)
	{
		g_mutex_unlock(&pChInfo->mutex);
		return ret;
	}
	if((ret = ipnc_client_send_ptz_cmd(pChInfo->lHandle, 
								   pChInfo->lLongonHandle, 
								   pPtz, 
								   sizeof(NVR_CTRL_PTZ_S))) < 0)
	{
		j_warning("Send ipnc ptz command failed");
		ipnc_client_logout_ipnc(pChInfo->lHandle, pChInfo->lLongonHandle);
		pChInfo->lLongonHandle = 0;
	}
	g_mutex_unlock(&pChInfo->mutex);

	return ret;
}

int nvr_channel_reboot_all_ipc()
{
	int chn;
	if(!nvr_get_dianxin_is_used())
	{
		NVR_CHANNEL_INFO_S *pChInfo;
		for(chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
		{
			pChInfo = &gChInfo[chn];
			g_mutex_lock(&pChInfo->mutex);
			if(pChInfo->lHandle != 0 && pChInfo->lLongonHandle != 0)
				ipnc_client_set_param(pChInfo->lHandle, pChInfo->lLongonHandle, IPNC_CMD_REBOOT, NULL, 0);
			g_mutex_unlock(&pChInfo->mutex);
			nvr_channel_set_param(chn, IPNC_CMD_REBOOT, NULL, 0);
		}
	}
	return 0;
}

int nvr_channel_halt_all_ipc()
{
	int chn;
	if(!nvr_get_dianxin_is_used())
	{
		NVR_CHANNEL_INFO_S *pChInfo;
		for(chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
		{
			pChInfo = &gChInfo[chn];
			g_mutex_lock(&pChInfo->mutex);
			if(pChInfo->lHandle != 0 && pChInfo->lLongonHandle != 0)
				ipnc_client_set_param(pChInfo->lHandle, pChInfo->lLongonHandle, IPNC_CMD_HALT, NULL, 0);
			g_mutex_unlock(&pChInfo->mutex);
			nvr_channel_set_param(chn, IPNC_CMD_HALT, NULL, 0);
		}
	}
	return 0;
}

int nvr_channel_default_all_ipc()
{
	int chn;
	if(!nvr_get_dianxin_is_used())
	{
		NVR_CHANNEL_INFO_S *pChInfo;
		for(chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
		{
			pChInfo = &gChInfo[chn];
			g_mutex_lock(&pChInfo->mutex);
			if(pChInfo->lHandle != 0 && pChInfo->lLongonHandle != 0)
				ipnc_client_set_param(pChInfo->lHandle, pChInfo->lLongonHandle, IPNC_CMD_DEFAULT, NULL, 0);
			g_mutex_unlock(&pChInfo->mutex);
			nvr_channel_set_param(chn, IPNC_CMD_DEFAULT, NULL, 0);
		}
	}
	return 0;
}

int nvr_channel_update_cfg(int port, NVR_CHANNEL_CFG_S *pChCfg)
{
	g_timeout_add_seconds(0, nvr_channel_check_status_timeout, GINT_TO_POINTER(port));
	return 0;
}

int nvr_channel_update_time(NVR_TIME_S *pTimeCfg)
{
	int i;

	for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{
		nvr_channel_set_param(i, IPNC_CMD_TIME_CFG, (IPNC_TIME_S*)pTimeCfg, sizeof(IPNC_TIME_S));
	}
	return 0;
}

static gpointer nvr_channel_av_msg_func(gpointer data)
{
	char msgbuff[256];
	long msgtype;
	while(av_pool_recv_msg(msgbuff, sizeof(msgbuff)) >= 0)
	{
		memcpy(&msgtype, msgbuff, sizeof(msgtype));
		switch(msgtype)
		{
			case AV_POOL_START_READ_FRAME:
			{
				AV_MSG_CHN_INFO_S *pMsg = (AV_MSG_CHN_INFO_S*)msgbuff;
				nvr_channel_start_stream(pMsg->chn, pMsg->stream);
				break;
			}
			case AV_POOL_STOP_READ_FRAME:
			{
				AV_MSG_CHN_INFO_S *pMsg = (AV_MSG_CHN_INFO_S*)msgbuff;
				nvr_channel_stop_stream(pMsg->chn, pMsg->stream);				
				break;
			}
			case AV_POOL_REQUEST_IFRAME:
			{
				AV_MSG_CHN_INFO_S *pMsg = (AV_MSG_CHN_INFO_S*)msgbuff;
				nvr_channel_request_iframe(pMsg->chn, pMsg->stream);	
				break;
			}
			default:
				break;
		}
	}
	return NULL;
}

int nvr_channel_init()
{
	int i;
	
	memset(gChInfo, 0, sizeof(gChInfo));
	for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{
		g_mutex_init(&gChInfo[i].mutex);
		gChInfo[i].nOpenStreamCnt[0] = 1;
	}
	gAvMsgThread = g_thread_new("avmsg", nvr_channel_av_msg_func, NULL);
	gChPool = g_thread_pool_new(nvr_channel_check_status_func, NULL, 1, FALSE, NULL);
	nvr_channel_check(-1);
	g_timeout_add_seconds(5, nvr_channel_check_status_timeout, GINT_TO_POINTER(-1));
	
	return 0;
}

void nvr_channel_uninit()
{
	int i;
	NVR_CHANNEL_INFO_S *pChnInfo;
	
	j_warning("channel uninit.");
	g_thread_pool_free(gChPool, FALSE, TRUE);
	for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{
		pChnInfo = &gChInfo[i];
		g_mutex_lock(&pChnInfo->mutex);
		nvr_channel_close_all_handle(pChnInfo);
		g_mutex_unlock(&gChInfo[i].mutex);
		
		g_mutex_clear(&pChnInfo->mutex);
	}
	
	j_warning("comm uninit finish.");
}


