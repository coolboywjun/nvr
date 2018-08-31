
#include <nvr_server_headers.h>
#include <nvr_param_proc.h>
#include <nvr_channel.h>

#include <ipnc_lib_client.h>
#include <storage_lib.h>

typedef struct _NVR_REC_INFO_S_
{
	int bStartRec;
}NVR_REC_INFO_S;

static int gStartRec[NVR_MAX_CHANNEL_NUM] = {0};
static StorageRecordType gRecType[NVR_MAX_CHANNEL_NUM] = {0};
static int gOverwrite[NVR_MAX_CHANNEL_NUM] = {-1, -1, -1, -1};
//static int gAlarmSet[NVR_MAX_CHANNEL_NUM] = {0};
static gint gAlarmHappen[NVR_MAX_CHANNEL_NUM] = {0};
static gint gAlarmSec[NVR_MAX_CHANNEL_NUM] = {0};
static gint gManualRec[NVR_MAX_CHANNEL_NUM] = {0};
G_LOCK_DEFINE_STATIC(mutex);

// day: 1 :星期天，2 :星期一... 7: 星期六
static int nvr_rec_is_in_segment(NVR_DEV_REC_STRC_S *pRecCfg)
{
	int seg, day;
	time_t cur_t = time(NULL);
	time_t t = cur_t % (24 * 60 * 60);
	struct tm *ptm, stm;
	NVR_SEGMENT_S *pTimerSeg;
	
	for(seg = 0; seg < 4; seg++)
	{
		pTimerSeg = &pRecCfg->struTimerSeg[seg][0];
		//j_debug("t: %ld, open: %u, begin:%u, end:%d", t, pTimerSeg->u8Open, pTimerSeg->beginSec, pTimerSeg->endSec);
		if(pTimerSeg->u8Open)
		{
			if(t >= pTimerSeg->beginSec && t <= pTimerSeg->endSec)
				return 1;
		}
	}

	ptm = localtime_r(&cur_t, &stm);
	day = (ptm->tm_wday + 1) % 8;
	for(seg = 0; seg < 4; seg++)
	{
		pTimerSeg = &pRecCfg->struTimerSeg[seg][day];
		if(pTimerSeg->u8Open)
		{
			if(t >= pTimerSeg->beginSec && t <= pTimerSeg->endSec)
				return 1;
		}
	}
	return 0;
}

static int nvr_rec_ctrl(int chn, NVR_DEV_REC_STRC_S *pRecCfg)
{
	g_assert(pRecCfg != NULL);
	g_return_val_if_fail(chn >= 0 && chn < NVR_MAX_CHANNEL_NUM, -1);
	static int bNeedSetMinutes = 1;
	
	// 电信看店平台要求每个录像段最大不超过15分钟
	if(bNeedSetMinutes)
	{
		if(nvr_get_dianxin_is_used())
		{
			if(storage_lib_record_set_minutes(15) >= 0)
				bNeedSetMinutes = 0;
		}
		else
			bNeedSetMinutes = 0;
	}
	
	if(gOverwrite[chn] != pRecCfg->u8OverWrite)
	{
		if(storage_lib_record_set_overwrite(chn, pRecCfg->u8OverWrite) >= 0)
			gOverwrite[chn] = pRecCfg->u8OverWrite;
	}
	
/*	
	// 如果是报警录像，直接设置相应通道ipnc的移动报警
	if(gAlarmSet[chn] == 0 && (pRecCfg->u8RecType == 2 || pRecCfg->u8RecType == 3))
	{
		IPNC_DEV_MD_CFG_S stMdCfg;

		if(nvr_channel_get_param(chn, IPNC_CMD_VMD_CFG, &stMdCfg, sizeof(stMdCfg)) >= 0)
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
			if(nvr_channel_set_param(chn, IPNC_CMD_VMD_CFG, &stMdCfg, sizeof(stMdCfg)) >= 0)
				gAlarmSet[chn] = 1;
		}
	}
	else if(gAlarmSet[chn] && pRecCfg->u8RecType != 2 && pRecCfg->u8RecType != 3)
	{
		IPNC_DEV_MD_CFG_S stMdCfg;
		if(nvr_channel_get_param(chn, IPNC_CMD_VMD_CFG, &stMdCfg, sizeof(stMdCfg)) >= 0)
		{
			stMdCfg.u8Enable = 0;
			if(nvr_channel_get_param(chn, IPNC_CMD_VMD_CFG, &stMdCfg, sizeof(stMdCfg)) >= 0)
				gAlarmSet[chn] = 0;
		}
	}
*/
	if(g_atomic_int_get(&gManualRec[chn]))	
	{
		// 开启了手动录像

		// 如果已经开启录像，但录像类型不是手动录像
		// 先关闭录像后再开启手动录像
		if(gStartRec[chn] && !(gRecType[chn] & RECORD_MANUAL_TYPE))
		{
			storage_lib_record_stop(chn);
			gStartRec[chn] = 0;
			gRecType[chn] = 0;
		}
		
		if(!gStartRec[chn])
		{
			if(storage_lib_record_start(chn, pRecCfg->u8RecStream, RECORD_MANUAL_TYPE) == 0)
			{
				gStartRec[chn] = 1;
				gRecType[chn] = RECORD_MANUAL_TYPE;
			}
			else
				j_warning("Start record failed");
		}
		// 直接返回，不再处理下面的逻辑
		return 0;
	}
	else
	{
		if(gStartRec[chn] && (gRecType[chn] & RECORD_MANUAL_TYPE))
		{
			storage_lib_record_stop(chn);
			gStartRec[chn] = 0;
			gRecType[chn] = 0;
		}
	}
	
	if(pRecCfg->u8Enable)
	{
		int alarm_happen = g_atomic_int_get(&gAlarmHappen[chn]);
		if(pRecCfg->u8RecType == 1)		// 定时录像
		{
			int bIn = nvr_rec_is_in_segment(pRecCfg);
			if(bIn && !gStartRec[chn])
			{
				if(storage_lib_record_start(chn, pRecCfg->u8RecStream, RECORD_TIME_TYPE) == 0)
				{
					gStartRec[chn] = 1;
					gRecType[chn] = RECORD_TIME_TYPE;
				}
				else
					j_warning("Start record failed");
			}
			else if(!bIn && gStartRec[chn])
			{
				storage_lib_record_stop(chn);
				gStartRec[chn] = 0;
				gRecType[chn] = 0;
			}
		}
		else if(pRecCfg->u8RecType == 2)		// 报警录像
		{
			if(alarm_happen && !gStartRec[chn])
			{
				gAlarmSec[chn] = 0;
				if(storage_lib_record_start(chn, pRecCfg->u8RecStream, RECORD_ALARM_TYPE) == 0)
				{
					gStartRec[chn] = 1;
					gRecType[chn] = RECORD_ALARM_TYPE;
				}
				else
					j_warning("Start record failed");
			}
			else if(!alarm_happen && gStartRec[chn])
			{
				if(!(gRecType[chn] & RECORD_ALARM_TYPE) || ++gAlarmSec[chn] >= 60)
				{
					storage_lib_record_stop(chn);
					gStartRec[chn] = 0;
					gRecType[chn] = 0;
				}
			}
			else if(alarm_happen)
			{
				gAlarmSec[chn] = 0;
			}
		}
		else if(pRecCfg->u8RecType == 3)
		{
			int bIn = nvr_rec_is_in_segment(pRecCfg);
			if(!gStartRec[chn] && (bIn || alarm_happen))
			{
				StorageRecordType type = 0;
				if(bIn)
					type |= RECORD_TIME_TYPE;
				if(alarm_happen)
				{
					type |= RECORD_ALARM_TYPE;
					gAlarmSec[chn] = 0;
				}
				if(storage_lib_record_start(chn, pRecCfg->u8RecStream, type) == 0)
				{
					gStartRec[chn] = 1;
					gRecType[chn] = type;
				}
				else
					j_warning("Start record failed");
			}
			else if(gStartRec[chn] && !bIn && !alarm_happen)
			{
				if(!(gRecType[chn] & RECORD_ALARM_TYPE)
					|| ((gRecType[chn] & RECORD_ALARM_TYPE) && ++gAlarmSec[chn] >= 60))
				{
					storage_lib_record_stop(chn);
					gStartRec[chn] = 0;
					gRecType[chn] = 0;
				}
			}
		}
		else /*if(pRecCfg->u8RecType == 0) */ // 全天录像
		{
			if(!gStartRec[chn])
			{
				if(storage_lib_record_start(chn, pRecCfg->u8RecStream, RECORD_STARTUP_TYPE) == 0)
				{
					gStartRec[chn] = 1;
					gRecType[chn] = RECORD_STARTUP_TYPE;
				}
				else
					j_warning("Start record failed");
			}
		}

		if(gStartRec[chn] && alarm_happen && (gRecType[chn] & RECORD_ALARM_TYPE) == 0)
		{
			//if(storage_lib_record_set_type(chn, RECORD_ALARM_TYPE) >= 0)
			//	gRecType[chn] |= RECORD_ALARM_TYPE;
		}
	}
	else
	{
		if(gStartRec[chn])
		{
			storage_lib_record_stop(chn);
			gStartRec[chn] = 0;
			gRecType[chn] = 0;
		}
	}
	
	return 0;
}

static gboolean __nvr_rec_timeout(gpointer user_data)
{
	int i;
	int chn = GPOINTER_TO_INT(user_data);
	NVR_DEV_REC_STRC_S stRecCfg;
	
	G_LOCK(mutex);
	for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{
		if(chn >= 0 && i != chn)
			continue;
		nvr_get_rec_cfg(i, &stRecCfg);
		nvr_rec_ctrl(i, &stRecCfg);
	}
	G_UNLOCK(mutex);
	if(chn >= 0)
		return G_SOURCE_REMOVE;
	return G_SOURCE_CONTINUE;
}

int nvr_rec_alarm(int chn, int happen)
{
	g_return_val_if_fail(chn >= 0 && chn < NVR_MAX_CHANNEL_NUM, -1);

	g_atomic_int_set(&gAlarmHappen[chn], happen);
	g_timeout_add_seconds(0, __nvr_rec_timeout, GINT_TO_POINTER(chn));
	return 0;
}

int nvr_rec_manual(int chn, int start)
{
	g_return_val_if_fail(chn >= 0 && chn < NVR_MAX_CHANNEL_NUM, -1);

	g_atomic_int_set(&gManualRec[chn], start);
	g_timeout_add_seconds(0, __nvr_rec_timeout, GINT_TO_POINTER(chn));
	return 0;
}

int nvr_rec_update(int chn)
{
	g_return_val_if_fail(chn >= 0 && chn < NVR_MAX_CHANNEL_NUM, -1);

	g_timeout_add_seconds(0, __nvr_rec_timeout, GINT_TO_POINTER(chn));
	return 0;
}

int nvr_rec_init()
{
	if(storage_lib_init() < 0)
	{
		j_warning("Storage lib init failed.");
		return -1;
	}
	g_timeout_add_seconds(10, __nvr_rec_timeout, GINT_TO_POINTER(-1));

	return 0;
}

void nvr_rec_uninit()
{
	int chn;
	
	j_warning("rec uninit.");
	G_LOCK(mutex);
	for(chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
	{
		if(gStartRec[chn])
			storage_lib_record_stop(chn);
	}
	G_UNLOCK(mutex);
	storage_lib_uninit();
	j_warning("rec uninit finish.");
}

