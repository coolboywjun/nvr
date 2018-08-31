
#ifndef __J_SDK_H__
#define __J_SDK_H__

#include <stdio.h>
#include "stdint.h"

#define J_SDK_MAX_ID_LEN					32
#define J_SDK_MAX_IP_LEN					64
#define J_SDK_MAX_HOST_LEN					128

#define J_SDK_MAX_MANU_INFO_LEN				128
#define J_SDK_MAX_DATA_LEN					32
#define J_SDK_MAX_VERSION_LEN				12

#define J_SDK_MAX_NETMASK_LEN				32
#define J_SDK_MAX_GATEWAY_LEN				32
#define J_SDK_MAX_MAC_LEN					32
#define J_SDK_MAX_DNS_LEN					32

#define J_SDK_MAX_ACCOUNT_LEN				32
#define J_SDK_MAX_PASSWD_LEN				32
#define J_SDK_MAX_PATH_LEN					128
#define J_SDK_MAX_ADDR_LEN					64

#define J_SDK_MAX_ALARM_DATA_LEN			128

#define J_SDK_MAX_COMMAND_LEN				64
#define J_SDK_MAX_PAGE_SIZE					1024

#define J_SDK_MAX_SEG_SZIE					4
#define J_SDK_MAX_TIME_LEN					32

#define J_SDK_MAX_DISK_NUMBER				16
#define J_SDK_MAX_DECTION_AREA_SIEZ			16
#define J_SDK_MAX_URL_LEN					128

#define J_SDK_MAX_NAME_LEN					64
#define J_SDK_MAX_PWD_LEN					64

#define J_SDK_MAX_TEXT_DATA_LEN				128
#define J_SDK_MAX_UPGRADE_DATA_LEN			200

#define J_SDK_MAX_STORE_LOG_SIZE  			24
#define J_SDK_MAX_STREAM_SIZE				4
#define J_SDK_MAX_WIFI_AP_SIZE				16

#define J_MAX_CHANNEL_INFO_SIZE				32
#define J_FILE_PROPERTY_SIZE				64

#define MAX_IVS_AVD_RULES 					5

#define MAX_NET_DISK_NAME_LEN				256


#define J_SDK_MAX_CHN_SIZE  				32
#define J_SDK_MAX_ALARMIN_SIZE 				32
#define J_SDK_MAX_ALARMOUT_SIZE 			32

#define J_SDK_MAX_PRESET_PORT_SIZE 			255
#define J_SDK_MAX_CRUISE_PORT_SIZE 			32
#define J_SDK_MAX_CRUISE_WAY_SIZE 			16

#define J_SDK_DEF_BASE_YEAR                 1900

#ifndef _WIN32
# define DEF_PRAGMA_PACKED					__attribute__((packed))
#else
# define DEF_PRAGMA_PACKED					
#endif

#pragma pack(push, 1)

//************************* enmu *******************************
typedef enum __PuType
{
    J_SDK_DVR=0,
    J_SDK_DVS,
    J_SDK_IPC,
    J_SDK_IPNC,
    J_SDK_SDEC,                         //软解码器
    J_SDK_DEC,                          //硬解码器
    J_SDK_NVR,
    J_SDK_HVR,
    
    J_SDK_OTHER_TYPE = 255
}JPuType;

typedef enum __IPCFactoryType
{
	JXJ_IPC=0,
	HIK_IPC,
	DAH_IPC
}JIPCFactoryType;

typedef enum __PuSubType
{
	J_SDK_RIFLE=0,						//枪机
	J_SDK_INFRARED,						//红外
	J_SDK_HEMISPHERE					//半球
}JPuSubType;

typedef enum __ProtocolType
{
	J_SDK_TCP=0,
	J_SDK_UDP
}JProtocolType;


typedef enum __NetworkType
{
	J_SDK_ETH0=0,
	J_SDK_WIFI,
	J_SDK_3G,
	J_SDK_MAX_NETWORK_TYPE
}JNetworkType;

typedef enum __PublicDomain
{
	J_SDK_OPTIONS=0,
	J_SDK_TEARDOWN
}JPublicDomain;

typedef enum __MediaType
{
	J_SDK_R_TIME_AV_S=0,				//实时音视频流
	J_SDK_R_TIME_CAPTURE_I,				//实时抓拍图片
	J_SDK_HISTORY_AV_DEMAND,			//历史音视频（点播）
	J_SDK_HISTORY_AV_DOWNLOAD,			//历史音视频（下载）
	J_SDK_HISTORY_IMAGE					//历史图片
}JMediaType;

typedef enum __VideoFormat
{
	J_SDK_PAL=0,
	J_SDK_NTSC
}JVideoFormat;

typedef enum __BitRate
{
	J_SDK_ABR=0,
	J_SDK_CBR,
	J_SDK_VBR,
	J_SDK_FIXQP,
	J_SDK_BUTT
}JBitRate;

typedef enum __DiskType
{
	J_SDK_SATA=0,
	J_SDK_USB,
	J_SDK_ISCSI,
	J_SDK_NFS,
	J_SDK_SD
}JDiskType;

typedef enum __DiskStatus
{
	J_SDK_USING=0,
	J_SDK_MOUNTED,
	J_SDK_UNMOUNT
}JDiskStatus;

typedef enum __WeekDay
{
	J_SDK_SUNDAY=0,
	J_SDK_MONDAY,
	J_SDK_TUESDAY,
	J_SDK_WEDNESDAY,
	J_SDK_THURSDAY,
	J_SDK_FRIDAY,
	J_SDK_SATURDAY,
	J_SDK_EVERYDAY,
	J_SDK_MAX_DAY_SIZE
}JWeekDay;

typedef enum __SysFileType
{
	J_SDK_FAT16=0,
	J_SDK_FAT32,
	J_SDK_NTFS,
	J_SDK_EXT,
	J_SDK_EXT2,
	J_SDK_EXT3
}JSysFileType;

typedef enum __AlarmType
{
	J_SDK_MOVE_ALARM=0,
	J_SDK_LOST_ALARM,
	J_SDK_HIDE_ALARM,
	J_SDK_IO_ALARM,
	J_SDK_AVD_ALARM,				    // 智能分析视频诊断报警	

	//+++++++++++++IPC++++++++++++//
    J_SDK_NETWORK_FAULT_ALARM,          // ipc 网络故障报警
	J_SDK_IOT_ANALYSE_ALARM = 100,      // 中科院物联网云智能分析报警
    J_SDK_PEA_OSC_ALARM = 101,	        //周界、绊线、物品看护等报警
}JAlarmType;

typedef enum __NetCardAlarm
{
    NETWORK_DISCONNECT = 0x2,
    NETWORK_RECONNECT  = 0x4,
}JNetCardAlarm;

typedef enum __AnalyseAlarm
{
    SUBMIT_ANALYSE_DATA = 0x2,
}JAnalyseAlarm;

typedef enum __OSCAlarm
{
    UNKNOW_EVENT = 0x1,
    SIMPLE_LINE  = 0x2,
    DOUBLE_LINE  = 0x4,
    ALERT_AREA_ALARM = 0x8,
    HOVER            = 0x10,
    HAND_DOWN        = 0x20,
    LOSING_SOMETHING = 0x40,
}JOSCAlarm;

typedef enum __IOType
{
	NORMAL_CLOSE=0,
	NORMAL_OPEN
}JIOType;

typedef enum __Action
{
	ACTION_UNPACK,
	ACTION_PACK,
}JAction;

//分辨率
typedef enum __Resolution
{
	J_SDK_VIDEO_QCIF		= 0x01,		//320*192
	J_SDK_VIDEO_CIF			= 0x02,		//352*288	n制352*240
	J_SDK_VIDEO_HD1			= 0x03,		//704*288	n制704*240
	J_SDK_VIDEO_D1			= 0x04,		//704*576	n制704*480
	
	J_SDK_VIDEO_QQVGA	    = 0x20,		//160*112
	J_SDK_VIDEO_QVGA		= 0x21,		//320*240
	J_SDK_VIDEO_VGA			= 0x22,		//640*480
	J_SDK_VIDEO_SVGA		= 0x23,		//800*600
	J_SDK_VIDEO_UXGA		= 0x24,		//1600*1200

	J_SDK_VIDEO_720P		= 0x40,		//1280*720
	J_SDK_VIDEO_960P		= 0x41,		//1280*960
	J_SDK_VIDEO_1080P		= 0x42,		//1920*1080
}JResolution;

//视频、音频编码类型
typedef enum __AVCodeType
{
	J_SDK_AV_CODEC_UNKNOWN = 0,

	J_SDK_AV_VIDEO_CODEC_START = 10,
	J_SDK_AV_VIDEO_H264,
	J_SDK_AV_VIDEO_MPEG4,
	J_SDK_AV_VIDEO_MJPEG,
	J_SDK_AV_VIDEO_CODEC_END,

	J_SDK_AV_AUDIO_CODEC_START = 20,
	J_SDK_AV_AUDIO_G711A,
	J_SDK_AV_AUDIO_G711U,
	J_SDK_AV_AUDIO_CODEC_END
}JAVCodeType;

typedef enum __AudioInputType
{
	J_SDK_AUDIO_MIC_IN=0,
	J_SDK_AUDIO_LINE_IN
}JAudioInputType;


typedef enum __EncodeLevel
{
	J_SDK_BASELINE,
	J_SDK_MAIN,
	J_SDK_HIGH
}JEncodeLevel;

typedef enum __RecodeType
{
	TIMER_RECODE  = 0x00000001,
	ALARM_RECODE  = 0x00000010,
	MOVE_RECODE   = 0x00000100,
	MANUAL_RECODE = 0x00001000,
	LOST_RECODE   = 0x00010000,
	HIDE_RECODE   = 0x00100000,
	//+++++++++++IPC+++++++++++++++//
    NET_FAULT_RECODE = 0x00200000,
	ALL_RECODE    = 0xFFFFFFFF,
}JRecodeType;

typedef enum __PTZAction
{
	PTZ_STOP        = 0,			//停止
	PTZ_AUTO        = 1,			//自动
	
	PTZ_LEFT        = 2,			//左
	PTZ_RIGHT       = 3,			//右
	PTZ_UP          = 4,			//上
	PTZ_DOWN        = 5,			//下

	PTZ_LEFT_UP     = 6,			//左上
	PTZ_LEFT_DOWN   = 7,			//左下
	PTZ_RIGHT_UP    = 8,			//右上
	PTZ_RIGHT_DOWN  = 9,			//右下
	
	PTZ_SET_PP      = 10,			//设置预置点
	PTZ_USE_PP      = 11,			//调用预置点
	PTZ_DEL_PP      = 12,			//删除预置点
	
	PTZ_ADD_ZOOM    = 13,			//焦距放大
	PTZ_SUB_ZOOM    = 14,			//焦距缩小
	PTZ_ADD_FOCUS   = 15,			//远焦点
	PTZ_SUB_FOCUS   = 16,			//近焦点
	
	PTZ_TURN_ON     = 17,			//开启灯光
	PTZ_TURN_OFF    = 18,			//关闭灯光
	
	PTZ_WIPERS_ON   = 19,			//开启雨刷
	PTZ_WIPERS_OFF  = 20,			//关闭雨刷

	PTZ_SET_CRZ		= 21,			//设置巡航
	PTZ_START_CRZ	= 22,			//开始巡航
	PTZ_STOP_CRZ	= 23,			//停止巡航
	PTZ_DEL_CRZ		= 24,			//删除巡航
}JPTZAction;

typedef enum __AvdType 
{
    AVD_BRIGHT         = 0,       /*亮度异常*/
    AVD_CLARITY        = 1,       /*清晰度异常*/
    AVD_NOISE          = 2,       /*噪声异常*/
    AVD_COLOR          = 3,       /*偏色*/  
    AVD_SCENE          = 4,       /*场景变换*/
}JAvdType;
  
//++++++++++++++++++++ivs 智能分析视频诊断事件类型+++++++++++++++++++++++++++++++++++++++++++
typedef enum __AvdEventType /*avd event*/
{
    AVD_BRIGHT_ABMNL         = 0x2,       /*亮度异常报警*/
    AVD_BRIGHT_NORMAL        = 0x4,       /*亮度正常*/
    AVD_CLARITY_ABMNL        = 0x8,       /*清晰度异常报警*/
    AVD_CLARITY_NORMAL       = 0x10,      /*清晰度正常*/
    AVD_NOISE_ABMNL          = 0x20,      /*噪声异常报警*/
    AVD_NOISE_NORMAL         = 0x40,
    AVD_COLOR_ABMNL          = 0x80,      /*偏色异常报警*/
    AVD_COLOR_NORMAL         = 0x100,
    AVD_SCENE_ABMNL          = 0x200,     /*场景变换报警*/
    AVD_SCENE_NORMAL         = 0x400,
    AVD_EVENT_ALL            = 0xffffffff,
}JAvdEventType;



//*************************************************************

typedef struct __Time
{
	uint8_t year;					//从1900开始, J_SDK_DEF_BASE_YEAR
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint16_t weekday;				//详情见JWeekDay
}JTime;

typedef struct __TimeSeg
{
	uint32_t enable;
	JTime time_start;
	JTime time_end;
}JTimeSeg;
typedef struct __Day
{
	uint32_t count;
	uint32_t day_id;
	uint32_t all_day_enable;				//是否全天录像
	JTimeSeg seg[J_SDK_MAX_SEG_SZIE];
}JDay;
typedef struct __Week
{
	uint32_t count;
	JDay days[J_SDK_MAX_DAY_SIZE];
}JWeek;

typedef struct __Network
{
	uint32_t type;							//网络接口类型，详情见JNetworkType
	uint32_t dhcp_enable;					//是否启用DHCP
	uint8_t mac[J_SDK_MAX_MAC_LEN];			//MAC地址
	uint8_t ip[J_SDK_MAX_IP_LEN];			//设备ip
	uint8_t netmask[J_SDK_MAX_NETMASK_LEN];	//子网掩码
	uint8_t gateway[J_SDK_MAX_GATEWAY_LEN];	//网关
}JNetwork;

typedef struct __Rectangle
{
	int16_t left;
	int16_t top;
	int16_t right;
	int16_t bottom;
}JRectangle;

typedef struct __Area
{
	uint32_t count;
	JRectangle rect[J_SDK_MAX_DECTION_AREA_SIEZ];
}JArea;

typedef struct __DiskInfo
{
	uint32_t disk_no;						//磁盘号
	uint32_t total_size;					//磁盘容量
	uint32_t free_size;						//剩余容量
	uint32_t is_backup;						//是否备份盘
	uint32_t status;						//磁盘状态，详情见JDiskStatus
	uint32_t disk_type;						//磁盘类型，详情见JDiskType
	uint32_t sys_file_type;					//系统文件类型，详情见JSysFileType
}JDiskInfo;

typedef struct __Joint
{
	uint32_t joint_record_enable_channel;
	uint32_t joint_output_enable_channel;
	uint32_t joint_snap_enable_channel;
	uint32_t joint_record_second;
	uint32_t joint_beep_enable;
	uint32_t joint_beep_second;
	uint32_t joint_output_times;
	uint32_t joint_snap_interval;
	uint32_t joint_snap_times;
	uint32_t joint_email_enable;
}JJoint;


typedef struct __Store
{
	uint32_t  rec_type;
	uint32_t  file_size;
	JTime beg_time;
	JTime end_time;
	uint8_t property[J_FILE_PROPERTY_SIZE];
}Store;

typedef struct __Channel
{
	uint8_t pu_id[J_SDK_MAX_ID_LEN];
    uint8_t ip[J_SDK_MAX_IP_LEN];
    uint32_t port;
    uint32_t media;
    uint32_t level;
    uint32_t channel;
    uint32_t link_num;
    uint32_t protocol;
}Channel;

//#######################################################

typedef struct __ResultCode
{
	uint32_t code;								//0:成功，~0：失败
}JResultCode;


///////////////////////////////////////////////////////////////////////////////
typedef struct __DeviceInfo
{
	uint8_t manu_info[J_SDK_MAX_MANU_INFO_LEN];	//厂家信息
	uint8_t release_date[J_SDK_MAX_DATA_LEN];	//出厂日期
	uint8_t dev_version[J_SDK_MAX_VERSION_LEN];	//设备版本
	uint8_t hw_version[J_SDK_MAX_VERSION_LEN];	//硬件版本
	uint32_t pu_type;							//设备类型，详情见JPuType
	uint32_t sub_type;							//设备的机器类型，指设备的小类，详情见JPuSubType
	uint32_t di_num;							//设备输入接口数目
	uint32_t do_num;							//设备输出接口数目
	uint32_t channel_num;						//通道数目
	uint32_t rs485_num;							//RS485接口数目，若0，则无
	uint32_t rs232_num;							//RS232接口数目，若0，则无
}JDeviceInfo;

typedef struct __DeviceNTPInfo
{
	uint8_t ntp_server_ip[J_SDK_MAX_IP_LEN];	//NTP服务器
	uint32_t time_zone;							//时区
	uint32_t time_interval;						//对时间隔
	uint32_t ntp_enable;						//ntp开关；1：启动，0：不启动
	uint32_t dst_enable;						//夏令时；1：启动，0：不启动
	uint32_t reserve;							//保留字节
}JDeviceNTPInfo;

typedef struct __DeviceTime
{
	JTime time;									//时间
	uint32_t zone;								//时区
	uint32_t sync_enable;						//与平台服务器时间同步是否生效
}JDeviceTime;
//扩展: 将时间与cms 同步标记分离
typedef struct __DeviceTimeExt
{
	JTime time;									//时间
	uint32_t zone;								//时区
}JDeviceTimeExt;

typedef struct __PlatformInfo
{
	uint8_t pu_id[J_SDK_MAX_ID_LEN];
	uint8_t cms_ip[J_SDK_MAX_IP_LEN];			//平台cms ip
	uint8_t mds_ip[J_SDK_MAX_IP_LEN];			//平台mds ip
	uint32_t cms_port;							//平台cms端口号
	uint32_t mds_port;							//平台mds端口号
	uint32_t protocol;							//数据流的传送协议，详情见JProtocolType 
	uint32_t is_con_cms;						//是否连接平台，1-连接  0-不连接
}JPlatformInfo;

typedef struct __NetworkInfo
{
	JNetwork network[J_SDK_MAX_NETWORK_TYPE];	//网络接口信息
	uint8_t main_dns[J_SDK_MAX_DNS_LEN];		//主DNS服务器地址
	uint8_t backup_dns[J_SDK_MAX_DNS_LEN];		//备用DNS服务器地址
	uint32_t auto_dns_enable;					//是否启用自动获取dns
	uint32_t cur_network;						//eth0/wifi/3G
	uint32_t cmd_port;							//设备信令端口
	uint32_t data_port;							//设备数据端口
	uint32_t web_port;							//设备web端口
	uint32_t talk_port;                         //设备对讲端口
}JNetworkInfo;

typedef struct __PPPOEInfo
{
	uint32_t type;								//PPPOE拨号的网络接口，详情见JNetworkType
	uint32_t enable;							//
	uint8_t ip[J_SDK_MAX_IP_LEN];				//
	uint8_t account[J_SDK_MAX_ACCOUNT_LEN];		//PPPOE拨号账号
	uint8_t passwd[J_SDK_MAX_PASSWD_LEN];		//PPPOE拨号密码
}JPPPOEInfo;

typedef struct __EncodeParameter
{
	uint32_t level;				//码流类型：主(0)、从(1)、次(2)
	uint32_t frame_rate;		//帧率
	uint32_t i_frame_interval;	//I帧间隔
	uint32_t video_type;		//视频编码类型，详情见JAVCodeType
	uint32_t audio_type; 		//音频编码类型，详情见JAVCodeType
	uint32_t au_in_mode;		//音频输入类型，详情见JAudioInputType
	uint32_t audio_enble;		//音频是否打开
	uint32_t resolution;		//分辨率，详情见JResolution
	uint32_t qp_value;			//质量
	uint32_t code_rate;			//码率
	uint32_t frame_priority;	//是否帧率优先
	uint32_t format;			//制式，详情见JVideoFormat 
	uint32_t bit_rate;			//位率类型，详情见JBitRate
	uint32_t encode_level;		//编码类型，详情见JEncodeLevel
}JEncodeParameter;

typedef struct __DisplayParameter
{
	uint32_t contrast;			//对比度
	uint32_t bright;			//亮度
	uint32_t hue;				//灰度
	uint32_t saturation;		//饱和度
	uint32_t sharpness;			//锐度
}JDisplayParameter;

typedef struct __RecordParameter
{
	uint32_t level;				//码流类型：主(0)、从(1)、次(2)
	uint32_t pre_record;		//通道预录时长，单位：秒
	uint32_t auto_cover;		//通道录像是否自动覆盖

	JWeek week;
}JRecordParameter;

typedef struct __HideParameter
{
	uint32_t hide_enable;		//是否启用遮挡
	uint32_t hide_color;		//遮挡时使用的颜色
	JArea hide_area;			//遮挡区域,数目为0即遮挡无效
	uint32_t max_width;			//最大宽度
	uint32_t max_height;		//最大高度
}JHideParameter;

typedef struct __SerialParameter
{
	uint32_t serial_no;			//串口号
	uint32_t baud_rate;			//波特率
	uint32_t data_bit;			//数据位 
	uint32_t stop_bit;			//停止位
	uint32_t verify;			//校验
}JSerialParameter;

typedef struct __OSDExtend
{
	uint32_t ext_text1_enable;					//是否显示扩展文本1；1：显示，0：隐藏	
	uint8_t  ext_text1_data[J_SDK_MAX_TEXT_DATA_LEN];//扩展文本1
	uint32_t ext_text1_display_x;				//扩展文本1坐标X
	uint32_t ext_text1_display_y;				//扩展文本1坐标Y
	uint32_t ext_text1_display_w;				//扩展文本1宽度(获取有效)
	uint32_t ext_text1_display_h;				//扩展文本1高度(获取有效)
	uint32_t ext_text2_enable;					//是否显示扩展文本2；1：显示，0：隐藏
	uint8_t  ext_text2_data[J_SDK_MAX_TEXT_DATA_LEN];//扩展文本2
	uint32_t ext_text2_display_x;				//扩展文本2坐标X
	uint32_t ext_text2_display_y;				//扩展文本2坐标Y
	uint32_t ext_text2_display_w;				//扩展文本2宽度(获取有效)
	uint32_t ext_text2_display_h;				//扩展文本2高度(获取有效)
}JOSDExtend;

typedef struct __OSDParameter
{
	uint32_t time_enable;					//是否显示时间；1：显示，0：隐藏
	uint32_t time_display_x;				//时间坐标X
	uint32_t time_display_y;				//时间坐标Y
	uint32_t time_display_color;			//时间显示颜色
	uint32_t text_enable;					//是否显示文本；1：显示，0：隐藏
	uint32_t text_display_x;				//文本坐标X
	uint32_t text_display_y;				//文本坐标Y
	uint32_t text_display_color;			//文本显示颜色
	uint32_t max_width;						//最大宽度
	uint32_t max_height;					//最大高度
	uint8_t text_data[J_SDK_MAX_TEXT_DATA_LEN];
	uint32_t stream_enable;					//是否显示码流信息；1：显示，0：隐藏
	
	uint32_t time_display_w;				//时间宽度(获取有效)
	uint32_t time_display_h;				//时间高度(获取有效)
	uint32_t text_display_w;				//文本宽度(获取有效)
	uint32_t text_display_h;				//文本高度(获取有效)
	uint32_t osd_invert_color;				//osd 反色；1：开启，0：关闭
	JOSDExtend ext_osd;
}JOSDParameter;

typedef struct __PTZParameter
{
	uint32_t serial_no;						//串口号
	uint32_t ptz_addr;						//云台地址
	uint32_t protocol;						//0:PelcoD, 1:PelcoP
	uint32_t baud_rate;						//波特率
	uint32_t data_bit;						//数据位 
	uint32_t stop_bit;						//停止位
	uint32_t verify;						//校验
}JPTZParameter;

typedef struct __FTPParameter
{
	uint8_t ftp_ip[J_SDK_MAX_IP_LEN];		//ftp服务器ip
	uint8_t ftp_usr[J_SDK_MAX_ACCOUNT_LEN];	//ftp服务器port
	uint8_t ftp_pwd[J_SDK_MAX_PASSWD_LEN];	//登陆用户名
	uint8_t ftp_path[J_SDK_MAX_PATH_LEN];	//登陆密码
	uint32_t ftp_port;						//Ftp上传路径
}JFTPParameter;

typedef struct __SMTPParameter
{
	uint8_t mail_ip[J_SDK_MAX_IP_LEN];		//smtp 服务器地址
	uint8_t mail_addr[J_SDK_MAX_ADDR_LEN];	//发送邮件地址
	uint8_t mail_usr[J_SDK_MAX_ACCOUNT_LEN];//发送邮件账号
	uint8_t mail_pwd[J_SDK_MAX_PASSWD_LEN];	//发送邮件密码
	uint8_t mail_rctp1[J_SDK_MAX_ADDR_LEN];	//接受邮件地址1
	uint8_t mail_rctp2[J_SDK_MAX_ADDR_LEN];	//接受邮件地址2
	uint8_t mail_rctp3[J_SDK_MAX_ADDR_LEN];	//接受邮件地址3
	uint32_t mail_port;						//smtp 服务器端口
	uint32_t ssl_enable;					//是否启用SSL
}JSMTPParameter;

typedef struct __UPNPParameter
{
	uint8_t  upnp_ip[J_SDK_MAX_IP_LEN];		//upnp 服务器IP
	uint32_t upnp_enable;					//upnp 开关
	uint32_t upnp_eth_no;					//0使用本机网卡映射 1 使用无线网卡映射
	uint32_t upnp_model;					//0固定映射 1自动映射
	uint32_t upnp_refresh_time;				//upnp 刷新时间(单位小时)
	uint32_t upnp_data_port;				//固定映射模式下, 数据端口的外网端口
	uint32_t upnp_web_port;					//固定映射模式下, Web端口的外网端口
	uint32_t upnp_data_port_result;			//数据端口的外网端口 0代表没有映射或不成功
	uint32_t upnp_web_port_result;			//Web端口的外网端口 0代表没有映射或不成功
    uint32_t upnp_cmd_port;                 // 命令端口的外网端口 
    uint32_t upnp_talk_port;                // 对讲端口的外网端口 
    uint32_t upnp_cmd_port_result;          //命令端口的外网端口映射状态 0代表没有映射或不成功
    uint32_t upnp_talk_port_result;         //对讲端口的外网端口映射状态 0代表没有映射或不成功
}JUPNPParameter;

typedef struct __DeviceDiskInfo
{
	uint32_t disk_num;
	JDiskInfo disk[J_SDK_MAX_DISK_NUMBER];
}JDeviceDiskInfo;

typedef struct __FormatDisk
{
	uint32_t disk_no;						//磁盘号
	uint32_t progress;						//格式化进度(0 - 100)
}JFormatDisk;

typedef struct __MoveAlarm
{
	uint32_t move_enable;
	uint32_t sensitive_level;
	uint32_t detect_interval;
	uint32_t max_width;
	uint32_t max_height;
	JArea detect_area;
	JWeek week;
}JMoveAlarm;

typedef struct __LostAlarm
{
	uint32_t lost_enable;
	uint32_t detect_interval;
	JWeek week;
}JLostAlarm;

typedef struct __HideAlarm
{
	uint32_t hide_enable;
	uint32_t detect_interval;
	uint32_t sensitive_level;				//	灵敏度字段，范围 0-4，值越低，越灵敏
	uint32_t max_width;
	uint32_t max_height;
	JArea detect_area;
	JWeek week;
}JHideAlarm;

typedef struct __IoAlarm
{
	uint32_t io_type;						//详情见JIOType
	uint32_t alarm_enable;					//
	uint32_t detect_interval;				//
	JWeek week;								//
}JIoAlarm;

typedef enum __JProcessType
{
	JOINT_SNAP_FTP = 0x1,
}JJProcessType;
typedef struct __JProcess
{
	uint32_t joint_post_process[J_SDK_MAX_CHN_SIZE];	// 详情见JJProcessType
}JJProcess;

typedef struct __JointAction
{
	uint32_t alarm_type;					//详情见JAlarmType
	JJoint joint;							//
	JJProcess process;
}JJointAction;

typedef struct __PTZControl
{
	uint32_t action;						//云台动作，详情见JPTZAction
	uint32_t param;							//各种操作参数值
}JPTZControl;

typedef struct __SubmitAlarm
{
	uint8_t data[J_SDK_MAX_ALARM_DATA_LEN];
	uint32_t channel;
	uint32_t alarm_type;					//详见 JAlarmType
	uint32_t action_type;	//如果alarm_type 为J_SDK_AVD_ALARM，参照JAvdEventType 定义按位解析
                            //否则action_type 为0：开始告警，1：结束告警
	JTime alarm_time;
}JSubmitAlarm;

typedef struct __MediaUrl
{
	uint32_t level;							//码流类型
	uint32_t media_type;					//详见 JMediaType
	uint8_t ip[J_SDK_MAX_IP_LEN];			//
	uint8_t url[J_SDK_MAX_URL_LEN];			//
	uint8_t cu_ip[J_SDK_MAX_IP_LEN];		//客户端IP
}JMediaUrl;

typedef struct __StoreLog
{
	uint32_t rec_type;						//详见 JRecodeType
	uint32_t beg_node;
	uint32_t end_node;
	JTime beg_time;
	JTime end_time;
	
	uint32_t node_count;
	uint32_t total_count;
	uint32_t sess_id;
	Store store[J_SDK_MAX_STORE_LOG_SIZE];
}JStoreLog;

typedef struct __UserInfo
{
	uint8_t username[J_SDK_MAX_NAME_LEN];	//用户名
	uint8_t password[J_SDK_MAX_PWD_LEN];	//密码	
}JUserInfo;

typedef struct __UserHeart
{
	JTime server_time;
}JUserHeart;

typedef struct __FirmwareUpgrade
{
	uint32_t percent;
	uint32_t data_len;
	uint32_t file_len;
	uint32_t sess_id;
	uint8_t addr[J_SDK_MAX_IP_LEN];
	uint8_t data[J_SDK_MAX_UPGRADE_DATA_LEN];
}JFirmwareUpgrade;

typedef struct __UpgradeProgress
{
	uint32_t percent;
}JUpgradeProgress;

typedef struct __ClientSimulatorConfig
{
    uint32_t count;
    Channel channel[J_MAX_CHANNEL_INFO_SIZE];
}ClientSimulatorConfig;

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

/***************************** Capability start ********************************/
typedef enum __PlatformType
{
	PF_JXJ		= 0x1,	//深圳佳信捷
	PF_ZSYH		= 0x2,	//中盛益华
	PF_HXHT     = 0x4,	//互信互通平台
	PF_XL_NVR   = 0x8,  //讯垒NVR
}JPlatformType;

// CpuType
typedef enum __HWEncodeType
{
	TI_DM365 = 0,
	TI_DM368,
	TI_DM8127,
	TI_DM8168,
	
	HI_3507	 = 0x100,
	HI_3510,
	HI_3511,
	HI_3512,
	HI_3515,
	HI_3516,
	HI_3517,
	HI_3518,
	HI_3520,
	HI_3521,
	HI_3531,
	HI_3532,
	HI_3531_3532,
}JHWEncodeType;

// EncodeType
typedef enum __VideoEncodeType
{
	VIDEO_ENCODE_H264_E	  = 0x1,
	VIDEO_ENCODE_MJPEG_E  = 0x2,
	VIDEO_ENCODE_JPEG_E   = 0x4,
	VIDEO_ENCODE_MPEG4_E  = 0x8
}JVideoEncodeType;


// NetSpeed
typedef enum __NetSpeed
{
	NET_SPEED_NONE = 0,
	NET_SPEED_10M,
	NET_SPEED_100M,
	NET_SPEED_1000M,
}JNetSpeed;

typedef enum __NetProtocolType
{
	PROTOCOL_CUSTOM	 = 0x1,
	PROTOCOL_RTSP	 = 0x2,
	PROTOCOL_ONVIF	 = 0x4,
	PROTOCOL_G28181  = 0x8,     //g28181
}JNetProtocolType;

typedef enum __SysCap
{
	DEV_CAP_VIDEO_IN 	= 0x1,				// 视频输入
	DEV_CAP_AUDIO		= 0x2,				// 音频
	DEV_CAP_IRIS		= 0x4,				// 光圈
	DEV_CAP_PTZ			= 0x8,				// 云台
	DEV_CAP_ALARM_IN	= 0x10,				// 报警输入
	DEV_CAP_ALARM_OUT	= 0x20,				// 报警输出
	DEV_CAP_WIFI		= 0x40,				// WIFI
	DEV_CAP_3G			= 0x80,				// 3G
	DEV_CAP_STORAGE		= 0x100,			// 存储
	DEV_CAP_WEB			= 0x200,			// WEB
	DEV_CAP_PLATFORM	= 0x400,			// 平台
	DEV_CAP_INTELLIGENT_ANALYSIS = 0x800,	// 智能分析
	DEV_CAP_UPDATE		= 0x1000,			// 更新
	DEV_CAP_VIDEO_OUT	= 0x2000,			// 视频输出
	DEV_CAP_IR			= 0x4000,			// 红外
	DEV_CAP_NET_STOR	= 0x8000,			// 网络存储
	//+++++++++++IPC++++++++++++++//
    DEV_CAP_NETWORK_FAULT = 0x10000,        //网络故障检测
}JSysCap;

typedef enum __IvsCap
{
	IVS_BRIGHT_ABMNL_CAP        = 0x1,		//亮度异常
    IVS_CLARITY_ABMNL_CAP       = 0x2,		//清晰度异常
    IVS_NOISE_ABMNL_CAP         = 0x4,		//噪声异常
	IVS_ABMNL_CAP				= 0x8,		//颜色异常
    IVS_SCENE_CHANGE_CAP        = 0x10,		//场景变换、人为干扰

    IVS_TRIP_WIRE_CAP           = 0x20,		//单绊线
    IVS_DOUBLE_TRIP_WIRE_CAP    = 0x40,		//双绊线
    IVS_PERI_METER_CAP          = 0x80,		//警戒区
    IVS_LOITER_CAP              = 0x100,	//徘徊检测
    IVS_LEFT_CAP                = 0x200,	//物品遗留
    IVS_TAKE_CAP                = 0x400,	//物品丢失
	//+++++++++++IPC++++++++++++//
	IVS_HERD_CAP                = 0x800,	//牧业分析
}JIvsCap;

typedef enum __ImaCap
{
    IMA_BRIGHTNESS 	= 0x00000001,			//亮度
    IMA_CONTRAST    = 0x00000002,			//对比度
    IMA_SATURATION 	= 0x00000004,			//饱和度
    IMA_HUE		  	= 0x00000008,			//色调
    IMA_GAMMA		= 0x00000010,			//GAMMA值
    IMA_SHARPNESS	= 0x00000020,			//锐度
                                    
    IMA_MIRROR		= 0X00000040,			//镜向
    IMA_FLIP		= 0X00000080,			//翻转
    IMA_PWDFREQ	    = 0X00000100,			//照明频率1   
    
	IMA_AWB			= 0x00000200,			//自动白平衡
	IMA_WDR			= 0x00000400,			//宽动态
	IMA_IRIS		= 0x00000800,			//光圈类型
	IMA_AEX_CMP     = 0x00001000,			//曝光补偿
	IMA_AEX_MODE    = 0x00002000,			//曝光模式
	
	IMA_LDC 	    = 0x00004000,			//畸变校正
	IMA_ANTI_FOG    = 0x00008000,			//去雾
	IMA_RATATE	    = 0x00010000,			//旋转
	IMA_SHUTTER     = 0X00020000,           //电子快门
}JImaCap;

typedef enum __ResCap
{
    CAP_VIDEO_QCIF      = 0x1,				//176*144  n制176*120
    CAP_VIDEO_CIF		= 0x2,				//352*288  n制352*240
    CAP_VIDEO_HD1		= 0x4,				//704*288  n制704*240
    CAP_VIDEO_D1		= 0x8,				//704*576  n制704*480
    CAP_VIDEO_QQVGA		= 0x10,				//160*112
    CAP_VIDEO_QVGA		= 0x20,				//320*240
    CAP_VIDEO_VGA		= 0x40,				//640*480
    CAP_VIDEO_SVGA		= 0x80,				//800*600
    CAP_VIDEO_UXGA		= 0x100,			//1600*1200
    CAP_VIDEO_720P		= 0x200,			//1280*720
    CAP_VIDEO_960		= 0x400,			//1280*960
    CAP_VIDEO_1080P		= 0x800,			//1920*1080
}JResCap;


// 通道能力集
typedef struct __ChnCap
{
	uint32_t	size;						//结构体大小
	uint8_t		encode;						//详情见JVideoEncodeType	
	uint8_t		supp_mask;					//是否支持区域遮挡	0 不支持  1支持
	uint8_t		mask_count;					//遮挡区域个数
	uint8_t		supp_hide_alarm;			//是否支持遮挡报警  
	uint8_t		hide_alarm_count;			//遮挡告警检查区域个数
	uint8_t		supp_move_alarm;			//是否支持移动侦测报警
	uint8_t		move_alarm_count;			//移动侦测报警区域个数
	uint8_t		supp_video_lost_alarm;		//是否支持视频丢失报警
	uint8_t		te_reserve[2];				//保留
	uint8_t		osd_count;					//OSD个数
	uint8_t		stream_count;				//码流数
	uint32_t	stream_supp_resolution[J_SDK_MAX_STREAM_SIZE];	//详情见JResCap
	uint32_t	stream_max_frate_rate[J_SDK_MAX_STREAM_SIZE];	//码流最大支持帧率
	uint32_t	img_cap;										//图像能力集 ，详情见JImaCap
}JChnCap;

typedef struct __DevCap
{
	uint32_t	size;
	uint32_t	cap0;						//能力集，详情见JSysCap
	uint32_t	ivs_cap;					//智能分析能力集，详情见JIvsCap
	uint32_t	res_cap;					//
	uint32_t	eth_speed;					//详情见JNetSpeed
	uint32_t	hw_encoder_type;			//详情见JHWEncodeType
	uint32_t	video_encode_type;			//详情见JVideoEncodeType
	uint32_t	platform_type;				//详情见JPlatformType
	uint32_t	protocal_type;				//详情见JNetProtocolType
	uint32_t	nvr_pu_proto;				//可接入的ipc 协议类型(pu_type = NVR)，JIPCFactoryType 按位取值
	uint32_t	pu_type;					//详情见JPuType
	uint32_t	herd_enable;				//牧业监控
	uint8_t		reserve[16];				//保留
	uint8_t		ftp_enable;					//是否支持 ftp，0 不支持  1支持
	uint8_t		upnp_enable;				//是否支持 upnp，0 不支持  1支持
	uint8_t 	eth_num;					//网口个数
	uint8_t		chn_num;					//通道数
	JChnCap		chn_cap;					//通道能力集 ，详情见JChnCap
}JDevCap;
/***************************** Capability end ********************************/

typedef enum __ChnType
{
	CH_LOCAL,								//通道类型: 本地
	CH_NETWORK								//通道类型: 网络
}JChnType;
typedef enum __ChnStatusType
{
	CH_FORBID,								//通道状态: 禁止
	CH_NORMAL,								//通道状态: 正常
	CH_CONNECTING,							//通道状态: 正在连接
}JChnStatusType;

typedef struct __RemoteChannelInfo
{
	uint32_t	ch_no;							//通道号（0-255）
	uint32_t	protocol;						//音视频传输协议
	uint32_t	audio_enable;					//是否传输音频
	uint8_t		user_name[J_SDK_MAX_NAME_LEN];	//用户名称
	uint8_t		user_pwd[J_SDK_MAX_PWD_LEN];	//用户密码
	uint32_t	win_mode;						//浏览窗口码流模式: 0-自动, 1-手动
	uint32_t 	win_max_strm;					//大窗口浏览时视频码流类型
	uint32_t 	win_min_strm;					//小窗口浏览时视频码流类型
}JRemoteChannelInfo;

typedef struct __RemoteDeviceInfo
{
	uint8_t 	ip[J_SDK_MAX_IP_LEN];		//IP地址
	uint32_t 	port;						//端口
	uint32_t 	dev_type;					//设备类型
	uint32_t 	ch_sum;						//总通道数
	uint32_t 	dns_enable;					//是否使用DNS链接
	uint8_t 	dns[J_SDK_MAX_DNS_LEN];		//DNS路径
}JRemoteDeviceInfo;

typedef struct __ChannelInfo
{
	uint32_t ch_no;							//通道号（0-255）
	uint32_t ch_type;						//通道类型，详情见JChnType
	uint32_t ch_status;						//通道状态（禁止、正常、正在链接）
	uint8_t ch_name[J_SDK_MAX_NAME_LEN];	//通道名称
	JRemoteChannelInfo rmt_ch_info;			//远程设备通道信息
	JRemoteDeviceInfo rmt_dev_info;			//远程设备信息
}JChannelInfo;

typedef struct __PictureInfo
{
	uint32_t mirror;						//镜像(图像左右调换) 1 镜像 0 不镜像
	uint32_t flip;							//翻转(图像上下调换) 1 翻转 0 不翻转
	uint32_t hz;							//抗闪烁 0-关闭 1-50Hz 2-60 Hz 
	uint32_t awb_mode;						//白平衡模式 0 自动白平衡 1手动白平衡
	uint32_t awb_red;						//白平衡红色强度 0-255
	uint32_t awb_blue;						//白平衡蓝色强度 0-255
	uint32_t wdr;							//宽动态0 关闭 1-255为宽动态强度
	uint32_t iris_type;						//光圈类型 0 手动光圈 1 自动光圈
	uint32_t exp_compensation;				//曝光补偿 0-255
	uint32_t ae_mode;	 					//自动曝光模式（0-2）：0 噪声优先	  1 帧率优先	2 增益优先
	uint32_t ldc_value;						//畸变校正强度(0-255)：0 不启用    1-255 畸变校正强度值
	uint32_t anti_fog;	 					//去雾：0 不启用    1 开启
	uint32_t rotate;	 					//旋转：0 无    1 旋转90度    2 旋转270度 
	uint32_t shutter;						//电子快门 0-8  0：自动快门 1：1/25S， 2：1/50S， 3：1/100S， 4：1/500S， 5：1/1000S， 6：1/2000S， 7：1/5000S， 8：1/10000S
}JPictureInfo;

typedef struct __WifiConfig
{
	uint8_t essid[J_SDK_MAX_ID_LEN];		//服务区别号
	uint8_t pwd[J_SDK_MAX_PWD_LEN];			//密码
	uint32_t wifi_enable;					//0：断开，1：连接
	uint32_t encrypt_type;					//加密类型0:NONE  1:WPA  2:WPA2 3:WEP
	uint32_t auth_mode;						//认证方式0:NONE  1:EAP 2:PSK 3:OPEN 4:SHARED 
	uint32_t secret_key_type;				//密钥管理方式 0:NONE  1:AES 2:TKIP 只对应于加密类型为WPA/WPA2的情况
	uint32_t wifi_st;						//无线网卡连接状态 0 无连接 1 正在连接2 成功
}JWifiConfig;

typedef struct __WifiApInfo
{
	uint8_t essid[J_SDK_MAX_ID_LEN];		//服务区别号
	uint32_t encrypt_type;					//加密类型0:NONE  1:WPA  2:WPA2 3:WEP
	uint32_t auth_mode;						//认证方式0:NONE  1:EAP 2:PSK 3:OPEN 4:SHARED 
	uint32_t secret_key_type;				//密钥管理方式 0:NONE  1:AES 2:TKIP 只对应于加密类型为WPA/WPA2的情况
	uint32_t quality;						//信号质量 0-100
	uint32_t bit_rate;						//传输速率
}JWifiApInfo;

typedef struct __WifiSearchRes
{
	uint32_t count;							//搜索到的wifi 个数
	JWifiApInfo wifi_ap[J_SDK_MAX_WIFI_AP_SIZE];//
}JWifiSearchRes;

typedef struct __NetworkStatus
{
	uint32_t eth_st;						//有线网卡连接状态 0 无连接 1正在连接 2 成功
	uint32_t wifi_st;						//无线网卡连接状态 0 无连接 1 正在连接 2 成功
	uint32_t pppoe_st;						//pppoe拨号状态 0 无连接 1正在连接 2 成功
}JNetworkStatus;

typedef enum __DevCntlCmd
{
	SHUTDOWN_DEVICE  = 0x01,				//设备关机
	RESTART_DEVICE   = 0x02,				//设备重启
	RESTORE_DEFAULT  = 0x03,				//恢复出厂默认
	DETECT_DEAL_PIX  = 0x04,				//坏点检测
	DETECT_IRIS      = 0x05,				//光圈校正
}JDevCntlCmd;

typedef enum __DetectCmd
{
	DETECT_DEAL_PIX_START   = 0x10,			//坏点检测开始
	DETECT_DEAL_PIX_END     = 0x11,			//坏点检测结束(成功)
	DETECT_DEAL_PIX_FAIL    = 0x12,			//坏点检测失败
	DETECT_DEAL_PIX_TIMEOUT = 0x13,			//坏点检测超时
}DetectCmd;

typedef enum __RegulateCmd
{
	REGULATE_IRIS_START   = 0x20,			//光圈校正开始
	REGULATE_IRIS_END	  = 0x21,			//光圈校正结束(成功)
	REGULATE_IRIS_FAIL    = 0x22,			//光圈校正失败
	REGULATE_IRIS_TIMEOUT = 0x23,			//光圈校正超时
}JRegulateCmd;

typedef struct __ControlDevice
{
	uint32_t command;						//控制命令类型
}JControlDevice;

typedef struct __SubmitDeviceStatus
{
	uint32_t ddp_status;					//坏点检测状态
}JSubmitDeviceStatus;

typedef struct __UserConfig
{
	uint8_t username[J_SDK_MAX_NAME_LEN];	//用户名
	uint8_t password[J_SDK_MAX_PWD_LEN];	//密码
	uint32_t local_right;					//本地操作权限
	uint32_t remote_right;					//远程操作权限
	uint32_t reserve;
}JUserConfig;

typedef struct __RegionConfig
{
	uint32_t enable;						//1:开启	0: 不开启
	uint32_t abs_qp;						//0 相对质量 1 绝对质量
	uint32_t qp_val;						//质量值 0-51
	uint32_t max_width;						//最大宽度
	uint32_t max_height;					//最大高度
	JArea region;							//区域
}JRegionConfig;//roi

typedef struct __DdnsConfig
{
	uint8_t ddns_account[J_SDK_MAX_NAME_LEN];	//DDNS 注册名
	uint8_t ddns_usr[J_SDK_MAX_NAME_LEN];		//用户名称
	uint8_t ddns_pwd[J_SDK_MAX_PWD_LEN];		//用户密码
	uint32_t ddns_open;							//DDNS开关
	uint32_t ddns_type;							//DDNS类型（0-dyndns 1-3322）
	uint32_t ddns_port;							//DDNS服务器端口
	uint32_t ddns_times;						//更新时间
}JDdnsConfig;

typedef struct __HxhtPfConfig
{
	uint8_t  dev_id[J_SDK_MAX_ID_LEN/*32*/];	//设备ID
	uint8_t  svr_ip[J_SDK_MAX_ID_LEN/*32*/];	//接入服务器地址
	uint16_t svr_port;							//接入服务器端口

	uint16_t  video_port;						//本地视频端口
	uint16_t  audio_port;						//本地音频端口
	uint16_t  msg_port;							//本地消息端口
	uint16_t  pb_port;							//本地回放端口

	uint8_t  max_video_conn;					//最大视频连接数
	uint8_t  pic_upload;						//开启照片上传功能

	uint8_t  dev_after_nat;						//0直连优先 1转发优先(是否在NAT 后面，上报0 则视频等优先直连，否则转发优先，默认走分发服务器)
	uint8_t  port_map;							//自动映射服务端口 (为支持设备内网时直连请求视频\对讲\回放)
    uint8_t  video_trans_mode;					// 视频传输模式:1 天翼手机监控模式，0 一般模式,默认为0
	uint8_t  res1;

	uint32_t upnp_ip;							//upnp 服务器地址 (主机字节序)
	uint8_t  res2[28];
}JHxhtPfConfig;

typedef struct __UserModConfig
{
	uint8_t old_usrname[J_SDK_MAX_NAME_LEN];	//旧用户名
	uint8_t old_password[J_SDK_MAX_PWD_LEN];	//旧用户名
	uint8_t new_username[J_SDK_MAX_NAME_LEN];	//新用户名
	uint8_t new_password[J_SDK_MAX_PWD_LEN];	//新密码
	uint32_t local_right;						//本地操作权限
	uint32_t remote_right;						//远程操作权限
	uint32_t reserve;
}JUserModConfig;

typedef struct __Segment
{
	uint32_t 	open     :8;					//开关
	uint32_t 	begin_sec:24;					//开始时间秒数 3600*hour+60*min+sec
	
	uint32_t 	reserve:8;						//保留
	uint32_t 	end_sec:24;						//结束时间秒数 3600*hour+60*min+sec
}JSegment;

typedef struct __AvdRule
{
    uint8_t		enable;          				//0 不检测 1 检测
    uint8_t		level;            				//灵敏度0-4 值越大灵敏度越高
    uint8_t		alarm_times;        			// 异常报警时间，（0-20秒），数值越大需要的报警时间越长
    uint8_t		reserve[29];          
}JAvdRule;

typedef struct __AvdConfig
{
    uint32_t	enable;                			//avd 使能
    JSegment	sched_time[J_SDK_MAX_SEG_SZIE]; //时间段
    JAvdRule	avd_rule[MAX_IVS_AVD_RULES];  	//视频诊断规则，index 下标取值见 JAvdType
}JAvdConfig;

typedef struct __GB28181Proto
{
	uint8_t device_id[J_SDK_MAX_ID_LEN];		// 设备ID
	uint8_t device_pwd[J_SDK_MAX_PASSWD_LEN];	// 设备密码 
	uint8_t server_id[J_SDK_MAX_ID_LEN];		// 服务器ID
	uint8_t server_ip[J_SDK_MAX_IP_LEN];		// 服务器地址
	uint16_t server_port;						// 服务器端口
	uint16_t msg_port;							// 本地消息端口    
	uint8_t res[32];
}JGB28181Proto;

typedef struct __IscsiDiskCfg
{
	uint16_t disk_port;							//网络磁盘服务端口，默认值为3260 
	uint16_t res1;								// 预留
	uint8_t  cli_name[MAX_NET_DISK_NAME_LEN];	// 启动器名称 [ ipsan 导出的SAN客户对应的Initiator名称，设备启动iscsi 服务使用]   ---》新增参数
	uint8_t  tg_name[MAX_NET_DISK_NAME_LEN];	// 网络磁盘Target名称 (目标器名，按照 RFC 3720 的规定，iscsi 节点的名字最大长度不能超过 223 bytes)
	uint32_t discovey_auth;						// discovey  Target时是否启用验证，0: NONE, 不需要认证 1. CHAP 认证   ---》新增参数
	uint8_t  dis_chap_usr[MAX_NET_DISK_NAME_LEN];// discovey  使用的CHAP验证用户名   ---》新增参数
	uint8_t  dis_chap_pwd[J_SDK_MAX_PWD_LEN];	// discovey  CHAP 认证密码(实际为 12-16 字节字符串)   ---》新增参数
	uint32_t tg_chap_auth;						// Target 访问认证方式 0: NONE, 不需要认证1. CHAP，访问时必需使用 CHAP 认证 
	uint8_t  tg_chap_usr[MAX_NET_DISK_NAME_LEN];// 登陆Target CHAP 认证用户名
	uint8_t  tg_chap_pwd[J_SDK_MAX_PWD_LEN];	// 登陆Target CHAP 认证密码(实际为 12-16 字节字符串)
	uint8_t  res2[32];
}JIscsiDiskCfg;

typedef struct __NetworkDisk
{
	uint32_t enable;							// 是否启用网络盘
	uint32_t proto;								// 网络盘协议类型 (目前仅支持iscsi，值为0，后续再添加)
	uint8_t  disk_ip[J_SDK_MAX_IP_LEN];			// 网络磁盘Ip地址
	JIscsiDiskCfg iscsi_disk;					// iscsi 磁盘配置
}JNetworkDisk;

typedef struct __DistState
{
	uint32_t volume;							//硬盘容量  M
	uint32_t free_space;						//硬盘剩余空间 M
	uint32_t hd_static;							//硬盘状态 0－正在使用；1－未挂载；2－已挂载未使用 
}JDistState;

typedef struct __ChnState
{
	uint8_t  record_static;						//通道是否在录像：0－不录像；1－录像 
	uint8_t  signal_static;						//连接的信号状态：0－正常，  1－信号丢失  
	uint8_t  hardware_static;					//通道硬件状态：  0－正常，  1－异常（例如DSP异常） 
	uint8_t  res1;
	uint32_t bit_rate[J_SDK_MAX_STREAM_SIZE];	//实际码率 
	uint8_t  res[12];
}JChnState;

typedef struct __DevWorkState
{  
	uint32_t   device_static;								//dwDeviceStatic 设备的状态：0－正常；1－CPU占用率太高，超过85%；
	JDistState hd_static[J_SDK_MAX_DISK_NUMBER];			//硬盘状态
	JChnState  chn_static[J_SDK_MAX_CHN_SIZE];				//通道状态 
	uint8_t    alarm_in_static[J_SDK_MAX_ALARMIN_SIZE];		//报警输入口的状态：0-没有报警；1-有报警
	uint8_t    alarm_out_static[J_SDK_MAX_ALARMOUT_SIZE];	//报警输出口的状态：0-没有输出，1-有报警输出 
	uint32_t   local_display;								//本地显示状态：0-正常，1-不正常
	uint8_t    audio_chn_status[J_SDK_MAX_CHN_SIZE];		//表示语音通道的状态：0-未使用，1-使用中，0xff无效 
	uint8_t    res[12];
}JDevWorkState;


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

typedef struct __BinaryStream
{
	uint32_t type;							//二进制数据类型
	uint32_t flag;							//包的标志
	uint32_t seq;							//包的序号
	uint32_t size;							//整个结构体的 size
	uint8_t data[0];						//附加二进制数据起始位置
}JBinaryStream;

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


#ifdef _USE_DECODER_PROTO_

#define J_SDK_MAX_MODE_SIZE					32
#define J_SDK_MAX_DIVISION_SIZE				64
#define J_SDK_MAX_DESCRIBE_SIZE				256


typedef struct __DivMode
{
	uint32_t div_id;
	uint8_t mode_name[J_SDK_MAX_NAME_LEN];
	uint8_t description[J_SDK_MAX_DESCRIBE_SIZE];
}JDivMode;

typedef struct __DivisionMode
{
	uint32_t page_size;						//请求个数
	uint32_t start_row;						//开始查询的行数
	
	uint32_t total;							//
	uint32_t count;							//
	JDivMode mode[J_SDK_MAX_MODE_SIZE];		//
}JDivisionMode;

typedef struct __DivisionInfo
{
	uint32_t div_no;						//分隔序号
	uint32_t action;						//动作类型: 1-STEP, 2-TOUR, 3-GROUP
	uint32_t result;						//0:成功, !0:失败
	uint8_t encoder[J_SDK_MAX_NAME_LEN];	//编码器名称
	uint8_t enc_chn;						//编码器通道
	uint8_t level;							//码流类型
	uint8_t url[J_SDK_MAX_URL_LEN];			//编码器URL（为NULL则在当前分割窗口显示黑屏）
	uint32_t flag;							//清分割标志（0：不清除分割1：清除分割）
}JDivisionInfo;

typedef struct __FullScreenMode
{
	uint8_t encoder[J_SDK_MAX_NAME_LEN];	//编码器名称
	uint8_t enc_chn;						//编码器通道
	uint8_t level;							//码流类型
	uint32_t action;						//动作类型: 1-STEP, 2-TOUR, 3-GROUP
	uint32_t result;						//0:成功, !0:失败
}JFullScreenMode;

typedef struct __ScreenState
{
	uint32_t div_id;						//分屏模式ID
	uint32_t count;							//分割个数
	JDivisionInfo division[J_SDK_MAX_DIVISION_SIZE];
	uint32_t fs_mode;						//是否全屏
	JFullScreenMode full_screen;
}JScreenState;

typedef struct __ChangeDMode
{
	uint32_t div_id;						//分屏模式ID
	uint32_t dis_chn;						//显示通道号
}JChangeDMode;

typedef struct __Screen
{
	uint32_t div_no;						//分割序号
	uint32_t dis_chn;						//显示通道号
}JScreen;

typedef struct __TVWallPlay
{
	uint32_t action;						//动作类型: 0-STEP, 1-TOUR, 2-GROUP
	uint8_t name[J_SDK_MAX_NAME_LEN];		//巡回或群组的名称（单步没有名称）
	uint32_t step;							//步序号（单步填1）
	uint32_t dis_chn;						//显示通道编号
	uint32_t div_id;						//分屏模式ID
	uint32_t k_other;						//是否保持其它分割的运行状态, 0-不保持, 黑屏, 1-保持
	uint32_t count;							//本次下发的编码器个数
	JDivisionInfo division[J_SDK_MAX_DIVISION_SIZE];
}JTVWallPlay;

# endif //_USE_DECODER_PROTO_

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#define J_SDK_MAX_DEV_LOG_SIZE 32

//日志操作类型
typedef enum __SDK_LOG_OP
{
	J_SDK_LOG_QUERY = 0x01,  // 查询 sdk_log_item_t, sdk_log_cond_t
	J_SDK_LOG_EMPTY,         // 清空(全部)
	J_SDK_LOG_BACKUP,        // 备份
	J_SDK_LOG_DEL,           // 删除(按时间段)
	J_SDK_LOG_RESUME,        // 恢复
}JSDK_LOG_OP;


//日志类型定义（主类型，最大支持32种）
typedef enum __LOG_MAJOR_TYPE
{
	J_LOG_MAJOR_SYSTEM 		= 0x00000001,  //系统日志
	J_LOG_MAJOR_ALARM  		= 0x00000002,  //告警日志
	J_LOG_MAJOR_OPERATE		= 0x00000004,  //操作日志
	J_LOG_MAJOR_NETWORK		= 0x00000008,  //网络日志
	J_LOG_MAJOR_PARAM  		= 0x00000010,  //参数日志
	J_LOG_MAJOR_EXCEPTION	= 0x00000020,  //异常日志
	    
	J_LOG_MAJOR_ALL    		= 0xffffffff,  //所有日志
}JLOG_MAJOR_TYPE;


//系统日志子类型
typedef enum __SYSTEM_MINOR_TYPE
{
	J_SYSTEM_MINOR_STARTUP = 0x0001,  //开机
	J_SYSTEM_MINOR_SHUTDOWN,          //关机
	J_SYSTEM_MINOR_REBOOT,			  //重启

	J_SYSTEM_MINOR_ALL     = 0xffff   //所有系统日志
}JSYSTEM_MINOR_TYPE;


//告警日志子类型
typedef enum __ALARM_MINOR_TYPE
{
	J_ALARM_MINOR_DI_START = 0x0001,   //输入告警发生
	J_ALARM_MINOR_DI_STOP,             //输入告警停止
	J_ALARM_MINOR_MD_START,            //移动侦测告警发生
	J_ALARM_MINOR_MD_STOP,             //移动侦测告警停止
	J_ALARM_MINOR_VL_START,			   //视频丢失告警发生
	J_ALARM_MINOR_VL_STOP,			   //视频丢失告警停止
	J_ALARM_MINOR_OD_START,			   //视频遮挡报警发生
	J_ALARM_MINOR_OD_STOP,			   //视频遮挡报警停止

	//+++++++++++++++++IPC+++++++++++++++++//
    J_ALARM_MINOR_NETCARD_DISCONN,     //网卡断线
    J_ALARM_MINOR_NETCARD_RECONN,      //网卡恢复连接
	J_ALARM_MINOR_HERD_START,     	   //牧业分析告警发生
	J_ALARM_MINOR_HERD_STOP,           //牧业分析告警停止
	//+++++++++++++++++IPC+++++++++++++++++//

	J_ALARM_MINOR_ALL		= 0xffff   //所有告警日志
}JALARM_MINOR_TYPE;


//操作日志子类型
typedef enum __OPERATE_MINOR_TYPE
{
	J_OPERATE_MINOR_LOGIN = 0x0001,		// 登陆	
	J_OPERATE_MINOR_LOGOUT, 			// 注销
	J_OPERATE_MINOR_USER_ADD,			// 用户管理-增加
	J_OPERATE_MINOR_USER_DEL,			// 用户管理-删除
	J_OPERATE_MINOR_USER_MODI,			// 用户管理-修改
	J_OPERATE_MINOR_SETTIME, 			// 设置系统时间
	J_OPERATE_MINOR_FORMAT_DISK, 		// 格式化硬盘
	J_OPERATE_MINOR_DEFAULT, 			// 恢复默认
	J_OPERATE_MINOR_UPGRADE, 			// 升级
	J_OPERATE_MINOR_PLAYBACK, 			// 回放
	J_OPERATE_MINOR_PTZ, 				// 云台控制
	J_OPERATE_MINOR_BACKUP, 			// 备份
	J_OPERATE_MINOR_RECORD_START, 		// 启动录像
	J_OPERATE_MINOR_RECORD_STOP, 		// 停止录像
	J_OPERATE_MINOR_CLR_ALARM, 			// 清除报警
	J_OPERATE_MINOR_TALKBACK_START, 	// 对讲开始
	J_OPERATE_MINOR_TALKBACK_STOP, 		// 对讲结束
	J_OPERATE_MINOR_LOG_DEL,			// 删除日志
	J_OPERATE_MINOR_LOG_EMPTY,			// 清空日志
	J_OPERATE_MINOR_LOG_BACKUP,			// 备份日志
	J_OPERATE_MINOR_MANUAL_RECORD_START,// 手动启动录像
	J_OPERATE_MINOR_MANUAL_RECORD_STOP, // 手动停止录像	
	J_OPERATE_MINOR_FORMAT_DISK_U, 		// 格式化U盘
	J_OPERATE_MINOR_CTRL_ALARM_OUT,		// 手动控制报警输出
	J_OPERATE_MINOR_DETECT_DEAL_PIX,	//坏点检测
	J_OPERATE_MINOR_DETECT_IRIS,		//光圈校正
	
	J_OPERATE_MINOR_ALL	= 0xffff		// 所有操作日志
}JOPERATE_MINOR_TYPE;


//参数日志子类型
typedef enum __PARAM_MINOR_TYPE
{
	J_PARAM_MINOR_PARAM_NETWORK = 0x0001, 	// 网络参数配置
	J_PARAM_MINOR_PARAM_UART, 				// 串口参数配置
	J_PARAM_MINOR_PARAM_PTZ, 				// 云台参数配置
	J_PARAM_MINOR_PARAM_CHAN, 				// 通道参数配置
	J_PARAM_MINOR_PARAM_VENC, 				// 编码参数配置
	J_PARAM_MINOR_PARAM_TV, 				// TV参数配置
	J_PARAM_MINOR_PARAM_PIC_ATTR,			// 图像属性配置
	J_PARAM_MINOR_PARAM_RECORD,				// 录像参数配置
	J_PARAM_MINOR_PARAM_MOTION,				// 移动侦测配置
	J_PARAM_MINOR_PARAM_VIDEOLOST,			// 视频丢失配置
	J_PARAM_MINOR_PARAM_ALARMIN,			// 输入告警配置
	J_PARAM_MINOR_PARAM_POLL,				// 轮巡配置
	J_PARAM_MINOR_PARAM_PREVIEW,			// 预览配置
	J_PARAM_MINOR_PARAM_OVERLAY,			// 遮挡区域配置
	J_PARAM_MINOR_PARAM_OSD,				// OSD配置
	J_PARAM_MINOR_PARAM_COMM,				// DVR 常规配置
	J_PARAM_MINOR_PARAM_OD,					// 视频遮挡配置
	J_PARAM_MINOR_PARAM_ROI,				// 感兴趣区域配置
	J_PARAM_MINOR_PARAM_FTP,				// FTP 配置
	J_PARAM_MINOR_PARAM_UPNP,				// UPNP 配置
	J_PARAM_MINOR_PARAM_SMTP,				// SMTP 配置
	J_PARAM_MINOR_PARAM_NTP,				// NTP  配置
	J_PARAM_MINOR_PARAM_DDNS,				// DDNS 配置
	J_PARAM_MINOR_PARAM_ALARMUP,			// 报警上传配置
	J_PARAM_MINOR_PARAM_NETFAULT,			// 网络故障检测配置
	J_PARAM_MINOR_PARAM_HERD,	            // 牧业系统分析
	J_PRARM_MINOR_ALL	= 0xffff			// 所有参数日志
}JPARAM_MINOR_TYPE;


//异常日志子类型
typedef enum __EXCEPTION_MINOR_TYPE
{
	J_EXCEPTION_HD_FULL 		= 0x0001,  	//硬盘满
	J_EXCEPTION_HD_ERROR,           		//硬盘错误
	J_EXCEPTION_NET_DISCONNECT,				//网络断开
	J_EXCEPTION_IP_CONFLICT,				//IP冲突
	
	J_EXCEPTION_MINOR_ALL     = 0xffff   	//所有异常日志
}JEXCEPTION_MINOR_TYPE;


//日志记录定义
typedef struct _OperationLogItem
{
	uint32_t times;						//Unix epoch 
	uint8_t  major_type;                //类型LOG_MAJOR_TYPE_E 
	uint8_t  minor_type;                //0x0000:直接显示desc内容，否则通过JXXX_MINOR_TYPE_E解析内容.
	uint8_t  args;                      //子类型参数(args=0时不用解析,否则根据minor_type解析内容。
										//如果minor_type和通道号相关，args表示通道号;如果minor_type和告警相关，args表示告警输入号)
	uint8_t	 res;
	uint8_t  user[J_SDK_MAX_NAME_LEN];  //操作用户
	uint32_t ip;
}JOperationLogItem;

//日志查询条件
typedef struct _OperationLogCond
{
	uint32_t beg_time;                   //开始时间
	uint32_t end_time;                   //结束时间
	uint32_t type;                       //类型 
	uint8_t  user[J_SDK_MAX_NAME_LEN];   //操作用户 不指定为空
	uint32_t ip;						 //用户IP  不指定为0
	uint32_t beg_node;
	uint32_t end_node;
	uint32_t sess_id;
}JOperationLogCond;

typedef struct _OperationLog
{
	JOperationLogCond cond;						//in 
	uint32_t total_count;						//out
	uint32_t node_count;						//out
	JOperationLogItem item[J_SDK_MAX_DEV_LOG_SIZE];// out
}JOperationLog;

typedef struct __AlarmUploadCfg
{
	uint32_t enable;							//报警上传使能开关
	uint8_t  host[J_SDK_MAX_HOST_LEN];			//报警主机IP
	uint16_t port;								//报警主机端口
	uint16_t type;								//报警协议类型
}JAlarmUploadCfg;

typedef struct __PresetPoint
{
	uint8_t  name[J_SDK_MAX_NAME_LEN];			//预置点名称
	uint32_t preset;							//预置点号，
}JPresetPoint;

typedef struct __PPConfig
{
	uint32_t action;							//预置点操作，详情见JPTZAction
	JPresetPoint pp;
}JPPConfig;

typedef struct __PPSet
{
	uint32_t pp_count;							//预置点数量
	JPresetPoint pp[J_SDK_MAX_PRESET_PORT_SIZE];//预置点集合
}JPPSet;

typedef struct __CruiseInfo
{
	uint8_t  crz_name[J_SDK_MAX_NAME_LEN];		//巡航路径名
	uint32_t crz_no;							//巡航路径号
}JCruiseInfo;

typedef struct __CruisePoint
{
    uint8_t preset;								//预置点
    uint8_t speed;								//移动速度
    uint8_t dwell;								//停留时间
    uint8_t res;								//保留
}JCruisePoint;

typedef struct __CruiseWay
{
	JCruiseInfo crz_info;
	uint32_t pp_count;									//巡航点数量
	JCruisePoint crz_pp[J_SDK_MAX_CRUISE_PORT_SIZE];	//巡航点集合(路径)
}JCruiseWay;

typedef struct __CruiseConfig
{
	uint32_t action;							//巡航路径操作，详情见JPTZAction
	uint32_t crz_no;							//巡航路径号
}JCruiseConfig;

typedef struct __CruiseWaySet
{
	uint32_t crz_count;									//巡航路径数量
	JCruiseInfo crz_info[J_SDK_MAX_CRUISE_WAY_SIZE];	//巡航路径集合
}JCruiseWaySet;

// 通道码率自动切换设置
typedef struct __VencBit
{
	uint8_t enable;				//使能码率自动切换开关
	uint8_t type;				//切换类型,值为0 则按设置的时间段来切换码率，1 为按icut的状态切换码率
	uint8_t sub_type;			//type = 0,按时间段切换时，0 为在时间段内切换成低码率，1 为在时间段内切换为高码率
								//type = 1, 按红外模式切换时，0 表示非红外模式设置为高码率值，1 表示非红外模式设置为低码率值
	uint8_t res;				//预留
	uint32_t high_er[J_SDK_MAX_STREAM_SIZE];	//各码流切换高码率值，值为0 时代表不切换
	uint32_t low_er[J_SDK_MAX_STREAM_SIZE];		//各码流切换低码率值，值为0 时代表不切换
	JSegment time_seg[J_SDK_MAX_SEG_SZIE];		//切换的时间段, type = 0 时有效
}JVencBit;

typedef struct __VencBitAutoSwitch
{
	JVencBit venc[J_SDK_MAX_CHN_SIZE];
}JVencBitAutoSwitch;

typedef struct __3DControl
{
	int32_t x_offset;
	int32_t y_offset;
	int32_t amplify;
}J3DControl;

// ir-cut 自动切换设置
typedef struct __IrcutAutoSwitch
{
    int      sensitive;     //ir switch sensitive 0:low sensitive 1 high sensitive
    uint8_t  res[24];   // 预留
}JIrcutAutoSwitch;

// ir-cut 按时间段切换设置
typedef struct __IrcutTimerSwitch
{ 
    JSegment seg_time[J_SDK_MAX_SEG_SZIE]; //时间段  in segment time open ir light, otherwise close it
    uint8_t  res[12]; // 预留
}JIrcutTimerSwitch;

//============================ 海思IPC 私用[start]===========================================
// 手动设置ir-cut打开/关闭，不再进行切换
typedef struct __IrcutManuSwitch
{
    int open;       // 1 open 0 close
    uint8_t res[12];    // 预留
}JIrcutManuSwitch;

// IR-CUT切换模式设置
typedef struct _IrcutCtrl
{
    int switch_type;    //0 auto switch 1 manual switch 2 timer switch
    int auto_c2b;       //0 auto change color 1 fix color 2  fix black white, 0 自动彩转黑, 1 固定为彩色 2 固定为黑白
    uint8_t res[8];
    JIrcutAutoSwitch auto_switch;   // nSwitchType = 0，按灵敏度等参数自动切换ir cut
    JIrcutManuSwitch manu_switch;   // nSwitchType = 1，手动设定ir cut打开/关闭
    JIrcutTimerSwitch timer_switch; // nSwitchType = 2，定时设置ir cut打开/关闭
}_JIrcutCtrl;

typedef struct __IrcutCtrl
{
    uint32_t   count;
    _JIrcutCtrl ircut_ctrl[J_SDK_MAX_CHN_SIZE];
}JIrcutCtrl;
//============================ 海思IPC 私用[end]===========================================

/* 联动IO */
typedef struct __LinkIO
{
	uint32_t time_len;
	uint8_t  data[J_SDK_MAX_ALARM_DATA_LEN];
    uint8_t  res[12];
}JLinkIO;

/* 联动预置位 */
typedef struct __LinkPreset
{
	uint32_t preset;
}JLinkPreset;

/*解码器屏幕参数(分辨率)
0: 	800   X  600
1:	1024 X  768
2:	1152 X  864
3:	1280 X  600
4:	1280 X  720
5:	1280 X  768
6:	1280 X  800
7:	1280 X  960
8:	1280 X  1024
9:	1360 X  768
10:	1366 X  768
11:	1440 X  900
12:	1680 X  900
13:	1680 X  1050
14:	1920 X  1080
15:	1920 X  1200
16:	2560 X  1600
*/
typedef struct __ResolutionInfo
{
    uint32_t resolution;
    uint8_t  res[12];
}JResolutionInfo;

//RTC:round-the-clock
typedef struct __IrcutRTCSwitch
{
    uint32_t rtc;       //0:白天,1:黑夜
    uint8_t  res[12];   // 预留
}JIrcutRTCSwitch;

enum
{
    IRCUT_AUTO = 0,
    IRCUT_RTC  = 1,
    IRCUT_TIMER= 2,
};

typedef struct __Ircut
{
    uint32_t ircut_mode;    //红外模式，0:自动，1:昼夜,2:定时模式
    uint32_t auto_c2b;      //0 auto change color 1 fix color 2  fix black white, 0 自动彩转黑, 1 固定为彩色 2 固定为黑白
    uint8_t  res[12];       // 预留
    union {
        JIrcutAutoSwitch autos;
        JIrcutRTCSwitch  rtcs;
        JIrcutTimerSwitch timers[J_SDK_MAX_DAY_SIZE];
    };
}JIrcut;

typedef struct __IrcutControl
{
    uint32_t count;
    JIrcut   ircut[J_SDK_MAX_CHN_SIZE];
}JIrcutControl;

//外网端口 
typedef struct __ExtranetPort
{
    uint16_t cmd_port;
    uint16_t data_port;   //目前仅数据端口被使用
    uint16_t http_port;
    uint16_t talk_port;
}JExtranetPort;


/*===========================牧业分析配置=======================*/
#define MAX_TROUGH_SIZE             16

typedef struct __Point
{
    uint16_t x;
    uint16_t y;
}JPoint;

typedef struct __QuadrAngle
{
    JPoint angle[4];
}JQuadrAngle;

typedef struct __Trough
{
    uint32_t id;                        //槽对应的id, 范围0~65535
    uint8_t  name[J_SDK_MAX_NAME_LEN];  //槽的名字
    JQuadrAngle quadr;                  //四边形
    uint32_t percent;                   //草料的百分比
}JTrough;

typedef struct __Field
{
    int trough_count;               //有效槽个数
    JTrough trough[MAX_TROUGH_SIZE];
}JField;

typedef struct __HerdAnalyse
{
    int8_t fodder_eable;            //是否启用草料分析, 1: 启用, 0不启用
    int8_t report_intv;             //上报间隔单位分钟, 1 ~ 10
    int16_t res;                    //保留
    uint32_t max_width;			    //参考坐标宽度
    uint32_t max_height;		    //参考坐标高度
    JWeek week;                     //分析时间段
    JField field;                   //槽配置
}JHerdAnalyse;

typedef struct __GrassPercent
{
    JField field;                   //槽配置
}JGrassPercent;


/*===========================牧业分析配置=======================*/



#if 0
// 高速球配置
#define J_SDK_MAX_BALL_ONTIME_NUM 8

// 报警参数
typedef struct __BallAlarm
{
	uint32_t action;                // 报警动作
	uint32_t duration_sec;			// 报警持续时间
	uint8_t res[32];
}JBallAlarm;

// 定时动作
typedef struct __BallOntimeAction
{
	uint8_t begin_hour;             // 开始小时
	uint8_t begin_min;              // 开始分钟
	uint8_t end_hour;               // 结束小时
	uint8_t end_min;                // 结束分钟
	uint32_t action;
	uint8_t res[12];
}JBallOntimeAction;

// 定时参数
typedef struct __BallOntimeParam
{
	JBallOntimeAction ontime_action[J_SDK_MAX_BALL_ONTIME_NUM];
	uint8_t res[32];
}JBallOntimeParam;

// 巡航点
typedef struct __BallCruisePoint
{
	uint8_t preset_enable;		// 是否使用这个预置点 
    uint8_t speed;								//移动速度
    uint8_t dwell;								//停留时间
	uint8_t res[8];
}JBallCruisePoint;

// 巡航参数
#define J_SDK_MAX_BALL_CRUISE_NUM 16
typedef struct __BallCruiseParam
{
	JBallCruisePoint cruise_point[J_SDK_MAX_BALL_CRUISE_NUM];
	JBallCruisePoint res_cruise_point[8];
	uint8_t res[64];
}JBallCruiseParam;
#endif


//=====================================================================================

typedef enum __ParamId
{
	PARAM_START               = -1,
	PARAM_SYNC_TIME           = 3,			//同步服务器时间
	PARAM_CHANGE_DISPATCH     = 4,			//更改流媒体服务器
	PARAM_DEVICE_INFO         = 5,			//设备出厂信息
	PARAM_DEVICE_NTP_INFO     = 6,			//NTP 信息
	PARAM_DEVICE_TIME         = 7,			//设置时间信息(保留)-->扩展: 60,61
	PARAM_PLATFORM_INFO       = 8,			//平台信息
	PARAM_NETWORK_INFO        = 9,			//网络信息
	PARAM_PPPOE_INFOE         = 10,			//PPPOE拨号信息
	PARAM_ENCODE_INFO         = 11,			//编码参数
	PARAM_DISPLAY_INFO        = 12,			//显示参数
	PARAM_RECORD_INFO         = 13,			//录像参数
	PARAM_HIDE_INFO           = 14,			//遮挡参数
	PARAM_SERIAL_INFO         = 15,			//串口参数
	PARAM_OSD_INFO            = 16,			//OSD参数
	PARAM_PTZ_INFO            = 17,			//云台信息
	PARAM_FTP_INFO            = 18,			//ftp 参数
	PARAM_SMTP_INFO           = 19,			//smtp 参数
	PARAM_UPNP_INFO           = 20,			//upnp 参数
	PARAM_DISK_INFO           = 21,			//磁盘信息
	PARAM_FORMAT_DISK         = 22,			//格式磁盘
	PARAM_MOVE_ALARM          = 23,			//移动告警
	PARAM_LOST_ALARM          = 24,			//丢失告警
	PARAM_HIDE_ALARM          = 25,			//遮挡告警
	PARAM_IO_ALARM            = 26,			//IO 告警
	PARAM_JOINT_INFO          = 27,			//联动操作
	PARAM_CONTROL_PTZ         = 28,			//云镜控制
	PARAM_SUBMINT_ALARM       = 29,			//上报告警
	PARAM_MEDIAL_URL          = 30,			//流媒体 URL
	PARAM_STORE_LOG           = 31,			//视频检索
	PARAM_USER_LOGIN          = 32,			//用户登录请求
	PARAM_FIRMWARE_UPGRADE    = 33,			//固件升级
	PARAM_DEVICE_CAP          = 34,			//设备能力集
	PARAM_SEARCH_DEV          = 35,			//局域网设备搜索
	PARAM_CHANNEL_INFO        = 36,			//设备通道配置
	PARAM_PIC_INFO            = 37,			//图像参数
	PARAM_WIFI_INFO           = 38,			//wifi 配置
	PARAM_WIFI_SEARCH         = 39,			//wifi 搜索
	PARAM_USER_CONFIG         = 40,			//用户信息配置JUserConfig
	PARAM_CONTROL_DEV         = 41,			//控制设备
	PARAM_NETWORK_STATUS      = 42,			//网络连接状态
	PARAM_INTEREST_CODING     = 43,			//感兴趣区域编码
	PARAM_DDNS_CODING         = 44,			//动态域名服务
	
	PARAM_DEF_DISPLAY_INFO    = 45,			//默认显示参数
	PARAM_DEF_PICTURE_INFO    = 46,			//默认图像参数
	PARAM_HXHT_CONFIG         = 47,			//互信互通平台JHxhxPfConfig
	PARAM_MODIFY_USER_CFG     = 48,			//修改用户信息JUserModConfig
	PARAM_AVD_CFG             = 49,			//AVD 配置
	PARAM_GB28181_PROTO_CFG   = 50,			//
	PARAM_NETWORK_DISK_CFG    = 51,			//网络磁盘管理
	PARAM_DEV_WORK_STATE      = 52,			//设备状态信息

	PARAM_OPERATION_LOG       = 53,			//获取用户操作记录
	PARAM_ALARM_UPLOAD_CFG    = 54,			//报警上传配置

	PARAM_PRESET_POINT_CFG    = 55,			//预置点配置
	PARAM_CRUISE_POINT_CFG    = 56,			//巡航路径(查询巡航路径号集合，启动、停止、删除某一巡航号)
	PARAM_CRUISE_WAY_CFG      = 57,			//巡航路径(查询、添加、修改某一巡航路径信息)
	PARAM_VENC_AUTO_SWITCH    = 58,			//通道码率自动切换设置
	PARAM_3D_CONTROL          = 59,			//3D 控制
	
	PARAM_SET_DEVICE_TIME     = 60,			//手动设置时间 JDeviceTimeExt
	PARAM_SYNC_CMS_TIME       = 61,			//设置同步cms 时间标记
	PARAM_IR_CUT_CFG          = 62,			//IR-CUT切换模式参数设置项:JIrcutCtrl(海思IPC 私有)

    //63,64已被占用!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	PARAM_ALARM_LINK_IO       = 65,			//联动设备IO 告警
	PARAM_ALARM_LINK_PRESET   = 66,         //联动预置位
	PARAM_RESOLUTION_CFG      = 67,			//解码器分辨率信息
	PARAM_IRCUT_CONTROL       = 68,			//红外控制:JIrcutControl
	PARAM_3D_GOBACK           = 69,         //3D 控制返回原样
	PARAM_EXTRANET_PORT       = 70,         //外网端口
	
# ifdef _USE_DECODER_PROTO_
	PARAM_NOTIFY_DIVISION_MODE= 200,		//通知分屏模式
	PARAM_GET_SCREEN_STATE    = 201,		//获取屏幕状态
	PARAM_CHANGE_DIVISION_MODE= 202,		//改变分屏模式
	PARAM_SET_FULL_SCREEN     = 203,		//设置全屏显示
	PARAM_EXIT_FULL_SCREEN    = 204,		//退出全屏显示
	PARAM_TV_WALL_PLAY        = 205,		//电视墙播放
	PARAM_CLEAR_DIVISION      = 206,		//清除分割显示
# endif //_USE_DECODER_PROTO_

    PARAM_HERD_ANALYSE        = 301,        //牧业分析配置
    PARAM_GRASS_PERCENT       = 302,        //草料百分比


	PARAM_MAX_SIZE,
}JParamId;


//******************************************************
#pragma pack(pop)



#endif //__J_SDK_H__


