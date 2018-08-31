
#include <nvr_server_headers.h>
#include <ipnc_lib_client.h>
#include <gpio.h>

#include "nvr_channel.h"
#include "nvr_param_sys.h"
#include "nvr_param_proc.h"
#include "nvr_ntp.h"
#include "nvr_upnp.h"
#include "nvr_misc.h"

#define HWRESET_GPIO	26

static long gHwresetGpio;

static gboolean nvr_auto_maintain_cb(gpointer user_data)
{
	NVR_TIME_S	timeCfg;
	NVR_AUTO_MAINTAIN_CFG_S autoMtCfg;
	
	// 自动维护,判断是否需要重启之类
	nvr_get_time_cfg(0, &timeCfg);
	nvr_get_auto_cfg(0, &autoMtCfg);
	if(autoMtCfg.u8Enable)
	{
		switch(autoMtCfg.u8WeekDay)
		{
			case 0://每天
			{
				if((autoMtCfg.u8Hour == timeCfg.u8Hour) &&
				   (autoMtCfg.u8Min  == timeCfg.u8Minute))
				   	nvr_do_system_reboot();
				break;
			}
			case 1://周日
			case 2://周一
			case 3://周二
			case 4://周三
			case 5://周四
			case 6://周五
			case 7://周六
			{
				if((autoMtCfg.u8WeekDay - 1) ==
					timeCfg.u8WDay)
				{
					if((autoMtCfg.u8Hour == timeCfg.u8Hour) &&
				   	   (autoMtCfg.u8Min  == timeCfg.u8Minute))
				   		nvr_do_system_reboot();
				}
				break;
			}
			default:
				break;
		}
	}
	// 因为测试发现rtc的时间不准，
	// 所以将系统时间同步写入到rtc中
	system("hwclock -w");
	
	return G_SOURCE_CONTINUE;
}

static gboolean nvr_check_system_reboot_cb(gpointer user_data)
{
	static int nNeedReboot = 0;
	static int nNeedShutdown = 0;
	static int nNeedDefault = 0;
	static int nRebootCnt = 0;

	if(nNeedReboot || nNeedShutdown || nNeedDefault)
	{
		if(nRebootCnt++ == 0)
		{
			if(nNeedReboot)
				nvr_do_system_reboot();
			else if(nNeedShutdown)
				nvr_do_system_shutdown();
			else if(nNeedDefault)
			{
				if(nNeedDefault == 1)
					nvr_do_system_reset(1);
				else  /*if(nNeedDefault == 2)*/
					nvr_do_system_reset(0);
			}
		}
		else if(nRebootCnt > 10)
		{
			// 系统不能正常重启， 使用wdt强制重启
			system("/opt/nvr/wdt restart");
		}
	}
	else
	{
		nNeedReboot = nvr_get_system_reboot_flag();
		nNeedShutdown = nvr_get_system_shutdown_flag();
		nNeedDefault = nvr_get_system_default_flag();
	}
	return G_SOURCE_CONTINUE;
}

static gboolean nvr_check_hw_reset_cb(gpointer user_data)
{
	int value;
	static int resetCnt = 0;
	if(!nvr_get_hwgpio_check_flag())
		return G_SOURCE_CONTINUE;
	if(gHwresetGpio != 0)
	{
		if(gpio_get_value(gHwresetGpio, &value) >= 0)
		{
			if(value == 0)
				resetCnt++;
			else
				resetCnt = 0;
			if(resetCnt == 2)
				nvr_set_system_default_flag(2);
		}
	}
	return G_SOURCE_CONTINUE;
}

#define LOG_MAX_SIZE	0x200000
#define LOG_FILE1	"/var/log/messages"
#define LOG_FILE2	"/mnt/log/messages"
#define LOG_FILE3	"/mnt/log/messages_1"
const char *gLogFile[] = {LOG_FILE1, LOG_FILE2, LOG_FILE3, NULL};
static gboolean nvr_check_log_size_cb(gpointer user_data)
{
	const char **pLogFile;
	struct stat st;
	for(pLogFile = gLogFile; *pLogFile != NULL; pLogFile++)
	{
		if(stat(*pLogFile, &st) < 0)
			continue;
		if(st.st_size > LOG_MAX_SIZE)
		{
			j_message("Delete log file %s.", *pLogFile);
			unlink(*pLogFile);
		}
	}
	return G_SOURCE_CONTINUE;
}

int nvr_timer_init()
{
	gHwresetGpio = gpio_open(HWRESET_GPIO);
	if(gHwresetGpio == 0)
		j_warning("Open gpio %d for hwreset failed", HWRESET_GPIO);
	else
		j_message("Open gpio %d for hwreset successfully", HWRESET_GPIO);
	g_timeout_add_seconds(60, nvr_auto_maintain_cb, NULL);
	g_timeout_add_seconds(3, nvr_check_system_reboot_cb, NULL);
	g_timeout_add_seconds(2, nvr_check_hw_reset_cb, NULL);
	g_timeout_add_seconds(5, nvr_check_log_size_cb, NULL);
	return 0;
}


void nvr_timer_uninit()
{
	j_warning("timer uninit.");
	if(gHwresetGpio != 0)
		gpio_close(gHwresetGpio);
		
	j_warning("timer uninit finish");
}

NVR_TIME_S *nvr_timer_tm_2_nvr(struct tm *pTmTime, NVR_TIME_S *pStruTime)
{
	if (pTmTime != NULL && pStruTime != NULL)
	{
		pStruTime->u8Year  = pTmTime->tm_year;   //since 1900
		pStruTime->u8Month= (pTmTime->tm_mon + 1) % 13;               //0    ~ 11,
		pStruTime->u8Day   = (pTmTime->tm_mday) % 32;
		pStruTime->u8WDay=  (pTmTime->tm_wday) % 7; 
		pStruTime->u8Hour  = (pTmTime->tm_hour) % 24;
		pStruTime->u8Minute=(pTmTime->tm_min) % 60;
		pStruTime->u8Second=(pTmTime->tm_sec)  % 60;
	}

	return pStruTime;
}

struct tm *nvr_timer_nvr_2_tm(NVR_TIME_S *pStruTime, struct tm *pTmTime)
{
	if (pTmTime != NULL && pStruTime != NULL)
	{
		pTmTime->tm_year = pStruTime->u8Year;
		pTmTime->tm_mon = (pStruTime->u8Month - 1) % 12;
		pTmTime->tm_mday= pStruTime->u8Day % 32;
		pTmTime->tm_hour = pStruTime->u8Hour % 24;			
		pTmTime->tm_min  = pStruTime->u8Minute % 60;		
		pTmTime->tm_sec  = pStruTime->u8Second % 60;
	}	

	return pTmTime;
}

int	nvr_sys_timer_get(int nPort, void *pData)
{
	
	time_t 	tTime  = 0;
	struct tm *pTmTime;


	NVR_TIME_S *pStruTime = (NVR_TIME_S *)pData;
	
	time(&tTime);
	pTmTime = localtime(&tTime);

	nvr_timer_tm_2_nvr(pTmTime, pStruTime);
	
	return 0;
}

int	nvr_sys_timer_set(int nPort, void *pData)
{
	struct tm *pCurTm = NULL;
	struct tm tmTime;
	
	struct timeval 	tvTime;
	struct timezone tzTime;
	
	time_t 	tTime  = 0;

	NVR_TIME_S *pStruTime = (NVR_TIME_S *)pData;
	
	memset(&tvTime, 0, sizeof(struct timeval));
	memset(&tzTime, 0, sizeof(struct timezone));
	
	time(&tTime);
	if((pCurTm = localtime_r(&tTime, &tmTime)) != NULL)
	{
		nvr_timer_nvr_2_tm(pStruTime, pCurTm); //systime -> tm
		if ((tTime = mktime(pCurTm)) != -1)
		{
			gettimeofday(&tvTime, &tzTime);
			
			tvTime.tv_sec  = tTime;
			tvTime.tv_usec = 0;
			settimeofday(&tvTime, &tzTime);//set tm
			system("hwclock -w");
		}	
	}

	return 0;
}

