#ifndef __JP2P_DEF_H__
#define	__JP2P_DEF_H__

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#ifdef	JP2P_EXPORT
	#define JP2P_TUTK_API		extern "C" 
#else
	#define JP2P_TUTK_API
#endif
#define JP2P_PRO_VERSION	0x0100			// 协议版本号
#define JP2P_MAGIC_FLAG0	0x4a503250		// 魔字首
#define JP2P_MAGIC_FLAG1	0x5032504a		// 魔字尾
#define	JP2P_MAX_CHN		64				// 最多64通道
#define	JP2P_MAX_CODE		3				// 最多三码流
#define	JP2P_MAX_CLIENT		4				// 最多四个客户端接入
#define	JP2P_MAX_REQ		12				// 最多请求个数(平均一个客户端对应3路请求)
#define	JP2P_BREATH_SPAN	10				// 心跳间隔
#define JP2P_USER_NAME_LEN	16				// 用户名称长
#define JP2P_USER_PWD_LEN	16				// 用户密码长
#define	JP2P_HOST_COUNT		4				// P2P服务器个数
#define	JP2P_HOST_NAME		64				// P2P服务器IP地址

#if (defined _WIN32) || (defined _WIN64)
#else
	typedef	int64_t			__int64;
#endif
//typedef byte				jdt_bool;
typedef char				jdt_char;
typedef char*				jdt_lpchar;
typedef const char			jdt_cchar;
typedef const char*			jdt_lpcchar;
typedef unsigned char		jdt_byte;
typedef unsigned char*		jdt_lpbyte;
typedef short				jdt_short;
typedef short*				jdt_lpshort;
typedef int					jdt_int;
typedef int*				jdt_lpint;
typedef unsigned int		jdt_uint;
typedef unsigned int*		jdt_lpuint;
typedef long				jdt_long;
typedef long*				jdt_lplong;
typedef unsigned short		jdt_word;
typedef unsigned short*		jdt_lpword;
typedef unsigned long		jdt_dword;
typedef unsigned long*		jdt_lpdword;
typedef __int64				jdt_l64;
typedef __int64*			jdt_lpl64;
typedef void*				jdt_lpvoid;

// 定义数据命令 begin
typedef enum
{
	eJP2PCmdUnknown			= 0,			// 未知命令
	eJP2PCmdBreath			= 1,			// 呼吸,			stJP2PBreath
	eJP2PCmdLogout			= 2,			// 登出命令,		stJP2PLogout
	eJP2PCmdLogin			= 3,			// 登录命令,		stJP2PLogin
	eJP2PCmdGetParam		= 4,			// 获取参数命令,	stJP2PGetParam
	eJP2PCmdSetParam		= 5,			// 设置参数命令,	stJP2PSetParam
	eJP2PCmdReqStream		= 6,			// 请求流命令,		stJP2PReqStream
	eJP2PCmdCloseStream		= 7,			// 结束流命令,		stJP2PCloseStream
	eJP2PCmdCtrlStream		= 8,			// 控制流命令,		stJP2PCtrlStream
	eJP2PCmdReqTalk			= 9,			// 请求对讲命令,	stJP2PReqTalk
	eJP2PCmdCloseTalk		= 10,			// 结束对讲命令,	stJP2PCloseTalk
	eJP2PCmdDevNotify		= 11,			// 设备通知命令,	stJP2PDevNotify
	eJP2PCmdStreamData		= 100,			// 数据信息,		stJP2PStreamData
	eJP2PCmdStreamEnd		= 101,			// 数据信息结束命令,stJP2PStreamEnd
}eJP2PCmd;
// 定义数据命令 end

// 返回值类型定义 begin
typedef enum
{
	eJP2PETSuccess			= 0,			// 返回成功
	eJP2PETOperator			= -1,			// 操作失败
	eJP2PETParam			= -2,			// 参数错误
	eJP2PETInit				= -3,			// 未初始化
	eJP2PETNoMem			= -4,			// 内存不足
	eJP2PETDeal				= -5,			// 正在处理中
	eJP2PETMaxClient		= -6,			// 客户端已经满
	eJP2PETUserNotExist		= -1000,		// 用户不存在
	eJP2PETUserPsw			= -1001,		// 用户密码错误
	eJP2PETRight			= -1003,		// 用户权限不足 
}eJP2PErrType;
// 返回值类型定义 end

// 流类型定义 begin
typedef enum
{
	eJP2PStreamUnknown		= 0,			// 未知类型
	eJP2PStreamReal			= 1,			// 实时流
	eJP2PStreamHistore		= 2,			// 历史流
	eJP2PStreamTalk			= 3,			// 对讲流
}eJP2PStreamType;
// 流类型定义 end

// 定义播放速度 begin
typedef enum
{
	eJP2PPlaySpeed_1_16X	= 0x1,			// 慢16倍速
	eJP2PPlaySpeed_1_8X		= 0x2,			// 慢8倍速
	eJP2PPlaySpeed_1_4X		= 0x3,			// 慢4倍速
	eJP2PPlaySpeed_1_2X		= 0x4,			// 慢2倍速
	eJP2PPlaySpeed_1X		= 0x5,			// 正常速度
	eJP2PPlaySpeed_2X		= 0x6,			// 快2倍速
	eJP2PPlaySpeed_4X		= 0x7,			// 快4倍速
	eJP2PPlaySpeed_8X		= 0x8,			// 快8倍速
	eJP2PPlaySpeed_16X		= 0x9,			// 快16倍速
	eJP2PPlaySpeed_32X		= 0x10,			// 快32倍速
	eJP2PPlaySpeed_0_1X		= 0x20,			// 快2倍速退
	eJP2PPlaySpeed_0_2X		= 0x21,			// 快2倍速退
	eJP2PPlaySpeed_0_4X		= 0x22,			// 快4倍速退
	eJP2PPlaySpeed_0_8X		= 0x23,			// 快8倍速退
	eJP2PPlaySpeed_0_16X	= 0x24,			// 快16倍速退
	eJP2PPlaySpeed_0_32X	= 0x25,			// 快32倍速退
}eJP2PPlaySpeed;
// 定义播放速度 end

// 流控制定义 begin
typedef enum
{
	eJP2PSCUnknwn			= 0,			// 未知类型
	eJP2PSCAudioOnly		= 1,			// 只放音频
	eJP2PSCIFrameOnly		= 2,			// 只放I帧
	
	eJP2PSCPBStart			= 100,			// 开始回放
	eJP2PSCPBStop			= 101,			// 停止回放
	eJP2PSCPBPause			= 102,			// 暂停回放
	eJP2PSCPBResume			= 103,			// 恢复回放
	eJP2PSCPBSetSpeed		= 104,			// 设置播放速度,m_dwCtrlValue1类型为速度值见 eJP2PPlaySpeed
	eJP2PSCPBGetSpeed		= 105,			// 获取播放速度,m_dwCtrlValue1类型为速度值见 eJP2PPlaySpeed
	eJP2PSCPBFrame			= 106,			// 单帧放
	eJP2PSCPBSetTime		= 107,			// 设置回放时间,m_dwCtrlValue1类型为time_t
	eJP2PSCPBGetTime		= 108,			// 获取回放时间,m_dwCtrlValue1类型为time_t
}eJP2PStreamCtrl;
// 流控制定义 end

// 数据帧类型定义 begin
typedef enum
{
	eJP2PFrameUnknown		= 0,			// 未知类型
	eJP2PFrameIFrame		= 0x1,			// I帧
	eJP2PFramePFrame		= 0x2,			// P帧
	eJP2PFrameBFrame		= 0x3,			// B帧
	eJP2PFrameAudio			= 0x4,			// 音频帧
	eJP2PFrameString		= 0x5,			// 字幕帧
	eJP2PFrameShape			= 0x6,			// 形状帧
	eJP2PFrameUser			= 0xF,			// 用户自定义帧
}eJP2PFrameType;
// 数据帧类型定义 end

// 编码类型定义 begin
typedef enum 
{ 
	eJP2PCodecH264			= 0,			// H264
	eJP2PCodecMJPEG			= 1,			// MJPEG
}eJP2PCodecVideo;
typedef enum 
{ 
	eJP2PCodecPCM			= 0,			// PCM
	eJP2PCodecG711A			= 1,			// G711A
	eJP2PCodecG711U			= 2,			// G711U
	eJP2PCodecG726			= 3,			// G726
}eJP2PCodecAudio;
// 编码类型定义 end


// 时间结构体 begin
typedef enum
{
	eJP2PSunday				= 0,			// 周日
	eJP2PMonday				= 1,			// 周一
	eJP2PTuesday			= 2,			// 周二
	eJP2PWednesday			= 3,			// 周三
	eJP2PHursday			= 4,			// 周四
	eJP2PFriday				= 5,			// 周五
	eJP2PSaturday			= 6,			// 周六
	eJP2PEveryday			= 7,			// 每天
}eJP2PWeekDay;
typedef struct stJP2PTime
{
	jdt_short	m_sYear;			// 年,如2014
	jdt_short	m_sMonth;			// 月,从0开始,即0~11
	jdt_short	m_sDay;				// 日,从0开始
	jdt_short	m_sHour;			// 时,24小时制,即0~23
	jdt_short	m_sMinute;			// 分,0~59
	jdt_short	m_sSecond;			// 秒,0~59
	jdt_short	m_sWeek;			// 星期,见 eJP2PWeekDay
}* PSTJP2PTime;
// 时间结构体 end

// 协议头 begin
typedef struct stJP2PProHead
{
	jdt_dword	m_dwMagicFlag0;		// 首魔术字
	jdt_word	m_wVersion;			// 版本号
	jdt_short	m_sCmdType;			// 命令类型,见 eJP2PCmd
	jdt_int		m_iDataSize;		// 包数据大小
	jdt_long	m_lSeq;				// 序列号
	jdt_long	m_lErrNo;			// 错误码,见 eJP2PErr
	jdt_dword	m_dwSession;		// 会话
	jdt_dword	m_dwSubSession;		// 子会话
	jdt_dword	m_dwRes[4];			// 保留位
	jdt_dword	m_dwMagicFlag1;		// 尾魔术字
	#ifdef __cplusplus
	void Reset()
	{
		m_dwMagicFlag0	= JP2P_MAGIC_FLAG0;
		m_wVersion		= JP2P_PRO_VERSION;
		m_sCmdType		= (jdt_short)0;
		m_iDataSize		= 0;
		m_lSeq			= 0;
		m_lErrNo		= 0;
		m_dwSession		= 0;		// 会话
		m_dwSubSession	= 0;		// 子会话
		memset(m_dwRes,0,sizeof(jdt_dword)*4);
		m_dwMagicFlag1	= JP2P_MAGIC_FLAG1;
	}
	stJP2PProHead()
	{
		Reset();
	}
	jdt_int CheckPacket()
	{
		if (m_dwMagicFlag0==JP2P_MAGIC_FLAG0 && m_dwMagicFlag1==JP2P_MAGIC_FLAG1) return 1;
		else return 0;
	}
	jdt_int SetNetHdr(jdt_dword dwSession,jdt_dword dwSubSession,jdt_int iDataSize, jdt_short sCmdType, jdt_long lErrNo)
	{
		Reset();
		m_dwSession		= dwSession;
		m_dwSubSession	= dwSubSession;
		m_iDataSize		= iDataSize;
		m_sCmdType		= sCmdType;
		m_lErrNo		= lErrNo;
		m_lSeq			= GetSeq();
		return m_lSeq;
	}
	static jdt_long GetSeq()
	{
		jdt_long	lSeq	= s_lSeq;
		s_lSeq++;
		if (s_lSeq>100000) s_lSeq	= 1;
		return lSeq;
	}
	static jdt_int		s_iSize;
	static jdt_long		s_lSeq;
	#endif	// __cplusplus
}* PSTJP2PProHead;
// 协议头 end

// 登录命令 begin
// eJP2PCmdLogin
typedef struct stJP2PLogin
{
	jdt_char	m_szUserName[JP2P_USER_NAME_LEN];	// 用户名
	jdt_char	m_szUserPwd[JP2P_USER_PWD_LEN];		// 密码
	jdt_int		m_iBreath;							// 呼吸时长
}* PSTJP2PLogin;
// 登录命令 end

// 登出命令 begin
// eJP2PCmdLogout
typedef struct stJP2PLogout
{
	jdt_dword	m_dwRes;							// 保留值
}* PSTJP2PLogout;
// 登出命令 end

// 呼吸 begin
// eJP2PCmdLogout
typedef struct stJP2PBreath
{
	jdt_dword	m_dwSendTick;						// 发送时的Tick
	jdt_dword	m_dwRecvTick;						// 接收时的Tick
}* PSTJP2PBreath;
// 呼吸 end

// 获取参数命令 begin
// eJP2PCmdGetParam
typedef struct stJP2PGetParam
{
	jdt_int		m_iParamID;							// 参数ID
	jdt_int		m_iChn;								// 通道号
	jdt_int		m_iDataSize;						// 数据大小
}* PSTJP2PGetParam;
// 获取参数命令 end

// 设置参数命令 begin
// eJP2PCmdSetParam
typedef struct stJP2PSetParam
{
	jdt_int		m_iParamID;							// 参数ID
	jdt_int		m_iChn;								// 通道号
	jdt_int		m_iDataSize;						// 数据大小
}* PSTJP2PSetParam;
// 设置参数命令 end

// 请求流命令 begin
//eJP2PCmdReqStream
typedef struct stJP2PReqStream
{
	jdt_int		m_iChn;								// 通道号
	jdt_int		m_iCode;							// 码流,0 主码流,1 次码流,2 三码流...
	jdt_int		m_iStreamType;						// 流类型,见 eJP2PStreamType
	struct stJP2PTime	m_stStartTime;						// 历史流的开始时间,实时流时忽略
	struct stJP2PTime	m_stStopTime;						// 历史流的结束时间,实时流时忽略
	jdt_dword	m_dwRecType;						// 历史流的类型,由协议自定义,实时流时忽略
	jdt_dword	m_dwSession;						// 会话SESSION,由服务器返回
}* PSTJP2PReqStream;
// 请求流命令 end

// 结束流命令 begin
// eJP2PCmdCloseStream
typedef struct stJP2PCloseStream
{
	jdt_dword	m_dwSession;						// 会话SESSION,由服务器返回
}* PSTJP2PCloseStream;
// 结束流命令 end

// 控制流命令 begin
// eJP2PCmdCtrlStream
typedef struct stJP2PCtrlStream
{
	jdt_dword	m_dwSession;						// 会话SESSION,服务器进行校验
	jdt_int		m_iCtrlCode;						// 控制码,见 eJP2PStreamCtrl
	jdt_dword	m_dwCtrlValue1;						// 控制值1
	jdt_dword	m_dwCtrlValue2;						// 控制值2
	jdt_dword	m_dwCtrlValue3;						// 控制值3
	jdt_dword	m_dwCtrlValue4;						// 控制值4
}* PSTJP2PCtrlStream;
// 控制流命令 end

// 请求对讲命令 begin
// eJP2PCmdReqTalk
typedef struct stJP2PReqTalk
{
	jdt_int		m_iSamples;							// 采样率	: 0--8k 1--16k 2-32k
	jdt_int		m_iEncodeType;						// 编码类型	: 0--pcm 1-g711a 2-g711u 3--g726
	jdt_int		m_iChannels;						// 通道数	: 1
	jdt_int		m_iBits;							// 位数		: 16bit
	jdt_dword	m_dwSession;						// 会话SESSION,由服务器返回
}* PSTJP2PReqTalk;
// 请求对讲命令 end

// 结束对讲命令 begin
// eJP2PCmdCloseTalk
typedef struct stJP2PCloseTalk
{
	jdt_dword	m_dwSession;						// 会话SESSION,由服务器返回
}* PSTJP2PCloseTalk;
// 结束对讲命令 end

// 设备通知命令 begin
// eJP2PCmdDevNotify
typedef struct stJP2PDevNotify
{
	jdt_int		m_iParamID;							// 参数ID
	jdt_int		m_iChn;								// 通道号
	jdt_int		m_iDataSize;						// 数据大小
}* PSTJP2PDevNotify;
// 设备通知命令 end

// 数据信息命令 begin
// eJP2PCmdStreamData
// 定义帧数据的信息 begin
// 视频参数
typedef struct stJP2PVideoInfo
{
	jdt_word		m_wImageWidth;		// 图像宽度
	jdt_word		m_wImageHeight;		// 图像高度
	jdt_byte		m_byEncodeType;		// 视频编码格式,见 eJP2PCodecVideo
	jdt_byte		m_byFrameRate;		// 帧率(保留)
	jdt_byte		m_byPal;			// 制式 0-n制1-pal制
	jdt_byte		m_byRes[1];	
}* PSTJP2PVideoInfo;
// 音频参数
typedef struct stJP2PAudioInfo
{
	jdt_byte		m_byAudioSamples;	// 采样率		0--8k 1--16k 2-32k
	jdt_byte		m_byEncodeType;		// 音频编码格式,见 eJP2PCodecAudio
	jdt_byte		m_byAudioChannels;	// 通道数		暂只支持1	
	jdt_byte		m_byAudioBits;		// 位数			16bit
	jdt_byte		m_byRes[4];
}* PSTJP2PAudioInfo;
typedef struct stJP2PStreamData
{
	jdt_word		m_wFrameFlag;		// 0x3448 magic data
	jdt_byte		m_byFrameType;		// I-0x7 p--0x8 b--0xb A--0xa
	jdt_byte		m_byRes[1];			// 保留
	jdt_dword		m_dwFrameNo;		// 帧号
	jdt_dword		m_dwTime;			// 系统时间
	jdt_dword		m_dwFrameSize;		// 数据流长度
	jdt_l64			m_dwTimeStamp;		// 时间戳
	union
	{
		struct stJP2PVideoInfo	m_stVideo;
		struct stJP2PAudioInfo	m_stAudio;
	};
}* PSTJP2PStreamData;
typedef struct stJP2PStreamEnd
{
	jdt_dword	m_dwSession;			// 会话SESSION,由服务器返回,该命令表示由服务器主动断开连接
}* PSTJP2PStreamEnd;
// 定义帧数据的信息 end
// 数据信息命令 end







// 接口定义 begin
#ifdef __cplusplus
extern "C" {
#endif
typedef jdt_int fcbJP2PLogTrace(jdt_lpchar lpszLog);																		// 日志回调
typedef jdt_int fcbJP2PLoginAuth(jdt_long lHandle, jdt_lpchar lpszName,jdt_lpchar lpszPwd);									// 登录认证回调
typedef jdt_int fcbJP2PGetParam(jdt_long lHandle, jdt_int iChn, jdt_int iParamID, jdt_lpvoid lpData, jdt_int iDataSize);	// 获取设备参数回调
typedef jdt_int fcbJP2PSetParam(jdt_long lHandle, jdt_int iChn, jdt_int iParamID, jdt_lpvoid lpData, jdt_int iDataSize);	// 设置设备参数回调
typedef jdt_int fcbJP2POpenStream(jdt_long lHandle, PSTJP2PReqStream pstReqParam);											// 请求打开流回调
typedef jdt_int fcbJP2PCloseStream(jdt_long lHandle);																		// 请求关闭流回调
typedef jdt_int fcbJP2PCtrlStream(jdt_long lHandle,PSTJP2PCtrlStream pstCtrlStream);										// 请求控制流回调
typedef jdt_int fcbJP2PSendFrame(jdt_long lHandle, jdt_lpvoid lpData, jdt_int iDataSize);									// 向设备发送帧数据回调
typedef jdt_int fcbJP2POpenTalk(jdt_long lHandle, PSTJP2PReqTalk pstReqParam);												// 请求打开对讲回调
typedef jdt_int fcbJP2PCloseTalk(jdt_long lHandle);																			// 请求关闭对讲回调
#ifdef __cplusplus
}
#endif

typedef	struct stJP2PInit
{
	fcbJP2PLogTrace*		m_fcbLogTrace;					// 日志回调
	fcbJP2PLoginAuth*		m_fcbLoginAuth;					// 登录认证回调
	fcbJP2PGetParam*		m_fcbGetParam;					// 获取设备参数回调
	fcbJP2PSetParam*		m_fcbSetParam;					// 设置设备参数回调
	fcbJP2POpenStream*		m_fcbOpenStream;				// 请求打开流回调
	fcbJP2PCloseStream*		m_fcbCloseStream;				// 请求关闭流回调
	fcbJP2PCtrlStream*		m_fcbCtrlStream;				// 请求控制流回调
	fcbJP2PSendFrame*		m_fcbSendFrame;					// 向设备发送帧数据回调
	fcbJP2POpenTalk*		m_fcbOpenTalk;					// 请求打开对讲回调
	fcbJP2PCloseTalk*		m_fcbCloseTalk;					// 请求关闭对讲回调
	jdt_int					m_iMaxChn;						// 最大通道数
	jdt_int					m_iMaxCode;						// 最大码流数
	#ifdef __cplusplus
	stJP2PInit()
	{
		m_fcbLogTrace		= NULL;							// 日志回调
		m_fcbLoginAuth		= NULL;							// 登录认证回调
		m_fcbGetParam		= NULL;							// 获取设备参数回调
		m_fcbSetParam		= NULL;							// 设置设备参数回调
		m_fcbOpenStream		= NULL;							// 请求打开流回调
		m_fcbCloseStream	= NULL;							// 请求打开流回调
		m_fcbCtrlStream		= NULL;							// 请求控制流回调
		m_fcbSendFrame		= NULL;							// 向设备发送帧数据回调
		m_fcbOpenTalk		= NULL;							// 请求打开对讲回调
		m_fcbCloseTalk		= NULL;							// 请求关闭对讲回调
		m_iMaxChn			= 0;							// 最大通道数
		m_iMaxCode			= 0;							// 最大码流数
	}
	#endif	// __cplusplus
}* PSTJP2PInit;
JP2P_TUTK_API jdt_int JP2P_StartSvr(PSTJP2PInit pstInit,jdt_lpcchar szUID,jdt_lpcchar szSvrAddr,jdt_int iSvrPort);
JP2P_TUTK_API jdt_int JP2P_StopSvr();
JP2P_TUTK_API jdt_int JP2P_OnSendFrame(jdt_lpvoid lpData,jdt_int iDataLen,jdt_long lHandle);
JP2P_TUTK_API jdt_int JP2P_OnSendNotify(jdt_int iParamID, jdt_lpvoid lpData,jdt_int iDataLen);
// 接口定义 end

#endif	// __JP2P_DEF_H__

