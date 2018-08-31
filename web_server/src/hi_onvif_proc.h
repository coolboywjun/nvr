#ifndef __HI_ONVIF_PROC_H__
#define __HI_ONVIF_PROC_H__

#include "soapStub.h"

#define MANUFACTURER_NAME		"CNVR-Company"
#define MODEL_NAME					"CNVR"


#define ONVIF_DEVICE_SERVICE                    "/onvif/device_service"
#define ONVIF_EVENTS_SERVICE                    "/onvif/events_service"
#define ONVIF_MEDIA_SERVICE                     "/onvif/media_service"
#define ONVIF_IMAGING_SERVICE                   "/onvif/imaging_service"
#define ONVIF_PTZ_SERVICE                       "/onvif/ptz_service"
#define ONVIF_ANALYTICS_SERVICE                 "/onvif/analytics_service"
#define ONVIF_DEVICEIO_SERVICE			"/onvif/deviceio_service"
#define ONVIF_RECORDING_CONTROL_SERVICE 	"/onvif/recordingcontrol_service"
#define ONVIF_RECORDING_SEARCH_SERVICE "/onvif/recordingsearch_service"
#define ONVIF_REPLAY_SERVICE 			"/onvif/replay_service"

#define ONVIF_MEDIA_MAIN_PROFILE_NAME			"MainProfileName"
#define ONVIF_MEDIA_MAIN_PROFILE_TOKEN			"MainProfileToken"
#define ONVIF_MEDIA_SUB_PROFILE_NAME				"IPNCSubProfileName"
#define ONVIF_MEDIA_SUB_PROFILE_TOKEN			"IPNCSubProfileToken"

#define ONVIF_MEDIA_VIDEO_MAIN_SOURCE_NAME		"VideoMainSourceName"
#define ONVIF_MEDIA_VIDEO_MAIN_SOURCE_TOKEN	"VideoMainSourceToken"
#define ONVIF_MEDIA_VIDEO_SUB_SOURCE_NAME		"VideoSubSourceName"
#define ONVIF_MEDIA_VIDEO_SUB_SOURCE_TOKEN	"VideoSubSourceToken"


#define ONVIF_MEDIA_VIDEO_MAIN_CFG_NAME		"VideoMainConfigureName"
#define ONVIF_MEDIA_VIDEO_MAIN_CFG_TOKEN	"VideoMainConfigureToken"
#define ONVIF_MEDIA_VIDEO_SUB_CFG_NAME		"VideoSubConfigureName"
#define ONVIF_MEDIA_VIDEO_SUB_CFG_TOKEN		"VideoSubConfigureToken"

#define ONVIF_MEDIA_VIDEO_MAIN_ENCODER_NAME		"VideoMainEncodeName"
#define ONVIF_MEDIA_VIDEO_MAIN_ENCODER_TOKEN	"VideoMainEncodeToken"
#define ONVIF_MEDIA_VIDEO_SUB_ENCODER_NAME		"VideoSubEncodeName"
#define ONVIF_MEDIA_VIDEO_SUB_ENCODER_TOKEN		"VideoSubEncodeToken"

#define ONVIF_MEDIA_AUDIO_SOURCE_NAME		"AudioSourceName"
#define ONVIF_MEDIA_AUDIO_SOURCE_TOKEN		"AudioSourceToken"

#define ONVIF_MEDIA_AUDIO_CFG_NAME		"AudioSourceConfigureName"
#define ONVIF_MEDIA_AUDIO_CFG_TOKEN		"AudioSourceConfigureToken"

#define ONVIF_MEDIA_AUDIO_ENCODER_NAME		"AudioSourceConfigureName"
#define ONVIF_MEDIA_AUDIO_ENCODER_TOKEN		"AudioSourceConfigureToken"

#define ONVIF_MEDIA_PTZ_NAME		"PTZName"
#define ONVIF_MEDIA_PTZ_TOKEN		"PTZToken"

#define ONVIF_RELAY_OUTPUT_NAME	"RelayOutputName"
#define ONVIF_RELAY_OUTPUT_TOKEN	"RelayOutputToken"

#define ONVIF_SCOPES_NUM 		9
#define ONVIF_SCOPES_STR_LEN 	128
#define ONVIF_NAME_LEN			128
#define ONVIF_TOKEN_LEN		128
#define ONVIF_ADDRESS_LEN		128
#define ONVIF_DEVICE_SERVICE_ADDR_SIZE	80

#define ONVIF_SCOPE_0 "onvif://www.onvif.org/type/Network_Video_Transmitter"
#define ONVIF_SCOPE_1 "onvif://www.onvif.org/location/sz/China"
#define ONVIF_SCOPE_2 "onvif://www.onvif.org/hardware/NVR_V1.2"
#define ONVIF_SCOPE_3 "onvif://www.onvif.org/name/NVR"
#define ONVIF_SCOPE_4 "onvif://www.onvif.org/Profile/Streaming"
#define ONVIF_SCOPE_5 "onvif://www.onvif.org/type/ptz"
#define ONVIF_SCOPE_6 "onvif://www.onvif.org/type/video_encoder"
#define ONVIF_SCOPE_7 "onvif://www.onvif.org/type/audio_encoder"
#define ONVIF_SCOPE_8 "onvif://www.onvif.org/type/video_analytics"

#define ONVIF_VIDEO_CHANNAL_NUM		4
#define ONVIF_PROFILE_NUM			(2*ONVIF_VIDEO_CHANNAL_NUM)
#define ONVIF_VIDEO_SOURCE_NUM		(1*ONVIF_VIDEO_CHANNAL_NUM)
#define ONVIF_VIDEO_SOURCE_CFG_NUM	(2*ONVIF_VIDEO_CHANNAL_NUM)
#define ONVIF_VIDEO_ENCODER_CFG_NUM	(2*ONVIF_VIDEO_CHANNAL_NUM)
#define ONVIF_AUDIO_SOURCE_NUM		(1*ONVIF_VIDEO_CHANNAL_NUM)
#define ONVIF_AUDIO_SOURCE_CFG_NUM	(1*ONVIF_VIDEO_CHANNAL_NUM)
#define ONVIF_AUDIO_ENCODER_CFG_NUM	(1*ONVIF_VIDEO_CHANNAL_NUM)
#define ONVIF_PTZ_CFG_NUM			(1*ONVIF_VIDEO_CHANNAL_NUM)

#define ONVIF_PTZ_NAME						"PtzName"
#define ONVIF_PTZ_TOKEN					"PtzToken"
#define ONVIF_PTZ_PRESET_NUM	255
#define ONVIF_PTZ_HOME_FIXED	0

#define ONVIF_IMAGING_MAX	255
#define ONVIF_IMAGING_MIN	0



/* WS-Discovery UDP Multicast Address & Port */
#define ONVIF_WSD_MC_ADDR               "239.255.255.250"

typedef enum _HI_ONVIF_NET_CARD_TYPE_S_
{
	HI_CARD_ETH0	= 0,
	HI_CARD_RA0		= 1,
}HI_ONVIF_NET_CARD_TYPE_S;

typedef struct _HI_ONVIF_SCOPE_S_
{
	int isFixed;
	char scope[ONVIF_SCOPES_STR_LEN];
}HI_ONVIF_SCOPE_S;

typedef struct _HI_ONVIF_VIDEO_SOURCE_S_
{
	char *token;	/* required attribute of type tt:ReferenceToken */
	float Framerate;	/* required element of type xsd:float */
	struct tt__VideoResolution Resolution;	/* required element of type tt:VideoResolution */
}HI_ONVIF_VIDEO_SOURCE_S;

typedef struct _HI_ONVIF_AUDIO_SOURCE_S_
{
	char *token;	/* required attribute of type tt:ReferenceToken */
	int Channels;	/* required element of type xsd:int */
}HI_ONVIF_AUDIO_SOURCE_S;

typedef struct _HI_ONVIF_PROFILE_S_
{
	char *Name;	/* required element of type tt:Name */
	char *token;	/* required attribute of type tt:ReferenceToken */
	int  fixed;			/* optional attribute of type xsd:boolean */
	char streamUri[ONVIF_ADDRESS_LEN];
	
	int videoSourceCfgIdx;
	int videoEncoderCfgIdx;
	int audioSourceCfgIdx;
	int audioEncoderCfgIdx;
	int ptzCfgIdx;
}HI_ONVIF_PROFILE_S;

typedef struct _HI_ONVIF_VIDEO_SOURCE_CFG_S_
{
	char *Name;	/* required element of type tt:Name */
	int UseCount;	/* required element of type xsd:int */
	char *token;	/* required attribute of type tt:ReferenceToken */
	char *SourceToken;	/* required element of type tt:ReferenceToken */
	struct tt__IntRectangle Bounds;	/* required element of type tt:IntRectangle */
}HI_ONVIF_VIDEO_SOURCE_CFG_S;

typedef struct _HI_ONVIF_AUDIO_SOURCE_CFG_S_
{
	char *Name;	/* required element of type tt:Name */
	int UseCount;	/* required element of type xsd:int */
	char *token;	/* required attribute of type tt:ReferenceToken */
	char *SourceToken;	/* required element of type tt:ReferenceToken */
}HI_ONVIF_AUDIO_SOURCE_CFG_S;

typedef struct _HI_ONVIF_VIDEO_ENCODER_CFG_S_
{
	char *Name;	/* required element of type tt:Name */
	int UseCount;	/* required element of type xsd:int */
	char *token;	/* required attribute of type tt:ReferenceToken */
	enum tt__VideoEncoding Encoding;	/* required element of type tt:VideoEncoding */
	struct tt__VideoResolution Resolution;	/* required element of type tt:VideoResolution */
	float Quality;	/* required element of type xsd:float */
	char* SessionTimeout;	/* external */
	int Gop;
	int FrameRate;
	int Bitrate;
}HI_ONVIF_VIDEO_ENCODER_CFG_S;

typedef struct _HI_ONVIF_AUDIO_ENCODER_CFG_S_
{
	char *Name;	/* required element of type tt:Name */
	int UseCount;	/* required element of type xsd:int */
	char *token;	/* required attribute of type tt:ReferenceToken */
	enum tt__AudioEncoding Encoding;	/* required element of type tt:AudioEncoding */
	int Bitrate;	/* required element of type xsd:int */
	int SampleRate;	/* required element of type xsd:int */
	char* SessionTimeout;	/* external */
}HI_ONVIF_AUDIO_ENCODER_CFG_S;

typedef struct _HI_ONVIF_PTZ_PRESET_CFG_S_
{
	char Name[ONVIF_TOKEN_LEN];	/* required element of type tt:Name */
	char token[ONVIF_TOKEN_LEN];	/* required attribute of type tt:ReferenceToken */
}HI_ONVIF_PTZ_PRESET_CFG_S;

typedef struct _HI_ONVIF_PTZ_CFG_S_
{	
	HI_ONVIF_PTZ_PRESET_CFG_S presetCfg[ONVIF_PTZ_PRESET_NUM]; //peset[0] used for home
}HI_ONVIF_PTZ_CFG_S;

typedef struct _HI_ONVIF_MEDIA_CFG_S_
{
	struct tt__VideoResolution h264Options[8];
	struct tt__VideoResolution VideoResolutionconfig;
	struct tt__VideoRateControl VideoEncoderRateControl;
	
}HI_ONVIF_MEDIA_CFG_S;

typedef struct _HI_ONVIF_INFO_S_
{
	int HttpPort;
	HI_ONVIF_PROFILE_S OnvifProfile[ONVIF_PROFILE_NUM];

	HI_ONVIF_VIDEO_SOURCE_S OnvifVideoSource[ONVIF_VIDEO_SOURCE_NUM];
	HI_ONVIF_AUDIO_SOURCE_S OnvifAudioSource[ONVIF_AUDIO_SOURCE_NUM];

	HI_ONVIF_VIDEO_SOURCE_CFG_S OnvifVideoSourceCfg[ONVIF_VIDEO_SOURCE_CFG_NUM];	/* optional element of type tt:VideoSourceConfiguration */
	HI_ONVIF_AUDIO_SOURCE_CFG_S OnvifAudioSourceCfg[ONVIF_AUDIO_SOURCE_CFG_NUM];	/* optional element of type tt:AudioSourceConfiguration */
	HI_ONVIF_VIDEO_ENCODER_CFG_S OnvifVideoEncoderCfg[ONVIF_VIDEO_ENCODER_CFG_NUM];	/* optional element of type tt:VideoEncoderConfiguration */
	HI_ONVIF_AUDIO_ENCODER_CFG_S OnvifAudioEncoderCfg[ONVIF_AUDIO_ENCODER_CFG_NUM];	/* optional element of type tt:AudioEncoderConfiguration */
	HI_ONVIF_PTZ_CFG_S OnvifPTZCfg[ONVIF_PTZ_CFG_NUM];



	//struct tt__Profile Profile[ONVIF_PROFILE_NUM];
	
	//struct tt__VideoSource VideoSource[ONVIF_VIDEO_SOURCE_NUM];
	//struct tt__AudioSource AudioSource[ONVIF_AUDIO_SOURCE_NUM];
	
	//struct tt__VideoSourceConfiguration VideoSourceConfiguration[ONVIF_VIDEO_SOURCE_CFG_NUM];	/* optional element of type tt:VideoSourceConfiguration */
	//struct tt__AudioSourceConfiguration AudioSourceConfiguration[ONVIF_AUDIO_SOURCE_CFG_NUM];	/* optional element of type tt:AudioSourceConfiguration */
	//struct tt__VideoEncoderConfiguration VideoEncoderConfiguration[ONVIF_VIDEO_ENCODER_CFG_NUM];	/* optional element of type tt:VideoEncoderConfiguration */
	//struct tt__AudioEncoderConfiguration AudioEncoderConfiguration[ONVIF_AUDIO_ENCODER_CFG_NUM];	/* optional element of type tt:AudioEncoderConfiguration */

	
	/* Service Addresses */	
	char device_xaddr [ONVIF_DEVICE_SERVICE_ADDR_SIZE];
	char deviceio_xaddr[ONVIF_DEVICE_SERVICE_ADDR_SIZE];
	char events_xaddr [ONVIF_DEVICE_SERVICE_ADDR_SIZE];
	char media_xaddr [ONVIF_DEVICE_SERVICE_ADDR_SIZE];
	char imaging_xaddr [ONVIF_DEVICE_SERVICE_ADDR_SIZE];
	char ptz_xaddr [ONVIF_DEVICE_SERVICE_ADDR_SIZE];
	char analytics_xaddr [ONVIF_DEVICE_SERVICE_ADDR_SIZE];
	char recordingcontrol_xaddr[ONVIF_DEVICE_SERVICE_ADDR_SIZE];
	char recordingsearch_xaddr[ONVIF_DEVICE_SERVICE_ADDR_SIZE];
	char replay_xaddr[ONVIF_DEVICE_SERVICE_ADDR_SIZE];

	HI_ONVIF_SCOPE_S type_scope[ONVIF_SCOPES_NUM];
	HI_ONVIF_SCOPE_S name_scope[ONVIF_SCOPES_NUM];
	HI_ONVIF_SCOPE_S location_scope[ONVIF_SCOPES_NUM];
	HI_ONVIF_SCOPE_S hardware_scope[ONVIF_SCOPES_NUM];
	HI_ONVIF_MEDIA_CFG_S MediaConfig;
	
}HI_ONVIF_INFO_S;

extern HI_ONVIF_INFO_S gOnvifInfo;

int onvif_fheader(struct soap *soap);

char * onvif_get_device_xaddr(int nIpAddr);

char * onvif_get_analytics_xaddr(int nIpAddr);

char * onvif_get_events_xaddr(int nIpAddr);

char * onvif_get_media_xaddr(int nIpAddr);

char * onvif_get_imaging_xaddr(int nIpAddr);

char * onvif_get_ptz_xaddr(int nIpAddr);

char *onvif_get_deviceio_xaddr(int nIpAddr);

char *onvif_get_recordingcontrol_xaddr(int nIpAddr);

char *onvif_get_recordingsearch_xaddr(int nIpAddr);

char *onvif_get_replay_xaddr(int nIpAddr);


unsigned int onvif_get_ipaddr(struct soap* soap);

unsigned long onvif_ip_n2a(unsigned long ip, char *ourIp, int len);

int onvif_get_mac(int nIpAddr, char macaddr[]);

char *onvif_get_hwid(int nIpAddr);

int onvif_get_scopes(char *str, int buflen);

int onvif_probe_match_scopes(char *item);

int onvif_get_profile_from_token(int nIpAddr, char *Token, HI_ONVIF_PROFILE_S *pOnvifProfile);
int onvif_get_video_source_from_token(char *Token, HI_ONVIF_VIDEO_SOURCE_S *pVideoSource);
int onvif_get_audio_source_from_token(char *Token, HI_ONVIF_AUDIO_SOURCE_S *pAudioSource);
int onvif_get_video_source_cfg_from_token(char *Token, HI_ONVIF_VIDEO_SOURCE_CFG_S *pVideoSourceCfg);
int onvif_get_video_encoder_cfg_from_token(char *Token, HI_ONVIF_VIDEO_ENCODER_CFG_S *pVideoEncoderCfg);
int onvif_get_audio_source_cfg_from_token(char *Token, HI_ONVIF_AUDIO_SOURCE_CFG_S *pAudioSourceCfg);
int onvif_get_audio_encoder_cfg_from_token(char *Token, HI_ONVIF_AUDIO_ENCODER_CFG_S *pAudioEncoderCfg);

int onvif_get_profile(int idx, int nIpAddr, HI_ONVIF_PROFILE_S *pOnvifProfile);
int onvif_get_video_source(int idx, HI_ONVIF_VIDEO_SOURCE_S *pVideoSource);
int onvif_get_audio_source(int idx, HI_ONVIF_AUDIO_SOURCE_S *pAudioSource);
int onvif_get_video_source_cfg(int idx, HI_ONVIF_VIDEO_SOURCE_CFG_S *pVideoSourceCfg);
int onvif_get_video_encoder_cfg(int idx, HI_ONVIF_VIDEO_ENCODER_CFG_S *pVideoEncoderCfg);
int onvif_get_audio_source_cfg(int idx, HI_ONVIF_AUDIO_SOURCE_CFG_S *pAudioSourceCfg);
int onvif_get_audio_encoder_cfg(int idx, HI_ONVIF_AUDIO_ENCODER_CFG_S *pAudioEncoderCfg);
int onvif_get_ptz_cfg(int idx, HI_ONVIF_PTZ_CFG_S *pPtzCfg);
int onvif_set_ptz_cfg(int idx, HI_ONVIF_PTZ_CFG_S *pPtzCfg);
int onvif_get_streamUri(char* uri, int streamType);

int onvif_server_init(int nHttpPort);

int onvif_check_security(struct soap *soap);

#endif

