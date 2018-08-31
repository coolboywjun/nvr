#ifndef __IPNC_PROTOCOL_H__
#define __IPNC_PROTOCOL_H__
#include <netinet/in.h>

#define	PROCOTOL_REQUEST_MAGIC		0x34ab12cd
#define	PROTOCOL_RESPONSE_MAGIC		0xab34cd12

#define RECV_BUFF_SIZE	4096*2


typedef struct CmdHead
{
	int 	  magic;			// 魔数
	long 	  ipncHandle;		// 标识操作的设备
	long 	  logonHandle;		// 登录设备操作句柄
	int 	  cmd;				// 命令码
	uint32_t  paramCmd;			// 参数命令码:IPNC_CMD_TYPE_E
	int 	  response;			// 是否需要回应
	int 	  result;			// 返回结果
	int 	  cmdDataLen;		// 命令数据大小
}CmdHead;
#define CMD_HAED_MAGIC(cmd)	(cmd->magic)
#define CMD_HAED_CMD(cmd)		(cmd->cmd)
#define CMD_HAED_LEN(cmd)		(cmd->cmdDataLen)
#define CMD_HEAD_RESULT(cmd)	(cmd->result)

enum
{
	// common command
	IPNC_CONNECT = 0,
	IPNC_DISCONNECT,
	
	IPNC_START_SEARCH,
	IPNC_STOP_SEARCH,
	IPNC_GET_SEARCH,
	IPNC_SEARCH_CFG,
	
	IPNC_LOGON,
	IPNC_LOGOUT,

	IPNC_GET_PARAM,
	IPNC_SET_PARAM,
	
	IPNC_START_GET_STREAM,
	IPNC_STOP_GET_STREAM,
	IPNC_REQUEST_IFRAME,
	
	// PARAM command
	IPNC_GET_DEV_INFO = 0x1000,
	IPNC_SET_DEV_INFO,

	// PTZ command
	IPNC_PTZ_CTRL,

	IPNC_TEST_IPC_STATUS,
};

typedef struct
{
	char ipAddr[16];
	int port;
	char user[32];
	char passwd[32];
	int poolCh;
}IpncConnect;

typedef struct
{
	unsigned long ipAddr;
	int port;
}IpncSearch;

typedef struct
{
	int chn;
	int streamNo;
	int poolCh;
	int poolStreamNo;
}IpncChannel;

/**********************************************ipnc结构体信息参数***********************************************/
#define IPNC_SERIALNO_LEN	48	//序列号长度
#define IPNC_COMMON_LEN		64	//长度
#define IPNC_IP_ADDR_LEN	64  //ip地址域名长度
#define IPNC_MAC_ADDR_LEN	6	//mac地址长度
#define	IPNC_USER_NAME_LEN	32	//用户名长度
#define	IPNC_PASS_WORD_LEN	32	//用户密码长度
#define	IPNC_MAX_EMAIL_LEN	64	//邮箱长度

#define IPNC_VMASK_REG_NUM		4	//视频遮挡个数
#define IPNC_VMD_REG_NUM		4	//视频侦测区域数
#define IPNC_MAX_SEG_NUM		4	//最大时间段
#define IPNC_MAX_IRCF_SEG_NUM	4	//红外定时最大时间段
#define IPNC_MAX_OSD_LABEL_NUM 	5	//osd标注的最大个数 


//=======================ptz命令
typedef enum _IPNC_PTZ_CMD_E_
{
	PTZ_CMD_UP				=	0,	//上
	PTZ_CMD_SENSE			=	1,	//
	PTZ_CMD_DOWN			=	2,	//下
	PTZ_CMD_LEFT			=	3,	//左
	PTZ_CMD_RIGHT			=	4,	//右
	PTZ_CMD_LEFT_UP			=	5,	//左上
	PTZ_CMD_LEFT_DOWN		=	6,	//左下
	PTZ_CMD_RIGHT_UP		=	7,	//右上
	PTZ_CMD_RIGHT_DOWN		=	8,	//右下
	PTZ_CMD_AUTOSCAN		=	9,	//自动扫描
	PTZ_CMD_MANUALSCAN		=	10,	//手动扫描
	PTZ_CMD_FOCUS_NEAR		=	11, //聚焦+
	PTZ_CMD_FOCUS_FAR		=	12, //聚焦-
	PTZ_CMD_IRIS_CLOSE		=	13, //光圈-
	PTZ_CMD_IRIS_OPEN		=	14,	//光圈+
	PTZ_CMD_ZOOM_WIDE		=	15,	//变倍+
	PTZ_CMD_ZOOM_TELE		=	16, //变倍-
	PTZ_CMD_STOP			=	17,	//停止

	PTZ_CMD_SET_PRESET		=	18,  //预置位设置
	PTZ_CMD_CLR_PRESET		=	19,	 //预置位清楚
	PTZ_CMD_GOTO_PRESET		=	20,  //预置位调用
	PTZ_CMD_FLIP			=	21,  //翻转
	PTZ_CMD_GOTO_ZEROPAN	=	22,  //零位调用
	PTZ_CMD_SET_AUX			=	23,  //设置辅助开关
	PTZ_CMD_CLR_AUX			=	24,  //清除辅助开关
	PTZ_CMD_REMOTE_RESET	=	25,  //远程恢复
	PTZ_CMD_ZONE_START		=	26,  //设置花样扫描开始
	PTZ_CMD_ZONE_END		=	27,  //设置花样扫描结束
	PTZ_CMD_WR_CHAR			=	28,  //写字符
	PTZ_CMD_CLR_SCR			=	29,  //清楚字符
	PTZ_CMD_ALM_ACK			=	30,  //报警确认
	PTZ_CMD_ZONE_SCAN_ON	=	31,  //开启花样扫描
	PTZ_CMD_ZONE_SCAN_OFF	=	32,  //停止谎言扫描
	PTZ_CMD_PTN_START		=	33,  //
	PTZ_CMD_PTN_STOP		=	34,  //
	PTZ_CMD_RUN_PTN			=	35,  //
	PTZ_CMD_ZOOM_SPEED		=	36,  //变倍速度
	PTZ_CMD_FOCUS_SPEED		=	37,  //聚焦速度
	PTZ_CMD_RESET_CAMERA	=	38,  //相机复位
	PTZ_CMD_AUTO_FOCUS		=	39,  //自动聚焦
	PTZ_CMD_AUTO_IRIS		=	40,  //自动光圈 
	PTZ_CMD_AUTO_AGC		=	41,  //自动增益
	PTZ_CMD_BACKLIGHT_COMP	=	42,	 //
	PTZ_CMD_AUTO_WB		    =	43,  //自动白平衡
	PTZ_CMD_DEV_PHASE_DELAY	=	44,  //
	PTZ_CMD_SET_SHTTER_SPEED=	45,  //设置快门速度
	PTZ_CMD_ADJ_LL_PHASE_DELAY=	46,
	PTZ_CMD_ADJ_WB_RB		=	47,  //调整自动白平衡红蓝
	PTZ_CMD_ADJ_WB_MG		=	48,  //调整自动白平衡
	PTZ_CMD_ADJ_GAIN		=	49,  //调整自动增益
	PTZ_CMD_AUTO_IRIS_LV	=	50,  //调整自动光圈level
	PTZ_CMD_AUTO_IRIS_PEAK	=	51,  //调整自动光圈peak
	PTZ_CMD_QUERY			=	52,  //查询
	
	PTZ_CMD_INVALID			=	53,
	
	PTZ_CMD_SET_CRUISE		 = 	1000,	// 设置巡航扫描
	PTZ_CMD_SET_CRUISE_START =	1001,	// 自动巡航扫描 
	PTZ_CMD_SET_GUARD_POS	 =	1002,	// 设置看守位
	PTZ_CMD_GOTO_GUARD_POS	 =	1003,	// 调用看守位
	PTZ_CMD_SET_AUTOSCAN_SETTING = 1004,	// 设置自动扫描
	PTZ_CMD_SET_AUTOSCAN_START	 = 1005,	// 开启自动扫描 
	PTZ_CMD_SET_ABSCAN_SETTING	 = 1006,	// 设置AB两点扫描
	PTZ_CMD_SET_ABSCAN_LEFT_POS	 = 1007,	// 设置AB两点左限位 
	PTZ_CMD_SET_ABSCAN_RIGHT_POS = 1008,	// 设置AB两点右限位
	PTZ_CMD_SET_ABSCAN_START	 = 1009,	// 开启AB两点扫描
	PTZ_CMD_SET_IDLE_SETTING	 = 1010,	// 设置闲置动作
	PTZ_CMD_SET_ONTIME_SETTING	 = 1011,	// 设置定时动作
	PTZ_CMD_SET_ALARM_SETTING	 = 1012,	// 设置报警联运时的动作
	PTZ_CMD_TRIG_ALARM			 = 1013,	// 报警触发
	PTZ_CMD_CLR_ALARM			 = 1014,
	PTZ_CMD_SET_IR_MODE			 = 1015,	// 红外灯功能
	PTZ_CMD_SET_DAY_NIGHT_MODE	 = 1016,	// 日夜模式
	PTZ_CMD_SET_180_AUTO_FLIP	 = 1017,	// 180自动翻转
	PTZ_CMD_SET_IRIS_MODE		 = 1018,	// 光圈工作模式
	PTZ_CMD_SET_BLC_MODE		 = 1019,	// 背光补偿
	PTZ_CMD_SET_ZOOM_DISPLAY	 = 1020,	// 变位显示
	PTZ_CMD_SET_FRAME_NOISE		 = 1021,	// 帧降噪
	PTZ_CMD_SET_WB_MODE			 = 1022,	// 白平衡模式
	PTZ_CMD_SET_WDR_MODE		 = 1023,	// 宽动态
	PTZ_CMD_SET_AUTO_ZOOM		 = 1024,	// 自动变倍
	PTZ_CMD_SET_3D_CONTROL		 =	1025,	// 设置球机3D控制
	PTZ_CMD_SET_3D_RECT_CENTER	 = 	1026,	// 3D定位框选缩放
	PTZ_CMD_SET_3D_POINT_CENTER	 =	1027,	// 3D定位点击居中
	PTZ_CMD_GET_ZOOM		 = 1050,		// 获取变倍大小
	PTZ_CMD_QUERY_STATUS	 = 1051,		// 查询球机状态
	PTZ_CMD_GET_VERSION_ID	 = 1052,		// 获取高速球版本号
	PTZ_CMD_SET_BRIGHT		 = 1053,		// 设置亮度
	PTZ_CMD_SET_CONTRAST	 = 1054,		// 设置对比度
	PTZ_CMD_SET_SATURATION	 = 1055,		// 设置饱和度
	PTZ_CMD_SET_SHARPNESS	 = 1056,		// 设置锐度
	PTZ_CMD_SET_RED			 = 1057,		// 设置红色
	PTZ_CMD_SET_BLUE		 = 1058,		// 设置蓝色
	PTZ_CMD_GET_SOFT_ID		 = 1059,		// 获取软件ID
	PTZ_CMD_GET_FEATURE_INFO = 1060,		// 获取功能信息
	PTZ_CMD_GET_CAMERA_INFO	 = 1061,		// 获取机芯信息
	PTZ_CMD_GET_FRAME_RATE	 = 1062,		// 获取球机当前帧率
	PTZ_CMD_GET_CAMERA_TYPE	 = 1063,		// 获取球机机芯类型
	
	PTZ_CMD_SET_DEFAULT 	= 	1100,		// 恢复默认出厂设置
	PTZ_CMD_SET_REBOOT		=	1101,		//重启

	PTZ_CMD_DEL_CRUISE		= 	1102,	// 删除巡航扫描 jxj_cu使用
	PTZ_CMD_STOP_CRUISE     =	1103,	// 停止巡航扫描 jxj_cu使用
	
	PTZ_CMD_INVALID_CMD 	= 	1200,		// 无效命令
	
} IPNC_PTZ_CMD_E;

// ========================ipnc protocol描述信息
typedef struct _IPNC_USR_PWD_S
{
	char usr[IPNC_USER_NAME_LEN];
	char pwd[IPNC_PASS_WORD_LEN];
}IPNC_USR_PWD_S;

typedef struct _IPNC_PROTOCOL_S
{
	char name[IPNC_USER_NAME_LEN];			//协议名
	char aliasName[IPNC_USER_NAME_LEN];		// 协议别名
	char factoryName[IPNC_USER_NAME_LEN];	//协议厂家
	IPNC_USR_PWD_S usrPwd[4];
	int dataPort;
	int webPort;
	int rtspPort;	
}IPNC_PROTOCOL_S;

//======================时间结构
typedef struct _IPNC_TIME_S
{
	uint8_t		u8Year;			//年(从1900年开始)
	uint8_t		u8Month;		//月1~12
	uint8_t		u8Day;			//日1~31
	uint8_t		u8WDay;			//星期天0 星期一1.....
	uint8_t		u8Hour;			//时0~23
	uint8_t		u8Minute;		//分0~59
	uint8_t		u8Second;		//秒0~59
	uint8_t		u8Millisec;		//毫妙(0~99)(百分之一秒)
}IPNC_TIME_S, *LPIPNC_TIME_S;

//=======================时间段结构
typedef struct _IPNC_SEGMENT_S
{
	uint32_t 	u8Open:8;		//开关
	uint32_t	beginSec:24;	//开始时间秒数 3600*hour+60*min+sec
	uint32_t	u8Rsvd:8;		//保留
	uint32_t	endSec:24;		//结束时间秒数 3600*hour+60*min+sec
}IPNC_SEGMENT_S, *LPIPNC_SEGMENT_S;

//=======================点结构
typedef struct _IPNC_POINT_S
{
	int		s32X;		//起点x坐标
	int		s32Y;		//起点y坐标
}IPNC_POINT_S, *LPIPNC_POINT_S;

//=======================区域结构
typedef struct _IPNC_RECT_S
{
	int			s32X;			//起点x坐标
	int			s32Y;			//起点y坐标
	uint32_t	u32Width;		//区域宽度
	uint32_t	u32Height;		//区域高度
}IPNC_RECT_S, *LPIPNC_RECT_S;

//=======================告警联动
typedef struct _IPNC_LINKACT_S
{
	uint32_t	u32ShootChannel;	//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
	uint32_t	u32ShootInterval;	//抓拍间隔(毫秒),0:连续抓拍
	uint32_t	u32RecTime;			//录像时间(秒)
	uint32_t	u32RecChannel;		//录像通道(bit) (0 不录像 1第一通道录像)
	uint32_t	u32PCRecChannel;	//联动PC录像通道	
	uint32_t	u32OutPut;			//联动输出(bit)
	uint32_t	u32OutPersist;		//输出持续时间
	uint8_t		u8ShootMode;		//联动抓拍模式,0本地存储，1ftp上传，2email
	uint8_t		u8ShootNum;			//抓拍张数	
	uint8_t		u8RecMode;			//联动录像模式，0本地存储，1ftp上传	
	uint8_t		u8RecPreTime;		//预录时间(秒)
	uint8_t		u8Email;			//联动email
	uint8_t		u8Rsvd[15];
}IPNC_LINKACT_S, *LPIPNC_LINKACT_S;

//设备信息
typedef struct _IPNC_DEV_STABLE_INFO_S_
{
	uint8_t		u8SerialId[IPNC_SERIALNO_LEN];  	//设备Id
	uint8_t		u8DiNum;						//报警输入路数
	uint8_t		u8DoNum;						//报警输出路数
	uint8_t		u8ChnNum  ;						//设备路数
	uint8_t		u8Rs232Num;						//设备232 串口个数
	uint8_t		u8Rs485Num;						//设备485 串口个数
	uint8_t		u8NetCardNum;					//网卡个数
	uint16_t	u16DevType ;					//设备类型						
	uint32_t	u32HwVer;						//硬件版本号
	uint32_t	u32CoreVer;						//内核版本号
	uint32_t	u32SoftVer;						//软件版本号
	uint32_t	u32ResolutionM;					//支持的主码流分辨率大小
	uint32_t	u32ResolutionS;					//支持的次码流分辨率大小	
	uint32_t	u32SensorType;					//前端sensor 类型
	uint8_t		u8Res2[12];	
}IPNC_DEV_STABLE_INFO_S, *LPIPNC_DEV_STABLE_INFO_S;

typedef struct _IPNC_DEV_VAR_INFO_S
{
	char		szDevName[IPNC_USER_NAME_LEN];  	//设备名称
	uint8_t		u8Language;						//语言0 中文1英文
	uint8_t		u8PalStand;						//制式1 pal 0 ntfc
	uint8_t		u8Hz;							//电源频率0 50Hz 1 60HZ
	uint8_t		u8Res[17];
}IPNC_DEV_VAR_INFO_S;

typedef struct _IPNC_DEV_CFG_S_
{
	char serialId[IPNC_SERIALNO_LEN];	//设备ID,序列号
	char hwVer[IPNC_COMMON_LEN];		//设备硬件版本
	char codeVer[IPNC_COMMON_LEN];		//设备内核版本
	char sofrVer[IPNC_COMMON_LEN];		//设备软件版本
	
	IPNC_DEV_VAR_INFO_S stVarInfo;		//系统设备可修改信息

	uint32_t di_num;							//设备输入接口数目
	uint32_t do_num;							//设备输出接口数目
	uint32_t channel_num;						//通道数目
	uint32_t rs485_num;							//RS485接口数目，若0，则无
	uint32_t rs232_num;							//RS232接口数目，若0，则无
	
	uint8_t uRes[12];
}IPNC_DEV_CFG_S, *LPIPNC_DEV_CFG_S;

//网络信息
typedef struct _IPNC_NET_CFG_S_
{
	char  ipAddr[IPNC_IP_ADDR_LEN];	//IP地址
	char  subMask[IPNC_IP_ADDR_LEN];//子网掩码
	char  gateWay[IPNC_IP_ADDR_LEN];//网关
	char  macAddr[IPNC_MAC_ADDR_LEN];//MAC地址
	char  dnsIp1[IPNC_IP_ADDR_LEN];	//DNS地址1
	char  dnsIp2[IPNC_IP_ADDR_LEN];	//DNS地址2
	uint16_t u16HttpPort;			//http 端口
	uint16_t u16NetPort;			//通信端口
	
	uint8_t uRes[32];
}IPNC_NET_CFG_S, *LPIPNC_NET_CFG_S;

//用户信息
typedef struct _IPNC_USER_CFG_S_
{
	char 	userName[IPNC_USER_NAME_LEN];	//原用户名
	char 	passWord[IPNC_PASS_WORD_LEN];	//源密码
	//char 	newUserName[IPNC_USER_NAME_LEN];//新用户名
	//char 	newPassWord[IPNC_PASS_WORD_LEN];//新密码
	char 	cfmPassWord[IPNC_PASS_WORD_LEN];//确认新密码
	uint8_t		u8Res[6];
}IPNC_USER_CFG_S, *LPIPNC_USER_CFG_S;

//ftp信息
typedef struct _IPNC_FTP_CFG_S_
{
	char 	 ftpSvr[IPNC_IP_ADDR_LEN]; 	//主ftp服务器ip
	char  	 ftpUsr[IPNC_USER_NAME_LEN];//登陆用户名
	char	 ftpPsw[IPNC_PASS_WORD_LEN];//登陆密码
	char	 ftpPath[IPNC_COMMON_LEN];  //Ftp上传路径
	uint16_t u16FtpPort;				//ftp端口

}IPNC_FTP_CFG_S, *LPIPNC_FTP_CFG_S;

//smtp信息
typedef struct _IPNC_SMTP_CFG_S_
{
	char 	 mailHub[IPNC_IP_ADDR_LEN]; 	//smtp 服务器地址
	char     mailAddr[IPNC_MAX_EMAIL_LEN];	//发送邮件 地址
	char  	 mailUsr[IPNC_MAX_EMAIL_LEN]; 	//发送邮件 账号
	char 	 mailPsw[IPNC_PASS_WORD_LEN];  	//发送邮件 密码
	char 	 mailRctp1[IPNC_MAX_EMAIL_LEN];	//接受邮件 地址1
	char 	 mailRctp2[IPNC_MAX_EMAIL_LEN];	//接受邮件 地址2
	char  	 mailRctp3[IPNC_MAX_EMAIL_LEN];	//接受邮件 地址3
	uint16_t u16MailPort;					//smtp 服务器端口
	
}IPNC_SMTP_CFG_S, *LPIPNC_SMTP_CFG_S;

//ntp信息
typedef struct _IPNC_NTP_CFG_S_
{
	uint8_t	 enable;						//ntp 开关
	char   	 ntpServer[IPNC_IP_ADDR_LEN];	//ntp服务器
	int	  	 timeZone;						//时区 单位s(-12*3600~11*3600)
	int	  	 euroTime;						//==1:启动夏令时 0:不启动夏令时
	uint8_t	 ntpRefTime;					//ntp 定时对时时间(单位小时)
	//uint16_t u16NtpPort;					//端口
}IPNC_NTP_CFG_S, *LPIPNC_NTP_CFG_S;

//upnp信息
typedef struct _IPNC_UPNP_CFG_S_
{
	uint32_t	u32Size;
	uint8_t		u8UpnpOpen;			//upnp 开关
	uint8_t		u8UpnpEthNo;		//0使用本机网卡映射 1 使用无线网卡映射
	uint8_t		u8UpnpModel;    	//0固定映射 1自动映射
	uint8_t		u8UpnpRefTime;  	//upnp 刷新时间(单位小时)
	char        szUpnpSvr[IPNC_IP_ADDR_LEN];
	uint16_t	u16UpnpDataPort;	//固定映射模式下, 数据端口的外网端口
	uint16_t	u16UpnpWebPort; 	//固定映射模式下, Web端口的外网端口
	uint16_t	u16UpnpRtspPort;	//固定映射模式下, RTSP端口的外网端口
	uint16_t	u16UpnpDataPortS;	//数据端口的外网端口 0代表没有映射或不成功
	uint16_t	u16UpnpWebPortS;	//Web端口的外网端口 0代表没有映射或不成功
	uint16_t	u16UpnpRtspPortS;	//RTSP端口的外网端口 0代表没有映射或不成功
	uint16_t	u16UpnpCmdPort;
	uint16_t	u16UpnpTalkPort; 
	uint16_t	u16UpnpJXJDataPort;
	uint16_t	u16UpnpCmdPortS;
	uint16_t	u16UpnpTalkPortS;
	uint16_t	u16UpnpJXJDataPortS;//JXJ平台数据端口的外网端口映射状态
	uint8_t		u8Res[20];

}IPNC_UPNP_CFG_S, *LPIPNC_UPNP_CFG_S;

//ddns信息
typedef struct _IPNC_DDNS_CFG_S_
{
	uint8_t		u8DDNSOpen;						//DDNS开关
	uint8_t		u8DDNSType;						//0-dyndns 1-3322
	uint16_t	u16DDNSPort;						//DDNS服务器端口
	char		szDDNSAccount[IPNC_USER_NAME_LEN];	//DDNS 注册名
	char		szDDNSUsr[IPNC_USER_NAME_LEN];		//用户名称
	char		szDDNSPsw[IPNC_PASS_WORD_LEN];		//用户密码	
	uint32_t	u32DDNSTimes;						//更新时间
}IPNC_DDNS_CFG_S, *LPIPNC_DDNS_CFG_S;

//PPPOE信息
typedef struct _IPNC_PPPOE_CFG_S_
{
	//uint8_t		u8PppoeOn;					//pppoe开关
	//uint8_t		u8PppoeStatus;				//pppoe 拨号状态0 没有拨号1 拨号中2拨号成功
	char		pppoeUsr[IPNC_USER_NAME_LEN];	//pppoe用户名
	char		pppoePsw[IPNC_PASS_WORD_LEN];	//pppoe密码
	char 		pppoeIp[IPNC_IP_ADDR_LEN];		//pppoe获取的ip
	//uint32_t	u32PppoeTimes;					//pppoe拨号时间
	uint8_t		u8Res[32];
}IPNC_PPPOE_CFG_S, *LPIPNC_PPPOE_CFG_S;

//COMM口信息
typedef struct _IPNC_COMM_CFG_S_
{
	uint32_t	u32BaudRate;	//波特率(bps)，0-50，1-75，2-110，3-150，4-300，
								//s5-600，6-1200，7-2400，8-4800，9-9600，
								//10-19200，11-38400，12-57600，13-76800，14-115.2k 
 	uint8_t     u8DataBit;		//数据位：0-5位，1-6位，2-7位，3-8位
 	uint8_t     u8StopBit;		//停止位：0-1位，1-2位
  	uint8_t     u8Parity;		//是否校验：0-无校验，1-奇校验，2-偶校验
  	uint8_t     u8Flowcontrol;	//是否流控：0-无，1-软流控,2-硬流控
  	uint32_t	u32BaudMask;
  	uint8_t		u8Res[4];
}IPNC_COMM_CFG_S, *LPIPNC_COMM_CFG_S;

//云台信息
typedef struct _IPNC_PTZ_CFG_S_
{
	uint8_t		u8ProtocalType;	// 0 pelco-d 1 pelco-p
	uint8_t		u8PtzAddr;		//0-255
	uint8_t		u8Res[6];
}IPNC_PTZ_CFG_S, *LPIPNC_PTZ_CFG_S;

//音频信息
typedef struct _IPNC_AUDIO_CFG_S_
{
	//uint32_t	u32Size;
	uint32_t	u32SampleRate;	//采样率 8k 80000
	uint8_t		u8Open;			//是否开启音频
	uint8_t		u8AEncType;		//0--PCM 1--G711A 2-G711U 3--G726
	uint8_t		u8Type;      	//0--mic麦克输入 1--线输入
	uint8_t		u8Res[5];
}IPNC_AUDIO_CFG_S, *LPIPNC_AUDIO_CFG_S;

//视频信息///////////////////////
//抓拍参数信息
typedef struct _IPNC_DEV_CAPTURE_CFG_S_
{
  uint16_t	u16Width;		//抓拍图片宽度
  uint16_t	u16Height;		//抓拍图片高度
  uint8_t	u8VideoEncType;	//视频编码格式 2--JPEG 固定值
  uint8_t	u8Quilty;		//抓拍质量 
  uint8_t	u8Res[14];	
  
}IPNC_DEV_CAPTURE_CFG_S, *LPIPNC_DEV_CAPTURE_CFG_S;

//视频编码参数	
typedef struct _IPNC_DEV_VENC_CFG_S_
{
  uint16_t	u16Width;		//视频编码宽度
  uint16_t	u16Height;		//视频编码高度

  uint8_t	u8VideoEncType;	//视频编码格式0--h264 1--MJPEG 2--JPEG	
  uint8_t	u8PicQuilty;	//编码质量0-最好，1-次好，2-较好，3-一般，4-较差，5-差	
  uint8_t	u8Gop;			//I帧间隔1-200
  uint8_t	u8FlowType;		//码率类型 0--定码流1--变码流
  uint32_t	u32BitrateRate;	//编码码率 32kbps-16Mkbps
  uint8_t	u8FrameRate;	//编码帧率1-25/30
  uint8_t	u8Res[7];
  
}IPNC_DEV_VENC_CFG_S, *LPIPNC_DEV_VENC_CFG_S;

//视频遮挡区域
typedef struct _IPNC_DEV_VMASK_CFG_S_
{
  uint8_t		u8Open;							//--1.启动视频遮挡0--屏蔽视频遮挡
  uint8_t		u8Res1[3];
  IPNC_RECT_S	struArea[IPNC_VMASK_REG_NUM];	//视频遮挡区域
  uint8_t		u8Res2[8];
}IPNC_DEV_VMASK_CFG_S, *LPIPNC_DEV_VMASK_CFG_S;

//增加5个信息标注
typedef struct _IPNC_DEV_OSD_LABEL_CFG_S_
{
	uint32_t		u32EnableLabel[IPNC_MAX_OSD_LABEL_NUM];					//--1 显示标题osd 0--隐藏标题osd
	char		szLabelName[IPNC_MAX_OSD_LABEL_NUM][IPNC_USER_NAME_LEN];			//标注名称
	uint32_t		u32ColorValidFlag[IPNC_MAX_OSD_LABEL_NUM];				// 0:使用u3Osdcolor,使用u32LabelColor
  	uint32_t		u32OsdColor[IPNC_MAX_OSD_LABEL_NUM];						//----0 ---black
  																		//---1---white
  																		//---2---blue
  																		//---3---red
  																		//---4---inv
	IPNC_POINT_S 	struLabelOrg[IPNC_MAX_OSD_LABEL_NUM];						//标注osd位置
	uint32_t		u32LabelColor[IPNC_MAX_OSD_LABEL_NUM];					// 标注颜色,红色:0-7bit, 绿色:8-15bit,蓝色:16-23bit
}IPNC_DEV_OSD_LABEL_CFG_S, *LPIPNC_DEV_OSD_LABEL_CFG_S;


//osd菜单设置 暂不支持用户自定义osd 菜单
typedef struct _IPNC_DEV_OSD_CFG_S_
{
  uint8_t		u8EnableTitleOsd;	//--1 显示标题osd 0--隐藏标题osd
  uint8_t		u8EnableTimeOsd;	//--1 显示时间osd 0--隐藏时间osd
  uint8_t		u8DispWeek;			//--1 显示星期0--隐藏星期
  uint8_t		u8EnableBitrateOsd;	//--1显示码流osd  0--隐藏码流osd
  uint8_t		u8TimeOsdType;		//--时间显示类型
  									//--0 YYYY-MM-DD HH:MM:SS
  									//--1 MM-DD-YYYY HH:MM:SS
  									//--2 DD-MM-YYYY HH:MM:SS
									//--3 YYYY/MM/DD HH:MM:SS
									//--4 MM/DD/YYYY HH:MM:SS
									//--5 DD/MM/YYYY HH:MM:SS
									
  uint8_t		u8HourOsdType;		//--0 24 hour --1 12 hour
  uint8_t		u8OsdColor;			//----0 ---black
  									//---1---white
  									//---2---blue
  									//---3---yellow
  uint8_t		u8EnableUsrOsd; //bit0 使能第一个用户菜单
  								//bit1 使能第二个用户菜单

  IPNC_POINT_S 	struTimeOrg;	//时间osd位置
  IPNC_POINT_S 	struTitleOrg;	//标题osd位置
  IPNC_POINT_S 	struUsrOrg[2];	//用户自定义osd位置
  char			szChnName[IPNC_USER_NAME_LEN];		 	//通道名称
  char			szUsrOsdInfo[2][IPNC_USER_NAME_LEN];	//用户自定义osd信息
  
  uint32_t		u32TimeColor;		// 时间颜色,红色:0-7bit, 绿色:8-15bit,蓝色:16-23bit
  uint32_t		u32ChnColor;		// 通道颜色,红色:0-7bit, 绿色:8-15bit,蓝色:16-23bit
  uint8_t		u8ColorValidFlag;	// 0:使用u8Osdcolor初始化u32TimeColor和u32ChnColor值, 1:直接使用u32TimeColor和u32ChnColor值
  uint8_t		u8Res[7];
}IPNC_DEV_OSD_CFG_S, *LPIPNC_DEV_OSD_CFG_S;

//编码等级
typedef enum _IPNC_DEV_H264_ENCODE_LEVEL_E_
{
	IPNC_H264_HIGH = 0,
	IPNC_H264_MAIN,
	IPNC_H264_BASELINE,
}IPNC_DEV_H264_ENCODE_LEVEL_E;


//图像参数设置
typedef struct _IPNC_DEV_IMA_PARA_S_
{
	uint8_t	u8Value;			//参数值
	uint8_t	u8Default;			//缺省参数值
	uint8_t	u8Step;				//参数值步进值
	uint8_t	u8Res;
}IPNC_DEV_IMA_PARA_S;

//视频参数设置类型定义
typedef enum _IPNC_DEV_IMA_MASK_E_
{
	IPNC_VCT_IMA_BRIGHTNESS 	= 0x00000001,
	IPNC_VCT_IMA_CONTRAST		= 0x00000002,
	IPNC_VCT_IMA_SATURATION 	= 0x00000004,
	IPNC_VCT_IMA_HUE		  	= 0x00000008,
	IPNC_VCT_IMA_GAMMA			= 0x00000010,
	IPNC_VCT_IMA_SHARPNESS		= 0x00000020,
	IPNC_VCT_IMA_BLUE			= 0x00000040,
	IPNC_VCT_IMA_RED			= 0x00000080,
	IPNC_VCT_IMA_ALL			= 0xFFFFFFFF,
}IPNC_DEV_IMA_MASK_E;

//图像信息
typedef struct _IPNC_DEV_IMA_CFG_S_
{	
/*有效支持的参数，该参数设置时候无效
在获取的时候有效，用以判断设备是否支持该参数*/	
	IPNC_DEV_IMA_MASK_E	eSuppMask;
/*设置有效的参数，该参数获取时候无效
   在设置的时候有效，用以指定具体设置的参数*/	
	IPNC_DEV_IMA_MASK_E	eSetMask;	

	IPNC_DEV_IMA_PARA_S	struBrightness;	//亮度
	IPNC_DEV_IMA_PARA_S	struContrast;	//对比度
	IPNC_DEV_IMA_PARA_S	struSaturation;	//饱和度
	IPNC_DEV_IMA_PARA_S	struHue;		//色度
	IPNC_DEV_IMA_PARA_S	struGamma;		//伽马gamma
	IPNC_DEV_IMA_PARA_S	struSharpness;	//锐度sharpness
	IPNC_DEV_IMA_PARA_S	struRed;		//red
	IPNC_DEV_IMA_PARA_S	struBlue;		//blue	
	IPNC_DEV_IMA_PARA_S	struRes[24];
}IPNC_DEV_IMA_CFG_S, *LPIPNC_DEV_IMA_CFG_S;

//视频信息
typedef struct _IPNC_DEV_VIDEO_CFG_S_
{
	uint32_t				u32Size;	
	IPNC_DEV_OSD_CFG_S 		struOsd;
	IPNC_DEV_VMASK_CFG_S	struVMask;
	IPNC_DEV_VENC_CFG_S 	struMainVenc;
	IPNC_DEV_VENC_CFG_S 	struSubVenc;
	IPNC_DEV_CAPTURE_CFG_S  struCapture;
	IPNC_DEV_IMA_CFG_S		struIma;
	IPNC_DEV_H264_ENCODE_LEVEL_E	eEncodeLevel;	// 0:high profile, 1:main profile, 2:baseline
	uint8_t   u8PalStand;		//制式1 pal 0 ntfc
	uint8_t					u8Res[27];
}IPNC_DEV_VIDEO_CFG_S, *LPIPNC_DEV_VIDEO_CFG_S;

//移动侦测信息
typedef struct _IPNC_DEV_MD_CFG_S_
{
  uint32_t			u32Size;
  uint8_t			u8Enable;	//是否开启移动侦测 0--关闭 1--开启			
  uint8_t			u8Level;	//侦测灵敏度 0--很高 1--高 2--中 3--低 4--很低	
  uint8_t			u8Res1[2];
  IPNC_RECT_S		struRect[IPNC_VMD_REG_NUM];		//侦测区域
  IPNC_SEGMENT_S	struSeg[IPNC_MAX_SEG_NUM][8];  	//0--每天1--星期天 2--星期一7--星期六	
  IPNC_LINKACT_S	struLinkAct;					//移动侦测联动消息
  uint32_t			u32DetectInterval;				//检测间隔时间
  uint8_t			u8Res2[12];
}IPNC_DEV_MD_CFG_S, *LPIPNC_DEV_MD_CFG_S;

//探头报警信息
typedef struct _IPNC_DEV_DI_CFG_S_
{
  uint32_t			u32Size;
  char				chDiName[IPNC_USER_NAME_LEN];//di探头名称
  uint8_t			u8Enable;					 //是否开启di报警检测 0--不开启 1--开启
  uint8_t			u8DiType;					 //0--常开探头1--常闭探头
  uint8_t			u8Res1[2];
  IPNC_SEGMENT_S	struSeg[IPNC_MAX_SEG_NUM][8]; 	//0--每天1--星期天 2--星期一7--星期六	
  IPNC_LINKACT_S	struLinkAct;					//di报警联动消息
  uint32_t			u32DetectInterval;				//检测间隔时间
  uint8_t			u8Res2[12];
}IPNC_DEV_DI_CFG_S, *LPIPNC_DEV_DI_CFG_S;

//3A参数
//--3A参数设置类型定义
typedef enum _IPNC_DEV_3A_MASK_E_
{	//光圈
	IPNC_VCT_3A_AUTOIRIS	= 0x00000001,//自动光圈
	IPNC_VCT_3A_IRISLEVEL	= 0X00000002,//光圈电平
	//聚焦
	IPNC_VCT_3A_AUTOFOCUS	= 0X00000004,//自动聚焦
	IPNC_VCT_3A_FOCUSLEVEL	= 0X00000008,//焦距
	IPNC_VCT_3A_MINFOCUSLEN = 0X00000010,//设置最小聚焦距离		
	//变倍
	IPNC_VCT_3A_ZOOMSPEED	 = 0X00000020,//变倍速度
	IPNC_VCT_3A_AUTOPTZSPEED = 0X00000040,//景深比例变速
	//电平控制
	IPNC_VCT_3A_AUTOALC		= 0X00000080,//自动电平控制
	IPNC_VCT_3A_ALCLEVEL	= 0X00000100, //电平控制值
	//彩转黑
	IPNC_VCT_3A_CTB			= 0X00000200,//彩转黑
	IPNC_VCT_3A_CTBLEVEL	= 0X00000400,//彩转黑阀值，当彩转黑为自动时有效
	//场景
	IPNC_VCT_3A_MIRROR		= 0X00000800,//镜向
	IPNC_VCT_3A_FLIP		= 0X00001000,//翻转
	IPNC_VCT_3A_AUTOFLIP	= 0X00002000,//自动翻转
	IPNC_VCT_3A_PWDFREQ		= 0X00004000,//照明频率1
	//红外
	IPNC_VCT_3A_IREXIST		= 0X00008000,//是否有红外
	IPNC_VCT_3A_IRCFMODE	= 0X00010000,//IRCF模式
	IPNC_VCT_3A_IRLIGHTTYPE	= 0X00020000,//红外灯类型
	//
	IPNC_VCT_3A_WDR 		= 0X00040000,//是否有宽动态
	IPNC_VCT_3A_WDRLEVEL	= 0X00080000,//宽动态的值
	IPNC_VCT_3A_LOWILLUM	= 0X00100000,//低照度
	IPNC_VCT_3A_IMAGEMODE	= 0X00200000,//图像模式
	//场景
	IPNC_VCT_3A_SCENE		=  0X00400000,//场景
	
	IPNC_VCT_3A_ALL		  	= 0xFFFFFFFF,
}IPNC_DEV_3A_MASK_E;
typedef struct _IPNC_DEV_3A_PARA_S_
{
	uint8_t	u8Value;		//参数值
	uint8_t	u8Default;		//缺省参数值
	uint8_t	u8Step;			//参数值步进值
	uint8_t	u8Res;
}IPNC_DEV_3A_PARA_S;

typedef struct _IPNC_DEV_3A_CFG_S_
{
	uint32_t			u32Size;
/*有效支持的参数，该参数设置时候无效
在获取的时候有效，用以判断设备是否支持该参数*/	
	IPNC_DEV_3A_MASK_E	eSuppMask;
/*设置有效的参数，该参数获取时候无效
   在设置的时候有效，用以指定具体设置的参数*/	
	IPNC_DEV_3A_MASK_E	eSetMask;	
	IPNC_DEV_3A_PARA_S	struFlip;			//翻转图像上下翻转
	IPNC_DEV_3A_PARA_S	struMirror;			//镜像图像左右翻转
	IPNC_DEV_3A_PARA_S	struPwdFreq;		//电源频率
	IPNC_DEV_3A_PARA_S	struScene;			//场景,0:自动 1:室内2:室外
	//IPNC_DEV_3A_PARA_S	struIrcfMode;		//红外模式，0:自动，1:白天，2:晚上,3:定时模式
	IPNC_DEV_3A_PARA_S	struShutterSpeed;	//电子快门 0-8  0：自动快门 1：
											//1/25S， 2：1/50S， 3：1/100S， 4：1/500S， 5：1/1000S， 
											//6：1/2000S， 7：1/5000S， 8：1/10000S
	IPNC_DEV_3A_PARA_S	struRes[26];
}IPNC_DEV_3A_CFG_S, *LPIPNC_DEV_3A_CFG_S;

/*搜索响应包*/
typedef struct _IPNC_NET_SEARCH_RES_S_
{
	struct sockaddr_in	addrClient;
	
	uint8_t	u8SerialId[IPNC_SERIALNO_LEN];
	uint32_t	u32SoftVer;
	uint16_t	u16DevType;
	uint16_t	u16DevPort;
	uint32_t	u32DevIp;
	uint8_t	u8MacAddr[IPNC_MAC_ADDR_LEN];
	uint8_t	u8Dns;
	uint8_t	u8DHCP;	
	uint32_t	u32DevIpMask;
	uint32_t	u32DevGwAddr;
	uint32_t	u32DevDns1;
	uint32_t	u32DevDns2;

	/* 不同协议的私有包*/
	uint8_t   szPriData[1024];
}IPNC_NET_SEARCH_RES_S, *LPIPNC_NET_SEARCH_RES_S; 

//搜索配置包
typedef struct _IPNC_NET_SEARCH_CFG_REQ_S
{	
	char szUsrName[IPNC_USER_NAME_LEN];
	char szPsw[IPNC_PASS_WORD_LEN];
	
	uint8_t	u8SerialId[IPNC_SERIALNO_LEN];
	uint32_t	u32SoftVer;
	uint16_t	u16DevType;
	uint16_t	u16DevPort;
	uint32_t	u32DevIp;
	uint8_t	u8MacAddr[IPNC_MAC_ADDR_LEN];
	uint8_t	u8Dns;
	uint8_t	u8DHCP; 
	uint32_t	u32DevIpMask;
	uint32_t	u32DevGwAddr;
	uint32_t	u32DevDns1;
	uint32_t	u32DevDns2;
	
	/* 不同协议的私有包*/
	uint8_t   szPriData[1024];
}IPNC_NET_SEARCH_CFG_REQ_S, *LPIPNC_NET_SEARCH_CFG_REQ_S; 

#if 0
//搜索结构体信息
/*搜索请求响应包*/
typedef struct _IPNC_NET_SEARCH_RES_S_
{	
	struct sockaddr_in	addrClient;
	uint32_t 	u32Capability0;
	uint32_t 	u32Capability1;
	uint32_t 	u32Capability2;
	uint8_t		u8MainDevType;
	uint8_t		u8SubDevType;
	uint16_t  	u16PlateformCap;  //能够支持平台，应用层填充
	uint32_t  	u32UpTime;        //启动时间
	
	uint8_t		u8SerialId[IPNC_SERIALNO_LEN];
	uint32_t	u32SoftVer;
	uint16_t	u16DevType;
	uint16_t	u16DevPort;
	uint32_t	u32DevIp;
	uint8_t		u8MacAddr[IPNC_MAC_ADDR_LEN];
	uint8_t		u8Dns;
	uint8_t		u8DHCP;	
	uint32_t	u32DevIpMask;
	uint32_t	u32DevGwAddr;
	uint32_t	u32DevDns1;
	uint32_t	u32DevDns2;
	/*uint32_t 	u32Capability0;
	uint32_t 	u32Capability1;
	uint32_t 	u32Capability2;
	uint8_t		u8MainDevType;
	uint8_t		u8SubDevType;
	uint16_t  	u16PlateformCap;  //能够支持平台，应用层填充
	uint32_t  	u32UpTime;        //启动时间*/

}IPNC_NET_SEARCH_RES_S, *LPIPNC_NET_SEARCH_RES_S; 

//搜索配置包
typedef struct _IPNC_NET_SEARCH_CFG_REQ_S
{	
	uint32_t	u32CliMultiIp;
	uint16_t	u16CliMultiPort;
	uint8_t		u8Res[2];
	uint32_t 	u32Capability0;
	uint32_t 	u32Capability1;
	uint32_t 	u32Capability2;	  //实际开启平台,syscfg获取
	uint8_t		u8MainDevType;
	uint8_t		u8SubDevType;
	uint16_t  	u16PlateformCap;  //能够支持平台，应用层填充
	uint32_t  	u32UpTime;        
	char 		szUsrName[IPNC_USER_NAME_LEN];
	char 		szPsw[IPNC_PASS_WORD_LEN];
	uint8_t		u8SerialId[IPNC_SERIALNO_LEN];
	uint32_t	u32SoftVer;
	uint16_t	u16DevType;
	uint16_t	u16DevPort;
	uint32_t	u32DevIp;
	uint8_t		u8MacAddr[IPNC_MAC_ADDR_LEN];
	uint8_t		u8Dns;
	uint8_t		u8DHCP;	
	uint32_t	u32DevIpMask;
	uint32_t	u32DevGwAddr;
	uint32_t	u32DevDns1;
	uint32_t	u32DevDns2;
	/*uint32_t 	u32Capability0;
	uint32_t 	u32Capability1;
	uint32_t 	u32Capability2;	  //实际开启平台,syscfg获取
	uint8_t		u8MainDevType;
	uint8_t		u8SubDevType;
	uint16_t  	u16PlateformCap;  //能够支持平台，应用层填充
	uint32_t  	u32UpTime;        //启动时间*/
}IPNC_NET_SEARCH_CFG_REQ_S, *LPIPNC_NET_SEARCH_CFG_REQ_S; 
#endif

//定时红外模式设置
typedef struct _IPNC_DEV_IRCFMODE_CFG_S_
{
	uint32_t 			u32Size;
	uint8_t			u8Mode;			// 0:auto 1:day 2:night 3:timer
	IPNC_SEGMENT_S	struSeg[IPNC_MAX_IRCF_SEG_NUM][8];  //0--每天1--星期天 2--星期一7--星期六	
	uint8_t			u8Res1[32];
}IPNC_DEV_IRCFMODE_CFG_S;

// ==========================================================
// 高速球配置
//
#define IPNC_BALL_ONTIME_NUM 8
// 报警参数
typedef struct _IPNC_BALL_ALARM_PARAM_S_
{
	int nAlarmActionIndex;		// 报警动作
	int nDurationSec;			// 报警持续时间
	uint8_t byRes[32];
}IPNC_BALL_ALARM_PARAM_S, *LPIPNC_BALL_ALARM_PARAM_S;
// 定时动作
typedef struct _IPNC_BALL_ONTIME_ACTION_S_
{
	uint8_t byBeginHour;					// 开始小时
	uint8_t byBeginMin;					// 开始分钟
	uint8_t byEndHour;					// 结束小时
	uint8_t byEndMin;						// 结束分钟
	int nActionIndex;
	uint8_t byRes[12];
}IPNC_BALL_ONTIME_ACTION_S, *LPIPNC_BALL_ONTIME_ACTION_S;
// 定时参数
typedef struct _IPNC_BALL_ONTIME_PARAM_S_
{
	IPNC_BALL_ONTIME_ACTION_S struOntimeAction[IPNC_BALL_ONTIME_NUM];
	uint8_t byRes[32];
}IPNC_BALL_ONTIME_PARAM_S, *LPIPNC_BALL_ONTIME_PARAM_S;
// 巡航点
typedef struct _IPNC_BALL_CRUISE_NODE_S_
{
	uint8_t byCruisePresetEnable;		// 是否使用这个预置点 
	uint8_t byCruisePresetSec;		// 停留时长(秒)
	uint8_t byCruisePresetSpeed;		// 移动速度
	uint8_t byRes[8];
}IPNC_BALL_CRUISE_NODE_S, *LPIPNC_BALL_CRUISE_NODE_S;
// 巡航参数
#define IPNC_BALL_CRUISE_NUM 16
#define IPNC_BALL_CRUISE_NAME_LEN 32
typedef struct _IPNC_BALL_CRUISE_PARAM_S_
{
	IPNC_BALL_CRUISE_NODE_S struCruiseNode[IPNC_BALL_CRUISE_NUM];
	IPNC_BALL_CRUISE_NODE_S struCruiseResNodE[8];
	/* begin:	add by hjl	2013/1/7*/
	char  crz_name[IPNC_BALL_CRUISE_NAME_LEN];	//巡航路径名
	unsigned int crz_no;						//巡航路径号
	unsigned int crz_enable;					//是否使用这条路径
	/* end:	add by hjl	2013/1/7*/
	uint8_t byRes[64-40];
}IPNC_BALL_CRUISE_PARAM_S, *LPIPNC_BALL_CRUISE_PARAM_S;
// 参数设置
#define DAY_NIGHT_MASK		(1<<0)
#define IR_MASK				(1<<1)
#define FLIP_MASK			(1<<2)
#define ELEC_ZOOM_MASK		(1<<3)
#define FOCUS_MASK			(1<<4)
#define IRIS_MASK			(1<<5)
#define BLC_MASK			(1<<6)
#define ZOOM_DISPLAY_MASK	(1<<7)
#define FRAME_NOISE_MASK	(1<<8)
#define ZOOM_SPEED_MASK		(1<<9)
#define WB_MASK				(1<<10)
#define WDR_MASK			(1<<11)
#define ZOOM_MASK			(1<<12)
typedef struct _IPNC_BALL_SETTING_PARAM_S_
{
	uint32_t dwMask;				// 0: 不支持,1:支持
								// bit0:日夜模式
								// bit1:红外模式
								// bit2:翻转
								// bit3:电子变倍
								// bit4:聚焦
								// bit5:光圈
								// bit6:背光补偿
								// bit7:变倍显示
								// bit8:帧降噪
								// bit9:变倍速度
								// bit10:白平衡
								// bit11:宽动态
	IPNC_BALL_CRUISE_PARAM_S struCruiseParam;
	uint8_t byAutoScanSpeed;			// 自动扫描速度
	uint8_t byABScanSpeed;			// AB扫描速度
	int nIdleActionIndex;			// 闲置动作
	uint8_t byIdleSec;				// 闲置秒数
	uint8_t byIRISMode;				// 光圈模式:0:手动,1:自动
	uint8_t byAutoFlip;				// 自动翻转:0:关闭,1:打开
	uint8_t byDayNightMode;			//0:白天模式,1:夜晚模式,2:自动模式,3:定时模式
	uint8_t byIRMode;					// 远近灯切换模式:0:自动切换远近灯,1:同时开启远近灯
	uint8_t byBLCMode;				// 背光补偿:0:关闭,1:打开
	uint8_t byFrameNoise;				// 降噪:0:关闭,1:打开
	uint8_t byZoomDisplay;			// 变倍显示:0:关闭,1:打开
	uint8_t byZoomSpeed;				// 变倍速度:0:快速,1:中速,2:低速
	uint8_t byWBMode;					// 白平衡:0:室外,1:室内 ,2:自动,3:手动
	uint8_t byWDRMode;				// 宽动态:0:关闭,1:打开
	uint8_t byAutoFocus;				// 自动聚焦:0:关闭,1:打开
	uint8_t byRes[32];
}IPNC_BALL_SETTING_PARAM_S, *LPIPNC_BALL_SETTING_PARAM_S;
// 高速球配置
typedef struct _IPNC_BALL_RUN_PARAM_
{
	uint32_t dwSize;
	IPNC_BALL_ALARM_PARAM_S struAlarmParam;
	IPNC_BALL_ONTIME_PARAM_S struOntimeParam;
	IPNC_BALL_SETTING_PARAM_S struSettingParam;
	uint8_t byRes[64];
}IPNC_BALL_RUN_PARAM_S, *LPIPNC_BALL_RUN_PARAM_S;

//预置点
#define IPNC_MAX_PRESET_PORT_SIZE 	255
#define IPNC_MAX_PRESET_NAME_LEN		64
typedef struct _IPNC_PRESET_POINT_S
{
	uint8_t		name[IPNC_MAX_PRESET_NAME_LEN];	//预置点名称
	uint32_t	preset;							//预置点号，
}IPNC_PRESET_POINT_S, *LPIPNC_PRESET_POINT_S;

typedef struct _IPNC_DEV_PP_SET_CFG_S
{
	uint32_t	u32Size;
	uint32_t	pp_count;								//预置点数量
	IPNC_PRESET_POINT_S pp[IPNC_MAX_PRESET_PORT_SIZE];	//预置点集合
}IPNC_DEV_PP_SET_CFG_S;

//===========================================================================
//云台控制参数结构体
/* 因为需要与枪机的PTZ控制相兼容,所以结构体的大小必须与之前的大小保持一致*/
#pragma pack(2)
typedef struct _IPNC_PTZ_CTRL_S_
{
	uint16_t	u16PtzCmd;			//云台控制命令

	union
	{
		struct
		{
			uint8_t	u8Chn;			//云台通道号
			uint8_t	u8Speed;		//速度
			uint8_t	u8PresetNo;
			uint8_t	u8Res[3];
		}struPreset;
		struct 
		{
			uint8_t	u8CruisePresetNo;
			uint8_t u8CruisePresetEnable;
			uint8_t	u8CruisePresetSec;
			uint8_t	u8CruisePresetSpeed;
		}struCruise;
		struct
		{
			uint8_t u8SetVerticalPos;	// 是否设置垂直位置
			uint8_t	u8Speed;
		}struAutoScan;
		struct
		{
			uint8_t u8SetVerticalPos;	// 是否设置垂直位置
			uint8_t	u8Speed;
		}struABScan;
		struct
		{
			uint8_t	u8IdleActionIndex;
			uint8_t	u8IdleSec;
		}struIdleAction;
		struct
		{
			uint8_t	u8OntimeIndex;		// 1-8的某个定时点
			uint8_t	u8OntimeActionIndex;	// 定时动作的索引
			uint8_t	u8BeginHour;
			uint8_t	u8BeginMin;			
			uint8_t	u8EndHour;
			uint8_t	u8EndMin;
		}struOntimeAction;
		struct
		{
			uint8_t	u8AlarmTypeIndex;		// 哪一种报警
			uint8_t	u8AlarmActionIndex;		// 报警动作的索引
			uint8_t u8AlarmDurationSec;
		}struAlarm;
		struct
		{
			unsigned short res;
			int s32XCoordinate;		//(x(fact)-x(total))*10000/x(total)
			int s32YCoordinate;		//(y(total)-y(fact))*10000/y(total)
			int s32Amplify;			//正为放大，负为缩小(xwide(total)/xwide(fact)>(ywide(total)/ywide(fact)))?xwide(total)/xwide(fact)*10:ywide(total)/ywide(fact)*10
		}stru3DControl;
		struct
		{
			unsigned char	u8Left;
			unsigned char 	u8Top;
			unsigned char	u8Right;
			unsigned char 	u8Bottom;
		}stru3DRectCenter;			//left,top,right,bottom为百分比坐标，范围0-100。 当 left>=right 且bottom<=top时，为框选缩小，其他情况下为框选放大。
		struct
		{
			unsigned char	u8XCoordinate;
			unsigned char 	u8YCoordinate;
		}stru3DPointCenter;					//x,y为百分比坐标，范围0-100 
		uint8_t	u8OpenOrClose;				// 0:close, 1:open
		uint8_t u8WorkMode;					// 0:手动,1:自动
		uint8_t u8IRMode;					// 0:远近灯自动切换,1:远近灯同时开启
		uint8_t u8DayNightMode;				// 0:白天模式,1:夜晚模式,2:自动模式
		uint8_t u8WBMode;					// 0 室外,1 室内 ,2 自动,3 手动
		uint8_t u8ZoomSpeed;				// 0 快速, 1 中速, 2 低速
		uint8_t u8Bright;					// 亮度
		uint8_t u8Contrast;					// 对比度
		uint8_t u8Saturation;				// 饱和度
		uint8_t u8Sharpness;				// 锐度
		uint8_t u8Red;						// 红色
		uint8_t u8Blue;						// 蓝色
		unsigned char u8Shutter;			// 电子快门速度
	};

}IPNC_PTZ_CTRL_S, *LPIPNC_PTZ_CTRL_S;
#pragma pack()

typedef struct _IPNC_DEV_JXJ_PLATFORM_CFG_S_
{
	uint32_t	u32Size;
	char 		szPuId[IPNC_USER_NAME_LEN];
	char		szCmsIp[IPNC_IP_ADDR_LEN];
	uint16_t	u16CmsPort;
	uint8_t		u8Enable;
	uint8_t		u8ProtocolType;
	uint8_t		u8SyncTimeEnable;
	uint8_t 	u8Res1[61];
}IPNC_DEV_JXJ_PLATFORM_CFG_S;

//===========================================================================
typedef struct _IPNC_DEV_REC_STRC_S_
{
	uint32_t	u32Size;		
	uint8_t		u8OverWrite;	//0 不覆盖1 覆盖
	uint8_t		u8RecStream;	//0 主码流录像1 次码流录像
	uint8_t		u8PreRecSecs;	//预录时间(秒)
	uint8_t		u8SaveMode;		//保存模式
	uint8_t		u8Enable;		// 是否开启录像功能
	uint8_t		u8RecType;		// 录像类型， 0:开机录像， 1:定时录像，2:报警录像,3:定时&报警
	uint8_t		u8Res[2];
	
	//定时录像策略
	IPNC_SEGMENT_S	struTimerSeg[IPNC_MAX_SEG_NUM][8]; //0--每天1--星期天 2--星期一7--星期六	

	uint8_t		u8Rec[32];
}IPNC_DEV_REC_STRC_S, *LPIPNC_DEV_REC_STRC_S;
/***************************************************************************************************************/

/**********************************************客户端操作IPC命令码********************************************/
typedef enum _IPNC_CMD_TYPE_E_
{
	IPNC_CMD_DEV_CFG		= 0x2000,	//系统配置 		对应结构体: IPNC_DEV_CFG_S
	IPNC_CMD_NET_CFG		= 0x2001,	//网络配置 		对应结构体: IPNC_NET_CFG_S
	IPNC_CMD_VIDEO_CFG		= 0x2002,	//视频参数 		对应结构体: IPNC_DEV_VIDEO_CFG_S
	IPNC_CMD_OSD_CFG		= 0x2003,	//视频osd		对应结构体: IPNC_DEV_OSD_CFG_S
	IPNC_CMD_VMASK_CFG	= 0x2004,	//视频遮挡
	IPNC_CMD_VMD_CFG		= 0x2005,	//移动侦测		对应结构体: IPNC_DEV_MD_CFG_S
	IPNC_CMD_VMAIN_CFG	= 0x2006,	//视频主码流
	IPNC_CMD_VSUB_CFG		= 0x2007,	//视频次码流
	IPNC_CMD_VCAP_CFG		= 0x2008,	//视频抓拍
	IPNC_CMD_IMA_CFG		= 0x2009,	//图像参数
	IPNC_CMD_3A_CFG			= 0x200a,	//3A参数		对应结构体: IPNC_DEV_3A_CFG_S
	IPNC_CMD_AUDIO_CFG		= 0x200b,	//音频参数 		对应结构体: IPNC_AUDIO_CFG_S
	IPNC_CMD_COMM_CFG		= 0x200c,	//串口信息 		对应结构体: IPNC_COMM_CFG_S
	IPNC_CMD_FTP_CFG		= 0x200d,	//ftp参数配置 	对应结构体: IPNC_FTP_CFG_S
	IPNC_CMD_EMAIL_CFG		= 0x200e,	//email参数配置 对应结构体: IPNC_SMTP_CFG_S
	IPNC_CMD_DDNS_CFG		= 0x200f,	//ddns参数配置 	对应结构体: IPNC_DDNS_CFG_S
	IPNC_CMD_UPNP_CFG		= 0x2010,	//upnp参数配置 	对应结构体: IPNC_UPNP_CFG_S
	IPNC_CMD_NTP_CFG		= 0x2011,	//ntp参数配置 	对应结构体: IPNC_NTP_CFG_S
	//IPNC_CMD_TIME_CFG		= 0x2012,	//时间参数配置
	IPNC_CMD_USR_CFG		= 0x2013,	//用户参数配置 	对应结构体: IPNC_USER_CFG_S
	IPNC_CMD_DI_CFG			= 0x2014,	//报警输入参数	对应结构体: IPNC_DEV_DI_CFG_S
	IPNC_CMD_REC_CFG		= 0x2015,	//录像策略配置	对应结构体: IPNC_DEV_REC_STRC_S
	//IPNC_CMD_GET_HD_CFG	= 0x2016,	//磁盘信息
	IPNC_CMD_PTZ_CFG		= 0x2017,	//云台配置 		对应结构体: IPNC_PTZ_CFG_S 
	//IPNC_CMD_STABLE_CFG   = 0x2018,   //设备信息
	IPNC_CMD_BALL_CFG		= 0x2019,	//高速球信息	对应结构体: IPNC_BALL_RUN_PARAM_S
	IPNC_CMD_PPPOE_CFG		= 0x201a,	//pppoe参数配置	对应结构体: IPNC_PPPOE_CFG_S 
	IPNC_CMD_IRCFMODE_CFG 	= 0x201b,	//红外配置 		对应结构体: IPNC_DEV_IRCFMODE_CFG_S
	IPNC_CMD_PP_CFG 		= 0x201c,	//预置点PP配置	对应结构体: IPNC_DEV_PP_SET_CFG_S
	IPNC_CMD_TIME_CFG		= 0x201d,	//time配置 		对应结构体: IPNC_TIME_S
	
	//IPNC_CMD_DEV_CAP				= 0x2031,	//设备的详细信息
	IPNC_CMD_JXJ_PLATFORM_CFG		= 0x2032,	//JXJ平台配置信息 对应结构体: IPNC_DEV_JXJ_PLATFORM_CFG_S
	//IPNC_CMD_ONVIF_CFG			= 0x2033,	//ONVIF配置信息
	//IPNC_CMD_HXHT_PLATFORM_CFG	= 0x2034,	//HXHT平台配置信息
	//IPNC_CMD_GB28181_PLATFORM_CFG	= 0x2035,	//GB28181平台配置信息
	//IPNC_CMD_IRCFMODE_CFG 		= 0x2036,	//红外配置
	IPNC_CMD_OSD_LABEL_CFG			= 0x2037,	//osd拓展标注  	  对应结构体: IPNC_DEV_OSD_LABEL_CFG_S

	IPNC_CMD_REBOOT				= 0x3000,
	IPNC_CMD_HALT					= 0x3001,
	IPNC_CMD_DEFAULT				= 0x3002,
}IPNC_CMD_TYPE_E;
/***************************************************************************************************************/

#endif

