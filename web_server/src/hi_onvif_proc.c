#include "net_api.h"
#include "nvr_lib_interface.h"
#include "hi_onvif.h"
#include "hi_onvif_proc.h"
#include "hi_onvif_subscribe.h"
#include "openssl/sha.h"
#include "hi_character_encode.h"
#include "ipnc_lib.h"

HI_ONVIF_INFO_S gOnvifInfo;

/////////////////////////////////////////////////////
//FunctionName: onvif_check_security
//Description:  
//Input parameters: 
//Output parameters: 
//Returns value: 
//made by zhx.2014-1-22
//Modified Details:
//Modified by:
int onvif_check_security(struct soap *soap)
{
#if 0	
	struct _wsse__Security *wsse__Security;
	struct _wsse__UsernameToken *UsernameToken;
	HI_DEV_USER_CFG_S stUserCfg;
	char nonce[256];
	char out[256];
	int noncelen = 0;
	int i;
	
	if(soap->header == NULL)
		return SOAP_USER_ERROR;
		
	wsse__Security = soap->header->wsse__Security;
	if(wsse__Security == NULL)	return SOAP_USER_ERROR;	
	UsernameToken = wsse__Security->UsernameToken;
	if(UsernameToken == NULL) return SOAP_USER_ERROR;
	if(UsernameToken->Username == NULL || UsernameToken->Password == NULL) return SOAP_USER_ERROR;
	if(UsernameToken->Password->__item == NULL) return SOAP_USER_ERROR;
	if(UsernameToken->Nonce == NULL) return SOAP_USER_ERROR; 	
	if(UsernameToken->wsu__Created== NULL) return SOAP_USER_ERROR; 
	hi_base64_decode((unsigned char *)UsernameToken->Nonce, strlen(UsernameToken->Nonce), nonce, &noncelen);
	memcpy(nonce+noncelen, UsernameToken->wsu__Created, strlen(UsernameToken->wsu__Created));
	noncelen += strlen(UsernameToken->wsu__Created);	

	//nvr_logon_get_user_cfg(int port,NVR_DEV_USER_CFG_S * pCfg)
	
	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{
		nvr_logon_get_user_cfg(i, &stUserCfg);
		
		if(strncmp(stUserCfg.szUsrName, UsernameToken->Username, sizeof(stUserCfg.szUsrName)) == 0)
		{
			break;
		}
	}

	if(i == NVR_MAX_USR_NUM) return SOAP_USER_ERROR;

	memcpy(nonce+noncelen, stUserCfg.szPsw, strlen(stUserCfg.szPsw));
	noncelen += strlen(stUserCfg.szPsw);
	
	{
		SHA_CTX c;
		SHA1_Init(&c);
		SHA1_Update(&c, nonce, noncelen);
		SHA1_Final((unsigned char *)out, &c);
		memset(nonce, 0, sizeof(nonce));	
		hi_base64_encode((unsigned char *)out, 20, nonce, &noncelen);

		if(strncmp(nonce, UsernameToken->Password->__item, noncelen) != 0) return SOAP_USER_ERROR;
	}
#endif
	return SOAP_OK;	
}

int onvif_fheader(struct soap *soap)
{
#if 0
	int i;
	char passwd[64];
	struct _wsse__Security *wsse__Security;
	struct _wsse__UsernameToken *UsernameToken;
	HI_DEV_USER_CFG_S stUserCfg;

	if(soap->header == NULL)
		return SOAP_OK;

	wsse__Security = soap->header->wsse__Security;
	if(wsse__Security == NULL)
		return SOAP_OK;

	UsernameToken = wsse__Security->UsernameToken;
	if(UsernameToken == NULL)
		return SOAP_OK;
	if(UsernameToken->Password != NULL && UsernameToken->Password->__item != NULL)
	{
		onvif_decode_base64(UsernameToken->Password->__item, passwd, sizeof(passwd));
	}
	else
	{
		passwd[0] = '\0';
	}
	
	for(i = 0; i < NVR_MAX_USR_NUM; i++)
	{
		nvr_logon_get_user_cfg(i, &stUserCfg);
		if(strncmp(stUserCfg.szUsrName, UsernameToken->Username, sizeof(stUserCfg.szUsrName)) == 0 &&
			strncmp(stUserCfg.szPsw, passwd, sizeof(stUserCfg.szPsw)) == 0)
		{
			return SOAP_OK;
		}
	}
	return SOAP_FAULT;
#endif
	return SOAP_OK;
}

char * onvif_get_device_xaddr(int nIpAddr)
{
	char ipAddr[64];
	net_ip_n2a(nIpAddr, ipAddr, 64);
	if(DEFAULT_ONVIF_SERVER_PORT == 80)
	{
		sprintf (gOnvifInfo.device_xaddr, "http://%s%s",
				ipAddr,  ONVIF_DEVICE_SERVICE);
	}else
	{
		sprintf (gOnvifInfo.device_xaddr, "http://%s:%d%s",
				ipAddr, DEFAULT_ONVIF_SERVER_PORT, ONVIF_DEVICE_SERVICE);
	}
	return gOnvifInfo.device_xaddr;
}

char *onvif_get_deviceio_xaddr(int nIpAddr)
{
	char ipAddr[64];
	net_ip_n2a(nIpAddr, ipAddr, 64);
	if(DEFAULT_ONVIF_SERVER_PORT == 80)
	{
		sprintf (gOnvifInfo.deviceio_xaddr, "http://%s%s",
				ipAddr,  ONVIF_DEVICEIO_SERVICE);
	}else
	{
		sprintf (gOnvifInfo.deviceio_xaddr, "http://%s:%d%s",
				ipAddr, DEFAULT_ONVIF_SERVER_PORT, ONVIF_DEVICEIO_SERVICE);
	}
	return gOnvifInfo.deviceio_xaddr;
}

char *onvif_get_recordingcontrol_xaddr(int nIpAddr)
{
			
	char ipAddr[64];
	net_ip_n2a(nIpAddr, ipAddr, 64);
	if(DEFAULT_ONVIF_SERVER_PORT == 80)
	{
		sprintf (gOnvifInfo.recordingcontrol_xaddr, "http://%s%s",
				ipAddr,  ONVIF_RECORDING_CONTROL_SERVICE);
	}else
	{
		sprintf (gOnvifInfo.recordingcontrol_xaddr, "http://%s:%d%s",
				ipAddr, DEFAULT_ONVIF_SERVER_PORT, ONVIF_RECORDING_CONTROL_SERVICE);
	}
	return gOnvifInfo.recordingcontrol_xaddr;
}

char *onvif_get_recordingsearch_xaddr(int nIpAddr)
{
			
	char ipAddr[64];
	net_ip_n2a(nIpAddr, ipAddr, 64);
	if(DEFAULT_ONVIF_SERVER_PORT == 80)
	{
		sprintf (gOnvifInfo.recordingsearch_xaddr, "http://%s%s",
				ipAddr,  ONVIF_RECORDING_SEARCH_SERVICE);
	}else
	{
		sprintf (gOnvifInfo.recordingsearch_xaddr, "http://%s:%d%s",
				ipAddr, DEFAULT_ONVIF_SERVER_PORT, ONVIF_RECORDING_SEARCH_SERVICE);
	}
	return gOnvifInfo.recordingsearch_xaddr;
}

char *onvif_get_replay_xaddr(int nIpAddr)
{
			
	char ipAddr[64];
	net_ip_n2a(nIpAddr, ipAddr, 64);
	if(DEFAULT_ONVIF_SERVER_PORT == 80)
	{
		sprintf (gOnvifInfo.replay_xaddr, "http://%s%s",
				ipAddr,  ONVIF_REPLAY_SERVICE);
	}else
	{
		sprintf (gOnvifInfo.replay_xaddr, "http://%s:%d%s",
				ipAddr, DEFAULT_ONVIF_SERVER_PORT, ONVIF_REPLAY_SERVICE);
	}
	return gOnvifInfo.replay_xaddr;
}



char * onvif_get_analytics_xaddr(int nIpAddr)
{
	char ipAddr[64];
	net_ip_n2a(nIpAddr, ipAddr, 64);
	if(DEFAULT_ONVIF_SERVER_PORT == 80)
	{
		sprintf (gOnvifInfo.analytics_xaddr, "http://%s%s",
				ipAddr,  ONVIF_ANALYTICS_SERVICE);
	}else
	{
		sprintf (gOnvifInfo.analytics_xaddr, "http://%s:%d%s",
				ipAddr, DEFAULT_ONVIF_SERVER_PORT, ONVIF_ANALYTICS_SERVICE);
	}
	return gOnvifInfo.analytics_xaddr;
}

char * onvif_get_events_xaddr(int nIpAddr)
{
	char ipAddr[64];
	net_ip_n2a(nIpAddr, ipAddr, 64);
	if(DEFAULT_ONVIF_SERVER_PORT == 80)
	{
		sprintf (gOnvifInfo.events_xaddr, "http://%s%s",
					ipAddr,  ONVIF_EVENTS_SERVICE);
	}else
	{
		sprintf (gOnvifInfo.events_xaddr, "http://%s:%d%s",
						ipAddr, DEFAULT_ONVIF_SERVER_PORT, ONVIF_EVENTS_SERVICE);		
	}
	return gOnvifInfo.events_xaddr;
}

char * onvif_get_media_xaddr(int nIpAddr)
{
	char ipAddr[64];
	net_ip_n2a(nIpAddr, ipAddr, 64);
	if(DEFAULT_ONVIF_SERVER_PORT == 80)
	{
		sprintf (gOnvifInfo.media_xaddr, "http://%s%s",
					ipAddr,  ONVIF_MEDIA_SERVICE);
	}else
	{
		sprintf (gOnvifInfo.media_xaddr, "http://%s:%d%s",
					ipAddr, DEFAULT_ONVIF_SERVER_PORT, ONVIF_MEDIA_SERVICE);		
	}
	return gOnvifInfo.media_xaddr;
}

char * onvif_get_imaging_xaddr(int nIpAddr)
{
	char ipAddr[64];
	net_ip_n2a(nIpAddr, ipAddr, 64);
	if(DEFAULT_ONVIF_SERVER_PORT == 80)
	{
		sprintf (gOnvifInfo.imaging_xaddr, "http://%s%s",
					ipAddr, ONVIF_IMAGING_SERVICE);
	}else
	{
		sprintf (gOnvifInfo.imaging_xaddr, "http://%s:%d%s",
					ipAddr, DEFAULT_ONVIF_SERVER_PORT, ONVIF_IMAGING_SERVICE);
	}
	return gOnvifInfo.imaging_xaddr;
}

char * onvif_get_ptz_xaddr(int nIpAddr)
{
	char ipAddr[64];
	net_ip_n2a(nIpAddr, ipAddr, 64);
	if(DEFAULT_ONVIF_SERVER_PORT == 80)
	{	
		sprintf (gOnvifInfo.ptz_xaddr, "http://%s%s",
					ipAddr, ONVIF_PTZ_SERVICE);
	}else
	{
		sprintf (gOnvifInfo.ptz_xaddr, "http://%s:%d%s",
					ipAddr, DEFAULT_ONVIF_SERVER_PORT, ONVIF_PTZ_SERVICE);	
	}
	return gOnvifInfo.ptz_xaddr;
}

/*
int onvif_get_ipaddr(int netCardType, char *ipaddr)
{	
	NVR_DEV_NET_CFG_S stNetCfg;
	unsigned int u32Addr;
	
	nvr_logon_get_net_cfg(0, &stNetCfg);
	if(netCardType == HI_CARD_ETH0)
		u32Addr = stNetCfg.struEtherCfg[0].u32IpAddr;
	else(netCardType == HI_CARD_RA0)
		u32Addr = stNetCfg.struEtherCfg[1].u32IpAddr;
	
	sprintf(ipaddr, "%d.%d.%d.%d",
			(u32Addr >> 24) & 0xff,	
			(u32Addr >> 16) & 0xff,	
			(u32Addr >> 8) & 0xff,	
			(u32Addr >> 0) & 0xff);
	return 0;
}
*/

unsigned int onvif_get_ipaddr(struct soap* soap)
{
	struct      sockaddr_in serv;
	socklen_t 	serv_len = sizeof(serv); 
	getsockname(soap->socket, (struct sockaddr *)&serv, &serv_len);
	return htonl(serv.sin_addr.s_addr);
}

int onvif_get_mac(int nIpAddr, char macaddr[])
{
	NVR_DEV_NET_CFG_S stNetCfg;

	nvr_logon_get_net_cfg(0, &stNetCfg);
	if(nIpAddr == stNetCfg.struEtherCfg[0].u32IpAddr)
		memcpy(macaddr, stNetCfg.struEtherCfg[0].u8MacAddr, 6);
	else if(nIpAddr == stNetCfg.struEtherCfg[1].u32IpAddr)
		memcpy(macaddr, stNetCfg.struEtherCfg[1].u8MacAddr, 6);
	
	return 0;
}

unsigned long onvif_ip_n2a(unsigned long ip, char *ourIp, int len)
{
	return net_ip_n2a(ip, ourIp, len);
}

char *onvif_get_hwid(int nIpAddr)
{
	static char hwid[128];
	char macaddr[6];
	
	onvif_get_mac(nIpAddr, macaddr);

	sprintf(hwid,"urn:uuid:1419d68a-1dd2-11b2-a105-%02X%02X%02X%02X%02X%02X",macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
		
	return hwid;
}

//////////////////////////////////////////////////
static int onvif_add_scope(HI_ONVIF_SCOPE_S *scopes, char *item)
{
	int i;
	for(i = 0; i< ONVIF_SCOPES_NUM; i++)
	{
		if(scopes[i].scope[0] == 0)
		{
			scopes[i].isFixed = 1;
			strcpy(scopes[i].scope, item);
			return 0;	
		}
	}
	
	return -1;
}

////////////////////////////////////////////////////
int onvif_get_scopes(char *str, int buflen)
{
	int i, len = 0;

	for(i= 0; i<ONVIF_SCOPES_NUM; i++)
	{	
		if(gOnvifInfo.type_scope[i].scope[0]) len += strlen(gOnvifInfo.type_scope[i].scope);
		if(gOnvifInfo.name_scope[i].scope[0]) len += strlen(gOnvifInfo.type_scope[i].scope);
		if(gOnvifInfo.location_scope[i].scope[0]) len += strlen(gOnvifInfo.type_scope[i].scope);
		if(gOnvifInfo.hardware_scope[i].scope[0]) len += strlen(gOnvifInfo.type_scope[i].scope);
	}
	
	if((len + i) > buflen) return -1;

	for(i= 0; i<ONVIF_SCOPES_NUM; i++)
	{	
		if(gOnvifInfo.type_scope[i].scope[0])
		{
		strcat(str, gOnvifInfo.type_scope[i].scope);
		strcat(str, " ");
		}
	}
	
	for(i= 0; i<ONVIF_SCOPES_NUM; i++)
	{	
		if(gOnvifInfo.location_scope[i].scope[0])
		{
		strcat(str, gOnvifInfo.location_scope[i].scope);
		strcat(str, " ");
		}
	}
	
	for(i= 0; i<ONVIF_SCOPES_NUM; i++)
	{	
		if(gOnvifInfo.hardware_scope[i].scope[0])
		{
		strcat(str, gOnvifInfo.hardware_scope[i].scope);
		strcat(str, " ");
		}
	}
	
	for(i= 0; i<ONVIF_SCOPES_NUM; i++)
	{	
		if(gOnvifInfo.name_scope[i].scope[0])
		{
		strcat(str, gOnvifInfo.name_scope[i].scope);
		strcat(str, " ");
		}
	}

	return 0;
}
/////////////////////////////////////////
int onvif_probe_match_scopes(char *item)
{
	
	char *pStrH, *pStrN;
	int len,TypeLen, LocatLen, hwLen, nameLen;
	int i;
	if(item == NULL)
	{
		printf("The item is null.\n");
		return 0;
	}
	printf("item: %s\n", item);
	if(item[0] == 0x0) return 0;
	printf("item: %s\n", item);
	TypeLen = strlen("onvif://www.onvif.org/type");
	LocatLen = strlen("onvif://www.onvif.org/location");
	hwLen = strlen("onvif://www.onvif.org/hardware");
	nameLen = strlen("onvif://www.onvif.org/name");
	
	pStrH = item;

	while(1)
	{
		pStrN = strchr(pStrH, ' ');
		
		if(pStrN == NULL) 
		{			
			len = strlen(pStrH);	
			
		}else
		{
			len = pStrN -pStrH;
		}
		
		if(!strncmp(pStrH, "onvif://www.onvif.org/type", TypeLen))
		{
			if(TypeLen == len) 
			{			
				return 0;
			}
			else
			{
				for(i = 0; i<ONVIF_SCOPES_NUM; i++)
				{
					if(!gOnvifInfo.type_scope[i].scope[0]) continue;
					if(!strncmp(pStrH, gOnvifInfo.type_scope[i].scope, len))
					{
						
						return 0;
					}
				}
			}		

		}else if(!strncmp(pStrH, "onvif://www.onvif.org/location", LocatLen))
		{
			if(TypeLen == len) 
			{
				
				return 0;
			}
			else
			{

				for(i = 0; i<ONVIF_SCOPES_NUM; i++)
				{
					if(!gOnvifInfo.location_scope[i].scope[0]) continue;
					if(!strncmp(pStrH, gOnvifInfo.location_scope[i].scope, len))
					{
						
						return 0;
					}
				}
			}
			
		}else if(!strncmp(pStrH, "onvif://www.onvif.org/hardware", hwLen))
		{
		
			if(TypeLen == len) 
			{
				
				return 0;
			}
			else
			{
				for(i = 0; i<ONVIF_SCOPES_NUM; i++)
				{
					if(!gOnvifInfo.hardware_scope[i].scope[0]) continue;
					if(!strncmp(pStrH, gOnvifInfo.hardware_scope[i].scope, len))
					{
						
						return 0;
					}
				}
			}

		}else if(!strncmp(pStrH, "onvif://www.onvif.org/name", nameLen))
		{
		
			if(TypeLen == len) 
			{				
				return 0;
			}
			else
			{
				for(i = 0; i<ONVIF_SCOPES_NUM; i++)
				{
					if(!gOnvifInfo.name_scope[i].scope[0]) continue;
					if(!strncmp(pStrH, gOnvifInfo.name_scope[i].scope, len))
					{						
						return 0;
					}
				}
			}
		}
		
		if(pStrN == NULL)
		{			
			break;
		}
		pStrH = pStrN+1;
	}

	return -1;
}

int onvif_get_profile_from_token(int nIpAddr, char *Token, HI_ONVIF_PROFILE_S *pOnvifProfile)
{
	int i;

	if(Token == NULL)
	{
		__E("The token is null.\n");
		return -1;
	}
	
	for(i = 0; i < ONVIF_PROFILE_NUM; i++)
	{
		if(strcmp(gOnvifInfo.OnvifProfile[i].token, Token) == 0)
		{
			return onvif_get_profile(i, nIpAddr, pOnvifProfile);
		}
	}
	return -1;
}

int onvif_get_video_source_from_token(char *Token, HI_ONVIF_VIDEO_SOURCE_S *pVideoSource)
{
	int i;

	if(Token == NULL)
	{
		__E("The token is null.\n");
		return -1;
	}

	for(i = 0; i < ONVIF_VIDEO_SOURCE_NUM; i++)
	{
		if(strcmp(gOnvifInfo.OnvifVideoSource[i].token, Token) == 0)
		{
			return onvif_get_video_source(i, pVideoSource);
		}
	}
	return -1;
}

int onvif_get_audio_source_from_token(char *Token, HI_ONVIF_AUDIO_SOURCE_S *pAudioSource)
{
	int i;

	if(Token == NULL)
	{
		__E("The token is null.\n");
		return -1;
	}

	for(i = 0; i < ONVIF_AUDIO_SOURCE_NUM; i++)
	{
		if(strcmp(gOnvifInfo.OnvifAudioSource[i].token, Token) == 0)
		{
			return onvif_get_audio_source(i, pAudioSource);
		}
	}
	return -1;
}

int onvif_get_video_source_cfg_from_token(char *Token, HI_ONVIF_VIDEO_SOURCE_CFG_S *pVideoSourceCfg)
{
	int i;

	if(Token == NULL)
	{
		__E("The token is null.\n");
		return -1;
	}

	for(i = 0; i < ONVIF_VIDEO_SOURCE_CFG_NUM; i++)
	{
		if(strcmp(gOnvifInfo.OnvifVideoSourceCfg[i].token, Token) == 0)
		{
			return onvif_get_video_source_cfg(i, pVideoSourceCfg);
		}
	}
	
	return onvif_get_video_source_cfg(0, pVideoSourceCfg);
}

int onvif_get_video_encoder_cfg_from_token(char *Token, HI_ONVIF_VIDEO_ENCODER_CFG_S *pVideoEncoderCfg)
{
	int i;

	if(Token == NULL)
	{
		__E("The token is null.\n");
		return -1;
	}

	for(i = 0; i < ONVIF_VIDEO_ENCODER_CFG_NUM; i++)
	{
		if(strcmp(gOnvifInfo.OnvifVideoEncoderCfg[i].token, Token) == 0)
		{
			return onvif_get_video_encoder_cfg(i, pVideoEncoderCfg);
		}
	}
	return -1;
}

int onvif_get_audio_source_cfg_from_token(char *Token, HI_ONVIF_AUDIO_SOURCE_CFG_S *pAudioSourceCfg)
{
	int i;

	if(Token == NULL)
	{
		__E("The token is null.\n");
		return -1;
	}

	for(i = 0; i < ONVIF_AUDIO_SOURCE_CFG_NUM; i++)
	{
		if(strcmp(gOnvifInfo.OnvifAudioSourceCfg[i].token, Token) == 0)
		{
			return onvif_get_audio_source_cfg(0, pAudioSourceCfg);
		}
	}
	return -1;
}

int onvif_get_audio_encoder_cfg_from_token(char *Token, HI_ONVIF_AUDIO_ENCODER_CFG_S *pAudioEncoderCfg)
{
	int i;

	if(Token == NULL)
	{
		__E("The token is null.\n");
		return -1;
	}

	for(i = 0; i < ONVIF_AUDIO_ENCODER_CFG_NUM; i++)
	{
		if(strcmp(gOnvifInfo.OnvifAudioEncoderCfg[i].token, Token) == 0)
		{
			return onvif_get_audio_encoder_cfg(0, pAudioEncoderCfg);
		}
	}
	return -1;
}

int onvif_get_profile(int idx, int nIpAddr, HI_ONVIF_PROFILE_S *pOnvifProfile)
{	
	int ch = idx>>1;
	int streamNo = idx%2;
	if(idx >= ONVIF_PROFILE_NUM)
		return -1;
	
	//rtsp://192.168.5.75:554/ch3_0.h264
	//rtsp://192.168.5.75:7554/dev=JXJ-NVR-12345678/media=0/channel=0&level=1
#if 1	
	snprintf(gOnvifInfo.OnvifProfile[idx].streamUri, 
			sizeof(gOnvifInfo.OnvifProfile[idx].streamUri),
			"rtsp://%d.%d.%d.%d:554/ch%d_%d.h264",
			(nIpAddr >> 24) & 0xff,
			(nIpAddr >> 16) & 0xff,
			(nIpAddr >> 8) & 0xff,
			(nIpAddr >> 0) & 0xff,
			ch,
			streamNo);
#else
	snprintf(gOnvifInfo.OnvifProfile[idx].streamUri, 
			sizeof(gOnvifInfo.OnvifProfile[idx].streamUri),
			"rtsp://%d.%d.%d.%d:7554/dev=JXJ-NVR-12345678/media=%d/channel=%d&level=%d",
			(nIpAddr >> 24) & 0xff,
			(nIpAddr >> 16) & 0xff,
			(nIpAddr >> 8) & 0xff,
			(nIpAddr >> 0) & 0xff,
			idx,
			0,1);

#endif
	memcpy(pOnvifProfile, &gOnvifInfo.OnvifProfile[idx], sizeof(HI_ONVIF_PROFILE_S));
	return 0;
}
int onvif_get_video_source(int idx, HI_ONVIF_VIDEO_SOURCE_S *pVideoSource)
{
	int ch = idx;
	IPNC_DEV_VENC_CFG_S stVencCfg;
	if(idx >= ONVIF_VIDEO_SOURCE_NUM)
		return -1;
	memset(&stVencCfg, 0, sizeof(IPNC_DEV_VENC_CFG_S));
	if(0 > nvr_get_ipnc_param(ch,IPNC_CMD_VMAIN_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S)))
	{
		memset(&stVencCfg,0, sizeof(IPNC_DEV_VENC_CFG_S));
	}
	gOnvifInfo.OnvifVideoSource[idx].Resolution.Width = stVencCfg.u16Width;
	gOnvifInfo.OnvifVideoSource[idx].Resolution.Height = stVencCfg.u16Height;
	gOnvifInfo.OnvifVideoSource[idx].Framerate = stVencCfg.u8FrameRate;
	memcpy(pVideoSource, &gOnvifInfo.OnvifVideoSource[idx], sizeof(HI_ONVIF_VIDEO_SOURCE_S));
	
	return 0;
}
int onvif_get_audio_source(int idx, HI_ONVIF_AUDIO_SOURCE_S *pAudioSource)
{	
	if(idx >= ONVIF_AUDIO_SOURCE_NUM)
		return -1;
	memcpy(pAudioSource, &gOnvifInfo.OnvifAudioSource[idx], sizeof(HI_ONVIF_AUDIO_SOURCE_S));
	return 0;
}
int onvif_get_video_source_cfg(int idx, HI_ONVIF_VIDEO_SOURCE_CFG_S *pVideoSourceCfg)
{
	if(idx >= ONVIF_VIDEO_SOURCE_CFG_NUM)
		return -1;
	memcpy(pVideoSourceCfg, &gOnvifInfo.OnvifVideoSourceCfg[idx], sizeof(HI_ONVIF_VIDEO_SOURCE_CFG_S));
	return 0;
}
int onvif_get_video_encoder_cfg(int idx, HI_ONVIF_VIDEO_ENCODER_CFG_S *pVideoEncoderCfg)
{
	int chn = idx>>1;
	int streamNo = idx%2;
	IPNC_DEV_VENC_CFG_S stVencCfg;

	if(idx >= ONVIF_VIDEO_ENCODER_CFG_NUM)
		return -1;
	
	if(streamNo == 0)
	{
		if(0 > nvr_get_ipnc_param(chn,IPNC_CMD_VMAIN_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S)))
		{
			memset(&stVencCfg,0, sizeof(IPNC_DEV_VENC_CFG_S));
		}
		gOnvifInfo.OnvifVideoEncoderCfg[idx].Quality= stVencCfg.u8PicQuilty;
		gOnvifInfo.OnvifVideoEncoderCfg[idx].Gop = stVencCfg.u8Gop;
		gOnvifInfo.OnvifVideoEncoderCfg[idx].FrameRate= stVencCfg.u8FrameRate;
		gOnvifInfo.OnvifVideoEncoderCfg[idx].Bitrate= stVencCfg.u32BitrateRate;
	}
	else
	{
		if(0 > nvr_get_ipnc_param(chn,IPNC_CMD_VSUB_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S)))
		{
			memset(&stVencCfg,0, sizeof(IPNC_DEV_VENC_CFG_S));
		}
		gOnvifInfo.OnvifVideoEncoderCfg[idx].Quality= stVencCfg.u8PicQuilty;
		gOnvifInfo.OnvifVideoEncoderCfg[idx].Gop = stVencCfg.u8Gop;
		gOnvifInfo.OnvifVideoEncoderCfg[idx].FrameRate= stVencCfg.u8FrameRate;
		gOnvifInfo.OnvifVideoEncoderCfg[idx].Bitrate= stVencCfg.u32BitrateRate;
	}
	memcpy(pVideoEncoderCfg, &gOnvifInfo.OnvifVideoEncoderCfg[idx], sizeof(HI_ONVIF_VIDEO_ENCODER_CFG_S));
	return 0;
}
int onvif_get_audio_source_cfg(int idx, HI_ONVIF_AUDIO_SOURCE_CFG_S *pAudioSourceCfg)
{
	if(idx >= ONVIF_AUDIO_SOURCE_CFG_NUM)
		return -1;
	memcpy(pAudioSourceCfg, &gOnvifInfo.OnvifAudioSourceCfg[idx], sizeof(HI_ONVIF_AUDIO_SOURCE_CFG_S));
	return 0;
}
int onvif_get_audio_encoder_cfg(int idx, HI_ONVIF_AUDIO_ENCODER_CFG_S *pAudioEncoderCfg)
{
	if(idx >= ONVIF_AUDIO_ENCODER_CFG_NUM)
		return -1;
	memcpy(pAudioEncoderCfg, &gOnvifInfo.OnvifAudioEncoderCfg[idx], sizeof(HI_ONVIF_AUDIO_ENCODER_CFG_S));
	return 0;
}
int onvif_get_ptz_cfg(int idx, HI_ONVIF_PTZ_CFG_S *pPtzCfg)
{
	if(idx >= ONVIF_PTZ_CFG_NUM)
		return -1;
	memcpy(pPtzCfg, &gOnvifInfo.OnvifPTZCfg[idx], sizeof(HI_ONVIF_PTZ_CFG_S));
	return 0;
}
int onvif_set_ptz_cfg(int idx, HI_ONVIF_PTZ_CFG_S *pPtzCfg)
{
	if(idx >= ONVIF_PTZ_CFG_NUM)
		return -1;
	memcpy(&gOnvifInfo.OnvifPTZCfg[idx], pPtzCfg, sizeof(HI_ONVIF_PTZ_CFG_S));
	return 0;
}

static int onvif_scope_init()
{
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_0);
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_4);
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_5);
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_6);
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_7);
	onvif_add_scope(gOnvifInfo.type_scope, ONVIF_SCOPE_8);
	onvif_add_scope(gOnvifInfo.location_scope, ONVIF_SCOPE_1);
	onvif_add_scope(gOnvifInfo.hardware_scope, ONVIF_SCOPE_2);
	onvif_add_scope(gOnvifInfo.name_scope, ONVIF_SCOPE_3);

	return 0;
}

static int onvif_server_address_init(int nHttpPort)
{
/*	char ipAddr[64];
	
	onvif_get_ipaddr(ipAddr);
	
	if (nHttpPort != 80)
	{
		sprintf (gOnvifInfo.device_xaddr, "http://%s:%d%s",
				ipAddr, nHttpPort, ONVIF_DEVICE_SERVICE);
		sprintf (gOnvifInfo.events_xaddr, "http://%s:%d%s",
				ipAddr, nHttpPort, ONVIF_EVENTS_SERVICE);
		sprintf (gOnvifInfo.media_xaddr, "http://%s:%d%s",
				ipAddr, nHttpPort, ONVIF_MEDIA_SERVICE);
		sprintf (gOnvifInfo.imaging_xaddr, "http://%s:%d%s",
				ipAddr, nHttpPort, ONVIF_IMAGING_SERVICE);
		sprintf (gOnvifInfo.ptz_xaddr, "http://%s:%d%s",
				ipAddr, nHttpPort, ONVIF_PTZ_SERVICE);
		sprintf (gOnvifInfo.analytics_xaddr, "http://%s:%d%s",
				ipAddr, nHttpPort, ONVIF_ANALYTICS_SERVICE);
	}
	else
	{	
		sprintf (gOnvifInfo.device_xaddr, "http://%s%s",
				ipAddr, ONVIF_DEVICE_SERVICE);
		sprintf (gOnvifInfo.events_xaddr, "http://%s%s",
				ipAddr, ONVIF_EVENTS_SERVICE);
		sprintf (gOnvifInfo.media_xaddr, "http://%s%s",
				ipAddr, ONVIF_MEDIA_SERVICE);
		sprintf (gOnvifInfo.imaging_xaddr, "http://%s%s",
				ipAddr, ONVIF_IMAGING_SERVICE);
		sprintf (gOnvifInfo.ptz_xaddr, "http://%s%s",
				ipAddr, ONVIF_PTZ_SERVICE);
		sprintf (gOnvifInfo.analytics_xaddr, "http://%s%s",
				ipAddr, ONVIF_ANALYTICS_SERVICE);
	}
*/
	return 0;
}

static int onvif_profile_init()
{
	// main profile initialize.
	int ch = 0;
	int streamNo = 0;	
	char *pName0 = NULL, *pName1 = NULL, *pToken0 = NULL, *pToken1 = NULL;
	for (ch = 0; ch < ONVIF_VIDEO_CHANNAL_NUM; ch++)
	{
		streamNo = 0;
		pName0 = (char *)malloc(ONVIF_NAME_LEN);
		if (pName0 == NULL) return -1;
		sprintf(pName0, "CH %d_%s", ch, ONVIF_MEDIA_MAIN_PROFILE_NAME);

		pToken0 = (char *)malloc(ONVIF_NAME_LEN);
		if (pToken0 == NULL) return -1;
		sprintf(pToken0, "CH %d_%s", ch, ONVIF_MEDIA_MAIN_PROFILE_TOKEN);
		gOnvifInfo.OnvifProfile[2*ch + streamNo].Name = pName0;//ONVIF_MEDIA_MAIN_PROFILE_NAME;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].token = pToken0;//ONVIF_MEDIA_MAIN_PROFILE_TOKEN;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].fixed = 0;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].videoSourceCfgIdx = 2*ch + streamNo;//0;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].videoEncoderCfgIdx = 2*ch + streamNo;//0;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].audioSourceCfgIdx = ch;//0;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].audioEncoderCfgIdx = ch;//0;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].ptzCfgIdx = ch;//0;
		
		// sub profile initialize.		
		streamNo = 1;
		pName1 = (char *)malloc(ONVIF_NAME_LEN);
		if (pName1 == NULL) return -1;
		sprintf(pName1, "CH %d_%s", ch, ONVIF_MEDIA_SUB_PROFILE_NAME);

		pToken1 = (char *)malloc(ONVIF_NAME_LEN);
		if (pToken1 == NULL) return -1;
		sprintf(pToken1, "CH %d_%s",ch,  ONVIF_MEDIA_SUB_PROFILE_TOKEN);
		gOnvifInfo.OnvifProfile[2*ch + streamNo].Name = pName1;//ONVIF_MEDIA_SUB_PROFILE_NAME;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].token = pToken1;//ONVIF_MEDIA_SUB_PROFILE_TOKEN;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].fixed = 0;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].videoSourceCfgIdx = 2*ch;//+ streamNo;//0;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].videoEncoderCfgIdx = 2*ch + streamNo;//1;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].audioSourceCfgIdx = ch;//0;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].audioEncoderCfgIdx = ch;//0;
		gOnvifInfo.OnvifProfile[2*ch + streamNo].ptzCfgIdx = ch;//0;
	}
	return 0;
}

static int onvif_video_source_init()
{	
	int streamNo = 0;
	int chn = 0;
	NVR_DEV_VENC_CFG_S stVencCfg;
	char *pToken0 = NULL;

	for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
	{		
		streamNo = 0;
		//nvr_get_ipnc_param(chn,IPNC_CMD_VMAIN_CFG, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S));
		nvr_logon_get_main_venc_cfg(chn, &stVencCfg);
		pToken0 = (char *)malloc(ONVIF_NAME_LEN);
		if (pToken0 == NULL) return -1;
		sprintf(pToken0, "CH %d_%s", chn, ONVIF_MEDIA_VIDEO_MAIN_SOURCE_TOKEN);
		gOnvifInfo.OnvifVideoSource[chn].token = pToken0;//ONVIF_MEDIA_VIDEO_MAIN_SOURCE_TOKEN;
		gOnvifInfo.OnvifVideoSource[chn].Framerate = stVencCfg.u8FrameRate;
		gOnvifInfo.OnvifVideoSource[chn].Resolution.Width = stVencCfg.u16Width;
		gOnvifInfo.OnvifVideoSource[chn].Resolution.Height = stVencCfg.u16Width;
	}
	
	return 0;
}

static int onvif_audio_source_init()
{	
	int chn = 0;
	char *pToken = NULL;
	for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
	{
		pToken = (char *)malloc(ONVIF_NAME_LEN);
		if (pToken == NULL) return -1;
		sprintf(pToken, "CH %d_%s", chn, ONVIF_MEDIA_AUDIO_SOURCE_TOKEN);
		
		gOnvifInfo.OnvifAudioSource[chn].token = pToken;//ONVIF_MEDIA_AUDIO_SOURCE_TOKEN;
		gOnvifInfo.OnvifAudioSource[chn].Channels = 1;
	}

	return 0;
}

static int onvif_video_source_cfg_init()
{
	int chn;	
	int streamNo = 0;
	NVR_DEV_VENC_CFG_S stVencCfg;
	
	char *pName0 = NULL, *pName1 = NULL, *pToken0 = NULL, *pToken1 = NULL;
	char *pSourceToken0 = NULL, *pSourceToken1 = NULL;

	for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
	{		
		streamNo = 0;
		nvr_logon_get_main_venc_cfg(chn, &stVencCfg);
		pName0 = (char *)malloc(ONVIF_NAME_LEN);
		if (pName0 == NULL) return -1;
		sprintf(pName0, "CH %d_%s", chn, ONVIF_MEDIA_VIDEO_MAIN_CFG_NAME);
		pToken0 = (char *)malloc(ONVIF_NAME_LEN);
		if (pToken0 == NULL) return -1;
		sprintf(pToken0, "CH %d_%s", chn, ONVIF_MEDIA_VIDEO_MAIN_CFG_TOKEN);
		pSourceToken0 =  (char *)malloc(ONVIF_NAME_LEN);
		if (pSourceToken0 == NULL) return -1;
		sprintf(pSourceToken0, "CH %d_%s",chn, ONVIF_MEDIA_VIDEO_MAIN_SOURCE_TOKEN);
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].Name = pName0;//ONVIF_MEDIA_VIDEO_MAIN_CFG_NAME;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].token = pToken0;//ONVIF_MEDIA_VIDEO_MAIN_CFG_TOKEN;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].UseCount = 1;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].SourceToken = pSourceToken0;//ONVIF_MEDIA_VIDEO_MAIN_SOURCE_TOKEN;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].Bounds.x = 0;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].Bounds.y = 0;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].Bounds.width = stVencCfg.u16Width;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].Bounds.height = stVencCfg.u16Height;

		
		streamNo = 1;
		nvr_logon_get_main_venc_cfg(chn, &stVencCfg);
		pName1 = (char *)malloc(ONVIF_NAME_LEN);
		if (pName1 == NULL) return -1;
		sprintf(pName1, "CH %d_%s", chn, ONVIF_MEDIA_VIDEO_SUB_CFG_NAME);
		pToken1 = (char *)malloc(ONVIF_NAME_LEN);
		if (pToken1 == NULL) return -1;
		sprintf(pToken1, "CH %d_%s",chn ,ONVIF_MEDIA_VIDEO_SUB_CFG_TOKEN);
		pSourceToken1 =  (char *)malloc(ONVIF_NAME_LEN);
		if (pSourceToken1 == NULL) return -1;
		sprintf(pSourceToken1, "CH %d_%s", chn, ONVIF_MEDIA_VIDEO_SUB_SOURCE_TOKEN);
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].Name = pName1;//ONVIF_MEDIA_VIDEO_SUB_CFG_NAME;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].token = pToken1;//ONVIF_MEDIA_VIDEO_SUB_CFG_TOKEN;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].UseCount = 1;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].SourceToken = pSourceToken1;//ONVIF_MEDIA_VIDEO_SUB_SOURCE_TOKEN;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].Bounds.x = 0;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].Bounds.y = 0;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].Bounds.width = stVencCfg.u16Width;
		gOnvifInfo.OnvifVideoSourceCfg[2*chn + streamNo].Bounds.height = stVencCfg.u16Height;
	}
	
	return 0;
}

static int onvif_video_encoder_cfg_init()
{
	int chn;	
	int streamNo = 0;
	NVR_DEV_VENC_CFG_S stVencCfg;
	
	char *pName0 = NULL, *pName1 = NULL, *pToken0 = NULL, *pToken1 = NULL;
	for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
	{		
		streamNo = 0;
		nvr_logon_get_main_venc_cfg(chn, &stVencCfg);
		pName0 = (char *)malloc(ONVIF_NAME_LEN);
		if (pName0 == NULL) return -1;
		sprintf(pName0, "CH %d_%s", chn, ONVIF_MEDIA_VIDEO_MAIN_ENCODER_NAME);

		pToken0 = (char *)malloc(ONVIF_NAME_LEN);
		if (pToken0 == NULL) return -1;
		sprintf(pToken0, "CH %d_%s", chn, ONVIF_MEDIA_VIDEO_MAIN_ENCODER_TOKEN);
		
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Name = pName0;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].token= pToken0;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].UseCount = 1;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Encoding = tt__VideoEncoding__H264;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Resolution.Width = stVencCfg.u16Width;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Resolution.Height= stVencCfg.u16Height;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Quality = stVencCfg.u8PicQuilty;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].SessionTimeout = "PT5S";
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Gop = stVencCfg.u8Gop;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].FrameRate= stVencCfg.u8FrameRate;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Bitrate= stVencCfg.u32BitrateRate;

		streamNo = 1;
		nvr_logon_get_main_venc_cfg(chn, &stVencCfg);
		pName1 = (char *)malloc(ONVIF_NAME_LEN);
		if (pName1 == NULL) return -1;
		sprintf(pName1, "CH %d_%s", chn, ONVIF_MEDIA_VIDEO_SUB_ENCODER_NAME);

		pToken1 = (char *)malloc(ONVIF_NAME_LEN);
		if (pToken1 == NULL) return -1;
		sprintf(pToken1, "CH %d_%s",chn, ONVIF_MEDIA_VIDEO_SUB_ENCODER_TOKEN);
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Name = pName1;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].token= pToken1;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].UseCount = 1;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Encoding = tt__VideoEncoding__H264;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Resolution.Width = stVencCfg.u16Width;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Resolution.Height= stVencCfg.u16Height;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Quality = stVencCfg.u8PicQuilty;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].SessionTimeout = "PT5S";
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Gop = stVencCfg.u8Gop;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].FrameRate= stVencCfg.u8FrameRate;
		gOnvifInfo.OnvifVideoEncoderCfg[2*chn + streamNo].Bitrate= stVencCfg.u32BitrateRate;
		
		
	}
	
	memset(&stVencCfg, 0, sizeof(stVencCfg));
	nvr_logon_get_main_venc_cfg(chn, &stVencCfg);
	gOnvifInfo.MediaConfig.h264Options[0].Height = stVencCfg.u16Height;
	gOnvifInfo.MediaConfig.h264Options[0].Width = stVencCfg.u16Width;
	gOnvifInfo.MediaConfig.VideoResolutionconfig = gOnvifInfo.MediaConfig.h264Options[0];
	gOnvifInfo.MediaConfig.VideoEncoderRateControl.BitrateLimit = stVencCfg.u32BitrateRate;
	gOnvifInfo.MediaConfig.VideoEncoderRateControl.FrameRateLimit = stVencCfg.u8FrameRate;
	gOnvifInfo.MediaConfig.VideoEncoderRateControl.EncodingInterval = stVencCfg.u8Gop;

	gOnvifInfo.MediaConfig.h264Options[1].Width = stVencCfg.u16Width;
	gOnvifInfo.MediaConfig.h264Options[1].Height = stVencCfg.u16Height;
	
	return 0;
}

static int onvif_audio_source_cfg_init()
{	
	int chn = 0;	
	char *pName = NULL, *pToken = NULL, *pSourceToken = NULL;
	for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
	{
		pName = (char *)malloc(ONVIF_NAME_LEN);
		if (pName == NULL) return -1;
		sprintf(pName, "CH %d_%s",chn, ONVIF_MEDIA_AUDIO_CFG_NAME);

		pToken = (char *)malloc(ONVIF_NAME_LEN);
		if (pToken == NULL) return -1;
		sprintf(pToken, "CH %d_%s",chn, ONVIF_MEDIA_AUDIO_CFG_TOKEN);

		pSourceToken = (char *)malloc(ONVIF_NAME_LEN);
		if (pSourceToken == NULL) return -1;
		sprintf(pSourceToken, "CH %d_%s",chn, ONVIF_MEDIA_AUDIO_SOURCE_TOKEN);

		gOnvifInfo.OnvifAudioSourceCfg[chn].Name = pName;
		gOnvifInfo.OnvifAudioSourceCfg[chn].token = pToken;
		gOnvifInfo.OnvifAudioSourceCfg[chn].UseCount = 2;
		gOnvifInfo.OnvifAudioSourceCfg[chn].SourceToken = pSourceToken;
	}

	return 0;
}

static int onvif_audio_encoder_cfg_init()
{
	int chn = 0;
	char *pName = NULL, *pToken = NULL;
	for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
	{
		pName = (char *)malloc(ONVIF_NAME_LEN);
		if (pName == NULL) return -1;
		sprintf(pName, "CH %d_%s", chn, ONVIF_MEDIA_AUDIO_ENCODER_NAME);
		gOnvifInfo.OnvifAudioEncoderCfg[chn].Name = pName;
		pToken = (char *)malloc(ONVIF_NAME_LEN);
		if (pToken == NULL) return -1;
		sprintf(pToken, "CH %d_%s", chn, ONVIF_MEDIA_AUDIO_ENCODER_TOKEN);
		gOnvifInfo.OnvifAudioEncoderCfg[chn].token = pToken;
		gOnvifInfo.OnvifAudioEncoderCfg[chn].UseCount = 1;
		gOnvifInfo.OnvifAudioEncoderCfg[chn].Encoding = tt__AudioEncoding__G711;
		gOnvifInfo.OnvifAudioEncoderCfg[chn].Bitrate = 16000;
		gOnvifInfo.OnvifAudioEncoderCfg[chn].SampleRate = 8000;
		gOnvifInfo.OnvifAudioEncoderCfg[chn].SessionTimeout = "PT5S";
	}

	return 0;
}

static int onvif_ptz_cfg_init()
{
	int i;	
	int chn = 0;
	
	for (chn = 0; chn < ONVIF_VIDEO_CHANNAL_NUM; chn++)
	{
		for(i = 0; i<ONVIF_PTZ_PRESET_NUM; i++)
		{
			memset(gOnvifInfo.OnvifPTZCfg[chn].presetCfg[i].Name, 0, sizeof(gOnvifInfo.OnvifPTZCfg[chn].presetCfg[i].Name));
			//sprintf(gOnvifInfo.OnvifPTZCfg[chn].presetCfg[i].Name, 	"%s_%d", ONVIF_PTZ_NAME, i);
			sprintf(gOnvifInfo.OnvifPTZCfg[chn].presetCfg[i].token, "%s_%d", ONVIF_PTZ_TOKEN, i);
		}
	}
	return 0;
}

int onvif_server_init(int nHttpPort)
{
	__FUN_BEGIN("\n");
	
	memset(&gOnvifInfo, 0, sizeof(gOnvifInfo));
	gOnvifInfo.HttpPort = nHttpPort;
	
	onvif_scope_init();
	onvif_server_address_init(nHttpPort);
	
	onvif_profile_init();
	onvif_video_source_init();
	onvif_audio_source_init();
	onvif_video_source_cfg_init();
	onvif_video_encoder_cfg_init();
	onvif_audio_source_cfg_init();
	onvif_audio_encoder_cfg_init();
	onvif_ptz_cfg_init();
	//tev_Init();
	
	__FUN_END("\n");

	return 0;
}

