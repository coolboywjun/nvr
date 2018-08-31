#include <stdio.h>
#include <string.h>
#include <time.h>
#include "onvif_api.h"
#include "onvif_ipnc_param.h"
#include "onvif_ipnc_logon.h"


static int onvif_net_get__param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set__param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_check__param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

/*net通用结构体函数************************************************************/
static int onvif_net_get_net_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_net_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{	
	return 0;
}

static int onvif_net_check_net_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_NET_CFG_S))
		return -1;
	
	return 0;
}
/*dev通用结构体函数************************************************************/
static int onvif_net_get_dev_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_dev_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_dev_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_CFG_S))
		return -1;
	
	return 0;
}
/*pppoe通用结构体函数************************************************************/
static int onvif_net_get_pppoe_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_pppoe_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{	
	return 0;
}

static int onvif_net_check_pppoe_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_PPPOE_CFG_S))
		return -1;
	
	return 0;
}

/*Ima通用结构体函数************************************************************/
static int onvif_net_get_ima_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{	
	int ret;
	IPNC_DEV_IMA_CFG_S *pCfg = (IPNC_DEV_IMA_CFG_S *)pData;
	char *VideoSoruceToken = NULL;	
	onvif_image_options_t options;
	onvif_image_settings_t settings;
	int rangecontrast = 0;
	int brightnessRange = 0;
	int colorSaturationRange = 0;
	int sharpnessRange = 0;

	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL)	
		return -1;	
	
	VideoSoruceToken = onvif_get_video_source_token(&pLogonInfo->profile, 0);
	if (VideoSoruceToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	//printf("ip:%s port:%s token = %s\r\n", pLogonInfo->ip , pLogonInfo->port, VideoSoruceToken);
	ret = nvr_onvif_get_image_options(pLogonInfo->ip , pLogonInfo->port, VideoSoruceToken ,&options);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	ret = nvr_onvif_get_image_settings(pLogonInfo->ip , pLogonInfo->port, VideoSoruceToken ,&settings);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	rangecontrast = options.contrast.Max - options.contrast.Min;
	brightnessRange = options.brightness.Max - options.brightness.Min;
	colorSaturationRange = options.colorSaturation.Max - options.colorSaturation.Min;
	sharpnessRange = options.sharpness.Max - options.sharpness.Min;
	if (rangecontrast <= 0 || rangecontrast > 255)
		rangecontrast = 255;
	if (brightnessRange <= 0 || brightnessRange > 255)
		brightnessRange = 255;	
	if (colorSaturationRange <= 0 || colorSaturationRange > 255)
		colorSaturationRange = 255;	
	if (sharpnessRange <= 0 || sharpnessRange > 255)
		sharpnessRange = 255;
	
	pCfg->struContrast.u8Value 	 = settings.contrast*255/rangecontrast;
	pCfg->struBrightness.u8Value = settings.brightness*255/brightnessRange;
	pCfg->struSaturation.u8Value = settings.colorsaturation*255/colorSaturationRange;
	pCfg->struSharpness.u8Value  = settings.sharpness*255/sharpnessRange;

	pCfg->struGamma.u8Default 	   = 128;
	pCfg->struBrightness.u8Default = 128;
	pCfg->struContrast.u8Default 	= 128;
	pCfg->struSaturation.u8Default 	= 128;
	pCfg->struSharpness.u8Default 	= 128;
	pCfg->struBlue.u8Default 	= 128;
	pCfg->struRed.u8Default 	= 128;
	
	return sizeof(IPNC_DEV_IMA_CFG_S);
}
static int onvif_net_set_ima_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{	
	int ret;
	IPNC_DEV_IMA_CFG_S *pCfg = (IPNC_DEV_IMA_CFG_S *)pData;
	char *VideoSoruceToken = NULL;	
	onvif_image_settings_t settings;

	//printf("@@@@@@@@@@@@onvif set ima\r\n");	
	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL)	
		return -1;	
	
	VideoSoruceToken = onvif_get_video_source_token(&pLogonInfo->profile, 0);
	if (VideoSoruceToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	//改变ima部分值，其他部分保持不变
	ret = nvr_onvif_get_image_settings(pLogonInfo->ip , pLogonInfo->port, VideoSoruceToken ,&settings);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	settings.brightness = pCfg->struBrightness.u8Value;
	settings.colorsaturation = pCfg->struSaturation.u8Value;
	settings.contrast = pCfg->struContrast.u8Value;
	settings.sharpness = pCfg->struSharpness.u8Value;
	ret = nvr_onvif_set_image_settings(pLogonInfo->ip , pLogonInfo->port, VideoSoruceToken ,&settings);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	return 0;	
}

static int onvif_net_check_ima_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	//printf("data size = %d, pData = 0x%0x\r\n", dataSize, pData);
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_IMA_CFG_S))
		return -1;
	
	//printf("@@@@@@@@@@@@onvif check ima\r\n");
	return 0;
}

/*Vmain通用结构体函数************************************************************/
static int onvif_net_get_vmain_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	char *profileToken;
	profile_data_t data;
	int ret = 0; 	
	IPNC_DEV_VENC_CFG_S *pCfg = (IPNC_DEV_VENC_CFG_S *)pData;
	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL)	
		return -1;	

	//printf("@@@@@@@@@onvif_net_get_vmain_param\r\n");
	profileToken = onvif_get_profile_token(&pLogonInfo->profile, 0);
	if (profileToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	//printf("ip:%s port:%s token = %s\r\n", pLogonInfo->ip , pLogonInfo->port, profileToken);
	ret = nvr_onvif_get_profile(pLogonInfo->ip , pLogonInfo->port, profileToken ,&data);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	pCfg->u16Height = data.VideoEncodeCfg.Resolution.Height;
	pCfg->u16Width= data.VideoEncodeCfg.Resolution.Width;
	pCfg->u32BitrateRate = data.VideoEncodeCfg.Bitrate;
	pCfg->u8FlowType = 0;
	pCfg->u8FrameRate = data.VideoEncodeCfg.FrameRate;
	pCfg->u8Gop = data.VideoEncodeCfg.Gop;
	pCfg->u8PicQuilty = data.VideoEncodeCfg.Quality;
	if (data.VideoEncodeCfg.Encoding == tt__VideoEncoding__JPEG)
		pCfg->u8VideoEncType = 2;
	else if (data.VideoEncodeCfg.Encoding == tt__VideoEncoding__MPEG4)
		pCfg->u8VideoEncType = 1;
	else if (data.VideoEncodeCfg.Encoding == tt__VideoEncoding__H264)
		pCfg->u8VideoEncType = 0;
	
	return sizeof(IPNC_DEV_VENC_CFG_S);
}
static int onvif_net_set_vmain_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	int ret;	
	video_encode_cfg_t vencCfg;
	IPNC_DEV_VENC_CFG_S *pCfg = (IPNC_DEV_VENC_CFG_S *)pData;
	char *vencToken = NULL, *vencName = NULL;	

	//printf("@@@@@@@@@@@@onvif_net_set_vmain_param\r\n");	
	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL)	
		return -1;	
	
	vencToken = onvif_get_video_encode_token(&pLogonInfo->profile, 0);
	if (vencToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	vencName = onvif_get_video_encode_name(&pLogonInfo->profile, 0);
	if (vencName == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	vencCfg.Bitrate = pCfg->u32BitrateRate;
	if (pCfg->u8VideoEncType == 0)
		vencCfg.Encoding = tt__VideoEncoding__H264;
	else if (pCfg->u8VideoEncType == 1)
		vencCfg.Encoding = tt__VideoEncoding__MPEG4;
	else if (pCfg->u8VideoEncType == 2)
		vencCfg.Encoding = tt__VideoEncoding__JPEG;
	vencCfg.FrameRate = pCfg->u8FrameRate;
	vencCfg.Gop = pCfg->u8Gop;
	strcpy(vencCfg.Name, vencName);
	strcpy(vencCfg.token, vencToken);
	vencCfg.Profile = tt__H264Profile__High;
	vencCfg.Quality = pCfg->u8PicQuilty;
	vencCfg.Resolution.Height = pCfg->u16Height;
	vencCfg.Resolution.Width = pCfg->u16Width;
	vencCfg.UseCount = 2;

	ret = onvif_set_video_encoder_configurations(pLogonInfo->ip , pLogonInfo->port, &vencCfg);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	return 0;
}
static int onvif_net_check_vmain_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_VENC_CFG_S))
		return -1;
	
	return 0;
}

/*vsub通用结构体函数************************************************************/
static int onvif_net_get_vsub_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	char *profileToken;
	profile_data_t data;
	int ret = 0; 	
	IPNC_DEV_VENC_CFG_S *pCfg = (IPNC_DEV_VENC_CFG_S *)pData;
	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL)	
		return -1;	

	//printf("@@@@@@@@@onvif_net_get_vsub_param\r\n");
	profileToken = onvif_get_profile_token(&pLogonInfo->profile, 1);
	if (profileToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	//printf("ip:%s port:%s token = %s\r\n", pLogonInfo->ip , pLogonInfo->port, profileToken);
	ret = nvr_onvif_get_profile(pLogonInfo->ip , pLogonInfo->port, profileToken ,&data);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	pCfg->u16Height = data.VideoEncodeCfg.Resolution.Height;
	pCfg->u16Width= data.VideoEncodeCfg.Resolution.Width;
	pCfg->u32BitrateRate = data.VideoEncodeCfg.Bitrate;
	pCfg->u8FlowType = 0;
	pCfg->u8FrameRate = data.VideoEncodeCfg.FrameRate;
	pCfg->u8Gop = data.VideoEncodeCfg.Gop;
	pCfg->u8PicQuilty = 1;
	pCfg->u8VideoEncType = 0;
	
	return sizeof(IPNC_DEV_VENC_CFG_S);
}
static int onvif_net_set_vsub_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	int ret;	
	video_encode_cfg_t vencCfg;
	IPNC_DEV_VENC_CFG_S *pCfg = (IPNC_DEV_VENC_CFG_S *)pData;
	char *vencToken = NULL, *vencName = NULL;	

	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL)	
		return -1;	
	
	vencToken = onvif_get_video_encode_token(&pLogonInfo->profile, 1);
	if (vencToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	vencName = onvif_get_video_encode_name(&pLogonInfo->profile, 1);
	if (vencName == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	printf("vsub pCfg->u8Gop = %d\r\n", pCfg->u8Gop);

	vencCfg.Bitrate = pCfg->u32BitrateRate;
	if (pCfg->u8VideoEncType == 0)
		vencCfg.Encoding = tt__VideoEncoding__H264;
	else if (pCfg->u8VideoEncType == 1)
		vencCfg.Encoding = tt__VideoEncoding__MPEG4;
	else if (pCfg->u8VideoEncType == 2)
		vencCfg.Encoding = tt__VideoEncoding__JPEG;
	vencCfg.FrameRate = pCfg->u8FrameRate;
	vencCfg.Gop = pCfg->u8Gop;
	strcpy(vencCfg.Name, vencName);
	strcpy(vencCfg.token, vencToken);
	vencCfg.Profile = tt__H264Profile__High;
	vencCfg.Quality = pCfg->u8PicQuilty;
	vencCfg.Resolution.Height = pCfg->u16Height;
	vencCfg.Resolution.Width = pCfg->u16Width;
	vencCfg.UseCount = 2;

	ret = onvif_set_video_encoder_configurations(pLogonInfo->ip , pLogonInfo->port, &vencCfg);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	return 0;
}
static int onvif_net_check_vsub_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_VENC_CFG_S))
		return -1;
	
	return 0;
}

/*vmask通用结构体函数************************************************************/
static int onvif_net_get_vmask_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_vmask_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_vmask_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_VMASK_CFG_S))
		return -1;
	
	return 0;
}


/*ntp通用结构体函数************************************************************/

static int onvif_net_get_ntp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0; 
}
static int onvif_net_set_ntp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_check_ntp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_NTP_CFG_S))
		return -1;
	
	return 0;
}
/*time通用结构体函数************************************************************/
static int onvif_net_get_time_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_time_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	time_t setTime;
	struct tm stm;
	struct tm *ptm;
	IPNC_TIME_S *pCfg = (IPNC_TIME_S *)pData;
	
	//printf("onvif_net_get_video_param nPort = %d\r\n", nPort);
	
	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL)	
		return -1;
	stm.tm_sec = pCfg->u8Second;
	stm.tm_min = pCfg->u8Minute;
	stm.tm_hour = pCfg->u8Hour;
	stm.tm_mday = pCfg->u8Day;
	stm.tm_mon = pCfg->u8Month - 1;
	stm.tm_year = pCfg->u8Year;
	setTime = mktime(&stm);
	if(setTime < 0)
		return -1;
	setTime -= 8 * 3600;
	ptm = localtime_r(&setTime, &stm);
	if(nvr_onvif_set_time_settings(pLogonInfo->ip , pLogonInfo->port, 8, 
									ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
									ptm->tm_hour, ptm->tm_min,  ptm->tm_sec) < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	return 0;
}

static int onvif_net_check_time_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_TIME_S))
		return -1;
	
	return 0;
}

/*jxj_platform通用结构体函数************************************************************/
static int onvif_net_get_jxj_platform_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_jxj_platform_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_jxj_platform_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_JXJ_PLATFORM_CFG_S))
		return -1;
	
	return 0;
}

/*rec通用结构体函数************************************************************/
static int onvif_net_get_rec_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_rec_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_rec_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_REC_STRC_S))
		return -1;
	
	return 0;
}

/*user通用结构体函数************************************************************/
static int onvif_net_get_user_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_user_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_user_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_USER_CFG_S))
		return -1;
	
	return 0;
}


static int onvif_net_get_comm_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_comm_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_comm_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_COMM_CFG_S))
		return -1;
	
	return 0;
}
/*ptz通用结构体函数************************************************************/
static int onvif_net_get_ptz_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_ptz_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_ptz_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_PTZ_CFG_S))
		return -1;
	
	return 0;
}

/*ftp通用结构体函数************************************************************/
static int onvif_net_get_ftp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_ftp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_ftp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_FTP_CFG_S))
		return -1;
	
	return 0;
}
/*smtp通用结构体函数************************************************************/
static int onvif_net_get_smtp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_smtp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_smtp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_SMTP_CFG_S))
		return -1;
	
	return 0;
}
/*upnp通用结构体函数************************************************************/
static int onvif_net_get_upnp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_upnp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_upnp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_UPNP_CFG_S))
		return -1;
	
	return 0;
}

/*ddns通用结构体函数************************************************************/
static int onvif_net_get_ddns_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_ddns_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_ddns_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DDNS_CFG_S))
		return -1;
	
	return 0;
}
/*osd通用结构体函数************************************************************/
static int onvif_net_get_osd_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_osd_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_osd_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_OSD_CFG_S))
		return -1;
	
	return 0;
}
/*audio通用结构体函数************************************************************/
static int onvif_net_get_audio_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_set_audio_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_audio_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_AUDIO_CFG_S))
		return -1;
	
	return 0;
}
/*video通用结构体函数************************************************************/
static int onvif_net_get_video_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	int ret = 0;
	profile_data_t  profile0, profile1;
	onvif_image_settings_t settings;
	onvif_image_options_t options;
	char *VideoSoruceToken = NULL;	
	char *profileToken = NULL;
	int rangecontrast = 0, brightnessRange = 0,colorSaturationRange = 0, sharpnessRange = 0;
	IPNC_DEV_VIDEO_CFG_S *pCfg = (IPNC_DEV_VIDEO_CFG_S *)pData;

	//printf("onvif_net_get_video_param nPort = %d\r\n", nPort);
	
	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL)	
		return -1;	
	
	VideoSoruceToken = onvif_get_video_source_token(&pLogonInfo->profile, 0);
	if (VideoSoruceToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	ret = nvr_onvif_get_image_options(pLogonInfo->ip , pLogonInfo->port, VideoSoruceToken ,&options);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	ret = nvr_onvif_get_image_settings(pLogonInfo->ip , pLogonInfo->port, VideoSoruceToken ,&settings);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	profileToken = onvif_get_profile_token(&pLogonInfo->profile, 0);
	if (profileToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	ret = nvr_onvif_get_profile(pLogonInfo->ip , pLogonInfo->port, profileToken ,&profile0);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	profileToken = onvif_get_profile_token(&pLogonInfo->profile, 1);
	if (profileToken != NULL)
	{
		ret = nvr_onvif_get_profile(pLogonInfo->ip , pLogonInfo->port, profileToken ,&profile1);
		if (ret < 0)
		{
			memset(&profile1, 0, sizeof(profile1));
			printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
			//return -1;
		}
	}

	if (profile0.VideoEncodeCfg.Profile == tt__H264Profile__Baseline)
		pCfg->eEncodeLevel = 2;
	else if (profile0.VideoEncodeCfg.Profile == tt__H264Profile__Main)
		pCfg->eEncodeLevel = 1;
	else if (profile0.VideoEncodeCfg.Profile == tt__H264Profile__High)
		pCfg->eEncodeLevel = 0;
	pCfg->u8PalStand = 0;

	rangecontrast = options.contrast.Max - options.contrast.Min;
	brightnessRange = options.brightness.Max - options.brightness.Min;
	colorSaturationRange = options.colorSaturation.Max - options.colorSaturation.Min;
	sharpnessRange = options.sharpness.Max - options.sharpness.Min;
	if (rangecontrast <= 0 || rangecontrast > 255)
		rangecontrast = 255;
	if (brightnessRange <= 0 || brightnessRange > 255)
		brightnessRange = 255;	
	if (colorSaturationRange <= 0 || colorSaturationRange > 255)
		colorSaturationRange = 255;	
	if (sharpnessRange <= 0 || sharpnessRange > 255)
		sharpnessRange = 255;

	pCfg->struIma.struContrast.u8Value = settings.contrast*255/rangecontrast;
	pCfg->struIma.struBrightness.u8Value = settings.brightness*255/brightnessRange;
	pCfg->struIma.struSaturation.u8Value = settings.colorsaturation*255/colorSaturationRange;
	pCfg->struIma.struSharpness.u8Value  = settings.sharpness*255/sharpnessRange;

	pCfg->struIma.struGamma.u8Default 	   = 128;
	pCfg->struIma.struBrightness.u8Default = 128;
	pCfg->struIma.struContrast.u8Default 	= 128;
	pCfg->struIma.struSaturation.u8Default 	= 128;
	pCfg->struIma.struSharpness.u8Default 	= 128;
	pCfg->struIma.struBlue.u8Default 	= 128;
	pCfg->struIma.struRed.u8Default 	= 128;

	pCfg->struMainVenc.u16Height = profile0.VideoEncodeCfg.Resolution.Height;
	pCfg->struMainVenc.u16Width= profile0.VideoEncodeCfg.Resolution.Width;
	pCfg->struMainVenc.u32BitrateRate = profile0.VideoEncodeCfg.Bitrate;
	pCfg->struMainVenc.u8FlowType = 0;
	pCfg->struMainVenc.u8FrameRate = profile0.VideoEncodeCfg.FrameRate;
	pCfg->struMainVenc.u8Gop = profile0.VideoEncodeCfg.Gop;
	pCfg->struMainVenc.u8PicQuilty = 1;
	pCfg->struMainVenc.u8VideoEncType = 0;

	pCfg->struSubVenc.u16Height = profile1.VideoEncodeCfg.Resolution.Height;
	pCfg->struSubVenc.u16Width= profile1.VideoEncodeCfg.Resolution.Width;
	pCfg->struSubVenc.u32BitrateRate = profile1.VideoEncodeCfg.Bitrate;
	pCfg->struSubVenc.u8FlowType = 0;
	pCfg->struSubVenc.u8FrameRate = profile1.VideoEncodeCfg.FrameRate;
	pCfg->struSubVenc.u8Gop = profile1.VideoEncodeCfg.Gop;
	pCfg->struSubVenc.u8PicQuilty = 1;
	pCfg->struSubVenc.u8VideoEncType = 0;

	return sizeof(IPNC_DEV_VIDEO_CFG_S);

}
static int onvif_net_set_video_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{		
	int ret = 0;
	profile_data_t	profile0, profile1;
	char *VideoSoruceToken = NULL;	
	char *profileToken = NULL;
	IPNC_DEV_VIDEO_CFG_S *pCfg = (IPNC_DEV_VIDEO_CFG_S *)pData;
	
	printf("onvif_net_set_video_param nPort = %d\r\n", nPort);
	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL) 
		return -1;	
	
	VideoSoruceToken = onvif_get_video_source_token(&pLogonInfo->profile, 0);
	if (VideoSoruceToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	profileToken = onvif_get_profile_token(&pLogonInfo->profile, 0);
	if (profileToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	ret = nvr_onvif_get_profile(pLogonInfo->ip , pLogonInfo->port, profileToken ,&profile0);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	profileToken = onvif_get_profile_token(&pLogonInfo->profile, 1);
	if (profileToken != NULL)
	{
		ret = nvr_onvif_get_profile(pLogonInfo->ip , pLogonInfo->port, profileToken ,&profile1);
		if (ret < 0)
		{
			memset(&profile1, 0, sizeof(profile1));
			printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
			//return -1;
		}
	}

	profile0.VideoEncodeCfg.Bitrate = pCfg->struMainVenc.u32BitrateRate;
	profile0.VideoEncodeCfg.Encoding = tt__VideoEncoding__H264;
	profile0.VideoEncodeCfg.FrameRate = pCfg->struMainVenc.u32BitrateRate;
	profile0.VideoEncodeCfg.Gop = pCfg->struMainVenc.u8Gop;
	if (pCfg->eEncodeLevel == 0)
		profile0.VideoEncodeCfg.Profile = tt__H264Profile__High;
	else if (pCfg->eEncodeLevel == 1)
		profile0.VideoEncodeCfg.Profile = tt__H264Profile__Main;
	else if (pCfg->eEncodeLevel == 2)
		profile0.VideoEncodeCfg.Profile = tt__H264Profile__Baseline;	
	profile0.VideoEncodeCfg.Quality = pCfg->struMainVenc.u8PicQuilty;
	profile0.VideoEncodeCfg.Resolution.Height = pCfg->struMainVenc.u16Height;
	profile0.VideoEncodeCfg.Resolution.Width = pCfg->struMainVenc.u16Width;

	ret = onvif_set_video_encoder_configurations(pLogonInfo->ip , pLogonInfo->port, &profile0.VideoEncodeCfg);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	profile1.VideoEncodeCfg.Bitrate = pCfg->struMainVenc.u32BitrateRate;
	profile1.VideoEncodeCfg.Encoding = tt__VideoEncoding__H264;
	profile1.VideoEncodeCfg.FrameRate = pCfg->struMainVenc.u32BitrateRate;
	profile1.VideoEncodeCfg.Gop = pCfg->struMainVenc.u8Gop;
	if (pCfg->eEncodeLevel == 0)
		profile1.VideoEncodeCfg.Profile = tt__H264Profile__High;
	else if (pCfg->eEncodeLevel == 1)
		profile1.VideoEncodeCfg.Profile = tt__H264Profile__Main;
	else if (pCfg->eEncodeLevel == 2)
		profile1.VideoEncodeCfg.Profile = tt__H264Profile__Baseline;	
	profile1.VideoEncodeCfg.Quality = pCfg->struMainVenc.u8PicQuilty;
	profile1.VideoEncodeCfg.Resolution.Height = pCfg->struMainVenc.u16Height;
	profile1.VideoEncodeCfg.Resolution.Width = pCfg->struMainVenc.u16Width;

	ret = onvif_set_video_encoder_configurations(pLogonInfo->ip , pLogonInfo->port, &profile1.VideoEncodeCfg);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}


	return 0;
}

static int onvif_net_check_video_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_VIDEO_CFG_S))
		return -1;
	
	return 0;
}

/*md通用结构体函数************************************************************/
static int onvif_net_get_md_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_md_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_md_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_MD_CFG_S))
		return -1;
	
	return 0;
}

/*di通用结构体函数************************************************************/
static int onvif_net_get_di_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_di_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_di_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_DI_CFG_S))
		return -1;
	
	return 0;
}

/*3A通用结构体函数************************************************************/
static int onvif_net_get_3a_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	//IPNC_DEV_3A_CFG_S *pCfg = NULL;
	return 0;
}
static int onvif_net_set_3a_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_3a_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_3A_CFG_S))
		return -1;
	
	return 0;
}

/*ir通用结构体函数************************************************************/
static int onvif_net_get_ircfmode_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	int ret;
	IPNC_DEV_IRCFMODE_CFG_S *pCfg = (IPNC_DEV_IRCFMODE_CFG_S *)pData;
	char *VideoSoruceToken = NULL;	
	onvif_image_settings_t settings;
	
	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL)	
		return -1;	
	
	VideoSoruceToken = onvif_get_video_source_token(&pLogonInfo->profile, 0);
	if (VideoSoruceToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	ret = nvr_onvif_get_image_settings(pLogonInfo->ip , pLogonInfo->port, VideoSoruceToken ,&settings);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	if (settings.IrCutFilter == IrCutFilterMode__ON)
		pCfg->u8Mode = 2;
	else if (settings.IrCutFilter == IrCutFilterMode__OFF)
		pCfg->u8Mode = 1;
	else if (settings.IrCutFilter == IrCutFilterMode__AUTO)
		pCfg->u8Mode = 0;

	return sizeof(IPNC_DEV_IRCFMODE_CFG_S);
}
static int onvif_net_set_ircfmode_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	int ret;
	IPNC_DEV_IRCFMODE_CFG_S *pCfg = (IPNC_DEV_IRCFMODE_CFG_S *)pData;
	char *VideoSoruceToken = NULL;	
	onvif_image_settings_t settings;

	onvif_logonInfo_t *pLogonInfo = (onvif_logonInfo_t *)logonHandle;
	if (pLogonInfo == NULL)	
		return -1;	
	VideoSoruceToken = onvif_get_video_source_token(&pLogonInfo->profile, 0);
	if (VideoSoruceToken == NULL)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	//改变ima部分值，其他部分保持不变
	ret = nvr_onvif_get_image_settings(pLogonInfo->ip , pLogonInfo->port, VideoSoruceToken ,&settings);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}

	if (pCfg->u8Mode == 0)
		settings.IrCutFilter = IrCutFilterMode__AUTO;
	else if (pCfg->u8Mode == 1)	
		settings.IrCutFilter = IrCutFilterMode__OFF;
	else if (pCfg->u8Mode == 2)
		settings.IrCutFilter = IrCutFilterMode__ON;

	ret = nvr_onvif_set_image_settings(pLogonInfo->ip , pLogonInfo->port, VideoSoruceToken ,&settings);
	if (ret < 0)
	{
		printf("file:%s func:%s line:%d\r\n", __FILE__, __func__, __LINE__);
		return -1;
	}
	
	return 0;	
}

static int onvif_net_check_ircfmode_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_IRCFMODE_CFG_S))
		return -1;
	
	return 0;
}

/*pp通用结构体函数************************************************************/
static int onvif_net_get_pp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_pp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_pp_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_PP_SET_CFG_S))
		return -1;
	
	return 0;
}
/*ball通用结构体函数************************************************************/
static int onvif_net_get_ball_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}
static int onvif_net_set_ball_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_ball_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_BALL_RUN_PARAM_S))
		return -1;
	
	return 0;
}

/*osd_lable通用结构体函数************************************************************/
static int onvif_net_get_osd_lable_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_set_osd_lable_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	return 0;
}

static int onvif_net_check_osd_lable_param(long logonHandle, int nPort, void *pData, uint32_t dataSize)
{
	if(NULL == pData)
		return -1;
		
	if(dataSize != sizeof(IPNC_DEV_OSD_LABEL_CFG_S))
		return -1;
	
	return 0;
}


//结构体操作函数集
static ONVIF_NET_CMD_PARAM_S sCmdParam[] =
{  
	{IPNC_CMD_DEV_CFG, -1,	/*设备信息配置*/
		onvif_net_get_dev_param,		
		onvif_net_set_dev_param,
		onvif_net_check_dev_param},
		
	{IPNC_CMD_NET_CFG, -1,	/*设备网络配置*/
		onvif_net_get_net_param,		
		onvif_net_set_net_param,
		onvif_net_check_net_param},

	{IPNC_CMD_FTP_CFG, -1,	/*设备ftp配置*/
		onvif_net_get_ftp_param,		
		onvif_net_set_ftp_param,
		onvif_net_check_ftp_param},

	{IPNC_CMD_EMAIL_CFG, -1,/*设备smtp配置*/
		onvif_net_get_smtp_param,		
		onvif_net_set_smtp_param,
		onvif_net_check_smtp_param},

	{IPNC_CMD_NTP_CFG, -1,/*设备ntp配置*/
		onvif_net_get_ntp_param,		
		onvif_net_set_ntp_param,
		onvif_net_check_ntp_param},

	{IPNC_CMD_UPNP_CFG, -1,/*设备upnp配置*/
		onvif_net_get_upnp_param,		
		onvif_net_set_upnp_param,
		onvif_net_check_upnp_param},

	{IPNC_CMD_DDNS_CFG, -1,/*设备ddns配置*/
		onvif_net_get_ddns_param,		
		onvif_net_set_ddns_param,
		onvif_net_check_ddns_param},

	{IPNC_CMD_COMM_CFG, -1,/*设备comm配置*/
		onvif_net_get_comm_param,		
		onvif_net_set_comm_param,
		onvif_net_check_comm_param},

	{IPNC_CMD_PPPOE_CFG, -1,/*设备pppoe配置*/
		onvif_net_get_pppoe_param,		
		onvif_net_set_pppoe_param,
		onvif_net_check_pppoe_param},

	{IPNC_CMD_PTZ_CFG, -1,/*设备ptz配置*/
		onvif_net_get_ptz_param,		
		onvif_net_set_ptz_param,
		onvif_net_check_ptz_param},

	{IPNC_CMD_AUDIO_CFG, -1,/*设备audio配置*/
		onvif_net_get_audio_param,		
		onvif_net_set_audio_param,
		onvif_net_check_audio_param},

	{IPNC_CMD_USR_CFG, -1,/*设备user配置*/
		onvif_net_get_user_param,		
		onvif_net_set_user_param,
		onvif_net_check_user_param},

	{IPNC_CMD_OSD_CFG, -1,/*设备osd配置*/
		onvif_net_get_osd_param,		
		onvif_net_set_osd_param,
		onvif_net_check_osd_param},

	{IPNC_CMD_VMASK_CFG, -1,/*设备vmask配置*/
		onvif_net_get_vmask_param,		
		onvif_net_set_vmask_param,
		onvif_net_check_vmask_param},
		
	{IPNC_CMD_VMAIN_CFG, -1,/*设备vmain配置*/
		onvif_net_get_vmain_param,		
		onvif_net_set_vmain_param,
		onvif_net_check_vmain_param},

	{IPNC_CMD_VSUB_CFG, -1,/*设备vsub配置*/
		onvif_net_get_vsub_param,		
		onvif_net_set_vsub_param,
		onvif_net_check_vsub_param},

	{IPNC_CMD_VCAP_CFG, -1,/*设备vcap配置*/
		onvif_net_get__param,		
		onvif_net_set__param,
		onvif_net_check__param},
	
	{IPNC_CMD_VIDEO_CFG, -1,/*设备video配置*/
		onvif_net_get_video_param,		
		onvif_net_set_video_param,
		onvif_net_check_video_param},

	{IPNC_CMD_IMA_CFG, -1,/*设备ima配置*/
		onvif_net_get_ima_param,		
		onvif_net_set_ima_param,
		onvif_net_check_ima_param},

	{IPNC_CMD_VMD_CFG, -1,/*设备md移动侦测配置*/
		onvif_net_get_md_param,		
		onvif_net_set_md_param,
		onvif_net_check_md_param},

	{IPNC_CMD_DI_CFG, -1,/*设备di探头报警配置*/
		onvif_net_get_di_param,		
		onvif_net_set_di_param,
		onvif_net_check_di_param},

	{IPNC_CMD_3A_CFG, -1,/*设备3a配置*/
		onvif_net_get_3a_param,		
		onvif_net_set_3a_param,
		onvif_net_check_3a_param},

	{IPNC_CMD_IRCFMODE_CFG, -1,/*设备红外配置*/
		onvif_net_get_ircfmode_param,		
		onvif_net_set_ircfmode_param,
		onvif_net_check_ircfmode_param},

	{IPNC_CMD_BALL_CFG, -1,/*设备高速球配置*/
		onvif_net_get_ball_param,		
		onvif_net_set_ball_param,
		onvif_net_check_ball_param},
			
	{IPNC_CMD_PP_CFG, -1,/*设备预置点PP配置*/
		onvif_net_get_pp_param,		
		onvif_net_set_pp_param,
		onvif_net_check_pp_param},
	{IPNC_CMD_TIME_CFG, -1,/*设备TIME配置*/
		onvif_net_get_time_param,		
		onvif_net_set_time_param,
		onvif_net_check_time_param},
	{IPNC_CMD_JXJ_PLATFORM_CFG, -1,/*设备jxj platform配置*/
		onvif_net_get_jxj_platform_param,		
		onvif_net_set_jxj_platform_param,
		onvif_net_check_jxj_platform_param},
		
	{IPNC_CMD_REC_CFG, -1,/*设备rec录像配置*/
		onvif_net_get_rec_param,		
		onvif_net_set_rec_param,
		onvif_net_check_rec_param},

	{IPNC_CMD_OSD_LABEL_CFG, -1,/*设备OSD LABEL配置*/
		onvif_net_get_osd_lable_param,		
		onvif_net_set_osd_lable_param,
		onvif_net_check_osd_lable_param},
	
	//结尾
	{0,-1,NULL,NULL,NULL},
};

/*根据结构大小,查找响应结构操作函数集.
成功返回操作函数集句柄,
失败返回空.
*/
long onvif_net_param_cmd(IPNC_CMD_TYPE_E paramCmd)
{
	int i = 0;
	for(i = 0; (sCmdParam[i].paramCmd		 != 0)    &&
				(sCmdParam[i].getParamProc   != NULL) &&
				(sCmdParam[i].setParamProc   != NULL) &&
				(sCmdParam[i].checkParamProc != NULL)   ;i++)
	{
		if(paramCmd == sCmdParam[i].paramCmd)
			return (long)&sCmdParam[i];
	}

	return (long)NULL;
}



