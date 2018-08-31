#ifndef _TI_STRUCT_H
#define _TI_STRUCT_H
#include "ti_type.h"

                                                  
#ifdef __cplusplus
extern "C" {
#endif

#define TI_SOFT_VERSION			0x022A0000

#define TI_MAX_SUPPORT_CHN		1	//最大支持通道数
#define TI_MAX_SUPPORT_STREAM	2	//最大支持码流数 0--主码流 1--次码流

#ifdef TI_ARCH
#define TI_MAX_SUPPORT_DI		2	//最大支持报警输入个数
#else
#define TI_MAX_SUPPORT_DI		1	//最大支持报警输入个数
#endif

#define TI_MAX_SUPPORT_DO		1	//最大支持报警输出个数
#define TI_MAX_SUPPORT_RS232	0	//最大支持232个数
#define TI_MAX_SUPPORT_RS485	1	//最大支持485个数
#define TI_MAX_SUPPORT_COMM_NO 	1 	//最大支持串口个数 0--485  1--232
#define TI_MAX_SUPPORT_DISK		1	//支持最大磁盘数
#define TI_MAX_SUPPORT_PART		4	//支持最大分区数

#define TI_SERIALNO_LEN			48	//序列号长度
#define TI_DEV_NAME_LEN			32	//设备名称长度
#define TI_NAME_LEN				32	//名称长度
#define TI_USER_NAME_LEN		32	//用户名长度	
#define TI_PASS_WORD_LEN		32	//密码长度
#define TI_PATH_LEN				64
#define TI_IP_ADDR_LEN			64  //ip地址域名长度
#define TI_MAC_ADDR_LEN			6	//mac地址长度
#define TI_ETHERNET_NUM			2	//支持网卡个数
#define TI_VMASK_REG_NUM		4	//视频遮挡个数
#define TI_VMD_REG_NUM			4	//视频侦测区域数
#define TI_MAX_SEG_NUM			4	//最大时间段
#define TI_MAX_EMAIL_LEN		64
#define TI_MAX_USR_NUM			3
#define TI_MAX_IRCF_SEG_NUM		4	//红外定时最大时间段
#define TI_MAX_WIF_ESSID_NUM	10 	//最大支持wifi节点个数

#define TI_MAX_SYS_IPC_MSG		32	//消息队列支持最大消息个数

#define TI_MAX_OSD_LABEL_NUM 	5	//osd标注的最大个数 


// define in the main Makefile
#ifndef TI_SUPPORT_PLATFORM
#define TI_SUPPORT_PLATFORM     0   //支持平台接入
#endif
#define JXJ_BALL 1

typedef enum _TI_SYS_MSG_KEY_E_
{
	TI_SMTP_KEY		= 5,
	TI_FTP_KEY		= 6,
	TI_FTP_UP_KEY	= 7,
	TI_FTP_DOWN_KEY	= 8,
	TI_COMM_KEY		= 8,
	
}TI_SYS_MSG_KEY_E;

typedef enum _TI_SYS_MSG_TYPE_E_
{
	TI_SMTP_MSG		= 5,
	TI_FTP_UP_MSG	= 6,
	TI_FTP_DOWN_MSG	= 7,
	TI_COMM_MSG		= 8,
	
}TI_SYS_MSG_TYPE_E;

typedef enum _TI_MACHINE_TYPE_E_
{
	TI_MACHINE_J5000 = 0x0,
    TI_MACHINE_J6000 = 0x1,
	TI_MACHINE_BUTT  = 0x2,
	
}TI_MACHINE_TYPE_E;

typedef enum _TI_LANGUAGE_TYPE_E_
{
	TI_LANGUAGE_ENG = 0x0,
	TI_LANGUAGE_CHS,
	TI_LANGUAGE_BUTT,
	
}TI_LANGUAGE_TYPE_E;

typedef enum _TI_ALARM_TYPE_E_
{
	TI_ALARM_IN_HAPPEN 		= 0X1,
	TI_ALARM_IN_DISAPPEAR 	= 0X2,
	TI_MD_HAPPEN			= 0X3,
	TI_MD_DISAPPEAR			= 0X4,
	
}TI_ALARM_TYPE_E;

typedef enum _TI_SAVE_TYPE_E_
{
	TI_SAVE_LOCAL_DISK	= 0x0,
	TI_SAVE_FTP			= 0x1,
	TI_SAVE_SMTP		= 0x2,
	TI_SAVE_HXHT		= 0x4,
	TI_SAVE_ONVIF		= 0x8,
}TI_SAVE_TYPE_E;

typedef enum _TI_VIDEO_RESOLUTION_TYPE_E_
{
	TI_VIDEO_D1			= 0x1,		//704*576	n制704*480
	TI_VIDEO_HD1		= 0x2,		//704*288	n制704*240
	TI_VIDEO_CIF		= 0x4,		//352*288	n制352*240
	TI_VIDEO_WCIF		= 0x8,		//320*192
	
	TI_VIDEO_UXGA		= 0x100,	//1600*1200
	TI_VIDEO_SVGA		= 0x200,	//800*600
	TI_VIDEO_VGA		= 0x400,	//640*480
	TI_VIDEO_QVGA		= 0x800,	//320*240
	TI_VIDEO_QQVGA		= 0x1000,	//160*112
	
	TI_VIDEO_1080P		= 0x10000,	//1920*1080
	TI_VIDEO_960		= 0x20000,	//1280*960
	TI_VIDEO_720P		= 0x40000,	//1280*720
}TI_VIDEO_RESOLUTION_TYPE_E;

typedef enum _TI_VIDEO_REC_TYPE_E_
{
	TI_REC_TIME_TYPE	= 0x1,
	TI_REC_MD_TYPE		= 0x2,
	TI_REC_DI_TYPE		= 0x4,
	TI_REC_MANUAL_TYPE	= 0x8,
}TI_VIDEO_REC_TYPE_E;

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

#ifndef PACKED
#define PACKED	__attribute__((packed, aligned(1)))
#endif

//================================================================
//数据帧格式
//================================================================
typedef struct _TI_VIDEO_INFO_S
{
	TI_U16		u16ImageWidth;	//图像宽度
	TI_U16		u16ImageHeight;	//图像高度
	TI_U8		u8EncodeType;	//视频编码格式0-h264 1-mjpeg 2-jpeg
	TI_U8		u8FrameRate;	//帧率(保留)
	TI_U8		u8Pal;			//制式0-n制1-pal制
	TI_U8		u8Res[1];
}TI_VIDEO_INFO_S,*LPTI_VIDEO_INFO_S;

typedef struct _TI_AUDIO_INFO_S
{
	TI_U8		u8AudioSamples;		//采样率 0--8k 1--16k 2-32k
	TI_U8		u16EncodeType;		//音频编码格式0--pcm 1-g711a 2-g711u 3--g726
	TI_U8		u8AudioChannels;	//通道数 暂只支持1	
	TI_U8		u8AudioBits;		//位数 16bit
	TI_U8		u8Res[4];
}TI_AUDIO_INFO_S, *LPTI_AUDIO_INFO_S;

typedef struct _TI_FRAME_HEAD_S
{
	TI_U16		u16FrameFlag;		//0x3448 magic data
	TI_U8		u8FrameType;		//I-0x7 p--0x8 b--0xb A--0xa
	TI_U8		u8Res[1];			//保留
	TI_U32		u32FrameNo;			//帧号
	TI_U32		u32Time;			//系统时间
	TI_U32		u32FrameSize;		//数据流长度
	TI_U64		u64TimeStamp;		//时间戳
	union
	{
		TI_VIDEO_INFO_S	unVidoeInfo;
		TI_AUDIO_INFO_S	unAudioInfo;
	};	
}TI_FRAME_HEAD_S, *LPTI_FRAME_HEAD_S;
//============================================================
//======================1、时间结构
typedef struct _TI_TIME_S
{
	TI_U8		u8Year;							//年(从1900年开始)
	TI_U8		u8Month;						//月1~12
	TI_U8		u8Day;							//日1~31
	TI_U8		u8WDay;							//星期天0 星期一1.....
	TI_U8		u8Hour;							//时0~23
	TI_U8		u8Minute;						//分0~59
	TI_U8		u8Second;						//秒0~59
	TI_U8		u8Millisec;						//毫妙(0~99)(百分之一秒)
}TI_TIME_S, *LPTI_TIME_S;
//=======================2、时间段结构
typedef struct _TI_SEGMENT_S
{
	TI_U32 		u8Open:8;		//开关
	TI_U32		beginSec:24;	//开始时间秒数 3600*hour+60*min+sec
	TI_U32		u8Rsvd:8;		//保留
	TI_U32		endSec:24;		//结束时间秒数 3600*hour+60*min+sec
}TI_SEGMENT_S, *LPTI_SEGMENT_S;

//=======================3、区域结构
typedef struct _TI_RECT_S
{
	TI_S32		s32X;							//起点x坐标
	TI_S32		s32Y;							//起点y坐标
	TI_U32		u32Width;						//区域宽度
	TI_U32		u32Height;						//区域高度
}TI_RECT_S, *LPTI_RECT_S;
//=======================4、点结构
typedef struct _TI_POINT_S
{
	TI_S32		s32X;							//起点x坐标
	TI_S32		s32Y;							//起点y坐标
}TI_POINT_S, *LPTI_POINT_S;
//=======================5、告警联动
typedef struct _TI_LINKACT_S
{
	TI_U32		u32ShootChannel;	//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
	TI_U32		u32ShootInterval;	//抓拍间隔(毫秒),0:连续抓拍
	TI_U32		u32RecTime;			//录像时间(秒)
	TI_U32		u32RecChannel;		//录像通道(bit) (0 不录像 1第一通道录像)
	TI_U32		u32PCRecChannel;	//联动PC录像通道	
	TI_U32		u32OutPut;			//联动输出(bit)
	TI_U32		u32OutPersist;		//输出持续时间
	TI_U8		u8ShootMode;		//联动抓拍模式,0本地存储，1ftp上传，2email
	TI_U8		u8ShootNum;			//抓拍张数	
	TI_U8		u8RecMode;			//联动录像模式，0本地存储，1ftp上传	
	TI_U8		u8RecPreTime;		//预录时间(秒)
	TI_U8		u8Email;			//联动email
	TI_U8		u8Rsvd[15];
}TI_LINKACT_S, *LPTI_LINKACT_S;
//==========================================================
//================================================
//报警消息回调结构信息
//
typedef struct _TI_ALARM_INFO_S
{
	TI_TIME_S	struAlarmTime;			//报警触发时间
	TI_U8		u8AlarmType;			//报警事件类型
	TI_U8		u8Channel;				//报警通道号
	TI_U8		u8AlarmInPutCh;			//报警输入通道号
	TI_U8		u8DiskNum;				//报警硬盘号
	TI_U32		u32AlarmOutPutStatus;	//联动报警输出状态
	TI_U32		u32AlarmRecStatus;		//联动报警录像状态
	TI_U32		u32PcRecStatus;			//联动pc录像状态
	TI_U32		u32SeqNo;
	TI_U8		u8Res[12];
}TI_ALARM_INFO_S, *LPTI_ALARM_INFO_S; 
//==================================================
//串口消息回调结构信息
typedef struct _TI_COMM_INFO_S_
{
	TI_U8		u8CommNo;				//串口号
	TI_U8		u8DataLen;				//串口信息长度
	TI_U8		u8Data[254];			//串口信息
}TI_COMM_INFO_S, *LPTI_COMM_INFO_S;
//==============================================================
//
typedef struct _TI_DEV_STABLE_INFO_S_
{
	TI_U8		u8SerialId[TI_SERIALNO_LEN];  	//设备Id
	TI_U8		u8DiNum;						//报警输入路数
	TI_U8		u8DoNum;						//报警输出路数
	TI_U8		u8ChnNum  ;						//设备路数
	TI_U8		u8Rs232Num;						//设备232 串口个数
	TI_U8		u8Rs485Num;						//设备485 串口个数
	TI_U8		u8NetCardNum;					//网卡个数
	TI_U16		u16DevType ;					//设备类型						
	TI_U32		u32HwVer;						//硬件版本号
	TI_U32		u32CoreVer;						//内核版本号
	TI_U32		u32SoftVer;						//软件版本号
	TI_U32		u32ResolutionM;					//支持的主码流分辨率大小
	TI_U32		u32ResolutionS;					//支持的次码流分辨率大小	
	TI_U32		u32SensorType;					//前端sensor 类型
	TI_U8		u8Res2[12];	
}TI_DEV_STABLE_INFO_S, *LPTI_DEV_STABLE_INFO_S;

typedef struct _TI_DEV_VAR_INFO_S
{
	TI_CHAR		szDevName[TI_DEV_NAME_LEN];  	//设备名称
	TI_U8		u8Language;						//语言0 中文1英文
	TI_U8		u8PalStand;						//制式1 pal 0 ntfc
	TI_U8		u8Hz;							//电源频率0 50Hz 1 60HZ
	TI_U8		u8Res[17];
}TI_DEV_VAR_INFO_S;

typedef struct _TI_DEV_INFO_S_
{
	TI_U32		u32Size;
	TI_DEV_STABLE_INFO_S stStableInfo;	//系统设备不可修改信息
	TI_DEV_VAR_INFO_S	  stVarInfo;	//系统设备可修改信息
}TI_DEV_INFO_S, *LPTI_DEV_INFO_S;

typedef struct _TI_DEV_ETHERNET_CFG_S_
{
	TI_U32		u32IpAddr;						//ip地址
	TI_U32		u32SubMask;						//子网掩码地址
	TI_U32		u32GateWay;						//网关地址
	TI_U8		u8MacAddr[TI_MAC_ADDR_LEN];		//mac地址
	TI_U8		u8DhcpOn;						//dhcp开关		1--开启 0--不开启
	TI_U8		u8DhcpStatus;					//dhcp状态(只读) 0-没开启 1--正在获取 2--获取成功 3--获取失败
	TI_U8		u8EtherStatus;					//网口状态(只读) 0-网口掉 1--网口正常(只能检测物理连接)
	TI_U8		u8NetcardSpeed;					//网卡速率		0-自动 1-100M 2-10M
	TI_U8		u8Res[10];
}TI_DEV_ETHERNET_CFG_S, *LPTI_DEV_ETHERNET_CFG_S;
//============================================================
//pppoe配置信息
//
typedef struct _TI_DEV_PPPOE_CFG_S_
{
	TI_U8		u8PppoeOn;							//pppoe开关
	TI_U8		u8PppoeStatus;						//pppoe 拨号状态0 没有拨号1 拨号中2拨号成功
	TI_U8		u8Res[6];
	TI_CHAR		szPppoeUsr[TI_USER_NAME_LEN];		//pppoe用户名
	TI_CHAR		szPppoePsw[TI_USER_NAME_LEN];		//pppoe密码
	TI_U32		u32PppoeIp;							//pppoe获取的ip
	TI_U32		u32PppoeTimes;						//pppoe拨号时间
	
}TI_DEV_PPPOE_CFG_S, *LPTI_DEV_PPPOE_CFG_S;

typedef struct _TI_DEV_NET_CFG_S_
{
  TI_U32				u32Size;
  TI_DEV_ETHERNET_CFG_S struEtherCfg[TI_ETHERNET_NUM];//网卡信息	
  TI_U32				u32AlarmHostIp;				//报警主机ip(预留)
  TI_U16				u16AlarmHostPort;			//报警主机端口(预留)
  TI_U8					u8DnsDhcp;					//dns是否自动获取
  TI_U8					u8Res1;
  TI_U32				u32DnsIp1;					//dns1
  TI_U32				u32DnsIp2;					//dns2
  TI_U16				u16HttpPort;				//http服务端口
  TI_U16				u16NetPort;					//网络服务端口
  TI_U32				u32MultiCastIp;				//多播地址
  TI_U16				u16MultiCastPort;			//多播端口
  TI_U16				u16Res;
  TI_DEV_PPPOE_CFG_S	struPppoeCfg;				//pppoe配置
  TI_U8                 u8Res2[32];
  
}TI_DEV_NET_CFG_S,*LPTI_DEV_NET_CFG_S;

typedef struct _TI_DEV_CAPTURE_CFG_S_
{
  TI_U16	u16Width;		//抓拍图片宽度
  TI_U16	u16Height;		//抓拍图片高度
  TI_U8		u8VideoEncType;	//视频编码格式 2--JPEG 固定值
  TI_U8		u8Quilty;		//抓拍质量 
  TI_U8		u8Res[14];	
  
}TI_DEV_CAPTURE_CFG_S, *LPTI_DEV_CAPTURE_CFG_S;
//=======================================================================
//视频编码参数	
typedef struct _TI_DEV_VENC_CFG_S_
{
  TI_U16	u16Width;		//视频编码宽度
  TI_U16	u16Height;		//视频编码高度

  TI_U8		u8VideoEncType;	//视频编码格式0--h264 1--MJPEG 2--JPEG	
  TI_U8		u8PicQuilty;	//编码质量0-最好，1-次好，2-较好，3-一般，4-较差，5-差	
  TI_U8		u8Gop;			//I 帧间隔1-200
  TI_U8		u8FlowType;		//码率类型 0--定码流1--变码流
  TI_U32	u32BitrateRate;	//编码码率 32kbps-16Mkbps
  TI_U8		u8FrameRate;	//编码帧率1-25/30
  TI_U8		u8Res[7];
  
}TI_DEV_VENC_CFG_S, *LPTI_DEV_VENC_CFG_S;
//=======================================================================
//视频遮挡区域
typedef struct _TI_DEV_VMASK_CFG_S_
{
  TI_U8		u8Open;			//--1.启动视频遮挡0--屏蔽视频遮挡
  TI_U8		u8Res1[3];
  TI_RECT_S	struArea[TI_VMASK_REG_NUM];	//视频遮挡区域
  TI_U8		u8Res2[8];
}TI_DEV_VMASK_CFG_S, *LPTI_DEV_VMASK_CFG_S;
//=====================================================================
//osd菜单设置
//增加5个信息标注
typedef struct _TI_DEV_OSD_LABEL_CFG_S_
{
	TI_U32		u32EnableLabel[TI_MAX_OSD_LABEL_NUM];					//--1 显示标题osd 0--隐藏标题osd
	TI_CHAR		szLabelName[TI_MAX_OSD_LABEL_NUM][TI_NAME_LEN];			//标注名称
	TI_U32		u32ColorValidFlag[TI_MAX_OSD_LABEL_NUM];				// 0:使用u3Osdcolor,使用u32LabelColor
  	TI_U32		u32OsdColor[TI_MAX_OSD_LABEL_NUM];						//----0 ---black
  																		//---1---white
  																		//---2---blue
  																		//---3---red
  																		//---4---inv
	TI_POINT_S 	struLabelOrg[TI_MAX_OSD_LABEL_NUM];						//标注osd位置
	TI_U32		u32LabelColor[TI_MAX_OSD_LABEL_NUM];					// 标注颜色,红色:0-7bit, 绿色:8-15bit,蓝色:16-23bit
}TI_DEV_OSD_LABEL_CFG_S, *LPTI_DEV_OSD_LABEL_CFG_S;


//暂不支持用户自定义osd 菜单
typedef struct _TI_DEV_OSD_CFG_S_
{
  TI_U8		u8EnableTitleOsd;		//--1 显示标题osd 0--隐藏标题osd
  TI_U8		u8EnableTimeOsd;		//--1 显示时间osd 0--隐藏时间osd
  TI_U8		u8DispWeek;				//--1 显示星期0--隐藏星期
  TI_U8		u8EnableBitrateOsd;		//--1显示码流osd  0--隐藏码流osd
  TI_U8		u8TimeOsdType;			//--时间显示类型
  									//--0 YYYY-MM-DD HH:MM:SS
  									//--1 MM-DD-YYYY HH:MM:SS
  									//--2 DD-MM-YYYY HH:MM:SS
									//--3 YYYY/MM/DD HH:MM:SS
									//--4 MM/DD/YYYY HH:MM:SS
									//--5 DD/MM/YYYY HH:MM:SS
									
  TI_U8		u8HourOsdType;			//--0 24 hour --1 12 hour
  TI_U8		u8OsdColor;				//----0 ---black
  									//---1---white
  									//---2---blue
  									//---3---red
  TI_U8		u8EnableUsrOsd; //bit0 使能第一个用户菜单
  							//bit1 使能第二个用户菜单

  TI_POINT_S struTimeOrg;	//时间osd位置
  TI_POINT_S struTitleOrg;	//标题osd位置
  TI_POINT_S struUsrOrg[2];	//用户自定义osd位置
  TI_CHAR	szChnName[TI_NAME_LEN];		 //通道名称
  TI_CHAR	szUsrOsdInfo[2][TI_NAME_LEN];//用户自定义osd信息
  TI_U32	u32TimeColor;		// 时间颜色,红色:0-7bit, 绿色:8-15bit,蓝色:16-23bit
  TI_U32	u32ChnColor;		// 通道颜色,红色:0-7bit, 绿色:8-15bit,蓝色:16-23bit
  TI_U8		u8ColorValidFlag;	// 0:使用u8Osdcolor初始化u32TimeColor和u32ChnColor值, 1:直接使用u32TimeColor和u32ChnColor值
  TI_U8		u8Res[7];
}TI_DEV_OSD_CFG_S, *LPTI_DEV_OSD_CFG_S;

typedef enum _TI_DEV_H264_ENCODE_LEVEL_E_
{
	H264_HIGH = 0,
	H264_MAIN,
	H264_BASELINE,
}TI_DEV_H264_ENCODE_LEVEL_E;

//====================================
//图像参数设置

typedef struct _TI_DEV_IMA_PARA_S_
{
	TI_U8	u8Value;			//参数值
	TI_U8	u8Default;			//缺省参数值
	TI_U8	u8Step;				//参数值步进值
	TI_U8	u8Res;
}TI_DEV_IMA_PARA_S;
//视频参数设置类型定义
typedef enum _TI_DEV_IMA_MASK_E_
{
	VCT_IMA_BRIGHTNESS 	= 0x00000001,
	VCT_IMA_CONTRAST	  	= 0x00000002,
	VCT_IMA_SATURATION 	= 0x00000004,
	VCT_IMA_HUE		  	= 0x00000008,
	VCT_IMA_GAMMA		= 0x00000010,
	VCT_IMA_SHARPNESS	= 0x00000020,
	VCT_IMA_BLUE			= 0x00000040,
	VCT_IMA_RED			= 0x00000080,
	VCT_IMA_ALL			= 0xFFFFFFFF,
}TI_DEV_IMA_MASK_E;


typedef struct _TI_DEV_IMA_CFG_S_
{	
/*有效支持的参数，该参数设置时候无效
在获取的时候有效，用以判断设备是否支持该参数*/	
	TI_DEV_IMA_MASK_E	eSuppMask;
/*设置有效的参数，该参数获取时候无效
   在设置的时候有效，用以指定具体设置的参数*/	
	TI_DEV_IMA_MASK_E	eSetMask;	

	TI_DEV_IMA_PARA_S	struBrightness;	//亮度
	TI_DEV_IMA_PARA_S	struContrast;	//对比度
	TI_DEV_IMA_PARA_S	struSaturation;//饱和度
	TI_DEV_IMA_PARA_S	struHue;		//色度
	TI_DEV_IMA_PARA_S	struGamma;		//gamma
	TI_DEV_IMA_PARA_S	struSharpness;	//sharpness
	TI_DEV_IMA_PARA_S	struRed;		//red
	TI_DEV_IMA_PARA_S	struBlue;	//blue	
	TI_DEV_IMA_PARA_S	struRes[24];
}TI_DEV_IMA_CFG_S, *LPTI_DEV_IMA_CFG_S;


typedef struct _TI_DEV_VIDEO_CFG_S_
{
	TI_U32					u32Size;	
	TI_DEV_OSD_CFG_S 		struOsd;
	TI_DEV_VMASK_CFG_S		struVMask;
	TI_DEV_VENC_CFG_S 		struMainVenc;
	TI_DEV_VENC_CFG_S 		struSubVenc;
	TI_DEV_CAPTURE_CFG_S   	struCapture;
	TI_DEV_IMA_CFG_S		struIma;
	TI_DEV_H264_ENCODE_LEVEL_E	eEncodeLevel;	// 0:high profile, 1:main profile, 2:baseline
	TI_U8					u8Res[28];
}TI_DEV_VIDEO_CFG_S, *LPTI_DEV_VIDEO_CFG_S;

typedef struct _TI_DEV_AUDIO_CFG_S_
{
	TI_U32		u32Size;
	TI_U32		u32SampleRate;		//采样率 8k 80000
	TI_U8		u8Open;				//是否开启音频
	TI_U8		u8AEncType;			//0--PCM 1--G711A 2-G711U 3--G726
	TI_U8		u8Type;      			// 0--mic输入 1--线输入
	TI_U8		u8Res[5];
}TI_DEV_AUDIO_CFG_S, *LPTI_DEV_AUDIO_CFG_S;

typedef struct _TI_DEV_PTZ_CFG_S_
{
	TI_U32		u32Size;
	TI_U8		u8ProtocalType;	// 0 pelco-d 1 pelco-p
	TI_U8		u8PtzAddr;		//0-255
	TI_U8		u8Res[6];
}TI_DEV_PTZ_CFG_S, *LPTI_DEV_PTZ_CFG_S;

typedef struct _TI_DEV_COMM_CFG_S_
{
	TI_U32		u32Size;
	TI_U32		u32BaudRate;//波特率(bps)，0-50，1-75，2-110，3-150，4-300，
						//s5-600，6-1200，7-2400，8-4800，9-9600，
						//10-19200，11-38400，12-57600，13-76800，14-115.2k 
 	TI_U8     	u8DataBit;	//数据位：0-5位，1-6位，2-7位，3-8位
 	TI_U8     	u8StopBit;	//停止位：0-1位，1-2位
  	TI_U8     	u8Parity;	//是否校验：0-无校验，1-奇校验，2-偶校验
  	TI_U8     	u8Flowcontrol;//是否流控：0-无，1-软流控,2-硬流控
  	TI_U32		u32BaudMask;
  	TI_U8		u8Res[4];
}TI_DEV_COMM_CFG_S, *LPTI_DEV_COMM_CFG_S;

typedef struct _TI_DEV_MD_CFG_S_
{
  TI_U32			u32Size;
  TI_U8				u8Enable;	//是否开启移动侦测 0--关闭 1--开启			
  TI_U8				u8Level;	//侦测灵敏度 0--很高 1--高 2--中 3--低 4--很低	
  TI_U8				u8Res1[2];
  TI_RECT_S			struRect[TI_VMD_REG_NUM];	//侦测区域
  TI_SEGMENT_S		struSeg[TI_MAX_SEG_NUM][8];  //0--每天1--星期天 2--星期一7--星期六	
  TI_LINKACT_S		struLinkAct;				//移动侦测联动消息
  TI_U32			u32DetectInterval;			// 检测间隔时间
  TI_U8				u8Res2[12];
}TI_DEV_MD_CFG_S, *LPTI_DEV_MD_CFG_S;

typedef struct _TI_DEV_DI_CFG_S_
{
  TI_U32			u32Size;
  TI_CHAR			chDiName[TI_NAME_LEN];	//di探头名称
  TI_U8				u8Enable;				//是否开启di报警检测 0--不开启 1--开启
  TI_U8				u8DiType;				//0--常开探头1--常闭探头
  TI_U8				u8Res1[2];
  TI_SEGMENT_S		struSeg[TI_MAX_SEG_NUM][8];  //0--每天1--星期天 2--星期一7--星期六	
  TI_LINKACT_S		struLinkAct;				//di报警联动消息
  TI_U32			u32DetectInterval;			// 检测间隔时间
  TI_U8				u8Res2[12];
}TI_DEV_DI_CFG_S, *LPTI_DEV_DI_CFG_S;
//==========================================================
//视频丢失参数结构
//
typedef struct _TI_DEV_VLOST_CFG_S_
{
  TI_U32			u32Size;
  TI_U8				u8Enable;					//是否开启视频丢失报警检测 0--不开启 1--开启		
  TI_U8				u8Res1[3];
  TI_SEGMENT_S		struSeg[TI_MAX_SEG_NUM][8];  //0--每天1--星期天 2--星期一7--星期六	
  TI_LINKACT_S		struLinkAct;				//视频丢失报警联动
  TI_U8				u8Res2[16];
}TI_DEV_VLOST_CFG_S, *LPTI_DEV_VLOST_CFG_S;
//========================================================
//用户信息结构参数 
//
typedef struct _TI_DEV_USER_CFG_S_
{
	TI_U32		u32Size;
	TI_CHAR		szUsrName[TI_USER_NAME_LEN];		//用户名
	TI_CHAR		szPsw[TI_PASS_WORD_LEN];			//密码
	TI_U64		u64LocalRight;						//本地操作权限
	TI_U64		u64RemoteRight;						//远程操作权限
	TI_U8		u8Res[28];
}TI_DEV_USER_CFG_S, *LPTI_DEV_USER_CFG_S;

//ddns 参数信息
typedef struct _TI_DDNS_CFG_S
{
	TI_U32		u32Size;
	TI_U8		u8DDNSOpen;						//DDNS开关
	TI_U8		u8DDNSType;						//0-dyndns 1-3322
	TI_U16		u16DDNSPort;					//DDNS服务器端口
	TI_CHAR		szDDNSAccount[TI_NAME_LEN];		//DDNS 注册名
	TI_CHAR		szDDNSUsr[TI_USER_NAME_LEN];	//用户名称
	TI_CHAR		szDDNSPsw[TI_PASS_WORD_LEN];	//用户密码	
	TI_U32		u32DDNSTimes;					//更新时间
	TI_U8		u8Res[16];
}TI_DDNS_CFG_S,*LPTI_DDNS_CFG_S;

//======================================================================
//ftp参数设置
//
typedef struct _TI_FTP_CFG_S
{
	TI_U32		u32Size;
	TI_CHAR 	szFtpSvr[TI_IP_ADDR_LEN]; //主ftp服务器ip
	TI_CHAR     szFtpUsr[TI_USER_NAME_LEN];//登陆用户名
	TI_CHAR     szFtpPsw[TI_PASS_WORD_LEN];//登陆密码
	TI_CHAR     szFtpPath[TI_PATH_LEN];   //Ftp上传路径
	TI_U16		u16FtpPort;
	TI_U8		u8Rsvd[30];
}TI_FTP_CFG_S, *LPTI_FTP_CFG_S;

//===================================================================
//ntp 参数结构设置
typedef struct _TI_NTP_CFG_S
{
	TI_U32		u32Size;
	TI_U8		u8NtpOpen;				//ntp开关
	TI_U8		u8Resv;
	TI_U8		u8EuroTime;				//==1:启动夏令时 0:不启动夏令时
	TI_U8		u8NtpRefTime;			//ntp 定时对时时间(单位小时)
	int			nTimeZone;				//时区 单位s(-12*3600~11*3600)
	TI_CHAR     szNtpServer[TI_IP_ADDR_LEN];
	TI_U8		u8Res[16];
}TI_NTP_CFG_S, *LPTI_NTP_CFG_S;

//===========================================================================
//upnp 参数结构
//
typedef struct _TI_UPNP_CFG_S
{
	TI_U32		u32Size;
	TI_U8		u8UpnpOpen;		//upnp 开关
	TI_U8		u8UpnpEthNo;	//0使用本机网卡映射 1 使用无线网卡映射
	TI_U8		u8UpnpModel;    //0固定映射 1自动映射
	TI_U8		u8UpnpRefTime;  //upnp 刷新时间(单位小时)
	TI_U32		u32UpnpSvr;		//upnp 服务器IP
	TI_U16		u16UpnpDataPort;//固定映射模式下, 数据端口的外网端口
	TI_U16		u16UpnpWebPort; //固定映射模式下, Web端口的外网端口
	TI_U16		u16UpnpDataPortS;//数据端口的外网端口 0代表没有映射或不成功
	TI_U16		u16UpnpWebPortS;//Web端口的外网端口 0代表没有映射或不成功
	TI_U8		u8Res[16];
}TI_UPNP_CFG_S, *LPTI_UPNP_CFG_S;

//smtp设置
typedef struct _TI_SMTP_CFG_S
{
	TI_U32		u32Size;
	char        szMailHub[TI_IP_ADDR_LEN]; 		//smtp 服务器地址
	char        szMailAddr[TI_MAX_EMAIL_LEN];	//发送邮件 地址
	char        szMailUsr[TI_MAX_EMAIL_LEN]; 	//发送邮件 账号
	char        szMailPsw[TI_PASS_WORD_LEN];  	//发送邮件 密码
	char        szMailRctp1[TI_MAX_EMAIL_LEN];	//接受邮件 地址1
	char        szMailRctp2[TI_MAX_EMAIL_LEN];	//接受邮件 地址2
	char       	szMailRctp3[TI_MAX_EMAIL_LEN];	//接受邮件 地址3
	TI_U8		u8EnableSSL;						//启用SSL
	TI_U8		u8Rsvd;
	TI_U16		u16MailPort;						//smtp 服务器端口
	TI_U8       u8Rsvd2[32];
}TI_SMTP_CFG_S, *LPTI_SMTP_CFG_S;

//定时红外模式设置
typedef struct _TI_DEV_IRCFMODE_S_
{
	TI_U32 				u32Size;
	TI_U8				u8Enable;							//是否开启定时红外模式 0-关闭，1-开启
	TI_SEGMENT_S		struSeg[TI_MAX_IRCF_SEG_NUM][8];  	//0--每天1--星期天 2--星期一7--星期六	
	TI_U8				u8Res1[32];
}TI_DEV_IRCFMODE_S;


typedef struct _TI_DEV_JXJ_PLATFORM_CFG_S_
{
	TI_U32	u32Size;
	char 	szPuId[TI_NAME_LEN];
	char	szCmsIp[TI_IP_ADDR_LEN];
	TI_U16	u16CmsPort;
	TI_U8	u8Enable;
	TI_U8	u8ProtocolType;
	TI_U8	u8SyncTimeEnable;
	TI_U8 	u8Res1[61];
}TI_DEV_JXJ_PLATFORM_CFG_S;

#define HXHT_PRESET_NUM 32
typedef struct _TI_DEV_HXHT_PLATFORM_CFG_S_
{
	TI_U32 u32Size;
	char szPuId[TI_NAME_LEN];
	char szCmsIp[TI_IP_ADDR_LEN];
	char szFtpIp[TI_IP_ADDR_LEN];
	char szFtpUser[TI_NAME_LEN];
	char szFtpPsw[TI_NAME_LEN];
	TI_U8 u8MaxConnectNum;
	char szPresetName[HXHT_PRESET_NUM][16];
	TI_U8 u8Res1[64];
}TI_DEV_HXHT_PLATFORM_CFG_S;

#define GB28181_ID_LEN			24
#define GB28181_IP_LEN			16
#define GB28181_PSW_LEN			32
typedef struct _TI_DEV_GB28181_PLATFORM_CFG_S
{
	TI_U32 			u32Size;
	char  			szServerId[GB28181_ID_LEN];
	char  			szServerIp[GB28181_IP_LEN];
	unsigned short 	ServerPort;
	char            szPuId[GB28181_ID_LEN];
	char            szPassword[GB28181_PSW_LEN];
	int             expire;
	TI_U8 			u8Res1[128];
}TI_DEV_GB28181_PLATFORM_CFG_S;
/* begin:	add by hjl	2013/1/7*/
#define J_SDK_MAX_PRESET_PORT_SIZE 			255
#define J_SDK_MAX_NAME_LEN					64

typedef struct _TI_PRESET_POINT_S
{
	TI_U8	name[J_SDK_MAX_NAME_LEN];	//预置点名称
	TI_U32	preset;						//预置点号，
}TI_PRESET_POINT_S, *LPTI_PRESET_POINT_S;

typedef struct _TI_DEV_PP_SET_CFG_S
{
	TI_U32	u32Size;
	TI_U32	pp_count;								//预置点数量
	TI_PRESET_POINT_S pp[J_SDK_MAX_PRESET_PORT_SIZE];//预置点集合
}TI_DEV_PP_SET_CFG_S;
/* end:	add by hjl	2013/1/7*/
typedef struct _TI_DEV_ONVIF_CFG_S_
{
	TI_U32	u32Size;
	TI_U8	u8Disconvery;
	char 	szDomain[32];
	TI_U8 	u8Res1[64];
}TI_DEV_ONVIF_CFG_S;

typedef struct _TI_WIFI_STR_CFG_S
{
	TI_U32	u32Size;
	char					szEssid[TI_NAME_LEN];   //账号
	char					szPsw[TI_PASS_WORD_LEN];//密码
	TI_U8 	u8Res1[64];
}TI_WIFI_STR_CFG_S;

typedef struct _TI_WIFI_CFG_S
{
	char		szEssid[TI_NAME_LEN];   		//账号
	char		szPsw[TI_PASS_WORD_LEN];		//密码
	char 		szAp_macaddr[TI_NAME_LEN];		//Mac地址
	char 		szAuthMode[TI_NAME_LEN];		//身份模式
	char 		szEncryption[TI_NAME_LEN];		//加密模式
	int 		nSignal;						//信号强度:0-100
	int 		nStatus;						//0-未连接，1-已连接
	TI_U8		u8Res[4];		
}TI_WIFI_CFG_S;

typedef struct _TI_WIFI_ESSIDCFG_S
{
	TI_U32	u32Size;
	TI_U8	u8ScanResult;	//0-成功，1-失败
	TI_U8	u8WifiConStatus;//wifi连接状态:1--已连接，0-未连接
	TI_U8	u8Count;		//扫描节点个数
	TI_U8 	u8Res1;
	TI_WIFI_CFG_S Essid[TI_MAX_WIF_ESSID_NUM]; //最大支持节点扫描节点个数:10个
	TI_U8 	u8Res2[64];
}TI_WIFI_ESSID_CFG_S;


typedef struct _TI_DEV_PART_INFO_S_
{
	TI_U8	u8Mount;		//0：未挂载，  1：已挂载
	TI_U8	u8Format;		//0: 未格式化，1：已格式化
	TI_U8	u8PartType;		//0：录像分区  1：备份分区
	TI_U8	u8Use;			//0：未使用，  1：正在使用
	TI_U8	u8Res[4];
	TI_U64	u64TotalSize;	//分区容量
	TI_U64	u64FreeSize;	//分区剩余容量	
}TI_DEV_PART_INFO_S, *LPTI_DEV_PART_INFO_S;

typedef struct _TI_DISK_INFO_S_
{
	TI_U8	u8Parted;		//是否已经分区，0：未分区，1：已经分区
	TI_U8	u8FsType;		//文件系统类型，0：EXT2 ， 1：EXT3，2：FAT32，3：NFS
	TI_U8	u8PartNum;		//分区数量
	TI_U8	u8RecStatus;	// === 1 正在录像0 没有录像 
	TI_U8	u8Res[4];		//res
	TI_U64	u64TotalSize;	//硬盘容量
	TI_U64	u64FreeSize;	//剩余容量
	TI_DEV_PART_INFO_S stPart[TI_MAX_SUPPORT_PART];//分区信息	
}TI_DEV_DISK_INFO_S, *LPTI_DEV_DISK_INFO_S;

typedef struct _TI_DEV_HD_INFO_S_
{
	TI_U32				u32Size;
	TI_U8				u8DiskNum;
	TI_U8				u8Res[3];
	TI_DEV_DISK_INFO_S	stDiskInfo[TI_MAX_SUPPORT_DISK];
}TI_DEV_HD_INFO_S, *LPTI_DEV_HD_INFO_S;

typedef struct _TI_DEV_REC_STRC_S_
{
	TI_U32		u32Size;		
	TI_U8		u8OverWrite;		//0 不覆盖1 覆盖
	TI_U8		u8RecStream;		//0 主码流录像1 次码流录像
	TI_U8		u8PreRecSecs;		//预录时间(秒)
	TI_U8		u8SaveMode;			//保存模式

	//定时录像策略
	TI_SEGMENT_S	struTimerSeg[TI_MAX_SEG_NUM][8]; //0--每天1--星期天 2--星期一7--星期六	
}TI_DEV_REC_STRC_S, *LPTI_DEV_REC_STRC_S;
// ==========================================================
// 高速球配置
//
#define TI_BALL_ONTIME_NUM 8
// 报警参数
typedef struct _TI_BALL_ALARM_PARAM_S_
{
	int nAlarmActionIndex;		// 报警动作
	int nDurationSec;			// 报警持续时间
	TI_U8 byRes[32];
}TI_BALL_ALARM_PARAM_S, *LPTI_BALL_ALARM_PARAM_S;
// 定时动作
typedef struct _TI_BALL_ONTIME_ACTION_S_
{
	TI_U8 byBeginHour;					// 开始小时
	TI_U8 byBeginMin;					// 开始分钟
	TI_U8 byEndHour;					// 结束小时
	TI_U8 byEndMin;						// 结束分钟
	int nActionIndex;
	TI_U8 byRes[12];
}TI_BALL_ONTIME_ACTION_S, *LPTI_BALL_ONTIME_ACTION_S;
// 定时参数
typedef struct _TI_BALL_ONTIME_PARAM_S_
{
	TI_BALL_ONTIME_ACTION_S struOntimeAction[TI_BALL_ONTIME_NUM];
	TI_U8 byRes[32];
}TI_BALL_ONTIME_PARAM_S, *LPTI_BALL_ONTIME_PARAM_S;
// 巡航点
typedef struct _TI_BALL_CRUISE_NODE_S_
{
	TI_U8 byCruisePresetEnable;		// 是否使用这个预置点 
	TI_U8 byCruisePresetSec;		// 停留时长(秒)
	TI_U8 byCruisePresetSpeed;		// 移动速度
	TI_U8 byRes[8];
}TI_BALL_CRUISE_NODE_S, *LPTI_BALL_CRUISE_NODE_S;
// 巡航参数
#define TI_BALL_CRUISE_NUM 16
#define TI_BALL_CRUISE_NAME_LEN 32
typedef struct _TI_BALL_CRUISE_PARAM_S_
{
	TI_BALL_CRUISE_NODE_S struCruiseNode[TI_BALL_CRUISE_NUM];
	TI_BALL_CRUISE_NODE_S struCruiseResNode[8];
	/* begin:	add by hjl	2013/1/7*/
	char  crz_name[TI_BALL_CRUISE_NAME_LEN];	//巡航路径名
	unsigned int crz_no;						//巡航路径号
	unsigned int crz_enable;					//是否使用这条路径
	/* end:	add by hjl	2013/1/7*/
	unsigned char byRes[64-40];
}TI_BALL_CRUISE_PARAM_S, *LPTI_BALL_CRUISE_PARAM_S;
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
typedef struct _TI_BALL_SETTING_PARAM_S_
{
	TI_U32 dwMask;				// 0: 不支持,1:支持
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
	TI_BALL_CRUISE_PARAM_S struCruiseParam;
	TI_U8 byAutoScanSpeed;			// 自动扫描速度
	TI_U8 byABScanSpeed;			// AB扫描速度
	int nIdleActionIndex;			// 闲置动作
	TI_U8 byIdleSec;				// 闲置秒数
	TI_U8 byIRISMode;				// 光圈模式:0:手动,1:自动
	TI_U8 byAutoFlip;				// 自动翻转:0:关闭,1:打开
	TI_U8 byDayNightMode;			//0:白天模式,1:夜晚模式,2:自动模式,3:定时模式
	TI_U8 byIRMode;					// 远近灯切换模式:0:自动切换远近灯,1:同时开启远近灯
	TI_U8 byBLCMode;				// 背光补偿:0:关闭,1:打开
	TI_U8 byFrameNoise;				// 降噪:0:关闭,1:打开
	TI_U8 byZoomDisplay;			// 变倍显示:0:关闭,1:打开
	TI_U8 byZoomSpeed;				// 变倍速度:0:快速,1:中速,2:低速
	TI_U8 byWBMode;					// 白平衡:0:室外,1:室内 ,2:自动,3:手动
	TI_U8 byWDRMode;				// 宽动态:0:关闭,1:打开
	TI_U8 byAutoFocus;				// 自动聚焦:0:关闭,1:打开
	TI_U8 byRes[32];
}TI_BALL_SETTING_PARAM_S, *LPTI_BALL_SETTING_PARAM_S;
// 高速球配置
typedef struct _TI_BALL_RUN_PARAM_
{
	TI_U32 dwSize;
	TI_BALL_ALARM_PARAM_S struAlarmParam;
	TI_BALL_ONTIME_PARAM_S struOntimeParam;
	TI_BALL_SETTING_PARAM_S struSettingParam;
	TI_U8 byRes[64];
}TI_BALL_RUN_PARAM_S, *LPTI_BALL_RUN_PARAM_S;

//3A参数
//--3A参数设置类型定义
typedef enum _TI_DEV_3A_MASK_E_
{	//光圈
	VCT_3A_AUTOIRIS		= 0x00000001,//自动光圈
	VCT_3A_IRISLEVEL	= 0X00000002,//光圈电平
	//聚焦
	VCT_3A_AUTOFOCUS	= 0X00000004,//自动聚焦
	VCT_3A_FOCUSLEVEL	= 0X00000008,//焦距
	VCT_3A_MINFOCUSLEN 	= 0X00000010,//设置最小聚焦距离		
	//变倍
	VCT_3A_ZOOMSPEED	 = 0X00000020,//变倍速度
	VCT_3A_AUTOPTZSPEED  = 0X00000040,//景深比例变速
	//电平控制
	VCT_3A_AUTOALC		= 0X00000080,//自动电平控制
	VCT_3A_ALCLEVEL		= 0X00000100, //电平控制值
	//彩转黑
	VCT_3A_CTB			= 0X00000200,//彩转黑
	VCT_3A_CTBLEVEL		= 0X00000400,//彩转黑阀值，当彩转黑为自动时有效
	//场景
	VCT_3A_MIRROR		= 0X00000800,//镜向
	VCT_3A_FLIP			= 0X00001000,//翻转
	VCT_3A_AUTOFLIP		= 0X00002000,//自动翻转
	VCT_3A_PWDFREQ		= 0X00004000,//照明频率1
	//红外
	VCT_3A_IREXIST		= 0X00008000,//是否有红外
	VCT_3A_IRCFMODE		= 0X00010000,//IRCF模式
	VCT_3A_IRLIGHTTYPE	= 0X00020000,//红外灯类型
	//
	VCT_3A_WDR 			= 0X00040000,//是否有宽动态
	VCT_3A_WDRLEVEL		= 0X00080000,//宽动态的值
	VCT_3A_LOWILLUM		= 0X00100000,//低照度
	VCT_3A_IMAGEMODE	= 0X00200000,//图像模式
	//场景
	VCT_3A_SCENE		=  0X00400000,//场景
	
	VCT_3A_ALL		  	= 0xFFFFFFFF,
}TI_DEV_3A_MASK_E;
typedef struct _TI_DEV_3A_PARA_S_
{
	TI_U8	u8Value;		//参数值
	TI_U8	u8Default;		//缺省参数值
	TI_U8	u8Step;			//参数值步进值
	TI_U8	u8Res;
}TI_DEV_3A_PARA_S;

typedef struct _TI_DEV_3A_CFG_S_
{
	TI_U32				u32Size;
/*有效支持的参数，该参数设置时候无效
在获取的时候有效，用以判断设备是否支持该参数*/	
	TI_DEV_3A_MASK_E	eSuppMask;
/*设置有效的参数，该参数获取时候无效
   在设置的时候有效，用以指定具体设置的参数*/	
	TI_DEV_3A_MASK_E	eSetMask;	
	TI_DEV_3A_PARA_S	struFlip;			//翻转图像上下翻转
	TI_DEV_3A_PARA_S	struMirror;			//镜像图像左右翻转
	TI_DEV_3A_PARA_S	struPwdFreq;		//电源频率
	TI_DEV_3A_PARA_S	struScene;			//场景,0:自动 1:室内2:室外
	TI_DEV_3A_PARA_S	struIrcfMode;		//红外模式，0:自动，1:白天，2:晚上,3:定时模式
	TI_DEV_3A_PARA_S	struShutterSpeed;	//电子快门 0-8  0：自动快门 1：
											//1/25S， 2：1/50S， 3：1/100S， 4：1/500S， 5：1/1000S， 
											//6：1/2000S， 7：1/5000S， 8：1/10000S
	TI_DEV_3A_PARA_S	struRes[26];
}TI_DEV_3A_CFG_S, *LPTI_DEV_3A_CFG_S;

//定时红外模式设置
typedef struct _TI_DEV_IRCFMODE_CFG_S_
{
	TI_U32 				u32Size;
	TI_U8				u8Enable;						//是否开启定时红外模式 0-关闭，1-开启
	TI_SEGMENT_S		struSeg[TI_MAX_IRCF_SEG_NUM][8];  //0--每天1--星期天 2--星期一7--星期六	
	TI_U8				u8Res1[32];
}TI_DEV_IRCFMODE_CFG_S;

#define DBG_LINE	printf("run line [%s %d]\n", __FILE__, __LINE__);

#ifdef __cplusplus
}
#endif



#endif
