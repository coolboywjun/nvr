#ifndef __ONVIF_API_H__
#define __ONVIF_API_H__
#include "linux_list.h"

enum
{
	EN_ONVIF_NVT,   //Network Video Transmitter
	EN_ONVIF_NVD,   //Network  Video Display
	EN_ONVIF_NVS,   //Network Video Storage
	EN_ONVIF_NVA    //Network Video Analytics
}ONVIF_NET_DEV_TYPE;


#define ONVIF_MAX_NAME_LEN	128
#define ONVIF_MAX_TOKEN_LEN 128
#define ONVIF_MAX_URL_LEN	128
#define ONVIF_MAX_PRESET_NUM 256

typedef struct _discover_info_t
{	
	char devType[ONVIF_MAX_NAME_LEN];
	char EPAddr[ONVIF_MAX_NAME_LEN];
	char devServiceAddr[ONVIF_MAX_NAME_LEN];
	char scopes[ONVIF_MAX_NAME_LEN];	
	char devIP[ONVIF_MAX_NAME_LEN];
	char port[8];
}discover_info_t;


typedef struct _device_capability_t
{
	int spportRTPMulticast;
	int supportRTP_TCP;
	int supportRTP_RTSP_TCP;
	int MaximumNumberOfProfiles;
}device_capability_t;


struct __Rectangle
{
	int x;		
	int y;		
	int width;	
	int height;
};

typedef struct _video_source_cfg_t_
{
	char Name[ONVIF_MAX_NAME_LEN];
	int   UseCount;							
	char token[ONVIF_MAX_TOKEN_LEN];		
	char SourceToken[ONVIF_MAX_NAME_LEN];	
	struct __Rectangle Bounds;				
}video_source_cfg_t;


struct tt__IntRange
{
	int Min;	/* required element of type xsd:int */
	int Max;	/* required element of type xsd:int */
};


struct tt__VideoResolution
{
	int Width;	
	int Height;
};
enum tt__VideoEncoding {tt__VideoEncoding__JPEG = 0, tt__VideoEncoding__MPEG4 = 1, tt__VideoEncoding__H264 = 2};
enum tt__H264Profile {tt__H264Profile__Baseline = 0, tt__H264Profile__Main = 1, tt__H264Profile__Extended = 2, tt__H264Profile__High = 3};
typedef struct _video_encode_cfg_t
{
	char 	Name[ONVIF_MAX_NAME_LEN];			
	int 	UseCount;							
	char 	token[ONVIF_MAX_TOKEN_LEN];			
	enum 	tt__VideoEncoding Encoding;	
	enum 	tt__H264Profile   Profile;
	struct 	tt__VideoResolution Resolution;		
	float 	Quality;							
	char 	SessionTimeout[ONVIF_MAX_NAME_LEN];
	int 	Gop;
	int 	FrameRate;
	int 	Bitrate;
}video_encode_cfg_t;


typedef struct _audio_source_cfg_t
{
	char 	Name[ONVIF_MAX_NAME_LEN];
	int		UseCount;
	char 	token[ONVIF_MAX_NAME_LEN];
	char 	SourceToken[ONVIF_MAX_NAME_LEN];
}audio_source_cfg_t;

enum tt__AudioEncoding {tt__AudioEncoding__G711 = 0, tt__AudioEncoding__G726 = 1, tt__AudioEncoding__AAC = 2};
typedef struct _audio_encode_cfg_t
{
	char Name[ONVIF_MAX_NAME_LEN];	
	int UseCount;
	char token[ONVIF_MAX_NAME_LEN];
	enum tt__AudioEncoding Encoding;
	int Bitrate;
	int SampleRate;
	char SessionTimeout[ONVIF_MAX_NAME_LEN];
}audio_encode_cfg_t;

typedef struct _ptz_presets_cfg_
{	
	char acPresetToken[64];
	char acPresetName[64];
}ptz_presets_cfg_t;

typedef struct _profile_data_t
{
	char  profileToken[ONVIF_MAX_NAME_LEN];
	char  profileName[ONVIF_MAX_NAME_LEN];
	video_source_cfg_t VideoSourceCfg;
	video_encode_cfg_t VideoEncodeCfg;
	audio_source_cfg_t AudioSourceCfg;
	audio_encode_cfg_t AudioEncodeCfg;	
	ptz_presets_cfg_t  PtzPresetCfg[ONVIF_MAX_PRESET_NUM];
}profile_data_t;

#if 1
typedef struct _onvif_profile_list_
{
	struct list_head list;
	profile_data_t data;
}onvif_profile_list_t;
#else
typedef struct _onvif_profile_list
{	
	profile_data_t data;
	struct _onvif_profile_list *next;
}
#endif

typedef struct _device_info_t
{
	char Manifacture[ONVIF_MAX_NAME_LEN];
	char Model[ONVIF_MAX_NAME_LEN];
	char FirmwareVersion[ONVIF_MAX_NAME_LEN];
	char SerialNum[ONVIF_MAX_NAME_LEN];
	char HardwareId[ONVIF_MAX_NAME_LEN];
}device_info_t;


typedef struct _network_interface_t
{
	int  nEnable;
	char ethName[ONVIF_MAX_NAME_LEN];
	char macAddr[18];
	int  nMTU;
	int  nSpeed;
	char IPaddr[ONVIF_MAX_NAME_LEN];
	int  dhcpEnable;
}network_interface_t;


typedef struct _scope_data_t
{
	char scopeDefalt[ONVIF_MAX_NAME_LEN];
	char scopeItem[ONVIF_MAX_URL_LEN];
}scope_data_t;

typedef struct _onvif_scopes_list_
{
	struct list_head list;
	scope_data_t data;
}onvif_scopes_list_t;

typedef struct _onvif_image_options
{
	char irCutFilterModes[32];  		//0 off 1 on	
	char backlightCompensation[32];  //0 off 1 on
	struct tt__IntRange brightness;
	struct tt__IntRange colorSaturation;
	struct tt__IntRange contrast;
	struct tt__IntRange sharpness;
}onvif_image_options_t;

typedef struct _onvif_venccfg_options
{	
	struct tt__IntRange QualityRange;
	struct tt__IntRange GovLengthRange;
	struct tt__IntRange FrameRateRange;
	struct tt__IntRange EncodingIntervalRange;
}onvif_venccfg_options_t;

#define RANGLE_SIZE(a) (a.Max - a.Min + 1)


enum BacklightCompensationMode {BacklightCompensationMode__OFF = 0, BacklightCompensationMode__ON = 1};

struct BacklightCompensation
{
	enum BacklightCompensationMode Mode;
	float Level;
};


enum WideDynamicMode {WideDynamicMode__OFF = 0, WideDynamicMode__ON = 1};
struct WideDynamicRange
{
	enum WideDynamicMode Mode;
	float Level;
};

enum WhiteBalanceMode {WhiteBalanceMode__AUTO = 0, WhiteBalanceMode__MANUAL = 1};
struct WhiteBalance
{
	enum WhiteBalanceMode Mode;
	float CrGain;
	float CbGain;
};


enum IrCutFilterMode {IrCutFilterMode__ON = 0, IrCutFilterMode__OFF = 1, IrCutFilterMode__AUTO = 2};
typedef struct _onvif_image_settings
{
	struct BacklightCompensation backlightcompensation;
	float brightness;
	float colorsaturation;
	float contrast;
	float sharpness;
	enum IrCutFilterMode IrCutFilter;
	struct WideDynamicRange widedynamicrange;
	struct WhiteBalance whitebalance;
}onvif_image_settings_t;


enum
{
	ONVIF_CMD_DISCOVER 			= 0,
	ONVIF_CMD_GET_CAPABILITY,
	ONVIF_CMD_GET_PROFILES,
	ONVIF_CMD_GET_URL,
	ONVIF_CMD_GET_DEV_INFOMATION,
	ONVIF_CMD_GET_SCOPES,
	ONVIF_CMD_GET_NETWORK_INTERFACE,
	ONVIF_CMD_GET_DNS,
	ONVIF_CMD_GET_SERVICES,
	ONVIF_CMD_START_STREAM,
	ONVIF_CMD_STOP_STREAM,
};


typedef struct Onvif_PTZPreset
{
	char PresetName[ONVIF_MAX_NAME_LEN];	
	char Presettoken[ONVIF_MAX_NAME_LEN];	
}Onvif_PTZPreset_t;

typedef struct _onvif_ptzpresets_list_
{
	struct list_head list;
	Onvif_PTZPreset_t data;
}onvif_ptzpresets_list_t;


#define _onvif_list_add(list_type, plist, data_type, pdata) 	\
	do{															\
		list_type *p = NULL;									\
		p = (list_type *)malloc(sizeof(list_type));				\
		if (p == NULL)											\
			return -1;											\
		memset(p, 0 , sizeof(list_type));						\
		memcpy(&p->data , pdata, sizeof(data_type)); 			\
		list_add_tail(&p->list, &plist->list);					\
	}while(0)

#define _onvif_list_clear(list_type, plist) 					\
	do{															\
		list_type *pDate = NULL; 								\
		struct list_head *pos = NULL, *n = NULL; 				\
		list_for_each_safe(pos, n, &(plist->list))				\
		{														\
			pDate = list_entry(pos, list_type, list);			\
			if (NULL != pDate)									\
			{													\
				list_del(&pDate->list);							\
				free(pDate);									\
				pDate = NULL;									\
			}													\
		}														\
	}while(0)



typedef void (*fSearchCb)(int nIndex, discover_info_t *pInfo);

char *onvif_get_profile_token(onvif_profile_list_t *profile_list, int index);

char *onvif_get_video_source_token(onvif_profile_list_t *profile_list, int index);

char *onvif_get_video_encode_token(onvif_profile_list_t *profile_list, int index);

char *onvif_get_video_encode_name(onvif_profile_list_t *profile_list, int index);

char *onvif_get_preset_token(onvif_ptzpresets_list_t *profile_list, int presetNo);

int nvr_onvif_discover(int timeout, fSearchCb cbSearchFunc);

int nvr_onvif_get_scopes(const char *host, char *port, onvif_scopes_list_t *list_scopes);/*GetScopes*/

int nvr_onvif_get_capability(const char *host, char *port, device_capability_t *capability);

int nvr_onvif_get_device_info(const char *host, char *port, device_info_t *dev_info);

int nvr_onvif_get_network_interface(const char *host, char *port, network_interface_t *network);

int nvr_onvif_get_profiles(const char *host, char *port, onvif_profile_list_t *list_profile);

int nvr_onvif_get_stream_url(const char *host, const char *port, const char *profileToken, char *fullUrl);

int nvr_onvif_system_reboot(const char *host, char *port);

int nvr_onvif_get_image_options(const char *host , char *port, const char *videosrcToken ,onvif_image_options_t *image);

int nvr_onvif_get_image_settings(const char *host , char *port, const char *videosrcToken ,onvif_image_settings_t *settings);

int nvr_onvif_set_image_settings(const char *host , char *port, const char *videosrcToken ,onvif_image_settings_t *settings);

int nvr_onvif_get_profile(const char *host, char *port, const char *profileToken, profile_data_t *data);

int onvif_set_video_encoder_configurations(const char *host, char *port, video_encode_cfg_t *cfg);

int nvr_onvif_get_video_encode_configuration_options(const char *host , char *port, const char *videosrcToken ,onvif_venccfg_options_t *vencoptions);

int nvr_onvif_get_presets(const char *host, char *port, const char *profileToken, onvif_ptzpresets_list_t *presets);

int nvr_onvif_goto_presets(const char *host, char *port, const char *profileToken, const char *prosetsToken);

int nvr_onvif_set_presets(const char *host, char *port, const char *profileToken, const char *presetName);

int nvr_onvif_ptz_continuous_move(const char *host, char *port, const char *profileToken, float speed_x, float speed_y);

int nvr_onvif_ptz_stop(const char *host, char *port, const char *profileToken);

int nvr_onvif_set_time_settings(const char *host , char *port ,int zone, int year, int mon, 
										int day, int hour, int min, int sec);

#endif

