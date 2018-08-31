#ifndef __NVR_PARAM_SYS_H__
#define __NVR_PARAM_SYS_H__

#include <nvr_server_headers.h>



typedef struct _NVR_PARAM_SYS_INFO_S_
{
	guint32					u32Size;
	guint32					u32Version;
	NVR_DEV_INFO_S			struDevCfg;
	NVR_DEV_NET_CFG_S		struNetCfg;	
	NVR_FTP_CFG_S			struFtpCfg;
	NVR_SMTP_CFG_S			struSmtpCfg;
	NVR_DDNS_CFG_S			struDdnsCfg;
	NVR_UPNP_CFG_S			struUpnpCfg;
	NVR_NTP_CFG_S			struNtpCfg;
	NVR_DEV_USER_CFG_S		struUsrCfg[NVR_MAX_USR_NUM];
	NVR_TIME_CFG_S			struTimeCfg;
	NVR_AUTO_MAINTAIN_CFG_S	struAutoCfg;
	NVR_CHANNEL_CFG_S		struChannelCfg[NVR_MAX_CHANNEL_NUM];
	NVR_DEV_COMM_CFG_S		struCommCfg[NVR_MAX_SUPPORT_COMM_NO];
	NVR_DEV_VIDEO_CFG_S		struVideoCfg[NVR_MAX_CHANNEL_NUM];
	NVR_DEV_AUDIO_CFG_S		struAudioCfg[NVR_MAX_CHANNEL_NUM];
	NVR_DEV_JXJ_PLATFORM_CFG_S struJxjPlatformCfg;

	NVR_DEV_REC_STRC_S		struRecCfg[NVR_MAX_CHANNEL_NUM];
	NVR_DEV_OSD_LABEL_CFG_S struOsdLabelCfg;
	NVR_DEV_PTZ_CFG_S		struPtzCfg[NVR_MAX_CHANNEL_NUM];

	//NVR_DEV_MD_CFG_S			struMdCfg[NVR_MAX_CHANNEL_NUM];
	NVR_DEV_DI_CFG_S			struDiCfg[NVR_MAX_SUPPORT_DI];
	NVR_DEV_IPNC_ALARM_CFG_S	struIpncAlarmCfg[NVR_MAX_CHANNEL_NUM];

	NVR_DEV_DIANXIN_PLATFORM_CFG_S stDianxinPlatformCfg;
	NVR_DEV_AUTO_CTRL_CFG_S	struAutoCtrlCfg;
	
	guint32					u8Res[2048];
	
}NVR_PARAM_SYS_INFO_S, *LPNVR_PARAM_SYS_INFO_S;

//===============================================
typedef struct _NVR_PARAM_MAN_S_
{
	gint					sysParamChange;
	
	GMutex		 		paramLock;
}NVR_PARAM_MAN_S, *LPNVR_PARAM_MAN_S;

extern NVR_PARAM_MAN_S  		gParamMan;
extern NVR_PARAM_SYS_INFO_S		gSysInfo;
extern NVR_DEV_STABLE_INFO_S		gStableInfo;
extern gint					gIsDefaultFileSystem;

#define NVR_DI_NUM	(gStableInfo.u8AlarmInputNum)
#define NVR_DO_NUM	(gStableInfo.u8AlarmOutputNum)
#define NVR_CHN_NUM	(gStableInfo.u8ChnNum)
#define NVR_ETH_NUM (gStableInfo.u8NetNum)
#define NVR_COMM_NUM (gStableInfo.u8RS485Num)

static inline void nvr_param_changed()
{
	g_atomic_int_set(&gParamMan.sysParamChange, 1);
}

static inline void nvr_param_no_change()
{
	g_atomic_int_set(&gParamMan.sysParamChange, 0);
}

static inline void nvr_param_lock()
{
	g_mutex_lock(&gParamMan.paramLock);
}
static inline void nvr_param_unlock()
{
	g_mutex_unlock(&gParamMan.paramLock);
}
#define PC	(nvr_param_changed())
#define PNC	(nvr_param_no_change())
#define PL	(nvr_param_lock())
#define PNL	(nvr_param_unlock())


int nvr_param_set_sys_change_flag(int bFlag);

int nvr_param_get_sys_change_flag();

int nvr_param_export(const char *pExportPath);

int nvr_param_import(const char *pImportPath);

gboolean	nvr_param_sw_reset();

gboolean	nvr_param_hw_reset();

gboolean nvr_param_write_sys_param_info();

int nvr_param_init();

void nvr_param_uninit();

#endif

