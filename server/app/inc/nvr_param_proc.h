#ifndef __NVR_PARAM_PROC_H__
#define __NVR_PARAM_PROC_H__

#include <nvr_server_headers.h>

gboolean nvr_get_dev_cfg(gint port, NVR_DEV_INFO_S *pDevCfg);
gboolean nvr_set_dev_cfg(gint port, NVR_DEV_INFO_S *pDevCfg);

gboolean nvr_get_var_cfg(gint port, NVR_DEV_VAR_INFO_S *pVarCfg);
gboolean nvr_set_var_cfg(gint port, NVR_DEV_VAR_INFO_S *pVarCfg);

gboolean nvr_get_stable_cfg(gint port, NVR_DEV_STABLE_INFO_S *pStableCfg);
//gboolean nvr_set_stable_cfg(gint port, NVR_DEV_STABLE_INFO_S *pStableCfg);

gboolean nvr_get_net_cfg(gint port, NVR_DEV_NET_CFG_S *pNetCfg);
gboolean nvr_set_net_cfg(gint port, NVR_DEV_NET_CFG_S *pNetCfg);
gboolean nvr_update_net_cfg(gint port, NVR_DEV_NET_CFG_S *pNetCfg);



gboolean nvr_get_ftp_cfg(gint port, NVR_FTP_CFG_S *pFtpCfg);
gboolean nvr_set_ftp_cfg(gint port, NVR_FTP_CFG_S *pFtpCfg);

gboolean nvr_get_smtp_cfg(gint port, NVR_SMTP_CFG_S *pFtpCfg);
gboolean nvr_set_smtp_cfg(gint port, NVR_SMTP_CFG_S *pFtpCfg);

gboolean nvr_get_ddns_cfg(gint port, NVR_DDNS_CFG_S *pFtpCfg);
gboolean nvr_set_ddns_cfg(gint port, NVR_DDNS_CFG_S *pFtpCfg);

gboolean nvr_get_upnp_cfg(gint port, NVR_UPNP_CFG_S *pUpnpCfg);
gboolean nvr_set_upnp_cfg(gint port, NVR_UPNP_CFG_S *pUpnpCfg);

gboolean nvr_get_ntp_cfg(gint port, NVR_NTP_CFG_S *pNtpCfg);
gboolean nvr_set_ntp_cfg(gint port, NVR_NTP_CFG_S *pNtpCfg);

//gboolean nvr_get_time_cfg(gint port, NVR_TIME_CFG_S *pTimeCfg);
//gboolean nvr_set_time_cfg(gint port, NVR_TIME_CFG_S *pTimeCfg);
gboolean nvr_get_time_cfg(gint port, NVR_TIME_S *pTimeCfg);
gboolean nvr_set_time_cfg(gint port, NVR_TIME_S *pTimeCfg);

gboolean nvr_get_user_cfg(gint port, NVR_DEV_USER_CFG_S *pUsrCfg);
gboolean nvr_set_user_cfg(gint port, NVR_DEV_USER_CFG_S *pUsrCfg);

gboolean nvr_get_auto_cfg(gint port, NVR_AUTO_MAINTAIN_CFG_S *pUsrCfg);
gboolean nvr_set_auto_cfg(gint port, NVR_AUTO_MAINTAIN_CFG_S *pUsrCfg);

gboolean nvr_get_channel_cfg(gint port, NVR_CHANNEL_CFG_S *pChannelCfg);
gboolean nvr_set_channel_cfg(gint port, NVR_CHANNEL_CFG_S *pChannelCfg);

gboolean nvr_get_comm_cfg(gint port, NVR_DEV_COMM_CFG_S *pCommCfg);
gboolean nvr_set_comm_cfg(gint port, NVR_DEV_COMM_CFG_S *pCommCfg);

gboolean nvr_get_video_cfg(gint port, NVR_DEV_VIDEO_CFG_S *pStruVideoCfg);
gboolean nvr_set_video_cfg(gint port, NVR_DEV_VIDEO_CFG_S *pStruVideoCfg);
gboolean nvr_get_audio_cfg(gint port, NVR_DEV_AUDIO_CFG_S *pStruAudioCfg);
gboolean nvr_set_audio_cfg(gint port, NVR_DEV_AUDIO_CFG_S *pStruAudioCfg);

gboolean nvr_get_vmask_cfg(gint port, NVR_DEV_VMASK_CFG_S *pStruVmaskCfg);
gboolean nvr_set_vmask_cfg(gint port, NVR_DEV_VMASK_CFG_S *pStruVmaskCfg);

gboolean nvr_get_osd_label_cfg(gint port, NVR_DEV_OSD_LABEL_CFG_S *pStruOsdLabelCfg);
gboolean nvr_set_osd_label_cfg(gint port, NVR_DEV_OSD_LABEL_CFG_S *pStruOsdLabelCfg);
gboolean nvr_get_osd_cfg(gint port, NVR_DEV_OSD_CFG_S *pStruOsdCfg);
gboolean nvr_set_osd_cfg(gint port, NVR_DEV_OSD_CFG_S *pStruOsdCfg);

gboolean nvr_get_ptz_cfg(gint port, NVR_DEV_PTZ_CFG_S *pStruPtzCfg);
gboolean nvr_set_ptz_cfg(gint port, NVR_DEV_PTZ_CFG_S *pStruPtzCfg);

//主码流
gboolean nvr_get_main_venc_cfg(int nPort, NVR_DEV_VENC_CFG_S *pVencCfg);
gboolean nvr_set_main_venc_cfg(int nPort, NVR_DEV_VENC_CFG_S *pVencCfg);
//次码流
gboolean nvr_get_sub_venc_cfg(int nPort, NVR_DEV_VENC_CFG_S *pVencCfg);
gboolean nvr_set_sub_venc_cfg(int nPort, NVR_DEV_VENC_CFG_S *pVencCfg);

gboolean nvr_get_rec_cfg(gint port, NVR_DEV_REC_STRC_S *pRecCfg);
gboolean nvr_set_rec_cfg(gint port, NVR_DEV_REC_STRC_S *pRecCfg);

gboolean nvr_get_jxj_platform_cfg(gint port, NVR_DEV_JXJ_PLATFORM_CFG_S *pJxjPlatformCfg);
gboolean nvr_set_jxj_platform_cfg(gint port, NVR_DEV_JXJ_PLATFORM_CFG_S *pJxjPlatformCfg);

gboolean nvr_get_dianxin_platform_cfg(gint port, NVR_DEV_DIANXIN_PLATFORM_CFG_S *pDianxinPlatformCfg);
gboolean nvr_set_dianxin_platform_cfg(gint port, NVR_DEV_DIANXIN_PLATFORM_CFG_S *pDianxinPlatformCfg);


gboolean nvr_get_di_cfg(gint port, NVR_DEV_DI_CFG_S *pDiCfg);
gboolean nvr_set_di_cfg(gint port, NVR_DEV_DI_CFG_S *pDiCfg);

gboolean nvr_get_ipnc_alarm_cfg(gint port, NVR_DEV_IPNC_ALARM_CFG_S *pIpncAlarmCfg);
gboolean nvr_set_ipnc_alarm_cfg(gint port, NVR_DEV_IPNC_ALARM_CFG_S *pIpncAlarmCfg);

gboolean nvr_get_auto_ctrl_cfg(gint port, NVR_DEV_AUTO_CTRL_CFG_S *pAutoCtrlCfg);
gboolean nvr_set_auto_ctrl_cfg(gint port, NVR_DEV_AUTO_CTRL_CFG_S *pAutoCtrlCfg);

int	nvr_get_sys_chn_num();
int	nvr_get_sys_di_num();
int	nvr_get_sys_do_num();
int	nvr_get_sys_rs232_num();
int	nvr_get_sys_rs485_num();
int	nvr_get_sys_comm_num();
int	nvr_get_dianxin_is_used();
int	nvr_get_filesystem_is_default();

#endif
