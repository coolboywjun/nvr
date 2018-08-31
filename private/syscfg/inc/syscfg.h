#ifndef __SYSCFG_H__
#define __SYSCFG_H__

#define SYSTEM_CFG_MAGIC 0x4e565258
#define DEV_SERIALNO_LEN 48



//SoftType
typedef enum _SOFT_TYPE
{
	EN_SOFT_NONNEUTER = 0,   //非中性
	EN_SOFT_NEUTER,			 //中性
}SOFT_TYPE;

enum __PLATFORM_SERVER
{
	PLATFORM_ONVIF_SERVER	= 1<<0,
	PLATFORM_TUTK_SERVER	= 1<<1,
	PLATFORM_VIVI_SERVER		= 1<<2,
	PLATFORM_KANDIAN_SERVER = 1<<3,

}PLATFORM_SERVER;

enum __PLATFORM_CLIENT
{
	PLATFORM_ONVIF_CLIENT	= 1<<0,
	
}PLATFORM_CLIENT;

enum __SUPPORT_LANG
{
	LANG_SIMPLIFIED_CHINESE 	= 0,
	LANG_ENGLISH 				= 1,
	LANG_TRADITIONAL_CHINESE = 2,
}SUPPORT_LANG;

#pragma pack(4)
typedef struct Tag_DEV_SystemCfg
{
unsigned int    Magic;
unsigned char u8SerialId[DEV_SERIALNO_LEN];  //设备Id
unsigned char u8DiNum; //报警输入路数
unsigned char u8DoNum; //报警输出路数
unsigned char u8ChnNum  ; //设备路数
unsigned char u8Rs232Num; //设备232 串口个数
unsigned char u8Rs485Num; //设备485 串口个数
unsigned char u8NetCardNum; //网卡个数
unsigned char u8SupportAudio;
unsigned char u8SupportHDMI;
unsigned char u8supportVGA;
unsigned char u8SataNum;
unsigned char u8SoftType;    //软件类型,中性，非中性
unsigned short  u16DevType ; //设备类型 
unsigned short u16PlatformFlag; //记录平台类型配置信息
unsigned char u8HwVer[16]; //硬件版本号
unsigned char u8Mac[6];
unsigned char u8FactoryChecked;	// 是否做过出厂检测
unsigned char u8NoWdt;			// 是否不使用wdt
unsigned int  defaultIP;
unsigned int  defaultMask;
unsigned int  defaultGateWay;
unsigned long platformServer;	// enum PLATFORM_SERVER, bit value 1: 支持，0:不支持
unsigned long platformClient;	// enum PLATFORM_CLIENT
unsigned long supportLang;		// enum SUPPORT_LANG, bit value 1: 支持，0:不支持
unsigned char u8DefLang;
unsigned char u8Rev[115];
unsigned long checksum;
}DEV_SystemCfg;
#pragma pack()
#define DEV_SYSTEM_CFG_LEN	(sizeof(DEV_SystemCfg))

typedef struct Tag_DEV_KanDianCfg
{
	char szGroupUrl[64];				//集团平台URL
	char szProvincialUrl[64];			//省平台URL
	char szDevId[32];					//设备ID
	char szKey[32];					//key
	char szMode[16];					//设备类型
	char szRes[128];
	unsigned long checksum;
}DEV_KanDianCfg;
#define DEV_KANDIAN_CFG_LEN	(sizeof(DEV_KanDianCfg))

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

DEV_SystemCfg * get_system_cfg();
	
int save_system_cfg(DEV_SystemCfg *pCfg);

int default_system_cfg();

DEV_KanDianCfg *get_kandian_cfg();

int save_kandian_cfg(DEV_KanDianCfg *pCfg);

#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif

