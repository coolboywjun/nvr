#ifndef _TI_NET_STRUCT_H_
#define _TI_NET_STRUCT_H_
#include "ti_type.h"
#include <netinet/in.h>

#define TI_USER_NAME_LEN		32	//用户名长度	
#define TI_PASS_WORD_LEN		32	//密码长度	
#define TI_MAX_SUPPORT_CHN		1	//最大支持通道数
#define TI_MAC_ADDR_LEN			6	//mac地址长度
#define TI_SERIALNO_LEN			48	//序列号长度
#define TI_DEV_NAME_LEN			32	//设备名称长度


/*音视频包包头*/
#define	TI_NET_STREAM_DATA_LEN	1360



typedef struct _TI_NET_HDR_S
{
	TI_U16	u16Version;
	TI_U16	u16CmdType;
	TI_U32   u32PacketSize;
	TI_U32	u32MagicFlag;
	TI_U32	u32PacketNo;
	TI_S32	s32ErrNo;
	TI_U8	u8Chn;
	TI_U8	u8Res[3];
}TI_NET_HDR_S, *LPTI_NET_HDR_S;
#define TI_NET_HDR_LEN (sizeof(TI_NET_HDR_S))

/* 因为需要与枪机的PTZ控制相兼容,所以结构体的大小必须与之前的大小保持一致*/
#pragma pack(2)
typedef struct _TI_NET_CTRL_PTZ_S_
{
	TI_U16	u16PtzCmd;		//云台控制命令

	union
	{
		struct
		{
			TI_U8	u8Chn;			//云台通道号
			TI_U8	u8Speed;		//速度
			TI_U8	u8PresetNo;
			TI_U8	u8Res[3];
		}struPreset;
		struct 
		{
			TI_U8	u8CruisePresetNo;
			TI_U8 	u8CruisePresetEnable;
			TI_U8	u8CruisePresetSec;
			TI_U8	u8CruisePresetSpeed;
		}struCruise;
		struct
		{
			TI_U8 u8SetVerticalPos;	// 是否设置垂直位置
			TI_U8	u8Speed;
		}struAutoScan;
		struct
		{
			TI_U8 u8SetVerticalPos;	// 是否设置垂直位置
			TI_U8	u8Speed;
		}struABScan;
		struct
		{
			TI_U8	u8IdleActionIndex;
			TI_U8	u8IdleSec;
		}struIdleAction;
		struct
		{
			TI_U8	u8OntimeIndex;			// 1-8的某个定时点
			TI_U8	u8OntimeActionIndex;	// 定时动作的索引
			TI_U8	u8BeginHour;
			TI_U8	u8BeginMin;			
			TI_U8	u8EndHour;
			TI_U8	u8EndMin;
		}struOntimeAction;
		struct
		{
			TI_U8	u8AlarmTypeIndex;		// 哪一种报警
			TI_U8	u8AlarmActionIndex;		// 报警动作的索引
			TI_U8 	u8AlarmDurationSec;
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
		TI_U8 u8OpenOrClose;	// 0:close, 1:open
		TI_U8 u8WorkMode;		// 0:手动,1:自动
		TI_U8 u8IRMode;			// 0:远近灯自动切换,1:远近灯同时开启
		TI_U8 u8DayNightMode;	// 0:白天模式,1:夜晚模式,2:自动模式
		TI_U8 u8WBMode;			// 0 室外,1 室内 ,2 自动,3 手动
		TI_U8 u8ZoomSpeed;		// 0 快速, 1 中速, 2 低速
		TI_U8 u8Bright;			// 亮度
		TI_U8 u8Contrast;		// 对比度
		TI_U8 u8Saturation;		// 饱和度
		TI_U8 u8Sharpness;		// 锐度
		TI_U8 u8Red;			// 红色
		TI_U8 u8Blue;			// 蓝色
		unsigned char u8Shutter;			// 电子快门速度
	};

}TI_NET_CTRL_PTZ_S, *LPTI_NET_CTRL_PTZ_S;
#pragma pack()


/*请求登录包*/
typedef struct _TI_NET_LOGON_S
{
	TI_CHAR	szUsr[TI_USER_NAME_LEN];	/*登录用户名*/
	TI_CHAR szPsw[TI_PASS_WORD_LEN];	/*登录密码*/
//	TI_U8	u8RecvAlarm;				/*是否接受报警信息*/
//	TI_U8	u8RecvCommData;				/*是否接受串口采集信息*/
	TI_U8	u8Res[20];					/*保留字节*/
}TI_NET_LOGON_S, *LPTI_NET_LOGON_S;


/*搜索请求包*/
typedef struct _TI_NET_SEARCH_REQ_S_
{
	TI_U32	u32CliMultiIp;
	TI_U16	u16CliMultiPort;
    TI_U8	u8Res[10];
}TI_NET_SEARCH_REQ_S, *LPTI_NET_SEARCH_REQ_S;
	
/*搜索响应包*/
typedef struct _TI_NET_SEARCH_RES_S_
{
	struct sockaddr_in	addrClient;
	
	TI_U8	u8SerialId[TI_SERIALNO_LEN];
	TI_U32	u32SoftVer;
	TI_U16	u16DevType;
	TI_U16	u16DevPort;
	TI_U32	u32DevIp;
	TI_U8	u8MacAddr[TI_MAC_ADDR_LEN];
	TI_U8	u8Dns;
	TI_U8	u8DHCP;	
	TI_U32	u32DevIpMask;
	TI_U32	u32DevGwAddr;
	TI_U32	u32DevDns1;
	TI_U32	u32DevDns2;
	TI_U32 	u32Capability0;
	TI_U32 	u32Capability1;
	TI_U32 	u32Capability2;
	TI_U8	u8MainDevType;
	TI_U8	u8SubDevType;
	TI_U16  u16PlateformCap;  //支持平台
	TI_U32  u32UpTime;        //启动时间
}TI_NET_SEARCH_RES_S, *LPTI_NET_SEARCH_RES_S;

/*搜索配置包*/
typedef struct _TI_NET_SEARCH_CFG_REQ_S
{	
	TI_U32	u32CliMultiIp;
	TI_U16	u16CliMultiPort;
	TI_U8	u8Res[2];
	TI_CHAR szUsrName[TI_USER_NAME_LEN];
	TI_CHAR szPsw[TI_PASS_WORD_LEN];
	TI_U8	u8SerialId[TI_SERIALNO_LEN];
	TI_U32	u32SoftVer;
	TI_U16	u16DevType;
	TI_U16	u16DevPort;
	TI_U32	u32DevIp;
	TI_U8	u8MacAddr[TI_MAC_ADDR_LEN];
	TI_U8	u8Dns;
	TI_U8	u8DHCP;	
	TI_U32	u32DevIpMask;
	TI_U32	u32DevGwAddr;
	TI_U32	u32DevDns1;
	TI_U32	u32DevDns2;
	TI_U32 	u32Capability0;
	TI_U32 	u32Capability1;
	TI_U32 	u32Capability2;	  //实际开启平台,syscfg获取
	TI_U8	u8MainDevType;
	TI_U8	u8SubDevType;
	TI_U16  u16PlateformCap;  //能够支持平台，应用层填充
	TI_U32  u32UpTime;        //启动时间
	
}TI_NET_SEARCH_CFG_REQ_S, *LPTI_NET_SEARCH_CFG_REQ_S; 

/*搜索配置响应包*/
typedef struct _TI_NET_SEARCH_CFG_RES_S_
{
	struct sockaddr_in	addrClient;
}TI_NET_SEARCH_CFG_RES_S, *LPTI_NET_SEARCH_CFG_RES_S;
/**升级百分比响应包*/
typedef struct _TI_NET_UPDATE_PRECENT_RES_S_
{
	TI_U8	u8Percent;
	TI_U8	u8Res[31];
}TI_NET_UPDATE_PRECENT_RES_S, *LPTI_NET_UPDATE_PRECENT_RES_S;

/*请求预览包*/
typedef struct _TI_NET_PREV_REQ_S
{
	TI_CHAR	szUsr[TI_USER_NAME_LEN];		/*预览用户名*/
	TI_CHAR szPsw[TI_PASS_WORD_LEN];		/*预览密码*/
	TI_U8	u8Ch;							/*预览视频通道号*/
	TI_U8	u8StreamType;					/*预览码流类型*/
	TI_U8	u8ProtocalType;					/*预览协议类型*/
	TI_U8	u8Res1[1];						/*保留字节*/
	TI_U32	u32DstIp;						/*预览目的ip 地址*/
	TI_U16  u16DstPort;						/*预览目的端口*/
	TI_U8	u8Res2[18];						/*保留字节*/
}TI_NET_PREV_REQ_S, *LPTI_NET_PREV_REQ_S;

//数据包头格式
//========================================
//网络音视频分包头
typedef struct _TI_NET_STREAM_HDR_S
{
	TI_U32		u32MagicFlag;	//魔术字
	TI_U32		u32FrameNo;
	TI_U16		u16Version;		//版本信息
	TI_U16		u16PacketSum;	//分片头
	TI_U32		u32FrameLen;	//帧长度
	TI_U8		u8FrameType;	//VIDEO_FRAME_I = 0x7,
								//VIDEO_FRAME_P = 0x8,
								//VIDEO_FRAME_B = 0xB,
								//AUDIO_FRAME_A = 0xA, 
	TI_U8		u8Res[3];	
}TI_NET_STREAM_HDR_S, *LPTI_NET_STREAM_HDR_S;

//分片头//音视频数据
//========================================
//========================================
//分片头//音视频数据
//========================================
typedef struct _TI_NET_STREAM_PIS_HDR_S
{
	TI_U32		u32MagicFlag;	//魔术字
	TI_U32		u32FrameNo;
	TI_U32		u32FrameOff;
	TI_U16		u16PacketNo;
	TI_U16		u16PacketLen;
	TI_U8		u8Res[4];
}TI_NET_STREAM_PIS_HDR_S, *LPTI_NET_STREAM_PIS_HDR_S;



/* 系统错误代码 */
typedef enum _TI_ERR_CODE_
{
	TI_ERR_SUCCESS   			= 0,			//成功
	TI_ERR_USER_NO_EXIST		= -1,			//用户名不存在
	TI_ERR_PASSWORD 			= -2,			//密码错误
	TI_ERR_NO_PERMIT			= -3,			//无权限
	TI_ERR_NO_INIT				= -4,			//没有初始化
	TI_ERR_CHANNEL				= -5,			//通道号错误
	TI_ERR_OVER_MAX_LINK		= -6,			//超过最大连接数
	TI_ERR_VERSION_NO_MATCH		= -7,			//协议版本错误
	TI_ERR_CREATE_SOCKET		= -8,			//建立套接字错误
	TI_ERR_SET_SOCKET			= -9,			//设置套接字属性错误
	TI_ERR_LISTEN_FAIL			= -10,			//侦听错误
	TI_ERR_BIND_FAIL			= -11,			//绑定错误
	TI_ERR_CONNECT_FAIL 		= -12,			//连接失败
	TI_ERR_SEND_FAIL			= -13,			//发送失败
	TI_ERR_RECV_FAIL			= -14,			//接收失败
	TI_ERR_RECV_TIME_OUT	 	= -15,			//接收超时
	TI_ERR_ORDER_ERR	 		= -16,			//调用顺序错误
	TI_ERR_PARAMETER			= -17,			//参数错误
	TI_ERR_NO_DISK				= -18,			//无磁盘
	TI_ERR_DISK_NUM				= -19,			//磁盘数错误
	TI_ERR_DISK_FULL			= -20,			//磁盘满错误
	TI_ERR_DISK_ERR				= -21,			//磁盘出错
	TI_ERR_NO_SUPPORT			= -22,			//操作、命令不支持
	TI_ERR_DEV_BUSY				= -23,			//设备忙错误
	TI_ERR_MODIFY_FAIL			= -24,			//修改失败
	TI_ERR_DISK_FORMATING		= -25,			//磁盘正在格式化
	TI_ERR_NO_RESOURCE			= -26,			//无资源错误
	TI_ERR_OPERATE_FAIL			= -27,			//操作失败
	TI_ERR_VOICE_OPEN			= -28,			//打开音频失败
	TI_ERR_INPUT_TIME_ERR		= -29,			//错误的输入时间
	TI_ERR_NO_FILE				= -30,			//无文件
	TI_ERR_FILE					= -31,			//错误的文件信息
	TI_ERR_ALLOC_RESOURCE		= -32,			//申请资源失败
	TI_ERR_WRITE_FLASH			= -33,			//写Flash 失败
	TI_ERR_PROTACAL				= -34,			//错误的协议
	TI_ERR_MANULE_STOP			= -35,			//手动停止
	TI_ERR_DATA_FORMAT			= -36,			//错误的数据格式
	TI_ERR_DATA_SIZE			= -37,			//错误的数据长度
	TI_ERR_NO_MORE_FILE			= -38,			//没有更多文件 查找结束
	TI_ERR_IS_FINDING			= -39,			//正在查找
}TI_ERR_CODE;


#endif /*  #ifndef _TI_NET_STRUCT_H_ */
