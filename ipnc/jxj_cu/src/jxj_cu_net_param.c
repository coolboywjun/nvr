#include <jxj_cu_heads.h>
#include "jxj_cu_param.h"
#include "jxj_cu_logon.h"
#include "jxj_cu_net_param.h"
#include "jxj_cu_character_conv.h"


static int jxj_cu_net_get__param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int jxj_cu_net_set__param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int jxj_cu_net_check__param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}


static int strcpy_safe(char* dstStr, char* srcStr, int dstStrSize)
{
	int len = 0;
	
	if(NULL == dstStr || NULL == srcStr)
		return 0;

	if(0 == strlen(srcStr))
		return 0;
	
	memset(dstStr, 0, dstStrSize);
	len	 =  strlen(srcStr);
	if(len > (dstStrSize-1))
		len = (dstStrSize-1);
	strncpy(dstStr, srcStr, len);
	dstStr[len+1] = '\0';

	return len;
}
/*net通用结构体函数************************************************************/
static int jxj_cu_net_get_net_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_NET_CFG_S *pCfg = (IPNC_NET_CFG_S *)pData;
	JNetworkInfo cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JNetworkInfo));
	ret = jxj_cu_net_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		//ip
		strcpy_safe(pCfg->ipAddr, (char*)cfg.network[0].ip,
					sizeof(pCfg->ipAddr));
		//mask
		strcpy_safe(pCfg->subMask, (char*)cfg.network[0].netmask,
					sizeof(pCfg->subMask));
		//gateway
		strcpy_safe(pCfg->gateWay, (char*)cfg.network[0].gateway,
					sizeof(pCfg->gateWay));
		//mac
		strcpy_safe(pCfg->macAddr, (char*)cfg.network[0].mac,
					sizeof(pCfg->macAddr));
		//dns
		strcpy_safe(pCfg->dnsIp1, (char*)cfg.main_dns,
					sizeof(pCfg->dnsIp1));
		strcpy_safe(pCfg->dnsIp2, (char*)cfg.backup_dns,
					sizeof(pCfg->dnsIp2));
		//port
		pCfg->u16NetPort  = cfg.cmd_port;
		pCfg->u16HttpPort = cfg.web_port;
	}
	else
		__ERR("jxj_cu_net_get_net_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_NET_CFG_S) : ret);
}
static int jxj_cu_net_set_net_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_NET_CFG_S *pCfg = (IPNC_NET_CFG_S *)pData;
	JNetworkInfo cfg;
	int ret = -1;

	memset(&cfg, 0, sizeof(JNetworkInfo));
	ret = jxj_cu_net_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		//ip
		strcpy_safe((char*)cfg.network[0].ip, pCfg->ipAddr,
					sizeof((char*)cfg.network[0].ip));
		//mask
		strcpy_safe((char*)cfg.network[0].netmask, pCfg->subMask, 
					sizeof((char*)cfg.network[0].netmask));
		//gateway
		strcpy_safe(pCfg->gateWay, (char*)cfg.network[0].gateway,
					sizeof(pCfg->gateWay));
		//mac
		strcpy_safe((char*)cfg.network[0].mac, pCfg->macAddr, 
					sizeof((char*)cfg.network[0].mac));
		//dns
		strcpy_safe((char*)cfg.main_dns, pCfg->dnsIp1,
					sizeof((char*)cfg.main_dns));
		strcpy_safe((char*)cfg.backup_dns, pCfg->dnsIp2, 
					sizeof((char*)cfg.backup_dns));
		//port
		cfg.cmd_port = pCfg->u16NetPort;
		cfg.web_port = pCfg->u16HttpPort;

		ret = jxj_cu_net_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_net_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_net_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_NET_CFG_S))
		return -1;
	
	return 0;
}
/*dev通用结构体函数************************************************************/
static int jxj_cu_net_get_dev_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_CFG_S *pCfg = (IPNC_DEV_CFG_S *)pData;
	JDeviceInfo cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JDeviceInfo));
	ret = jxj_cu_dev_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		//pCfg->serialId

		//soft
		strcpy_safe(pCfg->sofrVer, (char*)cfg.dev_version, 
					sizeof(pCfg->sofrVer));
		strcpy_safe(pCfg->hwVer, (char*)cfg.hw_version, 
					sizeof(pCfg->hwVer));
		pCfg->channel_num = cfg.channel_num;
		pCfg->di_num = cfg.di_num;
		pCfg->do_num = cfg.do_num;
		pCfg->rs232_num = cfg.rs232_num;
		pCfg->rs485_num = cfg.rs485_num;
	}
	else
		__ERR("jxj_cu_net_get_dev_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_CFG_S) : ret);
}
static int jxj_cu_net_set_dev_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_CFG_S *pCfg = (IPNC_DEV_CFG_S *)pData;
	JDeviceInfo cfg;
	int ret = -1;

	memset(&cfg, 0, sizeof(JDeviceInfo));
	ret = jxj_cu_dev_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe((char*)cfg.dev_version, pCfg->sofrVer, 
					sizeof(cfg.dev_version));
		strcpy_safe((char*)cfg.hw_version, pCfg->hwVer,  
					sizeof(cfg.hw_version));
		cfg.channel_num = pCfg->channel_num;
		cfg.di_num = pCfg->di_num;
		cfg.do_num = pCfg->do_num;
		cfg.rs232_num = pCfg->rs232_num;
		cfg.rs485_num = pCfg->rs485_num;

		ret = jxj_cu_dev_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_net_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_dev_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_CFG_S))
		return -1;
	
	return 0;
}
/*pppoe通用结构体函数************************************************************/
static int jxj_cu_net_get_pppoe_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_PPPOE_CFG_S *pCfg = (IPNC_PPPOE_CFG_S *)pData;
	JPPPOEInfo cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JPPPOEInfo));
	ret = jxj_cu_pppoe_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe(pCfg->pppoeIp, (char*)cfg.ip, 
					sizeof(pCfg->pppoeIp));
		strcpy_safe(pCfg->pppoeUsr, (char*)cfg.account, 
					sizeof(pCfg->pppoeUsr));
		strcpy_safe(pCfg->pppoePsw, (char*)cfg.passwd, 
					sizeof(pCfg->pppoePsw));
	}
	else
		__ERR("jxj_cu_net_get_pppoe_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_PPPOE_CFG_S) : ret);
}
static int jxj_cu_net_set_pppoe_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_PPPOE_CFG_S *pCfg = (IPNC_PPPOE_CFG_S *)pData;
	JPPPOEInfo cfg;
	int ret = -1;

	memset(&cfg, 0, sizeof(JPPPOEInfo));
	ret = jxj_cu_pppoe_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe((char*)cfg.ip, pCfg->pppoeIp, 
					sizeof(cfg.ip));
		strcpy_safe((char*)cfg.account, pCfg->pppoeUsr, 
					sizeof(cfg.account));
		strcpy_safe((char*)cfg.passwd, pCfg->pppoePsw, 
					sizeof(cfg.passwd));
		
		ret = jxj_cu_pppoe_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_pppoe_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_pppoe_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_PPPOE_CFG_S))
		return -1;
	
	return 0;
}

/*Ima通用结构体函数************************************************************/
static int jxj_cu_net_get_ima_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_IMA_CFG_S *pCfg = (IPNC_DEV_IMA_CFG_S *)pData;
	JDisplayParameter dispCfg = {0};
	//JPictureInfo	  picCfg;
	int ret = -1;
	
	//memset(&dispCfg, 0, sizeof(JDisplayParameter));
	//memset(&picCfg, 0, sizeof(JPictureInfo));
	/*if((0 == jxj_cu_disp_get(logonHandle, &dispCfg)) &&
		(0 == jxj_cu_pic_get(logonHandle, &picCfg)))
		ret = 0;*/
		
	//if(0 ==(ret = jxj_cu_disp_get(logonHandle, &dispCfg)))
	//	jxj_cu_pic_get(logonHandle, &picCfg);
	ret = jxj_cu_disp_get(logonHandle, &dispCfg);
	if((0 == ret) && (NULL != pCfg))
	{
		pCfg->struContrast.u8Value 	 = dispCfg.contrast;
		pCfg->struBrightness.u8Value = dispCfg.bright;
		pCfg->struHue.u8Value 		 = dispCfg.hue;
		pCfg->struSaturation.u8Value = dispCfg.saturation;
		pCfg->struSharpness.u8Value  = dispCfg.sharpness;
		//pCfg->struRed.u8Value = picCfg.awb_red;
		//pCfg->struBlue.u8Value = picCfg.awb_blue;

		
		pCfg->struGamma.u8Default 	   = 128;
		pCfg->struBrightness.u8Default = 128;
		pCfg->struContrast.u8Default 	= 128;
		pCfg->struSaturation.u8Default 	= 128;
		pCfg->struSharpness.u8Default 	= 128;
		pCfg->struBlue.u8Default 	= 128;
		pCfg->struRed.u8Default 	= 128;

		if(jxj_cu_disp_def_get(logonHandle, &dispCfg) == 0)
		{
			pCfg->struContrast.u8Default 	 = dispCfg.contrast;
			pCfg->struBrightness.u8Default = dispCfg.bright;
			pCfg->struHue.u8Default 		 = dispCfg.hue;
			pCfg->struSaturation.u8Default = dispCfg.saturation;
			pCfg->struSharpness.u8Default  = dispCfg.sharpness;
		}
	}
	else
		__ERR("jxj_cu_net_get_ima_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_IMA_CFG_S) : ret);
}
static int jxj_cu_net_set_ima_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_IMA_CFG_S *pCfg = (IPNC_DEV_IMA_CFG_S *)pData;
	JDisplayParameter dispCfg = {0};
	//JPictureInfo	  picCfg;
	int ret = -1;
	
	//memset(&dispCfg, 0, sizeof(JDisplayParameter));
	//memset(&picCfg, 0, sizeof(JPictureInfo));
	/*if((0 == jxj_cu_disp_get(logonHandle, &dispCfg)) &&
		(0 == jxj_cu_pic_get(logonHandle, &picCfg)))
		ret = 0;*/
	//if(0 ==(ret = jxj_cu_disp_get(logonHandle, &dispCfg)))
	//	jxj_cu_pic_get(logonHandle, &picCfg);
	ret = jxj_cu_disp_get(logonHandle, &dispCfg);
	if((0 == ret) && (NULL != pCfg))
	{
		dispCfg.contrast   = pCfg->struContrast.u8Value;
		dispCfg.bright     = pCfg->struBrightness.u8Value;
		dispCfg.hue 	   = pCfg->struHue.u8Value;
		dispCfg.saturation = pCfg->struSaturation.u8Value;
		dispCfg.sharpness  = pCfg->struSharpness.u8Value;

		//picCfg.awb_red = pCfg->struRed.u8Value;
		//picCfg.awb_blue = pCfg->struBlue.u8Value;
		ret = jxj_cu_disp_set(logonHandle, &dispCfg);
		//ret = jxj_cu_pic_set(logonHandle, &picCfg);
	}
	else
		__ERR("jxj_cu_net_set_ima_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_ima_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_IMA_CFG_S))
		return -1;
	
	return 0;
}

/*Vmain通用结构体函数************************************************************/
static int jxj_cu_net_get_vmain_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_VENC_CFG_S *pCfg = (IPNC_DEV_VENC_CFG_S *)pData;
	JEncodeParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JEncodeParameter));
	ret = jxj_cu_encode_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		// 视频参数
		__DBUG("cfg.frame_rate: %d\n", cfg.frame_rate);
		__DBUG("cfg.i_frame_interval: %d\n", cfg.i_frame_interval);
		pCfg->u8FrameRate = cfg.frame_rate;
		pCfg->u8Gop = cfg.i_frame_interval;

		if(cfg.video_type == J_SDK_AV_VIDEO_H264)
			pCfg->u8VideoEncType = 0;
		else if(cfg.video_type == J_SDK_AV_VIDEO_MJPEG)
			pCfg->u8VideoEncType = 1;
		else
			__ERR("The VideoEncType don't support this videoenvtype ! \n");
		
		switch(cfg.resolution)
		{
			case J_SDK_VIDEO_QCIF:
				pCfg->u16Width = 320;
				pCfg->u16Height = 192;
				break;
			case J_SDK_VIDEO_CIF	:
				pCfg->u16Width = 352;
				pCfg->u16Height = 288;
				break;
			case J_SDK_VIDEO_HD1:
				pCfg->u16Width = 704;
				pCfg->u16Height = 288;
				break;
			case J_SDK_VIDEO_D1:
				pCfg->u16Width = 704;
				pCfg->u16Height = 576;
				break;
			case J_SDK_VIDEO_QQVGA:
				pCfg->u16Width = 160;
				pCfg->u16Height = 112;
				break;
			case J_SDK_VIDEO_QVGA:
				pCfg->u16Width = 320;
				pCfg->u16Height = 240;
				break;
			case J_SDK_VIDEO_VGA:
				pCfg->u16Width = 640;
				pCfg->u16Height = 480;
				break;
			case J_SDK_VIDEO_SVGA:
				pCfg->u16Width = 800;
				pCfg->u16Height = 600;
				break;
			case J_SDK_VIDEO_UXGA:
				pCfg->u16Width = 1600;
				pCfg->u16Height = 1200;
				break;
			case J_SDK_VIDEO_720P:
				pCfg->u16Width = 1280;
				pCfg->u16Height = 720;
				break;
			case J_SDK_VIDEO_960P:
				pCfg->u16Width = 1280;
				pCfg->u16Height = 960;
				break;
			case J_SDK_VIDEO_1080P:
				pCfg->u16Width = 1920;
				pCfg->u16Height = 1080;
				break;
			default:
				pCfg->u16Width = 1280;
				pCfg->u16Height = 720;
				break;
		}

		pCfg->u8PicQuilty = cfg.qp_value;
		if(cfg.bit_rate == J_SDK_VBR)
		//位率类型又叫码率，动态码率（VBR）和固定码率（CBR）
			pCfg->u8FlowType = 1;
		else if(cfg.bit_rate == J_SDK_CBR)
			pCfg->u8FlowType = 0;

		__DBUG("The code_rate: %d\n", cfg.code_rate);

		pCfg->u32BitrateRate = cfg.code_rate;
		//pCfg->u8PalStand = 1 - cfg.format;

		__DBUG("encode level = %d\n", cfg.encode_level);
	}
	else
		__ERR("jxj_cu_net_get_vmain_param faile!\n");
	
	return ((ret == 0) ? sizeof(IPNC_DEV_VENC_CFG_S) : ret);
}
static int jxj_cu_net_set_vmain_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_VENC_CFG_S *pCfg = (IPNC_DEV_VENC_CFG_S *)pData;
	JEncodeParameter cfg;
	int ret = -1;

	memset(&cfg, 0, sizeof(JEncodeParameter));
	ret = jxj_cu_encode_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		// 视频参数
		cfg.frame_rate 	   = pCfg->u8FrameRate;
		cfg.i_frame_interval = pCfg->u8Gop;
		if(pCfg->u8VideoEncType == 0)	// H264
			cfg.video_type = J_SDK_AV_VIDEO_H264;
		else if(pCfg->u8VideoEncType == 1)	// MJPEG
			cfg.video_type = J_SDK_AV_VIDEO_MJPEG;
		else
			cfg.video_type = J_SDK_AV_CODEC_UNKNOWN;

		__DBUG("cfg.video_type: %d\n", cfg.video_type);

		if((pCfg->u16Width == 320) && (pCfg->u16Height == 192))
			cfg.resolution = J_SDK_VIDEO_CIF;
		else if((pCfg->u16Width == 704) && (pCfg->u16Height == 576))
			cfg.resolution = J_SDK_VIDEO_D1;
		else if((pCfg->u16Width == 1280) && (pCfg->u16Height == 720))
			cfg.resolution = J_SDK_VIDEO_720P;
		else if((pCfg->u16Width == 1920) && (pCfg->u16Height == 1080))
			cfg.resolution = J_SDK_VIDEO_1080P;
		else if((pCfg->u16Width == 1280) && (pCfg->u16Height == 960))
			cfg.resolution = J_SDK_VIDEO_960P;
		else
			cfg.resolution = J_SDK_VIDEO_720P;

		cfg.qp_value = pCfg->u8PicQuilty;
		cfg.code_rate = pCfg->u32BitrateRate;
		cfg.frame_priority = 0;

		__DBUG("u16Width: %d\n", pCfg->u16Width);
		__DBUG("u16Height: %d\n", pCfg->u16Height);
		__DBUG("cfg.resolution: %x\n", cfg.resolution);
		__DBUG("u8Gop: %d\n", pCfg->u8Gop);
		__DBUG("u8FrameRate: %d\n", pCfg->u8FrameRate);
		
		if(pCfg->u8FlowType == 0)	// CBR
			cfg.bit_rate = J_SDK_CBR;
		else //if(pCfg->u8FlowType == 1)
			cfg.bit_rate = J_SDK_VBR;

		//cfg.format = 1 - pCfg->u8PalStand;
		
		//设置
		ret = jxj_cu_encode_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_vmain_param faile!\n");
	
	return ret;
}
static int jxj_cu_net_check_vmain_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_VENC_CFG_S))
		return -1;
	
	return 0;
}

/*vsub通用结构体函数************************************************************/
static int jxj_cu_net_get_vsub_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_VENC_CFG_S *pCfg = (IPNC_DEV_VENC_CFG_S *)pData;
	JEncodeParameter cfg;
	int ret = -1;

	memset(&cfg, 0, sizeof(JEncodeParameter));
	cfg.level = 1;
	ret = jxj_cu_encode_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		// 视频参数
		__DBUG("cfg.frame_rate: %d\n", cfg.frame_rate);
		__DBUG("cfg.i_frame_interval: %d\n", cfg.i_frame_interval);
		pCfg->u8FrameRate = cfg.frame_rate;
		pCfg->u8Gop = cfg.i_frame_interval;
		
		if(cfg.video_type == J_SDK_AV_VIDEO_H264)
			pCfg->u8VideoEncType = 0;
		else if(cfg.video_type == J_SDK_AV_VIDEO_MJPEG)
			pCfg->u8VideoEncType = 1;
		else
			__ERR("The VideoEncType don't support this videoenvtype ! \n");

		if(cfg.resolution == J_SDK_VIDEO_D1)
		{
			pCfg->u16Width = 704;
			pCfg->u16Height = 576;
		}
		else
		{	
			pCfg->u16Width = 320;
			pCfg->u16Height = 192;		
		}

		pCfg->u8PicQuilty = cfg.qp_value;
		if(cfg.bit_rate == J_SDK_VBR)
		//位率类型又叫码率，动态码率（VBR）和固定码率（CBR）
			pCfg->u8FlowType = 1;
		else if(cfg.bit_rate == J_SDK_CBR)
			pCfg->u8FlowType = 0;

		__DBUG("The code_rate: %d\n", cfg.code_rate);

		pCfg->u32BitrateRate = cfg.code_rate;
		//pCfg->u8PalStand = 1 - cfg.format;

		__DBUG("u16Width: %d\n", pCfg->u16Width);
		__DBUG("u16Height: %d\n", pCfg->u16Height);
		__DBUG("cfg.resolution: %x\n", cfg.resolution);
		
		__DBUG("encode level = %d\n", cfg.encode_level);
	}
	else
		__ERR("jxj_cu_net_get_vmain_param faile!\n");
	
	return ((ret == 0) ? sizeof(IPNC_DEV_VENC_CFG_S) : ret);
}
static int jxj_cu_net_set_vsub_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_VENC_CFG_S *pCfg = (IPNC_DEV_VENC_CFG_S *)pData;
	JEncodeParameter cfg;
	int ret = -1;

	memset(&cfg, 0, sizeof(JEncodeParameter));
	cfg.level = 1;
	ret = jxj_cu_encode_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
				// 视频参数
		cfg.frame_rate 	   = pCfg->u8FrameRate;
		cfg.i_frame_interval = pCfg->u8Gop;
		if(pCfg->u8VideoEncType == 0)	// H264
			cfg.video_type = J_SDK_AV_VIDEO_H264;
		else if(pCfg->u8VideoEncType == 1)	// MJPEG
			cfg.video_type = J_SDK_AV_VIDEO_MJPEG;
		else
			cfg.video_type = J_SDK_AV_CODEC_UNKNOWN;

		__DBUG("cfg.video_type: %d\n", cfg.video_type);

		if((pCfg->u16Width == 320) && (pCfg->u16Height == 192))
			cfg.resolution = J_SDK_VIDEO_CIF;
		else if((pCfg->u16Width == 704) && (pCfg->u16Height == 576))
			cfg.resolution = J_SDK_VIDEO_D1;
		else if((pCfg->u16Width == 1280) && (pCfg->u16Height == 720))
			cfg.resolution = J_SDK_VIDEO_720P;
		else if((pCfg->u16Width == 1920) && (pCfg->u16Height == 1080))
			cfg.resolution = J_SDK_VIDEO_1080P;
		else if((pCfg->u16Width == 1280) && (pCfg->u16Height == 960))
			cfg.resolution = J_SDK_VIDEO_960P;
		else
			cfg.resolution = J_SDK_VIDEO_720P;

		cfg.qp_value = pCfg->u8PicQuilty;
		cfg.code_rate = pCfg->u32BitrateRate;
		cfg.frame_priority = 0;

		__DBUG("u16Width: %d\n", pCfg->u16Width);
		__DBUG("u16Height: %d\n", pCfg->u16Height);
		__DBUG("cfg.resolution: %x\n", cfg.resolution);
		__DBUG("u8Gop: %d\n", pCfg->u8Gop);
		__DBUG("u8FrameRate: %d\n", pCfg->u8FrameRate);
		
		if(pCfg->u8FlowType == 0)	// CBR
			cfg.bit_rate = J_SDK_CBR;
		else //if(pCfg->u8FlowType == 1)
			cfg.bit_rate = J_SDK_VBR;

		//cfg.format = 1 - pCfg->u8PalStand;
		//设置
		ret = jxj_cu_encode_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_vmain_param faile!\n");
	
	return ret;
}
static int jxj_cu_net_check_vsub_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_VENC_CFG_S))
		return -1;
	
	return 0;
}

/*vmask通用结构体函数************************************************************/
static int jxj_cu_net_get_vmask_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_VMASK_CFG_S *pCfg = (IPNC_DEV_VMASK_CFG_S *)pData;
	JHideParameter cfg;
	int ret = -1, area = 0;
	
	memset(&cfg, 0, sizeof(JHideParameter));
	ret = jxj_cu_vmask_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		pCfg->u8Open = cfg.hide_enable; 
		if(pCfg->u8Open == 1)	
		{
			memset(&pCfg->struArea, 0, sizeof(pCfg->struArea));
			for(area = 0; area < cfg.hide_area.count && area < IPNC_VMASK_REG_NUM; area++)
			{
				pCfg->struArea[area].s32X = cfg.hide_area.rect[area].left;
				pCfg->struArea[area].s32Y = cfg.hide_area.rect[area].top;
				pCfg->struArea[area].u32Width = cfg.hide_area.rect[area].right - cfg.hide_area.rect[area].left;
				pCfg->struArea[area].u32Height = cfg.hide_area.rect[area].bottom - cfg.hide_area.rect[area].top;
			}
		}
	
	}
	else
		__ERR("jxj_cu_param_get_vmask_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_VMASK_CFG_S) : ret);
}
static int jxj_cu_net_set_vmask_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_VMASK_CFG_S *pCfg = (IPNC_DEV_VMASK_CFG_S *)pData;
	JHideParameter cfg;
	int ret = -1, area = 0;
	
	memset(&cfg, 0, sizeof(JHideParameter));
	ret = jxj_cu_vmask_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		cfg.hide_color = 0;
		cfg.max_width  = 704;
		cfg.max_height = 576;

		cfg.hide_enable = pCfg->u8Open;
		if(cfg.hide_enable == 0)
			cfg.hide_area.count = 0;
		else
		{
			cfg.hide_area.count = 0;
			for(area = 0; area < IPNC_VMASK_REG_NUM; area++)
			{
				if(pCfg->struArea[area].s32X  == 0 &&
					pCfg->struArea[area].s32Y == 0 &&
					pCfg->struArea[area].u32Width  == 0 &&
					pCfg->struArea[area].u32Height == 0)
					continue;
				cfg.hide_area.rect[area].left  = pCfg->struArea[area].s32X;
				cfg.hide_area.rect[area].top   = pCfg->struArea[area].s32Y;
				cfg.hide_area.rect[area].right = pCfg->struArea[area].s32X 
														+ pCfg->struArea[area].u32Width;
				cfg.hide_area.rect[area].bottom = pCfg->struArea[area].s32Y 
														+ pCfg->struArea[area].u32Height;
				cfg.hide_area.count++;
			}
		}
		//设置
		ret = jxj_cu_vmask_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_param_set_vmask_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_vmask_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_VMASK_CFG_S))
		return -1;
	
	return 0;
}


/*ntp通用结构体函数************************************************************/

static int jxj_cu_net_get_ntp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_NTP_CFG_S *pCfg = (IPNC_NTP_CFG_S *)pData;
	JDeviceNTPInfo cfg;
	int ret = -1;

	memset(&cfg, 0, sizeof(JDeviceNTPInfo));
	ret = jxj_cu_ntp_get(logonHandle, &cfg);
	
	if((0 == ret) && (NULL != pCfg))
	{
		pCfg->enable = cfg.ntp_enable;
		//服务器地址
		strcpy_safe(pCfg->ntpServer, (char*)cfg.ntp_server_ip, sizeof(pCfg->ntpServer));

		//时区
		pCfg->timeZone = cfg.time_zone;
		
		//夏令时开关
		pCfg->euroTime = cfg.dst_enable;

		//定时更新时间
		pCfg->ntpRefTime = cfg.time_interval;
		//__DBUG("pCfg->ntpServer: %s\n",pCfg->ntpServer);
		//__DBUG("pCfg->timeZone: %d\n",pCfg->timeZone);
		//__DBUG("pCfg->euroTime: %d\n",pCfg->euroTime);

	}
	else
		__ERR("get_ntp_param faile!\n");
	return ((ret == 0) ? sizeof(IPNC_NTP_CFG_S) : ret);
}
static int jxj_cu_net_set_ntp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_NTP_CFG_S *pCfg = (IPNC_NTP_CFG_S *)pData;
	JDeviceNTPInfo cfg;
	int ret = -1;

	memset(&cfg, 0, sizeof(JDeviceNTPInfo));
	ret = jxj_cu_ntp_get(logonHandle, &cfg);
	
	if((0 == ret) && (NULL != pCfg))
	{
		cfg.ntp_enable = pCfg->enable;
		//服务器地址
		strcpy_safe((char*)cfg.ntp_server_ip, pCfg->ntpServer, sizeof(pCfg->ntpServer));

		//时区
		cfg.time_zone = pCfg->timeZone;
		
		//夏令时开关
		cfg.dst_enable = pCfg->euroTime;

		//定时更新时间
		cfg.time_interval = pCfg->ntpRefTime;

		//设置
		ret = jxj_cu_ntp_set(logonHandle, &cfg);
	}
	else
		__ERR("set_ntp_param faile!\n");

	return ret;
}
static int jxj_cu_net_check_ntp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_NTP_CFG_S))
		return -1;
	
	return 0;
}
/*time通用结构体函数************************************************************/
static int jxj_cu_net_get_time_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_TIME_S *pTimeInfo = (IPNC_TIME_S *)pData;
	JDeviceTime timeInfo;
	int ret = -1;
	
	memset(&timeInfo, 0, sizeof(JDeviceTime));
	ret = jxj_cu_time_get(logonHandle, &timeInfo);

	if((0 == ret) && (NULL != pTimeInfo))
	{
		pTimeInfo->u8Year 	= timeInfo.time.year;
		pTimeInfo->u8Month	= timeInfo.time.month;
		pTimeInfo->u8Day 	= timeInfo.time.date;
		pTimeInfo->u8Hour 	= timeInfo.time.hour;
		pTimeInfo->u8Minute = timeInfo.time.minute;
		pTimeInfo->u8Second = timeInfo.time.second;
		pTimeInfo->u8WDay   = timeInfo.time.weekday;
	}
	else
		__ERR("jxj_cu_param_get_time_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_TIME_S) : ret);
}
static int jxj_cu_net_set_time_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_TIME_S *pTimeInfo = (IPNC_TIME_S *)pData;
	JDeviceTime timeInfo;
	int ret = -1;
	
	memset(&timeInfo, 0, sizeof(JDeviceTime));
	ret = jxj_cu_time_get(logonHandle, &timeInfo);

	if((0 == ret) && (NULL != pTimeInfo))
	{
		
		timeInfo.time.year  = pTimeInfo->u8Year;
		timeInfo.time.month = pTimeInfo->u8Month;
		timeInfo.time.date  = pTimeInfo->u8Day;
		timeInfo.time.hour  = pTimeInfo->u8Hour;
		timeInfo.time.minute = pTimeInfo->u8Minute;
		timeInfo.time.second  = pTimeInfo->u8Second;
		timeInfo.time.weekday = pTimeInfo->u8WDay;
		
		//设置
		ret = jxj_cu_time_set(logonHandle, &timeInfo);
		//__ERR("jxj_cu_param_set_time_param end!\n");
	}
	else
		__ERR("jxj_cu_param_set_time_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_time_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_TIME_S))
		return -1;
	
	return 0;
}

/*jxj_platform通用结构体函数************************************************************/
static int jxj_cu_net_get_jxj_platform_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_JXJ_PLATFORM_CFG_S *pCfg = (IPNC_DEV_JXJ_PLATFORM_CFG_S *)pData;
	JPlatformInfo cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JPlatformInfo));
	ret = jxj_cu_jxj_platform_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		strncpy(pCfg->szPuId, (char *)cfg.pu_id, 
			sizeof(pCfg->szPuId) - 1);
		strncpy(pCfg->szCmsIp, (char *)cfg.cms_ip, 
				sizeof(pCfg->szCmsIp) - 1);
		pCfg->u16CmsPort     = cfg.cms_port;
		pCfg->u8ProtocolType = cfg.protocol;
		pCfg->u8Enable 		 = cfg.is_con_cms;
	}
	else
		__ERR("jxj_cu_net_get_jxj_platform_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_JXJ_PLATFORM_CFG_S) : ret);
}
static int jxj_cu_net_set_jxj_platform_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_JXJ_PLATFORM_CFG_S *pCfg = (IPNC_DEV_JXJ_PLATFORM_CFG_S *)pData;
	JPlatformInfo cfg;
	int ret = -1;

	memset(&cfg, 0, sizeof(JPlatformInfo));
	ret = jxj_cu_jxj_platform_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		strncpy((char *)cfg.pu_id, pCfg->szPuId, 
			sizeof(cfg.pu_id) - 1);
		strncpy((char *)cfg.cms_ip, pCfg->szCmsIp,
				sizeof(cfg.cms_ip) - 1);
		cfg.cms_port = pCfg->u16CmsPort;
		cfg.protocol = pCfg->u8ProtocolType;
		cfg.is_con_cms = pCfg->u8Enable;
		ret = jxj_cu_jxj_platform_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_jxj_platform_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_jxj_platform_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_JXJ_PLATFORM_CFG_S))
		return -1;
	
	return 0;
}

/*rec通用结构体函数************************************************************/
static int jxj_cu_net_get_rec_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_REC_STRC_S *pCfg = (IPNC_DEV_REC_STRC_S *)pData;
	JRecordParameter cfg;
	JTimeSeg *pTimeSeg = NULL;
	int i = 0, day = 0, seg = 0, ret = -1;
	
	memset(&cfg, 0, sizeof(JRecordParameter));
	ret = jxj_cu_rec_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		pCfg->u8RecStream = cfg.level;
		pCfg->u8PreRecSecs = cfg.pre_record;
		pCfg->u8OverWrite = cfg.auto_cover;
		pCfg->u8Enable = 1;
		pCfg->u8RecType = 1;		// 暂定为定时录像，客户端不支持配置其它录像方式
		memset(&pCfg->struTimerSeg, 0, sizeof(pCfg->struTimerSeg));
		for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
		{
			day = (i + 1) % J_SDK_MAX_DAY_SIZE;
			if(cfg.week.days[i].all_day_enable)
			{
				pCfg->struTimerSeg[0][day].u8Open = 1;
				pCfg->struTimerSeg[0][day].beginSec = 0;
				pCfg->struTimerSeg[0][day].endSec = 24 * 3600 - 1;
			}
			else
			{
				for(seg = 0; seg < cfg.week.days[i].count && seg < IPNC_MAX_SEG_NUM; seg++)
				{
					pTimeSeg = &cfg.week.days[i].seg[seg];
					if(pTimeSeg->time_end.hour > 23)
						continue;
					pCfg->struTimerSeg[seg][day].u8Open = pTimeSeg->enable;
					pCfg->struTimerSeg[seg][day].beginSec = (pTimeSeg->time_start.hour * 3600)
																+ (pTimeSeg->time_start.minute * 60)
																+ pTimeSeg->time_start.second;
					pCfg->struTimerSeg[seg][day].endSec = (pTimeSeg->time_end.hour * 3600)
																+ (pTimeSeg->time_end.minute * 60)
																+ pTimeSeg->time_end.second;
				}
			}
		}
	}
	else
		__ERR("jxj_cu_net_get_rec_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_REC_STRC_S) : ret);
}
static int jxj_cu_net_set_rec_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_REC_STRC_S *pCfg = (IPNC_DEV_REC_STRC_S *)pData;
	JRecordParameter cfg;
	int i = 0, day = 0, seg = 0, ret = -1;

	memset(&cfg, 0, sizeof(JRecordParameter));
	ret = jxj_cu_rec_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		cfg.level = pCfg->u8RecStream;	
		cfg.auto_cover = pCfg->u8OverWrite;
		cfg.pre_record = pCfg->u8PreRecSecs;
		
		for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
		{
			cfg.week.days[i].day_id = i;
			day = (i + 1) % J_SDK_MAX_DAY_SIZE;
			for(seg = 0; seg < IPNC_MAX_SEG_NUM; seg++)
			{
				if(pCfg->struTimerSeg[seg][day].u8Open == 0)
					continue;
				if(pCfg->struTimerSeg[seg][day].beginSec == 0 
					&& pCfg->struTimerSeg[seg][day].endSec == 24 * 3600 - 1)	// 全天
				{
					cfg.week.days[i].count = 0;
					cfg.week.days[i].all_day_enable = 1;
					memset(cfg.week.days[i].seg, 0, sizeof(cfg.week.days[i].seg));
					break;
					// 因为平台使用的全天时间是00:00:00 - 24:00:00
					//pCfg->struTimerSeg[seg][day].endSec++;
				}
				cfg.week.days[i].seg[seg].enable = pCfg->struTimerSeg[seg][day].u8Open;
				cfg.week.days[i].seg[seg].time_start.hour = pCfg->struTimerSeg[seg][day].beginSec / 3600;
				cfg.week.days[i].seg[seg].time_start.minute = (pCfg->struTimerSeg[seg][day].beginSec % 3600) / 60;
				cfg.week.days[i].seg[seg].time_start.second = pCfg->struTimerSeg[seg][day].beginSec % 60;
				cfg.week.days[i].seg[seg].time_end.hour = pCfg->struTimerSeg[seg][day].endSec / 3600;
				cfg.week.days[i].seg[seg].time_end.minute = (pCfg->struTimerSeg[seg][day].endSec % 3600) / 60;
				cfg.week.days[i].seg[seg].time_end.second = pCfg->struTimerSeg[seg][day].endSec % 60;
				cfg.week.days[i].count++;
			}
		}
		cfg.week.count = i;
		ret = jxj_cu_rec_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_rec_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_rec_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_REC_STRC_S))
		return -1;
	
	return 0;
}

/*user通用结构体函数************************************************************/
static int jxj_cu_net_get_user_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_USER_CFG_S *pCfg = (IPNC_USER_CFG_S *)pData;
	JUserInfo cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JUserInfo));
	ret = jxj_cu_user_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		//user
		strcpy_safe(pCfg->userName, (char*)cfg.username,
					sizeof(pCfg->userName));
		//pwd
		strcpy_safe(pCfg->passWord, (char*)cfg.password,
					sizeof(pCfg->passWord));

	}
	else
		__ERR("jxj_cu_net_get_user_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_USER_CFG_S) : ret);
}
static int jxj_cu_net_set_user_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_USER_CFG_S *pCfg = (IPNC_USER_CFG_S *)pData;
	JUserInfo cfg;
	int ret = -1;

	memset(&cfg, 0, sizeof(JUserInfo));
	ret = jxj_cu_user_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
				//user
		strcpy_safe((char*)cfg.username, pCfg->userName, 
					sizeof(cfg.username));
		//pwd
		strcpy_safe((char*)cfg.password, pCfg->passWord, 
					sizeof(cfg.password));

		ret = jxj_cu_user_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_user_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_user_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_USER_CFG_S))
		return -1;
	
	return 0;
}

/*comm通用结构体函数************************************************************/
static const unsigned int array_uart_baudrate[][2] = {
									{0, 50},
									{1, 75},
									{2, 110},
									{3, 150},
									{4, 300},
									{5, 600},
									{6, 1200},
									{7, 2400},
									{8, 4800},
									{9, 9600},
									{10, 19200},
									{11, 38400},
									{12, 57600},
									{13, 76800},
									{14, 115200}
								};

static int jxj_cu_net_get_comm_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_COMM_CFG_S *pCfg = (IPNC_COMM_CFG_S *)pData;
	JSerialParameter cfg;
	int ret = -1, i = 0;
	
	memset(&cfg, 0, sizeof(JSerialParameter));
	ret = jxj_cu_comm_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		for(i = 0; i < sizeof(array_uart_baudrate)/sizeof(array_uart_baudrate[0][0])/2; i++)
		{
			if(cfg.baud_rate == array_uart_baudrate[i][1])
			{
				pCfg->u32BaudRate = array_uart_baudrate[i][0];
				break;
			}
		}
		pCfg->u8DataBit = cfg.data_bit - 5; 
		pCfg->u8StopBit = cfg.stop_bit - 1; 
		pCfg->u8Parity = cfg.verify;

	}
	else
		__ERR("jxj_cu_net_get_comm_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_COMM_CFG_S) : ret);
}
static int jxj_cu_net_set_comm_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_COMM_CFG_S *pCfg = (IPNC_COMM_CFG_S *)pData;
	JSerialParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JSerialParameter));
	ret = jxj_cu_comm_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		
		cfg.serial_no = 0;
		cfg.baud_rate = array_uart_baudrate[pCfg->u32BaudRate][1];
		cfg.data_bit = pCfg->u8DataBit + 5;
		cfg.stop_bit = pCfg->u8StopBit + 1;
		cfg.verify = pCfg->u8Parity;

		ret = jxj_cu_comm_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_comm_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_comm_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_COMM_CFG_S))
		return -1;
	
	return 0;
}
/*ptz通用结构体函数************************************************************/
static int jxj_cu_net_get_ptz_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_PTZ_CFG_S *pCfg = (IPNC_PTZ_CFG_S *)pData;
	JPTZParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JPTZParameter));
	ret = jxj_cu_ptz_cfg_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		//云台协议
		pCfg->u8ProtocalType = (unsigned char)cfg.protocol;
		//云台地址码
		pCfg->u8PtzAddr = (unsigned char)cfg.ptz_addr;
	}
	else
		__ERR("jxj_cu_net_get_ptz_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_PTZ_CFG_S) : ret);
}
static int jxj_cu_net_set_ptz_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_PTZ_CFG_S *pCfg = (IPNC_PTZ_CFG_S *)pData;
	JPTZParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JPTZParameter));
	ret = jxj_cu_ptz_cfg_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		//云台协议
		cfg.protocol = pCfg->u8ProtocalType;
		//云台地址码
		cfg.ptz_addr = pCfg->u8PtzAddr;

		ret = jxj_cu_ptz_cfg_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_ptz_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_ptz_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_PTZ_CFG_S))
		return -1;
	
	return 0;
}

/*ftp通用结构体函数************************************************************/
static int jxj_cu_net_get_ftp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_FTP_CFG_S *pCfg = (IPNC_FTP_CFG_S *)pData;
	JFTPParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JFTPParameter));
	ret = jxj_cu_ftp_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe(pCfg->ftpSvr, (char *)cfg.ftp_ip, sizeof(pCfg->ftpSvr));
		strcpy_safe(pCfg->ftpUsr, (char *)cfg.ftp_usr, sizeof(pCfg->ftpUsr));
		strcpy_safe(pCfg->ftpPsw, (char *)cfg.ftp_pwd, sizeof(pCfg->ftpPsw));
		strcpy_safe(pCfg->ftpPath, (char *)cfg.ftp_path, sizeof(pCfg->ftpPath));
		pCfg->u16FtpPort = cfg.ftp_port;
	}
	else
		__ERR("jxj_cu_net_get_ftp_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_FTP_CFG_S) : ret);
}
static int jxj_cu_net_set_ftp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_FTP_CFG_S *pCfg = (IPNC_FTP_CFG_S *)pData;
	JFTPParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JFTPParameter));
	ret = jxj_cu_ftp_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe((char *)cfg.ftp_ip, pCfg->ftpSvr, sizeof(cfg.ftp_ip));
		strcpy_safe((char *)cfg.ftp_usr, pCfg->ftpUsr, sizeof(cfg.ftp_usr));
		strcpy_safe((char *)cfg.ftp_pwd, pCfg->ftpPsw, sizeof(cfg.ftp_pwd));
		strcpy_safe((char *)cfg.ftp_path, pCfg->ftpPath, sizeof(cfg.ftp_path));
		cfg.ftp_port = pCfg->u16FtpPort;

		ret = jxj_cu_ftp_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_ftp_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_ftp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_FTP_CFG_S))
		return -1;
	
	return 0;
}
/*smtp通用结构体函数************************************************************/
static int jxj_cu_net_get_smtp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_SMTP_CFG_S *pCfg = (IPNC_SMTP_CFG_S *)pData;
	JSMTPParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JSMTPParameter));
	ret = jxj_cu_smtp_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe(pCfg->mailHub, (char *)cfg.mail_ip, 
					sizeof(pCfg->mailHub));
		strcpy_safe(pCfg->mailAddr, (char *)cfg.mail_addr, 
					sizeof(pCfg->mailAddr));
		strcpy_safe(pCfg->mailUsr, (char *)cfg.mail_usr, 
					sizeof(pCfg->mailUsr));
		strcpy_safe(pCfg->mailPsw, (char *)cfg.mail_pwd, 
					sizeof(pCfg->mailPsw));
		strcpy_safe(pCfg->mailRctp1, (char *)cfg.mail_rctp1, 
					sizeof(pCfg->mailRctp1));
		strcpy_safe(pCfg->mailRctp2, (char *)cfg.mail_rctp2, 
					sizeof(pCfg->mailRctp2));
		strcpy_safe(pCfg->mailRctp3, (char *)cfg.mail_rctp3, 
					sizeof(pCfg->mailRctp3));
		pCfg->u16MailPort = cfg.mail_port;
		//pCfg->u8EnableSSL = cfg.ssl_enable;
	}
	else
		__ERR("jxj_cu_net_get_smtp_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_SMTP_CFG_S) : ret);
}
static int jxj_cu_net_set_smtp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_SMTP_CFG_S *pCfg = (IPNC_SMTP_CFG_S *)pData;
	JSMTPParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JSMTPParameter));
	ret = jxj_cu_smtp_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe((char *)cfg.mail_ip, pCfg->mailHub, 
					sizeof(cfg.mail_ip));
		strcpy_safe((char *)cfg.mail_addr, pCfg->mailAddr,  
					sizeof(cfg.mail_addr));
		strcpy_safe((char *)cfg.mail_usr, pCfg->mailUsr,  
					sizeof(cfg.mail_usr));
		strcpy_safe((char *)cfg.mail_pwd, pCfg->mailPsw,  
					sizeof(cfg.mail_pwd));
		strcpy_safe((char *)cfg.mail_rctp1, pCfg->mailRctp1, 
					sizeof(cfg.mail_rctp1));
		strcpy_safe((char *)cfg.mail_rctp2, pCfg->mailRctp2,  
					sizeof(cfg.mail_rctp2));
		strcpy_safe((char *)cfg.mail_rctp3, pCfg->mailRctp3, 
					sizeof(cfg.mail_rctp3));
		cfg.mail_port = pCfg->u16MailPort;
		ret = jxj_cu_smtp_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_smtp_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_smtp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_SMTP_CFG_S))
		return -1;
	
	return 0;
}
/*upnp通用结构体函数************************************************************/
static int jxj_cu_net_get_upnp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_UPNP_CFG_S *pCfg = (IPNC_UPNP_CFG_S *)pData;
	JUPNPParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JUPNPParameter));
	ret = jxj_cu_upnp_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe(pCfg->szUpnpSvr, (char *)cfg.upnp_ip, 
					sizeof(pCfg->szUpnpSvr));
		pCfg->u8UpnpOpen = cfg.upnp_enable;
		pCfg->u8UpnpEthNo = cfg.upnp_eth_no;
		pCfg->u8UpnpModel = cfg.upnp_model;
		pCfg->u8UpnpRefTime = cfg.upnp_refresh_time;

		pCfg->u16UpnpJXJDataPort = cfg.upnp_data_port;
		pCfg->u16UpnpDataPort= cfg.upnp_data_port;
		pCfg->u16UpnpWebPort = cfg.upnp_web_port;
		pCfg->u16UpnpCmdPort = cfg.upnp_cmd_port;
		pCfg->u16UpnpTalkPort = cfg.upnp_talk_port;

		pCfg->u16UpnpJXJDataPortS = cfg.upnp_data_port_result;
		pCfg->u16UpnpDataPortS = cfg.upnp_data_port_result;
		pCfg->u16UpnpWebPortS = cfg.upnp_web_port_result;
		pCfg->u16UpnpCmdPortS = cfg.upnp_cmd_port_result;
		pCfg->u16UpnpTalkPortS = cfg.upnp_talk_port_result;
	}
	else
		__ERR("jxj_cu_net_get_upnp_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_UPNP_CFG_S) : ret);
}
static int jxj_cu_net_set_upnp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_UPNP_CFG_S *pCfg = (IPNC_UPNP_CFG_S *)pData;
	JUPNPParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JUPNPParameter));
	ret = jxj_cu_upnp_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe((char *)cfg.upnp_ip, pCfg->szUpnpSvr, 
					sizeof(cfg.upnp_ip));
		cfg.upnp_enable = pCfg->u8UpnpOpen;
		cfg.upnp_eth_no = pCfg->u8UpnpEthNo;
		cfg.upnp_model = pCfg->u8UpnpModel;
		cfg.upnp_refresh_time = pCfg->u8UpnpRefTime;

		cfg.upnp_data_port = pCfg->u16UpnpJXJDataPort;
		cfg.upnp_data_port = pCfg->u16UpnpDataPort;
		cfg.upnp_web_port  = pCfg->u16UpnpWebPort;
		cfg.upnp_cmd_port  = pCfg->u16UpnpCmdPort;
		cfg.upnp_talk_port = pCfg->u16UpnpTalkPort;

		cfg.upnp_data_port_result = pCfg->u16UpnpJXJDataPortS;
		cfg.upnp_data_port_result = pCfg->u16UpnpDataPortS;
		cfg.upnp_web_port_result  = pCfg->u16UpnpWebPortS;
		cfg.upnp_cmd_port_result  = pCfg->u16UpnpCmdPortS;
		cfg.upnp_talk_port_result = pCfg->u16UpnpTalkPortS;
		
		ret = jxj_cu_upnp_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_upnp_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_upnp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_UPNP_CFG_S))
		return -1;
	
	return 0;
}

/*ddns通用结构体函数************************************************************/
static int jxj_cu_net_get_ddns_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DDNS_CFG_S *pCfg = (IPNC_DDNS_CFG_S *)pData;
	JDdnsConfig cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JDdnsConfig));
	ret = jxj_cu_ddns_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe(pCfg->szDDNSAccount, (char *)cfg.ddns_account, 
					sizeof(pCfg->szDDNSAccount));
		strcpy_safe(pCfg->szDDNSUsr, (char *)cfg.ddns_usr, 
					sizeof(pCfg->szDDNSUsr));
		strcpy_safe(pCfg->szDDNSPsw, (char *)cfg.ddns_pwd, 
					sizeof(pCfg->szDDNSPsw));

		pCfg->u8DDNSOpen = cfg.ddns_open;
		pCfg->u8DDNSType = cfg.ddns_type;
		pCfg->u16DDNSPort = cfg.ddns_port;
		pCfg->u32DDNSTimes = cfg.ddns_times;
	}
	else
		__ERR("jxj_cu_net_get_ddns_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DDNS_CFG_S) : ret);
}
static int jxj_cu_net_set_ddns_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DDNS_CFG_S *pCfg = (IPNC_DDNS_CFG_S *)pData;
	JDdnsConfig cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JDdnsConfig));
	ret = jxj_cu_ddns_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		strcpy_safe((char *)cfg.ddns_account, pCfg->szDDNSAccount, 
					sizeof(cfg.ddns_account));
		strcpy_safe((char *)cfg.ddns_usr, pCfg->szDDNSUsr, 
					sizeof(cfg.ddns_usr));
		strcpy_safe((char *)cfg.ddns_pwd, pCfg->szDDNSPsw, 
					sizeof(cfg.ddns_pwd));

		cfg.ddns_open = pCfg->u8DDNSOpen;
		cfg.ddns_type = pCfg->u8DDNSType;
		cfg.ddns_port = pCfg->u16DDNSPort;
		cfg.ddns_times = pCfg->u32DDNSTimes;
		
		ret = jxj_cu_ddns_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_ddns_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_ddns_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DDNS_CFG_S))
		return -1;
	
	return 0;
}
/*osd通用结构体函数************************************************************/
static int jxj_cu_net_get_osd_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_OSD_CFG_S *pCfg = (IPNC_DEV_OSD_CFG_S *)pData;
	JOSDParameter cfg;
	char text[512] = {0};
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JOSDParameter));
	ret = jxj_cu_osd_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		//时间使能 --1 显示时间osd 0--隐藏时间osd
		pCfg->u8EnableTimeOsd  = cfg.time_enable;
		//时间码率显示位置
		pCfg->struTimeOrg.s32X = cfg.time_display_x;
		pCfg->struTimeOrg.s32Y = cfg.time_display_y;
		//标题使能 --1 显示标题osd 0--隐藏标题osd
		pCfg->u8EnableTitleOsd = cfg.text_enable;
		//标题显示位置
		pCfg->struTitleOrg.s32X = cfg.text_display_x;
		pCfg->struTitleOrg.s32Y = cfg.text_display_y;
		//时间与标题显示颜色
		if(cfg.osd_invert_color)
		{
			pCfg->u8OsdColor	= 4;
			pCfg->u8ColorValidFlag = 1;
		}
		else
		{
			pCfg->u32ChnColor = cfg.text_display_color;
			pCfg->u32TimeColor = cfg.time_display_color;
			pCfg->u8ColorValidFlag = 0;
		}
		//码率使能 --1显示码流osd  0--隐藏码流osd
		pCfg->u8EnableBitrateOsd = cfg.stream_enable;
		
		__DBUG("text data len: %d\n", strlen((char*)cfg.text_data));
		//显示通道字符串
		jxj_cu_utf8_to_gb2312((char *)cfg.text_data, strlen((char *)cfg.text_data), text);
		text[sizeof(text)-1] = '\0';
		strcpy_safe(pCfg->szChnName, text, sizeof(pCfg->szChnName));
	}
	else
		__ERR("jxj_cu_net_get_osd_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_OSD_CFG_S) : ret);
}
static int jxj_cu_net_set_osd_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_OSD_CFG_S *pCfg = (IPNC_DEV_OSD_CFG_S *)pData;
	JOSDParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JOSDParameter));
	ret = jxj_cu_osd_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		//时间使能 --1 显示时间osd 0--隐藏时间osd
		cfg.time_enable = pCfg->u8EnableTimeOsd;
		//时间码率显示位置
		cfg.time_display_x = pCfg->struTimeOrg.s32X;
		cfg.time_display_y = pCfg->struTimeOrg.s32Y;
		
		//标题使能 --1 显示标题osd 0--隐藏标题osd
		cfg.text_enable = pCfg->u8EnableTitleOsd;
		//标题显示位置
		cfg.text_display_x = pCfg->struTitleOrg.s32X;
		cfg.text_display_y = pCfg->struTitleOrg.s32Y;
		
		//时间与标题显示颜色
		if((pCfg->u8ColorValidFlag) && (pCfg->u8OsdColor == 4))
		{
			cfg.time_display_color = 0;
			cfg.text_display_color = 0;
			cfg.osd_invert_color = 1;
		}else
		{
			cfg.time_display_color = pCfg->u32TimeColor;
			cfg.text_display_color = pCfg->u32ChnColor;
			cfg.osd_invert_color = 0;
		}
		//码率使能 --1显示码流osd  0--隐藏码流osd
		cfg.stream_enable = pCfg->u8EnableBitrateOsd;
		
		//显示通道字符串
		memset(cfg.text_data, 0, sizeof(cfg.text_data));
		jxj_cu_gb2312_to_utf8(pCfg->szChnName, strlen(pCfg->szChnName), (char*)cfg.text_data);
		cfg.text_data[sizeof(cfg.text_data)-1] = 0;
		__DBUG("text data len: %d\n", strlen((char *)cfg.text_data));
		__DBUG("text : %s\n", cfg.text_data);

		ret = jxj_cu_osd_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_osd_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_osd_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_OSD_CFG_S))
		return -1;
	
	return 0;
}
/*audio通用结构体函数************************************************************/
static int jxj_cu_net_get_audio_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_AUDIO_CFG_S *pCfg = (IPNC_AUDIO_CFG_S *)pData;
	JEncodeParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JEncodeParameter));
	ret = jxj_cu_encode_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		// 音频参数
		__DBUG("cfg.audio_type : %d\n", cfg.audio_type);
		
		if(cfg.audio_type == J_SDK_AV_AUDIO_G711A)
			pCfg->u8AEncType = 1;
		else if(cfg.audio_type == J_SDK_AV_AUDIO_G711U)
			pCfg->u8AEncType = 2;

		pCfg->u8Type = (cfg.au_in_mode==J_SDK_AUDIO_MIC_IN) ? 0 : 1;
		pCfg->u8Open = cfg.audio_enble;
	}
	else
		__ERR("jxj_cu_net_get_audio_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_AUDIO_CFG_S) : ret);
}

static int jxj_cu_net_set_audio_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_AUDIO_CFG_S *pCfg = (IPNC_AUDIO_CFG_S *)pData;
	JEncodeParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JEncodeParameter));
	ret = jxj_cu_encode_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		// 音频参数
		if(pCfg->u8AEncType == 1)	// G711A
			cfg.audio_type = J_SDK_AV_AUDIO_G711A;
		else if(pCfg->u8AEncType == 2)	// G711U
			cfg.audio_type = J_SDK_AV_AUDIO_G711U;
		else
			cfg.audio_type = J_SDK_AV_CODEC_UNKNOWN;

		//__DBUG("cfg.audio_type: %d", cfg.audio_type);
		//__DBUG("audio sample: %u", pCfg->u32SampleRate);
		
		cfg.au_in_mode = (pCfg->u8Type == 0)?J_SDK_AUDIO_MIC_IN : J_SDK_AUDIO_LINE_IN;
		cfg.audio_enble = pCfg->u8Open;

		ret = jxj_cu_encode_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_audio_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_audio_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_AUDIO_CFG_S))
		return -1;
	
	return 0;
}
/*video通用结构体函数************************************************************/
static int jxj_cu_net_get_video_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_VIDEO_CFG_S *pCfg = (IPNC_DEV_VIDEO_CFG_S *)pData;
	JEncodeParameter encode;

	int ret = 0;

	memset(&encode, 0, sizeof(JEncodeParameter));
	/*if((0 == (ret = jxj_cu_encode_get(logonHandle, &encode))) &&
	   (ret = jxj_cu_net_get_osd_param(logonHandle, nPort, 
							 &pCfg->struOsd, sizeof(pCfg->struOsd))) &&
	   (ret = jxj_cu_net_get_vmask_param(logonHandle, nPort, 
							 &pCfg->struVMask, sizeof(pCfg->struVMask))) &&
	   (ret = jxj_cu_net_get_vmain_param(logonHandle, nPort, 
							 &pCfg->struMainVenc, sizeof(pCfg->struMainVenc))) &&
	   (ret = jxj_cu_net_get_vsub_param(logonHandle, nPort, 
							 &pCfg->struSubVenc, sizeof(pCfg->struSubVenc))) &&
	   (ret = jxj_cu_net_get_ima_param(logonHandle, nPort, 
							 &pCfg->struIma, sizeof(pCfg->struIma))))*/
	if(0 == (ret = jxj_cu_encode_get(logonHandle, &encode)))
	{
	   jxj_cu_net_get_osd_param(logonHandle, nPort, 
							 &pCfg->struOsd, sizeof(pCfg->struOsd));
	   jxj_cu_net_get_vmask_param(logonHandle, nPort, 
							 &pCfg->struVMask, sizeof(pCfg->struVMask));
	   jxj_cu_net_get_vmain_param(logonHandle, nPort, 
							 &pCfg->struMainVenc, sizeof(pCfg->struMainVenc));
	   jxj_cu_net_get_vsub_param(logonHandle, nPort, 
							 &pCfg->struSubVenc, sizeof(pCfg->struSubVenc));
	   jxj_cu_net_get_ima_param(logonHandle, nPort, 
							 &pCfg->struIma, sizeof(pCfg->struIma));
							 
		if(encode.encode_level == J_SDK_BASELINE)
			pCfg->eEncodeLevel = IPNC_H264_BASELINE;
		else if(encode.encode_level == J_SDK_MAIN)
			pCfg->eEncodeLevel = IPNC_H264_MAIN;
		else if(encode.encode_level == J_SDK_HIGH)
			pCfg->eEncodeLevel = IPNC_H264_HIGH;

		pCfg->u8PalStand = 1 - encode.format;
	}
	else
		__DBUG("jxj_cu_net_get_video_param failed: %d. \n", ret);

	return ((ret == 0) ? sizeof(IPNC_DEV_VIDEO_CFG_S) : ret);
}
static int jxj_cu_net_set_video_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_VIDEO_CFG_S *pCfg = (IPNC_DEV_VIDEO_CFG_S *)pData;
	JEncodeParameter encode;
	int ret = 0;

	memset(&encode, 0, sizeof(JEncodeParameter));
	ret = jxj_cu_encode_get(logonHandle, &encode);
	if(0 == ret)
	{
		if(pCfg->eEncodeLevel == IPNC_H264_HIGH)
			encode.encode_level = J_SDK_HIGH;
		else if(pCfg->eEncodeLevel == IPNC_H264_MAIN)
			encode.encode_level = J_SDK_MAIN;
		else if(pCfg->eEncodeLevel == IPNC_H264_BASELINE)
			encode.encode_level = J_SDK_BASELINE;

		encode.format = 1 - pCfg->u8PalStand;
		ret = jxj_cu_encode_set(logonHandle, &encode);

		jxj_cu_net_set_osd_param(logonHandle, nPort, 
								 &pCfg->struOsd, sizeof(pCfg->struOsd));
		jxj_cu_net_set_vmask_param(logonHandle, nPort, 
								 &pCfg->struVMask, sizeof(pCfg->struVMask));
		jxj_cu_net_set_vmain_param(logonHandle, nPort, 
								 &pCfg->struMainVenc, sizeof(pCfg->struMainVenc));
		jxj_cu_net_set_vsub_param(logonHandle, nPort, 
								 &pCfg->struSubVenc, sizeof(pCfg->struSubVenc));
		jxj_cu_net_set_ima_param(logonHandle, nPort, 
								 &pCfg->struIma, sizeof(pCfg->struIma));
	}
	
	return ret;
}

static int jxj_cu_net_check_video_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_VIDEO_CFG_S))
		return -1;
	
	return 0;
}

/*md通用结构体函数************************************************************/
static int jxj_cu_net_get_md_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_MD_CFG_S *pCfg = (IPNC_DEV_MD_CFG_S *)pData;
	JMoveAlarm cfg;
	JJointAction jACfg;
	JTimeSeg *pSeg;
	int ret = -1;
	int i, rect, day, seg = 0;
	
	memset(&cfg, 0, sizeof(JMoveAlarm));
	memset(&jACfg, 0, sizeof(JJointAction));
	ret = jxj_cu_md_get(logonHandle, &cfg);
	ret = jxj_cu_ja_get(logonHandle, &jACfg);
	
	if((0 == ret) && (NULL != pCfg))
	{
		/*if(cfg.max_width != 704 || cfg.max_height != 576)
		{
			__ERR("The max height and max width is wrong.\n");
			return -1;
		}*/
		pCfg->u32Size = sizeof(IPNC_DEV_MD_CFG_S);
		pCfg->u8Enable = cfg.move_enable;
		pCfg->u8Level = cfg.sensitive_level;
		pCfg->u32DetectInterval = cfg.detect_interval;
		__DBUG("sensitive level: %d\n", cfg.sensitive_level);
		__DBUG("max_width: %d, max_height: %d\n", cfg.max_width, cfg.max_height);

		//获取联动信息
		//录像通道(bit) (0 不录像 1第一通道录像)
		pCfg->struLinkAct.u32RecChannel = jACfg.joint.joint_record_enable_channel;
		//联动输出(bit)
		pCfg->struLinkAct.u32OutPut = jACfg.joint.joint_output_enable_channel;
		//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
		pCfg->struLinkAct.u32ShootChannel = jACfg.joint.joint_snap_enable_channel;
		//录像时间(秒)
		pCfg->struLinkAct.u32RecTime = jACfg.joint.joint_record_second;
		//pCfg->struLinkAct.u32OutPut = jACfg.joint.joint_beep_enable;
		//pCfg->struLinkAct.u32OutPersist = jACfg.joint.joint_beep_second;
		//输出持续时间
		pCfg->struLinkAct.u32OutPersist = jACfg.joint.joint_output_times;
		//抓拍间隔(毫秒),0:连续抓拍
		pCfg->struLinkAct.u32ShootInterval = jACfg.joint.joint_snap_interval * 1000;
		//抓拍张数	
		pCfg->struLinkAct.u8ShootNum = jACfg.joint.joint_snap_times;
		//联动email
		pCfg->struLinkAct.u8Email = jACfg.joint.joint_email_enable;

		//获取布防时段
		memset(&pCfg->struRect, 0, sizeof(pCfg->struRect));
		for(rect = 0; rect < cfg.detect_area.count && rect < IPNC_VMD_REG_NUM; rect++)
		{
			pCfg->struRect[rect].s32X = cfg.detect_area.rect[rect].left;
			pCfg->struRect[rect].s32Y = cfg.detect_area.rect[rect].top;
			pCfg->struRect[rect].u32Width = cfg.detect_area.rect[rect].right -
											cfg.detect_area.rect[rect].left;
			pCfg->struRect[rect].u32Height = cfg.detect_area.rect[rect].bottom -
											cfg.detect_area.rect[rect].top;
			__DBUG("left: %d, right: %d, top: %d, bottom: %d\n", cfg.detect_area.rect[rect].left,
													cfg.detect_area.rect[rect].right,
													cfg.detect_area.rect[rect].top,
													cfg.detect_area.rect[rect].bottom);
			__DBUG("ipc: left: %d, right: %d, top: %d, bottom: %d\n", 	pCfg->struRect[rect].s32X,
														pCfg->struRect[rect].s32Y,
														pCfg->struRect[rect].u32Width,
														pCfg->struRect[rect].u32Height);
		}

		//获取侦测区域
		memset(&pCfg->struSeg, 0, sizeof(pCfg->struSeg));
		for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
		{
			day = (i + 1) % J_SDK_MAX_DAY_SIZE;
			if(cfg.week.days[i].all_day_enable)
			{
				pCfg->struSeg[0][day].u8Open = 1;
				pCfg->struSeg[0][day].beginSec = 0;
				pCfg->struSeg[0][day].endSec = 24 * 3600 - 1;
			}
			else
			{
				for(seg = 0; seg < cfg.week.days[i].count && seg < IPNC_MAX_SEG_NUM; seg++)
				{
					pSeg = &cfg.week.days[i].seg[seg];
					if(pSeg->time_end.hour > 23)
						continue;
					pCfg->struSeg[seg][day].u8Open = pSeg->enable;
					pCfg->struSeg[seg][day].beginSec = (pSeg->time_start.hour * 3600) +
															(pSeg->time_start.minute * 60) +
															pSeg->time_start.second;
					pCfg->struSeg[seg][day].endSec = (pSeg->time_end.hour * 3600) +
															(pSeg->time_end.minute * 60) +
															pSeg->time_end.second;
				}
			}
		}
	}
	else
		__ERR("jxj_cu_net_get_md_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_MD_CFG_S) : ret);
}
static int jxj_cu_net_set_md_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_MD_CFG_S *pCfg = (IPNC_DEV_MD_CFG_S *)pData;
	JMoveAlarm cfg;
	JJointAction jACfg;
	JTimeSeg *pSeg;
	int ret = -1;
	int i, rect, day, seg = 0;
	
	memset(&cfg, 0, sizeof(JMoveAlarm));
	memset(&jACfg, 0, sizeof(JJointAction));
	ret = jxj_cu_md_get(logonHandle, &cfg);
	ret = jxj_cu_ja_get(logonHandle, &jACfg);
	
	if((0 == ret) && (NULL != pCfg))
	{
		cfg.move_enable = pCfg->u8Enable;
		cfg.sensitive_level = pCfg->u8Level;
		cfg.detect_interval = pCfg->u32DetectInterval;
		cfg.max_width = 704;
		cfg.max_height = 576;

		//录像通道(bit) (0 不录像 1第一通道录像)
		jACfg.joint.joint_record_enable_channel = pCfg->struLinkAct.u32RecChannel;
		//联动输出(bit)
		jACfg.joint.joint_output_enable_channel = pCfg->struLinkAct.u32OutPut;
		//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
		jACfg.joint.joint_snap_enable_channel = pCfg->struLinkAct.u32ShootChannel;
		//录像时间(秒)
		jACfg.joint.joint_record_second = pCfg->struLinkAct.u32RecTime;
		//jACfg.joint.joint_beep_enable = pCfg->struLinkAct.u32OutPut;
		//jACfg.joint.joint_beep_second = pCfg->struLinkAct.u32OutPersist;
		//输出持续时间
		jACfg.joint.joint_output_times = pCfg->struLinkAct.u32OutPersist;
		//抓拍间隔(毫秒),0:连续抓拍
		jACfg.joint.joint_snap_interval = pCfg->struLinkAct.u32ShootInterval/1000;
		//抓拍张数	
		jACfg.joint.joint_snap_times = pCfg->struLinkAct.u8ShootNum;
		//联动email
		jACfg.joint.joint_email_enable = pCfg->struLinkAct.u8Email;
		
		for(rect = 0; rect < J_SDK_MAX_DECTION_AREA_SIEZ && rect < IPNC_VMD_REG_NUM; rect++)
		{
			if(pCfg->struRect[rect].s32X == 0 && pCfg->struRect[rect].s32Y == 0 &&
				pCfg->struRect[rect].u32Height == 0 && pCfg->struRect[rect].u32Width == 0)
				continue;
			cfg.detect_area.rect[rect].left = pCfg->struRect[rect].s32X;
			cfg.detect_area.rect[rect].top = pCfg->struRect[rect].s32Y;
			cfg.detect_area.rect[rect].right = pCfg->struRect[rect].s32X + 
														pCfg->struRect[rect].u32Width;
			cfg.detect_area.rect[rect].bottom = pCfg->struRect[rect].s32Y + 
														pCfg->struRect[rect].u32Height;
			cfg.detect_area.count++;
		}

		for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
		{
			cfg.week.days[i].day_id = i;
			day = (i + 1) % J_SDK_MAX_DAY_SIZE;
			for(seg = 0; seg < IPNC_MAX_SEG_NUM; seg++)
			{
				if(pCfg->struSeg[seg][day].u8Open == 0)
					continue;
				if(pCfg->struSeg[seg][day].beginSec == 0 
					&& pCfg->struSeg[seg][day].endSec == 24 * 3600 - 1)	// 全天
				{
					cfg.week.days[i].all_day_enable = 1;
					// 因为平台使用的全天时间是00:00:00 - 24:00:00
					pCfg->struSeg[seg][day].endSec++;
				}
				pSeg = &cfg.week.days[i].seg[seg];
				pSeg->enable = pCfg->struSeg[seg][day].u8Open;
				pSeg->time_start.hour = pCfg->struSeg[seg][day].beginSec / 3600;
				pSeg->time_start.minute = (pCfg->struSeg[seg][day].beginSec % 3600) / 60;
				pSeg->time_start.second = pCfg->struSeg[seg][day].beginSec % 60;
				pSeg->time_end.hour = pCfg->struSeg[seg][day].endSec / 3600;
				pSeg->time_end.minute = (pCfg->struSeg[seg][day].endSec % 3600) / 60;
				pSeg->time_end.second = pCfg->struSeg[seg][day].endSec % 60;
				cfg.week.days[i].count++;
			}
		}
		cfg.week.count = i;
		ret = jxj_cu_md_set(logonHandle, &cfg);
		ret = jxj_cu_ja_set(logonHandle, &jACfg);
	}
	else
		__ERR("jxj_cu_net_set_md_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_md_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_MD_CFG_S))
		return -1;
	
	return 0;
}

/*di通用结构体函数************************************************************/
static int jxj_cu_net_get_di_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_DI_CFG_S *pCfg = (IPNC_DEV_DI_CFG_S *)pData;
	JIoAlarm cfg;
	JJointAction jACfg;
	JTimeSeg *pSeg;
	int i, day, seg = 0;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JIoAlarm));
	memset(&jACfg, 0, sizeof(JJointAction));
	ret = jxj_cu_di_get(logonHandle, &cfg);
	ret = jxj_cu_ja_get(logonHandle, &jACfg);
	
	if((0 == ret) && (NULL != pCfg))
	{
		pCfg->u32Size = sizeof(IPNC_DEV_DI_CFG_S);
		//是否开启di报警检测 0--不开启 
		pCfg->u8Enable = cfg.alarm_enable;
		//0--常开探头1--常闭探头
		pCfg->u8DiType = (cfg.io_type == NORMAL_OPEN) ? 0 : 1;
		//检测间隔时间
		pCfg->u32DetectInterval = cfg.detect_interval;

		//联动信息
		//录像通道(bit) (0 不录像 1第一通道录像)
		pCfg->struLinkAct.u32RecChannel = jACfg.joint.joint_record_enable_channel;
		//联动输出(bit)
		pCfg->struLinkAct.u32OutPut = jACfg.joint.joint_output_enable_channel;
		//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
		pCfg->struLinkAct.u32ShootChannel = jACfg.joint.joint_snap_enable_channel;
		//录像时间(秒)
		pCfg->struLinkAct.u32RecTime = jACfg.joint.joint_record_second;
		//pCfg->struLinkAct.u32OutPut = jACfg.joint.joint_beep_enable;
		//pCfg->struLinkAct.u32OutPersist = jACfg.joint.joint_beep_second;
		//输出持续时间
		pCfg->struLinkAct.u32OutPersist = jACfg.joint.joint_output_times;
		//抓拍间隔(毫秒),0:连续抓拍
		pCfg->struLinkAct.u32ShootInterval = jACfg.joint.joint_snap_interval * 1000;
		//抓拍张数	
		pCfg->struLinkAct.u8ShootNum = jACfg.joint.joint_snap_times;//0~5
		//联动email
		pCfg->struLinkAct.u8Email = jACfg.joint.joint_email_enable;
		
		//布防时段
		memset(&pCfg->struSeg, 0, sizeof(pCfg->struSeg));
		for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
		{
			day = (i + 1) % J_SDK_MAX_DAY_SIZE;
			if(cfg.week.days[i].all_day_enable)
			{
				pCfg->struSeg[0][day].u8Open = 1;
				pCfg->struSeg[0][day].beginSec = 0;
				pCfg->struSeg[0][day].endSec = 24 * 3600 - 1;
			}
			else
			{
				for(seg = 0; seg < cfg.week.days[i].count && seg < IPNC_MAX_SEG_NUM; seg++)
				{
					pSeg = &cfg.week.days[i].seg[seg];
					if(pSeg->time_end.hour > 23)
						continue;
					pCfg->struSeg[seg][day].u8Open = pSeg->enable;
					pCfg->struSeg[seg][day].beginSec = (pSeg->time_start.hour * 3600) +
															(pSeg->time_start.minute * 60) +
															pSeg->time_start.second;
					pCfg->struSeg[seg][day].endSec = (pSeg->time_end.hour * 3600) +
															(pSeg->time_end.minute * 60) +
															pSeg->time_end.second;
				}
			}
		}
	}
	else
		__ERR("jxj_cu_net_get_di_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_DI_CFG_S) : ret);
}
static int jxj_cu_net_set_di_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_DI_CFG_S *pCfg = (IPNC_DEV_DI_CFG_S *)pData;
	JIoAlarm cfg;
	JJointAction jACfg;
	JTimeSeg *pSeg;
	int i, day, seg = 0;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JIoAlarm));
	memset(&jACfg, 0, sizeof(JJointAction));
	ret = jxj_cu_di_get(logonHandle, &cfg);
	ret = jxj_cu_ja_get(logonHandle, &jACfg);
	
	if((0 == ret) && (NULL != pCfg))
	{
		__ERR(">>>>>>>>>>>>>>pCfg->u32DetectInterval: %d\n", pCfg->u32DetectInterval);
		//是否开启di报警检测 0--不开启 
		cfg.io_type = (pCfg->u8DiType == 0) ? NORMAL_OPEN : NORMAL_CLOSE;
		//0--常开探头1--常闭探头
		cfg.alarm_enable = pCfg->u8Enable;
		//检测间隔时间
		cfg.detect_interval = pCfg->u32DetectInterval;

		//录像通道(bit) (0 不录像 1第一通道录像)
		jACfg.joint.joint_record_enable_channel = pCfg->struLinkAct.u32RecChannel;
		//联动输出(bit)
		jACfg.joint.joint_output_enable_channel = pCfg->struLinkAct.u32OutPut;
		//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
		jACfg.joint.joint_snap_enable_channel = pCfg->struLinkAct.u32ShootChannel;
		//录像时间(秒)
		jACfg.joint.joint_record_second = pCfg->struLinkAct.u32RecTime;
		//联动输出(bit)
		jACfg.joint.joint_beep_enable = pCfg->struLinkAct.u32OutPut;
		//输出持续时间
		jACfg.joint.joint_beep_second = pCfg->struLinkAct.u32OutPersist;
		//输出持续时间
		jACfg.joint.joint_output_times = pCfg->struLinkAct.u32OutPersist;
		//抓拍间隔(毫秒),0:连续抓拍
		jACfg.joint.joint_snap_interval = pCfg->struLinkAct.u32ShootInterval/1000;
		//抓拍张数	
		jACfg.joint.joint_snap_times = pCfg->struLinkAct.u8ShootNum;
		//联动email
		jACfg.joint.joint_email_enable = pCfg->struLinkAct.u8Email;
		
		//布防时段
		for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
		{
			cfg.week.days[i].day_id = i;
			day = (i + 1) % J_SDK_MAX_DAY_SIZE;
			for(seg = 0; seg < IPNC_MAX_SEG_NUM; seg++)
			{
				if(pCfg->struSeg[seg][day].u8Open == 0)
					continue;
				if(pCfg->struSeg[seg][day].beginSec == 0 
					&& pCfg->struSeg[seg][day].endSec == 24 * 3600 - 1)	// 全天
				{
					cfg.week.days[i].all_day_enable = 1;
					// 因为平台使用的全天时间是00:00:00 - 24:00:00
					pCfg->struSeg[seg][day].endSec++;
				}
				pSeg = &cfg.week.days[i].seg[seg];
				pSeg->enable = !!pCfg->struSeg[seg][day].u8Open;
				pSeg->time_start.hour = pCfg->struSeg[seg][day].beginSec / 3600;
				pSeg->time_start.minute = (pCfg->struSeg[seg][day].beginSec % 3600) / 60;
				pSeg->time_start.second = pCfg->struSeg[seg][day].beginSec % 60;
				pSeg->time_end.hour = pCfg->struSeg[seg][day].endSec / 3600;
				pSeg->time_end.minute = (pCfg->struSeg[seg][day].endSec % 3600) / 60;
				pSeg->time_end.second = pCfg->struSeg[seg][day].endSec % 60;
				cfg.week.days[i].count++;
			}
		}
		cfg.week.count = i;
		ret = jxj_cu_di_set(logonHandle, &cfg);
		ret = jxj_cu_ja_set(logonHandle, &jACfg);
	}
	else
		__ERR("jxj_cu_net_set_di_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_di_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_DI_CFG_S))
		return -1;
	
	return 0;
}

/*3A通用结构体函数************************************************************/
static int jxj_cu_net_get_3a_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	JXJ_CU_LOGON_S *pLongHdl = (JXJ_CU_LOGON_S *)logonHandle;
	IPNC_DEV_3A_CFG_S *pCfg = (IPNC_DEV_3A_CFG_S *)pData;
	JPictureInfo *pPicCfg;
	JPictureInfo *pDefPicCfg;
	//J3DControl    j3dCfg = {0};
	//JIrcutControl ircutCfg = {0};
	int ret = 0;

	if(pLongHdl == NULL || pCfg == NULL)
		return -1;
	//memset(&j3dCfg, 0, sizeof(j3dCfg));
	//memset(&irCfg, 0, sizeof(irCfg));
	/*if((0 == jxj_cu_pic_get(logonHandle, &picCfg)) &&
	   (0 == jxj_cu_3d_get(logonHandle, &j3dCfg)) &&
	   (0 == jxj_cu_ir_get(logonHandle, &irCfg)))
	   ret = 0;*/

	/*if((0 == jxj_cu_pic_get(logonHandle, &picCfg))
		&& (0 == jxj_cu_ir_get(logonHandle, &irCfg)))
	   ret = 0;*/

	if(pLongHdl->recvPicCfg == 0)
	{
		ret = jxj_cu_pic_get(logonHandle, &pLongHdl->picCfg);
		if(ret >= 0)
			pLongHdl->recvPicCfg = 1;
	}
	pPicCfg = &pLongHdl->picCfg;
	if(0 == ret)
	{
		//pic
		pCfg->struMirror.u8Value= pPicCfg->mirror;
		pCfg->struFlip.u8Value = pPicCfg->flip;
		if(pPicCfg->hz == 0)
			pCfg->struPwdFreq.u8Value = 0;
		else if(pPicCfg->hz == 1)
			pCfg->struPwdFreq.u8Value = 0;
		else
			pCfg->struPwdFreq.u8Value = 1;
		pCfg->struShutterSpeed.u8Value = pPicCfg->shutter;	
		
		if(pLongHdl->recvDefPicCfg == 0)
		{
			ret = jxj_cu_pic_def_get(logonHandle, &pLongHdl->defPicCfg);
			if(ret >= 0)
				pLongHdl->recvDefPicCfg = 1;
		}
		pDefPicCfg = &pLongHdl->defPicCfg;
		if(ret == 0)
		{
			pCfg->struMirror.u8Default= pDefPicCfg->mirror;
			pCfg->struFlip.u8Default = pDefPicCfg->flip;
			if(pDefPicCfg->hz == 0)
				pCfg->struPwdFreq.u8Default = 0;
			else if(pDefPicCfg->hz == 1)
				pCfg->struPwdFreq.u8Default = 0;
			else
				pCfg->struPwdFreq.u8Default = 1;
			pCfg->struShutterSpeed.u8Default = pDefPicCfg->shutter;	
		}
	}
	else
		__ERR("jxj_cu_net_get_3a_param faile!\n");
	
	return ((ret == 0) ? sizeof(IPNC_DEV_3A_CFG_S) : ret);
}
static int jxj_cu_net_set_3a_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	JXJ_CU_LOGON_S *pLongHdl = (JXJ_CU_LOGON_S *)logonHandle;
	IPNC_DEV_3A_CFG_S *pCfg = (IPNC_DEV_3A_CFG_S *)pData;
	JPictureInfo *pPicCfg;
	//J3DControl    j3dCfg;
	//JIrcutControl ircutCfg = {0};
	int ret = 0;
	
	if(pLongHdl == NULL);
	if(pLongHdl == NULL)
		return -1;
	//memset(&picCfg, 0, sizeof(picCfg));
	//memset(&j3dCfg, 0, sizeof(j3dCfg));
	//memset(&irCfg, 0, sizeof(irCfg));
	/*if((0 == jxj_cu_pic_get(logonHandle, &picCfg)) &&
	   (0 == jxj_cu_3d_get(logonHandle, &j3dCfg)) &&
	   (0 == jxj_cu_ir_get(logonHandle, &irCfg)))*/
	/*if((0 == jxj_cu_pic_get(logonHandle, &picCfg))
		&& (0 == jxj_cu_ir_get(logonHandle, &irCfg)))
	   ret = 0;*/

	if(pLongHdl->recvPicCfg == 0)
	{
		ret = jxj_cu_pic_get(logonHandle, &pLongHdl->picCfg);
		if(ret >= 0)
			pLongHdl->recvPicCfg = 1;
	}
	pPicCfg = &pLongHdl->picCfg;
	if((0 == ret) && (NULL != pCfg))
	{
		//pic
		pPicCfg->mirror = pCfg->struMirror.u8Value;
		pPicCfg->flip   = pCfg->struFlip.u8Value;
		if(pCfg->struPwdFreq.u8Value == 0)
			pPicCfg->hz = 1;
		else
			pPicCfg->hz = 2;
		pPicCfg->shutter = pCfg->struShutterSpeed.u8Value;
		
		ret = jxj_cu_pic_set(logonHandle, pPicCfg);
	}
	else
		__ERR("jxj_cu_net_set_3a_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_3a_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_3A_CFG_S))
		return -1;
	
	return 0;
}

/*ir通用结构体函数************************************************************/
static int jxj_cu_net_get_ircfmode_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	JXJ_CU_LOGON_S *pLongHdl = (JXJ_CU_LOGON_S *)logonHandle;
	IPNC_DEV_IRCFMODE_CFG_S *pCfg = (IPNC_DEV_IRCFMODE_CFG_S *)pData;
	JIrcutControl *pIrcutCfg;
	int ret = 0, i = 0, day = 0;

	if(pLongHdl == NULL)
		return -1;
	if(pLongHdl->recvIrcutCfg == 0)
	{
		ret = jxj_cu_ir_get(logonHandle, &pLongHdl->ircutCfg);
		if(ret >= 0)
			pLongHdl->recvIrcutCfg = 1;
	}
	pIrcutCfg = &pLongHdl->ircutCfg;
	if((0 == ret) && (NULL != pCfg))
	{
		if(pIrcutCfg->ircut[0].ircut_mode == IRCUT_AUTO)
			pCfg->u8Mode = 0;
		else if(pIrcutCfg->ircut[0].ircut_mode == IRCUT_RTC)
		{
			if(pIrcutCfg->ircut[0].auto_c2b == 1)
				pCfg->u8Mode = 1;
			else
				pCfg->u8Mode = 2;
		}
		else
			pCfg->u8Mode = 3;
		//if(pCfg->u8Mode == 3)
		{
			for(day = 0; day < J_SDK_MAX_DAY_SIZE; day++)
			{
				for(i=0; i<J_SDK_MAX_SEG_SZIE; i++)
				{
					pCfg->struSeg[i][(day+1)%J_SDK_MAX_DAY_SIZE].u8Open = pIrcutCfg->ircut[0].timers[day].seg_time[i].open;
					pCfg->struSeg[i][(day+1)%J_SDK_MAX_DAY_SIZE].beginSec = pIrcutCfg->ircut[0].timers[day].seg_time[i].begin_sec;
					pCfg->struSeg[i][(day+1)%J_SDK_MAX_DAY_SIZE].endSec = pIrcutCfg->ircut[0].timers[day].seg_time[i].end_sec;
				}
			}
		}
	}
	else
		__ERR("jxj_cu_net_get_ircfmode_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_IRCFMODE_CFG_S) : ret);
}
static int jxj_cu_net_set_ircfmode_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	JXJ_CU_LOGON_S *pLongHdl = (JXJ_CU_LOGON_S *)logonHandle;
	IPNC_DEV_IRCFMODE_CFG_S *pCfg = (IPNC_DEV_IRCFMODE_CFG_S *)pData;
	JIrcutControl *pIrcutCfg;
	int ret = 0, i = 0;
	int day = 0;

	if(pLongHdl == NULL)
		return -1;
	if(pLongHdl->recvIrcutCfg == 0)
	{
		ret = jxj_cu_ir_get(logonHandle, &pLongHdl->ircutCfg);
		if(ret >= 0)
			pLongHdl->recvIrcutCfg = 1;
	}
	pIrcutCfg = &pLongHdl->ircutCfg;
	
	if((0 == ret) && (NULL != pCfg))
	{
		pIrcutCfg->count = 1;		//	暂时只支持一个通道
		switch(pCfg->u8Mode)
		{
			default:
				/* break */
			case 0:
				pIrcutCfg->ircut[0].ircut_mode = IRCUT_AUTO;
				pIrcutCfg->ircut[0].auto_c2b = 0;
				pIrcutCfg->ircut[0].autos.sensitive = 0;
				break;
			case 1:
				pIrcutCfg->ircut[0].ircut_mode = IRCUT_RTC;
				pIrcutCfg->ircut[0].auto_c2b = 1;
				pIrcutCfg->ircut[0].rtcs.rtc = 0;
				break;
			case 2:
				pIrcutCfg->ircut[0].ircut_mode = IRCUT_RTC;
				pIrcutCfg->ircut[0].auto_c2b = 0;
				pIrcutCfg->ircut[0].rtcs.rtc = 1;
				break;
			case 3:
				pIrcutCfg->ircut[0].ircut_mode = IRCUT_TIMER;
				pIrcutCfg->ircut[0].auto_c2b = 0;
				for(day = 0; day < J_SDK_MAX_DAY_SIZE; day++)
				{
					for(i=0; i<J_SDK_MAX_SEG_SZIE; i++)
					{	
						pIrcutCfg->ircut[0].timers[day].seg_time[i].open = pCfg->struSeg[i][(day+1)%J_SDK_MAX_DAY_SIZE].u8Open;
						pIrcutCfg->ircut[0].timers[day].seg_time[i].begin_sec = pCfg->struSeg[i][(day+1)%J_SDK_MAX_DAY_SIZE].beginSec;
						pIrcutCfg->ircut[0].timers[day].seg_time[i].end_sec = pCfg->struSeg[i][(day+1)%J_SDK_MAX_DAY_SIZE].endSec;
					}
				}
				break;
		}
		ret = jxj_cu_ir_set(logonHandle, pIrcutCfg);
	}
	else
		__ERR("jxj_cu_net_set_ircfmode_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_ircfmode_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_IRCFMODE_CFG_S))
		return -1;
	
	return 0;
}

/*pp通用结构体函数************************************************************/
static int jxj_cu_net_get_pp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_PP_SET_CFG_S *pCfg = (IPNC_DEV_PP_SET_CFG_S *)pData;
	JPPSet cfg;
	int ret = -1, i = -1;
	
	memset(&cfg, 0, sizeof(JPPSet));
	ret = jxj_cu_pp_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		pCfg->pp_count = cfg.pp_count;
		for(i=0; i < pCfg->pp_count; i++)
		{
			memcpy(&pCfg->pp[i], &cfg.pp[i], sizeof(JPresetPoint));
		}
	}
	else
		__ERR("jxj_cu_net_get_pp_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_PP_SET_CFG_S) : ret);
}
static int jxj_cu_net_set_pp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_PP_SET_CFG_S *pCfg = (IPNC_DEV_PP_SET_CFG_S *)pData;
	JPPSet cfg;
	int ret = -1, i = 0;
	
	memset(&cfg, 0, sizeof(JPPSet));
	ret = jxj_cu_pp_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		cfg.pp_count = pCfg->pp_count;
		for(i=0; i < cfg.pp_count; i++)
		{
			memcpy(&cfg.pp[i], &pCfg->pp[i], sizeof(JPresetPoint));
		}
		ret = jxj_cu_pp_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_pp_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_pp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_PP_SET_CFG_S))
		return -1;
	
	return 0;
}
/*ball通用结构体函数************************************************************/
static int jxj_cu_net_get_ball_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_BALL_RUN_PARAM_S *pCfg = (IPNC_BALL_RUN_PARAM_S *)pData;
	IPNC_BALL_CRUISE_PARAM_S *pCruiseParam;
	//JCruiseConfig cpcCfg;
	JCruiseWay	  cwcCfg;
	//JCruiseConfig cpcCfg;
	JCruiseWay	  *pCruiseWay = &cwcCfg;
	
	int ret = -1, i = 0;
	
	//memset(&cpcCfg, 0, sizeof(JCruiseConfig));
	memset(&cwcCfg, 0, sizeof(JCruiseWay));
	//ret = jxj_cu_cpc_get(logonHandle, &cpcCfg);
	ret = jxj_cu_cwc_get(logonHandle, &cwcCfg);
	
	if((0 == ret) && (NULL != pCfg))
	{
		pCruiseParam = &pCfg->struSettingParam.struCruiseParam;
		strcpy((char *)&pCruiseParam->crz_name, (char *)&pCruiseWay->crz_info.crz_name);
		pCruiseWay->crz_info.crz_no = 1;
		pCruiseParam->crz_no = 1;	//只有一条路径
		pCruiseParam->crz_enable = 1;
		for(i = 0; i < IPNC_BALL_CRUISE_NUM; i++)	//因为只有一条路径，所以添加一条路径时将前面的路径清空
		{
			pCruiseParam->struCruiseNode[i].byCruisePresetEnable = 0;
		}
		for(i = 0; i < pCruiseWay->pp_count; i++)	//设置巡航点参数
		{
			if(pCruiseWay->crz_pp[i].dwell < 3)
				pCruiseWay->crz_pp[i].dwell = 3;
			else if(pCruiseWay->crz_pp[i].dwell > 240)
				pCruiseWay->crz_pp[i].dwell = 240;
			if(pCruiseWay->crz_pp[i].speed < 10)
				pCruiseWay->crz_pp[i].speed = 10;
			else if(pCruiseWay->crz_pp[i].speed > 120)
				pCruiseWay->crz_pp[i].speed = 120;
			if(pCruiseWay->crz_pp[i].preset <= 0 || pCruiseWay->crz_pp[i].preset > 16)
				continue;
			pCruiseParam->struCruiseNode[pCruiseWay->crz_pp[i].preset-1].byCruisePresetEnable = 1;
			pCruiseParam->struCruiseNode[pCruiseWay->crz_pp[i].preset-1].byCruisePresetSec = pCruiseWay->crz_pp[i].dwell; 
			pCruiseParam->struCruiseNode[pCruiseWay->crz_pp[i].preset-1].byCruisePresetSpeed = pCruiseWay->crz_pp[i].speed;
		}
	
	}
	else
		__ERR("jxj_cu_net_get_ball_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_BALL_RUN_PARAM_S) : ret);
}
static int jxj_cu_net_set_ball_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_BALL_RUN_PARAM_S *pCfg = (IPNC_BALL_RUN_PARAM_S *)pData;
	IPNC_BALL_CRUISE_PARAM_S *pCruiseParam;
	//JCruiseConfig cpcCfg;
	JCruiseWay	  cwcCfg;
	JCruiseWay	  *pCruiseWay = &cwcCfg;
	int ret = -1, i = 0, nCount = 0;
	
	//memset(&cpcCfg, 0, sizeof(JCruiseConfig));
	memset(&cwcCfg, 0, sizeof(JCruiseWay));
	//ret = jxj_cu_cpc_get(logonHandle, &cpcCfg);
	ret = jxj_cu_cwc_get(logonHandle, &cwcCfg);
	if((0 == ret) && (NULL != pCfg))
	{
		pCruiseParam = &pCfg->struSettingParam.struCruiseParam;
		strcpy((char *)&pCruiseWay->crz_info.crz_name, (char *)&pCruiseParam->crz_name);
		pCruiseWay->crz_info.crz_no = pCruiseParam->crz_no;

		for(i = 0; i < IPNC_BALL_CRUISE_NUM; i++)
		{
			if(pCruiseParam->struCruiseNode[i].byCruisePresetEnable)
			{
				pCruiseWay->crz_pp[nCount].preset = i + 1;
				pCruiseWay->crz_pp[nCount].speed = pCruiseParam->struCruiseNode[i].byCruisePresetSpeed;
				pCruiseWay->crz_pp[nCount].dwell = pCruiseParam->struCruiseNode[i].byCruisePresetSec;
				nCount++;
			}
		}
		pCruiseWay->pp_count = nCount;
			
		//ret = jxj_cu_cpc_set(logonHandle, &cpcCfg);
		ret = jxj_cu_cwc_set(logonHandle, &cwcCfg);
	}
	else
		__ERR("jxj_cu_net_set_ball_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_ball_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_BALL_RUN_PARAM_S))
		return -1;
	
	return 0;
}

/*osd_lable通用结构体函数************************************************************/
static int jxj_cu_net_get_osd_lable_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_OSD_LABEL_CFG_S *pCfg = (IPNC_DEV_OSD_LABEL_CFG_S *)pData;
	JOSDParameter cfg;
	int ret = -1;
	char text[512] = {0};
	memset(&cfg, 0, sizeof(JOSDParameter));
	ret = jxj_cu_osd_get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
		pCfg->u32EnableLabel[0] = cfg.ext_osd.ext_text1_enable;
		pCfg->struLabelOrg[0].s32X = cfg.ext_osd.ext_text1_display_x + cfg.ext_osd.ext_text1_display_w;
		pCfg->struLabelOrg[0].s32Y = cfg.ext_osd.ext_text1_display_y;	//标注的字体是右对齐的
		if(cfg.osd_invert_color)
		{
			pCfg->u32OsdColor[0] = 4;
			pCfg->u32ColorValidFlag[0] = 0;
		}
		else
		{
			pCfg->u32LabelColor[0] = cfg.text_display_color;
			pCfg->u32ColorValidFlag[0] = 1;
		}
		memset(text, 0, sizeof(text));
		jxj_cu_utf8_to_gb2312((char *)cfg.ext_osd.ext_text1_data, strlen((char *)cfg.ext_osd.ext_text1_data), text);
		text[sizeof(text)-1] = '\0';
		strncpy(pCfg->szLabelName[0], text, sizeof(pCfg->szLabelName[0])-1);
		pCfg->szLabelName[0][sizeof(pCfg->szLabelName[0])-1] = '\0';
		
		pCfg->u32EnableLabel[1] = cfg.ext_osd.ext_text2_enable;
		pCfg->struLabelOrg[1].s32X = cfg.ext_osd.ext_text2_display_x + cfg.ext_osd.ext_text2_display_w;
		pCfg->struLabelOrg[1].s32Y = cfg.ext_osd.ext_text2_display_y;
		if(cfg.osd_invert_color)
		{
			pCfg->u32OsdColor[1] = 4;
			pCfg->u32ColorValidFlag[1] = 0;
		}
		else
		{
			pCfg->u32LabelColor[1] = cfg.text_display_color;
			pCfg->u32ColorValidFlag[1] = 1;
		}
		memset(text, 0, sizeof(text));
		jxj_cu_utf8_to_gb2312((char *)cfg.ext_osd.ext_text2_data, strlen((char *)cfg.ext_osd.ext_text2_data), text);
		text[sizeof(text)-1] = '\0';
		strncpy(pCfg->szLabelName[1], text, sizeof(pCfg->szLabelName[1])-1);
		pCfg->szLabelName[1][sizeof(pCfg->szLabelName[1])-1] = '\0';
	}
	else
		__ERR("jxj_cu_net_get_osd_lable_param faile!\n");

	return ((ret == 0) ? sizeof(IPNC_DEV_OSD_LABEL_CFG_S) : ret);
}
// pOsdStr 为GB2312字符串
static int jxj_cu_get_osd_pixel_len(const char *pOsdStr, int *width, int *height)
{
	int i, cnt, charSize=32, strHeight = 32, strWidth = 0, strWidthMax = 0;
	const char *ch;

	cnt = strlen(pOsdStr);
	ch = pOsdStr;
	for(i=0; i<cnt; i++)
	{
		if(ch[i] == '\n')
		{
			if(strWidthMax < strWidth )
				strWidthMax = strWidth;
			strWidth =	0;
			strHeight += charSize;
			continue;
		}
		if(ch[i] >= 128)
		{
			if(ch[i+1] >= 128)
			{
				strWidth += charSize;
				i++;
			}
			else
			{
				i++;
				continue;
			}
		}
		else
		{
			strWidth += charSize/2;
		}
	}
	if(strWidthMax < strWidth)
	{
		strWidthMax = strWidth;
	}
	
	if(width != NULL)
		*width = strWidthMax;
	if(height != NULL)
		*height = strHeight;
	return 0;
}

static int jxj_cu_net_set_osd_lable_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC_DEV_OSD_LABEL_CFG_S *pCfg = (IPNC_DEV_OSD_LABEL_CFG_S *)pData;
	IPNC_DEV_VENC_CFG_S stVencCfg = {0};
	JOSDParameter cfg;
	int ret = -1;
	int width;
	int height;
	
	memset(&cfg, 0, sizeof(JOSDParameter));
	ret = jxj_cu_osd_get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{
		jxj_cu_net_get_vmain_param(logonHandle, nPort, 
							 &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S));
		 
		cfg.ext_osd.ext_text1_enable = pCfg->u32EnableLabel[0];
		memset(cfg.ext_osd.ext_text1_data, 0, sizeof(cfg.ext_osd.ext_text1_data));
		jxj_cu_gb2312_to_utf8(pCfg->szLabelName[0], strlen(pCfg->szLabelName[0]), 
			(char*)cfg.ext_osd.ext_text1_data);
		cfg.ext_osd.ext_text1_data[sizeof(cfg.ext_osd.ext_text1_data)-1] = 0;
		jxj_cu_get_osd_pixel_len(pCfg->szLabelName[0], &width, &height);
		cfg.ext_osd.ext_text1_display_w = width*704/stVencCfg.u16Width;
		cfg.ext_osd.ext_text1_display_h = height*576/stVencCfg.u16Height;
		cfg.ext_osd.ext_text1_display_x = pCfg->struLabelOrg[0].s32X - cfg.ext_osd.ext_text1_display_w;
		cfg.ext_osd.ext_text1_display_y = pCfg->struLabelOrg[0].s32Y;

		cfg.ext_osd.ext_text2_enable = pCfg->u32EnableLabel[1];
		memset(cfg.ext_osd.ext_text2_data, 0, sizeof(cfg.ext_osd.ext_text2_data));
		jxj_cu_gb2312_to_utf8(pCfg->szLabelName[1], strlen(pCfg->szLabelName[1]), 
			(char*)cfg.ext_osd.ext_text2_data);
		cfg.ext_osd.ext_text2_data[sizeof(cfg.ext_osd.ext_text2_data)-1] = 0;
		jxj_cu_get_osd_pixel_len(pCfg->szLabelName[1], &width, &height);
		cfg.ext_osd.ext_text2_display_w = width*704/stVencCfg.u16Width;
		cfg.ext_osd.ext_text2_display_h = height*576/stVencCfg.u16Height;
		cfg.ext_osd.ext_text2_display_x = pCfg->struLabelOrg[1].s32X - cfg.ext_osd.ext_text2_display_w;
		cfg.ext_osd.ext_text2_display_y = pCfg->struLabelOrg[1].s32Y;
		ret = jxj_cu_osd_set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set_osd_lable_param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check_osd_lable_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_OSD_LABEL_CFG_S))
		return -1;
	
	return 0;
}
static int jxj_cu_net_set_reboot(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	JControlDevice stCtrlDev;
	stCtrlDev.command = 2;
	return jxj_cu_reboot_set(logonHandle, &stCtrlDev);
}
static int jxj_cu_net_set_halt(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	JControlDevice stCtrlDev;
	stCtrlDev.command = 1;
	return jxj_cu_reboot_set(logonHandle, &stCtrlDev);
}
static int jxj_cu_net_set_default(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	JControlDevice stCtrlDev;
	stCtrlDev.command = 3;
	return jxj_cu_default_set(logonHandle, &stCtrlDev);
}
/*通用结构体函数************************************************************/
/*
static int jxj_cu_net_get__param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC__CFG_S *pCfg = (IPNC__CFG_S *)pData;
	JSerialParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JSerialParameter));
	ret = jxj_cu__get(logonHandle, &cfg);

	if((0 == ret) && (NULL != pCfg))
	{
	}
	else
		__ERR("jxj_cu_net_get__param faile!\n");

	return ((ret == 0) ? sizeof(IPNC__CFG_S) : ret);
}
static int jxj_cu_net_set__param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	IPNC__CFG_S *pCfg = (IPNC__CFG_S *)pData;
	JSerialParameter cfg;
	int ret = -1;
	
	memset(&cfg, 0, sizeof(JSerialParameter));
	ret = jxj_cu__get(logonHandle, &cfg);
	if((0 == ret) && (NULL != pCfg))
	{

		ret = jxj_cu__set(logonHandle, &cfg);
	}
	else
		__ERR("jxj_cu_net_set__param faile!\n");
	
	return ret;
}

static int jxj_cu_net_check__param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC__CFG_S))
		return -1;
	
	return 0;
}

*/


//结构体操作函数集
static JXJ_CU_NET_CMD_PARAM_S sCmdParam[] =
{  
	{IPNC_CMD_DEV_CFG, -1,	/*设备信息配置*/
		jxj_cu_net_get_dev_param,		
		jxj_cu_net_set_dev_param,
		jxj_cu_net_check_dev_param},
		
	{IPNC_CMD_NET_CFG, -1,	/*设备网络配置*/
		jxj_cu_net_get_net_param,		
		jxj_cu_net_set_net_param,
		jxj_cu_net_check_net_param},

	{IPNC_CMD_FTP_CFG, -1,	/*设备ftp配置*/
		jxj_cu_net_get_ftp_param,		
		jxj_cu_net_set_ftp_param,
		jxj_cu_net_check_ftp_param},

	{IPNC_CMD_EMAIL_CFG, -1,/*设备smtp配置*/
		jxj_cu_net_get_smtp_param,		
		jxj_cu_net_set_smtp_param,
		jxj_cu_net_check_smtp_param},

	{IPNC_CMD_NTP_CFG, -1,/*设备ntp配置*/
		jxj_cu_net_get_ntp_param,		
		jxj_cu_net_set_ntp_param,
		jxj_cu_net_check_ntp_param},

	{IPNC_CMD_UPNP_CFG, -1,/*设备upnp配置*/
		jxj_cu_net_get_upnp_param,		
		jxj_cu_net_set_upnp_param,
		jxj_cu_net_check_upnp_param},

	{IPNC_CMD_DDNS_CFG, -1,/*设备ddns配置*/
		jxj_cu_net_get_ddns_param,		
		jxj_cu_net_set_ddns_param,
		jxj_cu_net_check_ddns_param},

	{IPNC_CMD_COMM_CFG, -1,/*设备comm配置*/
		jxj_cu_net_get_comm_param,		
		jxj_cu_net_set_comm_param,
		jxj_cu_net_check_comm_param},

	{IPNC_CMD_PPPOE_CFG, -1,/*设备pppoe配置*/
		jxj_cu_net_get_pppoe_param,		
		jxj_cu_net_set_pppoe_param,
		jxj_cu_net_check_pppoe_param},

	{IPNC_CMD_PTZ_CFG, -1,/*设备ptz配置*/
		jxj_cu_net_get_ptz_param,		
		jxj_cu_net_set_ptz_param,
		jxj_cu_net_check_ptz_param},

	{IPNC_CMD_AUDIO_CFG, -1,/*设备audio配置*/
		jxj_cu_net_get_audio_param,		
		jxj_cu_net_set_audio_param,
		jxj_cu_net_check_audio_param},

	{IPNC_CMD_USR_CFG, -1,/*设备user配置*/
		jxj_cu_net_get_user_param,		
		jxj_cu_net_set_user_param,
		jxj_cu_net_check_user_param},

	{IPNC_CMD_OSD_CFG, -1,/*设备osd配置*/
		jxj_cu_net_get_osd_param,		
		jxj_cu_net_set_osd_param,
		jxj_cu_net_check_osd_param},

	{IPNC_CMD_VMASK_CFG, -1,/*设备vmask配置*/
		jxj_cu_net_get_vmask_param,		
		jxj_cu_net_set_vmask_param,
		jxj_cu_net_check_vmask_param},
		
	{IPNC_CMD_VMAIN_CFG, -1,/*设备vmain配置*/
		jxj_cu_net_get_vmain_param,		
		jxj_cu_net_set_vmain_param,
		jxj_cu_net_check_vmain_param},

	{IPNC_CMD_VSUB_CFG, -1,/*设备vsub配置*/
		jxj_cu_net_get_vsub_param,		
		jxj_cu_net_set_vsub_param,
		jxj_cu_net_check_vsub_param},

	{IPNC_CMD_VCAP_CFG, -1,/*设备vcap配置*/
		jxj_cu_net_get__param,		
		jxj_cu_net_set__param,
		jxj_cu_net_check__param},
	
	{IPNC_CMD_VIDEO_CFG, -1,/*设备video配置*/
		jxj_cu_net_get_video_param,		
		jxj_cu_net_set_video_param,
		jxj_cu_net_check_video_param},

	{IPNC_CMD_IMA_CFG, -1,/*设备ima配置*/
		jxj_cu_net_get_ima_param,		
		jxj_cu_net_set_ima_param,
		jxj_cu_net_check_ima_param},

	{IPNC_CMD_VMD_CFG, -1,/*设备md移动侦测配置*/
		jxj_cu_net_get_md_param,		
		jxj_cu_net_set_md_param,
		jxj_cu_net_check_md_param},

	{IPNC_CMD_DI_CFG, -1,/*设备di探头报警配置*/
		jxj_cu_net_get_di_param,		
		jxj_cu_net_set_di_param,
		jxj_cu_net_check_di_param},

	{IPNC_CMD_3A_CFG, -1,/*设备3a配置*/
		jxj_cu_net_get_3a_param,		
		jxj_cu_net_set_3a_param,
		jxj_cu_net_check_3a_param},

	{IPNC_CMD_IRCFMODE_CFG, -1,/*设备红外配置*/
		jxj_cu_net_get_ircfmode_param,		
		jxj_cu_net_set_ircfmode_param,
		jxj_cu_net_check_ircfmode_param},

	{IPNC_CMD_BALL_CFG, -1,/*设备高速球配置*/
		jxj_cu_net_get_ball_param,		
		jxj_cu_net_set_ball_param,
		jxj_cu_net_check_ball_param},
			
	{IPNC_CMD_PP_CFG, -1,/*设备预置点PP配置*/
		jxj_cu_net_get_pp_param,		
		jxj_cu_net_set_pp_param,
		jxj_cu_net_check_pp_param},
	{IPNC_CMD_TIME_CFG, -1,/*设备TIME配置*/
		jxj_cu_net_get_time_param,		
		jxj_cu_net_set_time_param,
		jxj_cu_net_check_time_param},
	{IPNC_CMD_JXJ_PLATFORM_CFG, -1,/*设备jxj platform配置*/
		jxj_cu_net_get_jxj_platform_param,		
		jxj_cu_net_set_jxj_platform_param,
		jxj_cu_net_check_jxj_platform_param},
		
	{IPNC_CMD_REC_CFG, -1,/*设备rec录像配置*/
		jxj_cu_net_get_rec_param,		
		jxj_cu_net_set_rec_param,
		jxj_cu_net_check_rec_param},

	{IPNC_CMD_OSD_LABEL_CFG, -1,/*设备OSD LABEL配置*/
		jxj_cu_net_get_osd_lable_param,		
		jxj_cu_net_set_osd_lable_param,
		jxj_cu_net_check_osd_lable_param},

	{IPNC_CMD_REBOOT, -1,
		NULL, 
		jxj_cu_net_set_reboot,
		NULL},
	{IPNC_CMD_HALT, -1,
		NULL, 
		jxj_cu_net_set_halt,
		NULL},
	{IPNC_CMD_DEFAULT, -1,
		NULL, 
		jxj_cu_net_set_default,
		NULL},
		
	//结尾
	{0,-1,NULL,NULL,NULL},
};

/*根据结构大小,查找响应结构操作函数集.
成功返回操作函数集句柄,
失败返回空.
*/
long jxj_cu_net_param_cmd(IPNC_CMD_TYPE_E paramCmd)
{
	int i = 0;
	//__DBUG("paramCmd: %x\n", paramCmd);
	for(i = 0; 
		(sCmdParam[i].paramCmd != 0) &&
			((sCmdParam[i].getParamProc != NULL) || (sCmdParam[i].setParamProc != NULL));
		i++)
	{
		//__DBUG("sCmdParam[%d].paramCmd: %x\n",i, sCmdParam[i].paramCmd);
		if(paramCmd == sCmdParam[i].paramCmd)
			return (long)&sCmdParam[i];
	}

	return (long)NULL;
}



