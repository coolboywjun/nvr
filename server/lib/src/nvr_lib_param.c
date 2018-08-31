
#include <nvr_headers.h>
#include <nvr_lib_common.h>
#include <nvr_lib_interface.h>

int nvr_export_param(const char *pExportPath)
{
	char szSendBuf[1024];
	NVR_NET_HDR_S *pNetHdr = (NVR_NET_HDR_S *)szSendBuf;
	int ret = -1;
	int nPacketSize;

	if(pExportPath == NULL || strlen(pExportPath) == 0)
		return -1;
	strcpy((char *)(pNetHdr + 1), pExportPath);
	nPacketSize = strlen(pExportPath) + 1;
	nvr_make_net_hdr(pNetHdr, -1, NVR_CMD_EXPORT_PARAM, nPacketSize, 0);
	if(0 > (ret = nvr_send_cmd(NULL, 0, szSendBuf, sizeof(NVR_NET_HDR_S)+nPacketSize)))
	{
		__ERR("nvr reset failed :%d\n", ret);
		return NVR_FAILURE;
	}

	return NVR_SUCCESS;
}

int nvr_import_param(const char *pImportPath)
{
	char szSendBuf[1024];
	NVR_NET_HDR_S *pNetHdr = (NVR_NET_HDR_S *)szSendBuf;
	int ret = -1;
	int nPacketSize;

	if(pImportPath == NULL || strlen(pImportPath) == 0)
		return -1;
	strcpy((char *)(pNetHdr + 1), pImportPath);
	nPacketSize = strlen(pImportPath) + 1;
	nvr_make_net_hdr(pNetHdr, -1, NVR_CMD_IMPORT_PARAM, nPacketSize, 0);
	if(0 > (ret = nvr_send_cmd(NULL, 0, szSendBuf, sizeof(NVR_NET_HDR_S)+nPacketSize)))
	{
		__ERR("nvr reset failed :%d\n", ret);
		return NVR_FAILURE;
	}

	return NVR_SUCCESS;
}

int nvr_get_ipnc_param(int chnNo, IPNC_CMD_TYPE_E paramCmd, void* pNetParam_S, int paramSize)
{
	NVR_NET_REQ_PARAM_S rGetParam;
	char sendBuf[4096] = {0};
	int  headSize = sizeof(NVR_NET_HDR_S);
	int  dataSize = sizeof(NVR_NET_REQ_PARAM_S);
	int  recvSize = paramSize;
	int  sendLen  = headSize + dataSize;
	int  cmd	  = NVR_CMD_GET_PARAM;
	
	memset(&rGetParam,   0, dataSize);
	memset(pNetParam_S, 0, recvSize);
	
	rGetParam.dataSize = paramSize;
	rGetParam.paramCmd = paramCmd;

	nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, chnNo, cmd, dataSize, 0);
	memcpy(sendBuf + headSize, &rGetParam, dataSize);
	
	if(0 > nvr_send_cmd(pNetParam_S, recvSize, 
						sendBuf, sendLen))
		return NVR_FAILURE;
	
	return NVR_SUCCESS;
}
			
int nvr_set_ipnc_param(int chnNo, IPNC_CMD_TYPE_E paramCmd, void* pNetParam_S, int paramSize)
{
	NVR_NET_REQ_PARAM_S rGetParam;
	char sendBuf[4096] = {0};
	int  headSize = sizeof(NVR_NET_HDR_S);
	int  dataSize = sizeof(NVR_NET_REQ_PARAM_S);
	int  sendLen  = headSize + dataSize;
	int  cmd	  = NVR_CMD_SET_PARAM;
	
	memset(&rGetParam,   0, dataSize);

	rGetParam.dataSize = paramSize;
	rGetParam.paramCmd = paramCmd;
	memcpy(rGetParam.paramData, pNetParam_S, rGetParam.dataSize);
	
	nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, chnNo, cmd, dataSize, 0);
	memcpy(sendBuf + headSize, &rGetParam, dataSize);
	
	if(0 > nvr_send_cmd(NULL, 0, sendBuf, sendLen))
		return NVR_FAILURE;
	
	return NVR_SUCCESS;
}

int nvr_set_ipnc_ima_def_param(int chnNo, IPNC_DEV_IMA_CFG_S *pDefImaCfg)
{
	IPNC_DEV_IMA_CFG_S stImaCfg = {0};
/*	
	NVR_DEV_INFO_S devCfg;
	if(0 > nvr_logon_get_dev_cfg(0, &devCfg))
		return NVR_FAILURE;
	
	if(devCfg.stStableInfo.u8ChnNum < chnNo || 0 > chnNo)
		return NVR_FAILURE;
*/
	//获取配置	
	if(0 > nvr_get_ipnc_param(chnNo,  IPNC_CMD_IMA_CFG,
					   		&stImaCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		return NVR_FAILURE;
	
	//////////////////////ima
	//亮度
	stImaCfg.struBrightness.u8Value = stImaCfg.struBrightness.u8Default;
	//对比度
	stImaCfg.struContrast.u8Value = stImaCfg.struContrast.u8Default;
	//饱和度
	stImaCfg.struSaturation.u8Value = stImaCfg.struSaturation.u8Default;
	//色度
	stImaCfg.struHue.u8Value = stImaCfg.struHue.u8Default;
	//伽马
	stImaCfg.struGamma.u8Value = stImaCfg.struGamma.u8Default;
	//锐度
	stImaCfg.struSharpness.u8Value = stImaCfg.struSharpness.u8Default;
	//红
	stImaCfg.struRed.u8Value = stImaCfg.struRed.u8Default;
	//蓝
	stImaCfg.struBlue.u8Value = stImaCfg.struBlue.u8Default;
	//设置
	if(0 > nvr_set_ipnc_param(chnNo,  IPNC_CMD_IMA_CFG,
					   		&stImaCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		return NVR_FAILURE;
	if(pDefImaCfg != NULL)
		memcpy(pDefImaCfg, &stImaCfg, sizeof(stImaCfg));
	return NVR_SUCCESS;
}

int nvr_set_ipnc_3a_def_param(int chnNo, IPNC_DEV_3A_CFG_S *pDef3aCfg)
{
	IPNC_DEV_3A_CFG_S st3ACfg = {0};
/*	
	NVR_DEV_INFO_S devCfg;
	if(0 > nvr_logon_get_dev_cfg(0, &devCfg))
		return NVR_FAILURE;
	
	if(devCfg.stStableInfo.u8ChnNum < chnNo || 0 > chnNo)
		return NVR_FAILURE;
*/
	//获取3A参数配置	
	if(0 > nvr_get_ipnc_param(chnNo,  IPNC_CMD_3A_CFG,
					   		 &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return NVR_FAILURE;

	//翻转图像上下翻转
	st3ACfg.struFlip.u8Value = st3ACfg.struFlip.u8Default;
	//镜像图像左右翻转
	st3ACfg.struMirror.u8Value =  st3ACfg.struMirror.u8Default;
	//电源频率
	st3ACfg.struPwdFreq.u8Value =  st3ACfg.struPwdFreq.u8Default;
	//场景,0:自动 1:室内2:室外
	st3ACfg.struScene.u8Value =  st3ACfg.struScene.u8Default;
	//红外模式，0:自动，1:白天，2:晚上,3:定时模式
	//st3ACfg.struIrcfMode.u8Value =  st3ACfg.struIrcfMode.u8Default;
	//电子快门 0-8  0：自动快门 1：											
	//1/25S， 2：1/50S， 3：1/100S， 4：1/500S， 5：1/1000S， 											
	//6：1/2000S， 7：1/5000S， 8：1/10000S
	st3ACfg.struShutterSpeed.u8Value =  st3ACfg.struShutterSpeed.u8Default;

	//设置配置	
	if(0 > nvr_set_ipnc_param(chnNo,  IPNC_CMD_3A_CFG,
					   		 &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return NVR_FAILURE;
	if(pDef3aCfg != NULL)
		memcpy(pDef3aCfg, &st3ACfg, sizeof(st3ACfg));
	return NVR_SUCCESS;
}



static int __nvr_logon_get_cfg(int cmdType, int port, void *pCfg, int cfgLen)
{
	NVR_NET_HDR_S netHdr;
	
	nvr_make_net_hdr(&netHdr, port, cmdType, 0, 0);
	if(0 > nvr_send_cmd(pCfg, cfgLen, &netHdr, sizeof(NVR_NET_HDR_S)))
	{
		__ERR("send data packet failed...\n");
		return NVR_FAILURE;
	}
	
	return NVR_SUCCESS;
}

static int __nvr_logon_set_cfg(int cmdType, int port, void *pCfg, int cfgLen)
{
	char sendBuf[4096] = {0};
	
	nvr_make_net_hdr((NVR_NET_HDR_S*)sendBuf, port, cmdType, cfgLen, 0);
	memcpy(sendBuf+sizeof(NVR_NET_HDR_S), pCfg, cfgLen);
	
	if(0 > nvr_send_cmd(NULL, 0, sendBuf, sizeof(NVR_NET_HDR_S)+cfgLen))
	{
		__ERR("send data packet failed...\n");
		return NVR_FAILURE;
	}
	
	return NVR_SUCCESS;
}

#define NVR_LOGON_GET_CFG(CfgName, StructType, CmdType)	\
int nvr_logon_get_##CfgName##_cfg(int port, StructType *pCfg)	\
{	\
	return __nvr_logon_get_cfg(CmdType, port, pCfg, sizeof(StructType));	\
}

#define NVR_LOGON_SET_CFG(CfgName, StructType, CmdType)	\
int nvr_logon_set_##CfgName##_cfg(int port, StructType *pCfg)	\
{	\
	return __nvr_logon_set_cfg(CmdType, port, pCfg, sizeof(StructType));	\
}

NVR_LOGON_GET_CFG(dev, NVR_DEV_INFO_S, NVR_CMD_GET_DEV_INFO);
NVR_LOGON_SET_CFG(dev, NVR_DEV_INFO_S, NVR_CMD_SET_DEV_INFO);

NVR_LOGON_GET_CFG(user, NVR_DEV_USER_CFG_S, NVR_CMD_GET_USR_CFG);
NVR_LOGON_SET_CFG(user, NVR_DEV_USER_CFG_S, NVR_CMD_SET_USR_CFG);

NVR_LOGON_GET_CFG(net, NVR_DEV_NET_CFG_S, NVR_CMD_GET_NET_CFG);
NVR_LOGON_SET_CFG(net, NVR_DEV_NET_CFG_S, NVR_CMD_SET_NET_CFG);

NVR_LOGON_GET_CFG(ntp, NVR_NTP_CFG_S, NVR_CMD_GET_NTP_CFG);
NVR_LOGON_SET_CFG(ntp, NVR_NTP_CFG_S, NVR_CMD_SET_NTP_CFG);

NVR_LOGON_GET_CFG(upnp, NVR_UPNP_CFG_S, NVR_CMD_GET_UPNP_CFG);
NVR_LOGON_SET_CFG(upnp, NVR_UPNP_CFG_S, NVR_CMD_SET_UPNP_CFG);

NVR_LOGON_GET_CFG(smtp, NVR_SMTP_CFG_S, NVR_CMD_GET_EMAIL_CFG);
NVR_LOGON_SET_CFG(smtp, NVR_SMTP_CFG_S, NVR_CMD_SET_EMAIL_CFG);

NVR_LOGON_GET_CFG(ftp, NVR_FTP_CFG_S, NVR_CMD_GET_FTP_CFG);
NVR_LOGON_SET_CFG(ftp, NVR_FTP_CFG_S, NVR_CMD_SET_FTP_CFG);

NVR_LOGON_GET_CFG(comm, NVR_DEV_COMM_CFG_S, NVR_CMD_GET_COMM_CFG);
NVR_LOGON_SET_CFG(comm, NVR_DEV_COMM_CFG_S, NVR_CMD_SET_COMM_CFG);

NVR_LOGON_GET_CFG(ddns, NVR_DDNS_CFG_S, NVR_CMD_GET_DDNS_CFG);
NVR_LOGON_SET_CFG(ddns, NVR_DDNS_CFG_S, NVR_CMD_SET_DDNS_CFG);

NVR_LOGON_GET_CFG(channel, NVR_CHANNEL_CFG_S, NVR_CMD_GET_CHANNEL_CFG);
NVR_LOGON_SET_CFG(channel, NVR_CHANNEL_CFG_S, NVR_CMD_SET_CHANNEL_CFG);

//NVR_LOGON_GET_CFG(time, NVR_TIME_CFG_S, NVR_CMD_GET_TIME_CFG);
//NVR_LOGON_SET_CFG(time, NVR_TIME_CFG_S, NVR_CMD_SET_TIME_CFG);
NVR_LOGON_GET_CFG(time, NVR_TIME_S, NVR_CMD_GET_TIME_CFG);
NVR_LOGON_SET_CFG(time, NVR_TIME_S, NVR_CMD_SET_TIME_CFG);

NVR_LOGON_GET_CFG(auto, NVR_AUTO_MAINTAIN_CFG_S, NVR_CMD_GET_AUTO_CFG);
NVR_LOGON_SET_CFG(auto, NVR_AUTO_MAINTAIN_CFG_S, NVR_CMD_SET_AUTO_CFG);

NVR_LOGON_GET_CFG(jxj_platform, NVR_DEV_JXJ_PLATFORM_CFG_S, NVR_CMD_GET_JXJ_PLF_CFG);
NVR_LOGON_SET_CFG(jxj_platform, NVR_DEV_JXJ_PLATFORM_CFG_S, NVR_CMD_SET_JXJ_PLF_CFG);

NVR_LOGON_GET_CFG(dianxin_platform, NVR_DEV_DIANXIN_PLATFORM_CFG_S, NVR_CMD_GET_DIANXIN_PLF_CFG);
NVR_LOGON_SET_CFG(dianxin_platform, NVR_DEV_DIANXIN_PLATFORM_CFG_S, NVR_CMD_SET_DIANXIN_PLF_CFG);


NVR_LOGON_GET_CFG(main_venc, NVR_DEV_VENC_CFG_S, NVR_CMD_GET_VMAIN_CFG);
NVR_LOGON_SET_CFG(main_venc, NVR_DEV_VENC_CFG_S, NVR_CMD_SET_VMAIN_CFG);

NVR_LOGON_GET_CFG(rec, NVR_DEV_REC_STRC_S, NVR_CMD_GET_REC_CFG);
NVR_LOGON_SET_CFG(rec, NVR_DEV_REC_STRC_S, NVR_CMD_SET_REC_CFG);

NVR_LOGON_GET_CFG(stable, NVR_DEV_STABLE_INFO_S, NVR_CMD_GET_STABLE_CFG);
NVR_LOGON_SET_CFG(stable, NVR_DEV_STABLE_INFO_S, NVR_CMD_SET_STABLE_CFG);

NVR_LOGON_GET_CFG(di, NVR_DEV_DI_CFG_S, NVR_CMD_GET_DI_CFG);
NVR_LOGON_SET_CFG(di, NVR_DEV_DI_CFG_S, NVR_CMD_SET_DI_CFG);

NVR_LOGON_GET_CFG(ipnc_alarm, NVR_DEV_IPNC_ALARM_CFG_S, NVR_CMD_GET_IPNC_ALARM_CFG);
NVR_LOGON_SET_CFG(ipnc_alarm, NVR_DEV_IPNC_ALARM_CFG_S, NVR_CMD_SET_IPNC_ALARM_CFG);

NVR_LOGON_GET_CFG(auto_ctrl, NVR_DEV_AUTO_CTRL_CFG_S, NVR_CMD_GET_AUTO_CTRL_CFG);
NVR_LOGON_SET_CFG(auto_ctrl, NVR_DEV_AUTO_CTRL_CFG_S, NVR_CMD_SET_AUTO_CTRL_CFG);

NVR_LOGON_GET_CFG(osd, NVR_DEV_OSD_CFG_S, NVR_CMD_GET_OSD_CFG);
NVR_LOGON_SET_CFG(osd, NVR_DEV_OSD_CFG_S, NVR_CMD_SET_OSD_CFG);

