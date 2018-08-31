#ifndef __NVR_LIB_INTERFACE_H__
#define __NVR_LIB_INTERFACE_H__


#include <ipnc_lib.h>
#include <nvr_struct.h>
#include <nvr_net_struct.h>
#include <nvr_net_interface.h>


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

///////////////////////////////////
int nvr_lib_init();
void nvr_lib_uninit();


////////////////////////////////////
int nvr_restart();
int nvr_shutdown();
int nvr_sw_reset();
int nvr_set_hwgpio_check(int open);

int nvr_lib_get_pro_table(IPNC_PROTOCOL_S* ipncProtocol, int protocolNum);
int nvr_lib_get_pro_table_num(void);

/////////////////////////////////////
long nvr_lib_search_start(const char* pProtocolName);
int  nvr_lib_search_get(long lHandle, IPNC_NET_SEARCH_RES_S *pRecvBuf);
int  nvr_lib_search_stop(long lHandle);
int  nvr_lib_search_get_ipc_status(long lHandle, char* ipAddr, int ipSize);
int  nvr_lib_search_cfg(long lHandle, IPNC_NET_SEARCH_CFG_REQ_S *pSearchCfg, int size);
int  nvr_lib_search_req_cpy_cfg(IPNC_NET_SEARCH_CFG_REQ_S *pSearchCfg, IPNC_NET_SEARCH_RES_S *pDev);

///////////////////////////////////////
int nvr_export_param(const char *pExportPath);
int nvr_import_param(const char *pImportPath);

///////////////////////////////////////
int nvr_get_ipnc_param(int chnNo, IPNC_CMD_TYPE_E paramCmd, void* pNetParam_S, int paramSize);
int nvr_set_ipnc_param(int chnNo, IPNC_CMD_TYPE_E paramCmd, void* pNetParam_S, int paramSize);


////////////////////////////////////////

int nvr_set_ipnc_ima_def_param(int chnNo, IPNC_DEV_IMA_CFG_S *pDefImaCfg);
int nvr_set_ipnc_3a_def_param(int chnNo, IPNC_DEV_3A_CFG_S *pDef3aCfg);


int nvr_logon_get_dev_cfg(int port, NVR_DEV_INFO_S *pCfg);
int nvr_logon_set_dev_cfg(int port, NVR_DEV_INFO_S *pCfg);

int nvr_logon_get_user_cfg(int port, NVR_DEV_USER_CFG_S *pCfg);
int nvr_logon_set_user_cfg(int port, NVR_DEV_USER_CFG_S *pCfg);

int nvr_logon_get_net_cfg(int port, NVR_DEV_NET_CFG_S *pCfg);
int nvr_logon_set_net_cfg(int port, NVR_DEV_NET_CFG_S *pCfg);

int nvr_logon_get_ntp_cfg(int port, NVR_NTP_CFG_S *pCfg);
int nvr_logon_set_ntp_cfg(int port, NVR_NTP_CFG_S *pCfg);

int nvr_logon_get_upnp_cfg(int port, NVR_UPNP_CFG_S *pCfg);
int nvr_logon_set_upnp_cfg(int port, NVR_UPNP_CFG_S *pCfg);

int nvr_logon_get_smtp_cfg(int port, NVR_SMTP_CFG_S *pCfg);
int nvr_logon_set_smtp_cfg(int port, NVR_SMTP_CFG_S *pCfg);

int nvr_logon_get_ftp_cfg(int port, NVR_FTP_CFG_S *pCfg);
int nvr_logon_set_ftp_cfg(int port, NVR_FTP_CFG_S *pCfg);

int nvr_logon_get_comm_cfg(int port, NVR_DEV_COMM_CFG_S *pCfg);
int nvr_logon_set_comm_cfg(int port, NVR_DEV_COMM_CFG_S *pCfg);

int nvr_logon_get_ddns_cfg(int port, NVR_DDNS_CFG_S *pCfg);
int nvr_logon_set_ddns_cfg(int port, NVR_DDNS_CFG_S *pCfg);

int nvr_logon_get_channel_cfg(int port, NVR_CHANNEL_CFG_S *pCfg);
int nvr_logon_set_channel_cfg(int port, NVR_CHANNEL_CFG_S *pCfg);

//int nvr_logon_get_time_cfg(int port, NVR_TIME_CFG_S *pCfg);
//int nvr_logon_set_time_cfg(int port, NVR_TIME_CFG_S *pCfg);
int nvr_logon_get_time_cfg(int port, NVR_TIME_S *pCfg);
int nvr_logon_set_time_cfg(int port, NVR_TIME_S *pCfg);

int nvr_logon_get_auto_cfg(int port, NVR_AUTO_MAINTAIN_CFG_S *pCfg);
int nvr_logon_set_auto_cfg(int port, NVR_AUTO_MAINTAIN_CFG_S *pCfg);

int nvr_logon_get_jxj_platform_cfg(int port, NVR_DEV_JXJ_PLATFORM_CFG_S *pCfg);
int nvr_logon_set_jxj_platform_cfg(int port, NVR_DEV_JXJ_PLATFORM_CFG_S *pCfg);

int nvr_logon_get_dianxin_platform_cfg(int port, NVR_DEV_DIANXIN_PLATFORM_CFG_S *pCfg);
int nvr_logon_set_dianxin_platform_cfg(int port, NVR_DEV_DIANXIN_PLATFORM_CFG_S *pCfg);

int nvr_logon_get_main_venc_cfg(int port, NVR_DEV_VENC_CFG_S *pCfg);
int nvr_logon_set_main_venc_cfg(int port, NVR_DEV_VENC_CFG_S *pCfg);

int nvr_logon_get_rec_cfg(int port, NVR_DEV_REC_STRC_S *pCfg);
int nvr_logon_set_rec_cfg(int port, NVR_DEV_REC_STRC_S *pCfg);

int nvr_logon_get_stable_cfg(int port, NVR_DEV_STABLE_INFO_S *pCfg);
int nvr_logon_set_stable_cfg(int port, NVR_DEV_STABLE_INFO_S *pCfg);

int nvr_logon_get_di_cfg(int port, NVR_DEV_DI_CFG_S *pCfg);
int nvr_logon_set_di_cfg(int port, NVR_DEV_DI_CFG_S *pCfg);

int nvr_logon_get_ipnc_alarm_cfg(int port, NVR_DEV_IPNC_ALARM_CFG_S *pCfg);
int nvr_logon_set_ipnc_alarm_cfg(int port, NVR_DEV_IPNC_ALARM_CFG_S *pCfg);

int nvr_logon_get_auto_ctrl_cfg(int port, NVR_DEV_AUTO_CTRL_CFG_S *pCfg);
int nvr_logon_set_auto_ctrl_cfg(int port, NVR_DEV_AUTO_CTRL_CFG_S *pCfg);

int nvr_logon_get_osd_cfg(int port, NVR_DEV_OSD_CFG_S *pCfg);
int nvr_logon_set_osd_cfg(int port, NVR_DEV_OSD_CFG_S *pCfg);


////////////////////////////////////////////////////////////////////////////
/*发送云控制台控制命令*/
int nvr_lib_send_ptz_cmd(uint8_t u8Chn, void* pPtz);

/* 设置位置方向 */
int Nvr_Client_Crtl_Ptz(uint16_t u16PtzCmd, uint8_t u8Chn, uint8_t u8Speed, uint8_t u8PresetNo);

/* 设置自动巡航 */
int Nvr_client_Ctrl_Ptz_Cruise(uint8_t u8Chn, uint8_t u8CruisePresetNo, uint8_t u8CruisePresetEnable, 
								uint8_t u8CruisePresetSec, uint8_t u8CruisePresetSpeed);
/* 设置自动扫描 */
int Nvr_Client_Ctrl_Ptz_AutoScan(uint8_t u8Chn, uint8_t u8SetVerticalPos, uint8_t u8Speed);

/* 设置AB两点扫描 */
int Nvr_Client_Ctrl_Ptz_ABScan(uint8_t u8Chn, uint8_t u8SetVerticalPos, uint8_t u8Speed);
int Nvr_Client_Ctrl_Ptz_ABScan_Left_Pos(uint8_t u8Chn);
int Nvr_Client_Ctrl_Ptz_ABScan_Right_Pos(uint8_t u8Chn);

/* 设置闲置动作 */
int Nvr_Client_Ctrl_Ptz_SetIdle(uint8_t u8Chn, uint8_t u8IdleActionIndex, uint8_t u8IdleSec);

/* 设置定时动作 */
int Nvr_Client_Ctrl_Ptz_SetOntime(uint8_t u8Chn, uint8_t u8OntimeIndex, uint8_t u8OntimeActionIndex,
								uint8_t u8BeginHour, uint8_t u8BeginMin, uint8_t u8EndHour, uint8_t u8EndMin);

/* 设置报警动作 */
int Nvr_Client_Ctrl_Ptz_SetAlarm(uint8_t u8Chn, uint8_t u8AlarmTypeIndex, uint8_t u8AlarmActionIndex,
									uint8_t u8AlarmDurationSec);
/* 红外灯远近开关*/ 
/*u8IRMode: 0:远近灯自动切换,1:远近灯同时开启 */
int Nvr_Client_Ctrl_Ptz_SetIRMode(uint8_t u8Chn, uint8_t u8IRMode);

/*  设置日夜模式 */
/* 0:白天模式,1:夜晚模式,2:自动模式 3:红外定时开启模式 */
int Nvr_Client_Ctrl_Ptz_SetDayNightMode(uint8_t u8Chn, uint8_t u8DayNightMode);

/*  设置变倍速度 */
/*  0 快速, 1 中速, 2 低速*/
int Nvr_Client_Ctrl_Ptz_SetZoomSpeed(uint8_t u8Chn, uint8_t u8ZoomSpeed);

/*  设置白平衡工作模式 */
/* 0 室外,1 室内 ,2 自动 ,3 手动 */
int Nvr_Client_Ctrl_Ptz_SetWBMode(uint8_t u8Chn, uint8_t u8WBMode);

/* 设置某些选项的使能
* 包括:
* 	180度自动翻转
*	电子放大
*	背光补偿
*	变倍显示
*	帧降噪
*	宽动态
*	移动侦测
*	数字慢快门
*/
/*	0:close, 1:open */
int Nvr_Client_Ctrl_Ptz_SetEnable(uint8_t u8Chn, uint16_t u8PtzCmd, uint8_t u8Enable);





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif


