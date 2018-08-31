
#ifndef __JXJ_CU_PARAM_H__
#define __JXJ_CU_PARAM_H__

#include "j_sdk.h"

int jxj_cu_ntp_get(long logonHandle, JDeviceNTPInfo* pCfg);
int jxj_cu_ntp_set(long logonHandle, JDeviceNTPInfo* pCfg);

int jxj_cu_media_get(long logonHandle, JMediaUrl* pCfg);
int jxj_cu_media_set(long logonHandle, JMediaUrl* pCfg);

int jxj_cu_encode_get(long logonHandle, JEncodeParameter* pCfg);
int jxj_cu_encode_set(long logonHandle, JEncodeParameter* pCfg);

int jxj_cu_time_get(long logonHandle, JDeviceTime* pCfg);
int jxj_cu_time_set(long logonHandle, JDeviceTime* pCfg);

int jxj_cu_jxj_platform_get(long logonHandle, JPlatformInfo* pCfg);
int jxj_cu_jxj_platform_set(long logonHandle, JPlatformInfo* pCfg);

int jxj_cu_net_get(long logonHandle, JNetworkInfo* pCfg);
int jxj_cu_net_set(long logonHandle, JNetworkInfo* pCfg);

int jxj_cu_dev_get(long logonHandle, JDeviceInfo* pCfg);
int jxj_cu_dev_set(long logonHandle, JDeviceInfo* pCfg);

int jxj_cu_disp_get(long logonHandle, JDisplayParameter* pCfg);
int jxj_cu_disp_set(long logonHandle, JDisplayParameter* pCfg);
int jxj_cu_disp_def_get(long logonHandle, JDisplayParameter* pCfg);

int jxj_cu_pppoe_get(long logonHandle, JPPPOEInfo* pCfg);
int jxj_cu_pppoe_set(long logonHandle, JPPPOEInfo* pCfg);

int jxj_cu_rec_get(long logonHandle, JRecordParameter* pCfg);
int jxj_cu_rec_set(long logonHandle, JRecordParameter* pCfg);

int jxj_cu_vmask_get(long logonHandle, JHideParameter* pCfg);
int jxj_cu_vmask_set(long logonHandle, JHideParameter* pCfg);

int jxj_cu_user_get(long logonHandle, JUserInfo* pCfg);
int jxj_cu_user_set(long logonHandle, JUserInfo* pCfg);

int jxj_cu_comm_get(long logonHandle, JSerialParameter* pCfg);
int jxj_cu_comm_set(long logonHandle, JSerialParameter* pCfg);

int jxj_cu_ptz_cfg_get(long logonHandle, JPTZParameter* pCfg);
int jxj_cu_ptz_cfg_set(long logonHandle, JPTZParameter* pCfg);

int jxj_cu_ftp_get(long logonHandle, JFTPParameter* pCfg);
int jxj_cu_ftp_set(long logonHandle, JFTPParameter* pCfg);

int jxj_cu_smtp_get(long logonHandle, JSMTPParameter* pCfg);
int jxj_cu_smtp_set(long logonHandle, JSMTPParameter* pCfg);

int jxj_cu_upnp_get(long logonHandle, JUPNPParameter* pCfg);
int jxj_cu_upnp_set(long logonHandle, JUPNPParameter* pCfg);

int jxj_cu_ddns_get(long logonHandle, JDdnsConfig* pCfg);
int jxj_cu_ddns_set(long logonHandle, JDdnsConfig* pCfg);

int jxj_cu_osd_get(long logonHandle, JOSDParameter* pCfg);
int jxj_cu_osd_set(long logonHandle, JOSDParameter* pCfg);

int jxj_cu_md_get(long logonHandle, JMoveAlarm* pCfg);
int jxj_cu_md_set(long logonHandle, JMoveAlarm* pCfg);

int jxj_cu_di_get(long logonHandle, JIoAlarm* pCfg);
int jxj_cu_di_set(long logonHandle, JIoAlarm* pCfg);

int jxj_cu_pic_get(long logonHandle, JPictureInfo* pCfg);
int jxj_cu_pic_set(long logonHandle, JPictureInfo* pCfg);
int jxj_cu_pic_def_get(long logonHandle, JPictureInfo* pCfg);

int jxj_cu_3d_get(long logonHandle, J3DControl* pCfg);
int jxj_cu_3d_set(long logonHandle, J3DControl* pCfg);

int jxj_cu_ir_get(long logonHandle, JIrcutControl* pCfg);
int jxj_cu_ir_set(long logonHandle, JIrcutControl* pCfg);

int jxj_cu_pp_get(long logonHandle, JPPSet* pCfg);
int jxj_cu_pp_set(long logonHandle, JPPSet* pCfg);

int jxj_cu_cpc_get(long logonHandle, JCruiseConfig* pCfg);
int jxj_cu_cpc_set(long logonHandle, JCruiseConfig* pCfg);

int jxj_cu_cwc_get(long logonHandle, JCruiseWay* pCfg);
int jxj_cu_cwc_set(long logonHandle, JCruiseWay* pCfg);

int jxj_cu_ja_get(long logonHandle, JJointAction* pCfg);
int jxj_cu_ja_set(long logonHandle, JJointAction* pCfg);

int jxj_cu_reboot_set(long logonHandle, JControlDevice* pCfg);
int jxj_cu_halt_set(long logonHandle, JControlDevice* pCfg);
int jxj_cu_default_set(long logonHandle, JControlDevice* pCfg);


#endif

