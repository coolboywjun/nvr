
#include "jxj_cu_logon.h"
#include "jxj_cu_param.h"


#define DECLARE_JXJ_CU_GET_CFG(CfgName, CmdVal, StructType)					\
int jxj_cu_##CfgName##_get(long logonHandle, StructType *pCfg)				\
{																			\
	int len = sizeof(StructType);											\
	if(0 > jxj_cu_cfg_get(logonHandle, CmdVal, 								\
							  pCfg, len))									\
		return -1;															\
	return 0;																\
}

#define DECLARE_JXJ_CU_SET_CFG(CfgName, CmdVal, StructType)					\
int jxj_cu_##CfgName##_set(long logonHandle, StructType *pCfg)				\
{																			\
	int len = sizeof(StructType);											\
	if(0 > jxj_cu_cfg_set(logonHandle, CmdVal, 								\
							  pCfg, len))									\
		return -1;															\
	return 0;																\
}

DECLARE_JXJ_CU_GET_CFG(ntp, PARAM_DEVICE_NTP_INFO, JDeviceNTPInfo);
DECLARE_JXJ_CU_SET_CFG(ntp, PARAM_DEVICE_NTP_INFO, JDeviceNTPInfo);

DECLARE_JXJ_CU_GET_CFG(media, PARAM_MEDIAL_URL, JMediaUrl);
DECLARE_JXJ_CU_SET_CFG(media, PARAM_MEDIAL_URL, JMediaUrl);

DECLARE_JXJ_CU_GET_CFG(encode, PARAM_ENCODE_INFO, JEncodeParameter);
DECLARE_JXJ_CU_SET_CFG(encode, PARAM_ENCODE_INFO, JEncodeParameter);

DECLARE_JXJ_CU_GET_CFG(time, PARAM_DEVICE_TIME, JDeviceTime);
DECLARE_JXJ_CU_SET_CFG(time, PARAM_DEVICE_TIME, JDeviceTime);

DECLARE_JXJ_CU_GET_CFG(jxj_platform, PARAM_PLATFORM_INFO, JPlatformInfo);
DECLARE_JXJ_CU_SET_CFG(jxj_platform, PARAM_PLATFORM_INFO, JPlatformInfo);

DECLARE_JXJ_CU_GET_CFG(net, PARAM_NETWORK_INFO, JNetworkInfo);
DECLARE_JXJ_CU_SET_CFG(net, PARAM_NETWORK_INFO, JNetworkInfo);

DECLARE_JXJ_CU_GET_CFG(dev, PARAM_DEVICE_INFO, JDeviceInfo);
DECLARE_JXJ_CU_SET_CFG(dev, PARAM_DEVICE_INFO, JDeviceInfo);

DECLARE_JXJ_CU_GET_CFG(disp, PARAM_DISPLAY_INFO, JDisplayParameter);
DECLARE_JXJ_CU_SET_CFG(disp, PARAM_DISPLAY_INFO, JDisplayParameter);
// default display param
DECLARE_JXJ_CU_GET_CFG(disp_def, PARAM_DEF_DISPLAY_INFO, JDisplayParameter);

DECLARE_JXJ_CU_GET_CFG(pppoe, PARAM_PPPOE_INFOE, JPPPOEInfo);
DECLARE_JXJ_CU_SET_CFG(pppoe, PARAM_PPPOE_INFOE, JPPPOEInfo);

DECLARE_JXJ_CU_GET_CFG(rec, PARAM_RECORD_INFO, JRecordParameter);
DECLARE_JXJ_CU_SET_CFG(rec, PARAM_RECORD_INFO, JRecordParameter);

DECLARE_JXJ_CU_GET_CFG(vmask, PARAM_HIDE_INFO, JHideParameter);
DECLARE_JXJ_CU_SET_CFG(vmask, PARAM_HIDE_INFO, JHideParameter);

DECLARE_JXJ_CU_GET_CFG(user, PARAM_USER_LOGIN, JUserInfo);
DECLARE_JXJ_CU_SET_CFG(user, PARAM_USER_LOGIN, JUserInfo);

DECLARE_JXJ_CU_GET_CFG(comm, PARAM_SERIAL_INFO, JSerialParameter);
DECLARE_JXJ_CU_SET_CFG(comm, PARAM_SERIAL_INFO, JSerialParameter);

DECLARE_JXJ_CU_GET_CFG(ptz_cfg, PARAM_PTZ_INFO, JPTZParameter);
DECLARE_JXJ_CU_SET_CFG(ptz_cfg, PARAM_PTZ_INFO, JPTZParameter);

DECLARE_JXJ_CU_GET_CFG(ftp, PARAM_FTP_INFO, JFTPParameter);
DECLARE_JXJ_CU_SET_CFG(ftp, PARAM_FTP_INFO, JFTPParameter);

DECLARE_JXJ_CU_GET_CFG(smtp, PARAM_SMTP_INFO, JSMTPParameter);
DECLARE_JXJ_CU_SET_CFG(smtp, PARAM_SMTP_INFO, JSMTPParameter);

DECLARE_JXJ_CU_GET_CFG(upnp, PARAM_UPNP_INFO, JUPNPParameter);
DECLARE_JXJ_CU_SET_CFG(upnp, PARAM_UPNP_INFO, JUPNPParameter);

DECLARE_JXJ_CU_GET_CFG(ddns, PARAM_DDNS_CODING, JDdnsConfig);
DECLARE_JXJ_CU_SET_CFG(ddns, PARAM_DDNS_CODING, JDdnsConfig);

DECLARE_JXJ_CU_GET_CFG(osd, PARAM_OSD_INFO, JOSDParameter);
DECLARE_JXJ_CU_SET_CFG(osd, PARAM_OSD_INFO, JOSDParameter);

DECLARE_JXJ_CU_GET_CFG(ja, PARAM_JOINT_INFO, JJointAction);
DECLARE_JXJ_CU_SET_CFG(ja, PARAM_JOINT_INFO, JJointAction);

DECLARE_JXJ_CU_GET_CFG(md, PARAM_MOVE_ALARM, JMoveAlarm);
DECLARE_JXJ_CU_SET_CFG(md, PARAM_MOVE_ALARM, JMoveAlarm);

DECLARE_JXJ_CU_GET_CFG(di, PARAM_IO_ALARM, JIoAlarm);
DECLARE_JXJ_CU_SET_CFG(di, PARAM_IO_ALARM, JIoAlarm);

DECLARE_JXJ_CU_GET_CFG(pic, PARAM_PIC_INFO, JPictureInfo);
DECLARE_JXJ_CU_SET_CFG(pic, PARAM_PIC_INFO, JPictureInfo);

DECLARE_JXJ_CU_GET_CFG(pic_def, PARAM_DEF_PICTURE_INFO, JPictureInfo);


DECLARE_JXJ_CU_GET_CFG(3d, PARAM_3D_CONTROL, J3DControl);
DECLARE_JXJ_CU_SET_CFG(3d, PARAM_3D_CONTROL, J3DControl);

DECLARE_JXJ_CU_GET_CFG(ir, PARAM_IRCUT_CONTROL, JIrcutControl);
DECLARE_JXJ_CU_SET_CFG(ir, PARAM_IRCUT_CONTROL, JIrcutControl);

DECLARE_JXJ_CU_GET_CFG(pp, PARAM_IRCUT_CONTROL, JPPSet);
DECLARE_JXJ_CU_SET_CFG(pp, PARAM_IRCUT_CONTROL, JPPSet);

DECLARE_JXJ_CU_GET_CFG(cpc, PARAM_CRUISE_POINT_CFG, JCruiseConfig);
DECLARE_JXJ_CU_SET_CFG(cpc, PARAM_CRUISE_POINT_CFG, JCruiseConfig);

DECLARE_JXJ_CU_GET_CFG(cwc, PARAM_CRUISE_WAY_CFG, JCruiseWay);
DECLARE_JXJ_CU_SET_CFG(cwc, PARAM_CRUISE_WAY_CFG, JCruiseWay);

DECLARE_JXJ_CU_SET_CFG(reboot, PARAM_CONTROL_DEV, JControlDevice);
DECLARE_JXJ_CU_SET_CFG(halt, PARAM_CONTROL_DEV, JControlDevice);
DECLARE_JXJ_CU_SET_CFG(default, PARAM_CONTROL_DEV, JControlDevice);






