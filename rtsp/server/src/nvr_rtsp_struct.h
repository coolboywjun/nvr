#ifndef _NVR_RTSP_STRUCT_H_
#define _NVR_RTSP_STRUCT_H_

#include   <netinet/in.h>
#include   <arpa/inet.h>
#include   <sys/types.h>
#include   <sys/socket.h>

#include "linux_list.h"
/*
* Copyright (c) 2010
* All rights reserved.
*
* filename:nvr_rtsp_struct.h
*
* func:本文件实现了对rtsp 的结构定义
*
* author : nvr
* createdate: 2011-01-27
*
* modify list:
*
*/
#ifdef __cplusplus
extern "C" {
#endif

#define RTSP_MAX_CHN	        8 //0 主码流 1次码流
#define RTSP_MAX_STREAM_TYPE    2 //0  main 1 sub
#define RTSP_MAX_CLIENT			16

#define RTSP_MAX_FRAME_LEN		1024*1024
	/*rtsp的socket收发buffer大小*/
#define RTSP_MAX_PROTOCOL_BUFFER 4096
#define RTP_MAX_PACKET_BUFF 	 1460
#define RTSP_DEFAULT_SVR_PORT	 554
#define RTSP_MAX_USERNAME_LEN	 32
#define RTSP_MAX_USERPASSWD_LEN	 32
#define RTP_DEFAULT_SSRC 		 14080
	//========================================================
#define RTSP_VER_STR  "RTSP/1.0"

#define RTSP_LR   "\r"
#define RTSP_LF   "\n"
#define RTSP_LRLF "\r\n"


#define RTSP_METHOD_SETUP      "SETUP"
#define RTSP_METHOD_REDIRECT   "REDIRECT"
#define RTSP_METHOD_PLAY       "PLAY"
#define RTSP_METHOD_PAUSE      "PAUSE"
#define RTSP_METHOD_SESSION    "SESSION"
#define RTSP_METHOD_RECORD     "RECORD"

#define RTSP_METHOD_EXT_METHOD "EXT-"

#define RTSP_METHOD_OPTIONS    "OPTIONS"
#define RTSP_METHOD_DESCRIBE   "DESCRIBE"
#define RTSP_METHOD_GET_PARAM  "GET_PARAMETER"
#define RTSP_METHOD_SET_PARAM  "SET_PARAMETER"
#define RTSP_METHOD_TEARDOWN   "TEARDOWN"


	/* message header keywords */
#define RTSP_HDR_CONTENTLENGTH "Content-Length"
#define RTSP_HDR_ACCEPT "Accept"
#define RTSP_HDR_ALLOW "Allow"
#define RTSP_HDR_BLOCKSIZE "Blocksize"
#define RTSP_HDR_CONTENTTYPE "Content-Type"
#define RTSP_HDR_DATE "Date"
#define RTSP_HDR_REQUIRE "Require"
#define RTSP_HDR_TRANSPORTREQUIRE "Transport-Require"
#define RTSP_HDR_SEQUENCENO "SequenceNo"
#define RTSP_HDR_CSEQ "CSeq"
#define RTSP_HDR_STREAM "Stream"
#define RTSP_HDR_SESSION "Session"
#define RTSP_HDR_TRANSPORT "Transport"
#define RTSP_HDR_RANGE "Range"
#define RTSP_HDR_USER_AGENT "User-Agent"
#define RTSP_HDR_AUTHORIZATION "Authorization"

	typedef enum nvrRTSP_REQ_METHOD
	{
		/* method codes */
		RTSP_SETUP_METHOD     = 0 ,
		RTSP_DESCRIBE_METHOD  = 1 ,
		RTSP_REDIRECT_METHOD  = 2 ,
		RTSP_PLAY_METHOD      = 3 ,
		RTSP_PAUSE_METHOD     = 4 ,
		VOD_SESSION_METHOD    = 5 ,
		RTSP_OPTIONS_METHOD   = 6 ,
		RTSP_RECORD_METHOD    = 7 ,
		RTSP_TEARDOWN_METHOD  = 8 ,
		RTSP_GET_PARAM_METHOD = 9 ,
		RTSP_SET_PARAM_METHOD = 10,
		RTSP_EXTENSION_METHOD = 11,
		RTSP_REQ_METHOD_BUTT
	}
	RTSP_REQ_METHOD_E;

	/*
	* method response codes.  These are 100 greater than their
	* associated method values.  Tnvrs allows for simplified
	* creation of event codes that get used in event_handler()
	*/
#define RTSP_MAKE_RESP_CMD(req) (req + 100)
	typedef enum _RTSP_REP_
	{
	    RTSP_SETUP_RESPONSE      = RTSP_MAKE_RESP_CMD(RTSP_SETUP_METHOD),
	    RTSP_GET_RESPONSE        ,/*101*/
	    RTSP_REDIRECT_RESPONSE   ,/*102*/
	    RTSP_PLAY_RESPONSE       ,/*103*/
	    RTSP_PAUSE_RESPONSE      ,/*104*/
	    VOD_SESSION_RESPONSE    ,/*105*/
	    RTSP_OPTIONS_RESPONSE    ,/*106*/
	    RTSP_RECORD_RESPONSE     ,/*107*/
	    RTSP_TEARDOWN_RESPONSE   ,/*108*/
	    RTSP_GET_PARAM_RESPONSE  ,/*109*/
	    RTSP_SET_PARAM_RESPONSE  ,/*110*/
	    RTSP_EXTENSION_RESPONSE  ,/*111*/
	    RTSP_RESP_BUTT
	} RTSP_RESP_E;

	/* user command event codes */
#define RTSP_CMD_OPEN		10000
#define RTSP_CMD_GET		10001
#define RTSP_CMD_PLAY		10002
#define RTSP_CMD_PAUSE		10003
#define RTSP_CMD_CLOSE		10004


#define RTSP_STATUS_CONTINUE            100
#define RTSP_STATUS_OK                  200
#define RTSP_STATUS_ACCEPTED            202
#define RTSP_STATUS_BAD_REQUEST         400
#define RTSP_STATUS_METHOD_NOT_ALLOWED  405

	typedef struct RTSP_TOKENS_
	{
		char  *token;
		int   opcode;
	} RTSP_TKN_S;

	typedef enum _AV_TYPE_E
	{
	    AV_TYPE_VIDEO = 0,
	    AV_TYPE_AUDIO,
	    AV_TYPE_AV,
	    AV_TYPE_BUTT
	} AV_TYPE_E;

	/*RTP 传输模式*/
	typedef enum _RTP_TRANSPORT_TYPE_E_
	{
	    RTP_TRANSPORT_UDP = 0,
	    RTP_TRANSPORT_TCP,
	    RTP_TRANSPORT_BUTT
	} RTP_TRANSPORT_TYPE_E;

	typedef struct _RTP_STATS_S
	{
		unsigned long long  u64SendPacket;    /* number of packets send */
		unsigned long long  u64SendBytes;      /* bytes sent */
		unsigned long long  u64SendError;     /* error times when send */
		unsigned long long  u64RecvPacket;    /* number of packets received */
		unsigned long long  u64RecvBytes;      /* bytes of payload received */
		unsigned long long  u64Unavaillable;   /* packets not availlable when they were queried */
		unsigned long long  u64Bad;            /* packets that did not appear to be RTP */
		unsigned long long  u64Discarded;      /* incoming packets discarded because the queue exceeds its max size */
		unsigned long long  u64TimeoutCnt;
	} RTP_STATS_S;

	/*RTP TCP 传输标签*/
	typedef struct
	{
		int RTP;
		int RTCP;
	} RTP_TCP_TOKEN;

	/*RTSP状态机，暂时没有使用*/
	typedef enum RTSP_SESSION_STATE
	{
	    RTSP_STATE_INIT        = 0,
	    RTSP_STATE_READY       = 1,
	    RTSP_STATE_PLAY        = 2,
	    RTSP_STATE_STOP        = 3,
	    RTSP_STATE_BUTT
	} RTSP_SESSION_STATE_E;

	/*rtp发送数据流类型*/
#define RTSP_STREAM_VIDEO 0
#define RTSP_STREAM_AUDIO 1
#define RTSP_STREAM_MAX   2

	typedef struct _RTP_SENDER_S
	{
		RTP_TCP_TOKEN		stInterleaved[2];     /*interleaved for TCP video and audio*/
		unsigned int 			nAudioSsrc;
		unsigned int 			nVideoSsrc;
		unsigned int            	nPayLoad[2];
		unsigned short 			u16LastSn[2];			/*last recv sn*/
		unsigned short 			u16LastTs[2];			/*last recv sn*/
		unsigned short          	u16LastSndSeq[2];
		unsigned short  			u16ChnId;				/*该sender所对应的通道号*/
		int                     		nRtpTcpSock;
		int                     		nRtpSock[RTSP_MAX_STREAM_TYPE];				/*send/recv socket*/
		int                     		nRtcpSock[RTSP_MAX_STREAM_TYPE];
		int                     		nRtpPort[RTSP_MAX_STREAM_TYPE];
		int                     		nRtcpPort[RTSP_MAX_STREAM_TYPE];
		struct sockaddr_in       addrRemote[2];
		unsigned long           	u32LastSndPts[2];			/*上一帧数据的pts*/
		unsigned long           	u32LastSndTs[2];          /*上一帧数据的timestamp*/
		char           		   	*pFrameBuf;
		unsigned char 			szBuff[RTP_MAX_PACKET_BUFF];	/*保存待发送数据的rtp buffer, 存放已经过rtp打包的数据*/
		unsigned int 			nBuffLen;			/*buff中的实际需要发送数据的长度*/
	} RTP_SENDER_S;

	//struct nvrRTP_TARGETHOST;
	typedef struct _RTSP_SESSION_S_
	{
		struct list_head	 list;						/*用于维护链表*/
		int				 nChId;								/*该Session所对应的通道号*/
		int				 nStreamId;
		int 				 nAudioChId;						/*音频对应通道号 */
		RTP_TRANSPORT_TYPE_E eTransType;							/*Transport: RTP/AVP/TCP  Transport: RTP/AVP*/
		RTP_TCP_TOKEN		stInterleaved[RTSP_MAX_STREAM_TYPE];	/*interleaved for TCP video & TCP audio*/
		char				szId[16];							/*该session所对应的session id, 由链接创建时候随机产生*/
		char				szSSRC[16];
		char				szUserAgent[128];
		char				szUri[256];							/*该session中， 用户输入的url， rtsp://xxxxxx:553/1 */
		int					nRtspSocket;						/*该session的对应socket，在链接创建时候被赋值*/
		char				szHostIP[64];							/*本地ip*/
		char				szRemoteIP[64];						/*客户端的ip地址*/
		char				szBuffRecv[RTSP_MAX_PROTOCOL_BUFFER]; /*rtsp接收消息的buffer*/
		char				szBuffSend[RTSP_MAX_PROTOCOL_BUFFER]; /*rtsp发送消息的buffer*/
		int					nMsgSize;							/*rtsp接收消息长度*/
		unsigned short		u16RemotePort;							/*客户端的rtsp链接端口*/
		unsigned char		u8RequestStreamFlag[RTSP_MAX_STREAM_TYPE];		/*是否请求视频， 音频的标志*/
		unsigned short		u16RemoteRTPPort[RTSP_MAX_STREAM_TYPE];	/*客户端rtp端口 [0] 是视频端口， 1是音频端口*/
		unsigned short		u16RemoteRTCPPort[RTSP_MAX_STREAM_TYPE];/*客户端rtcp端口 [0] 是视频端口， 1是音频端口*/
		unsigned long 		u32LastSndReq;						/*最后发送的Req*/
		unsigned long		u32LastSndSeq;						/*最后发送的Cseq*/
		unsigned long		u32LastRecvSeq;						/*最后接收命令的Cseq*/
		int					nLan;								/*连接ip属于同一个局区网内*/
		int					nLink;							/*表示是否已经发送link消息回调*/
		RTSP_SESSION_STATE_E eState;
		RTP_SENDER_S	    stRtpSender;	/* tcp/udp 传输*/
		pthread_t			sessThd;
		char 				nonce[17];
		pthread_mutex_t	mutex;
		time_t 			preTime;
		struct list_head	 transList;

		long lPriData;
	} RTSP_SESSION_S;

	typedef enum _RTSP_SVR_STATE_E
	{
	    VOD_SVR_STATE_INIT    = 0, /*初始化状态， 还没有被创建*/
	    VOD_SVR_STATE_RUNNING = 1, /*已经创建，而且在运行*/
	    VOD_SVR_STATE_STOP    = 2, /*已经创建，但停止没有运行*/
	    VOD_SVR_STATE_BUTT
	} RTSP_SVR_STATE_E;

	typedef struct _NVR_RTSP_SVR
	{
		pthread_t thd;

		int       nInitOk;

		/*rtsp服务监听的socket*/
		int 	  nSvrSocket;

		/*rtsp 服务监听端口*/
		int   	  nSvrPort;
		/*支持的最大通道数*/
		int       nMaxCh;

		int       nUseAuth;

		int	    nSvrExits;

		RTSP_SVR_STATE_E  eState;

		/*用来存放working的session list
		0, 0chn
		1, 1chn
		VOD_MAX_CHN + 1, init , unknown chn
		*/

		RTSP_SESSION_S  sessList;

		unsigned long   clientCnt[RTSP_MAX_CHN];
	} NVR_RTSP_SVR, *LPNVR_RTSP_SVR;

#if 0
	//////////////////////////////////////////////////
	//音视频帧头定义
	typedef struct _RTSP_VIDEO_INFO_S
	{
		unsigned short		u16EncodeType;					//视频编码格式
		unsigned short		u16ImageWidth;					//图像宽度
		unsigned short		u16ImageHeight;					//图像高度
		unsigned char		u8FrameRate;					//帧率(保留)
		unsigned char		u8Pal;							//制式
	} RTSP_VIDEO_INFO_S, *LPRTSP_VIDEO_INFO_S;

	typedef struct _RTSP_AUDIO_INFO_S
	{
		unsigned short		u16EncodeType;					    //音频编码格式
		unsigned char		u8AudioChannels;					//通道数
		unsigned char		u8AudioBits;						//位数
		unsigned long		u32AudioSamples;					//采样率
	} RTSP_AUDIO_INFO_S, *LPRTSP_AUDIO_INFO_S;

	typedef struct _RTSP_FRAME_HEAD_S
	{
		unsigned short		u16FrameFlag;		//0x7468
		unsigned short		u16FrameType;		//AB_FRAME_TYPE
		unsigned long		u32FrameNo;			//帧号
		unsigned long		u32TimeStamp;		//时间戳
		unsigned long		u32Rsvd;			//保留
		union
		{
			RTSP_VIDEO_INFO_S	vidoeInfo;
			RTSP_AUDIO_INFO_S	audioInfo;
		};
		unsigned long		u32FrameSize;						//数据流长度
	} RTSP_FRAME_HEAD_S, *LPRTSP_FRAME_HEAD_S;
#endif
#ifdef __cplusplus
}
#endif

#endif
