#ifndef __NVR_PARAM_CHECK_H__
#define __NVR_PARAM_CHECK_H__

#include <nvr_server_headers.h>


void nvr_check_var_cfg(NVR_DEV_VAR_INFO_S *varInfo);
void nvr_check_stable_cfg(NVR_DEV_STABLE_INFO_S *stableInfo);
void nvr_check_dev_cfg(NVR_DEV_INFO_S *varInfo);
void nvr_check_net_cfg(NVR_DEV_NET_CFG_S *netCfg);
void nvr_check_ftp_cfg(NVR_FTP_CFG_S *ftpCfg);
void nvr_check_smtp_cfg(NVR_SMTP_CFG_S *smtpCfg);
void nvr_check_ddns_cfg(NVR_DDNS_CFG_S *ddnsCfg);
void nvr_check_upnp_cfg(NVR_UPNP_CFG_S *upnpCfg);
void nvr_check_time_cfg(NVR_TIME_CFG_S *timeCfg);
void nvr_check_ntp_cfg(NVR_NTP_CFG_S *ntpCfg);
void nvr_check_rec_cfg(NVR_DEV_REC_STRC_S *recCfg);
void nvr_check_user_cfg(NVR_DEV_USER_CFG_S *userCfg);
void nvr_check_auto_cfg(NVR_AUTO_MAINTAIN_CFG_S *autoCfg);
void nvr_check_comm_cfg(NVR_DEV_COMM_CFG_S *commCfg);
void nvr_check_channel_cfg(NVR_CHANNEL_CFG_S *channelCfg);
void nvr_check_video_cfg(NVR_DEV_VIDEO_CFG_S *videoCfg);
void nvr_check_audio_cfg(NVR_DEV_AUDIO_CFG_S *audioCfg);
void nvr_check_jxj_platform_cfg(NVR_DEV_JXJ_PLATFORM_CFG_S *pJxjPlatformCfg);
void nvr_check_dianxin_platform_cfg(NVR_DEV_DIANXIN_PLATFORM_CFG_S *pDianxinPlatformCfg);
void nvr_check_vmask_cfg(NVR_DEV_VMASK_CFG_S *vmaskCfg);
void nvr_check_main_venc_cfg(NVR_DEV_VENC_CFG_S *vmainCfg);

void nvr_check_osd_label_cfg(NVR_DEV_OSD_LABEL_CFG_S *osdLabelCfg);
void nvr_check_osd_cfg(NVR_DEV_OSD_CFG_S *osdCfg);

void nvr_check_ptz_cfg(NVR_DEV_PTZ_CFG_S *ptzCfg);
void nvr_check_di_cfg(NVR_DEV_DI_CFG_S *pDiCfg);
void nvr_check_ipnc_alarm_cfg(NVR_DEV_IPNC_ALARM_CFG_S *pIpncAlarmCfg);
void nvr_check_auto_ctrl_cfg(NVR_DEV_AUTO_CTRL_CFG_S *pAutoCtrlCfg);

void nvr_param_check_stable_param_info();
void nvr_param_check_sys_param_info();


#endif

