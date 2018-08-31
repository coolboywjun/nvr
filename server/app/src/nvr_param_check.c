
#include <nvr_server_headers.h>
#include <nvr_param_sys.h>
#include "nvr_net_common.h"
#include "nvr_ntp.h"
#include "nvr_upnp.h"


#define NVR_PARAM_MAX_VALUE(a, b) {if ((a) > (b)) (a)=(b);}

#define NVR_PARAM_MAX_MIN_VALUE(value, maxV, minV) \
											{		   \
												if ((value) > (maxV))\
													value = maxV;\
												else if ((value) < (minV))\
													value = minV;\
											}

static void nvr_param_check_point(LPNVR_POINT_S pPoint, int32_t s32MaxX, int32_t s32MaxY )
{
	if (pPoint == NULL)
		return ;
	
	if (pPoint->s32X > s32MaxX)
		pPoint->s32X = s32MaxX;

	if (pPoint->s32Y > s32MaxY)
		pPoint->s32Y = s32MaxY;
	return ;
}

static void nvr_param_check_area(LPNVR_RECT_S pArea, uint32_t u32MaxW, uint32_t u32MaxH)
{
	if (pArea == NULL)
		return ;
	
	if (pArea->s32X < 0)
		pArea->s32X = 0;
	else if (pArea->s32X > u32MaxW)
		pArea->s32X = u32MaxW;

	if (pArea->s32Y < 0)
		pArea->s32Y = 0;
	else if (pArea->s32Y > u32MaxH)
		pArea->s32Y = u32MaxH;

	if (pArea->u32Width < 0)
		pArea->u32Width = 0;
	else if (pArea->u32Width > (u32MaxW - pArea->s32X))
		pArea->u32Width = (u32MaxW - pArea->s32X);

	if (pArea->u32Height < 0)
		pArea->u32Height = 0;
	else if (pArea->u32Height > (u32MaxH - pArea->s32Y))
		pArea->u32Height = (u32MaxH - pArea->s32Y);
	
	return ;
}

void	nvr_param_check_bool_value(uint8_t *pU8Value, uint8_t u8DefValue)
{
	if (pU8Value == NULL)
		return ;

	if (*pU8Value != 0 && *pU8Value != 1)
		*pU8Value = u8DefValue;
	return ;
}

void nvr_param_check_time_seg(NVR_SEGMENT_S *pSeg)
{

	if (pSeg->u8Open != 0 && pSeg->u8Open != 1)
		pSeg->u8Open = 0;	
		
	NVR_PARAM_MAX_MIN_VALUE(pSeg->beginSec, 24*3600-1, 0);
	NVR_PARAM_MAX_MIN_VALUE(pSeg->endSec, 24*3600-1, pSeg->beginSec);
}

void nvr_check_var_cfg(NVR_DEV_VAR_INFO_S *varInfo)
{
}

void nvr_check_stable_cfg(NVR_DEV_STABLE_INFO_S *stableInfo)
{
}

void nvr_check_dev_cfg(NVR_DEV_INFO_S *varInfo)
{
	nvr_check_var_cfg(&varInfo->stVarInfo);
	nvr_check_stable_cfg(&varInfo->stStableInfo);
}

void nvr_check_net_cfg(NVR_DEV_NET_CFG_S *netCfg)
{
}

void nvr_check_ftp_cfg(NVR_FTP_CFG_S *ftpCfg)
{
}

void nvr_check_smtp_cfg(NVR_SMTP_CFG_S *smtpCfg)
{
	if (smtpCfg == NULL)
		return ;

	if (smtpCfg->u32Size != sizeof(NVR_SMTP_CFG_S))
		smtpCfg->u32Size = sizeof(NVR_SMTP_CFG_S);

	smtpCfg->szMailHub[NVR_IP_ADDR_LEN-1] 	= '\0'; //smtp 服务器地址
	smtpCfg->szMailAddr[NVR_MAX_EMAIL_LEN-1]	='\0';//发送邮件 地址
	smtpCfg->szMailUsr[NVR_MAX_EMAIL_LEN-1]	= '\0'; //发送邮件 账号
	smtpCfg->szMailPsw[NVR_PASS_WORD_LEN-1]	= '\0';   //发送邮件 密码
	smtpCfg->szMailRctp1[NVR_MAX_EMAIL_LEN-1]	= '\0';//接受邮件 地址1
	smtpCfg->szMailRctp2[NVR_MAX_EMAIL_LEN-1]	= '\0';//接受邮件 地址2
	smtpCfg->szMailRctp3[NVR_MAX_EMAIL_LEN-1]	= '\0';//接受邮件 地址3
	nvr_param_check_bool_value(&smtpCfg->u8EnableSSL, 0);	//启用SSL
}

void nvr_check_ddns_cfg(NVR_DDNS_CFG_S *ddnsCfg)
{
	if (ddnsCfg->u32Size != sizeof(NVR_DDNS_CFG_S))
		ddnsCfg->u32Size = sizeof(NVR_DDNS_CFG_S);
		
	nvr_param_check_bool_value(&ddnsCfg->u8DDNSOpen, 0);	//DDNS开关
	NVR_PARAM_MAX_VALUE(ddnsCfg->u8DDNSType, 1);
	ddnsCfg->szDDNSAccount[NVR_NAME_LEN - 1] = '\0';		//DDNS 注册名
	ddnsCfg->szDDNSUsr[NVR_USER_NAME_LEN-1]	= '\0';	//用户名称
	ddnsCfg->szDDNSPsw[NVR_PASS_WORD_LEN-1]	= '\0';	//用户密码	
	NVR_PARAM_MAX_MIN_VALUE(ddnsCfg->u32DDNSTimes, 48, 1);
}

void nvr_check_video_cfg(NVR_DEV_VIDEO_CFG_S *videoCfg)
{
}
void nvr_check_audio_cfg(NVR_DEV_AUDIO_CFG_S *audioCfg)
{
}
void nvr_check_rec_cfg(NVR_DEV_REC_STRC_S *recCfg)
{

}

void nvr_check_ptz_cfg(NVR_DEV_PTZ_CFG_S *ptzCfg)
{

}

void nvr_check_osd_label_cfg(NVR_DEV_OSD_LABEL_CFG_S *osdLabelCfg)
{
}
void nvr_check_osd_cfg(NVR_DEV_OSD_CFG_S *osdCfg)
{
}

void nvr_check_vmask_cfg(NVR_DEV_VMASK_CFG_S *vmaskCfg)
{
}
void nvr_check_main_venc_cfg(NVR_DEV_VENC_CFG_S *vmainCfg)
{
}

void nvr_check_upnp_cfg(NVR_UPNP_CFG_S *upnpCfg)
{
	if (upnpCfg == NULL)
		return ;

	if (upnpCfg->u32Size != sizeof(NVR_UPNP_CFG_S))
		upnpCfg->u32Size = sizeof(NVR_UPNP_CFG_S);

	nvr_param_check_bool_value(&upnpCfg->u8UpnpOpen, 0);
	nvr_param_check_bool_value(&upnpCfg->u8UpnpEthNo, 0);
	nvr_param_check_bool_value(&upnpCfg->u8UpnpModel, 0);
	NVR_PARAM_MAX_MIN_VALUE(upnpCfg->u8UpnpRefTime, 48, 1);
	upnpCfg->szUpnpSvr[NVR_IP_ADDR_LEN-1] = '\0';
}

void nvr_check_ntp_cfg(NVR_NTP_CFG_S *ntpCfg)
{
	if (ntpCfg == NULL)
		return ;
	
	if (ntpCfg->u32Size != sizeof(NVR_NTP_CFG_S))
		ntpCfg->u32Size = sizeof(NVR_NTP_CFG_S);

	nvr_param_check_bool_value(&ntpCfg->u8NtpOpen, 0);
	nvr_param_check_bool_value(&ntpCfg->u8EuroTime, 0);

	NVR_PARAM_MAX_MIN_VALUE(ntpCfg->u8NtpRefTime, 24*7, 1);
	NVR_PARAM_MAX_MIN_VALUE(ntpCfg->nTimeZone, 11*3600, -12*3600);
	ntpCfg->szNtpServer[NVR_IP_ADDR_LEN-1] = '\0';
}

void nvr_check_time_cfg(NVR_TIME_S *ntpCfg)
{
}

void nvr_check_user_cfg(NVR_DEV_USER_CFG_S *userCfg)
{
}

void nvr_check_auto_cfg(NVR_AUTO_MAINTAIN_CFG_S *autoCfg)
{
	if(NULL == autoCfg)
		return ;

	autoCfg->u8Enable = !!autoCfg->u8Enable;
}

void nvr_check_comm_cfg(NVR_DEV_COMM_CFG_S *commCfg)
{
}

void nvr_check_channel_cfg(NVR_CHANNEL_CFG_S *channelCfg)
{
}

void nvr_check_jxj_platform_cfg(NVR_DEV_JXJ_PLATFORM_CFG_S *pJxjPlatformCfg)
{
	if(pJxjPlatformCfg == NULL)
		return;
	
	if(pJxjPlatformCfg->u32Size != sizeof(NVR_DEV_JXJ_PLATFORM_CFG_S))
		pJxjPlatformCfg->u32Size = sizeof(NVR_DEV_JXJ_PLATFORM_CFG_S);
	if(pJxjPlatformCfg->u8Enable != 0 && pJxjPlatformCfg->u8Enable != 1)
		pJxjPlatformCfg->u8Enable = 1;
	if(pJxjPlatformCfg->u8ProtocolType != 0 && pJxjPlatformCfg->u8ProtocolType != 1)
		pJxjPlatformCfg->u8ProtocolType = 0;
}

void nvr_check_dianxin_platform_cfg(NVR_DEV_DIANXIN_PLATFORM_CFG_S *pDianxinPlatformCfg)
{
	if(pDianxinPlatformCfg == NULL)
		return;
}


void nvr_check_di_cfg(NVR_DEV_DI_CFG_S *pDiCfg)
{
	
}

void nvr_check_ipnc_alarm_cfg(NVR_DEV_IPNC_ALARM_CFG_S *pIpncAlarmCfg)
{
	g_return_if_fail(pIpncAlarmCfg != NULL);
	if(pIpncAlarmCfg->u32Size != sizeof(NVR_DEV_IPNC_ALARM_CFG_S))
		pIpncAlarmCfg->u32Size = sizeof(NVR_DEV_IPNC_ALARM_CFG_S);
}

void nvr_check_auto_ctrl_cfg(NVR_DEV_AUTO_CTRL_CFG_S *pAutoCtrlCfg)
{
	g_return_if_fail(pAutoCtrlCfg != NULL);
	if(pAutoCtrlCfg->u32Size != sizeof(NVR_DEV_AUTO_CTRL_CFG_S))
		pAutoCtrlCfg->u32Size = sizeof(NVR_DEV_AUTO_CTRL_CFG_S);
}

void nvr_param_check_stable_param_info()
{
	int i;
	DEV_SystemCfg *pCfg = get_system_cfg();
	
	if(gStableInfo.u32Size  != sizeof(gStableInfo))
		gStableInfo.u32Size = sizeof(gStableInfo);
	g_stpcpy(gStableInfo.szSerialId, (char *)pCfg->u8SerialId);
	g_stpcpy(gStableInfo.szHwVer, (char *)pCfg->u8HwVer);
	g_stpcpy(gStableInfo.szSoftVer, NVR_SOFT_VERSION);
	j_debug("software version: %s", NVR_SOFT_VERSION);
	
	for (i = 0; i < NVR_ETHERNET_NUM; ++i)
	{
		gStableInfo.u8MacAddr[i][0] = pCfg->u8Mac[0];
		gStableInfo.u8MacAddr[i][1] = pCfg->u8Mac[1];
		gStableInfo.u8MacAddr[i][2] = pCfg->u8Mac[2];
		gStableInfo.u8MacAddr[i][3] = pCfg->u8Mac[3];
		gStableInfo.u8MacAddr[i][4] = pCfg->u8Mac[4];
		gStableInfo.u8MacAddr[i][5] = pCfg->u8Mac[5] + i;

		gStableInfo.u32IPAddr[i] = pCfg->defaultIP + i;
		gStableInfo.u32NetMask[i]= pCfg->defaultMask;
		gStableInfo.u32GateWay[i]= pCfg->defaultGateWay;
	}
	
	gStableInfo.u8SupportAudio = pCfg->u8SupportAudio;
	gStableInfo.u8SupportHDMI = pCfg->u8SupportHDMI;
	gStableInfo.u8supportVGA = pCfg->u8supportVGA;
	gStableInfo.u8NetNum = pCfg->u8NetCardNum;
	gStableInfo.u8SataNum = pCfg->u8SataNum;
	gStableInfo.u8ChnNum = (pCfg->u8ChnNum < NVR_MAX_CHANNEL_NUM) ? pCfg->u8ChnNum : NVR_MAX_CHANNEL_NUM;
	gStableInfo.u8RS232Num = (pCfg->u8Rs232Num < NVR_MAX_SUPPORT_RS232) ? pCfg->u8Rs232Num : NVR_MAX_SUPPORT_RS232;
	gStableInfo.u8RS485Num = (pCfg->u8Rs485Num < NVR_MAX_SUPPORT_RS485) ? pCfg->u8Rs485Num : NVR_MAX_SUPPORT_RS485;
	gStableInfo.u8AlarmInputNum = (pCfg->u8DiNum < NVR_MAX_SUPPORT_DI) ? pCfg->u8DiNum : NVR_MAX_SUPPORT_DI; 
	gStableInfo.u8AlarmOutputNum = (pCfg->u8DoNum < NVR_MAX_SUPPORT_DO) ? pCfg->u8DoNum : NVR_MAX_SUPPORT_DO;
}

void nvr_param_check_sys_param_info()
{
	if(gSysInfo.u32Size != sizeof(gSysInfo))
		gSysInfo.u32Size = sizeof(gSysInfo);

	memcpy(&gSysInfo.struDevCfg.stStableInfo, &gStableInfo, sizeof(gStableInfo));
	// 清除upnp状态
	memset(gSysInfo.struUpnpCfg.szExternalIp, 0, sizeof(gSysInfo.struUpnpCfg.szExternalIp));
	gSysInfo.struUpnpCfg.u16UpnpWebPort = gSysInfo.struNetCfg.u16HttpPort;
	gSysInfo.struUpnpCfg.u16UpnpRtspPort = 1554;
	gSysInfo.struUpnpCfg.u16UpnpCmdPort = gSysInfo.struNetCfg.u16CmdPort;
	gSysInfo.struUpnpCfg.u16UpnpJXJDataPort = gSysInfo.struNetCfg.u16JXJDataPort;
	gSysInfo.struUpnpCfg.u16UpnpTalkPort = gSysInfo.struNetCfg.u16TalkPort;
	gSysInfo.struUpnpCfg.u16UpnpDataPortS = 0;
	gSysInfo.struUpnpCfg.u16UpnpWebPortS = 0;
	gSysInfo.struUpnpCfg.u16UpnpRtspPortS = 0;
	gSysInfo.struUpnpCfg.u16UpnpCmdPortS = 0;
	gSysInfo.struUpnpCfg.u16UpnpTalkPortS = 0;
	gSysInfo.struUpnpCfg.u16UpnpJXJDataPortS = 0;
}

