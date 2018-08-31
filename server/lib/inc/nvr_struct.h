#ifndef __NVR_STRUCT_H__
#define __NVR_STRUCT_H__

#include <stdint.h>
#include <sys/time.h>


#define NVR_ETHERNET_NUM		2	// eth0 and wifi
#define NVR_MAX_USR_NUM			4	
#define NVR_MAX_CHANNEL_NUM		4
#define NVR_MAX_STREAM_NUM		2

#define NVR_SERIALNO_LEN		48	//序列号长度
#define NVR_DEV_NAME_LEN		32	//设备名称长度
#define NVR_NAME_LEN			32	//名称长度
#define NVR_USER_NAME_LEN		32	//用户名长度	
#define NVR_PASS_WORD_LEN		32	//密码长度
#define NVR_PATH_LEN			64
#define NVR_IP_ADDR_LEN			64  //ip地址域名长度
#define NVR_MAC_ADDR_LEN		6	//mac地址长度
#define NVR_MAX_EMAIL_LEN		64
#define NVR_VERSION_LEN			64

#define NVR_MAX_SUPPORT_DI		4	//最大支持报警输入个数
#define NVR_MAX_SUPPORT_DO		1	//最大支持报警输出个数
#define NVR_MAX_SUPPORT_RS232	0	//最大支持232个数
#define NVR_MAX_SUPPORT_RS485	1	//最大支持485个数
#define NVR_MAX_SUPPORT_COMM_NO 1 	//最大支持串口个数 0--485  1--232

#define NVR_VMASK_REG_NUM		4	//视频遮挡个数
#define NVR_VMD_REG_NUM			4	//视频侦测区域数
#define NVR_MAX_SEG_NUM			4	//最大时间段

#define NVR_MAX_SUPPORT_DISK	1	//支持最大磁盘数
#define NVR_MAX_SUPPORT_PART	4	//支持最大分区数

#define NVR_MAX_OSD_LABEL_NUM 	5	//osd标注的最大个数 

#define NVR_MAX_SYS_IPC_MSG		32	//消息队列支持最大消息个数

typedef enum _NVR_SYS_MSG_KEY_E_
{
	NVR_SMTP_KEY		= 5,
	NVR_FTP_KEY			= 6,
	NVR_FTP_UP_KEY		= 7,
	NVR_FTP_DOWN_KEY	= 8,
	NVR_COMM_KEY		= 8,
	
}NVR_SYS_MSG_KEY_E;

typedef enum _NVR_SYS_MSG_TYPE_E_
{
	NVR_SMTP_MSG		= 5,
	NVR_FTP_UP_MSG		= 6,
	NVR_FTP_DOWN_MSG	= 7,
	NVR_COMM_MSG		= 8,
	
}NVR_SYS_MSG_TYPE_E;

typedef enum _NVR_MACHINE_TYPE_E_
{
	NVR_MACHINE_J5000 = 0x0,
    NVR_MACHINE_J6000 = 0x1,
	NVR_MACHINE_BUTT  = 0x2,
	
}NVR_MACHINE_TYPE_E;

typedef enum _NVR_LANGUAGE_TYPE_E_
{
	NVR_LANGUAGE_ENG = 0x0,
	NVR_LANGUAGE_CHS,
	NVR_LANGUAGE_BUTT,
	
}NVR_LANGUAGE_TYPE_E;

typedef enum _NVR_ALARM_TYPE_E_
{
	NVR_ALARM_IN_HAPPEN 	= 0X1,
	NVR_ALARM_IN_DISAPPEAR 	= 0X2,
	NVR_MD_HAPPEN			= 0X3,
	NVR_MD_DISAPPEAR		= 0X4,
	
}NVR_ALARM_TYPE_E;

typedef enum _NVR_SAVE_TYPE_E_
{
	NVR_SAVE_LOCAL_DISK	= 0x0,
	NVR_SAVE_FTP		= 0x1,
	NVR_SAVE_SMTP		= 0x2,
	NVR_SAVE_HXHT		= 0x4,
	NVR_SAVE_ONVIF		= 0x8,
}NVR_SAVE_TYPE_E;

typedef enum _NVR_VIDEO_RESOLUTION_TYPE_E_
{
	NVR_VIDEO_D1		= 0x1,		//704*576	n制704*480
	NVR_VIDEO_HD1		= 0x2,		//704*288	n制704*240
	NVR_VIDEO_CIF		= 0x4,		//352*288	n制352*240
	NVR_VIDEO_WCIF		= 0x8,		//320*192
	
	NVR_VIDEO_UXGA		= 0x100,	//1600*1200
	NVR_VIDEO_SVGA		= 0x200,	//800*600
	NVR_VIDEO_VGA		= 0x400,	//640*480
	NVR_VIDEO_QVGA		= 0x800,	//320*240
	NVR_VIDEO_QQVGA		= 0x1000,	//160*112
	
	NVR_VIDEO_1080P		= 0x10000,	//1920*1080
	NVR_VIDEO_960		= 0x20000,	//1280*960
	NVR_VIDEO_720P		= 0x40000,	//1280*720
}NVR_VIDEO_RESOLUTION_TYPE_E;

typedef enum _NVR_VIDEO_REC_TYPE_E_
{
	NVR_REC_TIME_TYPE	= 0x1,
	NVR_REC_MD_TYPE		= 0x2,
	NVR_REC_DI_TYPE		= 0x4,
	NVR_REC_MANUAL_TYPE	= 0x8,
}NVR_VIDEO_REC_TYPE_E;
#if 0
typedef enum _NVR_FRAME_TYPE_E_
{
	VIDEO_FRAME_I = 0x7,
	VIDEO_FRAME_P = 0x8,
	VIDEO_FRAME_B = 0xB,
	AUDIO_FRAME_A = 0xA, 
}NVR_FRAME_TYPE_E;
#endif

#define UART_BARD_50	(1<<0)
#define UART_BARD_75	(1<<1)
#define UART_BARD_110	(1<<2)
#define UART_BARD_150	(1<<3)
#define UART_BARD_300	(1<<4)
#define UART_BARD_600	(1<<5)
#define UART_BARD_1200	(1<<6)
#define UART_BARD_2400	(1<<7)
#define UART_BARD_4800	(1<<8)
#define UART_BARD_9600	(1<<9)
#define UART_BARD_19200	(1<<10)
#define UART_BARD_38400	(1<<11)
#define UART_BARD_57600	(1<<12)
#define UART_BARD_76800	(1<<13)
#define UART_BARD_115200 (1<<14)


//============================================================
//======================1、时间结构
typedef struct _NVR_TIME_S
{
	uint8_t		u8Year;						//年(从1900年开始)
	uint8_t		u8Month;					//月1~12
	uint8_t		u8Day;						//日1~31
	uint8_t		u8WDay;						//星期天0 星期一1.....
	uint8_t		u8Hour;						//时0~23
	uint8_t		u8Minute;					//分0~59
	uint8_t		u8Second;					//秒0~59
	uint8_t		u8Millisec;					//毫妙(0~99)(百分之一秒)
}NVR_TIME_S, *LPNVR_TIME_S;
//=======================2、时间段结构
typedef struct _NVR_SEGMENT_S
{
	uint32_t 	u8Open:8;		//开关
	uint32_t	beginSec:24;	//开始时间秒数 3600*hour+60*min+sec
	uint32_t	u8Rsvd:8;		//保留
	uint32_t	endSec:24;		//结束时间秒数 3600*hour+60*min+sec
}NVR_SEGMENT_S, *LPNVR_SEGMENT_S;

//=======================3、区域结构
typedef struct _NVR_RECT_S
{
	int32_t		s32X;							//起点x坐标
	int32_t		s32Y;							//起点y坐标
	uint32_t	u32Width;						//区域宽度
	uint32_t	u32Height;						//区域高度
}NVR_RECT_S, *LPNVR_RECT_S;
//=======================4、点结构
typedef struct _NVR_POINT_S
{
	int32_t		s32X;							//起点x坐标
	int32_t		s32Y;							//起点y坐标
}NVR_POINT_S, *LPNVR_POINT_S;
//=======================5、告警联动
typedef struct _NVR_LINKACT_S
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
}NVR_LINKACT_S, *LPNVR_LINKACT_S;

typedef struct _NVR_DEV_JXJ_PLATFORM_CFG_S_
{
	uint32_t	u32Size;
	char 		szPuId[NVR_NAME_LEN];
	char		szCmsIp[NVR_IP_ADDR_LEN];
	uint16_t	u16CmsPort;
	uint8_t		u8Enable;
	uint8_t		u8ProtocolType;
	uint8_t		u8SyncTimeEnable;
	uint8_t 	u8Res1[61];
}NVR_DEV_JXJ_PLATFORM_CFG_S;

typedef struct _NVR_LINK_INFO_S_
{	
	int	 bUsed;
	char szCamId[32];						//设备ID
	int  nCamIP;
	int  onvifPort;
	char szOnvifAccount[32];
	char szOnvifPassword[32];
}NVR_LINK_INFO_S;

typedef struct _NVR_DEV_DIANXIN_PLATFORM_CFG_S_
{	
	uint32_t 				u32Size;
	char 					szGroupUrl[64];				//集团平台URL
	char 					szProvincialUrl[64];			//省平台URL
	char 					szDevId[32];						//设备ID
	char 					szKey[32];							//key
	char 					szMode[16];						//设备类型
	uint16_t				localRtspPort;						// 本地rtsp端口
	uint16_t				upnpRtspPort;							// upnp rtsp端口
	uint8_t 				u8Res[28];	
	NVR_LINK_INFO_S			link[NVR_MAX_CHANNEL_NUM];
}NVR_DEV_DIANXIN_PLATFORM_CFG_S;


typedef struct _NVR_DEV_VAR_INFO_S
{
	char		szDevName[NVR_DEV_NAME_LEN];  		//设备名称
	uint8_t		u8Language;							//语言0 中文1英文
	uint8_t		u8DevNo;							// 本机编号
	uint8_t		u8VideoFormat;						// 0: pal, 1:ntsc
	uint8_t		u8Res[33];
}NVR_DEV_VAR_INFO_S;

typedef struct _NVR_DEV_STABLE_INFO_S_
{
	uint32_t				u32Size;
	char					szSerialId[NVR_SERIALNO_LEN];  		//设备Id				
	char					szHwVer[NVR_VERSION_LEN];			//硬件版本号
	char					szSoftVer[NVR_VERSION_LEN];			// 软件版本号
	uint8_t					u8MacAddr[NVR_ETHERNET_NUM][6];
	uint32_t				u32IPAddr[NVR_ETHERNET_NUM];
	uint32_t				u32NetMask[NVR_ETHERNET_NUM];
	uint32_t				u32GateWay[NVR_ETHERNET_NUM];
	uint32_t				u32DevCap[4];
	uint8_t					u8SupportAudio;		// 是否支持音频
	uint8_t					u8SupportHDMI;		// 是否支持HDMI
	uint8_t					u8supportVGA;		// 是否支持VGA
	uint8_t					u8ChnNum;			// 支持通道数
	uint8_t					u8SataNum;			// sata接口个数
	uint8_t					u8NetNum;			// 网卡个数
	uint8_t					u8RS232Num;			// rs232 个数
	uint8_t					u8RS485Num;			// rs485个数
	uint8_t					u8AlarmInputNum;	// 报警输入个数
	uint8_t					u8AlarmOutputNum;	// 报警输出个数
	uint8_t					u8Res[254];
}NVR_DEV_STABLE_INFO_S, *LPNVR_DEV_STABLE_INFO_S;

typedef struct _NVR_DEV_INFO_S_
{
	uint32_t		u32Size;
	NVR_DEV_STABLE_INFO_S stStableInfo;	//系统设备不可修改信息
	NVR_DEV_VAR_INFO_S	  stVarInfo;	//系统设备可修改信息
}NVR_DEV_INFO_S, *LPNVR_DEV_INFO_S;

typedef struct _NVR_DEV_ETHERNET_CFG_S_
{
	uint32_t	u32IpAddr;						//ip地址
	uint32_t	u32SubMask;						//子网掩码地址
	uint32_t	u32GateWay;						//网关地址
	uint8_t		u8MacAddr[NVR_MAC_ADDR_LEN];	//mac地址
	uint8_t		u8DhcpOn;						//dhcp开关		1--开启 0--不开启
	uint8_t		u8DhcpStatus;					//dhcp状态(只读) 0-没开启 1--正在获取 2--获取成功 3--获取失败
	uint8_t		u8EtherStatus;					//网口状态(只读) 0-网口掉 1--网口正常(只能检测物理连接)
	uint8_t		u8NetcardSpeed;					//网卡速率		0-自动 1-100M 2-10M
	uint8_t		u8Res[2];
	uint32_t		u32IpAddr1;					// eth0:1 ip地址
	uint32_t		u32SubMask1;				// eth0:1 子网掩码地址
}NVR_DEV_ETHERNET_CFG_S, *LPNVR_DEV_ETHERNET_CFG_S;


//pppoe配置信息
//
typedef struct _NVR_DEV_PPPOE_CFG_S_
{
	uint8_t		u8PppoeOn;							//pppoe开关
	uint8_t		u8PppoeStatus;						//pppoe 拨号状态0 没有拨号1 拨号中2拨号成功
	uint8_t		u8PppoeEth;							//网卡类型 0-eth0 1-ra0
	uint8_t		u8PppNum;							//0- ppp0 1-ppp 1
	uint8_t		u8Res[4];
	char		szPppoeUsr[NVR_USER_NAME_LEN];		//pppoe用户名
	char		szPppoePsw[NVR_USER_NAME_LEN];		//pppoe密码
	uint32_t	u32PppoeIp;							//pppoe获取的ip
	uint32_t	u32PppoeTimes;						//pppoe拨号时间
	
}NVR_DEV_PPPOE_CFG_S, *LPNVR_DEV_PPPOE_CFG_S;
//====================================================================

typedef struct _NVR_DEV_NET_CFG_S_
{
	uint32_t					u32Size;
	NVR_DEV_ETHERNET_CFG_S 		struEtherCfg[NVR_ETHERNET_NUM];	//网卡信息	
	uint32_t					u32AlarmHostIp;					//报警主机ip(预留)
	uint16_t					u16AlarmHostPort;				//报警主机端口(预留)
	uint8_t						u8DnsDhcp;						//dns是否自动获取
	uint8_t						u8Res1;
	uint32_t					u32DnsIp1;						//dns1
	uint32_t					u32DnsIp2;						//dns2
	uint16_t					u16HttpPort;					//http服务端口
	uint16_t					u16NetPort;						//网络服务端口
	uint32_t					u32MultiCastIp;					//多播地址
	uint16_t					u16MultiCastPort;				//多播端口
	uint16_t					u16RtspPort;					// RTSP端口
	NVR_DEV_PPPOE_CFG_S			struPppoeCfg;					//pppoe配置
	uint16_t					u16JXJDataPort;				//JXJ平台数据端口
  	uint16_t					u16CmdPort;					//命令端口
  	uint16_t					u16TalkPort;				//对讲端口
	uint8_t                 	u8Res2[26];
}NVR_DEV_NET_CFG_S,*LPNVR_DEV_NET_CFG_S;

typedef struct _NVR_TIME_CFG_S_
{
	uint32_t			u32Size;
	struct timeval		struTv;			// 当前时间
	uint8_t				u8DateFormat;	// 日期格式0:年月日, 1:月日年, 2:日月年
	uint8_t				u8TimeFormat;	// 时间格式, 0:24小时制, 1:12小时制
	char				cSeparatorChar;	// -, /, .
	uint8_t				u8Res[33];
}NVR_TIME_CFG_S,*LPNVR_TIME_CFG_S;

typedef struct _NVR_DEV_PTZ_CFG_S_
{
	uint32_t	u32Size;
	uint8_t		u8ProtocalType;	// 0 pelco-d 1 pelco-p
	uint8_t		u8PtzAddr;		//0-255
	uint8_t		u8Res[6];
}NVR_DEV_PTZ_CFG_S, *LPNVR_DEV_PTZ_CFG_S;

typedef struct _NVR_DEV_COMM_CFG_S_
{
	uint32_t		u32Size;
	uint32_t		u32BaudRate;//波特率(bps)，0-50，1-75，2-110，3-150，4-300，
								//s5-600，6-1200，7-2400，8-4800，9-9600，
								//10-19200，11-38400，12-57600，13-76800，14-115.2k 
 	uint8_t     	u8DataBit;	//数据位：0-5位，1-6位，2-7位，3-8位
 	uint8_t     	u8StopBit;	//停止位：0-1位，1-2位
  	uint8_t     	u8Parity;	//是否校验：0-无校验，1-奇校验，2-偶校验
  	uint8_t     	u8Flowcontrol;//是否流控：0-无，1-软流控,2-硬流控
  	uint32_t		u32BaudMask;
  	uint8_t			u8Res[16];
}NVR_DEV_COMM_CFG_S, *LPNVR_DEV_COMM_CFG_S;

//=====================================================================
//ddns 参数信息
typedef struct _NVR_DDNS_CFG_S
{
	uint32_t	u32Size;
	uint8_t		u8DDNSOpen;						//DDNS开关
	uint8_t		u8DDNSType;						//0-dyndns 1-3322
	uint16_t	u16DDNSPort;					//DDNS服务器端口
	char		szDDNSAccount[NVR_NAME_LEN];	//DDNS 注册名
	char		szDDNSUsr[NVR_USER_NAME_LEN];	//用户名称
	char		szDDNSPsw[NVR_PASS_WORD_LEN];	//用户密码	
	uint32_t	u32DDNSTimes;					//更新时间
	uint8_t		u8Res[32];
}NVR_DDNS_CFG_S,*LPNVR_DDNS_CFG_S;

//======================================================================
//ftp参数设置
//
typedef struct _NVR_FTP_CFG_S
{
	uint32_t		u32Size;
	char 			szFtpSvr[NVR_IP_ADDR_LEN]; 	//主ftp服务器ip
	char        	szFtpUsr[NVR_USER_NAME_LEN];//登陆用户名
	char        	szFtpPsw[NVR_PASS_WORD_LEN];//登陆密码
	char        	szFtpPath[NVR_PATH_LEN];   	//Ftp上传路径
	uint16_t		u16FtpPort;
	uint8_t			u8Rsvd[30];
}NVR_FTP_CFG_S, *LPNVR_FTP_CFG_S;

//===================================================================
//ntp 参数结构设置
typedef struct _NVR_NTP_CFG_S
{
	uint32_t	u32Size;
	uint8_t		u8NtpOpen;				//ntp开关
	uint8_t		u8Resv;
	uint16_t	u16NtpPort;				//端口
	uint8_t		u8EuroTime;				//==1:启动夏令时 0:不启动夏令时
	uint8_t		u8NtpRefTime;			//ntp 定时对时时间(单位小时)
	int32_t		nTimeZone;				//时区 单位s(-12*3600~11*3600)
	char        szNtpServer[NVR_IP_ADDR_LEN];
	uint8_t		u8Res[32];
}NVR_NTP_CFG_S, *LPNVR_NTP_CFG_S;

//===========================================================================
//upnp 参数结构
//
typedef struct _NVR_UPNP_CFG_S
{
	uint32_t	u32Size;
	uint8_t		u8UpnpOpen;			//upnp 开关
	uint8_t		u8UpnpEthNo;		//0使用本机网卡映射 1 使用无线网卡映射
	uint8_t		u8UpnpModel;    	//0固定映射 1自动映射
	uint8_t		u8UpnpRefTime;  	//upnp 刷新时间(单位小时)
	char        szUpnpSvr[32];
	char		szExternalIp[32];
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
}NVR_UPNP_CFG_S, *LPNVR_UPNP_CFG_S;

//smtp设置
typedef struct _NVR_SMTP_CFG_S
{
	uint32_t	u32Size;
	char        szMailHub[NVR_IP_ADDR_LEN]; 	//smtp 服务器地址
	char        szMailAddr[NVR_MAX_EMAIL_LEN];	//发送邮件 地址
	char        szMailUsr[NVR_MAX_EMAIL_LEN]; 	//发送邮件 账号
	char        szMailPsw[NVR_PASS_WORD_LEN];   //发送邮件 密码
	char        szMailRctp1[NVR_MAX_EMAIL_LEN];	//接受邮件 地址1
	char        szMailRctp2[NVR_MAX_EMAIL_LEN];	//接受邮件 地址2
	char       	szMailRctp3[NVR_MAX_EMAIL_LEN];	//接受邮件 地址3
	uint8_t		u8EnableSSL;					//启用SSL
	uint8_t		u8Rsvd;
	uint16_t	u16MailPort;					//smtp 服务器端口
	uint8_t     u8Rsvd2[32];
}NVR_SMTP_CFG_S, *LPNVR_SMTP_CFG_S;

//user配置
typedef struct _NVR_DEV_USER_CFG_S_
{
	uint32_t		u32Size;
	char			szUsrName[NVR_USER_NAME_LEN];	//用户名
	char			szPsw[NVR_PASS_WORD_LEN];		//密码
	uint64_t		u64LocalRight;					//本地操作权限
	uint64_t		u64RemoteRight;					//远程操作权限
	uint8_t			u8Res[32];
}NVR_DEV_USER_CFG_S, *LPNVR_DEV_USER_CFG_S;

//通道配置
typedef struct _NVR_CHANNEL_CFG_S_
{
	uint32_t		u32Size;
	char			szFactoryName[NVR_NAME_LEN];	// 厂商名称, utf8
	char			szProtocolName[NVR_NAME_LEN];	// jxj, onvif, hxht, zsyh, etc.
	char			szDevName[NVR_NAME_LEN];		// 远程设备名称, utf8
	char			szUser[NVR_NAME_LEN];			// 用户名
	char			szPasswd[NVR_PASS_WORD_LEN];	// 密码
	uint32_t		u32IpAddr;
	uint16_t		u16DataPort;
	uint16_t		u16WebPort;
	uint16_t		u16RtspPort;
	uint8_t			u8RemoteChn;					// 远程通道数
	uint8_t			u8UseChn;						// 使用哪一个远程通道
	uint8_t			u8Enable;						// 是否启用连接
	uint8_t			u8SearchA;						// 智能搜索标记
	uint8_t			u8Res[30];
}NVR_CHANNEL_CFG_S, *LPNVR_CHANNEL_CFG_S;

typedef struct _NVR_CHNNL_INFO_
{
	NVR_CHANNEL_CFG_S struChannelCfg;
	long 	lHandle;
	long 	lIpncHandle;
	long 	logonHandle;
	long 	lStreamHandle[2];
	
}NVR_CHANNEL_CFG_INFO_S;

//自动任务配置
typedef struct _NVR_AUTO_MAINTAIN_CFG_S_
{	
	uint32_t		u32Size;
	uint8_t		u8Enable;
	uint8_t		u8WeekDay;		// 0, 每天,1 星期天2,星期一...
	uint8_t		u8Hour;			// 0-23
	uint8_t		u8Min;			// 0-59
	uint8_t		u8Res[28];
}NVR_AUTO_MAINTAIN_CFG_S, *PLNVR_AUTO_MAINTAIN_CFG_S;

//磁盘配置
typedef struct _NVR_DEV_PART_INFO_S_
{
	uint8_t	u8Mount;		//0：未挂载，  1：已挂载
	uint8_t	u8Format;		//0: 未格式化，1：已格式化
	uint8_t	u8PartType;		//0：录像分区  1：备份分区
	uint8_t	u8Use;			//0：未使用，  1：正在使用
	uint8_t	u8Res[4];
	uint64_t	u64TotalSize;	//分区容量
	uint64_t	u64FreeSize;	//分区剩余容量	
}NVR_DEV_PART_INFO_S, *LPNVR_DEV_PART_INFO_S;

typedef struct _NVR_DISK_INFO_S_
{
	uint8_t	u8Parted;		//是否已经分区，0：未分区，1：已经分区
	uint8_t	u8FsType;		//文件系统类型，0：EXT2 ， 1：EXT3，2：FAT32，3：NFS
	uint8_t	u8PartNum;		//分区数量
	uint8_t	u8RecStatus;	// === 1 正在录像0 没有录像 
	uint8_t	u8Res[4];		//res
	uint64_t	u64TotalSize;	//硬盘容量
	uint64_t	u64FreeSize;	//剩余容量
	NVR_DEV_PART_INFO_S stPart[NVR_MAX_SUPPORT_PART];//分区信息	
}NVR_DEV_DISK_INFO_S, *LPNVR_DEV_DISK_INFO_S;
typedef struct _NVR_DEV_HD_INFO_S_
{
	uint32_t			u32Size;
	uint8_t				u8DiskNum;
	uint8_t				u8Res[3];
	NVR_DEV_DISK_INFO_S	stDiskInfo[NVR_MAX_SUPPORT_DISK];
}NVR_DEV_HD_INFO_S, *LPNVR_DEV_HD_INFO_S;

//==================================================================================
typedef struct _NVR_DEV_IMA_PARA_S_
{
	uint8_t	u8Value;		//参数值
	uint8_t	u8Default;		//缺省参数值
	uint8_t	u8Step;			//参数值步进值
	uint8_t	u8Res;
}NVR_DEV_IMA_PARA_S;

//视频参数设置类型定义

typedef enum _NVR_DEV_IMA_MASK_E_
{
	VCT_IMA_BRIGHTNESS 	= 0x00000001,
	VCT_IMA_CONTRAST	= 0x00000002,
	VCT_IMA_SATURATION 	= 0x00000004,
	VCT_IMA_HUE		  	= 0x00000008,
	VCT_IMA_GAMMA		= 0x00000010,
	VCT_IMA_SHARPNESS	= 0x00000020,
	VCT_IMA_BLUE		= 0x00000040,
	VCT_IMA_RED			= 0x00000080,
	VCT_IMA_ALL			= 0xFFFFFFFF,
}NVR_DEV_IMA_MASK_E;

typedef struct _NVR_DEV_IMA_CFG_S_
{	
//有效支持的参数，该参数设置时候无效
//在获取的时候有效，用以判断设备是否支持该参数
	NVR_DEV_IMA_MASK_E	eSuppMask;
//设置有效的参数，该参数获取时候无效
//在设置的时候有效，用以指定具体设置的参数
	NVR_DEV_IMA_MASK_E	eSetMask;	

	NVR_DEV_IMA_PARA_S	struBrightness;	//亮度
	NVR_DEV_IMA_PARA_S	struContrast;	//对比度
	NVR_DEV_IMA_PARA_S	struSaturation;	//饱和度
	NVR_DEV_IMA_PARA_S	struHue;		//色度
	NVR_DEV_IMA_PARA_S	struGamma;		//gamma
	NVR_DEV_IMA_PARA_S	struSharpness;	//sharpness
	NVR_DEV_IMA_PARA_S	struRed;		//red
	NVR_DEV_IMA_PARA_S	struBlue;		//blue	
	NVR_DEV_IMA_PARA_S	struRes[24];
}NVR_DEV_IMA_CFG_S, *LPNVR_DEV_IMA_CFG_S;

typedef struct _NVR_DEV_CAPTURE_CFG_S_
{
  uint16_t		u16Width;		//抓拍图片宽度
  uint16_t		u16Height;		//抓拍图片高度

  uint8_t		u8VideoEncType;	//视频编码格式 2--JPEG 固定值
  uint8_t		u8Quilty;		//抓拍质量 
   
  uint8_t		u8Res[14];	
}NVR_DEV_CAPTURE_CFG_S, *LPNVR_DEV_CAPTURE_CFG_S;

//=======================================================================
//视频编码参数	
typedef struct _NVR_DEV_VENC_CFG_S_
{
  uint16_t		u16Width;		//视频编码宽度
  uint16_t		u16Height;		//视频编码高度

  uint8_t		u8VideoEncType;	//视频编码格式0--h264 1--MJPEG 2--JPEG	
  uint8_t		u8PicQuilty;	//编码质量0-最好，1-次好，2-较好，3-一般，4-较差，5-差	
  uint8_t		u8Gop;			//I 帧间隔1-200
  uint8_t		u8FlowType;		//码率类型 0--定码流1--变码流
  uint32_t		u32BitrateRate;	//编码码率 32kbps-16Mkbps
  uint8_t		u8FrameRate;	//编码帧率1-25/30
  uint8_t		u8Res[7];
}NVR_DEV_VENC_CFG_S, *LPNVR_DEV_VENC_CFG_S;

//=======================================================================
//视频遮挡区域
typedef struct _NVR_DEV_VMASK_CFG_S_
{
  uint8_t		u8Open;			//--1.启动视频遮挡0--屏蔽视频遮挡
  uint8_t		u8Res1[3];
  NVR_RECT_S	struArea[NVR_VMASK_REG_NUM];	//视频遮挡区域
  uint8_t		u8Res2[8];
}NVR_DEV_VMASK_CFG_S, *LPNVR_DEV_VMASK_CFG_S;

//osd菜单设置
//增加5个信息标注
typedef struct _NVR_DEV_OSD_LABEL_CFG_S_
{
	uint32_t	u32EnableLabel[NVR_MAX_OSD_LABEL_NUM];				//--1 显示标题osd 0--隐藏标题osd
	char		szLabelName[NVR_MAX_OSD_LABEL_NUM][NVR_NAME_LEN];	//标注名称
	uint32_t	u32ColorValidFlag[NVR_MAX_OSD_LABEL_NUM];			// 0:使用u3Osdcolor,使用u32LabelColor
  	uint32_t	u32OsdColor[NVR_MAX_OSD_LABEL_NUM];					//----0 ---black
  																	//---1---white
  																	//---2---blue																																	//---3---red
  																	//---4---inv
	NVR_POINT_S struLabelOrg[NVR_MAX_OSD_LABEL_NUM];				//标注osd位置
	uint32_t	u32LabelColor[NVR_MAX_OSD_LABEL_NUM];				// 标注颜色,红色:0-7bit, 绿色:8-15bit,蓝色:16-23bit
}NVR_DEV_OSD_LABEL_CFG_S, *LPNVR_DEV_OSD_LABEL_CFG_S;

//暂不支持用户自定义osd 菜单
typedef struct _NVR_DEV_OSD_CFG_S_
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
  									//---3---red
  									//---4---inv
  uint8_t		u8EnableUsrOsd; 	//bit0 使能第一个用户菜单
  									//bit1 使能第二个用户菜单

  NVR_POINT_S 	struTimeOrg;		//时间osd位置
  NVR_POINT_S 	struTitleOrg;		//标题osd位置
  NVR_POINT_S 	struUsrOrg[2];		//用户自定义osd位置
  char			szChnName[NVR_NAME_LEN];		//通道名称
  char			szUsrOsdInfo[2][NVR_NAME_LEN];	//用户自定义osd信息
  uint32_t		u32TimeColor;		//时间颜色,红色:0-7bit, 绿色:8-15bit,蓝色:16-23bit
  uint32_t		u32ChnColor;		//通道颜色,红色:0-7bit, 绿色:8-15bit,蓝色:16-23bit
  uint8_t		u8ColorValidFlag;	// 0:使用u8Osdcolor初始化u32TimeColor和u32ChnColor值, 1:直接使用u32TimeColor和u32ChnColor值
  uint8_t		u8Res[7];
}NVR_DEV_OSD_CFG_S, *LPNVR_DEV_OSD_CFG_S;

typedef enum _NVR_DEV_H264_ENCODE_LEVEL_E_
{
	H264_HIGH = 0,
	H264_MAIN,
	H264_BASELINE,
}NVR_DEV_H264_ENCODE_LEVEL_E;

//图像参数设置
typedef struct _NVR_DEV_VIDEO_CFG_S_
{
	uint32_t				u32Size;	
	NVR_DEV_OSD_CFG_S 		struOsd;
	NVR_DEV_VMASK_CFG_S		struVMask;
	NVR_DEV_VENC_CFG_S 		struMainVenc;
	NVR_DEV_VENC_CFG_S 		struSubVenc;
	NVR_DEV_CAPTURE_CFG_S   struCapture;
	NVR_DEV_IMA_CFG_S		struIma;
	NVR_DEV_H264_ENCODE_LEVEL_E	eEncodeLevel;	// 0:high profile, 1:main profile, 2:baseline
	uint8_t					u8Res[28];
}NVR_DEV_VIDEO_CFG_S, *LPNVR_DEV_VIDEO_CFG_S;

typedef struct _NVR_DEV_AUDIO_CFG_S_
{
	uint32_t	u32Size;
	uint32_t	u32SampleRate;		//采样率 8k 80000
	uint8_t		u8Open;				//是否开启音频
	uint8_t		u8AEncType;			//0--PCM 1--G711A 2-G711U 3--G726
	uint8_t		u8Type;      		// 0--mic输入 1--线输入
	uint8_t		u8Res[5];
}NVR_DEV_AUDIO_CFG_S, *LPNVR_DEV_AUDIO_CFG_S;

//===========================================================================
typedef struct _NVR_DEV_REC_STRC_S_
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
	NVR_SEGMENT_S	struTimerSeg[NVR_MAX_SEG_NUM][8]; //0--每天1--星期天 2--星期一7--星期六	

	uint8_t		u8Rec[32];
}NVR_DEV_REC_STRC_S, *LPNVR_DEV_REC_STRC_S;

//===========================================================================
//云台控制参数结构体
/* 因为需要与枪机的PTZ控制相兼容,所以结构体的大小必须与之前的大小保持一致*/
#pragma pack(2)
typedef struct _NVR_CTRL_PTZ_S_
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

}NVR_CTRL_PTZ_S, *LPNVR_NET_CTRL_PTZ_S;
#pragma pack()

//移动侦测信息
typedef struct _NVR_DEV_MD_CFG_S_
{
	uint32_t			u32Size;
	uint8_t			u8Enable;	//是否开启移动侦测 0--关闭 1--开启			
	uint8_t			u8Level;	//侦测灵敏度 0--很高 1--高 2--中 3--低 4--很低	
	uint8_t			u8Res1[2];
	NVR_RECT_S		struRect[NVR_VMD_REG_NUM];	//侦测区域
	NVR_SEGMENT_S	struSeg[NVR_MAX_SEG_NUM][8];  	//0--每天1--星期天 2--星期一7--星期六	
	NVR_LINKACT_S	struLinkAct;					//移动侦测联动消息
	uint32_t		u32DetectInterval;			//检测间隔时间
	uint8_t			u8Res2[12];
}NVR_DEV_MD_CFG_S, *LPNVR_DEV_MD_CFG_S;

//探头报警信息
typedef struct _NVR_DEV_DI_CFG_S_
{
	uint32_t		u32Size;
	char			chDiName[NVR_USER_NAME_LEN]; //di探头名称
	uint8_t			u8Enable;					 //是否开启di报警检测 0--不开启 1--开启
	uint8_t			u8DiType;					 //0--常开探头1--常闭探头
	uint8_t			u8Res1[2];
	NVR_SEGMENT_S	struSeg[NVR_MAX_SEG_NUM][8]; 	 //0--每天1--星期天 2--星期一7--星期六	
	NVR_LINKACT_S	struLinkAct;					 //di报警联动消息
	uint32_t		u32DetectInterval;			 //检测间隔时间
	uint8_t			u8Res2[12];
}NVR_DEV_DI_CFG_S, *LPNVR_DEV_DI_CFG_S;

// 远程ipnc报警
typedef struct _NVR_DEV_IPNC_ALARM_CFG_S_
{
	uint32_t		u32Size;
	uint8_t			u8Enable;  
	uint8_t			u8Res1[3];
	NVR_LINKACT_S	struLinkAct;					 //di报警联动消息
	uint8_t			u8Res2[32];
}NVR_DEV_IPNC_ALARM_CFG_S;

// 全自动控制
typedef struct _NVR_DEV_AUTO_CTRL_CFG_S_
{
	uint32_t		u32Size;
	uint8_t 		u8Enable;
	uint8_t 		u8Res1[3];
	uint8_t		u8Res2[64];
}NVR_DEV_AUTO_CTRL_CFG_S;

#endif

