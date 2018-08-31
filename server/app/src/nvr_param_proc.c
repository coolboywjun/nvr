#include "nvr_param_proc.h"
#include "nvr_param_sys.h"
#include "nvr_param_check.h"
#include "nvr_net_common.h"
#include "nvr_channel.h"
#include "nvr_ntp.h"
#include "nvr_upnp.h"
#include "nvr_net_man.h"
#include "nvr_misc.h"
#include "nvr_timer.h"
#include "nvr_alarm.h"
#include "nvr_rec.h"
#include "nvr_net_search.h"
#include "nvr_smart_search.h"

static inline gboolean __nvr_compare_cfg(void *cfg1, void *cfg2, gint cfgLen)
{
	if(memcmp(cfg1, cfg2, cfgLen) != 0)
	{
		PC;
		return TRUE;
	}
	return FALSE;
}

static inline gboolean __nvr_set_cfg(void *setCfg, void *sysCfg, gint cfgLen)
{
	PL;
	if(__nvr_compare_cfg(setCfg, sysCfg, cfgLen))
	{
		memcpy(sysCfg, setCfg, cfgLen);
		PNL;
		return TRUE;
	}
	PNL;
	return FALSE;
}

static inline gboolean __nvr_get_cfg(void *getCfg, void *sysCfg, gint cfgLen)
{
	PL;
	memcpy(getCfg, sysCfg, cfgLen);
	PNL;
	
	return TRUE;
}

#define DECLARE_GET_CFG(CfgName, StructType, StructName)	\
gboolean nvr_get_##CfgName##_cfg(gint port, StructType *cfg)	\
{	\
	return __nvr_get_cfg(cfg, &gSysInfo.StructName, sizeof(StructType));	\
}

#define DECLARE_SET_CFG(CfgName, StructType, StructName)	\
gboolean nvr_set_##CfgName##_cfg(gint port, StructType *cfg)	\
{	\
	nvr_check_##CfgName##_cfg(cfg);	\
	return __nvr_set_cfg(cfg, &gSysInfo.StructName, sizeof(StructType));	\
}

#define DECLARE_GET_ARRAY_CFG(CfgName, StructType, StructName)	\
gboolean nvr_get_##CfgName##_cfg(gint port, StructType *cfg)	\
{	\
	return __nvr_get_cfg(cfg, &gSysInfo.StructName[port], sizeof(StructType));	\
}

#define DECLARE_SET_ARRAY_CFG(CfgName, StructType, StructName)	\
gboolean nvr_set_##CfgName##_cfg(gint port, StructType *cfg)	\
{	\
	nvr_check_##CfgName##_cfg(cfg);	\
	return __nvr_set_cfg(cfg, &gSysInfo.StructName[port], sizeof(StructType));	\
}

#define DECLARE_GET_ARRAY_STUM_CFG(CfgName, StructType, StructName, StructMem)	\
gboolean nvr_get_##CfgName##_cfg(gint port, StructType *cfg)	\
{	\
	return __nvr_get_cfg(cfg, &gSysInfo.StructName[port].StructMem, sizeof(StructType));	\
}

#define DECLARE_SET_ARRAY_STUM_CFG(CfgName, StructType, StructName, StructMem)	\
gboolean nvr_set_##CfgName##_cfg(gint port, StructType *cfg)	\
{	\
	nvr_check_##CfgName##_cfg(cfg);	\
	return __nvr_set_cfg(cfg, &gSysInfo.StructName[port].StructMem, sizeof(StructType));	\
}

DECLARE_GET_CFG(dev, NVR_DEV_INFO_S, struDevCfg);
DECLARE_SET_CFG(dev, NVR_DEV_INFO_S, struDevCfg);

//DECLARE_GET_CFG(net, NVR_DEV_NET_CFG_S, struNetCfg);
gboolean nvr_get_net_cfg(gint port, NVR_DEV_NET_CFG_S *pNetCfg)
{
	PL;
	nvr_net_get_net_info(&gSysInfo.struNetCfg);
	memcpy(pNetCfg, &gSysInfo.struNetCfg, sizeof(NVR_DEV_NET_CFG_S));
	PNL;
	return TRUE;
}
//DECLARE_SET_CFG(net, NVR_DEV_NET_CFG_S, struNetCfg);
gboolean nvr_set_net_cfg(gint port, NVR_DEV_NET_CFG_S *pNetCfg)
{
	NVR_DEV_NET_CFG_S *pDevNetCfg = (NVR_DEV_NET_CFG_S *)pNetCfg;
	//int nNeedRb = 0, nUpdateDns = 0;
	
	PL;

	pDevNetCfg->struEtherCfg[0].u8DhcpStatus = gSysInfo.struNetCfg.struEtherCfg[0].u8DhcpStatus;
	pDevNetCfg->struEtherCfg[0].u8EtherStatus  = gSysInfo.struNetCfg.struEtherCfg[0].u8EtherStatus;
	pDevNetCfg->struEtherCfg[1].u8DhcpStatus = gSysInfo.struNetCfg.struEtherCfg[1].u8DhcpStatus;
	pDevNetCfg->struEtherCfg[1].u8EtherStatus  = gSysInfo.struNetCfg.struEtherCfg[1].u8EtherStatus;
	pDevNetCfg->struPppoeCfg.u8PppoeStatus = gSysInfo.struNetCfg.struPppoeCfg.u8PppoeStatus;
	pDevNetCfg->struPppoeCfg.u32PppoeIp = gSysInfo.struNetCfg.struPppoeCfg.u32PppoeIp;
	pDevNetCfg->struPppoeCfg.u32PppoeTimes = gSysInfo.struNetCfg.struPppoeCfg.u32PppoeTimes;

	if(__nvr_compare_cfg(pDevNetCfg, &gSysInfo.struNetCfg, sizeof(NVR_DEV_NET_CFG_S)))
	{	
		if((pDevNetCfg->struEtherCfg[0].u32IpAddr  & pDevNetCfg->struEtherCfg[0].u32SubMask) !=
		   (pDevNetCfg->struEtherCfg[0].u32GateWay & pDevNetCfg->struEtherCfg[0].u32SubMask)
		   && !pDevNetCfg->struEtherCfg[0].u8DhcpOn)
			j_debug("ipadd and gateway conflict");
		memcpy(&gSysInfo.struNetCfg, pDevNetCfg,  sizeof(NVR_DEV_NET_CFG_S));
		PNL;
		nvr_net_update_cfg();
	}	
	else
	{
		PNL;
	}

	return NVR_SUCCESS;
}

gboolean nvr_update_net_cfg(gint port, NVR_DEV_NET_CFG_S *pNetCfg)
{
	PL;
	if(__nvr_compare_cfg(pNetCfg, &gSysInfo.struNetCfg, sizeof(NVR_DEV_NET_CFG_S)))
		memcpy(&gSysInfo.struNetCfg, pNetCfg,  sizeof(NVR_DEV_NET_CFG_S));
	PNL;
	
	return NVR_SUCCESS;
}

DECLARE_GET_CFG(ftp, NVR_FTP_CFG_S, struFtpCfg);
DECLARE_SET_CFG(ftp, NVR_FTP_CFG_S, struFtpCfg);
DECLARE_GET_CFG(smtp, NVR_SMTP_CFG_S, struSmtpCfg);
DECLARE_SET_CFG(smtp, NVR_SMTP_CFG_S, struSmtpCfg);
DECLARE_GET_CFG(ddns, NVR_DDNS_CFG_S, struDdnsCfg);
DECLARE_SET_CFG(ddns, NVR_DDNS_CFG_S, struDdnsCfg);
DECLARE_GET_CFG(upnp, NVR_UPNP_CFG_S, struUpnpCfg);
//DECLARE_SET_CFG(upnp, NVR_UPNP_CFG_S, struUpnpCfg);
gboolean nvr_set_upnp_cfg(gint port, NVR_UPNP_CFG_S *pUpnpCfg)
{
	PL;
	if(pUpnpCfg->u8UpnpOpen != gSysInfo.struUpnpCfg.u8UpnpOpen
		|| pUpnpCfg->u8UpnpEthNo != gSysInfo.struUpnpCfg.u8UpnpEthNo
		|| pUpnpCfg->u8UpnpModel != gSysInfo.struUpnpCfg.u8UpnpModel
		|| pUpnpCfg->u8UpnpRefTime != gSysInfo.struUpnpCfg.u8UpnpRefTime)
	{
		PC;
	}
	memcpy(&gSysInfo.struUpnpCfg, pUpnpCfg, sizeof(NVR_UPNP_CFG_S));
	PNL;
	return TRUE;
}

DECLARE_GET_CFG(ntp, NVR_NTP_CFG_S, struNtpCfg);
//DECLARE_SET_CFG(ntp, NVR_NTP_CFG_S, struNtpCfg);

//DECLARE_GET_CFG(time, NVR_TIME_CFG_S, struTimeCfg);
//DECLARE_SET_CFG(time, NVR_TIME_CFG_S, struTimeCfg);
DECLARE_GET_CFG(auto, NVR_AUTO_MAINTAIN_CFG_S, struAutoCfg);
DECLARE_SET_CFG(auto, NVR_AUTO_MAINTAIN_CFG_S, struAutoCfg);

DECLARE_GET_ARRAY_CFG(rec, NVR_DEV_REC_STRC_S, struRecCfg);
//DECLARE_SET_ARRAY_CFG(rec, NVR_DEV_REC_STRC_S, struRecCfg);
gboolean nvr_set_rec_cfg(gint port, NVR_DEV_REC_STRC_S *pRecCfg)
{
	gboolean res;
	nvr_check_rec_cfg(pRecCfg);
	res = __nvr_set_cfg(pRecCfg, &gSysInfo.struRecCfg[port], sizeof(NVR_DEV_REC_STRC_S));
	if(res)
		nvr_rec_update(port);
	return res;
		
}

DECLARE_GET_CFG(osd_label, NVR_DEV_OSD_LABEL_CFG_S, struOsdLabelCfg);
DECLARE_SET_CFG(osd_label, NVR_DEV_OSD_LABEL_CFG_S, struOsdLabelCfg);

DECLARE_GET_ARRAY_STUM_CFG(vmask, NVR_DEV_VMASK_CFG_S, struVideoCfg, struVMask);
DECLARE_SET_ARRAY_STUM_CFG(vmask, NVR_DEV_VMASK_CFG_S, struVideoCfg, struVMask);

DECLARE_GET_ARRAY_STUM_CFG(osd, NVR_DEV_OSD_CFG_S, struVideoCfg, struOsd);
DECLARE_SET_ARRAY_STUM_CFG(osd, NVR_DEV_OSD_CFG_S, struVideoCfg, struOsd);

DECLARE_GET_ARRAY_CFG(user, NVR_DEV_USER_CFG_S, struUsrCfg);
DECLARE_SET_ARRAY_CFG(user, NVR_DEV_USER_CFG_S, struUsrCfg);
DECLARE_GET_ARRAY_CFG(comm, NVR_DEV_COMM_CFG_S, struCommCfg);
DECLARE_SET_ARRAY_CFG(comm, NVR_DEV_COMM_CFG_S, struCommCfg);

DECLARE_GET_ARRAY_CFG(ptz, NVR_DEV_PTZ_CFG_S, struPtzCfg);
DECLARE_SET_ARRAY_CFG(ptz, NVR_DEV_PTZ_CFG_S, struPtzCfg);

DECLARE_GET_ARRAY_CFG(video, NVR_DEV_VIDEO_CFG_S, struVideoCfg);
DECLARE_SET_ARRAY_CFG(video, NVR_DEV_VIDEO_CFG_S, struVideoCfg);
DECLARE_GET_ARRAY_CFG(audio, NVR_DEV_AUDIO_CFG_S, struAudioCfg);
DECLARE_SET_ARRAY_CFG(audio, NVR_DEV_AUDIO_CFG_S, struAudioCfg);

DECLARE_GET_CFG(jxj_platform, NVR_DEV_JXJ_PLATFORM_CFG_S, struJxjPlatformCfg);
DECLARE_SET_CFG(jxj_platform, NVR_DEV_JXJ_PLATFORM_CFG_S, struJxjPlatformCfg);

//DECLARE_GET_CFG(dianxin_platform, NVR_DEV_DIANXIN_PLATFORM_CFG_S, stDianxinPlatformCfg);
//DECLARE_SET_CFG(dianxin_platform, NVR_DEV_DIANXIN_PLATFORM_CFG_S, stDianxinPlatformCfg);
gboolean nvr_get_dianxin_platform_cfg(gint port, NVR_DEV_DIANXIN_PLATFORM_CFG_S *pDianxinPlatformCfg)
{
	DEV_KanDianCfg *pCfg = get_kandian_cfg();
	PL;
	memcpy(pDianxinPlatformCfg, &gSysInfo.stDianxinPlatformCfg, sizeof(NVR_DEV_DIANXIN_PLATFORM_CFG_S));
	if(strlen(pCfg->szGroupUrl) != 0)
		g_strlcpy(pDianxinPlatformCfg->szGroupUrl, pCfg->szGroupUrl, sizeof(pDianxinPlatformCfg->szGroupUrl));
	if(strlen(pCfg->szProvincialUrl) != 0)
		g_strlcpy(pDianxinPlatformCfg->szProvincialUrl, pCfg->szProvincialUrl, sizeof(pDianxinPlatformCfg->szProvincialUrl));
	if(strlen(pCfg->szDevId) != 0)
		g_strlcpy(pDianxinPlatformCfg->szDevId, pCfg->szDevId, sizeof(pDianxinPlatformCfg->szDevId));
	if(strlen(pCfg->szKey) != 0)
		g_strlcpy(pDianxinPlatformCfg->szKey, pCfg->szKey, sizeof(pDianxinPlatformCfg->szKey));
	if(strlen(pCfg->szMode) != 0)
		g_strlcpy(pDianxinPlatformCfg->szMode, pCfg->szMode, sizeof(pDianxinPlatformCfg->szMode));
	PNL;
	return TRUE;
}

gboolean nvr_set_dianxin_platform_cfg(gint port, NVR_DEV_DIANXIN_PLATFORM_CFG_S *pDianxinPlatformCfg)
{
	DEV_KanDianCfg *pCfg = get_kandian_cfg();
	PL;
	if(__nvr_compare_cfg(pDianxinPlatformCfg, &gSysInfo.stDianxinPlatformCfg, sizeof(NVR_DEV_DIANXIN_PLATFORM_CFG_S)))
	{
		int bChanged = 0;
		memcpy(&gSysInfo.stDianxinPlatformCfg, pDianxinPlatformCfg,  sizeof(NVR_DEV_DIANXIN_PLATFORM_CFG_S));
		PC;
		if(g_strcmp0(pDianxinPlatformCfg->szGroupUrl, pCfg->szGroupUrl) != 0)
		{
			g_strlcpy(pCfg->szGroupUrl, pDianxinPlatformCfg->szGroupUrl, sizeof(pCfg->szGroupUrl));
			bChanged = 1;
		}
		if(g_strcmp0(pDianxinPlatformCfg->szProvincialUrl, pCfg->szProvincialUrl) != 0)
		{
			g_strlcpy(pCfg->szProvincialUrl, pDianxinPlatformCfg->szProvincialUrl, sizeof(pCfg->szProvincialUrl));
			bChanged = 1;
		}
		if(g_strcmp0(pDianxinPlatformCfg->szDevId, pCfg->szDevId) != 0)
		{
			g_strlcpy(pCfg->szDevId, pDianxinPlatformCfg->szDevId, sizeof(pCfg->szDevId));
			bChanged = 1;
		}
		if(g_strcmp0(pDianxinPlatformCfg->szKey, pCfg->szKey) != 0)
		{
			g_strlcpy(pCfg->szKey, pDianxinPlatformCfg->szKey, sizeof(pCfg->szKey));
			bChanged = 1;
		}
		if(g_strcmp0(pDianxinPlatformCfg->szMode, pCfg->szMode) != 0)
		{
			g_strlcpy(pCfg->szMode, pDianxinPlatformCfg->szMode, sizeof(pCfg->szMode));
			bChanged = 1;
		}
		if(bChanged)
			save_kandian_cfg(pCfg);
	}
	PNL;
	return TRUE;
}

DECLARE_GET_ARRAY_CFG(di, NVR_DEV_DI_CFG_S, struDiCfg);
DECLARE_SET_ARRAY_CFG(di, NVR_DEV_DI_CFG_S, struDiCfg);
/*
gboolean nvr_set_di_cfg(gint port, NVR_DEV_DI_CFG_S *struDiCfg)
{
	PL;
	if(__nvr_compare_cfg(struDiCfg, &gSysInfo.struDiCfg[port], sizeof(NVR_DEV_DI_CFG_S)))
	{
		memcpy(&gSysInfo.struDiCfg[port], struDiCfg, sizeof(NVR_DEV_DI_CFG_S));
		PC;
		PNL;
		//报警触发处理
		nvr_alarm_proc(port);
		return TRUE;
	}
	PNL;
	return TRUE;
}
*/
DECLARE_GET_ARRAY_CFG(ipnc_alarm, NVR_DEV_IPNC_ALARM_CFG_S, struIpncAlarmCfg);
DECLARE_SET_ARRAY_CFG(ipnc_alarm, NVR_DEV_IPNC_ALARM_CFG_S, struIpncAlarmCfg);

DECLARE_GET_CFG(auto_ctrl, NVR_DEV_AUTO_CTRL_CFG_S, struAutoCtrlCfg);
DECLARE_SET_CFG(auto_ctrl, NVR_DEV_AUTO_CTRL_CFG_S, struAutoCtrlCfg);

gboolean nvr_get_time_cfg(gint port, NVR_TIME_S *pTimeCfg)
{
	nvr_sys_timer_get(port, pTimeCfg);
	return TRUE;
}
gboolean nvr_set_time_cfg(gint port, NVR_TIME_S *pTimeCfg)
{
	nvr_sys_timer_set(port, pTimeCfg);
	nvr_channel_update_time(pTimeCfg);
	return TRUE;
}

gboolean nvr_get_var_cfg(gint port, NVR_DEV_VAR_INFO_S *pVarCfg)
{
	PL;
	memcpy(pVarCfg, &gSysInfo.struDevCfg.stVarInfo, sizeof(NVR_DEV_VAR_INFO_S));
	PNL;
	return TRUE;
}
gboolean nvr_set_var_cfg(gint port, NVR_DEV_VAR_INFO_S *pVarCfg)
{
	PL;
	if(__nvr_compare_cfg(pVarCfg, &gSysInfo.struDevCfg.stVarInfo, sizeof(NVR_DEV_VAR_INFO_S)))
	{
		memcpy(&gSysInfo.struDevCfg.stVarInfo, pVarCfg, sizeof(NVR_DEV_VAR_INFO_S));
		PC;
	}
	PNL;
	return TRUE;
}

gboolean nvr_get_stable_cfg(gint port, NVR_DEV_STABLE_INFO_S *pStableCfg)
{
	PL;
	memcpy(pStableCfg, &gSysInfo.struDevCfg.stStableInfo, sizeof(NVR_DEV_STABLE_INFO_S));
	PNL;
	return TRUE;
}
//主码流
gboolean nvr_get_main_venc_cfg(int nPort, NVR_DEV_VENC_CFG_S *pVencCfg)
{
		
	PL;
	if(nPort < NVR_MAX_CHANNEL_NUM)
		memcpy(pVencCfg, &gSysInfo.struVideoCfg[nPort].struMainVenc, sizeof(NVR_DEV_VENC_CFG_S));
	PNL;

	return TRUE;
}

gboolean nvr_set_main_venc_cfg(int nPort, NVR_DEV_VENC_CFG_S *pVencCfg)
{
	PL;
	if(__nvr_compare_cfg(pVencCfg, &gSysInfo.struVideoCfg[nPort].struMainVenc, sizeof(NVR_DEV_VENC_CFG_S)))
	{
		memcpy(&gSysInfo.struVideoCfg[nPort].struMainVenc, pVencCfg, sizeof(NVR_DEV_VENC_CFG_S));
		PC;
	}
	PNL;

	return TRUE;
}
//次码流
gboolean nvr_get_sub_venc_cfg(int nPort, NVR_DEV_VENC_CFG_S *pVencCfg)
{
		
	PL;
	if(nPort < NVR_MAX_CHANNEL_NUM)
		memcpy(pVencCfg, &gSysInfo.struVideoCfg[nPort].struSubVenc, sizeof(NVR_DEV_VENC_CFG_S));
	PNL;

	return TRUE;
}

gboolean nvr_set_sub_venc_cfg(int nPort, NVR_DEV_VENC_CFG_S *pVencCfg)
{
	PL;
	if(__nvr_compare_cfg(pVencCfg, &gSysInfo.struVideoCfg[nPort].struSubVenc, sizeof(NVR_DEV_VENC_CFG_S)))
	{
		memcpy(&gSysInfo.struVideoCfg[nPort].struSubVenc, pVencCfg, sizeof(NVR_DEV_VENC_CFG_S));
		PC;
	}
	PNL;

	return TRUE;
}


DECLARE_GET_ARRAY_CFG(channel, NVR_CHANNEL_CFG_S, struChannelCfg);
gboolean nvr_set_channel_cfg(gint port, NVR_CHANNEL_CFG_S *pChannelCfg)
{
#if 0	
	int ret = TRUE;
	int i;
	
	PL;
	// 先检测是否设置过相同的channel
	for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{
		if(i == port)
			continue;
		if(pChannelCfg->u32IpAddr == gSysInfo.struChannelCfg[i].u32IpAddr)
		{
			PNL;
			return FALSE;
		}
		else if(0 == pChannelCfg->u32IpAddr)
		{
			PNL;
			return FALSE;
		}	
	}
	PNL;
#endif	
	if(0 == pChannelCfg->u32IpAddr)
		pChannelCfg->u8SearchA = 1;
	__nvr_set_cfg(pChannelCfg, 
					&gSysInfo.struChannelCfg[port], 
					sizeof(NVR_CHANNEL_CFG_S));
	// 因为onvif暂时不支持报警，所以设置为全天录像
	if(strcmp(pChannelCfg->szProtocolName, "onvif_cu") == 0)
	{
		PL;
		if(gSysInfo.struRecCfg[port].u8RecType == 2)
		{
			gSysInfo.struRecCfg[port].u8RecType = 0;
			PC;
		}
		PNL;
	}
	nvr_channel_update_cfg(port, pChannelCfg);
	nvr_search_update_channel(port, pChannelCfg);
	nvr_smart_update_channel(port, pChannelCfg);
	
	return TRUE;
}

gboolean nvr_set_ntp_cfg(gint port, NVR_NTP_CFG_S *pNtpCfg)
{
	/*配置*/
	/*j_debug("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	j_debug("nTimeZone %d, %d", pNtpCfg->nTimeZone, gSysInfo.struNtpCfg.nTimeZone);
	j_debug("szNtpServer %s, %s", pNtpCfg->szNtpServer, gSysInfo.struNtpCfg.szNtpServer);
	j_debug("u16NtpPort %d, %d", pNtpCfg->u16NtpPort, gSysInfo.struNtpCfg.u16NtpPort);
	j_debug("u32Size %d, %d", pNtpCfg->u32Size, gSysInfo.struNtpCfg.u32Size);
	j_debug("u8EuroTime %d, %d", pNtpCfg->u8EuroTime, gSysInfo.struNtpCfg.u8EuroTime);
	j_debug("u8NtpOpen %d, %d", pNtpCfg->u8NtpOpen, gSysInfo.struNtpCfg.u8NtpOpen);
	j_debug("u8NtpRefTime %d, %d", pNtpCfg->u8NtpRefTime, gSysInfo.struNtpCfg.u8NtpRefTime);
	j_debug("u8NtpUpdateFlag %d, %d", pNtpCfg->u8NtpUpdateFlag, gSysInfo.struNtpCfg.u8NtpUpdateFlag);
	j_debug("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");*/
	__nvr_set_cfg(pNtpCfg, &gSysInfo.struNtpCfg, sizeof(gSysInfo.struNtpCfg));
	nvr_ntp_update();
	
	return 0;
}

int	nvr_get_sys_chn_num()
{
	return gStableInfo.u8ChnNum;
}

int	nvr_get_sys_di_num()
{
	return gStableInfo.u8AlarmInputNum;
}

int	nvr_get_sys_do_num()
{
	return gStableInfo.u8AlarmOutputNum;
}

int	nvr_get_sys_rs232_num()
{
	return gStableInfo.u8RS232Num;
}

int	nvr_get_sys_rs485_num()
{
	return gStableInfo.u8RS485Num;
}

int	nvr_get_sys_comm_num()
{
	return nvr_get_sys_rs232_num() + nvr_get_sys_rs485_num();
}

int	nvr_get_dianxin_is_used()
{
	int ret;
	DEV_SystemCfg *pCfg = get_system_cfg();
	// 通过看店平台设置参数，设置标志来控制有些nvr行为
	PL;
	ret = (strlen(gSysInfo.stDianxinPlatformCfg.szProvincialUrl) != 0 || strlen(gSysInfo.stDianxinPlatformCfg.szGroupUrl) != 0)
			&& strlen(gSysInfo.stDianxinPlatformCfg.szDevId) != 0 
			&& strlen(gSysInfo.stDianxinPlatformCfg.szKey) != 0 
			&& strlen(gSysInfo.stDianxinPlatformCfg.szMode) != 0
			&& (pCfg->platformServer & PLATFORM_KANDIAN_SERVER);
	PNL;
	return ret;
}

int	nvr_get_filesystem_is_default()
{	
	return g_atomic_int_get(&gIsDefaultFileSystem);
}

