
#include <ipnc_lib.h>
#include <ipnc_lib_client.h>

#include <nvr_server_headers.h>
#include <nvr_param_sys.h>
#include <nvr_param_proc.h>

void nvr_param_default_user_cfg()
{
	NVR_DEV_USER_CFG_S *pUsr = NULL;
	int i = 0;
	
	for (i = 0; i < NVR_MAX_USR_NUM; ++i)
	{
		memset(&gSysInfo.struUsrCfg[i], 0, sizeof(NVR_DEV_USER_CFG_S));
		pUsr = &gSysInfo.struUsrCfg[i];

		pUsr->u32Size = sizeof(NVR_DEV_USER_CFG_S);
		if (i == 0)
		{
			g_stpcpy(pUsr->szUsrName, "admin");
			g_stpcpy(pUsr->szPsw, "admin");
			pUsr->u64LocalRight = (0xFFFFFFFF << 31) | 0xFFFFFFFF;
			pUsr->u64RemoteRight = (0xFFFFFFFF << 31) | 0xFFFFFFFF;
		}
		else if(i == 1)
		{
			g_stpcpy(pUsr->szUsrName, "guest");
			g_stpcpy(pUsr->szPsw, "guest");
			pUsr->u64LocalRight = 0x0;
			pUsr->u64RemoteRight = 0x0;
		}
	}
}


void nvr_param_default_net_cfg()
{
	int i;
	uint8_t	u8MacAddr[NVR_MAC_ADDR_LEN] = {0xe0, 0x3c, 0x5b, 0x11, 0x22, 0x33};
	NVR_DEV_NET_CFG_S *pNetCfg = &gSysInfo.struNetCfg;
	struct timespec tp;
	int nRandom;

	memset(&gSysInfo.struNetCfg, 0, sizeof(gSysInfo.struNetCfg));
	pNetCfg->u32Size = sizeof(NVR_DEV_NET_CFG_S);
	
	for (i = 0; i < NVR_ETHERNET_NUM; ++i)
	{		
		pNetCfg->struEtherCfg[i].u32IpAddr = gStableInfo.u32IPAddr[i];
		pNetCfg->struEtherCfg[i].u32SubMask = gStableInfo.u32NetMask[i];
		pNetCfg->struEtherCfg[i].u32GateWay = gStableInfo.u32GateWay[i];
		memcpy(pNetCfg->struEtherCfg[i].u8MacAddr, gStableInfo.u8MacAddr[i], NVR_MAC_ADDR_LEN);
		if(memcmp(pNetCfg->struEtherCfg[i].u8MacAddr, u8MacAddr, sizeof(u8MacAddr)) == 0)
		{
			clock_gettime(CLOCK_MONOTONIC, &tp);
			srand(tp.tv_nsec);
			nRandom = rand();
			pNetCfg->struEtherCfg[i].u8MacAddr[0] = 0xe0;
			pNetCfg->struEtherCfg[i].u8MacAddr[1] = 0x3c;
			pNetCfg->struEtherCfg[i].u8MacAddr[2] = 0x5b;
			pNetCfg->struEtherCfg[i].u8MacAddr[3] = nRandom & 0xff;
			pNetCfg->struEtherCfg[i].u8MacAddr[4] = (nRandom >> 8) & 0xff;
			pNetCfg->struEtherCfg[i].u8MacAddr[5] = (nRandom >> 16) & 0xff;
		}
		pNetCfg->struEtherCfg[i].u8DhcpOn      = 0;
		pNetCfg->struEtherCfg[i].u8DhcpStatus = 0;
		pNetCfg->struEtherCfg[i].u8EtherStatus  = 0;
	}
	
	pNetCfg->struEtherCfg[1].u8DhcpOn = 0;
	pNetCfg->u32AlarmHostIp		= 0;
	pNetCfg->u16AlarmHostPort	= 0;
	pNetCfg->u8DnsDhcp			= 1;
  	pNetCfg->u32DnsIp1			= 0xCA6080A6;
 	pNetCfg->u32DnsIp2			= 0xCA608685;	
	pNetCfg->u16HttpPort		= 80;
	pNetCfg->u16NetPort			= 554;
	pNetCfg->u32MultiCastIp		= 0xF0F0F0EA;
	pNetCfg->u16MultiCastPort	= 7080;

	//sdk端口
	pNetCfg->u16CmdPort 	= 3321;
	pNetCfg->u16JXJDataPort = 7554;
	pNetCfg->u16TalkPort 	= 3322;

}

void nvr_param_default_var_cfg()
{
	NVR_DEV_VAR_INFO_S		*pVarCfg = &gSysInfo.struDevCfg.stVarInfo;
	DEV_SystemCfg *pCfg = get_system_cfg();
	g_stpcpy(pVarCfg->szDevName, "nvr");
	pVarCfg->u8Language = pCfg->u8DefLang;
}

void nvr_param_default_stable_cfg()
{
	NVR_DEV_STABLE_INFO_S	*pStableCfg = &gSysInfo.struDevCfg.stStableInfo;

	memcpy(pStableCfg, &gStableInfo, sizeof(NVR_DEV_STABLE_INFO_S));
}

int	nvr_param_default_video_cfg()
{
	int i = 0;
	NVR_DEV_VIDEO_CFG_S *pVideoCfg = NULL;
	uint8_t u8DefaultFrame = 30;
	uint32_t u32DefaultBitrate = 4096;
	
#if defined(TI_ARCH)	
	IPC_SystemCfg *pSysCfg;
  	pSysCfg = GetSystemCfg();
  	if(pSysCfg->u32SensorType == IPC_RESOLUTION_1080P)
  	{
  		u8DefaultFrame = 25;
  		u32DefaultBitrate = 6144;
  	}
  	else
  	{
  		u8DefaultFrame = 30;
  		u32DefaultBitrate = 4096;
  	}
#endif

	for (; i < NVR_MAX_CHANNEL_NUM; ++i)
	{
		pVideoCfg = &gSysInfo.struVideoCfg[i];

		pVideoCfg->u32Size = sizeof(NVR_DEV_VIDEO_CFG_S);
		//default osd param
		memset(&pVideoCfg->struOsd, 0, sizeof(NVR_DEV_OSD_CFG_S));
  		pVideoCfg->struOsd.u8EnableTitleOsd		= 1;
		pVideoCfg->struOsd.u8EnableTimeOsd		= 1;
		pVideoCfg->struOsd.u8DispWeek	      	= 0;          
		
		pVideoCfg->struOsd.u8EnableBitrateOsd	= 0;
		pVideoCfg->struOsd.u8TimeOsdType		= 0;
		pVideoCfg->struOsd.u8HourOsdType		= 0;
		pVideoCfg->struOsd.u8OsdColor			= 4;
		pVideoCfg->struOsd.u32TimeColor 		= 0x00ffffffUL;
		pVideoCfg->struOsd.u32ChnColor 			= 0x00ffffffUL;

		pVideoCfg->struOsd.struTitleOrg.s32X		= 0;
		pVideoCfg->struOsd.struTitleOrg.s32Y		= 528;
		sprintf(pVideoCfg->struOsd.szChnName, "Ch%d", i);
		//default video mask param
		memset(&pVideoCfg->struVMask, 0, sizeof(NVR_DEV_VMASK_CFG_S));
		//default main stream param
		memset(&pVideoCfg->struMainVenc, 0, sizeof(NVR_DEV_VENC_CFG_S));
		
		/*hi_param_get_default_size(i, gStableInfo.struStableInfo.u16DevType, 
									VENC_MAIN_STREAM, 
									&pVideoCfg->struMainVenc.u16Width,
									&pVideoCfg->struMainVenc.u16Height);*/
		pVideoCfg->struMainVenc.u16Width = 1280;
		pVideoCfg->struMainVenc.u16Height = 720;
		
		pVideoCfg->struMainVenc.u8VideoEncType = 0;
		pVideoCfg->struMainVenc.u8Gop		   = 75;
		pVideoCfg->struMainVenc.u8FlowType	   = 1;
		pVideoCfg->struMainVenc.u8FrameRate	   = u8DefaultFrame;
		pVideoCfg->struMainVenc.u32BitrateRate = u32DefaultBitrate;
		pVideoCfg->struMainVenc.u8PicQuilty	   = 1;
		//default sub stream param
		memset(&pVideoCfg->struSubVenc, 0, sizeof(NVR_DEV_VENC_CFG_S));

		/*hi_param_get_default_size(i ,gStableInfo.struStableInfo.u16DevType, 
								VENC_SUB_STREAM, 
								&pVideoCfg->struSubVenc.u16Width,
								&pVideoCfg->struSubVenc.u16Height);*/
		pVideoCfg->struMainVenc.u16Width = 1280;
		pVideoCfg->struMainVenc.u16Height = 720;
		
		pVideoCfg->struSubVenc.u8VideoEncType = 0;
		pVideoCfg->struSubVenc.u8Gop		  = 50;
		pVideoCfg->struSubVenc.u8FlowType	  = 0;
		pVideoCfg->struSubVenc.u8FrameRate	  = 30;
		pVideoCfg->struSubVenc.u32BitrateRate = 512;
		pVideoCfg->struSubVenc.u8PicQuilty	  = 1;
#if defined(TI_ARCH)	
	{
	  	IPC_AVCfg *pAvCfg;
		pAvCfg = GetAVCfg();
		DefaultAVCfg(pAvCfg);
		pAvCfg->u16MainWidth = pVideoCfg->struMainVenc.u16Width;
		pAvCfg->u16MainHeight = pVideoCfg->struMainVenc.u16Height;
		pAvCfg->u16SubWidth = pVideoCfg->struSubVenc.u16Width;
		pAvCfg->u16SubHeight = pVideoCfg->struSubVenc.u16Height;
		pAvCfg->u8H264EncodeType = pVideoCfg->eEncodeLevel;
		SetAVCfg(pAvCfg);
	}
#else
		pVideoCfg->struSubVenc.u16Width = 640;
		pVideoCfg->struSubVenc.u16Height = 480;
		pVideoCfg->eEncodeLevel = 0;		// high profile
#endif

		//default capture param
		memset(&pVideoCfg->struCapture, 0, sizeof(NVR_DEV_CAPTURE_CFG_S));

		/*hi_param_get_default_size(i, gStableInfo.struStableInfo.u16DevType, 
								VENC_JPEG_STREAM, 
								&pVideoCfg->struCapture.u16Width,
								&pVideoCfg->struCapture.u16Height);*/
		pVideoCfg->struMainVenc.u16Width = 1280;
		pVideoCfg->struMainVenc.u16Height = 720;
		
		pVideoCfg->struCapture.u8VideoEncType = 0x2;
		pVideoCfg->struCapture.u8Quilty	   = 0;
	}

	return NVR_SUCCESS;
}

int	nvr_param_default_audio_cfg()
{
	int i = 0;
	NVR_DEV_AUDIO_CFG_S *pAudioCfg = NULL;
	
	for (; i < NVR_MAX_CHANNEL_NUM; ++i)
	{
		pAudioCfg = &gSysInfo.struAudioCfg[i];
		memset(pAudioCfg, 0, sizeof(NVR_DEV_AUDIO_CFG_S));
		pAudioCfg->u32Size		= sizeof(NVR_DEV_AUDIO_CFG_S);
		pAudioCfg->u32SampleRate = 8000;
		pAudioCfg->u8Open		= 0;
		pAudioCfg->u8AEncType	= 1;
		pAudioCfg->u8Type		= 1;
	}
	
	return NVR_SUCCESS;
}

void nvr_param_default_comm_cfg()
{
	NVR_DEV_COMM_CFG_S *pCommCfg = NULL;
	int i = 0;
	
	for (i = 0; i < NVR_MAX_SUPPORT_COMM_NO; ++i)
	{
		pCommCfg = &gSysInfo.struCommCfg[i];
		memset(pCommCfg, 0, sizeof(NVR_DEV_COMM_CFG_S));
		pCommCfg->u32Size 		= sizeof(NVR_DEV_COMM_CFG_S);
		pCommCfg->u32BaudRate	= 14;		// 9600
		pCommCfg->u32BaudMask = 0xEFFF;// 支持从50到115200的波特率
		pCommCfg->u8DataBit	= 3;
		pCommCfg->u8StopBit	= 0;
		pCommCfg->u8Parity		= 0;
		pCommCfg->u8Flowcontrol = 0;	
	}	

}

void nvr_param_default_ntp_cfg()
{
	NVR_NTP_CFG_S *pNtpCfg = NULL;
	
	memset(&gSysInfo.struNtpCfg, 0, sizeof(NVR_NTP_CFG_S));
	pNtpCfg = &gSysInfo.struNtpCfg;
	
	pNtpCfg->u32Size		= sizeof(NVR_NTP_CFG_S);
	pNtpCfg->u8NtpOpen		= 1;		//ntp开关
	pNtpCfg->u8EuroTime		= 0;
	pNtpCfg->u16NtpPort		= 123;
	pNtpCfg->u8NtpRefTime 	= 12;		//ntp 定时对时时间(单位小时)
	pNtpCfg->nTimeZone		= 8*3600;	//时区 单位s(-12*3600~11*3600)
	strcpy(pNtpCfg->szNtpServer, "time.windows.com");

}


void nvr_param_default_ftp_cfg()
{
	NVR_FTP_CFG_S *pFtpCfg = NULL;

	pFtpCfg = &gSysInfo.struFtpCfg;
	memset(pFtpCfg, 0, sizeof(NVR_FTP_CFG_S));
	
	pFtpCfg->u32Size 	= sizeof(NVR_FTP_CFG_S);
	pFtpCfg->u16FtpPort = 21;
	
	strcpy(pFtpCfg->szFtpPath, "/");

}

void nvr_param_default_smtp_cfg()
{
	NVR_SMTP_CFG_S *pSmtpCfg = NULL;

	pSmtpCfg = &gSysInfo.struSmtpCfg;
	memset(pSmtpCfg, 0, sizeof(NVR_SMTP_CFG_S));
	
	pSmtpCfg->u16MailPort = 25;
	pSmtpCfg->u32Size 	  = sizeof(NVR_SMTP_CFG_S);
	strcpy(pSmtpCfg->szMailHub, "smtp.com");
	strcpy(pSmtpCfg->szMailUsr, "anonymous");

}

void nvr_param_default_ddns_cfg()
{
	NVR_DDNS_CFG_S *pDdnsCfg = NULL;
	
	memset(&gSysInfo.struDdnsCfg, 0, sizeof(NVR_DDNS_CFG_S));
	pDdnsCfg = &gSysInfo.struDdnsCfg;
	
	pDdnsCfg->u32Size		= sizeof(NVR_DDNS_CFG_S);
	pDdnsCfg->u8DDNSOpen	= 0;
	pDdnsCfg->u8DDNSType	= 0;
	pDdnsCfg->u16DDNSPort	= 80;
	pDdnsCfg->u32DDNSTimes	= 2;

}

void nvr_param_default_upnp_cfg()
{
	NVR_UPNP_CFG_S *pUpnpCfg = NULL;
	memset(&gSysInfo.struUpnpCfg, 0, sizeof(NVR_UPNP_CFG_S));
	pUpnpCfg = &gSysInfo.struUpnpCfg;
	
	pUpnpCfg->u32Size			= sizeof(NVR_UPNP_CFG_S);
	pUpnpCfg->u8UpnpOpen		= 1;		//upnp 开关
	pUpnpCfg->u8UpnpEthNo		= 0;		//0使用本机网卡映射 1 使用无线网卡映射
	pUpnpCfg->u8UpnpModel		= 1;    	//0固定映射 1自动映射
	pUpnpCfg->u8UpnpRefTime 	= 2;  		//upnp 刷新时间(单位小时)
	strcpy(pUpnpCfg->szUpnpSvr, "192.168.1.1");
	pUpnpCfg->u16UpnpDataPort	= gSysInfo.struNetCfg.u16NetPort;	//固定映射模式下, 数据端口的外网端口
	pUpnpCfg->u16UpnpWebPort	= gSysInfo.struNetCfg.u16HttpPort;; //固定映射模式下, Web端口的外网端口
	pUpnpCfg->u16UpnpRtspPort = 1554;
	pUpnpCfg->u16UpnpDataPortS = 0;		//数据端口的外网端口 0代表没有映射或不成功
	pUpnpCfg->u16UpnpWebPortS	= 0;		//Web端口的外网端口 0代表没有映射或不成功
	pUpnpCfg->u16UpnpRtspPortS = 0;
	pUpnpCfg->u16UpnpCmdPort 	= gSysInfo.struNetCfg.u16CmdPort;		//命令端口暂时为JXJ平台
	pUpnpCfg->u16UpnpTalkPort 	= gSysInfo.struNetCfg.u16TalkPort; 	//对讲端口暂时为JXJ平台
	pUpnpCfg->u16UpnpJXJDataPort  = gSysInfo.struNetCfg.u16JXJDataPort;	//JXJ平台数据端口
	pUpnpCfg->u16UpnpCmdPortS 	  = 0;		//命令端口的外网端口 0代表没有映射或不成功
	pUpnpCfg->u16UpnpTalkPortS    = 0;		//对讲端口的外网端口 0代表没有映射或不成功
	pUpnpCfg->u16UpnpJXJDataPortS = 0;		//JXJ平台数据端口

}

void nvr_param_default_ptz_cfg()
{
	NVR_DEV_PTZ_CFG_S *pPtz = NULL;
	int i = 0;
	
	for (i = 0; i < NVR_MAX_CHANNEL_NUM; ++i)
	{
		memset(&gSysInfo.struPtzCfg[i], 0, sizeof(NVR_DEV_PTZ_CFG_S));
		pPtz = &gSysInfo.struPtzCfg[i];

		pPtz->u32Size = sizeof(NVR_DEV_PTZ_CFG_S);
		pPtz->u8ProtocalType = 0;
		pPtz->u8PtzAddr	     = 1;
	}	

}

void nvr_param_default_jxj_platform_cfg()
{
	NVR_DEV_JXJ_PLATFORM_CFG_S *pJxjPlatformCfg = NULL;

	pJxjPlatformCfg = &gSysInfo.struJxjPlatformCfg;
	memset(pJxjPlatformCfg, 0, sizeof(NVR_DEV_JXJ_PLATFORM_CFG_S));
	
	pJxjPlatformCfg->u32Size 	= sizeof(NVR_DEV_JXJ_PLATFORM_CFG_S);
	pJxjPlatformCfg->u16CmsPort = 9902;
	
	strcpy(pJxjPlatformCfg->szPuId, "JXJ-NVR-00001234");
	strcpy(pJxjPlatformCfg->szCmsIp, "22.22.22.22");

}

void nvr_param_default_dianxin_platform_cfg()
{
	NVR_DEV_DIANXIN_PLATFORM_CFG_S *pDianxianPlatformCfg = NULL;
	DEV_KanDianCfg *pCfg = get_kandian_cfg();
	
	pDianxianPlatformCfg = &gSysInfo.stDianxinPlatformCfg;
	//memset(pDianxianPlatformCfg, 0, sizeof(NVR_DEV_DIANXIN_PLATFORM_CFG_S));
	
	pDianxianPlatformCfg->u32Size 	= sizeof(NVR_DEV_DIANXIN_PLATFORM_CFG_S);

	if(strlen(pCfg->szGroupUrl) != 0)
		g_strlcpy(pDianxianPlatformCfg->szGroupUrl, pCfg->szGroupUrl, sizeof(pDianxianPlatformCfg->szGroupUrl));
	if(strlen(pCfg->szProvincialUrl) != 0)
		g_strlcpy(pDianxianPlatformCfg->szProvincialUrl, pCfg->szProvincialUrl, sizeof(pDianxianPlatformCfg->szProvincialUrl));
	if(strlen(pCfg->szDevId) != 0)
		g_strlcpy(pDianxianPlatformCfg->szDevId, pCfg->szDevId, sizeof(pDianxianPlatformCfg->szDevId));
	if(strlen(pCfg->szKey) != 0)
		g_strlcpy(pDianxianPlatformCfg->szKey, pCfg->szKey, sizeof(pDianxianPlatformCfg->szKey));
	if(strlen(pCfg->szMode) != 0)
		g_strlcpy(pDianxianPlatformCfg->szMode, pCfg->szMode, sizeof(pDianxianPlatformCfg->szMode));
	
	pDianxianPlatformCfg->localRtspPort = 1554;
	//pDianxianPlatformCfg->upnpRtspPort = 18588;
}


void nvr_param_default_rec_cfg()
{
	int i;
	NVR_DEV_REC_STRC_S *pRecCfg;

	for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{
		pRecCfg = (NVR_DEV_REC_STRC_S *)&gSysInfo.struRecCfg[i];
		pRecCfg->u32Size = sizeof(NVR_DEV_REC_STRC_S);
		pRecCfg->u8OverWrite = 1;
		pRecCfg->u8RecStream = 0;
		pRecCfg->u8PreRecSecs = 0;
		pRecCfg->u8SaveMode = 0;
		pRecCfg->u8Enable = 1;
		if(nvr_get_dianxin_is_used())
			pRecCfg->u8RecType = 0;	// 全天录像
		else
			pRecCfg->u8RecType = 2;	// 报警录像
		pRecCfg->struTimerSeg[0][0].u8Open = 0;
		pRecCfg->struTimerSeg[0][0].beginSec = 0;
		pRecCfg->struTimerSeg[0][0].endSec = 0;
	}
}

void nvr_param_default_channel_cfg()
{
	int i;
	NVR_CHANNEL_CFG_S *pChannelCfg;
	const IPNC_PROTOCOL_S *protocol = ipnc_client_get_protocols();
	
	for(i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{
		pChannelCfg = &gSysInfo.struChannelCfg[i];
		memset(pChannelCfg, 0, sizeof(NVR_CHANNEL_CFG_S));
		
		pChannelCfg->u32Size = sizeof(NVR_CHANNEL_CFG_INFO_S);
		g_strlcpy(pChannelCfg->szFactoryName, protocol->factoryName, sizeof(pChannelCfg->szFactoryName));
		g_strlcpy(pChannelCfg->szProtocolName, protocol->name, sizeof(pChannelCfg->szProtocolName));
		g_snprintf(pChannelCfg->szDevName, sizeof(pChannelCfg->szDevName), "Ch%d", i);
		g_strlcpy(pChannelCfg->szUser, protocol->usrPwd[0].usr, sizeof(pChannelCfg->szUser));
		g_strlcpy(pChannelCfg->szPasswd, protocol->usrPwd[0].pwd, sizeof(pChannelCfg->szPasswd));
		pChannelCfg->u32IpAddr = 0xC0A801FA + i;
		pChannelCfg->u16DataPort = protocol->dataPort;
		pChannelCfg->u16WebPort = protocol->webPort;
		pChannelCfg->u16RtspPort = protocol->rtspPort;
		pChannelCfg->u8RemoteChn = 1;
		pChannelCfg->u8UseChn = 0;
		pChannelCfg->u8Enable = 1;
		pChannelCfg->u8SearchA = 1;//智能搜索标记
	}
}

void nvr_param_default_di_cfg()
{
	NVR_DEV_DI_CFG_S *pDi = NULL;
	int i = 0, j = 0, k = 0;
	
	for (i = 0; i < NVR_MAX_SUPPORT_DI; i++)
	{
		memset(&gSysInfo.struDiCfg[i], 0, sizeof(NVR_DEV_DI_CFG_S));
		pDi = &gSysInfo.struDiCfg[i];

		pDi->u32Size = sizeof(NVR_DEV_DI_CFG_S);
		pDi->u8Enable= 0;
		pDi->u8DiType= 0;

		//布防时段
		for (j = 0; j < NVR_MAX_SEG_NUM; j++)
		{
			for (k = 0; k < 8; k++)
			{
				pDi->struSeg[j][k].u8Open   = 1;
				pDi->struSeg[j][k].beginSec = 0;
				pDi->struSeg[j][k].endSec   = 24 * 3600 -1;
			}
		}
		
		//联动
		pDi->struLinkAct.u8Email 		  = 0;
		pDi->struLinkAct.u32ShootChannel  = 0;
		pDi->struLinkAct.u8ShootMode      = 0;
		pDi->struLinkAct.u8ShootNum	 	  = 0;
		pDi->struLinkAct.u32ShootInterval = 100;
		pDi->struLinkAct.u32RecTime	 	  = 120;
		pDi->struLinkAct.u32RecChannel	  = 0;
		pDi->struLinkAct.u8RecMode	      = 0;
		pDi->struLinkAct.u8RecPreTime	  = 5;
		pDi->struLinkAct.u32OutPut		  = 0;
		pDi->struLinkAct.u32OutPersist	  = 30;
		pDi->struLinkAct.u32PCRecChannel  = 0;
	}

}

void nvr_param_default_ipnc_alarm_cfg()
{
	NVR_DEV_IPNC_ALARM_CFG_S *pIpncAlarm = NULL;
	int i = 0;
	
	for (i = 0; i < NVR_MAX_CHANNEL_NUM; i++)
	{
		memset(&gSysInfo.struIpncAlarmCfg[i], 0, sizeof(NVR_DEV_IPNC_ALARM_CFG_S));
		pIpncAlarm = &gSysInfo.struIpncAlarmCfg[i];

		pIpncAlarm->u32Size = sizeof(NVR_DEV_IPNC_ALARM_CFG_S);
		pIpncAlarm->u8Enable= 0;
		
		//联动
		pIpncAlarm->struLinkAct.u8Email 		  = 0;
		pIpncAlarm->struLinkAct.u32ShootChannel   = 0;
		pIpncAlarm->struLinkAct.u8ShootMode       = 0;
		pIpncAlarm->struLinkAct.u8ShootNum	 	  = 0;
		pIpncAlarm->struLinkAct.u32ShootInterval  = 100;
		pIpncAlarm->struLinkAct.u32RecTime	 	  = 120;
		pIpncAlarm->struLinkAct.u32RecChannel	  = 0;
		pIpncAlarm->struLinkAct.u8RecMode	      = 0;
		pIpncAlarm->struLinkAct.u8RecPreTime	  = 5;
		pIpncAlarm->struLinkAct.u32OutPut		  = 0;
		pIpncAlarm->struLinkAct.u32OutPersist	  = 30;
		pIpncAlarm->struLinkAct.u32PCRecChannel   = 0;
	}

}

void nvr_param_default_auto_maintain_cfg()
{
	NVR_AUTO_MAINTAIN_CFG_S *pAutoCfg = &gSysInfo.struAutoCfg;
	
	memset(pAutoCfg, 0, sizeof(NVR_AUTO_MAINTAIN_CFG_S));
	pAutoCfg->u32Size = sizeof(NVR_AUTO_MAINTAIN_CFG_S);
	pAutoCfg->u8Enable = 1;
	pAutoCfg->u8WeekDay = 0;
	pAutoCfg->u8Hour = 4;
	pAutoCfg->u8Min = 0;
}

void nvr_param_default_auto_ctrl_cfg()
{
	NVR_DEV_AUTO_CTRL_CFG_S *pAutoCtrlCfg = &gSysInfo.struAutoCtrlCfg;
	memset(pAutoCtrlCfg, 0, sizeof(NVR_DEV_AUTO_CTRL_CFG_S));
	pAutoCtrlCfg->u32Size = sizeof(NVR_DEV_AUTO_CTRL_CFG_S);
	pAutoCtrlCfg->u8Enable = 1;
}

void nvr_param_default_sys_cfg()
{
	gSysInfo.u32Size = sizeof(NVR_PARAM_SYS_INFO_S);
	gSysInfo.u32Version = 0x0001;
	
	nvr_param_default_stable_cfg();
	nvr_param_default_var_cfg();
	nvr_param_default_user_cfg();
	nvr_param_default_video_cfg();
	nvr_param_default_audio_cfg();
	nvr_param_default_comm_cfg();
	nvr_param_default_ftp_cfg();
	nvr_param_default_smtp_cfg();
	nvr_param_default_ntp_cfg();
	nvr_param_default_ddns_cfg();
	nvr_param_default_upnp_cfg();
	nvr_param_default_ptz_cfg();
	nvr_param_default_rec_cfg();
	nvr_param_default_jxj_platform_cfg();
	nvr_param_default_channel_cfg();
	nvr_param_default_di_cfg();
	nvr_param_default_ipnc_alarm_cfg();
	nvr_param_default_auto_maintain_cfg();
	nvr_param_default_dianxin_platform_cfg();
	nvr_param_default_auto_ctrl_cfg();
}

