
#include "nvr_alarm.h"
#include "gpio.h"
#include "nvr_smtp.h"
#include "nvr_param_proc.h"
#include "nvr_param_sys.h"
#include "nvr_net_common.h"
#include "nvr_rec.h"
#include "av_pool.h"
#include "ipnc_lib.h"


static const int g_diGpioNum[NVR_MAX_SUPPORT_DI] = {185, 186, 187, 189};
static const int g_doGpioNum[NVR_MAX_SUPPORT_DO] = {70};
static long g_diGpio[NVR_MAX_SUPPORT_DI];
static long g_doGpio[NVR_MAX_SUPPORT_DO];

// day: 1 :星期天，2 :星期一... 7: 星期六 0:每天
static int nvr_alarm_time_in_seg(NVR_SEGMENT_S (*pTimerSeg)[8])
{
	int seg, day;
	time_t cur_t = time(NULL);
	time_t t = cur_t % (24 * 60 * 60);
	struct tm *ptm, stm;
	NVR_SEGMENT_S *pTimerSegTmp;
	
	for(seg = 0; seg < NVR_MAX_SEG_NUM; seg++)
	{
		pTimerSegTmp = &pTimerSeg[seg][0];
		//j_debug("t: %ld, open: %u, begin:%u, end:%d", t, pTimerSegTmp->u8Open, pTimerSegTmp->beginSec, pTimerSegTmp->endSec);
		if(pTimerSegTmp->u8Open)
		{
			if(t >= pTimerSegTmp->beginSec && t <= pTimerSegTmp->endSec)
				return 1;
		}
	}

	ptm = localtime_r(&cur_t, &stm);
	day = ptm->tm_wday + 1;
	for(seg = 0; seg < NVR_MAX_SEG_NUM; seg++)
	{
		pTimerSegTmp = &pTimerSeg[seg][day];
		if(pTimerSegTmp->u8Open)
		{
			if(t >= pTimerSegTmp->beginSec && t <= pTimerSegTmp->endSec)
				return 1;
		}
	}
	return 0;
}

//报警动作
static int nvr_alarm_action(NVR_LINKACT_S* pActionCfg)
{
	if(NULL == pActionCfg)
		return -1;

	//j_debug("pActionCfg->u8Email: %d", pActionCfg->u8Email);
	//报警联动发送Email
	if(pActionCfg->u8Email)
	{
		nvr_stmp_send_mail("nvr alarm", "alarm");
	}
	
	//报警输出
	if(pActionCfg->u32OutPut)
	{
		///////////////////////////////////////////////
	}

	return 0;
}

static void nvr_alarm_ipnc(int chn, int happen)
{
	if(happen)
	{
		NVR_DEV_IPNC_ALARM_CFG_S stIpncAlarmCfg;
		nvr_rec_alarm(chn, 1);
		// nvr联动消息
		nvr_get_ipnc_alarm_cfg(chn, &stIpncAlarmCfg);
		nvr_alarm_action(&stIpncAlarmCfg.struLinkAct);
	}
	else
	{
		nvr_rec_alarm(chn, 0);
	}
}

//ipc报警检测
static gboolean nvr_alarm_check_timeout(gpointer user_data)
{
	int chn;
	int alarm;
	static int ipncAlarmStatus[NVR_MAX_CHANNEL_NUM] = {0};
	static int ipncAlarmTime[NVR_MAX_CHANNEL_NUM];

	//j_debug("nvr_alarm_check_timeout...");
	for(chn = 0; chn < NVR_CHN_NUM; chn++)
	{
		av_pool_get_alarm_status(chn, &alarm);
		//j_debug("nvr_alarm_check_timeout: %d", alarm);
		//j_debug("ipncAlarmStatus[%d]: %d", chn, ipncAlarmStatus[chn]);
		if(alarm && ipncAlarmStatus[chn] != 1)
		{
			if(ipncAlarmStatus[chn] == 0)
				nvr_alarm_ipnc(chn, 1);
			ipncAlarmStatus[chn] = 1;
		}
		else if(alarm == 0 && ipncAlarmStatus[chn])
		{
			if(ipncAlarmStatus[chn] == 1)
			{
				ipncAlarmStatus[chn] = 2;
				ipncAlarmTime[chn] = 10;
			}
			else if(ipncAlarmStatus[chn] == 2)
			{
				if(--ipncAlarmTime[chn] == 0)
				{
					nvr_alarm_ipnc(chn, 0);
					ipncAlarmStatus[chn] = 0;
				}
			}
		}
	}

	return G_SOURCE_CONTINUE;
}

int nvr_alarm_init(void)
{
	int i = 0;
	
	for(i = 0; i < NVR_DI_NUM; i++)
		g_diGpio[i] = gpio_open(g_diGpioNum[i]);
	for(i = 0; i < NVR_DO_NUM; i++)
		g_doGpio[i] = gpio_open(g_doGpioNum[i]);
	g_timeout_add_seconds(1, nvr_alarm_check_timeout, NULL);
	
	return 0;
}

void nvr_alarm_uninit(void)
{
	int i;
	
	j_warning("alarm uninit.");
	for(i = 0; i < NVR_DI_NUM; i++)
		gpio_close(g_diGpio[i]);
	for(i = 0; i < NVR_DO_NUM; i++)
		gpio_close(g_doGpio[i]);
	j_warning("alarm uninit finish");
}



