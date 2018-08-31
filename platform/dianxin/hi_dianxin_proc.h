/////////////////////////////////////////////////////////////
//////////////////// hi_dianxin_proc.h //////////////////////
/////////////////////////////////////////////////////////////

#ifndef __HI_DIANXIN_PROC_H__
#define __HI_DIANXIN_PROC_H__

#include "hi_platform_dianxin.h"

#define MANUFACTURER_JXJ  "850"		//厂商编号


typedef enum _DIANXIN_DEVTYPE_E_
{
	DIANXIN_IPCAM 	= 1,
	DIANXIN_DVR		= 2,
	DIANXIN_NVR		= 3,
}DIANXIN_DEVTYPE;


typedef enum _DIANXIN_CMD_TYPE_E_
{
	CMD_LOGIN				= 0x31,			//登陆认证请求/确认
	CMD_ABILITYUP			= 0x32,			//能力上报请求/确认
	CMD_HEARTBEAT			= 0x33,			//链路检测请求/确认
	CMD_EXCEPTION_NOTIFY	= 0x34,			//异常数据上报/确认
	CMD_VIDEO_PLAY			= 0x35,			//视频监控Token验证请求/确认
	CMD_NOTIFY_PICTURE		= 0x36,			//FTP图片抓拍通知 
	CMD_ALARM_NOTIFY		= 0x37,			//设备告警数据上报
	
	CMD_SET_CHANNEL			= 0x60,			//视频参数设置请求/确认
	CMD_GET_CHANNEL			= 0x61, 		//视频参数查询请求/确认 
	CMD_GET_WIFI			= 0x62,			//无线网络查询/确认	
	CMD_SET_WIFI			= 0x63,			//无线网络查询/确认
	CMD_VIDEO_PTZ			= 0x64,			//云台控制请求/确认
	CMD_SET_ESTORE			= 0x65,			//天翼云存储帐号设置请求/确认
	CMD_GET_ESTORE			= 0x66, 		//天翼云存储帐号查询请求/确认 
	CMD_ADD_RECORD_POLICY	= 0x67,			//存储录像策略添加/确认 
	CMD_QUERY_RECORD_POLICY	= 0x68, 		//存储录像策略查询/确认 
	CMD_DEL_RECORD_POLICY	= 0x69,			//存储录像策略删除/确认 
	CMD_REBOOT				= 0x6c,			//摄象机远程重启
	CMD_UPGRADE				= 0x6d,			//远程升级通知 
	CMD_NVR_BIND_CAMERA		= 0x6e,			//NVR 绑定网络摄像机
	CMD_NVR_MODIFY_CAMERA	= 0x6f,			//NVR 修改网络摄像机绑定信息
	CMD_NVR_REMOVE_CAMERA	= 0x70,			//NVR 解除网络摄像机绑定
	CMD_NVR_QUERY_CAMERA	= 0x71,			//NVR 查询网络摄像机绑定
	CMD_FILE_QUERY			= 0x72,			//视频录像检索
	CMD_RECORD_PLAY			= 0x74,			//视频录像回放验证
	CMD_ADD_PICTURE_POLICY	= 0x75,			//存储图片策略添加 
	CMD_QUERY_PICTURE_POLICY= 0x76,			//存储图片策略查询 
	CMD_DEL_PICTURE_POLICY	= 0x77,			//存储图片策略删除 
	CMD_SET_RECORD_FULL		= 0x78,   		//一键全天录像设置
	CMD_GET_RECORD_FULL		= 0x79, 		//一键全天录像查询 
	CMD_SET_RECORD_LOOP		= 0x7a,			//循环录像标识设置 
	CMD_GET_RECORD_LOOP		= 0x7b,			//循环录像标识查询 
	CMD_SET_ALARM			= 0x7c,			//设备报警开关设置
	CMD_GET_ALARM			= 0x7d,			//设备报警开关查询
	CMD_LOG_UPDATE			= 0x7e,			//设备日志上传通知 
	CMD_QUERY_STORAGE		= 0x7f,			//本地存储状态查询
	CMD_STORAGE_FORMAT		= 0x80,			//本地存储格式化
	CMD_SET_FORMAT			= 0x91,			//一键格式化
}DIANXIN_CMD_TYPE;

typedef enum _DIANXIN_ALARM_TYPE_E
{
	ALARM_MD			= 1,	//移动侦测
	ALARM_BODY_IRC 		= 2,	//人体红外
	ALARM_MANUAL		= 3, 	//手动报警
	ALARM_DI			= 4,	//开关量报警
}DIANXIN_ALARM_TYPE;

typedef struct _DINAXIN_FILE_QUERY_S_
{
	int chn;
	int nStartTime;
	int nEndTime;
	int PageNo;
}DIANXIN_FILE_QUERY;

#define MXMLGETTEXT(node, whitespace) ((mxmlGetText(node, whitespace) == NULL)? (""):(mxmlGetText(node, whitespace)))


int hi_dianxin_group_register(DIANXIN_CTRL *pDianxinCtrl);

int hi_dianxin_lcms_register(DIANXIN_CTRL *pDianxinCtrl);

int hi_dianxin_login_req(DIANXIN_CTRL *pDianxinCtrl);

int hi_dianxin_abilityup_req(DIANXIN_CTRL *pDianxinCtrl);

int hi_dianxin_heart_beat_req(DIANXIN_CTRL *pDianxinCtrl);

int hi_dianxin_exception_notify_proc(DIANXIN_CTRL *pDianxinCtrl);

//int hi_dianxin_alarm_notify_req(DIANXIN_CTRL *pDianxinCtrl, LPHI_ALARM_INFO_S pAlarmInfo);

int hi_dianxin_recv(DIANXIN_CTRL *pDianxinCtrl);

int hi_dianxin_video_play_req(DIANXIN_VIDEO_PLAY *pVideoPlay);

int hi_dianxin_record_play_req(DIANXIN_VOD_PLAY *pVodPlay);







#endif
