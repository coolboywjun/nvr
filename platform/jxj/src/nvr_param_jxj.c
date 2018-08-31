
#include <nvr_headers.h>
#include <ipnc_lib.h>
#include <update.h>
#include <storage_lib.h>
#include <nvr_lib_interface.h>

#include "nvr_platform_jxj.h"
#include "nvr_param_jxj.h"
#include "nvr_character_conv.h"
#include "nvr_vod_jxj.h"


#undef	G_LOG_DOMAIN
#define	G_LOG_DOMAIN "param_jxj"

int gIsSetReboot = 0;

typedef int(* JXJ_SET_FUNC)(int id, int channel, void *param, user_t *u);
typedef int(* JXJ_GET_FUNC)(int id, int channel, void *param, user_t *u);
typedef struct _JXJ_FUNC_STRUCT
{
	int id;
	JXJ_SET_FUNC set_func;
	JXJ_GET_FUNC get_func;
}JXJ_FUNC_STRUCT;


#define CHANNEL_CHECK(chnNo)	\
do								\
{								\
	if((0 > chnNo) || (chnNo >= NVR_MAX_CHANNEL_NUM))	\
		return -1;				\
}while(0)

static inline unsigned long nvr_jxj_convert_ip(const char *ipstr, uint32_t *ip)
{
	unsigned int data[4];
	
	if(sscanf(ipstr, "%u.%u.%u.%u", 
			&data[0],&data[1],&data[2],&data[3]) != 4)
		return -1;
	*ip = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	return 0;
}

static inline void nvr_jxj_reconvert_ip(unsigned long ip, char *ipstr)
{
	sprintf(ipstr, "%lu.%lu.%lu.%lu", 
			(ip >> 24) & 0xff, (ip >> 16) & 0xff,
			(ip >> 8) & 0xff, (ip >> 0) & 0xff);
}


static void nvr_jxj_display_week(JWeek *pWeek)
{
//#ifdef __DEBUG__
#if 0
	int i, day;
	JTimeSeg *pSeg;

	printf("++++++++++++ display week ++++++++++++\n");
	printf("week count = %d\n", pWeek->count);
	for(day = 0; day < J_SDK_MAX_DAY_SIZE; day++)
	{
		printf("day[%d].count = %d\n", day, pWeek->days[day].count);
		printf("day[%d].day_id = %d\n", day, pWeek->days[day].day_id);
		printf("day[%d].all_day_enable = %d\n", day, pWeek->days[day].all_day_enable);
		pSeg = pWeek->days[day].seg;
		for(i = 0; i<J_SDK_MAX_SEG_SZIE; i++)
		{
			printf("seg[%d].enable = %d\n", i, pSeg[i].enable);
			printf("%d:%d:%d - %d:%d:%d\n",
					pSeg[i].time_start.hour, pSeg[i].time_start.minute, pSeg[i].time_start.second,
					pSeg[i].time_end.hour, pSeg[i].time_end.minute, pSeg[i].time_end.second);
		}
		printf("\n");
	}
	printf("++++++++++++++++++++++++++++++++++++\n");
#endif
}

/*
static int nvr_jxj_set_register_request(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_register_request(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_register_response(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_register_response(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_heart_beat_request(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_heart_beat_request(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_heart_beat_response(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_heart_beat_response(int id, int channel, void *param, user_t *u)
{
	return -1;
}
*/
static int nvr_jxj_set_sync_time(int id, int channel, void *param, user_t *u)//时区和同步没有处理
{

	JDeviceTime *pDeviceTime = (JDeviceTime *)param;
	NVR_DEV_JXJ_PLATFORM_CFG_S stJxjCfg;
	//NVR_NTP_CFG_S stNtpCfg = {0};

	//nvr_logon_get_ntp_cfg(0, &stNtpCfg);
	if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjCfg))
	{
		j_warning("nvr_logon_get_jxj_platform_cfg failed.");
		return -1;
	}
	if(stJxjCfg.u8SyncTimeEnable)
	{
		NVR_TIME_S stTimeCfg;
		stTimeCfg.u8Year = 	pDeviceTime->time.year;
		stTimeCfg.u8Month= pDeviceTime->time.month;
		stTimeCfg.u8Day = pDeviceTime->time.date;
		stTimeCfg.u8Hour = pDeviceTime->time.hour;
		stTimeCfg.u8Minute = pDeviceTime->time.minute;
		stTimeCfg.u8Second = pDeviceTime->time.second;
		if(0 > nvr_logon_set_time_cfg(0, &stTimeCfg))
		{
			j_warning("nvr_logon_set_time_cfg failed.");
			return -1;
		}
	}

	return 0;
}

static int nvr_jxj_get_sync_time(int id, int channel, void *param, user_t *u)
{
	JDeviceTime *pDeviceTime = (JDeviceTime *)param;
	NVR_TIME_S stTimeCfg = {0};
	NVR_NTP_CFG_S stNtpCfg = {0};
	NVR_DEV_JXJ_PLATFORM_CFG_S stJxjCfg;

	memset(&stTimeCfg, 0, sizeof(NVR_TIME_S));
	if(0 > nvr_logon_get_time_cfg(0, &stTimeCfg))
	{	
		j_warning("nvr_logon_get_time_cfg failed."); 
		return -1;
	}
	if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjCfg))
	{
		j_warning("nvr_logon_get_jxj_platform_cfg failed."); 	
		return -1;
	}
	if(0 > nvr_logon_get_ntp_cfg(0, &stNtpCfg))
	{
		j_warning("nvr_logon_get_ntp_cfg failed."); 	
		return -1;
	}
	pDeviceTime->time.year = stTimeCfg.u8Year;
	pDeviceTime->time.month = stTimeCfg.u8Month;
	pDeviceTime->time.date = stTimeCfg.u8Day;
	pDeviceTime->time.hour = stTimeCfg.u8Hour;
	pDeviceTime->time.minute = stTimeCfg.u8Minute;
	pDeviceTime->time.second = stTimeCfg.u8Second;
	pDeviceTime->time.weekday = stTimeCfg.u8WDay;
	pDeviceTime->zone = stNtpCfg.nTimeZone /3600;
	pDeviceTime->sync_enable = stJxjCfg.u8SyncTimeEnable;
	//j_debug("The pDeviceTime->zone is %d", pDeviceTime->zone);
	return 0;
}

static int nvr_jxj_set_change_dispatch(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_change_dispatch(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_device_info(int id, int channel, void *param, user_t *u)
{
	j_debug("manu_info   : %s", ((JDeviceInfo*)param)->manu_info);
	j_debug("release_date: %s", ((JDeviceInfo*)param)->release_date);
	j_debug("dev_version : %s", ((JDeviceInfo*)param)->dev_version);			
	j_debug("hw_version  : %s", ((JDeviceInfo*)param)->hw_version);			
	j_debug("pu_type     : %d", ((JDeviceInfo*)param)->pu_type);
	j_debug("sub_type    : %d", ((JDeviceInfo*)param)->sub_type);
	j_debug("di_num      : %d", ((JDeviceInfo*)param)->di_num);
	j_debug("do_num      : %d", ((JDeviceInfo*)param)->do_num);
	j_debug("channel_num : %d", ((JDeviceInfo*)param)->channel_num);
	j_debug("rs485_num   : %d", ((JDeviceInfo*)param)->rs485_num);
	j_debug("rs232_num   : %d", ((JDeviceInfo*)param)->rs232_num);
	/*stable信息不可修改*/
	return 0;
}

static int nvr_jxj_get_device_info(int id, int channel, void *param, user_t *u)
{
	int size;
	JDeviceInfo *pDeviceInfo = (JDeviceInfo *)param;
	NVR_DEV_INFO_S stNVRDevInfo = {0};
	//NVR_DEV_STABLE_INFO_S stStableInfo = {0};
	
	if(0 > nvr_logon_get_dev_cfg(0, &stNVRDevInfo))
	{
		j_warning("nvr_logon_get_dev_cfg failed.");
		return -1;
	}
	/*if(0 > nvr_logon_get_stable_cfg(0, &stStableInfo))
	{
		j_warning("nvr_logon_get_stable_cfg failed.");
		return -1;
	}*/
	size = sizeof(pDeviceInfo->manu_info);
	strncpy((char *)pDeviceInfo->manu_info, "JXJ", size-1);
	pDeviceInfo->manu_info[size-1] = '\0';
	
	size = sizeof(pDeviceInfo->release_date);
	snprintf((char *)pDeviceInfo->release_date, size - 1, "%s - %s", __DATE__, __TIME__);
	pDeviceInfo->release_date[size-1] = '\0';
	
	size = sizeof(pDeviceInfo->dev_version);
	strncpy((char *)pDeviceInfo->dev_version, stNVRDevInfo.stStableInfo.szSoftVer,size - 1);
	pDeviceInfo->dev_version[size-1] = '\0';
	j_debug("software version: %s", stNVRDevInfo.stStableInfo.szSoftVer);
	
	size = sizeof(pDeviceInfo->hw_version);
	strncpy((char *)pDeviceInfo->hw_version, stNVRDevInfo.stStableInfo.szHwVer,size - 1);
	pDeviceInfo->hw_version[size-1] = '\0';
	
	pDeviceInfo->pu_type = PU_TYPE;
	pDeviceInfo->sub_type = 1;
	pDeviceInfo->di_num = stNVRDevInfo.stStableInfo.u8AlarmInputNum;
	pDeviceInfo->do_num = stNVRDevInfo.stStableInfo.u8AlarmOutputNum;
	pDeviceInfo->channel_num = stNVRDevInfo.stStableInfo.u8ChnNum;
	pDeviceInfo->rs485_num = stNVRDevInfo.stStableInfo.u8RS485Num;
	pDeviceInfo->rs232_num = stNVRDevInfo.stStableInfo.u8RS232Num;
	return 0;
}

static int nvr_jxj_set_device_ntp_info(int id, int channel, void *param, user_t *u)
{
	JDeviceNTPInfo *pDeviceNtpInfo = (JDeviceNTPInfo *)param;
	NVR_NTP_CFG_S stNtpCfg = {0};
	NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg;
	
	if(0 > nvr_logon_get_ntp_cfg(0, &stNtpCfg))
	{
		j_warning("nvr_logon_get_ntp_cfg failed.");
		return -1;
	}
	
	stNtpCfg.u32Size = sizeof(stNtpCfg);
	stNtpCfg.u8NtpOpen = pDeviceNtpInfo->ntp_enable;
	stNtpCfg.u8EuroTime = pDeviceNtpInfo->dst_enable;
	stNtpCfg.u8NtpRefTime = pDeviceNtpInfo->time_interval;	// 对时间隔
	stNtpCfg.nTimeZone = pDeviceNtpInfo->time_zone *3600;
	strncpy(stNtpCfg.szNtpServer, (char *)pDeviceNtpInfo->ntp_server_ip, 
			sizeof(stNtpCfg.szNtpServer) - 1);
	stNtpCfg.szNtpServer[sizeof(stNtpCfg.szNtpServer) - 1] = '\0';

	if(0 > nvr_logon_set_ntp_cfg(0, &stNtpCfg))
		return -1;

	if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg))
	{
		j_warning("nvr_logon_get_jxj_platform_cfg failed.");
		return -1;
	}
	stJxjPlatformCfg.u8SyncTimeEnable = 0;
	if(0 > nvr_logon_set_jxj_platform_cfg(0, &stJxjPlatformCfg))
		return -1;

	return 0;
}

static int nvr_jxj_get_device_ntp_info(int id, int channel, void *param, user_t *u)
{
	JDeviceNTPInfo *pDeviceNtpInfo = (JDeviceNTPInfo *)param;
	NVR_NTP_CFG_S stNtpCfg = {0};

	if(0 > nvr_logon_get_ntp_cfg(0, &stNtpCfg))
	{
		j_warning("nvr_logon_get_ntp_cfg failed.");
		return -1;
	}

	strncpy((char *)pDeviceNtpInfo->ntp_server_ip, stNtpCfg.szNtpServer,
			sizeof(pDeviceNtpInfo->ntp_server_ip) - 1);
	pDeviceNtpInfo->ntp_server_ip[sizeof(pDeviceNtpInfo->ntp_server_ip) - 1] = '\0';
	pDeviceNtpInfo->time_zone = stNtpCfg.nTimeZone / 3600;
	pDeviceNtpInfo->time_interval = stNtpCfg.u8NtpRefTime;	// 对时间隔
	pDeviceNtpInfo->ntp_enable = stNtpCfg.u8NtpOpen;
	pDeviceNtpInfo->dst_enable = stNtpCfg.u8EuroTime;
	return 0;
}

static int nvr_jxj_set_device_time(int id, int channel, void *param, user_t *u)
{
	NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg;
	JDeviceTime *pDeviceTime = (JDeviceTime *)param;
	NVR_TIME_S stTimeCfg = {0};

	if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg))
	{
		j_warning("nvr_logon_get_jxj_platform_cfg failed.");
		return -1;
	}	
	stJxjPlatformCfg.u8SyncTimeEnable = pDeviceTime->sync_enable;
	if(0 > nvr_logon_set_jxj_platform_cfg(0, &stJxjPlatformCfg))
	{
		j_warning("nvr_logon_set_jxj_platform_cfg failed.");
		return -1;
	}

	if(stJxjPlatformCfg.u8SyncTimeEnable == 0)
	{		
		if(0 > nvr_logon_get_time_cfg(0, &stTimeCfg))
		{
			j_warning("nvr_logon_get_time_cfg failed.");
			return -1;
		}	
		stTimeCfg.u8Year = 	pDeviceTime->time.year;
		stTimeCfg.u8Month= pDeviceTime->time.month;
		stTimeCfg.u8Day = pDeviceTime->time.date;
		stTimeCfg.u8Hour = pDeviceTime->time.hour;
		stTimeCfg.u8Minute = pDeviceTime->time.minute;
		stTimeCfg.u8Second = pDeviceTime->time.second;
		if(0 > nvr_logon_set_time_cfg(0, &stTimeCfg))
		{
			j_warning("nvr_logon_set_time_cfg failed.");
			return -1;
		}	
	}
	return 0;
}

static int nvr_jxj_get_device_time(int id, int channel, void *param, user_t *u)
{
	JDeviceTime *pDeviceTime = (JDeviceTime *)param;
	NVR_TIME_S stTimeCfg = {0};
	NVR_DEV_JXJ_PLATFORM_CFG_S stJxjCfg;
	NVR_NTP_CFG_S stNtpCfg = {0};

	memset(&stTimeCfg, 0, sizeof(stTimeCfg));
	if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjCfg))
	{
		j_warning("nvr_logon_get_jxj_platform_cfg failed.");
		return -1;
	}
	if(0 > nvr_logon_get_time_cfg(0, &stTimeCfg))
	{
		j_warning("nvr_logon_get_time_cfg failed.");
		return -1;
	}
	if(0 > nvr_logon_get_ntp_cfg(0, &stNtpCfg))
	{
		j_warning("nvr_logon_get_ntp_cfg failed.");
		return -1;
	}	

	pDeviceTime->time.year = stTimeCfg.u8Year;
	pDeviceTime->time.month = stTimeCfg.u8Month;
	pDeviceTime->time.date = stTimeCfg.u8Day;
	pDeviceTime->time.hour = stTimeCfg.u8Hour;
	pDeviceTime->time.minute = stTimeCfg.u8Minute;
	pDeviceTime->time.second = stTimeCfg.u8Second;
	pDeviceTime->time.weekday = stTimeCfg.u8WDay;
	pDeviceTime->zone = stNtpCfg.nTimeZone /3600;
	pDeviceTime->sync_enable = !!stJxjCfg.u8SyncTimeEnable;

	return 0;
}

static int nvr_jxj_set_platform_info(int id, int channel, void *param, user_t *u)
{
	JPlatformInfo *pPlatformInfo = (JPlatformInfo *)param;
	NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg;
	
	if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg))
		return -1;
	strncpy(stJxjPlatformCfg.szPuId, (char *)pPlatformInfo->pu_id, 
			sizeof(stJxjPlatformCfg.szPuId) - 1);
	strncpy(stJxjPlatformCfg.szCmsIp, (char *)pPlatformInfo->cms_ip, 
			sizeof(stJxjPlatformCfg.szCmsIp) - 1);
	stJxjPlatformCfg.u16CmsPort = pPlatformInfo->cms_port;
	stJxjPlatformCfg.u8ProtocolType = pPlatformInfo->protocol;
	stJxjPlatformCfg.u8Enable = pPlatformInfo->is_con_cms;
	if(0 > nvr_logon_set_jxj_platform_cfg(0, &stJxjPlatformCfg))
		return -1;

	return 0;
}

static int nvr_jxj_get_platform_info(int id, int channel, void *param, user_t *u)
{
	JPlatformInfo *pPlatformInfo = (JPlatformInfo *)param;
	NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg;

	memset(pPlatformInfo, 0, sizeof(JPlatformInfo));
	if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg))
		return -1;
	strncpy((char *)pPlatformInfo->pu_id, stJxjPlatformCfg.szPuId, 
			sizeof(pPlatformInfo->pu_id) - 1);
	strncpy((char *)pPlatformInfo->cms_ip, stJxjPlatformCfg.szCmsIp,
			sizeof(pPlatformInfo->cms_ip) - 1);
	pPlatformInfo->cms_port = stJxjPlatformCfg.u16CmsPort;
	pPlatformInfo->protocol = stJxjPlatformCfg.u8ProtocolType;
	pPlatformInfo->is_con_cms = stJxjPlatformCfg.u8Enable;
	return 0;
}

static int nvr_jxj_set_network_info(int id, int channel, void *param, user_t *u)
{
	int i;
	JNetworkInfo *pNetworkInfo = (JNetworkInfo *)param;
	NVR_DEV_NET_CFG_S stNetCfg = {0};
	unsigned int data[6];
	
	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
	{
		j_warning("nvr_logon_get_net_cfg failed.");
		return -1;
	}
	stNetCfg.struEtherCfg[0].u8DhcpOn = pNetworkInfo->network[J_SDK_ETH0].dhcp_enable;
	sscanf((char *)pNetworkInfo->network[J_SDK_ETH0].mac, "%02X:%02X:%02X:%02X:%02X:%02X", 
			&data[0], &data[1], &data[2],
			&data[3], &data[4], &data[5]);
	for(i=0; i<sizeof(data)/sizeof(int); i++)
	{
		stNetCfg.struEtherCfg[0].u8MacAddr[i] = data[i];
	}
	sscanf((char *)pNetworkInfo->network[J_SDK_ETH0].ip, "%u.%u.%u.%u", 
			&data[0],&data[1],&data[2],&data[3]);
	stNetCfg.struEtherCfg[0].u32IpAddr = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	
	sscanf((char *)pNetworkInfo->network[J_SDK_ETH0].netmask, "%u.%u.%u.%u", 
			&data[0],&data[1],&data[2],&data[3]);
	stNetCfg.struEtherCfg[0].u32SubMask = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

	sscanf((char *)pNetworkInfo->network[J_SDK_ETH0].gateway, "%u.%u.%u.%u", 
			&data[0],&data[1],&data[2],&data[3]);
	stNetCfg.struEtherCfg[0].u32GateWay = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	
	sscanf((char *)pNetworkInfo->main_dns, "%u.%u.%u.%u", 
			&data[0],&data[1],&data[2],&data[3]);
	stNetCfg.u32DnsIp1 = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

	sscanf((char *)pNetworkInfo->backup_dns, "%u.%u.%u.%u", 
			&data[0],&data[1],&data[2],&data[3]);
	stNetCfg.u32DnsIp2 = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	
	stNetCfg.u8DnsDhcp 	= pNetworkInfo->auto_dns_enable;
	stNetCfg.u16CmdPort = pNetworkInfo->cmd_port;
	stNetCfg.u16HttpPort = pNetworkInfo->web_port;
	//stNetCfg.u16TalkPort = pNetworkInfo->talk_port;
	//stNetCfg.u16JXJDataPort = pNetworkInfo->data_port;
	
	if(0 > nvr_logon_set_net_cfg(0, &stNetCfg))
	{
		j_warning("nvr_logon_set_net_cfg failed.");
		return -1;
	}	
	return 0;
}

static int nvr_jxj_get_network_info(int id, int channel, void *param, user_t *u)
{
	JNetworkInfo *pNetworkInfo = (JNetworkInfo *)param;
	NVR_DEV_NET_CFG_S stNetCfg = {0};
	char szIp[64];
	
	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
	{
		j_warning("nvr_logon_get_net_cfg failed.");
		return -1;
	}	
	
	memset(pNetworkInfo, 0, sizeof(JNetworkInfo));
	pNetworkInfo->network[J_SDK_ETH0].type = J_SDK_ETH0;
	pNetworkInfo->network[J_SDK_ETH0].dhcp_enable = stNetCfg.struEtherCfg[0].u8DhcpOn;
	
	snprintf((char *)pNetworkInfo->network[J_SDK_ETH0].mac, sizeof(pNetworkInfo->network[J_SDK_ETH0].mac),
			"%02X:%02X:%02X:%02X:%02X:%02X",
			stNetCfg.struEtherCfg[0].u8MacAddr[0],
			stNetCfg.struEtherCfg[0].u8MacAddr[1],
			stNetCfg.struEtherCfg[0].u8MacAddr[2],
			stNetCfg.struEtherCfg[0].u8MacAddr[3],
			stNetCfg.struEtherCfg[0].u8MacAddr[4],
			stNetCfg.struEtherCfg[0].u8MacAddr[5]);
	snprintf((char *)pNetworkInfo->network[J_SDK_ETH0].ip, sizeof(pNetworkInfo->network[J_SDK_ETH0].ip), 
			"%d.%d.%d.%d", 
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 24) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 16) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 8) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 0) & 0xff);
	snprintf((char *)pNetworkInfo->network[J_SDK_ETH0].netmask, sizeof(pNetworkInfo->network[J_SDK_ETH0].netmask), 
			"%d.%d.%d.%d",
			(stNetCfg.struEtherCfg[0].u32SubMask >> 24) & 0xff,
			(stNetCfg.struEtherCfg[0].u32SubMask >> 16) & 0xff,
			(stNetCfg.struEtherCfg[0].u32SubMask >> 8) & 0xff,
			(stNetCfg.struEtherCfg[0].u32SubMask >> 0) & 0xff);
	snprintf((char *)pNetworkInfo->network[J_SDK_ETH0].gateway, sizeof(pNetworkInfo->network[J_SDK_ETH0].gateway), 
			"%d.%d.%d.%d", 
			(stNetCfg.struEtherCfg[0].u32GateWay >> 24) & 0xff,
			(stNetCfg.struEtherCfg[0].u32GateWay >> 16) & 0xff,
			(stNetCfg.struEtherCfg[0].u32GateWay >> 8) & 0xff,
			(stNetCfg.struEtherCfg[0].u32GateWay >> 0) & 0xff);
	snprintf((char *)pNetworkInfo->main_dns, sizeof(pNetworkInfo->main_dns), 
			"%d.%d.%d.%d", 
			(stNetCfg.u32DnsIp1 >> 24) & 0xff,
			(stNetCfg.u32DnsIp1 >> 16) & 0xff,
			(stNetCfg.u32DnsIp1 >> 8) & 0xff,
			(stNetCfg.u32DnsIp1 >> 0) & 0xff);
	snprintf((char *)pNetworkInfo->backup_dns, sizeof(pNetworkInfo->backup_dns), 
			"%d.%d.%d.%d", 
			(stNetCfg.u32DnsIp2 >> 24) & 0xff,
			(stNetCfg.u32DnsIp2 >> 16) & 0xff,
			(stNetCfg.u32DnsIp2 >> 8) & 0xff,
			(stNetCfg.u32DnsIp2 >> 0) & 0xff);

	pNetworkInfo->auto_dns_enable = stNetCfg.u8DnsDhcp;
	pNetworkInfo->cur_network = J_SDK_ETH0;
	pNetworkInfo->cmd_port = JXJ_SDK_PORT;
	pNetworkInfo->data_port = JXJ_RTSP_PORT;
	pNetworkInfo->web_port = stNetCfg.u16HttpPort;
	pNetworkInfo->talk_port = JXJ_TALK_PORT;
	if(net_get_ip_addr("ra0", szIp) == 0)
	{
		pNetworkInfo->network[J_SDK_WIFI].type = J_SDK_WIFI;
		pNetworkInfo->network[J_SDK_WIFI].dhcp_enable = stNetCfg.u8DnsDhcp;
		snprintf((char *)pNetworkInfo->network[J_SDK_WIFI].mac, sizeof(pNetworkInfo->network[J_SDK_WIFI].mac),
				"%02X:%02X:%02X:%02X:%02X:%02X",
				stNetCfg.struEtherCfg[1].u8MacAddr[0],
				stNetCfg.struEtherCfg[1].u8MacAddr[1],
				stNetCfg.struEtherCfg[1].u8MacAddr[2],
				stNetCfg.struEtherCfg[1].u8MacAddr[3],
				stNetCfg.struEtherCfg[1].u8MacAddr[4],
				stNetCfg.struEtherCfg[1].u8MacAddr[5]);
		snprintf((char *)pNetworkInfo->network[J_SDK_WIFI].ip, sizeof(pNetworkInfo->network[J_SDK_WIFI].ip), 
				"%d.%d.%d.%d", 
				(stNetCfg.struEtherCfg[1].u32IpAddr >> 24) & 0xff,
				(stNetCfg.struEtherCfg[1].u32IpAddr >> 16) & 0xff,
				(stNetCfg.struEtherCfg[1].u32IpAddr >> 8) & 0xff,
				(stNetCfg.struEtherCfg[1].u32IpAddr >> 0) & 0xff);
		snprintf((char *)pNetworkInfo->network[J_SDK_WIFI].netmask, sizeof(pNetworkInfo->network[J_SDK_WIFI].netmask), 
				"%d.%d.%d.%d",
				(stNetCfg.struEtherCfg[1].u32SubMask >> 24) & 0xff,
				(stNetCfg.struEtherCfg[1].u32SubMask >> 16) & 0xff,
				(stNetCfg.struEtherCfg[1].u32SubMask >> 8) & 0xff,
				(stNetCfg.struEtherCfg[1].u32SubMask >> 0) & 0xff);
		snprintf((char *)pNetworkInfo->network[J_SDK_WIFI].gateway, sizeof(pNetworkInfo->network[J_SDK_WIFI].gateway), 
				"%d.%d.%d.%d", 
				(stNetCfg.struEtherCfg[1].u32GateWay >> 24) & 0xff,
				(stNetCfg.struEtherCfg[1].u32GateWay >> 16) & 0xff,
				(stNetCfg.struEtherCfg[1].u32GateWay >> 8) & 0xff,
				(stNetCfg.struEtherCfg[1].u32GateWay >> 0) & 0xff);
	}
	return 0;
}

static int nvr_jxj_set_pppoe_info(int id, int channel, void *param, user_t *u)
{

	JPPPOEInfo *pPppoeInfo = (JPPPOEInfo *)param;
	NVR_DEV_NET_CFG_S stNetCfg = {0};

	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
	{
		j_warning("nvr_logon_get_net_cfg failed.");
		return -1;
	}

	stNetCfg.struPppoeCfg.u8PppoeOn = pPppoeInfo->enable;

	if(stNetCfg.struPppoeCfg.u8PppoeOn == 1)
	{
		strncpy(stNetCfg.struPppoeCfg.szPppoeUsr, (char *)pPppoeInfo->account, 
				sizeof(stNetCfg.struPppoeCfg.szPppoeUsr) - 1);
		stNetCfg.struPppoeCfg.szPppoeUsr[sizeof(stNetCfg.struPppoeCfg.szPppoeUsr) - 1] = '\0';
		
		strncpy(stNetCfg.struPppoeCfg.szPppoePsw, (char *)pPppoeInfo->passwd, 
				sizeof(stNetCfg.struPppoeCfg.szPppoePsw) - 1);
		stNetCfg.struPppoeCfg.szPppoePsw[sizeof(stNetCfg.struPppoeCfg.szPppoePsw) - 1] = '\0';
	}
	if(0 > nvr_logon_set_net_cfg(0, &stNetCfg))
	{
		j_warning("nvr_logon_set_net_cfg failed.");
		return -1;
	}	
	return 0;
}

static int nvr_jxj_get_pppoe_info(int id, int channel, void *param, user_t *u)
{

	JPPPOEInfo *pPppoeInfo = (JPPPOEInfo *)param;
	NVR_DEV_NET_CFG_S stNetCfg = {0};
	
	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
	{
		j_warning("nvr_logon_get_net_cfg failed.");
		return -1;
	}
	if(stNetCfg.struPppoeCfg.u8PppoeOn)
	{
		pPppoeInfo->type = J_SDK_ETH0;
		pPppoeInfo->enable = 1;
		net_ip_n2a(stNetCfg.struPppoeCfg.u32PppoeIp, (char *)pPppoeInfo->ip, sizeof(pPppoeInfo->ip));
		strncpy((char *)pPppoeInfo->account, stNetCfg.struPppoeCfg.szPppoeUsr, 
				sizeof(pPppoeInfo->account) - 1);
		pPppoeInfo->account[sizeof(pPppoeInfo->account) - 1] = '\0';
		
		strncpy((char *)pPppoeInfo->passwd, stNetCfg.struPppoeCfg.szPppoePsw,
				sizeof(pPppoeInfo->passwd) - 1);
		pPppoeInfo->passwd[sizeof(pPppoeInfo->passwd) - 1] = '\0';

	}
	else
	{
		pPppoeInfo->enable = 0;
		memset(pPppoeInfo->account, 0, sizeof(pPppoeInfo->account));
		memset(pPppoeInfo->passwd, 0, sizeof(pPppoeInfo->passwd));
		memset(pPppoeInfo->ip, 0, sizeof(pPppoeInfo->ip));
	}
	pPppoeInfo->type = J_SDK_ETH0;

	return 0;
}

static int nvr_jxj_set_encode_info(int id, int channel, void *param, user_t *u)
{
	JEncodeParameter *pEncodeParam = (JEncodeParameter *)param;
	IPNC_DEV_VENC_CFG_S stVencCfg = {0};
	IPNC_AUDIO_CFG_S 	stAudioCfg = {0};
	IPNC_DEV_VIDEO_CFG_S stVideoCfg = {0};
	IPNC_DEV_CFG_S stDevInfo = {{0}};

	CHANNEL_CHECK(channel);
	
	//获取视频参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_VIDEO_CFG, 
					  		&stVideoCfg, sizeof(IPNC_DEV_VIDEO_CFG_S)))
		return -1;
	//获取设备参数配置
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_DEV_CFG,  
					  		&stDevInfo, sizeof(IPNC_DEV_CFG_S)))
		return -1;

	//获取音频参数配置
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_AUDIO_CFG, 
					  		&stAudioCfg, sizeof(IPNC_AUDIO_CFG_S)))
		return -1;
	
	if(pEncodeParam->level  == 0)	// 主码流
		memcpy(&stVencCfg, &stVideoCfg.struMainVenc, sizeof(IPNC_DEV_VENC_CFG_S));
	else if(pEncodeParam->level == 1)	// 从码流
		memcpy(&stVencCfg, &stVideoCfg.struSubVenc, sizeof(IPNC_DEV_VENC_CFG_S));
	else
	{
		j_warning("The encode level don't support in TI ipc.");
		return -1;
	}

	j_debug("FrameRate: %d", pEncodeParam->frame_rate);
	j_debug("i_frame_interval: %d", pEncodeParam->i_frame_interval);
	
	// 视频参数
	stVencCfg.u8FrameRate = pEncodeParam->frame_rate;
	stVencCfg.u8Gop = pEncodeParam->i_frame_interval;

	if(pEncodeParam->video_type == J_SDK_AV_VIDEO_H264)
		stVencCfg.u8VideoEncType = 0;
	else if(pEncodeParam->video_type == J_SDK_AV_VIDEO_MJPEG)
		stVencCfg.u8VideoEncType = 1;
	else
		j_warning("The VideoEncType don't support this videoenvtype ! " );

	if(pEncodeParam->level == 1)	// 从码流,支持设置分辩率
	{
		if(pEncodeParam->resolution == J_SDK_VIDEO_D1)
		{
			stVencCfg.u16Width = 704;
			stVencCfg.u16Height = 576;
		}
		else
		{	
			stVencCfg.u16Width = 320;
			stVencCfg.u16Height = 192;		
		}
	}
	if(pEncodeParam->level == 0)
	{
		if(pEncodeParam->resolution == J_SDK_VIDEO_960P)
		{
			stVencCfg.u16Width = 1280;
			stVencCfg.u16Height = 960;
		}
		else
		{
			stVencCfg.u16Width = 1280;
			stVencCfg.u16Height = 720;
		}
	}
	stVencCfg.u8PicQuilty = pEncodeParam->qp_value;
	if(pEncodeParam->bit_rate == J_SDK_VBR)
	//位率类型又叫码率，动态码率（VBR）和固定码率（CBR）
		stVencCfg.u8FlowType = 1;
	else if(pEncodeParam->bit_rate == J_SDK_CBR)
		stVencCfg.u8FlowType = 0;

	j_debug("The code_rate: %d", pEncodeParam->code_rate);
	
	stVencCfg.u32BitrateRate = pEncodeParam->code_rate;
	stVideoCfg.u8PalStand = 1 - pEncodeParam->format;
	stDevInfo.stVarInfo.u8PalStand = 1 - pEncodeParam->format;
	
	// 音频参数
	j_debug("pEncodeParam->audio_type : %d", pEncodeParam->audio_type);
	
	if(pEncodeParam->audio_type == J_SDK_AV_AUDIO_G711A)
		stAudioCfg.u8AEncType = 1;
	else if(pEncodeParam->audio_type == J_SDK_AV_AUDIO_G711U)
		stAudioCfg.u8AEncType = 2;

	stAudioCfg.u8Type = (pEncodeParam->au_in_mode==J_SDK_AUDIO_MIC_IN) ? 0 : 1;
	stAudioCfg.u8Open = pEncodeParam->audio_enble;

	j_debug("encode level = %d", pEncodeParam->encode_level);
	j_debug("pEncodeParam->format : %d", pEncodeParam->format);
	//j_debug("stVencCfg.u8PalStand : %d", stVencCfg.u8PalStand);
	if(pEncodeParam->encode_level == J_SDK_BASELINE)
		stVideoCfg.eEncodeLevel = IPNC_H264_BASELINE;
	else if(pEncodeParam->encode_level == J_SDK_MAIN)
		stVideoCfg.eEncodeLevel = IPNC_H264_MAIN;
	else if(pEncodeParam->encode_level == J_SDK_HIGH)
		stVideoCfg.eEncodeLevel = IPNC_H264_HIGH;
	
	if(pEncodeParam->level == 0)	// 主码流
		memcpy(&stVideoCfg.struMainVenc, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S));
	else if(pEncodeParam->level == 1)	// 从码流
		memcpy(&stVideoCfg.struSubVenc, &stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S));
	else
	{
		j_warning("The encode level don't support in TI ipc.");
		return -1;
	}

	//设置
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_AUDIO_CFG, 
					  		 &stAudioCfg, sizeof(IPNC_AUDIO_CFG_S)))
		return -1;
	
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_VIDEO_CFG, 
					  		 &stVideoCfg, sizeof(IPNC_DEV_VIDEO_CFG_S)))
		return -1;
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_DEV_CFG, 
					  		&stDevInfo, sizeof(IPNC_DEV_CFG_S)))
		return -1;
	
	return 0;
}

static int nvr_jxj_get_encode_info(int id, int channel, void *param, user_t *u)
{
	JEncodeParameter *pEncodeParam = (JEncodeParameter *)param;
	IPNC_DEV_VENC_CFG_S stVencCfg = {0};
	IPNC_AUDIO_CFG_S 	stAudioCfg = {0};
	IPNC_DEV_VIDEO_CFG_S stVideoCfg = {0};
	IPNC_DEV_CFG_S stDevInfo = {{0}};

	CHANNEL_CHECK(channel);
	
	//获取视频参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_VIDEO_CFG,  
					  		&stVideoCfg, sizeof(IPNC_DEV_VIDEO_CFG_S)))
		return -1;
	//获取设备参数配置
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_DEV_CFG, 
					  		&stDevInfo, sizeof(IPNC_DEV_CFG_S)))
		return -1;

	//获取音频参数配置
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_AUDIO_CFG, 
					  		&stAudioCfg, sizeof(IPNC_AUDIO_CFG_S)))
		return -1;
	
	if(pEncodeParam->level == 0)	// 主码流
		memcpy(&stVencCfg, &stVideoCfg.struMainVenc, sizeof(IPNC_DEV_VENC_CFG_S));
	else if(pEncodeParam->level == 1)	// 从码流
		memcpy(&stVencCfg, &stVideoCfg.struSubVenc, sizeof(IPNC_DEV_VENC_CFG_S));
	else
	{
		j_warning("The encode level don't support in TI ipc.");
		return -1;
	}
	j_debug("stVideoCfg.struMainVenc.u16Width: %d", stVideoCfg.struMainVenc.u16Width);
	j_debug("stVideoCfg.struMainVenc.u16Height: %d", stVideoCfg.struMainVenc.u16Height);
	j_debug("stVideoCfg.struSubVenc.u16Width: %d", stVideoCfg.struSubVenc.u16Width);
	j_debug("stVideoCfg.struSubVenc.u16Height: %d", stVideoCfg.struSubVenc.u16Height);
	j_debug("stVencCfg.u16Width: %d", stVencCfg.u16Width);
	j_debug("stVencCfg.u16Height: %d", stVencCfg.u16Height);
	j_debug("pEncodeParam->level: %d", pEncodeParam->level);

	// 视频参数
	pEncodeParam->frame_rate 	   = stVencCfg.u8FrameRate;
	pEncodeParam->i_frame_interval = stVencCfg.u8Gop;
	if(stVencCfg.u8VideoEncType == 0)	// H264
		pEncodeParam->video_type = J_SDK_AV_VIDEO_H264;
	else if(stVencCfg.u8VideoEncType == 1)	// MJPEG
		pEncodeParam->video_type = J_SDK_AV_VIDEO_MJPEG;
	else
		pEncodeParam->video_type = J_SDK_AV_CODEC_UNKNOWN;

	j_debug("pEncodeParam->video_type: %d", pEncodeParam->video_type);

	if((stVencCfg.u16Width == 320) && (stVencCfg.u16Height == 192))
		pEncodeParam->resolution = J_SDK_VIDEO_CIF;
	else if((stVencCfg.u16Width == 704) && (stVencCfg.u16Height == 576))
		pEncodeParam->resolution = J_SDK_VIDEO_D1;
	else if((stVencCfg.u16Width == 1280) && (stVencCfg.u16Height == 720))
		pEncodeParam->resolution = J_SDK_VIDEO_720P;
	else if((stVencCfg.u16Width == 1920) && (stVencCfg.u16Height == 1080))
		pEncodeParam->resolution = J_SDK_VIDEO_1080P;
	else if((stVencCfg.u16Width == 1280) && (stVencCfg.u16Height == 960))
		pEncodeParam->resolution = J_SDK_VIDEO_960P;
	else
		pEncodeParam->resolution = J_SDK_VIDEO_720P;

	pEncodeParam->qp_value = stVencCfg.u8PicQuilty;
	pEncodeParam->code_rate = stVencCfg.u32BitrateRate;
	pEncodeParam->frame_priority = 0;
	pEncodeParam->format = 1 - stDevInfo.stVarInfo.u8PalStand;
	stVideoCfg.u8PalStand = stDevInfo.stVarInfo.u8PalStand;

	j_debug("u16Width: %d", stVencCfg.u16Width);
	j_debug("u16Height: %d", stVencCfg.u16Height);
	j_debug("pEncodeParam->resolution: %x", pEncodeParam->resolution);
	j_debug("u8Gop: %d", stVencCfg.u8Gop);
	j_debug("u8FrameRate: %d", stVencCfg.u8FrameRate);
	
	if(stVencCfg.u8FlowType == 0)	// CBR
		pEncodeParam->bit_rate = J_SDK_CBR;
	else //if(stVencCfg.u8FlowType == 1)
		pEncodeParam->bit_rate = J_SDK_VBR;

	// 音频参数
	if(stAudioCfg.u8AEncType == 1)	// G711A
		pEncodeParam->audio_type = J_SDK_AV_AUDIO_G711A;
	else if(stAudioCfg.u8AEncType == 2)	// G711U
		pEncodeParam->audio_type = J_SDK_AV_AUDIO_G711U;
	else
		pEncodeParam->audio_type = J_SDK_AV_CODEC_UNKNOWN;

	j_debug("pEncodeParam->audio_type: %d", pEncodeParam->audio_type);
	j_debug("audio sample: %u", stAudioCfg.u32SampleRate);
	j_debug("pEncodeParam->format : %d", pEncodeParam->format);
	//j_debug("stVencCfg.u8PalStand : %d", stVencCfg.u8PalStand);
	pEncodeParam->au_in_mode = (stAudioCfg.u8Type == 0)?J_SDK_AUDIO_MIC_IN : J_SDK_AUDIO_LINE_IN;
	pEncodeParam->audio_enble = stAudioCfg.u8Open;
	
	if(stVideoCfg.eEncodeLevel == IPNC_H264_HIGH)
		pEncodeParam->encode_level = J_SDK_HIGH;
	else if(stVideoCfg.eEncodeLevel == IPNC_H264_MAIN)
		pEncodeParam->encode_level = J_SDK_MAIN;
	else if(stVideoCfg.eEncodeLevel == IPNC_H264_BASELINE)
		pEncodeParam->encode_level = J_SDK_BASELINE;
	
	j_debug("encode level = %d", stVideoCfg.eEncodeLevel);
	return 0;
}


static int nvr_jxj_set_display_info(int id, int channel, void *param, user_t *u)
{
	j_debug("nvr_jxj_set_display_info...");
	JDisplayParameter *pDisplayParam = (JDisplayParameter *)param;
	IPNC_DEV_IMA_CFG_S  struImaCfg = {0};
	CHANNEL_CHECK(channel);
	
	//获取视频参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_IMA_CFG, 
					  		&struImaCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		return -1;
	struImaCfg.struContrast.u8Value 	= pDisplayParam->contrast;
	struImaCfg.struBrightness.u8Value = pDisplayParam->bright;
	struImaCfg.struHue.u8Value 		= pDisplayParam->hue;
	struImaCfg.struSaturation.u8Value = pDisplayParam->saturation;
	struImaCfg.struSharpness.u8Value 	= pDisplayParam->sharpness;
	
	//设置
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_IMA_CFG, 
					  		&struImaCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		return -1;
	
	return 0;
}

static int nvr_jxj_get_display_info(int id, int channel, void *param, user_t *u)
{
	j_debug("nvr_jxj_get_display_info...");
	JDisplayParameter *pDisplayParam = (JDisplayParameter *)param;
	IPNC_DEV_IMA_CFG_S  struImaCfg = {0};
	
	CHANNEL_CHECK(channel);
	
	//获取视频参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_IMA_CFG, 
					   		&struImaCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		return -1;
	
	pDisplayParam->contrast = struImaCfg.struContrast.u8Value;
	pDisplayParam->bright   = struImaCfg.struBrightness.u8Value;
	pDisplayParam->hue 		= struImaCfg.struHue.u8Value;
	pDisplayParam->saturation = struImaCfg.struSaturation.u8Value;
	pDisplayParam->sharpness  = struImaCfg.struSharpness.u8Value;

	return 0;
}

static int nvr_jxj_set_record_info(int id, int channel, void *param, user_t *u)
{
	int i, day, seg = 0;
	JRecordParameter *pRecordParam = (JRecordParameter *)param;
	NVR_DEV_REC_STRC_S stDevRec = {0};
	JTimeSeg *pTimeSeg;

	CHANNEL_CHECK(channel);
	if(pRecordParam->level < 0 || pRecordParam->level > 1)
	{
		j_warning("The IPC just record the main and sub stream.");
		return -1;
	}
	
	if(0 > nvr_logon_get_rec_cfg(channel, &stDevRec))
		return -1;
	//stDevRec.u32Size = sizeof(NVR_DEV_REC_STRC_S);
	stDevRec.u8RecStream = pRecordParam->level;
	stDevRec.u8PreRecSecs = pRecordParam->pre_record;
	stDevRec.u8OverWrite = pRecordParam->auto_cover;
	stDevRec.u8Enable = 1;
	stDevRec.u8RecType = 1;		// 暂定为定时录像，客户端不支持配置其它录像方式
	memset(&stDevRec.struTimerSeg, 0, sizeof(stDevRec.struTimerSeg));
	for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
	{
		day = (i + 1) % J_SDK_MAX_DAY_SIZE;
		if(pRecordParam->week.days[i].all_day_enable)
		{
			stDevRec.struTimerSeg[0][day].u8Open = 1;
			stDevRec.struTimerSeg[0][day].beginSec = 0;
			stDevRec.struTimerSeg[0][day].endSec = 24 * 3600 - 1;
		}
		else
		{
			for(seg = 0; seg < pRecordParam->week.days[i].count && seg < NVR_MAX_SEG_NUM; seg++)
			{
				pTimeSeg = &pRecordParam->week.days[i].seg[seg];
				if(pTimeSeg->time_end.hour > 23)
					continue;
				stDevRec.struTimerSeg[seg][day].u8Open = pTimeSeg->enable;
				stDevRec.struTimerSeg[seg][day].beginSec = (pTimeSeg->time_start.hour * 3600)
															+ (pTimeSeg->time_start.minute * 60)
															+ pTimeSeg->time_start.second;
				stDevRec.struTimerSeg[seg][day].endSec = (pTimeSeg->time_end.hour * 3600)
															+ (pTimeSeg->time_end.minute * 60)
															+ pTimeSeg->time_end.second;
			}
		}
	}
	if(0 > nvr_logon_set_rec_cfg(channel, &stDevRec))
		return -1;
	return 0;
}

static int nvr_jxj_get_record_info(int id, int channel, void *param, user_t *u)
{
	int i, day, seg;
	JRecordParameter *pRecordParam = (JRecordParameter *)param;
	NVR_DEV_REC_STRC_S stDevRec = {0};

	CHANNEL_CHECK(channel);
	memset(pRecordParam, 0, sizeof(JRecordParameter));
	if(0 > nvr_logon_get_rec_cfg(channel, &stDevRec))
		return -1;

	pRecordParam->level = stDevRec.u8RecStream;	
	pRecordParam->auto_cover = stDevRec.u8OverWrite;
	pRecordParam->pre_record = stDevRec.u8PreRecSecs;
	
	for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
	{
		pRecordParam->week.days[i].day_id = i;
		day = (i + 1) % J_SDK_MAX_DAY_SIZE;
		for(seg = 0; seg < NVR_MAX_SEG_NUM; seg++)
		{
			if(stDevRec.struTimerSeg[seg][day].u8Open == 0)
				continue;
			if(stDevRec.struTimerSeg[seg][day].beginSec == 0 
				&& stDevRec.struTimerSeg[seg][day].endSec == 24 * 3600 - 1)	// 全天
			{
				pRecordParam->week.days[i].count = 0;
				pRecordParam->week.days[i].all_day_enable = 1;
				memset(pRecordParam->week.days[i].seg, 0, sizeof(pRecordParam->week.days[i].seg));
				break;
				// 因为平台使用的全天时间是00:00:00 - 24:00:00
				//stDevRec.struTimerSeg[seg][day].endSec++;
			}
			pRecordParam->week.days[i].seg[seg].enable = stDevRec.struTimerSeg[seg][day].u8Open;
			pRecordParam->week.days[i].seg[seg].time_start.hour = stDevRec.struTimerSeg[seg][day].beginSec / 3600;
			pRecordParam->week.days[i].seg[seg].time_start.minute = (stDevRec.struTimerSeg[seg][day].beginSec % 3600) / 60;
			pRecordParam->week.days[i].seg[seg].time_start.second = stDevRec.struTimerSeg[seg][day].beginSec % 60;
			pRecordParam->week.days[i].seg[seg].time_end.hour = stDevRec.struTimerSeg[seg][day].endSec / 3600;
			pRecordParam->week.days[i].seg[seg].time_end.minute = (stDevRec.struTimerSeg[seg][day].endSec % 3600) / 60;
			pRecordParam->week.days[i].seg[seg].time_end.second = stDevRec.struTimerSeg[seg][day].endSec % 60;
			pRecordParam->week.days[i].count++;
		}
	}
	pRecordParam->week.count = i;
	return 0;
}

static int nvr_jxj_set_hide_info(int id, int channel, void *param, user_t *u)
{
	int area;
	JHideParameter *pHideParam = (JHideParameter *)param;
	IPNC_DEV_VMASK_CFG_S stVmaskCfg = {0};
	
	CHANNEL_CHECK(channel);
	
	//获取视频参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_VMASK_CFG,  
					  		&stVmaskCfg, sizeof(IPNC_DEV_VMASK_CFG_S)))
		return -1;

	j_debug("max_width:%d, max_height:%d", pHideParam->max_width, pHideParam->max_height);
	if(pHideParam->max_height != 576 || pHideParam->max_width != 704)
	{
		j_warning("The max height and max width is wrong.");
		return -1;
	}
	
	stVmaskCfg.u8Open = pHideParam->hide_enable; 
	if(stVmaskCfg.u8Open == 1)	
	{
		memset(&stVmaskCfg.struArea, 0, sizeof(stVmaskCfg.struArea));
		for(area = 0; area < pHideParam->hide_area.count && area < IPNC_VMASK_REG_NUM; area++)
		{
			stVmaskCfg.struArea[area].s32X = pHideParam->hide_area.rect[area].left;
			stVmaskCfg.struArea[area].s32Y = pHideParam->hide_area.rect[area].top;
			stVmaskCfg.struArea[area].u32Width = pHideParam->hide_area.rect[area].right - pHideParam->hide_area.rect[area].left;
			stVmaskCfg.struArea[area].u32Height = pHideParam->hide_area.rect[area].bottom - pHideParam->hide_area.rect[area].top;
		}
	}
	
	//设置
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_VMASK_CFG, 
					  		&stVmaskCfg, sizeof(IPNC_DEV_VMASK_CFG_S)))
		return -1;
	
	return 0;
}

static int nvr_jxj_get_hide_info(int id, int channel, void *param, user_t *u)
{
	int area;
	JHideParameter *pHideParam = (JHideParameter *)param;
	IPNC_DEV_VMASK_CFG_S stVmaskCfg = {0};

	CHANNEL_CHECK(channel);
	memset(pHideParam, 0, sizeof(JHideParameter));
	
	//获取视频参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_VMASK_CFG,  
					  		&stVmaskCfg, sizeof(IPNC_DEV_VMASK_CFG_S)))
		return -1;
	
	pHideParam->hide_color = 0;
	pHideParam->max_width = 704;
	pHideParam->max_height = 576;

	pHideParam->hide_enable = stVmaskCfg.u8Open;
	if(pHideParam->hide_enable == 0)
		pHideParam->hide_area.count = 0;
	else
	{
		pHideParam->hide_area.count = 0;
		for(area = 0; area < IPNC_VMASK_REG_NUM; area++)
		{
			if(stVmaskCfg.struArea[area].s32X== 0 &&
				stVmaskCfg.struArea[area].s32Y == 0 &&
				stVmaskCfg.struArea[area].u32Width == 0 &&
				stVmaskCfg.struArea[area].u32Height == 0)
				continue;
			pHideParam->hide_area.rect[area].left = stVmaskCfg.struArea[area].s32X;
			pHideParam->hide_area.rect[area].top = stVmaskCfg.struArea[area].s32Y;
			pHideParam->hide_area.rect[area].right = stVmaskCfg.struArea[area].s32X 
													+ stVmaskCfg.struArea[area].u32Width;
			pHideParam->hide_area.rect[area].bottom = stVmaskCfg.struArea[area].s32Y 
													+ stVmaskCfg.struArea[area].u32Height;
			pHideParam->hide_area.count++;
		}
	}
	return 0;
}

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

static int nvr_jxj_set_serail_info(int id, int channel, void *param, user_t *u)
{	
		int i;
	JSerialParameter *pSerialParam = (JSerialParameter *)param;
	NVR_DEV_COMM_CFG_S stCommCfg = {0};

	if(0 > nvr_logon_get_comm_cfg(0, &stCommCfg))
		return -1;

	for(i = 0; i < sizeof(array_uart_baudrate)/sizeof(array_uart_baudrate[0][0])/2; i++)
	{
		if(pSerialParam->baud_rate == array_uart_baudrate[i][1])
		{
			stCommCfg.u32BaudRate = array_uart_baudrate[i][0];
			break;
		}
	}
	stCommCfg.u8DataBit = pSerialParam->data_bit - 5; 
	stCommCfg.u8StopBit = pSerialParam->stop_bit - 1; 
	stCommCfg.u8Parity = pSerialParam->verify;
	
	if(0 > nvr_logon_set_comm_cfg(0, &stCommCfg))
		return -1;
	
	return 0;
}

static int nvr_jxj_get_serail_info(int id, int channel, void *param, user_t *u)
{
	JSerialParameter *pSerialParam = (JSerialParameter *)param;
	NVR_DEV_COMM_CFG_S stCommCfg = {0};

	if(0 > nvr_logon_get_comm_cfg(0, &stCommCfg))
		return -1;
	pSerialParam->serial_no = 0;
	pSerialParam->baud_rate = array_uart_baudrate[stCommCfg.u32BaudRate][1];
	pSerialParam->data_bit = stCommCfg.u8DataBit + 5;
	pSerialParam->stop_bit = stCommCfg.u8StopBit + 1;
	pSerialParam->verify = stCommCfg.u8Parity;

	return 0;
}

static int nvr_jxj_set_osd_info(int id, int channel, void *param, user_t *u)
{
	JOSDParameter *pOsdParam = (JOSDParameter *)param;
	IPNC_DEV_OSD_CFG_S  stOsdCfg = {0};
	IPNC_DEV_OSD_LABEL_CFG_S stOsdLabelCfg;
	char text[512] = {0};
	
	CHANNEL_CHECK(channel);
	
	//获取osd参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_OSD_CFG, 
					  		&stOsdCfg, sizeof(IPNC_DEV_OSD_CFG_S)))
		return -1;
	
	//时间使能 --1 显示时间osd 0--隐藏时间osd
	stOsdCfg.u8EnableTimeOsd 	= pOsdParam->time_enable;
	//时间码率显示位置
	stOsdCfg.struTimeOrg.s32X = pOsdParam->time_display_x;
	stOsdCfg.struTimeOrg.s32Y = pOsdParam->time_display_y;
	//标题使能 --1 显示标题osd 0--隐藏标题osd
	stOsdCfg.u8EnableTitleOsd = pOsdParam->text_enable;
	//标题显示位置
	stOsdCfg.struTitleOrg.s32X = pOsdParam->text_display_x;
	stOsdCfg.struTitleOrg.s32Y = pOsdParam->text_display_y;
	//时间与标题显示颜色
	if(pOsdParam->osd_invert_color)
	{
		stOsdCfg.u8OsdColor	= 4;
		stOsdCfg.u8ColorValidFlag = 1;
	}
	else
	{
		stOsdCfg.u32ChnColor = pOsdParam->text_display_color;
		stOsdCfg.u32TimeColor = pOsdParam->time_display_color;
		stOsdCfg.u8ColorValidFlag = 0;
	}
	//码率使能 --1显示码流osd  0--隐藏码流osd
	stOsdCfg.u8EnableBitrateOsd = pOsdParam->stream_enable;
	j_debug("stOsdCfg.u32TimeColor: %d, stOsdCfg.u32ChnColor: %d",  stOsdCfg.u32TimeColor, stOsdCfg.u32ChnColor);
	j_debug("text data len: %d", strlen((char*)pOsdParam->text_data));
	//显示通道字符串
	nvr_utf8_to_gb2312((char *)pOsdParam->text_data, strlen((char *)pOsdParam->text_data), text);
	text[sizeof(text)-1] = '\0';
	strncpy(stOsdCfg.szChnName, text, sizeof(stOsdCfg.szChnName)-1);
	stOsdCfg.szChnName[sizeof(stOsdCfg.szChnName)-1] = '\0';
	
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_OSD_CFG, 
					  		&stOsdCfg, sizeof(IPNC_DEV_OSD_CFG_S)))
		return -1;
	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_OSD_LABEL_CFG, 
					  		&stOsdLabelCfg, sizeof(IPNC_DEV_OSD_LABEL_CFG_S)))
		return -1;
	stOsdLabelCfg.u32EnableLabel[0] = pOsdParam->ext_osd.ext_text1_enable;
	stOsdLabelCfg.struLabelOrg[0].s32X = pOsdParam->ext_osd.ext_text1_display_x + pOsdParam->ext_osd.ext_text1_display_w;
	stOsdLabelCfg.struLabelOrg[0].s32Y = pOsdParam->ext_osd.ext_text1_display_y;	//标注的字体是右对齐的
	if(pOsdParam->osd_invert_color)
	{
		stOsdLabelCfg.u32OsdColor[0] = 4;
		stOsdLabelCfg.u32ColorValidFlag[0] = 0;
	}
	else
	{
		stOsdLabelCfg.u32LabelColor[0] = pOsdParam->text_display_color;
		stOsdLabelCfg.u32ColorValidFlag[0] = 1;
	}
	memset(text, 0, sizeof(text));
	nvr_utf8_to_gb2312((char *)pOsdParam->ext_osd.ext_text1_data, strlen((char *)pOsdParam->ext_osd.ext_text1_data), text);
	text[sizeof(text)-1] = '\0';
	strncpy(stOsdLabelCfg.szLabelName[0], text, sizeof(stOsdLabelCfg.szLabelName[0])-1);
	stOsdLabelCfg.szLabelName[0][sizeof(stOsdLabelCfg.szLabelName[0])-1] = '\0';
	
	stOsdLabelCfg.u32EnableLabel[1] = pOsdParam->ext_osd.ext_text2_enable;
	stOsdLabelCfg.struLabelOrg[1].s32X = pOsdParam->ext_osd.ext_text2_display_x + pOsdParam->ext_osd.ext_text2_display_w;
	stOsdLabelCfg.struLabelOrg[1].s32Y = pOsdParam->ext_osd.ext_text2_display_y;
	if(pOsdParam->osd_invert_color)
	{
		stOsdLabelCfg.u32OsdColor[1] = 4;
		stOsdLabelCfg.u32ColorValidFlag[1] = 0;
	}
	else
	{
		stOsdLabelCfg.u32LabelColor[1] = pOsdParam->text_display_color;
		stOsdLabelCfg.u32ColorValidFlag[1] = 1;
	}
	memset(text, 0, sizeof(text));
	nvr_utf8_to_gb2312((char *)pOsdParam->ext_osd.ext_text2_data, strlen((char *)pOsdParam->ext_osd.ext_text2_data), text);
	text[sizeof(text)-1] = '\0';
	strncpy(stOsdLabelCfg.szLabelName[1], text, sizeof(stOsdLabelCfg.szLabelName[1])-1);
	stOsdLabelCfg.szLabelName[1][sizeof(stOsdLabelCfg.szLabelName[1])-1] = '\0';
	
	
	if(0 > nvr_set_ipnc_param(channel,  IPNC_CMD_OSD_LABEL_CFG, 
					  		&stOsdLabelCfg, sizeof(IPNC_DEV_OSD_LABEL_CFG_S)))
		return -1;
	
	return 0;
}
// pOsdStr 为GB2312字符串
static int nvr_jxj_get_osd_pixel_len(const char *pOsdStr, int *width, int *height)
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
static const char gDateFlag[6][32] = {
						{"%Y-%m-%d"},
						{"%m-%d-%Y"},
						{"%d-%m-%Y"},
						{"%Y/%m/%d"},
						{"%m/%d/%Y"},
						{"%d/%m/%Y"}
					};

static const char gTimeFlag[2][16]	= {
						{"%H:%M:%S"},
						{"%r"}
					}; 

int nvr_osd_get_time_title(IPNC_TIME_S *pStTimeCfg, char *pSzTitle, int nTitleLen, IPNC_DEV_OSD_CFG_S *pOsdCfg)
{
	struct tm tmTime;
	char szFlag[32] = {0};
	
	if (pOsdCfg->u8EnableTimeOsd)
	{	
		if (pOsdCfg->u8DispWeek)
		{
			sprintf(szFlag, "%s %s %%a", gDateFlag[pOsdCfg->u8TimeOsdType],
								gTimeFlag[pOsdCfg->u8HourOsdType]);	
		}	
		else	//alex 2011-08-12 modify the bug can't display the week when not show the week day
			sprintf(szFlag, "%s %s", gDateFlag[pOsdCfg->u8TimeOsdType], gTimeFlag[pOsdCfg->u8HourOsdType]);

		if (pStTimeCfg != NULL)
		{
			tmTime.tm_year = pStTimeCfg->u8Year;
			tmTime.tm_mon = (pStTimeCfg->u8Month - 1) % 12;
			tmTime.tm_mday= pStTimeCfg->u8Day % 32;
			tmTime.tm_hour = pStTimeCfg->u8Hour % 24;			
			tmTime.tm_min  = pStTimeCfg->u8Minute % 60;		
			tmTime.tm_sec  = pStTimeCfg->u8Second % 60;
		}	
			
		strftime(pSzTitle,  nTitleLen, szFlag, &tmTime);
	}

	return 0;
}
int nvr_osd_get_bitrate_title(char *pSzTitle, int nCh, int nStreamType, IPNC_DEV_OSD_CFG_S *pOsdCfg)
{
	//uint32_t u32FrameRate = 0, u32BitRate = 0;

	if (pOsdCfg->u8EnableBitrateOsd && pSzTitle != NULL)
	{
		//ipc_avlib_video_get_bit_info(nCh, nStreamType, &u32FrameRate, &u32BitRate);
		if (strlen(pSzTitle) != 0)
		{
			sprintf(pSzTitle, "%s\nFR=%dfps BR=%dkb", pSzTitle, 30, 2000);
		}
		else
		{
			sprintf(pSzTitle, "FR=%dfps BR=%dkb", 30, 2000);
		}
	}	

	return 0;
}

static int nvr_jxj_get_osd_info(int id, int channel, void *param, user_t *u)
{
	JOSDParameter *pOsdParam = (JOSDParameter *)param;
	IPNC_DEV_OSD_CFG_S	stOsdCfg = {0};
	IPNC_DEV_VENC_CFG_S stVencCfg = {0};
	IPNC_DEV_OSD_LABEL_CFG_S stOsdLabelCfg;
	IPNC_TIME_S stTimeCfg = {0};
	char szOsdTime[128];
	int width;
	int height;
	
	CHANNEL_CHECK(channel);

	//获取OSD参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_OSD_CFG, 
					  		&stOsdCfg, sizeof(IPNC_DEV_OSD_CFG_S)))
		return -1;
	
	j_debug("time.x: %d, time.y: %d", stOsdCfg.struTimeOrg.s32X, stOsdCfg.struTimeOrg.s32Y);
	j_debug("text.x: %d, text.y: %d", stOsdCfg.struTitleOrg.s32X, stOsdCfg.struTitleOrg.s32Y);

	//时间使能 --1 显示时间osd 0--隐藏时间osd
	pOsdParam->time_enable = stOsdCfg.u8EnableTimeOsd;
	//时间码率显示位置
	pOsdParam->time_display_x = stOsdCfg.struTimeOrg.s32X;
	pOsdParam->time_display_y = stOsdCfg.struTimeOrg.s32Y;
	
	//标题使能 --1 显示标题osd 0--隐藏标题osd
	pOsdParam->text_enable = stOsdCfg.u8EnableTitleOsd;
	//标题显示位置
	pOsdParam->text_display_x = stOsdCfg.struTitleOrg.s32X;
	pOsdParam->text_display_y = stOsdCfg.struTitleOrg.s32Y;
	
	//时间与标题显示颜色
	if((stOsdCfg.u8ColorValidFlag) && (stOsdCfg.u8OsdColor == 4))
	{
		pOsdParam->time_display_color = 0;
		pOsdParam->text_display_color = 0;
		pOsdParam->osd_invert_color = 1;
	}else
	{
		pOsdParam->time_display_color = stOsdCfg.u32TimeColor;
		pOsdParam->text_display_color = stOsdCfg.u32ChnColor;
		pOsdParam->osd_invert_color = 0;
	}

	j_debug("stOsdCfg.u32TimeColor: %d, stOsdCfg.u32ChnColor: %d",  stOsdCfg.u32TimeColor, stOsdCfg.u32ChnColor);
	//码率使能 --1显示码流osd  0--隐藏码流osd
	pOsdParam->stream_enable = stOsdCfg.u8EnableBitrateOsd;
	
	memset(pOsdParam->text_data, 0, sizeof(pOsdParam->text_data));
	nvr_gb2312_to_utf8(stOsdCfg.szChnName, strlen(stOsdCfg.szChnName), (char*)pOsdParam->text_data);
	pOsdParam->text_data[sizeof(pOsdParam->text_data)-1] = 0;
	j_debug("text data len: %d", strlen((char *)pOsdParam->text_data));
	j_debug("text : %s", pOsdParam->text_data);

	//显示通道字符串
	pOsdParam->max_height = 576;
	pOsdParam->max_width = 704;
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_VMAIN_CFG, 
					  		&stVencCfg, sizeof(IPNC_DEV_VENC_CFG_S)))
		return -1;
	nvr_jxj_get_osd_pixel_len(stOsdCfg.szChnName, &width, &height);
	pOsdParam->text_display_w = width*704/stVencCfg.u16Width;
	pOsdParam->text_display_h = height*576/stVencCfg.u16Height;

	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_TIME_CFG, 
					  		&stTimeCfg, sizeof(IPNC_TIME_S)))
		return -1;
	nvr_osd_get_time_title(&stTimeCfg, szOsdTime, sizeof(szOsdTime), &stOsdCfg);
	nvr_osd_get_bitrate_title(szOsdTime, 0, 0, &stOsdCfg);
	nvr_jxj_get_osd_pixel_len(szOsdTime, &width, &height);
	pOsdParam->time_display_w = width*704/stVencCfg.u16Width;
	pOsdParam->time_display_h = height*576/stVencCfg.u16Height;


	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_OSD_LABEL_CFG, 
					  		&stOsdLabelCfg, sizeof(IPNC_DEV_OSD_LABEL_CFG_S)))
		return -1;
	pOsdParam->ext_osd.ext_text1_enable = stOsdLabelCfg.u32EnableLabel[0];
	memset(pOsdParam->ext_osd.ext_text1_data, 0, sizeof(pOsdParam->ext_osd.ext_text1_data));
	nvr_gb2312_to_utf8(stOsdLabelCfg.szLabelName[0], strlen(stOsdLabelCfg.szLabelName[0]), 
		(char*)pOsdParam->ext_osd.ext_text1_data);
	pOsdParam->ext_osd.ext_text1_data[sizeof(pOsdParam->ext_osd.ext_text1_data)-1] = 0;
	nvr_jxj_get_osd_pixel_len(stOsdLabelCfg.szLabelName[0], &width, &height);
	pOsdParam->ext_osd.ext_text1_display_w = width*704/stVencCfg.u16Width;
	pOsdParam->ext_osd.ext_text1_display_h = height*576/stVencCfg.u16Height;
	pOsdParam->ext_osd.ext_text1_display_x = stOsdLabelCfg.struLabelOrg[0].s32X - pOsdParam->ext_osd.ext_text1_display_w;
	pOsdParam->ext_osd.ext_text1_display_y = stOsdLabelCfg.struLabelOrg[0].s32Y;

	pOsdParam->ext_osd.ext_text2_enable = stOsdLabelCfg.u32EnableLabel[1];
	memset(pOsdParam->ext_osd.ext_text2_data, 0, sizeof(pOsdParam->ext_osd.ext_text2_data));
	nvr_gb2312_to_utf8(stOsdLabelCfg.szLabelName[1], strlen(stOsdLabelCfg.szLabelName[1]), 
		(char*)pOsdParam->ext_osd.ext_text2_data);
	pOsdParam->ext_osd.ext_text2_data[sizeof(pOsdParam->ext_osd.ext_text2_data)-1] = 0;
	nvr_jxj_get_osd_pixel_len(stOsdLabelCfg.szLabelName[1], &width, &height);
	pOsdParam->ext_osd.ext_text2_display_w = width*704/stVencCfg.u16Width;
	pOsdParam->ext_osd.ext_text2_display_h = height*576/stVencCfg.u16Height;
	pOsdParam->ext_osd.ext_text2_display_x = stOsdLabelCfg.struLabelOrg[1].s32X - pOsdParam->ext_osd.ext_text2_display_w;
	pOsdParam->ext_osd.ext_text2_display_y = stOsdLabelCfg.struLabelOrg[1].s32Y;
	return 0;
}

static int nvr_jxj_set_ptz_info(int id, int channel, void *param, user_t *u)
{
	int i;
	JPTZParameter *pPtzParam = (JPTZParameter *)param;
	IPNC_PTZ_CFG_S stPtzCfg = {0};
	IPNC_COMM_CFG_S stCommCfg = {0};

	if(pPtzParam->serial_no != 0)
	{
		j_warning("The IPC don't support PTZ %d.", pPtzParam->serial_no);
		return -1;
	}
	
	CHANNEL_CHECK(channel);
	//获取云台参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_PTZ_CFG,  
					  		&stPtzCfg, sizeof(IPNC_PTZ_CFG_S)))
		return -1;
	//获取COMM参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_COMM_CFG,  
					  		&stCommCfg, sizeof(IPNC_COMM_CFG_S)))
		return -1;

	//云台协议
	stPtzCfg.u8ProtocalType = (unsigned char)pPtzParam->protocol;
	//云台地址码
	stPtzCfg.u8PtzAddr = (unsigned char)pPtzParam->ptz_addr;
	
	//波特率
	for(i = 0; i < sizeof(array_uart_baudrate)/sizeof(array_uart_baudrate[0]); i++)
	{
		if(pPtzParam->baud_rate == array_uart_baudrate[i][1])
		{
			stCommCfg.u32BaudRate = array_uart_baudrate[i][0];
			break;
		}
	}
	if(i == sizeof(array_uart_baudrate)/sizeof(array_uart_baudrate[0]))
	{
		return -1;
	}
	//数据位
	stCommCfg.u8DataBit = pPtzParam->data_bit - 5;
	//停止位
	stCommCfg.u8StopBit = pPtzParam->stop_bit - 1;
	//检验位
	stCommCfg.u8Parity = pPtzParam->verify;
	
	//设置
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_COMM_CFG, 
					  		&stCommCfg, sizeof(IPNC_COMM_CFG_S)))
		return -1;
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_PTZ_CFG, 
					  		&stPtzCfg, sizeof(IPNC_PTZ_CFG_S)))
		return -1;
	
	return 0;
}

static int nvr_jxj_get_ptz_info(int id, int channel, void *param, user_t *u)
{
	JPTZParameter *pPtzParam = (JPTZParameter *)param;
	IPNC_PTZ_CFG_S stPtzCfg = {0};
	IPNC_COMM_CFG_S stCommCfg = {0};

	if(pPtzParam->serial_no != 0)
	{
		j_warning("The IPC don't support PTZ %d.", pPtzParam->serial_no);
		return -1;
	}

	CHANNEL_CHECK(channel);
	//获取云台参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_PTZ_CFG, 
					  		 &stPtzCfg, sizeof(IPNC_PTZ_CFG_S)))
		return -1;
	//获取COMM参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_COMM_CFG, 
					  		&stCommCfg, sizeof(IPNC_COMM_CFG_S)))
		return -1;
	
	//云台协议
	pPtzParam->protocol = stPtzCfg.u8ProtocalType;
	//云台地址码
	pPtzParam->ptz_addr = stPtzCfg.u8PtzAddr;

	//串口信息
	pPtzParam->baud_rate = array_uart_baudrate[stCommCfg.u32BaudRate][1];
	pPtzParam->data_bit = stCommCfg.u8DataBit + 5;
	pPtzParam->stop_bit = stCommCfg.u8StopBit + 1;
	pPtzParam->verify = stCommCfg.u8Parity;
	return 0;
}

static int nvr_jxj_set_ftp_info(int id, int channel, void *param, user_t *u)
{
	JFTPParameter *pFtpParam = (JFTPParameter *)param;
	NVR_FTP_CFG_S stFtpCfg = {0};

	if(0 > nvr_logon_get_ftp_cfg(0, &stFtpCfg))
		return -1;
		
	stFtpCfg.u32Size = sizeof(stFtpCfg);
	strncpy(stFtpCfg.szFtpSvr, (char *)pFtpParam->ftp_ip, sizeof(stFtpCfg.szFtpSvr) - 1);
	stFtpCfg.szFtpSvr[sizeof(stFtpCfg.szFtpSvr) - 1] = '\0';
	strncpy(stFtpCfg.szFtpUsr, (char *)pFtpParam->ftp_usr, sizeof(stFtpCfg.szFtpUsr) - 1);
	stFtpCfg.szFtpUsr[sizeof(stFtpCfg.szFtpUsr) - 1] = '\0';
	strncpy(stFtpCfg.szFtpPsw, (char *)pFtpParam->ftp_pwd, sizeof(stFtpCfg.szFtpPsw) - 1);
	stFtpCfg.szFtpPsw[sizeof(stFtpCfg.szFtpPsw) - 1] = '\0';
	strncpy(stFtpCfg.szFtpPath, (char *)pFtpParam->ftp_path, sizeof(stFtpCfg.szFtpPath) - 1);
	stFtpCfg.szFtpPath[sizeof(stFtpCfg.szFtpPath) - 1] = '\0';
	stFtpCfg.u16FtpPort = pFtpParam->ftp_port;
	if(0 > nvr_logon_set_ftp_cfg(0, &stFtpCfg))
		return -1;
	
	return 0;
}

static int nvr_jxj_get_ftp_info(int id, int channel, void *param, user_t *u)
{
	JFTPParameter *pFtpParam = (JFTPParameter *)param;
	NVR_FTP_CFG_S stFtpCfg = {0};

	if(0 > nvr_logon_get_ftp_cfg(0, &stFtpCfg))
		return -1;
	
	strncpy((char *)pFtpParam->ftp_ip, stFtpCfg.szFtpSvr, sizeof(pFtpParam->ftp_ip));
	pFtpParam->ftp_ip[sizeof(pFtpParam->ftp_ip) - 1] = '\0';
	strncpy((char *)pFtpParam->ftp_usr, stFtpCfg.szFtpUsr, sizeof(pFtpParam->ftp_usr));
	pFtpParam->ftp_usr[sizeof(pFtpParam->ftp_usr) - 1] = '\0';
	strncpy((char *)pFtpParam->ftp_pwd, stFtpCfg.szFtpPsw, sizeof(pFtpParam->ftp_pwd));
	pFtpParam->ftp_pwd[sizeof(pFtpParam->ftp_pwd) - 1] = '\0';
	strncpy((char *)pFtpParam->ftp_path, stFtpCfg.szFtpPath, sizeof(pFtpParam->ftp_path));
	pFtpParam->ftp_path[sizeof(pFtpParam->ftp_path) - 1] = '\0';
	pFtpParam->ftp_port = stFtpCfg.u16FtpPort;

	return 0;
}

static int nvr_jxj_set_smtp_info(int id, int channel, void *param, user_t *u)
{
	JSMTPParameter *pSmtpParam = (JSMTPParameter *)param;
	NVR_SMTP_CFG_S stSmtpCfg = {0};

	if(0 > nvr_logon_get_smtp_cfg(0, &stSmtpCfg))
		return -1;
	
	strncpy(stSmtpCfg.szMailHub, (char *)pSmtpParam->mail_ip, sizeof(stSmtpCfg.szMailHub));
	stSmtpCfg.szMailHub[sizeof(stSmtpCfg.szMailHub) - 1] = '\0';
	strncpy(stSmtpCfg.szMailAddr, (char *)pSmtpParam->mail_addr, sizeof(stSmtpCfg.szMailAddr));
	stSmtpCfg.szMailAddr[sizeof(stSmtpCfg.szMailAddr) - 1] = '\0';
	strncpy(stSmtpCfg.szMailUsr, (char *)pSmtpParam->mail_usr, sizeof(stSmtpCfg.szMailUsr));
	stSmtpCfg.szMailUsr[sizeof(stSmtpCfg.szMailUsr) - 1] = '\0';
	strncpy(stSmtpCfg.szMailPsw, (char *)pSmtpParam->mail_pwd, sizeof(stSmtpCfg.szMailPsw));
	stSmtpCfg.szMailPsw[sizeof(stSmtpCfg.szMailPsw) - 1] = '\0';
	strncpy(stSmtpCfg.szMailRctp1, (char *)pSmtpParam->mail_rctp1, sizeof(stSmtpCfg.szMailRctp1));
	stSmtpCfg.szMailRctp1[sizeof(stSmtpCfg.szMailRctp1) - 1] = '\0';
	strncpy(stSmtpCfg.szMailRctp2, (char *)pSmtpParam->mail_rctp2, sizeof(stSmtpCfg.szMailRctp2));
	stSmtpCfg.szMailRctp2[sizeof(stSmtpCfg.szMailRctp2) - 1] = '\0';
	strncpy(stSmtpCfg.szMailRctp3, (char *)pSmtpParam->mail_rctp3, sizeof(stSmtpCfg.szMailRctp3));
	stSmtpCfg.szMailRctp3[sizeof(stSmtpCfg.szMailRctp3) - 1] = '\0';
	stSmtpCfg.u16MailPort = pSmtpParam->mail_port;
	stSmtpCfg.u8EnableSSL = pSmtpParam->ssl_enable;
	
	if(0 > nvr_logon_set_smtp_cfg(0, &stSmtpCfg))
		return -1;
		
	return 0;
}

static int nvr_jxj_get_smtp_info(int id, int channel, void *param, user_t *u)
{
	JSMTPParameter *pSmtpParam = (JSMTPParameter *)param;
	NVR_SMTP_CFG_S stSmtpCfg = {0};

	if(0 > nvr_logon_get_smtp_cfg(0, &stSmtpCfg))
		return -1;
		
	strncpy((char *)pSmtpParam->mail_ip, stSmtpCfg.szMailHub, sizeof(pSmtpParam->mail_ip));
	pSmtpParam->mail_ip[sizeof(pSmtpParam->mail_ip) - 1] = '\0';
	strncpy((char *)pSmtpParam->mail_addr, stSmtpCfg.szMailAddr, sizeof(pSmtpParam->mail_addr));
	pSmtpParam->mail_addr[sizeof(pSmtpParam->mail_addr) - 1] = '\0';
	strncpy((char *)pSmtpParam->mail_usr, stSmtpCfg.szMailUsr, sizeof(pSmtpParam->mail_usr));
	pSmtpParam->mail_usr[sizeof(pSmtpParam->mail_usr) - 1] = '\0';
	strncpy((char *)pSmtpParam->mail_pwd, stSmtpCfg.szMailPsw, sizeof(pSmtpParam->mail_pwd));
	pSmtpParam->mail_pwd[sizeof(pSmtpParam->mail_pwd) - 1] = '\0';
	strncpy((char *)pSmtpParam->mail_rctp1, stSmtpCfg.szMailRctp1, sizeof(pSmtpParam->mail_rctp1));
	pSmtpParam->mail_rctp1[sizeof(pSmtpParam->mail_rctp1) - 1] = '\0';
	strncpy((char *)pSmtpParam->mail_rctp2, stSmtpCfg.szMailRctp2, sizeof(pSmtpParam->mail_rctp2));
	pSmtpParam->mail_rctp2[sizeof(pSmtpParam->mail_rctp2) - 1] = '\0';
	strncpy((char *)pSmtpParam->mail_rctp3, stSmtpCfg.szMailRctp3, sizeof(pSmtpParam->mail_rctp3));
	pSmtpParam->mail_rctp3[sizeof(pSmtpParam->mail_rctp3) - 1] = '\0';
	pSmtpParam->mail_port = stSmtpCfg.u16MailPort;
	pSmtpParam->ssl_enable = stSmtpCfg.u8EnableSSL;
	return 0;
}

static int nvr_jxj_set_upnp_info(int id, int channel, void *param, user_t *u)
{
	JUPNPParameter *pUpnpParam = (JUPNPParameter *)param;
	NVR_UPNP_CFG_S stUpnpCfg = {0};
	NVR_DEV_NET_CFG_S stNetCfg;
	
	if(0 > nvr_logon_get_upnp_cfg(0, &stUpnpCfg))
		return -1;
	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
		return -1;
	strncpy(stUpnpCfg.szUpnpSvr, (char *)pUpnpParam->upnp_ip, sizeof(stUpnpCfg.szUpnpSvr)-1);
	stUpnpCfg.szUpnpSvr[sizeof(stUpnpCfg.szUpnpSvr) - 1] = '\0';
	stUpnpCfg.u32Size = sizeof(stUpnpCfg);
	stUpnpCfg.u8UpnpOpen = pUpnpParam->upnp_enable;
	stUpnpCfg.u8UpnpEthNo = pUpnpParam->upnp_eth_no;
	stUpnpCfg.u8UpnpModel = pUpnpParam->upnp_model;
	stUpnpCfg.u8UpnpRefTime = pUpnpParam->upnp_refresh_time;
	stNetCfg.u16JXJDataPort = pUpnpParam->upnp_data_port;
	stNetCfg.u16CmdPort = pUpnpParam->upnp_cmd_port;
	stNetCfg.u16TalkPort = pUpnpParam->upnp_talk_port;

	j_debug("the upnp_enable is %d.", pUpnpParam->upnp_enable);
	j_debug("the upnp_eth_no is %d.", pUpnpParam->upnp_eth_no);
	j_debug("the upnp_model is %d.", pUpnpParam->upnp_model);
	j_debug("the upnp_data_port is %d.", pUpnpParam->upnp_data_port);
	j_debug("the upnp_web_port is %d.", pUpnpParam->upnp_web_port);
	j_debug("the upnp_data_port_result is %d.", pUpnpParam->upnp_data_port_result);
	j_debug("the upnp_web_port_result is %d.", pUpnpParam->upnp_web_port_result);

	/*
	if(stUpnpCfg.u8UpnpModel)
	{
		stUpnpCfg.u16UpnpJXJDataPort = pUpnpParam->upnp_data_port;
		stUpnpCfg.u16UpnpDataPort= pUpnpParam->upnp_data_port;
		stUpnpCfg.u16UpnpWebPort = pUpnpParam->upnp_web_port;
		stUpnpCfg.u16UpnpCmdPort = pUpnpParam->upnp_cmd_port;
		stUpnpCfg.u16UpnpTalkPort = pUpnpParam->upnp_talk_port;
	}
	else
	{
		stUpnpCfg.u16UpnpJXJDataPort = pUpnpParam->upnp_data_port_result;
		stUpnpCfg.u16UpnpDataPort= pUpnpParam->upnp_data_port_result;
		stUpnpCfg.u16UpnpWebPort = pUpnpParam->upnp_web_port_result;
		stUpnpCfg.u16UpnpCmdPort = pUpnpParam->upnp_cmd_port_result;
		stUpnpCfg.u16UpnpTalkPort = pUpnpParam->upnp_talk_port_result;
	}*/

	stUpnpCfg.u16UpnpJXJDataPort = pUpnpParam->upnp_data_port;
	stUpnpCfg.u16UpnpDataPort= pUpnpParam->upnp_data_port;
	stUpnpCfg.u16UpnpWebPort = pUpnpParam->upnp_web_port;
	stUpnpCfg.u16UpnpCmdPort = pUpnpParam->upnp_cmd_port;
	stUpnpCfg.u16UpnpTalkPort = pUpnpParam->upnp_talk_port;
	
	j_debug("the upnp_cmd_port is %d.", pUpnpParam->upnp_cmd_port);
	j_debug("the upnp_talk_port is %d.", pUpnpParam->upnp_talk_port);
	j_debug("the upnp_cmd_port_result is %d.", pUpnpParam->upnp_cmd_port_result);
	j_debug("the upnp_talk_port_result is %d.", pUpnpParam->upnp_talk_port_result);

	if(0 > nvr_logon_set_upnp_cfg(0, &stUpnpCfg))
		return -1;
	if(0 > nvr_logon_set_net_cfg(0, &stNetCfg))
		return -1;
	return 0;
}

static int nvr_jxj_get_upnp_info(int id, int channel, void *param, user_t *u)
{
	JUPNPParameter *pUpnpParam = (JUPNPParameter *)param;
	NVR_UPNP_CFG_S stUpnpCfg = {0};
	NVR_DEV_NET_CFG_S stNetCfg;
	
	if(0 > nvr_logon_get_upnp_cfg(0, &stUpnpCfg))
		return -1;
	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
		return -1;

	pUpnpParam->upnp_enable = stUpnpCfg.u8UpnpOpen;
	pUpnpParam->upnp_eth_no = stUpnpCfg.u8UpnpEthNo;
	pUpnpParam->upnp_model = stUpnpCfg.u8UpnpModel;
	pUpnpParam->upnp_refresh_time = stUpnpCfg.u8UpnpRefTime;
	pUpnpParam->upnp_data_port = stNetCfg.u16JXJDataPort;
	pUpnpParam->upnp_web_port = stNetCfg.u16HttpPort;
	pUpnpParam->upnp_data_port_result = stUpnpCfg.u16UpnpJXJDataPortS;
	pUpnpParam->upnp_web_port_result = stUpnpCfg.u16UpnpWebPortS;
	
	pUpnpParam->upnp_cmd_port = stNetCfg.u16CmdPort;
	pUpnpParam->upnp_talk_port = stNetCfg.u16TalkPort;
	pUpnpParam->upnp_cmd_port_result = stUpnpCfg.u16UpnpCmdPortS;
	pUpnpParam->upnp_talk_port_result = stUpnpCfg.u16UpnpTalkPortS;
	strncpy((char *)pUpnpParam->upnp_ip, stUpnpCfg.szUpnpSvr, sizeof(pUpnpParam->upnp_ip));
	pUpnpParam->upnp_ip[sizeof(pUpnpParam->upnp_ip)-1] = '\0';

	j_debug("the upnp_data_port is %d.", pUpnpParam->upnp_data_port);
	j_debug("the upnp_web_port is %d.", pUpnpParam->upnp_web_port);
	j_debug("the upnp_cmd_port is %d.", pUpnpParam->upnp_cmd_port);
	j_debug("the upnp_talk_port is %d.", pUpnpParam->upnp_talk_port);
	
	return 0;
}

static int nvr_jxj_set_disk_info(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_disk_info(int id, int channel, void *param, user_t *u)
{
	int i;
	JDeviceDiskInfo *pDiskInfo = (JDeviceDiskInfo *)param;
	StorageDisksInfo stDisksInfo;

	if(storage_lib_get_disk_info(&stDisksInfo) < 0)
	{
		j_warning("Get disk info failed");
		return -1;
	}

	pDiskInfo->disk_num = stDisksInfo.disk_num;
	for(i = 0; i < stDisksInfo.disk_num && i < J_SDK_MAX_DISK_NUMBER; i++)
	{
		pDiskInfo->disk[i].disk_no = stDisksInfo.disk_info[i].disk_idx;
		pDiskInfo->disk[i].total_size = stDisksInfo.disk_info[i].total_size;
		pDiskInfo->disk[i].free_size = stDisksInfo.disk_info[i].free_size;
		pDiskInfo->disk[i].is_backup = 0;
		if(stDisksInfo.disk_info[i].disk_status == DISK_USEING)
			pDiskInfo->disk[i].status = J_SDK_USING; 
		else if(stDisksInfo.disk_info[i].disk_status == DISK_MOUNTED)
			pDiskInfo->disk[i].status = J_SDK_MOUNTED; 
		else /* if(stDisksInfo.disk_info[i].disk_status == DISK_NO_FORMATED) */
			pDiskInfo->disk[i].status = J_SDK_UNMOUNT;
		pDiskInfo->disk[i].disk_type = J_SDK_SATA;
		pDiskInfo->disk[i].sys_file_type = J_SDK_EXT3;
	}
	return 0;
}
/*
static void * nvr_get_format_percent(void *arg)
{
	return 0;
}*/
static gint gIsFormating = 0;
static gpointer nvr_jxj_format_func(gpointer data)
{
	JFormatDisk pFormatDisk;
	int disk_no = GPOINTER_TO_INT(data);
	int percent;
	
	pFormatDisk.disk_no = disk_no;
	pFormatDisk.progress = 0;
	while(pFormatDisk.progress < 100)
	{
		if(storage_lib_format_get_percent(&percent) < 0)
		{	
			pFormatDisk.progress = -1;
			j_dev_sdk_submit_param(PARAM_FORMAT_DISK, 0, (void*)&pFormatDisk);
			break;
		}
		pFormatDisk.progress = percent;
		j_dev_sdk_submit_param(PARAM_FORMAT_DISK, 0, (void*)&pFormatDisk);	
		sleep(1);
	}
	storage_lib_format_disk_stop(disk_no);
	g_thread_unref(g_thread_self());
	g_atomic_int_set(&gIsFormating, 0);
	return NULL;
}

static int nvr_jxj_set_format_disk(int id, int channel, void *param, user_t *u)
{
	JFormatDisk *pDisk = (JFormatDisk *)param;
	GThread *thread;
	
	if(!g_atomic_int_compare_and_exchange(&gIsFormating, 0, 1))
		return -1;
	if(storage_lib_format_disk_start(pDisk->disk_no, 0) < 0)
	{
		g_atomic_int_set(&gIsFormating, 0);
		return -1;
	}
	thread = g_thread_new("JXJFormat", nvr_jxj_format_func, GINT_TO_POINTER(pDisk->disk_no));
	g_thread_unref(thread);
	
	return 0;
}

static int nvr_jxj_get_format_disk(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_move_alarm(int id, int channel, void *param, user_t *u)
{
	int i, rect, day, seg = 0;
	JMoveAlarm *pMoveAlarm = (JMoveAlarm *)param;
	IPNC_DEV_MD_CFG_S stMdCfg;
	JTimeSeg *pSeg;
	
	CHANNEL_CHECK(channel);
	//获取移动侦测参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_VMD_CFG,  
						  	&stMdCfg, sizeof(IPNC_DEV_MD_CFG_S)))
			return -1;
	
	if(pMoveAlarm->max_width != 704 || pMoveAlarm->max_height != 576)
	{
		j_warning("The max height and max width is wrong.");
		return -1;
	}
	stMdCfg.u32Size = sizeof(stMdCfg);
	stMdCfg.u8Enable = pMoveAlarm->move_enable;
	stMdCfg.u8Level = pMoveAlarm->sensitive_level;
	stMdCfg.u32DetectInterval = pMoveAlarm->detect_interval;
	j_debug("sensitive level: %d", pMoveAlarm->sensitive_level);
	j_debug("max_width: %d, max_height: %d", pMoveAlarm->max_width, pMoveAlarm->max_height);
	memset(&stMdCfg.struRect, 0, sizeof(stMdCfg.struRect));
	for(rect = 0; rect < pMoveAlarm->detect_area.count && rect < IPNC_VMD_REG_NUM; rect++)
	{
		stMdCfg.struRect[rect].s32X = pMoveAlarm->detect_area.rect[rect].left;
		stMdCfg.struRect[rect].s32Y = pMoveAlarm->detect_area.rect[rect].top;
		stMdCfg.struRect[rect].u32Width = pMoveAlarm->detect_area.rect[rect].right -
										pMoveAlarm->detect_area.rect[rect].left;
		stMdCfg.struRect[rect].u32Height = pMoveAlarm->detect_area.rect[rect].bottom -
										pMoveAlarm->detect_area.rect[rect].top;
		j_debug("left: %d, right: %d, top: %d, bottom: %d", pMoveAlarm->detect_area.rect[rect].left,
												pMoveAlarm->detect_area.rect[rect].right,
												pMoveAlarm->detect_area.rect[rect].top,
												pMoveAlarm->detect_area.rect[rect].bottom);
		j_debug("ipc: left: %d, right: %d, top: %d, bottom: %d", 	stMdCfg.struRect[rect].s32X,
													stMdCfg.struRect[rect].s32Y,
													stMdCfg.struRect[rect].u32Width,
													stMdCfg.struRect[rect].u32Height);
	}
	
	nvr_jxj_display_week(&pMoveAlarm->week);
	memset(&stMdCfg.struSeg, 0, sizeof(stMdCfg.struSeg));
	for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
	{
		day = (i + 1) % J_SDK_MAX_DAY_SIZE;
		if(pMoveAlarm->week.days[i].all_day_enable)
		{
			stMdCfg.struSeg[0][day].u8Open = 1;
			stMdCfg.struSeg[0][day].beginSec = 0;
			stMdCfg.struSeg[0][day].endSec = 24 * 3600 - 1;
		}
		else
		{
			for(seg = 0; seg < pMoveAlarm->week.days[i].count && seg < IPNC_MAX_SEG_NUM; seg++)
			{
				pSeg = &pMoveAlarm->week.days[i].seg[seg];
				if(pSeg->time_end.hour > 23)
					continue;
				stMdCfg.struSeg[seg][day].u8Open = pSeg->enable;
				stMdCfg.struSeg[seg][day].beginSec = (pSeg->time_start.hour * 3600) +
														(pSeg->time_start.minute * 60) +
														pSeg->time_start.second;
				stMdCfg.struSeg[seg][day].endSec = (pSeg->time_end.hour * 3600) +
														(pSeg->time_end.minute * 60) +
														pSeg->time_end.second;
			}
		}
	}
	//设置
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_VMD_CFG, 
						  	&stMdCfg, sizeof(IPNC_DEV_MD_CFG_S)))
			return -1;
	
	return 0;
}

static int nvr_jxj_get_move_alarm(int id, int channel, void *param, user_t *u)
{
	int i, rect, day, seg;
	JMoveAlarm *pMoveAlarm = (JMoveAlarm *)param;
	IPNC_DEV_MD_CFG_S stMdCfg = {0};
	JTimeSeg *pSeg;

	CHANNEL_CHECK(channel);
	//获取移动侦测参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_VMD_CFG, 
						  	&stMdCfg, sizeof(IPNC_DEV_MD_CFG_S)))
			return -1;
	
	memset(pMoveAlarm, 0, sizeof(JMoveAlarm));
	pMoveAlarm->move_enable = stMdCfg.u8Enable;
	pMoveAlarm->sensitive_level = stMdCfg.u8Level;
	pMoveAlarm->detect_interval = stMdCfg.u32DetectInterval;
	pMoveAlarm->max_width = 704;
	pMoveAlarm->max_height = 576;
	for(rect = 0; rect < J_SDK_MAX_DECTION_AREA_SIEZ && rect < IPNC_VMD_REG_NUM; rect++)
	{
		if(stMdCfg.struRect[rect].s32X == 0 && stMdCfg.struRect[rect].s32Y == 0 &&
			stMdCfg.struRect[rect].u32Height == 0 && stMdCfg.struRect[rect].u32Width == 0)
			continue;
		pMoveAlarm->detect_area.rect[rect].left = stMdCfg.struRect[rect].s32X;
		pMoveAlarm->detect_area.rect[rect].top = stMdCfg.struRect[rect].s32Y;
		pMoveAlarm->detect_area.rect[rect].right = stMdCfg.struRect[rect].s32X + 
													stMdCfg.struRect[rect].u32Width;
		pMoveAlarm->detect_area.rect[rect].bottom = stMdCfg.struRect[rect].s32Y + 
													stMdCfg.struRect[rect].u32Height;
		pMoveAlarm->detect_area.count++;
	}

	for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
	{
		pMoveAlarm->week.days[i].day_id = i;
		day = (i + 1) % J_SDK_MAX_DAY_SIZE;
		for(seg = 0; seg < IPNC_MAX_SEG_NUM; seg++)
		{
			if(stMdCfg.struSeg[seg][day].u8Open == 0)
				continue;
			if(stMdCfg.struSeg[seg][day].beginSec == 0 
				&& stMdCfg.struSeg[seg][day].endSec == 24 * 3600 - 1)	// 全天
			{
				pMoveAlarm->week.days[i].all_day_enable = 1;
				// 因为平台使用的全天时间是00:00:00 - 24:00:00
				stMdCfg.struSeg[seg][day].endSec++;
			}
			pSeg = &pMoveAlarm->week.days[i].seg[seg];
			pSeg->enable = stMdCfg.struSeg[seg][day].u8Open;
			pSeg->time_start.hour = stMdCfg.struSeg[seg][day].beginSec / 3600;
			pSeg->time_start.minute = (stMdCfg.struSeg[seg][day].beginSec % 3600) / 60;
			pSeg->time_start.second = stMdCfg.struSeg[seg][day].beginSec % 60;
			pSeg->time_end.hour = stMdCfg.struSeg[seg][day].endSec / 3600;
			pSeg->time_end.minute = (stMdCfg.struSeg[seg][day].endSec % 3600) / 60;
			pSeg->time_end.second = stMdCfg.struSeg[seg][day].endSec % 60;
			pMoveAlarm->week.days[i].count++;
		}
	}
	pMoveAlarm->week.count = i;
	nvr_jxj_display_week(&pMoveAlarm->week);
	
	return 0;
}

static int nvr_jxj_set_lost_alarm(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_lost_alarm(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_hide_alarm(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_hide_alarm(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_io_alarm(int id, int channel, void *param, user_t *u)
{
	int i, day, seg = 0;
	JIoAlarm *pIoAlarm = (JIoAlarm *)param;
	IPNC_DEV_DI_CFG_S stDiCfg = {0};
	JTimeSeg *pSeg;

	CHANNEL_CHECK(channel);

	j_debug(">>>>>>>>>>channel: %d", channel);
	
	//获取DI参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_DI_CFG, 
					   		&stDiCfg, sizeof(IPNC_DEV_DI_CFG_S)))
		return -1;

	stDiCfg.u32Size = sizeof(stDiCfg);
	//是否开启di报警检测 0--不开启 
	stDiCfg.u8Enable = pIoAlarm->alarm_enable;
	//0--常开探头1--常闭探头
	stDiCfg.u8DiType = (pIoAlarm->io_type == NORMAL_OPEN) ? 0 : 1;
	//检测间隔时间
	stDiCfg.u32DetectInterval = pIoAlarm->detect_interval;
	j_debug(">>>>>>>>>>stDiCfg.u32DetectInterval: %d", stDiCfg.u32DetectInterval);
	j_debug(">>>>>>>>>>pIoAlarm->detect_interval: %d", pIoAlarm->detect_interval);
	nvr_jxj_display_week(&pIoAlarm->week);
	//布防时段
	memset(&stDiCfg.struSeg, 0, sizeof(stDiCfg.struSeg));
	for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
	{
		day = (i + 1) % J_SDK_MAX_DAY_SIZE;
		if(pIoAlarm->week.days[i].all_day_enable)
		{
			stDiCfg.struSeg[0][day].u8Open = 1;
			stDiCfg.struSeg[0][day].beginSec = 0;
			stDiCfg.struSeg[0][day].endSec = 24 * 3600 - 1;
		}
		else
		{
			for(seg = 0; seg < pIoAlarm->week.days[i].count && seg < IPNC_MAX_SEG_NUM; seg++)
			{
				pSeg = &pIoAlarm->week.days[i].seg[seg];
				if(pSeg->time_end.hour > 23)
					continue;
				stDiCfg.struSeg[seg][day].u8Open = pSeg->enable;
				stDiCfg.struSeg[seg][day].beginSec = (pSeg->time_start.hour * 3600) +
														(pSeg->time_start.minute * 60) +
														pSeg->time_start.second;
				stDiCfg.struSeg[seg][day].endSec = (pSeg->time_end.hour * 3600) +
														(pSeg->time_end.minute * 60) +
														pSeg->time_end.second;
			}
		}
	}

	//设置
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_DI_CFG, 
					 		&stDiCfg, sizeof(IPNC_DEV_DI_CFG_S)))
		return -1;
	return 0;
}

static int nvr_jxj_get_io_alarm(int id, int channel, void *param, user_t *u)
{
	int i, day, seg;
	JIoAlarm *pIoAlarm = (JIoAlarm *)param;
	IPNC_DEV_DI_CFG_S stDiCfg = {0};
	JTimeSeg *pSeg;

	memset(pIoAlarm, 0, sizeof(JIoAlarm));
	
	CHANNEL_CHECK(channel);
	//获取DI参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_DI_CFG, 
					   		&stDiCfg, sizeof(IPNC_DEV_DI_CFG_S)))
		return -1;

	//是否开启di报警检测 0--不开启 
	pIoAlarm->io_type = (stDiCfg.u8DiType == 0) ? NORMAL_OPEN : NORMAL_CLOSE;
	//0--常开探头1--常闭探头
	pIoAlarm->alarm_enable = stDiCfg.u8Enable;
	//检测间隔时间
	pIoAlarm->detect_interval = stDiCfg.u32DetectInterval;
	//布防时段
	for(i = 0; i < J_SDK_MAX_DAY_SIZE; i++)
	{
		pIoAlarm->week.days[i].day_id = i;
		day = (i + 1) % J_SDK_MAX_DAY_SIZE;
		for(seg = 0; seg < IPNC_MAX_SEG_NUM; seg++)
		{
			if(stDiCfg.struSeg[seg][day].u8Open == 0)
				continue;
			if(stDiCfg.struSeg[seg][day].beginSec == 0 
				&& stDiCfg.struSeg[seg][day].endSec == 24 * 3600 - 1)	// 全天
			{
				pIoAlarm->week.days[i].all_day_enable = 1;
				// 因为平台使用的全天时间是00:00:00 - 24:00:00
				stDiCfg.struSeg[seg][day].endSec++;
			}
			pSeg = &pIoAlarm->week.days[i].seg[seg];
			pSeg->enable = !!stDiCfg.struSeg[seg][day].u8Open;
			pSeg->time_start.hour = stDiCfg.struSeg[seg][day].beginSec / 3600;
			pSeg->time_start.minute = (stDiCfg.struSeg[seg][day].beginSec % 3600) / 60;
			pSeg->time_start.second = stDiCfg.struSeg[seg][day].beginSec % 60;
			pSeg->time_end.hour = stDiCfg.struSeg[seg][day].endSec / 3600;
			pSeg->time_end.minute = (stDiCfg.struSeg[seg][day].endSec % 3600) / 60;
			pSeg->time_end.second = stDiCfg.struSeg[seg][day].endSec % 60;
			pIoAlarm->week.days[i].count++;
		}
	}
	pIoAlarm->week.count = i;
	nvr_jxj_display_week(&pIoAlarm->week);
	
	return 0;
}

static int nvr_jxj_set_joint_info(int id, int channel, void *param, user_t *u)
{
	JJointAction *pJointAction = (JJointAction *)param;

	CHANNEL_CHECK(channel);
	//j_debug(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>chn: %d", channel);
	//j_debug("pJointAction->alarm_type: %d", pJointAction->alarm_type);
	if(pJointAction->alarm_type == J_SDK_MOVE_ALARM)
	{
		IPNC_DEV_MD_CFG_S stMdCfg = {0};
	
		//获取移动侦测参数配置	
		if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_VMD_CFG,  
						  		 &stMdCfg, sizeof(IPNC_DEV_MD_CFG_S)))
			return -1;
		
		//录像通道(bit) (0 不录像 1第一通道录像)
		stMdCfg.struLinkAct.u32RecChannel = pJointAction->joint.joint_record_enable_channel;
		//联动输出(bit)
		stMdCfg.struLinkAct.u32OutPut = pJointAction->joint.joint_output_enable_channel;
		//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
		stMdCfg.struLinkAct.u32ShootChannel = pJointAction->joint.joint_snap_enable_channel;
		//录像时间(秒)
		stMdCfg.struLinkAct.u32RecTime = pJointAction->joint.joint_record_second;
		//stMdCfg.struLinkAct.u32OutPut = pJointAction->joint.joint_beep_enable;
		//stMdCfg.struLinkAct.u32OutPersist = pJointAction->joint.joint_beep_second;
		//输出持续时间
		stMdCfg.struLinkAct.u32OutPersist = pJointAction->joint.joint_output_times;
		//抓拍间隔(毫秒),0:连续抓拍
		stMdCfg.struLinkAct.u32ShootInterval = pJointAction->joint.joint_snap_interval * 1000;
		//抓拍张数	
		stMdCfg.struLinkAct.u8ShootNum = pJointAction->joint.joint_snap_times;
		//联动email
		stMdCfg.struLinkAct.u8Email = pJointAction->joint.joint_email_enable;
		
		//设置
		if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_VMD_CFG,
						   		&stMdCfg, sizeof(IPNC_DEV_MD_CFG_S)))
			return -1;
	}
	else if(pJointAction->alarm_type == J_SDK_LOST_ALARM)
	{
		return -1;
	}
	else if(pJointAction->alarm_type == J_SDK_HIDE_ALARM)
	{
		return -1;
	}
	else if(pJointAction->alarm_type == J_SDK_IO_ALARM)
	{
		IPNC_DEV_DI_CFG_S stDiCfg = {0};
		//获取DI参数配置	
		if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_DI_CFG,
						   		 &stDiCfg, sizeof(IPNC_DEV_DI_CFG_S)))
			return -1;
		
		//录像通道(bit) (0 不录像 1第一通道录像)
		stDiCfg.struLinkAct.u32RecChannel = pJointAction->joint.joint_record_enable_channel;
		//联动输出(bit)
		stDiCfg.struLinkAct.u32OutPut = pJointAction->joint.joint_output_enable_channel;
		//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
		stDiCfg.struLinkAct.u32ShootChannel = pJointAction->joint.joint_snap_enable_channel;
		//录像时间(秒)
		stDiCfg.struLinkAct.u32RecTime = pJointAction->joint.joint_record_second;
		//stDiCfg.struLinkAct.u32OutPut = pJointAction->joint.joint_beep_enable;
		//stDiCfg.struLinkAct.u32OutPersist = pJointAction->joint.joint_beep_second;
		//输出持续时间
		stDiCfg.struLinkAct.u32OutPersist = pJointAction->joint.joint_output_times;
		//抓拍间隔(毫秒),0:连续抓拍
		stDiCfg.struLinkAct.u32ShootInterval = pJointAction->joint.joint_snap_interval * 1000;
		//抓拍张数	
		stDiCfg.struLinkAct.u8ShootNum = pJointAction->joint.joint_snap_times;//0~5
		//联动email
		stDiCfg.struLinkAct.u8Email = pJointAction->joint.joint_email_enable;

		j_debug("joint_output_enable_channel = %d", pJointAction->joint.joint_output_enable_channel);
		j_debug("joint_beep_enable = %d", pJointAction->joint.joint_beep_enable);

		//设置
		if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_DI_CFG, 
						  		&stDiCfg, sizeof(IPNC_DEV_DI_CFG_S)))
			return -1;
	}
	else if(pJointAction->alarm_type == J_SDK_AVD_ALARM)
		return -1;
	else
		return -1;

	return 0;
}

static int nvr_jxj_get_joint_info(int id, int channel, void *param, user_t *u)
{
	JJointAction *pJointAction = (JJointAction *)param;

	CHANNEL_CHECK(channel);
	//j_debug(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>chn: %d", channel);
	if(pJointAction->alarm_type == J_SDK_MOVE_ALARM)
	{
		IPNC_DEV_MD_CFG_S stMdCfg = {0};
	
		//获取移动侦测参数配置	
		if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_VMD_CFG,
						   		 &stMdCfg, sizeof(IPNC_DEV_MD_CFG_S)))
			return -1;
		
		//录像通道(bit) (0 不录像 1第一通道录像)
		pJointAction->joint.joint_record_enable_channel = stMdCfg.struLinkAct.u32RecChannel;
		//联动输出(bit)
		pJointAction->joint.joint_output_enable_channel = stMdCfg.struLinkAct.u32OutPut;
		//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
		pJointAction->joint.joint_snap_enable_channel = stMdCfg.struLinkAct.u32ShootChannel;
		//录像时间(秒)
		pJointAction->joint.joint_record_second = stMdCfg.struLinkAct.u32RecTime;
		//pJointAction->joint.joint_beep_enable = stMdCfg.struLinkAct.u32OutPut;
		//pJointAction->joint.joint_beep_second = stMdCfg.struLinkAct.u32OutPersist;
		//输出持续时间
		pJointAction->joint.joint_output_times = stMdCfg.struLinkAct.u32OutPersist;
		//抓拍间隔(毫秒),0:连续抓拍
		pJointAction->joint.joint_snap_interval = stMdCfg.struLinkAct.u32ShootInterval/1000;
		//抓拍张数	
		pJointAction->joint.joint_snap_times = stMdCfg.struLinkAct.u8ShootNum;
		//联动email
		pJointAction->joint.joint_email_enable = stMdCfg.struLinkAct.u8Email;
	}
	else if(pJointAction->alarm_type == J_SDK_LOST_ALARM)
	{
		return -1;
	}
	else if(pJointAction->alarm_type == J_SDK_HIDE_ALARM)
	{
		return -1;
	}
	else if(pJointAction->alarm_type == J_SDK_IO_ALARM)
	{
		IPNC_DEV_DI_CFG_S stDiCfg = {0};
		//获取DI参数配置	
		if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_DI_CFG,
						   		&stDiCfg, sizeof(IPNC_DEV_DI_CFG_S)))
			return -1;
		
		//录像通道(bit) (0 不录像 1第一通道录像)
		pJointAction->joint.joint_record_enable_channel = stDiCfg.struLinkAct.u32RecChannel;
		//联动输出(bit)
		pJointAction->joint.joint_output_enable_channel = stDiCfg.struLinkAct.u32OutPut;
		//抓拍通道(bit)(0 不抓拍 1第一通道抓拍)
		pJointAction->joint.joint_snap_enable_channel = stDiCfg.struLinkAct.u32ShootChannel;
		//录像时间(秒)
		pJointAction->joint.joint_record_second = stDiCfg.struLinkAct.u32RecTime;
		//联动输出(bit)
		pJointAction->joint.joint_beep_enable = stDiCfg.struLinkAct.u32OutPut;
		//输出持续时间
		pJointAction->joint.joint_beep_second = stDiCfg.struLinkAct.u32OutPersist;
		//输出持续时间
		pJointAction->joint.joint_output_times = stDiCfg.struLinkAct.u32OutPersist;
		//抓拍间隔(毫秒),0:连续抓拍
		pJointAction->joint.joint_snap_interval = stDiCfg.struLinkAct.u32ShootInterval/1000;
		//抓拍张数	
		pJointAction->joint.joint_snap_times = stDiCfg.struLinkAct.u8ShootNum;
		//联动email
		pJointAction->joint.joint_email_enable = stDiCfg.struLinkAct.u8Email;
		j_debug("joint_output_enable_channel = %d", pJointAction->joint.joint_output_enable_channel);
		j_debug("joint_beep_enable = %d", pJointAction->joint.joint_beep_enable);
	}
	else
		return -1;
	
	return 0;
}

static int nvr_jxj_set_control_ptz(int id, int channel, void *param, user_t *u)
{
	int nRet = 0;
	JPTZControl *pPtzCtrl = (JPTZControl *)param;
	JCruisePoint stCruisePoint;
	NVR_CTRL_PTZ_S stPtzBallCtrl;

	j_debug("channel : %d", channel);
	j_debug("ptz cmd : %d", pPtzCtrl->action);
	j_debug("ptz param : %d", pPtzCtrl->param);

	CHANNEL_CHECK(channel);
	switch(pPtzCtrl->action)
	{
		case PTZ_STOP:	// 停止
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_STOP, channel, 0, 0);
			break;
		case PTZ_AUTO:	// 自动
			//memcpy(&stCruisePoint, &pPtzCtrl->param, sizeof(stCruisePoint));
			stPtzBallCtrl.u16PtzCmd = PTZ_CMD_SET_AUTOSCAN_SETTING;
			stPtzBallCtrl.struAutoScan.u8SetVerticalPos = 0;
			stPtzBallCtrl.struAutoScan.u8Speed = (pPtzCtrl->param)*7/10 + 10;	//平台为1-100，实际为10-80
			nRet = nvr_lib_send_ptz_cmd(channel, &stPtzBallCtrl);
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_SET_AUTOSCAN_START, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_LEFT:	// 左
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_LEFT, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_RIGHT:	// 右
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_RIGHT, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_UP:	// 上
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_UP, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_DOWN:	// 下
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_DOWN, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_LEFT_UP:	//左上
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_LEFT_UP, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_LEFT_DOWN:	//左下
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_LEFT_DOWN, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_RIGHT_UP:	//右上
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_RIGHT_UP, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_RIGHT_DOWN:	//右下
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_RIGHT_DOWN, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_SET_PP:	// 设置预置点
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_SET_PRESET, channel, 0, pPtzCtrl->param);
			break;
		case PTZ_USE_PP:	// 调用预置点
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_GOTO_PRESET, channel, 0, pPtzCtrl->param);
			break;
		case PTZ_DEL_PP:	// 删除预置点
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_CLR_PRESET, channel, 0, pPtzCtrl->param);
			break;
		case PTZ_ADD_ZOOM:	// 焦距放大
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_ZOOM_TELE, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_SUB_ZOOM:	// 焦距缩小
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_ZOOM_WIDE, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_ADD_FOCUS:	// 远焦点
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_FOCUS_FAR, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_SUB_FOCUS:	// 近焦点
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_FOCUS_NEAR, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_TURN_ON:	// 开启灯光
			break;
		case PTZ_TURN_OFF:	// 关闭灯光
			break;
		case PTZ_WIPERS_ON:	// 雨刷开
			break;
		case PTZ_WIPERS_OFF:	// 雨刷关
			break;
		case PTZ_SET_CRZ:	//设置巡航
			memcpy(&stCruisePoint, &pPtzCtrl->param, sizeof(stCruisePoint));
			nRet = Nvr_client_Ctrl_Ptz_Cruise(channel, stCruisePoint.preset, 1, 
						stCruisePoint.dwell, stCruisePoint.speed);
			break;
		case PTZ_START_CRZ:	//开始巡航
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_SET_CRUISE_START, channel, pPtzCtrl->param, 0);
			break;
		case PTZ_STOP_CRZ:	//停止巡航
			nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_STOP, channel, 0, 0);
			break;
		case PTZ_DEL_CRZ:	//删除巡航
			memcpy(&stCruisePoint, &pPtzCtrl->param, sizeof(stCruisePoint));
			nRet = Nvr_client_Ctrl_Ptz_Cruise(channel, stCruisePoint.preset, 0, 
						stCruisePoint.dwell, stCruisePoint.speed);
			break;
		default:
			break;
	}
	return nRet;
}		

static int nvr_jxj_get_control_ptz(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_submit_alarm(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_submit_alarm(int id, int channel, void *param, user_t *u)
{

	return -1;
}

static int nvr_jxj_set_medial_url(int id, int channel, void *param, user_t *u)
{
	return 0;
}

static int nvr_jxj_get_medial_url(int id, int channel, void *param, user_t *u)
{
	//int size;
	char szIp[64];
	int s32Port;
	JMediaUrl *pMediaUrl = (JMediaUrl *) param;
	NVR_DEV_NET_CFG_S stNetCfg = {0};
	NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg;
	NVR_UPNP_CFG_S stUpnpCfg;

	CHANNEL_CHECK(channel);
	
	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
		return -1;
	if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg))
		return -1;
	if(0 > nvr_logon_get_upnp_cfg(0, &stUpnpCfg))
		return -1;

	j_debug("mediaType : %d", pMediaUrl->media_type);
	j_debug("level : %d", pMediaUrl->level);
	//JMediaType
	//pMediaUrl->level =j_primary_stream ;
	//pMediaUrl->media_type = J_SDK_R_TIME_AV_S;
	if(strlen(stJxjPlatformCfg.szPuId) == 0)
		strcpy(stJxjPlatformCfg.szPuId, "JXJ-NVR-00000000");
	
	/*size = sizeof(pMediaUrl->ip);	
	snprintf(szIp, size - 1, "%d.%d.%d.%d",
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 24) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 16) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 8) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 0) & 0xff);

	szIp[size -1] = '\0';*/
	sprintf(szIp, "%d.%d.%d.%d",
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 24) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 16) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 8) & 0xff,
			(stNetCfg.struEtherCfg[0].u32IpAddr >> 0) & 0xff);

	if(strcmp((char *)pMediaUrl->ip, szIp))
	{
		//strcpy((char *)pMediaUrl->ip, szIp);
		if(stUpnpCfg.u8UpnpOpen && stUpnpCfg.u16UpnpJXJDataPortS)
		{
			s32Port = stUpnpCfg.u16UpnpJXJDataPortS;
		}else if(stUpnpCfg.u8UpnpOpen && stUpnpCfg.u16UpnpJXJDataPort)
		{
			s32Port = stUpnpCfg.u16UpnpJXJDataPort;
		}else
		{
			s32Port = JXJ_RTSP_PORT;
		}
		
	}else
	{
		s32Port = JXJ_RTSP_PORT;
	}

	j_debug("pMediaUrl->ip: %s", (char *)pMediaUrl->ip);
	
	snprintf((char *)pMediaUrl->url, sizeof(pMediaUrl->url), 
				"rtsp://%s:%d/dev=%s/media=%d/channel=%d&level=%d", 
				(char *)pMediaUrl->ip, s32Port, stJxjPlatformCfg.szPuId, 
				pMediaUrl->media_type, channel, pMediaUrl->level);
	j_debug("pMediaUrl->url: %s", (char *)pMediaUrl->url);
	
	strncpy((char *)pMediaUrl->cu_ip, stJxjPlatformCfg.szCmsIp, sizeof(pMediaUrl->cu_ip));	//add by hjl 2013/03/20
	/*
	snprintf((char *)pMediaUrl->url, sizeof(pMediaUrl->url), 
				"rtsp://%s/dev=%s/media=%d/channel=%d&level=%d", 
				pMediaUrl->ip, stJxjPlatformCfg.szPuId, 
				pMediaUrl->media_type, channel, pMediaUrl->level);
	*/
	return 0;

}

static int nvr_jxj_set_store_log(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_store_log(int id, int channel, void *param, user_t *u)
{
	JStoreLog *pStoreLog = (JStoreLog *)param;

	CHANNEL_CHECK(channel);
	if(nvr_jxj_query(channel, pStoreLog) < 0)
	{
		j_debug("Failed to query record.");
		return -1;
	}
	
	return 0;
}

static int nvr_jxj_set_user_login(int id, int channel, void *param, user_t *u)
{
	JUserInfo *pJUserInfo = (JUserInfo *)param;
	NVR_DEV_USER_CFG_S stUserCfg = {0};

	memset(pJUserInfo, 0, sizeof(JUserInfo));
	
	if(0 > nvr_logon_get_user_cfg(0, &stUserCfg))
		return -1;
	
	strncpy(stUserCfg.szUsrName, (char *)pJUserInfo->username, sizeof(stUserCfg.szUsrName) - 1);
	stUserCfg.szUsrName[sizeof(stUserCfg.szUsrName) - 1] = '\0';
	strncpy(stUserCfg.szPsw, (char *)pJUserInfo->password, sizeof(stUserCfg.szPsw) - 1);
	stUserCfg.szPsw[sizeof(stUserCfg.szPsw) - 1] = '\0';
	
	if(0 > nvr_logon_set_user_cfg(0, &stUserCfg))
		return -1;
	
	return 0;
}

static int nvr_jxj_get_user_login(int id, int channel, void *param, user_t *u)
{
	JUserInfo *pJUserInfo = (JUserInfo *)param;
	NVR_DEV_USER_CFG_S stUserCfg = {0};

	if(0 > nvr_logon_get_user_cfg(0, &stUserCfg))
		return -1;

	j_debug("%s, %s", (char *)pJUserInfo->username, (char *)pJUserInfo->password);
	j_debug("%s, %s", stUserCfg.szUsrName, stUserCfg.szPsw);
#if 0
	return 0;
#else
	if((strcmp((char *)pJUserInfo->username, stUserCfg.szUsrName) == 0)
		&& (strcmp((char *)pJUserInfo->password, stUserCfg.szPsw) == 0))
		return 0;
	else return -1;
#endif	
}

static gint gIsStartUpgrade = 0;
static int gUpgradeFileLen = 0;

static int nvr_jxj_set_firmware_upgrade(int id, int channel, void *param, user_t *u)
{
	JBinaryStream *pBinStream = (JBinaryStream *)param;
	JUpgradeProgress upgrade;
	int nPacketSize;	
	static int nPacketNo = 0;
	static int nPrePercent = 0;
	
	if(pBinStream->flag == UPGRADE_START)
	{
		j_debug(">>>>>>>>>>>>>>>>>>>>>>>>>>>The update start....");
		if(g_atomic_int_compare_and_exchange(&gIsStartUpgrade, 0, 1))
		{
			j_warning("The upgrade is running.");
			return -1;
		}
		nPacketNo = 0;
	}
	else if(pBinStream->flag == UPGRADE_NORMAL)
	{
	}
	else if(pBinStream->flag == UPGRADE_ERROR)
	{
		j_warning("Update error.");
		if(nvr_update_write((char *)pBinStream->data, 0, nPacketNo++, gUpgradeFileLen) < 0)
		{
			j_warning("Failed to write update data.");
			return -1;
		}
		g_atomic_int_set(&gIsStartUpgrade, 0);
		return 0;
	}
	
	nPacketSize = pBinStream->size - sizeof(JBinaryStream);
	if(nPacketSize > 0)
	{
		if(nvr_update_write((char *)pBinStream->data, nPacketSize, nPacketNo++, gUpgradeFileLen) < 0)
		{
			j_warning("Failed to write update data.");
			upgrade.percent = (uint32_t)-1; 	// 升级失败
			j_dev_sdk_submit_param(PARAM_FIRMWARE_UPGRADE, 0, &upgrade);
			return -1;
		}
		upgrade.percent = nvr_update_get_percent();
		if(upgrade.percent != nPrePercent)
		{
			if(upgrade.percent == 100)
				upgrade.percent = 99;
			j_debug("percent: %d", upgrade.percent);
			nPrePercent = upgrade.percent;
			j_dev_sdk_submit_param(PARAM_FIRMWARE_UPGRADE, 0, &upgrade);	
		}
	}
	
	if(pBinStream->flag == UPGRADE_END)
	{
		j_debug(">>>>>>>>>>>>>>>>>>>>>>>>>>>The update end....");
		nvr_update_write((char *)pBinStream->data, 0, nPacketNo++, gUpgradeFileLen);
		upgrade.percent = 100;
		j_dev_sdk_submit_param(PARAM_FIRMWARE_UPGRADE, 0, &upgrade);
		g_atomic_int_set(&gIsStartUpgrade, 0);
		// set the reboot flag
		gIsSetReboot = 1;
		nvr_restart();
	}
	
	return 0;
}

static int nvr_jxj_get_firmware_upgrade(int id, int channel, void *param, user_t *u)
{
	JFirmwareUpgrade *pUpgrade = (JFirmwareUpgrade *)param;
	NVR_UPGRADE_FILE_HDR_S stFileHdr;
	NVR_DEV_NET_CFG_S stNetCfg;
	uint32_t		u32IpAddr;
	
	j_debug("percent = %d", pUpgrade->percent);
	j_debug("data_len = %d", pUpgrade->data_len);
	j_debug("file_len = %d", pUpgrade->file_len);
	j_debug("sess_id = %d", pUpgrade->sess_id);

	if(strlen((char *)pUpgrade->addr) == 0) // 被动升级模式
	{
		if(g_atomic_int_get(&gIsStartUpgrade))
		{
			g_error("The upgrade is busy!!!");
			return -1;
		}
		memcpy(&stFileHdr, &pUpgrade->data, sizeof(stFileHdr));
		if(stFileHdr.u32Magic != UPDATE_FILE_HDR_MAGIC)
		{
			g_error("The update file header magic number is wrong.");
			return -1;
		}
		if(stFileHdr.u32TotalLen != pUpgrade->file_len - sizeof(stFileHdr))
		{
			g_error("The update file lenght is wrong.");
			return -1;
		}
		
		if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
			return -1;
		u32IpAddr = stNetCfg.struEtherCfg[0].u32IpAddr;
		snprintf((char *)pUpgrade->addr, sizeof(pUpgrade->addr)-1, "%d.%d.%d.%d:%d", 
					(u32IpAddr >> 24) & 0xff,
					(u32IpAddr >> 16) & 0xff,
					(u32IpAddr >> 8) & 0xff,
					(u32IpAddr >> 0) & 0xff,
					JXJ_UPGRADE_PORT);
		j_debug("upgrade address : %s", (char *)pUpgrade->addr);
		//gIsStartUpgrade = 1;
		gUpgradeFileLen = pUpgrade->file_len;
		return 0;
	}
	else			// 主动升级模式
	{
		return -1;
	}
	return 0;
}


static int nvr_jxj_set_device_cap(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_device_cap(int id, int channel, void *param, user_t *u)
{
	//int nSensorType = 0;
	NVR_DEV_VENC_CFG_S stVencCfg;
	JDevCap *pDevCap = (JDevCap *)param;
	JChnCap *pChnCap = (JChnCap *)&pDevCap->chn_cap;

	memset(pDevCap, 0, sizeof(JDevCap));
	pDevCap->size = sizeof(JDevCap);
	pDevCap->cap0 = DEV_CAP_VIDEO_IN | DEV_CAP_AUDIO | DEV_CAP_IRIS | DEV_CAP_PTZ |
						DEV_CAP_ALARM_IN | DEV_CAP_ALARM_OUT | DEV_CAP_STORAGE | 
						DEV_CAP_WEB | DEV_CAP_PLATFORM | DEV_CAP_UPDATE | 
						DEV_CAP_VIDEO_OUT | DEV_CAP_IR;
	if(0)//wifi
		pDevCap->cap0 = pDevCap->cap0 | DEV_CAP_WIFI;
	if(0)//3g
		pDevCap->cap0 = pDevCap->cap0 | DEV_CAP_3G;
	pDevCap->ivs_cap = 0;
	pDevCap->eth_speed = NET_SPEED_100M;
	pDevCap->hw_encoder_type = TI_DM365;		
	pDevCap->video_encode_type = VIDEO_ENCODE_H264_E;
	pDevCap->platform_type = PF_JXJ;
	pDevCap->protocal_type = PROTOCOL_RTSP;
	pDevCap->nvr_pu_proto = JXJ_IPC;
	pDevCap->pu_type = PU_TYPE;
	//begin :add by hjl 2013/03/20
	pDevCap->ftp_enable = 1;
	pDevCap->upnp_enable = 1;
	//end :add by hjl 2013/03/20
	pDevCap->eth_num = NVR_ETHERNET_NUM;
	pDevCap->chn_num = NVR_MAX_CHANNEL_NUM;

	pChnCap->size = sizeof(JChnCap);
	pChnCap->encode = VIDEO_ENCODE_H264_E;
	pChnCap->supp_mask = 1;
	pChnCap->mask_count = NVR_VMASK_REG_NUM;
	pChnCap->supp_hide_alarm = 0;
	pChnCap->hide_alarm_count = 0;
	pChnCap->supp_move_alarm = 1;
	pChnCap->move_alarm_count = NVR_VMD_REG_NUM;
	pChnCap->supp_video_lost_alarm = 0;
	pChnCap->osd_count = 2;
	pChnCap->stream_count = 2;
	
	if(0 > nvr_logon_get_main_venc_cfg(0, &stVencCfg))
		return -1;
	if(stVencCfg.u16Width == 1280)
	{
		//nSensorType = hi_platform_get_sensor_type();
		//if(nSensorType == DRV_SENSOR_TYPE_AR0130 || nSensorType == DRV_SENSOR_TYPE_AP100_MT9P034)
		//	pChnCap->stream_supp_resolution[0] = CAP_VIDEO_720P | CAP_VIDEO_960;
		//else 
			pChnCap->stream_supp_resolution[0] = CAP_VIDEO_720P;
		pChnCap->stream_supp_resolution[1] = CAP_VIDEO_CIF | CAP_VIDEO_D1;
	}
	else if(stVencCfg.u16Width == 1920)
	{
		pChnCap->stream_supp_resolution[0] = CAP_VIDEO_1080P;
		pChnCap->stream_supp_resolution[1] = CAP_VIDEO_CIF;
	}

	pChnCap->stream_max_frate_rate[0] = 30;
	pChnCap->stream_max_frate_rate[1] = 30;
	/**/
	//hi_platform_get_ima_cfg(0, &stImaCfg);
	//hi_platform_get_3a_cfg(0, &st3ACfg);
	//if(stImaCfg.eSuppMask & VCT_IMA_BRIGHTNESS)
	//	pChnCap->img_cap |= IMA_BRIGHTNESS;
	//if(stImaCfg.eSuppMask & VCT_IMA_CONTRAST)
	//	pChnCap->img_cap |= IMA_CONTRAST;
	//if(stImaCfg.eSuppMask & VCT_IMA_SATURATION)
	//	pChnCap->img_cap |= IMA_SATURATION;
	//if(stImaCfg.eSuppMask & VCT_IMA_HUE)
	//	pChnCap->img_cap |= IMA_HUE;
	//if(stImaCfg.eSuppMask & VCT_IMA_SHARPNESS)
	//	pChnCap->img_cap |= IMA_SHARPNESS;


	pChnCap->img_cap = 0;
	pChnCap->img_cap |= IMA_BRIGHTNESS;
	pChnCap->img_cap |= IMA_CONTRAST;
	pChnCap->img_cap |= IMA_SATURATION;
	pChnCap->img_cap |= IMA_HUE;
	pChnCap->img_cap |= IMA_SHARPNESS;

	pChnCap->img_cap |= IMA_FLIP;
	pChnCap->img_cap |= IMA_MIRROR;
	pChnCap->img_cap |= IMA_PWDFREQ;
	pChnCap->img_cap |= IMA_AWB;

	j_debug("pChnCap->img_cap:%#x", pChnCap->img_cap) ;
	return 0;
}

static int nvr_jxj_set_search_dev(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_search_dev(int id, int channel, void *param, user_t *u)
{
	return -1;
}
/*
static int Onvr_jxj_set_channel_info(int id, int channel, void *param, user_t *u)
{
	JChannelInfo *pChInfo = (JChannelInfo *)param;
	JRemoteChannelInfo *pRemoteChInfo = &pChInfo->rmt_ch_info;
	JRemoteDeviceInfo *pRemoteDevInfo = &pChInfo->rmt_dev_info;
	NVR_CHANNEL_CFG_S stChInfo;
	
	CHANNEL_CHECK(channel);
	if(0 > nvr_logon_get_channel_cfg(channel, &stChInfo))
		return -1

	g_strlcpy(stChInfo.szProtocolName, "jxj_ti", sizeof(stChInfo.szProtocolName));
	g_strlcpy(stChInfo.szDevName, (char *)pChInfo->ch_name, sizeof(pChInfo->ch_name));
	
	g_strlcpy(stChInfo.szUser, (char *)pRemoteChInfo->user_name, sizeof(stChInfo.szUser));
	g_strlcpy(stChInfo.szPasswd, (char *)pRemoteChInfo->user_pwd, sizeof(stChInfo.szPasswd));

	nvr_jxj_convert_ip((char *)pRemoteDevInfo->ip, &stChInfo.u32IpAddr);
	stChInfo.u16DataPort = pRemoteDevInfo->port;
	if(0 > nvr_logon_set_channel_cfg(channel, &stChInfo))
		return -1;
	return 0;
}*/
static int nvr_jxj_set_channel_info(int id, int channel, void *param, user_t *u)
{
	JChannelInfo *pChInfo = (JChannelInfo *)param;
	JRemoteChannelInfo *pRemoteChInfo = &pChInfo->rmt_ch_info;
	JRemoteDeviceInfo *pRemoteDevInfo = &pChInfo->rmt_dev_info;
	NVR_CHANNEL_CFG_S stChInfo;
	char *pProName = NULL;
	
	CHANNEL_CHECK(channel);
	if(0 > nvr_logon_get_channel_cfg(channel, &stChInfo))
		return -1;

	j_debug(">>>>>>>>>>>>>>>>>>>pRemoteDevInfo->port: %d\n", pRemoteDevInfo->port);
	if(3321 == pRemoteDevInfo->port)
		pProName = "jxj_cu";
	else
		pProName = "jxj_ti";

	stChInfo.u8Enable = pChInfo->ch_status;
	stChInfo.u8SearchA = 0;//智能搜索标记
	g_strlcpy(stChInfo.szProtocolName, pProName, sizeof(stChInfo.szProtocolName));
	g_strlcpy(stChInfo.szDevName, (char *)pChInfo->ch_name, sizeof(stChInfo.szDevName));
	
	g_strlcpy(stChInfo.szUser, (char *)pRemoteChInfo->user_name, sizeof(stChInfo.szUser));
	g_strlcpy(stChInfo.szPasswd, (char *)pRemoteChInfo->user_pwd, sizeof(stChInfo.szPasswd));

	j_debug(">>>>>>>>>>>>>>>>>>>stChInfo.szProtocolName: %s\n", stChInfo.szProtocolName);
	
	nvr_jxj_convert_ip((char *)pRemoteDevInfo->ip, &stChInfo.u32IpAddr);
	stChInfo.u16DataPort = pRemoteDevInfo->port;
	if(0 > nvr_logon_set_channel_cfg(channel, &stChInfo))
		return -1;
	return 0;
}

static int nvr_jxj_get_channel_info(int id, int channel, void *param, user_t *u)
{
	JChannelInfo *pChInfo = (JChannelInfo *)param;
	JRemoteChannelInfo *pRemoteChInfo = &pChInfo->rmt_ch_info;
	JRemoteDeviceInfo *pRemoteDevInfo = &pChInfo->rmt_dev_info;
	NVR_CHANNEL_CFG_S stChInfo;
	
	CHANNEL_CHECK(channel);
	if(0 > nvr_logon_get_channel_cfg(channel, &stChInfo))
		return -1;
	pChInfo->ch_no = channel;
	pChInfo->ch_type = CH_NETWORK;
	pChInfo->ch_status = stChInfo.u8Enable;
	g_strlcpy((char *)pChInfo->ch_name, stChInfo.szDevName, sizeof(pChInfo->ch_name));

	pRemoteChInfo->ch_no = stChInfo.u8RemoteChn;
	pRemoteChInfo->protocol = 0;
	pRemoteChInfo->audio_enable = 0;
	g_strlcpy((char *)pRemoteChInfo->user_name, stChInfo.szUser, sizeof(pRemoteChInfo->user_name));
	g_strlcpy((char *)pRemoteChInfo->user_pwd, stChInfo.szPasswd, sizeof(pRemoteChInfo->user_pwd));

	nvr_jxj_reconvert_ip(stChInfo.u32IpAddr, (char *)pRemoteDevInfo->ip);
	pRemoteDevInfo->port = stChInfo.u16DataPort;
	pRemoteDevInfo->dev_type = J_Dev_IPNC_;
	pRemoteDevInfo->ch_sum = 1;
	pRemoteDevInfo->dns_enable = 0;

	return 0;
}

static int nvr_jxj_set_pic_info(int id, int channel, void *param, user_t *u)
{
	j_debug("nvr_jxj_set_pic_info...");
	JPictureInfo *pPictureInfo = (JPictureInfo *)param;
	IPNC_DEV_3A_CFG_S st3ACfg = {0};
	IPNC_DEV_IMA_CFG_S  stImaCfg = {0};
	
	CHANNEL_CHECK(channel);
	
	//获取视频参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_IMA_CFG,
					   		&stImaCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		return -1;
	//获取3A参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_3A_CFG,
					   		 &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return -1;
	
	st3ACfg.struMirror.u8Value= pPictureInfo->mirror;
	st3ACfg.struFlip.u8Value = pPictureInfo->flip;
	st3ACfg.struPwdFreq.u8Value = pPictureInfo->hz - 1;
	st3ACfg.struShutterSpeed.u8Value = pPictureInfo->shutter;
	
	stImaCfg.struRed.u8Value = pPictureInfo->awb_red;
	stImaCfg.struBlue.u8Value = pPictureInfo->awb_blue;

	//设置
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_IMA_CFG, 
					  		&stImaCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		return -1;
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_3A_CFG,
					  		 &st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return -1;

	return 0;
}

static int nvr_jxj_get_pic_info(int id, int channel, void *param, user_t *u)
{
	j_debug("nvr_jxj_get_pic_info...");
	JPictureInfo *pPictureInfo = (JPictureInfo *)param;
	IPNC_DEV_3A_CFG_S st3ACfg = {0};
	IPNC_DEV_IMA_CFG_S  stImaCfg = {0};
	
	CHANNEL_CHECK(channel);
	
	//获取视频参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_IMA_CFG, 
					  		 &stImaCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		return -1;
	//获取3A参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_3A_CFG,
					   		&st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return -1;
	
	pPictureInfo->mirror = st3ACfg.struMirror.u8Value;
	pPictureInfo->flip 	 = st3ACfg.struFlip.u8Value;
	pPictureInfo->hz 	 = st3ACfg.struPwdFreq.u8Value + 1;
	pPictureInfo->awb_mode = 0;
	pPictureInfo->awb_red  = stImaCfg.struRed.u8Value;
	pPictureInfo->awb_blue = stImaCfg.struBlue.u8Value;
	pPictureInfo->wdr = 100;
	pPictureInfo->iris_type = 1;
	pPictureInfo->exp_compensation = 0;
	
	//if(hi_ptz_is_jxj_ball())
	//	pPictureInfo->shutter = st3ACfg.struShutterSpeed.u8Value;
	//else 
	//	pPictureInfo->shutter = 0;
	pPictureInfo->shutter = 0;
	
	return 0;
}
static int nvr_jxj_set_wifi_info(int id, int channel, void *param, user_t *u)
{
/*	JWifiConfig *pWifiConfig = (JWifiConfig *)param;
	HI_WIFI_STR_CFG_S stWifiCfg = {0};

	hi_platform_get_wifi_cfg(0, &stWifiCfg);

	stWifiCfg.u8Connect = pWifiConfig->wifi_enable;
	strncpy(stWifiCfg.szEssid, pWifiConfig->essid, sizeof(stWifiCfg.szEssid));
	strncpy(stWifiCfg.szPsw, pWifiConfig->pwd, sizeof(stWifiCfg.szPsw));

*/	
	return -1;
}

static int nvr_jxj_get_wifi_info(int id, int channel, void *param, user_t *u)
{

	return -1;
}

static int nvr_jxj_set_wifi_search(int id, int channel, void *param, user_t *u)
{

	return -1;
}

static int nvr_jxj_get_wifi_search(int id, int channel, void *param, user_t *u)
{
	/*int i = 0;
	JWifiSearchRes *pWifiSearchRes = (JWifiSearchRes *)param;
	HI_WIFI_ESSID_CFG_S stWifiEssid = {0};

	hi_platform_wifi_scan_cfg(0, &stWifiEssid);
	pWifiSearchRes->count = stWifiEssid.u8Count;
	for(i=0; i<stWifiEssid.u8Count; i++)
	{
		strncpy((char *)pWifiSearchRes->wifi_ap[i].essid, stWifiEssid.Essid[i].szEssid, J_SDK_MAX_ID_LEN);
		if(strcmp(stWifiEssid.Essid[i].szAuthMode, "OPEN") == 0)
		{
			pWifiSearchRes->wifi_ap[i].encrypt_type = 0;
			pWifiSearchRes->wifi_ap[i].auth_mode = 3;
		}else if(strcmp(stWifiEssid.Essid[i].szAuthMode, "WPAPSK") == 0)
		{
			pWifiSearchRes->wifi_ap[i].encrypt_type = 1;
			pWifiSearchRes->wifi_ap[i].auth_mode = 2;
		}else if(strcmp(stWifiEssid.Essid[i].szAuthMode, "WPA2PSK") == 0)
		{
			pWifiSearchRes->wifi_ap[i].encrypt_type = 2;
			pWifiSearchRes->wifi_ap[i].auth_mode = 2;
		}else if(strcmp(stWifiEssid.Essid[i].szAuthMode, "WEPAUTO") == 0)
		{
			pWifiSearchRes->wifi_ap[i].encrypt_type = 3;
			pWifiSearchRes->wifi_ap[i].auth_mode = 0;
		}

		if(strcmp(stWifiEssid.Essid[i].szEncryption, "NONE") == 0)
		{
			pWifiSearchRes->wifi_ap[i].secret_key_type = 0;
		}else if(strcmp(stWifiEssid.Essid[i].szEncryption, "AES") == 0)
		{
			pWifiSearchRes->wifi_ap[i].secret_key_type = 1;
		}else if(strcmp(stWifiEssid.Essid[i].szEncryption, "TKIP") == 0)
		{
			pWifiSearchRes->wifi_ap[i].secret_key_type = 2;
		}
		pWifiSearchRes->wifi_ap[i].quality = stWifiEssid.Essid[i].nSignal;
		pWifiSearchRes->wifi_ap[i].bit_rate = stWifiEssid.Essid[i].nMaxBitRate;
	}*/
	
	return 0;
}

static int nvr_jxj_set_user_config(int id, int channel, void *param, user_t *u)
{
	JUserConfig *pUserConfig = (JUserConfig *)param;
	NVR_DEV_USER_CFG_S stUserCfg;

	if(0 > nvr_logon_get_user_cfg(0, &stUserCfg))
		return -1;
	strncpy(stUserCfg.szUsrName, (char *)pUserConfig->username, sizeof(stUserCfg.szUsrName)-1);
	stUserCfg.szUsrName[sizeof(stUserCfg.szUsrName)-1] = '\0';
	strncpy(stUserCfg.szPsw, (char *)pUserConfig->password, sizeof(stUserCfg.szPsw)-1);
	stUserCfg.szPsw[sizeof(stUserCfg.szPsw)-1] = '\0';
	stUserCfg.u64LocalRight = pUserConfig->local_right;
	stUserCfg.u64RemoteRight = pUserConfig->remote_right;
	if(0 > nvr_logon_set_user_cfg(0, &stUserCfg))
		return -1;
	
	return 0;
}

static int nvr_jxj_get_user_config(int id, int channel, void *param, user_t *u)
{
	JUserConfig *pUserConfig = (JUserConfig *)param;
	NVR_DEV_USER_CFG_S stUserCfg;

	if(0 > nvr_logon_get_user_cfg(0, &stUserCfg))
		return -1;
	strncpy((char *)pUserConfig->username, stUserCfg.szUsrName, sizeof(pUserConfig->username)-1);
	pUserConfig->username[sizeof(pUserConfig->username)-1] = '\0';
	strncpy((char *)pUserConfig->password, stUserCfg.szPsw, sizeof(pUserConfig->password)-1);
	pUserConfig->password[sizeof(pUserConfig->password)-1] = '\0';
	pUserConfig->local_right = stUserCfg.u64LocalRight;
	pUserConfig->remote_right = stUserCfg.u64RemoteRight;
	
	return 0;
}

static int nvr_jxj_set_control_dev(int id, int channel, void *param, user_t *u)
{
	JControlDevice *pControlDevice = (JControlDevice *)param;

	switch(pControlDevice->command)
	{
		case 1:	// 关机
			gIsSetReboot = 1;
			nvr_shutdown();
			break;
		case 2:	// 重启
			gIsSetReboot = 1;
			nvr_restart();
			break;
		case 3:	// 恢复默认出厂设置
			nvr_sw_reset();
			break;
		case 4:	// 坏点检测
			break;
		default:
			j_warning("The command of controlling device is wrong, command is %d", pControlDevice->command);
			break;
	}
	return 0;
}

static int nvr_jxj_get_control_dev(int id, int channel, void *param, user_t *u)
{

	return -1;
}

static int nvr_jxj_set_network_status(int id, int channel, void *param, user_t *u)
{

	return -1;
}

static int nvr_jxj_get_network_status(int id, int channel, void *param, user_t *u)
{
	JNetworkStatus *pNetStatus = (JNetworkStatus *)param;
	NVR_DEV_NET_CFG_S stNetCfg;

	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
		return -1;
	pNetStatus->eth_st = 2;
	pNetStatus->wifi_st = 0;
	pNetStatus->pppoe_st = stNetCfg.struPppoeCfg.u8PppoeStatus % 3;
	return 0;
}

static int nvr_jxj_set_interest_coding(int id, int channel, void *param, user_t *u)
{

	return -1;
}

static int nvr_jxj_get_interest_coding(int id, int channel, void *param, user_t *u)
{

	return -1;
}

static int nvr_jxj_set_ddns_coding(int id, int channel, void *param, user_t *u)
{
	
	JDdnsConfig *pDdnsConfig = (JDdnsConfig *)param;
	NVR_DDNS_CFG_S stDdnsCfg;

	if(0 > nvr_logon_get_ddns_cfg(0, &stDdnsCfg))
		return -1;
	strncpy(stDdnsCfg.szDDNSAccount, (char *)pDdnsConfig->ddns_account, sizeof(stDdnsCfg.szDDNSAccount) -1 );
	stDdnsCfg.szDDNSAccount[sizeof(stDdnsCfg.szDDNSAccount)-1] = '\0';
	strncpy(stDdnsCfg.szDDNSUsr, (char *)pDdnsConfig->ddns_usr, sizeof(stDdnsCfg.szDDNSUsr) -1 );
	stDdnsCfg.szDDNSUsr[sizeof(stDdnsCfg.szDDNSUsr)-1] = '\0';
	strncpy(stDdnsCfg.szDDNSPsw, (char *)pDdnsConfig->ddns_pwd, sizeof(stDdnsCfg.szDDNSPsw) -1 );
	stDdnsCfg.szDDNSPsw[sizeof(stDdnsCfg.szDDNSPsw)-1] = '\0';
	stDdnsCfg.u8DDNSOpen = pDdnsConfig->ddns_open;
	stDdnsCfg.u8DDNSType = pDdnsConfig->ddns_type;
	stDdnsCfg.u16DDNSPort = pDdnsConfig->ddns_port;
	stDdnsCfg.u32DDNSTimes = pDdnsConfig->ddns_times;
	if(0 > nvr_logon_set_ddns_cfg(0, &stDdnsCfg))
		return -1;

	return 0;
}

static int nvr_jxj_get_ddns_coding(int id, int channel, void *param, user_t *u)
{
	JDdnsConfig *pDdnsConfig = (JDdnsConfig *)param;
	NVR_DDNS_CFG_S stDdnsCfg;

	if(0 > nvr_logon_get_ddns_cfg(0, &stDdnsCfg))
		return -1;
	strncpy((char *)pDdnsConfig->ddns_account, stDdnsCfg.szDDNSAccount, sizeof(pDdnsConfig->ddns_account) -1 );
	pDdnsConfig->ddns_account[sizeof(pDdnsConfig->ddns_account)-1] = '\0';
	strncpy((char *)pDdnsConfig->ddns_usr, stDdnsCfg.szDDNSUsr, sizeof(pDdnsConfig->ddns_usr) -1 );
	pDdnsConfig->ddns_usr[sizeof(pDdnsConfig->ddns_usr)-1] = '\0';
	strncpy((char *)pDdnsConfig->ddns_pwd, stDdnsCfg.szDDNSPsw, sizeof(pDdnsConfig->ddns_pwd) -1 );
	pDdnsConfig->ddns_pwd[sizeof(pDdnsConfig->ddns_pwd)-1] = '\0';
	pDdnsConfig->ddns_open = stDdnsCfg.u8DDNSOpen;
	pDdnsConfig->ddns_type = stDdnsCfg.u8DDNSType;
	pDdnsConfig->ddns_port = stDdnsCfg.u16DDNSPort;
	pDdnsConfig->ddns_times = stDdnsCfg.u32DDNSTimes;

	return 0;
}

static int nvr_jxj_set_def_display_info(int id, int channel, void *param, user_t *u)
{
	j_debug("nvr_jxj_set_def_display_info...");
	return nvr_jxj_set_display_info(id, channel, param, u);
}

static int nvr_jxj_get_def_display_info(int id, int channel, void *param, user_t *u)
{
	j_debug("nvr_jxj_get_def_display_info...");
	JDisplayParameter *pDisplay = (JDisplayParameter *)param;
	IPNC_DEV_IMA_CFG_S  struImaCfg = {0};
	
	CHANNEL_CHECK(channel);
	
	//获取视频参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_IMA_CFG, 
					  		 &struImaCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		return -1;

	pDisplay->contrast   = struImaCfg.struContrast.u8Default;
	pDisplay->bright 	 = struImaCfg.struBrightness.u8Default;
	pDisplay->hue 		 = struImaCfg.struHue.u8Default;
	pDisplay->saturation = struImaCfg.struSaturation.u8Default;
	pDisplay->sharpness  = struImaCfg.struSharpness.u8Default;
	return 0;
}

static int nvr_jxj_set_def_picture_info(int id, int channel, void *param, user_t *u)
{
	j_debug("nvr_jxj_set_def_picture_info...");
	return nvr_jxj_set_pic_info(id, channel, param, u);
}

static int nvr_jxj_get_def_picture_info(int id, int channel, void *param, user_t *u)
{
	j_debug("nvr_jxj_get_def_picture_info...");
	JPictureInfo *pPictureInfo = (JPictureInfo *)param;
	IPNC_DEV_3A_CFG_S st3ACfg = {0};
	IPNC_DEV_IMA_CFG_S  stImaCfg = {0};
	
	CHANNEL_CHECK(channel);
	
	//获取视频参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_IMA_CFG, 
					  		 &stImaCfg, sizeof(IPNC_DEV_IMA_CFG_S)))
		return -1;
	//获取3A参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_3A_CFG, 
					  		&st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return -1;

	pPictureInfo->mirror = st3ACfg.struMirror.u8Default;
	pPictureInfo->flip = st3ACfg.struFlip.u8Default;
	pPictureInfo->hz = st3ACfg.struPwdFreq.u8Default + 1;
	pPictureInfo->awb_mode = 0;
	pPictureInfo->awb_red = stImaCfg.struRed.u8Default;
	pPictureInfo->awb_blue = stImaCfg.struBlue.u8Default;
	pPictureInfo->wdr = 100;
	pPictureInfo->iris_type = 1;
	pPictureInfo->exp_compensation = 0;
	return 0;
}

static int nvr_jxj_set_hxht_config(int id, int channel, void *param, user_t *u)
{
	return 0;
}

static int nvr_jxj_get_hxht_config(int id, int channel, void *param, user_t *u)
{
	return 0;
}

static int nvr_jxj_set_modify_user_config(int id, int channel, void *param, user_t *u)
{
	JUserModConfig *pUserModCfg = (JUserModConfig *)param;
	NVR_DEV_USER_CFG_S stUserCfg;
	
	if(0 > nvr_logon_get_user_cfg(0, &stUserCfg))
		return -1;
	if(strcmp(stUserCfg.szUsrName, (char *)pUserModCfg->old_usrname) != 0
		|| strcmp(stUserCfg.szPsw, (char *)pUserModCfg->old_password) != 0)
	{
		j_warning("The old user name or old password is wrong.");
		return -1;
	}
	strncpy(stUserCfg.szUsrName, (char *)pUserModCfg->new_username, sizeof(stUserCfg.szUsrName)-1);
	stUserCfg.szUsrName[sizeof(stUserCfg.szUsrName)-1] = '\0';
	strncpy(stUserCfg.szPsw, (char *)pUserModCfg->new_password, sizeof(stUserCfg.szPsw)-1);
	stUserCfg.szPsw[sizeof(stUserCfg.szPsw)-1] = '\0';
	stUserCfg.u64LocalRight = pUserModCfg->local_right;
	stUserCfg.u64RemoteRight = pUserModCfg->remote_right;
	if(0 > nvr_logon_set_user_cfg(0, &stUserCfg))
		return -1;
	return 0;
}

static int nvr_jxj_get_modify_user_config(int id, int channel, void *param, user_t *u)
{
	JUserModConfig *pUserModCfg = (JUserModConfig *)param;
	NVR_DEV_USER_CFG_S stUserCfg;

	if(0 > nvr_logon_get_user_cfg(0, &stUserCfg))
		return -1;
	strncpy((char *)pUserModCfg->old_usrname, stUserCfg.szUsrName, sizeof(pUserModCfg->old_usrname)-1);
	pUserModCfg->old_usrname[sizeof(pUserModCfg->old_usrname)-1] = '\0';
	strncpy((char *)pUserModCfg->old_password, stUserCfg.szPsw, sizeof(pUserModCfg->old_password)-1);
	pUserModCfg->old_password[sizeof(pUserModCfg->old_password)-1] = '\0';
	
	pUserModCfg->local_right = stUserCfg.u64LocalRight;
	pUserModCfg->remote_right = stUserCfg.u64RemoteRight;
	return 0;
}

static int nvr_jxj_set_avd_config(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_avd_config(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_gb28181_config(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_gb28181_config(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_network_disk_config(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_network_disk_config(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_dev_work_state(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_dev_work_state(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_operation_log(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_operation_log(int id, int channel, void *param, user_t *u)
{
	return 0;
}

static int nvr_jxj_set_alarm_upload_cfg(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_alarm_upload_cfg(int id, int channel, void *param, user_t *u)
{
	return -1;
}
/* begin:	add by hjl	2013/1/7*/
static int nvr_jxj_set_preset_point_cfg(int id, int channel, void *param, user_t *u)
{
	//int nRet = 0;
	int nCount = 0;
	int i;
	int flag = 0;
	JPPConfig *pPPConfig = (JPPConfig *)param;
	IPNC_DEV_PP_SET_CFG_S stPPSetCfg;

	CHANNEL_CHECK(channel);
	//获取预置点配置
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_PP_CFG, 
					  		 &stPPSetCfg, sizeof(IPNC_DEV_PP_SET_CFG_S)))
		return -1;
	
	nCount = stPPSetCfg.pp_count;
	switch(pPPConfig->action)
	{
		case PTZ_SET_PP:	// 设置预置点
			//nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_SET_PRESET, channel, 0, pPPConfig->pp.preset);
			Nvr_Client_Crtl_Ptz(PTZ_CMD_SET_PRESET, channel, 0, pPPConfig->pp.preset);
			for(i=0; i<nCount; i++)		//设置的预置点是否存在
			{
				if(stPPSetCfg.pp[i].preset == pPPConfig->pp.preset)	//存在则替换
				{
					memcpy(&stPPSetCfg.pp[i], &pPPConfig->pp, sizeof(IPNC_PRESET_POINT_S));
					flag = 1;
					break;
				}
			}
			if(!flag)
			{
				memcpy(&stPPSetCfg.pp[nCount], &pPPConfig->pp, sizeof(IPNC_PRESET_POINT_S));	//不存在则添加
				stPPSetCfg.pp_count++;
			}
			break;
		case PTZ_USE_PP:	// 调用预置点
			//nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_GOTO_PRESET, channel, 0, pPPConfig->pp.preset);
			Nvr_Client_Crtl_Ptz(PTZ_CMD_GOTO_PRESET, channel, 0, pPPConfig->pp.preset);
			break;
		case PTZ_DEL_PP:	// 删除预置点
			//nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_CLR_PRESET, channel, 0, pPPConfig->pp.preset);
			Nvr_Client_Crtl_Ptz(PTZ_CMD_CLR_PRESET, channel, 0, pPPConfig->pp.preset);
			for(i=0; i<nCount; i++)	//寻找删除的预置点
			{
				if(stPPSetCfg.pp[i].preset == pPPConfig->pp.preset)
				{
					for(i=i+1; i<nCount; i++)	//将后面的预置点前移覆盖
					{
						memcpy(&stPPSetCfg.pp[i-1], &stPPSetCfg.pp[i], sizeof(IPNC_PRESET_POINT_S));
					}
					stPPSetCfg.pp_count--;
					break;
				}
			}
			break;
		default:
			break;
	}
	//设置预置点配置
	if(0 > nvr_set_ipnc_param(channel,  IPNC_CMD_PP_CFG, 
					  		 &stPPSetCfg, sizeof(IPNC_DEV_PP_SET_CFG_S)))
		return -1;
	return 0;
}

static int nvr_jxj_get_preset_point_cfg(int id, int channel, void *param, user_t *u)
{
	int i;
	JPPSet *pPPSet = (JPPSet *)param;
	IPNC_DEV_PP_SET_CFG_S stPPSetCfg;

	CHANNEL_CHECK(channel);
	//获取预置点配置
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_PP_CFG, 
					  		 &stPPSetCfg, sizeof(IPNC_DEV_PP_SET_CFG_S)))
		return -1;
	
	pPPSet->pp_count = stPPSetCfg.pp_count;
	for(i=0; i<stPPSetCfg.pp_count; i++)
	{
		memcpy(&pPPSet->pp[i], &stPPSetCfg.pp[i], sizeof(JPresetPoint));
	}
	return 0;
}

static int nvr_jxj_set_cruise_point_cfg(int id, int channel, void *param, user_t *u)	//因为只有一条路径，就不判断是哪条路径了
{
	int i;
	//int nRet = 0;
	JCruiseConfig *pCruiseConfig = (JCruiseConfig *)param;
	NVR_CTRL_PTZ_S stPtzBallCtrl;
	IPNC_BALL_RUN_PARAM_S stBallCfg;
	IPNC_BALL_CRUISE_PARAM_S *pBallCruise;
	pBallCruise = &stBallCfg.struSettingParam.struCruiseParam;

	CHANNEL_CHECK(channel);
	//获取高速球参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_BALL_CFG, 
					  		 &stBallCfg, sizeof(IPNC_BALL_RUN_PARAM_S)))
		return -1;
	
	switch(pCruiseConfig->action)
	{
		case PTZ_START_CRZ:	//开始巡航
			//nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_SET_CRUISE_START, channel, 0, 0);
			Nvr_Client_Crtl_Ptz(PTZ_CMD_SET_CRUISE_START, channel, 0, 0);
			break;
		case PTZ_STOP_CRZ:	//停止巡航
			//nRet = Nvr_Client_Crtl_Ptz(PTZ_CMD_STOP, channel, 0, 0);
			Nvr_Client_Crtl_Ptz(PTZ_CMD_STOP, channel, 0, 0);
			break;
		case PTZ_DEL_CRZ:	//删除巡航
			for(i = 0; i < IPNC_BALL_CRUISE_NUM; i++)
			{
				stPtzBallCtrl.u16PtzCmd = PTZ_CMD_SET_CRUISE;
				stPtzBallCtrl.struCruise.u8CruisePresetNo = i;		//只有一条路径，所有的巡航点都清0
				stPtzBallCtrl.struCruise.u8CruisePresetEnable = 0;
				//nRet = nvr_lib_send_ptz_cmd(channel, &stPtzBallCtrl);
				nvr_lib_send_ptz_cmd(channel, &stPtzBallCtrl);
				pBallCruise->struCruiseNode[i].byCruisePresetEnable = 0;
			}
			pBallCruise->crz_enable = 0;
			break;
		default:
			break;	
	}
	//设置高速球参数配置	
	if(0 > nvr_set_ipnc_param(channel,  IPNC_CMD_BALL_CFG, 
					  		 &stBallCfg, sizeof(IPNC_BALL_RUN_PARAM_S)))
		return -1;
	return 0;
}

static int nvr_jxj_get_cruise_point_cfg(int id, int channel, void *param, user_t *u)
{
	JCruiseWaySet *pCruiseWaySet = (JCruiseWaySet *)param;
	IPNC_BALL_RUN_PARAM_S stBallCfg;
	IPNC_BALL_CRUISE_PARAM_S *pBallCruise;
	pBallCruise = &stBallCfg.struSettingParam.struCruiseParam;
	
	CHANNEL_CHECK(channel);
	//获取高速球参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_BALL_CFG, 
					  		 &stBallCfg, sizeof(IPNC_BALL_RUN_PARAM_S)))
		return -1;
	
	if(pBallCruise->crz_enable == 0)
	{
		pCruiseWaySet->crz_count = 0;
	}else
	{
		pCruiseWaySet->crz_count = 1;
		strcpy((char *)&pCruiseWaySet->crz_info[0].crz_name, (char *)&pBallCruise->crz_name);
		pCruiseWaySet->crz_info[0].crz_no = stBallCfg.struSettingParam.struCruiseParam.crz_no;
	}
	return 0;
}

static int nvr_jxj_set_cruise_way_cfg(int id, int channel, void *param, user_t *u)
{
	//int nRet = 0;
	int i;
	NVR_CTRL_PTZ_S stPtzBallCtrl;
	IPNC_BALL_RUN_PARAM_S stBallCfg;
	IPNC_BALL_CRUISE_PARAM_S *pCruiseParam;
	JCruiseWay *pCruiseWay = (JCruiseWay *)param;

	CHANNEL_CHECK(channel);
	//获取高速球参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_BALL_CFG, 
					  		 &stBallCfg, sizeof(IPNC_BALL_RUN_PARAM_S)))
		return -1;
	
	pCruiseParam = &stBallCfg.struSettingParam.struCruiseParam;
	strcpy((char *)&pCruiseParam->crz_name, (char *)&pCruiseWay->crz_info.crz_name);
	pCruiseWay->crz_info.crz_no = 1;
	pCruiseParam->crz_no = 1;	//只有一条路径
	pCruiseParam->crz_enable = 1;
	for(i = 0; i < IPNC_BALL_CRUISE_NUM; i++)	//因为只有一条路径，所以添加一条路径时将前面的路径清空
	{
		stPtzBallCtrl.u16PtzCmd = PTZ_CMD_SET_CRUISE;
		stPtzBallCtrl.struCruise.u8CruisePresetNo = i;		//只有一条路径，所有的巡航点都清0
		stPtzBallCtrl.struCruise.u8CruisePresetEnable = 0;
		//nRet = nvr_lib_send_ptz_cmd(channel, &stPtzBallCtrl);
		nvr_lib_send_ptz_cmd(channel, &stPtzBallCtrl);
		pCruiseParam->struCruiseNode[i].byCruisePresetEnable = 0;
	}
	for(i = 0; i < pCruiseWay->pp_count; i++)	//设置巡航点参数
	{
		stPtzBallCtrl.u16PtzCmd = PTZ_CMD_SET_CRUISE;
		stPtzBallCtrl.struCruise.u8CruisePresetNo = pCruiseWay->crz_pp[i].preset;
		stPtzBallCtrl.struCruise.u8CruisePresetEnable = 1;
		if(pCruiseWay->crz_pp[i].dwell < 3)
			pCruiseWay->crz_pp[i].dwell = 3;
		else if(pCruiseWay->crz_pp[i].dwell > 240)
			pCruiseWay->crz_pp[i].dwell = 240;
		if(pCruiseWay->crz_pp[i].speed < 10)
			pCruiseWay->crz_pp[i].speed = 10;
		else if(pCruiseWay->crz_pp[i].speed > 120)
			pCruiseWay->crz_pp[i].speed = 120;
		stPtzBallCtrl.struCruise.u8CruisePresetSec = pCruiseWay->crz_pp[i].dwell;
		stPtzBallCtrl.struCruise.u8CruisePresetSpeed = pCruiseWay->crz_pp[i].speed;
		//nRet = nvr_lib_send_ptz_cmd(channel, &stPtzBallCtrl);
		nvr_lib_send_ptz_cmd(channel, &stPtzBallCtrl);
		if(pCruiseWay->crz_pp[i].preset <= 0 || pCruiseWay->crz_pp[i].preset > 16)
			continue;
		pCruiseParam->struCruiseNode[pCruiseWay->crz_pp[i].preset-1].byCruisePresetEnable = 1;
		pCruiseParam->struCruiseNode[pCruiseWay->crz_pp[i].preset-1].byCruisePresetSec = pCruiseWay->crz_pp[i].dwell; 
		pCruiseParam->struCruiseNode[pCruiseWay->crz_pp[i].preset-1].byCruisePresetSpeed = pCruiseWay->crz_pp[i].speed;
	}
	//设置高速球参数配置	
	if(0 > nvr_set_ipnc_param(channel,  IPNC_CMD_BALL_CFG, 
					  		 &stBallCfg, sizeof(IPNC_BALL_RUN_PARAM_S)))
		return -1;
	
	return 0;
}

static int nvr_jxj_get_cruise_way_cfg(int id, int channel, void *param, user_t *u)
{
	JCruiseWay *pCruiseWay = (JCruiseWay *)param;
	int i;
	int nCount = 0;
	IPNC_BALL_RUN_PARAM_S stBallCfg;
	IPNC_BALL_CRUISE_PARAM_S *pCruiseParam;
	
	CHANNEL_CHECK(channel);
	//获取高速球参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_BALL_CFG, 
					  		 &stBallCfg, sizeof(IPNC_BALL_RUN_PARAM_S)))
		return -1;
	
	pCruiseParam = &stBallCfg.struSettingParam.struCruiseParam;
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
	return 0;
}

static int nvr_jxj_set_venc_auto_switch_cfg(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_get_venc_auto_switch_cfg(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_3D_control(int id, int channel, void *param, user_t *u)
{
	J3DControl *p3DControl = (J3DControl *)param;
	NVR_CTRL_PTZ_S stPtzBallCtrl = {0};
	IPNC_DEV_3A_CFG_S st3ACfg;

	CHANNEL_CHECK(channel);
	//获取3A参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_3A_CFG,
					   		&st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return -1;

	stPtzBallCtrl.u16PtzCmd = PTZ_CMD_SET_3D_CONTROL;
	stPtzBallCtrl.stru3DControl.s32XCoordinate = p3DControl->x_offset;
	stPtzBallCtrl.stru3DControl.s32YCoordinate = p3DControl->y_offset;
	if(st3ACfg.struMirror.u8Value)
		stPtzBallCtrl.stru3DControl.s32XCoordinate = -stPtzBallCtrl.stru3DControl.s32XCoordinate;
	if(st3ACfg.struFlip.u8Value)
		stPtzBallCtrl.stru3DControl.s32YCoordinate = -stPtzBallCtrl.stru3DControl.s32YCoordinate;
	stPtzBallCtrl.stru3DControl.s32Amplify = p3DControl->amplify;
	/*st3DGoback.s32CurXCoordinate = stPtzBallCtrl.stru3DControl.s32XCoordinate;
	st3DGoback.s32CurYCoordinate = stPtzBallCtrl.stru3DControl.s32YCoordinate;
	st3DGoback.s32CurAmplify = stPtzBallCtrl.stru3DControl.s32Amplify;
	j_debug("the s32XCoordinate is %d", stPtzBallCtrl.stru3DControl.s32XCoordinate);
	j_debug("the s32YCoordinate is %d", stPtzBallCtrl.stru3DControl.s32YCoordinate);
	j_debug("the s32Amplify is %d", stPtzBallCtrl.stru3DControl.s32Amplify);*/

	return nvr_lib_send_ptz_cmd(channel, &stPtzBallCtrl);
}

static int nvr_jxj_get_3D_control(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_set_device_time(int id, int channel, void *param, user_t *u)
{
	JDeviceTimeExt *pDeviceTimeExt = (JDeviceTimeExt *)param;
	NVR_TIME_S stTimeCfg = {0};
	NVR_NTP_CFG_S stNtpCfg = {0};
	
	if(0 > nvr_logon_get_ntp_cfg(0, &stNtpCfg))
		return -1;
	stNtpCfg.u8NtpOpen = 0;
	stNtpCfg.nTimeZone = pDeviceTimeExt->zone *3600;
	if(0 > nvr_logon_set_ntp_cfg(0, &stNtpCfg))
		return -1;
		
	if(0 > nvr_logon_get_time_cfg(0, &stTimeCfg))
		return -1;
	stTimeCfg.u8Year = 	pDeviceTimeExt->time.year;
	stTimeCfg.u8Month= pDeviceTimeExt->time.month;
	stTimeCfg.u8Day = pDeviceTimeExt->time.date;
	stTimeCfg.u8Hour = pDeviceTimeExt->time.hour;
	stTimeCfg.u8Minute = pDeviceTimeExt->time.minute;
	stTimeCfg.u8Second = pDeviceTimeExt->time.second;
	if(0 > nvr_logon_set_time_cfg(0, &stTimeCfg))
		return -1;
	return 0;
}

static int nvr_jxj_get_set_device_time(int id, int channel, void *param, user_t *u)
{
	JDeviceTimeExt *pDeviceTimeExt = (JDeviceTimeExt *)param;
	NVR_TIME_S stTimeCfg = {0};
	NVR_NTP_CFG_S stNtpCfg = {0};

	if(0 > nvr_logon_get_ntp_cfg(0, &stNtpCfg))
		return -1;
	if(0 > nvr_logon_get_time_cfg(0, &stTimeCfg))
		return -1;

	pDeviceTimeExt->time.year = stTimeCfg.u8Year;
	pDeviceTimeExt->time.month = stTimeCfg.u8Month;
	pDeviceTimeExt->time.date = stTimeCfg.u8Day;
	pDeviceTimeExt->time.hour = stTimeCfg.u8Hour;
	pDeviceTimeExt->time.minute = stTimeCfg.u8Minute;
	pDeviceTimeExt->time.second = stTimeCfg.u8Second;
	pDeviceTimeExt->zone = stNtpCfg.nTimeZone/3600;
	return 0;
}

static int nvr_jxj_set_sync_cms_time(int id, int channel, void *param, user_t *u)
{
	NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg;
	if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg))
		return -1;
	stJxjPlatformCfg.u8SyncTimeEnable = (int)param;
	if(0 > nvr_logon_set_jxj_platform_cfg(0, &stJxjPlatformCfg))
		return -1;
	return 0;
}

static int nvr_jxj_get_sync_cms_time(int id, int channel, void *param, user_t *u)
{
	return -1;
}
/* end:	add by hjl	2013/1/7*/

static int nvr_jxj_set_alarm_link_io(int id, int channel, void *param, user_t *u)
{
	JLinkIO *pLinkIO = (JLinkIO *)param;
	IPNC_DEV_DI_CFG_S stDevDiCfg = {0};

	CHANNEL_CHECK(channel);
	//获取DI参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_DI_CFG, 
					  		 &stDevDiCfg, sizeof(IPNC_DEV_DI_CFG_S)))
		return -1;

	stDevDiCfg.struLinkAct.u32OutPut = 1;
	stDevDiCfg.struLinkAct.u32OutPersist = pLinkIO->time_len;
	
	//设置	
	if(0 > nvr_set_ipnc_param(channel, IPNC_CMD_DI_CFG, 
					  		&stDevDiCfg, sizeof(IPNC_DEV_DI_CFG_S)))
		return -1;
	return 0;
}

static int nvr_jxj_get_alarm_link_io(int id, int channel, void *param, user_t *u)
{
	return -1;
}

static int nvr_jxj_set_alarm_link_preset(int id, int channel, void *param, user_t *u)
{
	JLinkPreset *pLinkPreset = (JLinkPreset *)param;
	return Nvr_Client_Crtl_Ptz(PTZ_CMD_GOTO_PRESET, channel, 0, pLinkPreset->preset);
}

static int nvr_jxj_get_alarm_link_preset(int id, int channel, void *param, user_t *u)
{
	return -1;
}

//begin :add by hjl 2013/03/20

static int nvr_jxj_set_ir_cut_cfg(int id, int channel, void *param, user_t *u)
{
	int i = 0;
	JIrcutControl *pIrcutCtrl = (JIrcutControl *)param;
	IPNC_DEV_IRCFMODE_CFG_S stIrcfMode = {0};
	IPNC_DEV_3A_CFG_S st3ACfg = {0};

	CHANNEL_CHECK(channel);
	//获取红外参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_IRCFMODE_CFG, 
					  		 &stIrcfMode, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
		return -1;
	//获取3A参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_3A_CFG,
					   		&st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return -1;
	
	switch(pIrcutCtrl->ircut[0].ircut_mode)
	{
		case IRCUT_AUTO:
			stIrcfMode.u8Mode = 0;
			break;
		case IRCUT_RTC:
			if(pIrcutCtrl->ircut[0].rtcs.rtc)
				stIrcfMode.u8Mode = 2;
			else
				stIrcfMode.u8Mode = 1;
			break;
		case IRCUT_TIMER:
			stIrcfMode.u8Mode = 3;
			for(i=0; i<J_SDK_MAX_SEG_SZIE; i++)
			{
				stIrcfMode.struSeg[i][0].u8Open = pIrcutCtrl->ircut[0].timers[J_SDK_EVERYDAY].seg_time[i].open;
				stIrcfMode.struSeg[i][0].beginSec = pIrcutCtrl->ircut[0].timers[J_SDK_EVERYDAY].seg_time[i].begin_sec;
				stIrcfMode.struSeg[i][0].endSec = pIrcutCtrl->ircut[0].timers[J_SDK_EVERYDAY].seg_time[i].end_sec;
			}
			break;
		default:
			return -1;
	}
	
	//设置
	if(0 > nvr_set_ipnc_param(channel,  IPNC_CMD_IRCFMODE_CFG, 
					  		 &stIrcfMode, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
		return -1;
	if(0 > nvr_set_ipnc_param(channel,  IPNC_CMD_3A_CFG,
					   		&st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return -1;
	
	return 0;
}

static int nvr_jxj_get_ir_cut_cfg(int id, int channel, void *param, user_t *u)
{
	int i;
	JIrcutControl *pIrcutCtrl = (JIrcutControl *)param;
	IPNC_DEV_IRCFMODE_CFG_S stIrcfMode = {0};
	IPNC_DEV_3A_CFG_S st3ACfg = {0};

	CHANNEL_CHECK(channel);
	//获取红外参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_IRCFMODE_CFG, 
					  		 &stIrcfMode, sizeof(IPNC_DEV_IRCFMODE_CFG_S)))
		return -1;
	//获取3A参数配置	
	if(0 > nvr_get_ipnc_param(channel,  IPNC_CMD_3A_CFG,
					   		&st3ACfg, sizeof(IPNC_DEV_3A_CFG_S)))
		return -1;
	
	pIrcutCtrl->count = 1;		//	暂时只支持一个通道
	switch(stIrcfMode.u8Mode)
	{
		case 0:
			pIrcutCtrl->ircut[0].ircut_mode = IRCUT_AUTO;
			pIrcutCtrl->ircut[0].auto_c2b = 0;
			break;
		case 1:
			pIrcutCtrl->ircut[0].ircut_mode = IRCUT_RTC;
			pIrcutCtrl->ircut[0].auto_c2b = 0;
			pIrcutCtrl->ircut[0].rtcs.rtc = 0;
			break;
		case 2:
			pIrcutCtrl->ircut[0].ircut_mode = IRCUT_RTC;
			pIrcutCtrl->ircut[0].auto_c2b = 0;
			pIrcutCtrl->ircut[0].rtcs.rtc = 1;
			break;
		case 3:
			pIrcutCtrl->ircut[0].ircut_mode = IRCUT_TIMER;
			pIrcutCtrl->ircut[0].auto_c2b = 0;
			for(i=0; i<J_SDK_MAX_SEG_SZIE; i++)
			{
				pIrcutCtrl->ircut[0].timers[7].seg_time[i].open = stIrcfMode.struSeg[i][0].u8Open;
				pIrcutCtrl->ircut[0].timers[7].seg_time[i].begin_sec = stIrcfMode.struSeg[i][0].beginSec;
				pIrcutCtrl->ircut[0].timers[7].seg_time[i].end_sec = stIrcfMode.struSeg[i][0].endSec;
			}
			break;
		default:
			break;
	}

	return 0;
}

static int nvr_jxj_set_3D_goback(int id, int channel, void *param, user_t *u)
{
/*	J3DControl st3DControl = {0};
	st3DControl.x_offset = -st3DGoback.s32CurXCoordinate;
	st3DControl.y_offset = -st3DGoback.s32CurYCoordinate;
	st3DControl.amplify = -st3DGoback.s32CurAmplify;
	hi_jxj_set_3D_control(id, channel, &st3DControl, u);
	return 0;
	*/
	return -1;
}

static int nvr_jxj_get_3D_goback(int id, int channel, void *param, user_t *u)
{
	return -1;
}

typedef struct _P2P_VIVI_MSG
{
	long msgtype;
	long cmdtype;
	char data[64];
}P2P_VIVI_MSG;

static int nvr_jxj_set_p2p_id(int id, int channel, void *param, user_t *u)
{
	JP2PId *pP2PId = (JP2PId *)param;
	int msgid;
	key_t keyval;
	P2P_VIVI_MSG stViviMsg = {0};

	keyval=ftok("/opt/nvr/p2p_vivi_platform", 0xabab);
	msgid=msgget(keyval, 0);
	if(msgid < 0)
		return -1;
	stViviMsg.msgtype = 1;
	if(strlen(pP2PId->p2p_id))
	{
		stViviMsg.cmdtype = 1;	// open vivi
		strcpy(stViviMsg.data, pP2PId->p2p_id);
	}
	else
		stViviMsg.cmdtype = 2;	// close vivi
	msgsnd(msgid, &stViviMsg, sizeof(stViviMsg)-sizeof(long), IPC_NOWAIT);
	return 0;
}

static int nvr_jxj_get_p2p_id(int id, int channel, void *param, user_t *u)
{
	return -1;
}



static const JXJ_FUNC_STRUCT jxj_func_table[] =
{
	//{ PARAM_REGISTER_REQUEST,  nvr_jxj_set_register_request, nvr_jxj_get_register_request},			//注册请求
	//{ PARAM_REGISTER_RESPONSE,  nvr_jxj_set_register_response, nvr_jxj_get_register_response},		//注册响应
	//{ PARAM_HEART_BEAT_REQUEST,  nvr_jxj_set_heart_beat_request, nvr_jxj_get_heart_beat_request},		//心跳请求
	//{ PARAM_HEART_BEAT_RESPONSE,  nvr_jxj_set_heart_beat_response, nvr_jxj_get_heart_beat_response},	//心跳响应
	{0, NULL, NULL},
	{1, NULL, NULL},
	{2, NULL, NULL},
	{ PARAM_SYNC_TIME, nvr_jxj_set_sync_time, nvr_jxj_get_sync_time},							//同步服务器时间
	{ PARAM_CHANGE_DISPATCH,  nvr_jxj_set_change_dispatch, nvr_jxj_get_change_dispatch},		//更改流媒体服务器
	
	{ PARAM_DEVICE_INFO,  nvr_jxj_set_device_info, nvr_jxj_get_device_info},					//设备出厂信息
	{ PARAM_DEVICE_NTP_INFO,  nvr_jxj_set_device_ntp_info, nvr_jxj_get_device_ntp_info},		//NTP 信息
	{ PARAM_DEVICE_TIME,  nvr_jxj_set_device_time, nvr_jxj_get_device_time},					//设置时间信息
	{ PARAM_PLATFORM_INFO,  nvr_jxj_set_platform_info, nvr_jxj_get_platform_info},				//平台信息
	{ PARAM_NETWORK_INFO,  nvr_jxj_set_network_info, nvr_jxj_get_network_info},					//网络信息
	{ PARAM_PPPOE_INFOE,  nvr_jxj_set_pppoe_info, nvr_jxj_get_pppoe_info},						//PPPOE拨号信息
	{ PARAM_ENCODE_INFO,  nvr_jxj_set_encode_info, nvr_jxj_get_encode_info},					//编码参数
	{ PARAM_DISPLAY_INFO,  nvr_jxj_set_display_info, nvr_jxj_get_display_info},					//显示参数
	{ PARAM_RECORD_INFO,  nvr_jxj_set_record_info, nvr_jxj_get_record_info},					//录像参数
	{ PARAM_HIDE_INFO,  nvr_jxj_set_hide_info, nvr_jxj_get_hide_info},							//遮挡参数
	{ PARAM_SERIAL_INFO,  nvr_jxj_set_serail_info, nvr_jxj_get_serail_info},					//串口参数
	{ PARAM_OSD_INFO,  nvr_jxj_set_osd_info, nvr_jxj_get_osd_info},								//OSD参数
	{ PARAM_PTZ_INFO,  nvr_jxj_set_ptz_info, nvr_jxj_get_ptz_info},								//云台信息
	{ PARAM_FTP_INFO,  nvr_jxj_set_ftp_info, nvr_jxj_get_ftp_info},								//ftp 参数
	{ PARAM_SMTP_INFO,  nvr_jxj_set_smtp_info, nvr_jxj_get_smtp_info},							//smtp 参数
	{ PARAM_UPNP_INFO,  nvr_jxj_set_upnp_info, nvr_jxj_get_upnp_info},							//upnp 参数
	{ PARAM_DISK_INFO,  nvr_jxj_set_disk_info, nvr_jxj_get_disk_info},							//磁盘信息
	{ PARAM_FORMAT_DISK,  nvr_jxj_set_format_disk, nvr_jxj_get_format_disk},					//格式磁盘
	{ PARAM_MOVE_ALARM,  nvr_jxj_set_move_alarm, nvr_jxj_get_move_alarm},						//移动告警
	{ PARAM_LOST_ALARM,  nvr_jxj_set_lost_alarm, nvr_jxj_get_lost_alarm},						//丢失告警
	{ PARAM_HIDE_ALARM,  nvr_jxj_set_hide_alarm, nvr_jxj_get_hide_alarm},						//遮挡告警
	{ PARAM_IO_ALARM,  nvr_jxj_set_io_alarm, nvr_jxj_get_io_alarm},								//IO 告警
	{ PARAM_JOINT_INFO,  nvr_jxj_set_joint_info, nvr_jxj_get_joint_info},						//联动操作
	{ PARAM_CONTROL_PTZ,  nvr_jxj_set_control_ptz, nvr_jxj_get_control_ptz},					//云镜控制
	{ PARAM_SUBMINT_ALARM,  nvr_jxj_set_submit_alarm, nvr_jxj_get_submit_alarm},				//上报告警
	{ PARAM_MEDIAL_URL, nvr_jxj_set_medial_url,nvr_jxj_get_medial_url},							//流媒体 URL
	{ PARAM_STORE_LOG, nvr_jxj_set_store_log, nvr_jxj_get_store_log},							//视频检索}
	{ PARAM_USER_LOGIN, nvr_jxj_set_user_login, nvr_jxj_get_user_login},						//用户登录请求
	{ PARAM_FIRMWARE_UPGRADE, nvr_jxj_set_firmware_upgrade, nvr_jxj_get_firmware_upgrade},		//固件升级
	{ PARAM_DEVICE_CAP, nvr_jxj_set_device_cap, nvr_jxj_get_device_cap},						//设备能力集
	{ PARAM_SEARCH_DEV, nvr_jxj_set_search_dev, nvr_jxj_get_search_dev},						//局域网设备搜索
	{ PARAM_CHANNEL_INFO, nvr_jxj_set_channel_info, nvr_jxj_get_channel_info},					//设备通道配置
	{ PARAM_PIC_INFO, nvr_jxj_set_pic_info, nvr_jxj_get_pic_info},								//图像参数
	{ PARAM_WIFI_INFO, nvr_jxj_set_wifi_info, nvr_jxj_get_wifi_info},							//wifi 配置
	{ PARAM_WIFI_SEARCH, nvr_jxj_set_wifi_search, nvr_jxj_get_wifi_search},						//wifi 搜索
	{ PARAM_USER_CONFIG, nvr_jxj_set_user_config, nvr_jxj_get_user_config},						//用户配置信息
	{ PARAM_CONTROL_DEV, nvr_jxj_set_control_dev, nvr_jxj_get_control_dev},						//控制设备
	{ PARAM_NETWORK_STATUS, nvr_jxj_set_network_status, nvr_jxj_get_network_status},			//网络连接状态
	{ PARAM_INTEREST_CODING, nvr_jxj_set_interest_coding, nvr_jxj_get_interest_coding},			//感兴趣区域编码
	{ PARAM_DDNS_CODING, nvr_jxj_set_ddns_coding, nvr_jxj_get_ddns_coding},						//动态域名服务
	
	{ PARAM_DEF_DISPLAY_INFO, nvr_jxj_set_def_display_info, nvr_jxj_get_def_display_info},		//默认显示参数
	{ PARAM_DEF_PICTURE_INFO, nvr_jxj_set_def_picture_info, nvr_jxj_get_def_picture_info},		//默认图像参数
	{ PARAM_HXHT_CONFIG, nvr_jxj_set_hxht_config, nvr_jxj_get_hxht_config},
	{ PARAM_MODIFY_USER_CFG, nvr_jxj_set_modify_user_config, nvr_jxj_get_modify_user_config},
	{ PARAM_AVD_CFG, nvr_jxj_set_avd_config, nvr_jxj_get_avd_config},
	{PARAM_GB28181_PROTO_CFG, nvr_jxj_set_gb28181_config, nvr_jxj_get_gb28181_config},
	{PARAM_NETWORK_DISK_CFG, nvr_jxj_set_network_disk_config, nvr_jxj_get_network_disk_config},
	{PARAM_DEV_WORK_STATE, nvr_jxj_set_dev_work_state, nvr_jxj_get_dev_work_state},				//设备状态信息
	
/* begin:	add by hjl	2013/1/7*/	
	{PARAM_OPERATION_LOG, nvr_jxj_set_operation_log, nvr_jxj_get_operation_log},				//获取用户操作记录
	{PARAM_ALARM_UPLOAD_CFG, nvr_jxj_set_alarm_upload_cfg, nvr_jxj_get_alarm_upload_cfg},		//报警上传配置
	
	{PARAM_PRESET_POINT_CFG, nvr_jxj_set_preset_point_cfg, nvr_jxj_get_preset_point_cfg},		//预置点配置
	{PARAM_CRUISE_POINT_CFG, nvr_jxj_set_cruise_point_cfg, nvr_jxj_get_cruise_point_cfg},		//巡航路径(查询巡航路径号集合，启动、停止、删除某一巡航号)
	{PARAM_CRUISE_WAY_CFG, nvr_jxj_set_cruise_way_cfg, nvr_jxj_get_cruise_way_cfg},				//巡航路径(查询、添加、修改某一巡航路径信息)
	{PARAM_VENC_AUTO_SWITCH, nvr_jxj_set_venc_auto_switch_cfg, nvr_jxj_get_venc_auto_switch_cfg},//通道码率自动切换设置
	{PARAM_3D_CONTROL, nvr_jxj_set_3D_control, nvr_jxj_get_3D_control},							//3D 控制
	{PARAM_SET_DEVICE_TIME, nvr_jxj_set_set_device_time, nvr_jxj_get_set_device_time},			//手动设置时间 JDeviceTimeExt
	{PARAM_SYNC_CMS_TIME, nvr_jxj_set_sync_cms_time, nvr_jxj_get_sync_cms_time},				//设置同步cms 时间标记
/* end:	add by hjl	2013/1/7*/
//add by hjl 2013/03/20
	{62, NULL, NULL},
	{63, NULL, NULL},
	{64, NULL, NULL},
	{PARAM_ALARM_LINK_IO, nvr_jxj_set_alarm_link_io, nvr_jxj_get_alarm_link_io},
	{PARAM_ALARM_LINK_PRESET, nvr_jxj_set_alarm_link_preset, nvr_jxj_get_alarm_link_preset},
	{67, NULL, NULL},
	{PARAM_IRCUT_CONTROL, nvr_jxj_set_ir_cut_cfg, nvr_jxj_get_ir_cut_cfg},
	{PARAM_3D_GOBACK, nvr_jxj_set_3D_goback, nvr_jxj_get_3D_goback},
	{PARAM_EXTRANET_PORT, NULL, NULL},
	{PARAM_HW_PF_CFG, NULL, NULL},
	{PARAM_P2P_ID, nvr_jxj_set_p2p_id, nvr_jxj_get_p2p_id},
};
		
#ifdef __DEBUG__
static const char *get_id_string(int id)
{
	static const char id_str[][64] = 
	{
		//"PARAM_REGISTER_REQUEST",			//注册请求
		//"PARAM_REGISTER_RESPONSE",		//注册响应
		//"PARAM_HEART_BEAT_REQUEST",		//心跳请求
		//"PARAM_HEART_BEAT_RESPONSE",		//心跳响应
		"0","1","2",
		"PARAM_SYNC_TIME",					//同步服务器时间
		"PARAM_CHANGE_DISPATCH",			//更改流媒体服务器
		"PARAM_DEVICE_INFO",				//设备出厂信息1.1
		"PARAM_DEVICE_NTP_INFO",			//NTP 信息3.7
		"PARAM_DEVICE_TIME",				//设置时间信息3.6
		"PARAM_PLATFORM_INFO",				//平台信息1.2
		"PARAM_NETWORK_INFO",				//网络信息3.4
		"PARAM_PPPOE_INFOE",				//PPPOE拨号信息3.11
		"PARAM_ENCODE_INFO",				//编码参数1.3
		"PARAM_DISPLAY_INFO",				//显示参数1.4
		"PARAM_RECORD_INFO",				// 录像参数2.1
		"PARAM_HIDE_INFO",					//遮挡参数3.2
		"PARAM_SERIAL_INFO",				//串口参数3.5
		"PARAM_OSD_INFO",					// OSD参数1.5
		"PARAM_PTZ_INFO",					//云台信息2.8
		"PARAM_FTP_INFO",					//ftp 参数3.8
		"PARAM_SMTP_INFO",					//smtp 参数3.9
		"PARAM_UPNP_INFO",					//upnp 参数3.10
		"PARAM_DISK_INFO",					//磁盘信息3.3.1
		"PARAM_FROMAT_DISK",				//格式磁盘3.3
		"PARAM_MOVE_ALARM",					//移动告警2.2
		"PARAM_LOST_ALARM",					//丢失告警2.3
		"PARAM_HIDE_ALARM",					//遮挡告警2.4
		"PARAM_IO_ALARM",					//IO 告警2.5
		"PARAM_JOINT_INFO",					//联动操作2.6
		"PARAM_CONTROL_PTZ",				//云镜控制2.7
		"PARAM_SUBMINT_ALARM",				//上报告警3.1
		"PARAM_MEDIAL_URL",					//流媒体 URL
		"PARAM_STORE_LOG",					//视频检索
		"PARAM_USER_LOGIN",					//用户登录请求
		"PARAM_FIRMWARE_UPGRADE",			//固件升级
		"PARAM_DEVICE_CAP",					//设备能力集
		"PARAM_SEARCH_DEV",					//局域网设备搜索
		"PARAM_CHANNEL_INFO",				//设备通道配置
		"PARAM_PIC_INFO",					//图像参数
		"PARAM_WIFI_INFO",					//wifi 配置
		"PARAM_WIFI_SEARCH",				//wifi 搜索
		"PARAM_USER_CONFIG",				//用户配置信息
		"PARAM_CONTROL_DEV",				//控制设备
		"PARAM_NETWORK_STATUS",				//网络连接状态
		"PARAM_INTEREST_CODING",			//感兴趣区域编码
		"PARAM_DDNS_CODING",				//动态域名服务
		
		"PARAM_DEF_DISPLAY_INFO",			//默认显示参数
		"PARAM_DEF_PICTURE_INFO",			//默认图像参数
		"PARAM_HXHT_CONFIG",
		"PARAM_MODIFY_USER_CFG",
		"PARAM_AVD_CFG",
		"PARAM_GB28181_PROTO_CFG",
		"PARAM_NETWORK_DISK_CFG",	
		"PARAM_DEV_WORK_STATE",				//设备状态信息
/* begin:	add by hjl	2013/1/7*/

		"PARAM_OPERATION_LOG",				//获取用户操作记录
		"PARAM_ALARM_UPLOAD_CFG",			//报警上传配置
		
		"PARAM_PRESET_POINT_CFG",			//预置点配置
		"PARAM_CRUISE_POINT_CFG",			//巡航路径(查询巡航路径号集合，启动、停止、删除某一巡航号)
		"PARAM_CRUISE_WAY_CFG",				//巡航路径(查询、添加、修改某一巡航路径信息)
		"PARAM_VENC_AUTO_SWITCH",			//通道码率自动切换设置
		"PARAM_3D_CONTROL",					// 3D 控制
		"PARAM_SET_DEVICE_TIME",			//手动设置时间 JDeviceTimeExt
		"PARAM_SYNC_CMS_TIME",				//设置同步cms 时间标记
/* end:	add by hjl	2013/1/7*/
//add by hjl 2013/03/20
		"PARAM_IR_CUT_CFG",					//IR-CUT切换模式参数设置项
		"63",
		"64",
		"PARAM_ALARM_LINK_IO",
		"PARAM_ALARM_LINK_PRESET",
		"PARAM_RESOLUTION_CFG",
		"PARAM_IRCUT_CONTROL",
		"PARAM_3D_GOBACK",

	};

	if(id >= 0 && id < PARAM_MAX_SIZE)
	{
		return id_str[id];
	}
	return "NULL";
}
#endif
int nvr_jxj_get_device_param(int id, int channel, void *param, user_t *u)
{
	int ret = -1;

	//if(id != PARAM_FIRMWARE_UPGRADE)
	//	j_debug("begin-->nvr platform gettting: id[%s], channel[%d]", get_id_string(id), channel);
	if (PARAM_SYNC_CMS_TIME != id && NULL == param && PARAM_3D_GOBACK != id)
	{
		j_warning("nvr platform getting param NULL.");
		return -1;
	}
	if (PARAM_MAX_SIZE <= id || PARAM_START >= id)
	{
		j_warning("nvr platform getting id invalid!");
		return -1;
	}
	if(id < sizeof(jxj_func_table) / sizeof(jxj_func_table[0]))
	{
		if(jxj_func_table[id].id != id)
		{
			j_warning("The nvr function table has some fault, please check the code.");
			return -1;
		}

		if(jxj_func_table[id].get_func != NULL)
		{
			ret = jxj_func_table[id].get_func(id, channel, param, u);
		}
	}
	//if(id != PARAM_FIRMWARE_UPGRADE)
	//	j_debug("end-->nvr platform gettting: id[%s], channel[%d], ret[%d]", get_id_string(id), channel, ret);

	return ret;
}

int nvr_jxj_set_device_param(int id, int channel, void *param, user_t *u)
{
	int ret = -1;

	//if(id != PARAM_FIRMWARE_UPGRADE)
	//	j_debug("begin-->nvr platform settting: id[%s], channel[%d]", get_id_string(id), channel);
	if (PARAM_SYNC_CMS_TIME != id && NULL == param && PARAM_3D_GOBACK != id)	//当是PARAM_SYNC_CMS_TIME时，进来的不是一个指针，而是直接一个变量
	{
		j_warning("nvr platform setting param NULL.");
		return -1;
	}
	if (PARAM_MAX_SIZE <= id || PARAM_START >= id)
	{
		j_warning("nvr platform setting id invalid!");
		return -1;
	}
	if(id < sizeof(jxj_func_table) / sizeof(jxj_func_table[0]))
	{
		if(jxj_func_table[id].id != id)
		{
			j_warning("The nvr function table has some fault, please check the code.");
			return -1;
		}

		if(jxj_func_table[id].set_func != NULL)
		{

			ret = jxj_func_table[id].set_func(id, channel, param, u);
		}
	}
	//if(id != PARAM_FIRMWARE_UPGRADE)
	//	j_debug("end-->nvr platform settting: id[%s], channel[%d], ret[%d]", get_id_string(id), channel, ret);

	return ret;
}

static int nvr_jxj_get_sys_cfg(int id, int channel, void *param, user_t *u)
{
	NVR_DEV_INFO_S stDevInfo;
	NVR_DEV_STABLE_INFO_S *pStableInfo;
	
	J_SysCfg_T *pSysCfg = (J_SysCfg_T *)param;

	if(0 > nvr_logon_get_dev_cfg(0, &stDevInfo))
		return -1;
	
	pStableInfo = &stDevInfo.stStableInfo;
	
	strncpy((char *)pSysCfg->serial_no, pStableInfo->szSerialId, sizeof(pSysCfg->serial_no) - 1);
	strncpy((char *)pSysCfg->device_type, "NVR", sizeof(pSysCfg->device_type) - 1);
	strncpy((char *)pSysCfg->software_ver, pStableInfo->szSoftVer,sizeof(pSysCfg->software_ver) - 1);
	snprintf((char *)pSysCfg->software_date, sizeof(pSysCfg->software_date) - 1, "%s", __DATE__);
	strncpy((char *)pSysCfg->hardware_ver, pStableInfo->szHwVer,sizeof(pSysCfg->hardware_ver) - 1);
	pSysCfg->dev_type = DEV_TYPE;
	pSysCfg->ana_chan_num = 0;
	pSysCfg->ip_chan_num = pStableInfo->u8ChnNum;
	pSysCfg->dec_chan_num = 2;
	pSysCfg->stream_num = 2;

	pSysCfg->audio_in_num = 1;
	pSysCfg->audio_out_num = 1;
	pSysCfg->alarm_in_num = pStableInfo->u8AlarmInputNum;
	pSysCfg->alarm_out_num = pStableInfo->u8AlarmOutputNum;

	pSysCfg->net_port_num = pStableInfo->u8NetNum;
	pSysCfg->rs232_num = pStableInfo->u8RS232Num;
	pSysCfg->rs485_num = pStableInfo->u8RS485Num;
	pSysCfg->usb_num = 0;
	return 0;
}

static int nvr_jxj_get_manuf_cfg(int id, int channel, void *param, user_t *u)
{
	J_ManufCfg_T    *pManufCfg = (J_ManufCfg_T *)param;
	
	NVR_DEV_NET_CFG_S stNetCfg;
	NVR_DEV_STABLE_INFO_S stStableInfo;
	NVR_DEV_STABLE_INFO_S *pStableInfo;

	memset(&stNetCfg, 0, sizeof(NVR_DEV_NET_CFG_S));
	memset(&stStableInfo, 0, sizeof(NVR_DEV_STABLE_INFO_S));
	
	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
		return -1;
	if(0 > nvr_logon_get_stable_cfg(0, &stStableInfo))
		return -1;
	pStableInfo = &stStableInfo;
	
	strncpy((char *)pManufCfg->serial_no, (char *)pStableInfo->szSerialId, sizeof(pManufCfg->serial_no) - 1);
	strncpy((char *)pManufCfg->device_type, "NVR", sizeof(pManufCfg->device_type) - 1);
	pManufCfg->dev_type = DEV_TYPE;
	pManufCfg->ana_chan_num = pStableInfo->u8ChnNum;
	pManufCfg->ip_chan_num = pStableInfo->u8ChnNum;
	pManufCfg->dec_chan_num = 0;
	pManufCfg->stream_num = 2;
	pManufCfg->netport_num = pStableInfo->u8NetNum;
	snprintf((char *)pManufCfg->mac, sizeof(pManufCfg->mac),
			"%02X:%02X:%02X:%02X:%02X:%02X",
			stNetCfg.struEtherCfg[0].u8MacAddr[0],
			stNetCfg.struEtherCfg[0].u8MacAddr[1],
			stNetCfg.struEtherCfg[0].u8MacAddr[2],
			stNetCfg.struEtherCfg[0].u8MacAddr[3],
			stNetCfg.struEtherCfg[0].u8MacAddr[4],
			stNetCfg.struEtherCfg[0].u8MacAddr[5]);	
	if(pStableInfo->u8NetNum == 2)
	{
		snprintf((char *)pManufCfg->mac2, sizeof(pManufCfg->mac2),
			"%02X:%02X:%02X:%02X:%02X:%02X",
			stNetCfg.struEtherCfg[1].u8MacAddr[0],
			stNetCfg.struEtherCfg[1].u8MacAddr[1],
			stNetCfg.struEtherCfg[1].u8MacAddr[2],
			stNetCfg.struEtherCfg[1].u8MacAddr[3],
			stNetCfg.struEtherCfg[1].u8MacAddr[4],
			stNetCfg.struEtherCfg[1].u8MacAddr[5]);
	}

	pManufCfg->platform_type = PF_JXJ;
	pManufCfg->ivs_type = 0;
	pManufCfg->net_protocol = PROTOCOL_CUSTOM | PROTOCOL_RTSP;
	return 0;
}


static int nvr_jxj_set_dhcp_cfg(int id, int channel, void *param, user_t *u)
{
	J_DhcpCtl_T *pDhcpCtl = (J_DhcpCtl_T *)param;
	NVR_DEV_NET_CFG_S stNetCfg;

	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
		return -1;
	stNetCfg.struEtherCfg[0].u8DhcpOn = pDhcpCtl->dhcp_en;
	if(0 > nvr_logon_set_net_cfg(0, &stNetCfg))
		return -1;
	return 0;
}

static int nvr_jxj_get_dhcp_cfg(int id, int channel, void *param, user_t *u)
{
	J_DhcpCtl_T *pDhcpCtl = (J_DhcpCtl_T *)param;
	NVR_DEV_NET_CFG_S stNetCfg;

	if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
		return -1;
	pDhcpCtl->dhcp_en = stNetCfg.struEtherCfg[0].u8DhcpOn;
	pDhcpCtl->if_type = J_SDK_ETH0;
	return 0;
}

int nvr_jxj_search_device_cb(pu_process_cb_t *handle, mb_pu_parm_t *parm)
{
	if(parm == NULL)
	{
		j_warning("The pu param is null.");
		return MB_NOTIFY_ERR_FAILED;
	}
	int size;
	unsigned int data[6];
	
	//j_debug("nvr_jxj_search_device_cb ............");
	//j_debug("ID : 0x%08x", parm->id);
	//j_debug("args : %d", parm->args);
	//j_debug("user : %s", parm->user);
	//j_debug("pass : %s", parm->pass);
	
	if(parm->args == MB_MSG_SET)
	{
		//j_debug("Set device param...");
		switch(parm->id)
		{
			case J_MB_Device_Id:
			{
				//j_debug("Set device J_MB_Device_Id");
				J_Device_T device;
				if(*parm->size < sizeof(device))
				{
					j_debug("The size is to small.");
					return MB_NOTIFY_ERR_FAILED;
				}
				memcpy(&device, parm->data, sizeof(device));
				nvr_jxj_set_network_info(0, 0, &device.NetworkInfo, NULL);
				*parm->size = sizeof(J_Device_T);
				break;
			}
			case J_MB_SysCfg_Id:
			{
				//j_debug("Set device J_MB_SysCfg_Id");
				break;
			}
			case J_MB_NetCfg_Id:
			{
				//j_debug("Set device J_MB_NetCfg_Id");
				JNetworkInfo NetworkInfo;
				if(*parm->size < sizeof(NetworkInfo))
				{
					j_warning("The size is to small.");
					return MB_NOTIFY_ERR_FAILED;
				}
				memcpy(&NetworkInfo, parm->data, sizeof(NetworkInfo));
				nvr_jxj_set_network_info(0, 0, &NetworkInfo, NULL);
				*parm->size = sizeof(JNetworkInfo);
				break;
			}
			case J_MB_DvrCommCfg_Id:
				break;
			case J_MB_ManufCfg_Id:
			{
				break;
			}
			case J_MB_DHCPCtl_Id:
			{
				J_DhcpCtl_T    DhcpCtl;
				if(*parm->size < sizeof(DhcpCtl))
				{
					j_warning("The size is to small.");
					return MB_NOTIFY_ERR_FAILED;
				}
				memcpy(&DhcpCtl, parm->data, sizeof(DhcpCtl));
				nvr_jxj_set_dhcp_cfg(0, 0, &DhcpCtl, NULL);
				*parm->size = sizeof(J_DhcpCtl_T);
				break;
			}
			case J_MB_SysStatus_Id:
			{
				break;
			}
			case J_MB_Jpf_Search_Id:
			{
				//j_debug("Set device J_MB_Jpf_Search_Id");
				Jpf_Search *pPfSearch = (Jpf_Search *)parm->data;
				NVR_DEV_NET_CFG_S stNetCfg = {0};
				NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg = {0};

				if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
					return -1;
				if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg))
					return -1;

				sscanf(pPfSearch->dev_info.dev_ip, "%u.%u.%u.%u",
					&data[0], &data[1], &data[2], &data[3]);
				stNetCfg.struEtherCfg[0].u32IpAddr = 
					(data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

				size = sizeof(stJxjPlatformCfg.szPuId);
				strncpy(stJxjPlatformCfg.szPuId, pPfSearch->dev_info.pu_id, size-1);
				stJxjPlatformCfg.szPuId[size-1] = '\0';
				
				size = sizeof(stJxjPlatformCfg.szCmsIp);
				strncpy(stJxjPlatformCfg.szCmsIp, pPfSearch->jpf_plt.cms_ip, size-1);
				stJxjPlatformCfg.szCmsIp[size-1] = '\0';
				
				/*
				size = sizeof(stJxjPlatformCfg.szCmsIp);
				strncpy(stJxjPlatformCfg.szCmsIp, pPfSearch->jpf_plt.mds_ip, size-1);
				stJxjPlatformCfg.szCmsIp[size-1] = '\0';
				*/

				stJxjPlatformCfg.u16CmsPort = pPfSearch->jpf_plt.cms_port;
				stJxjPlatformCfg.u8Enable = pPfSearch->jpf_plt.conn_cms;

				if(0 > nvr_logon_set_net_cfg(0, &stNetCfg))
					return -1;
				if(0 > nvr_logon_set_jxj_platform_cfg(0, &stJxjPlatformCfg))
					return -1;
				*parm->size = sizeof(Jpf_Search);
				break;
			}
			case J_MB_Jpf_Redirect_Id:
			{
				Jpf_Redirect *pRedirect = (Jpf_Redirect *)parm->data;
				NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg = {0};
				if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg))
					return -1;

				size = sizeof(stJxjPlatformCfg.szPuId);
				strncpy(stJxjPlatformCfg.szPuId, pRedirect->pu_id, size-1);
				stJxjPlatformCfg.szPuId[size-1] = '\0';
				size = sizeof(stJxjPlatformCfg.szCmsIp);
				strncpy(stJxjPlatformCfg.szCmsIp, pRedirect->cms_ip, size-1);
				stJxjPlatformCfg.szCmsIp[size-1] = '\0';

				stJxjPlatformCfg.u16CmsPort = pRedirect->cms_port;
				stJxjPlatformCfg.u8Enable = pRedirect->conn_cms;
				
				if(0 > nvr_logon_set_jxj_platform_cfg(0, &stJxjPlatformCfg))
					return -1;
				*parm->size = sizeof(Jpf_Redirect);
				break;
			}
			default:
				break;
		}
	}
	else if(parm->args == MB_MSG_GET)
	{
		//j_debug("Get device param...");
		switch(parm->id)
		{
			case J_MB_Device_Id:
			{
				//j_debug("Get device J_MB_Device_Id");
				J_Device_T device;

				if(*parm->size < sizeof(device))
				{
					j_warning("The size is to small.");
					return -1;
				}
	
				memset(&device, 0, sizeof(device));
				device.SysSize = sizeof(device.SysCfg);
				device.NetSize = sizeof(device.NetworkInfo);
				nvr_jxj_get_sys_cfg(0, 0, &device.SysCfg, NULL);

				nvr_jxj_get_network_info(0, 0, &device.NetworkInfo, NULL);
				*parm->size = sizeof(device);
				memcpy(parm->data, &device, sizeof(device));
				*parm->size = sizeof(J_Device_T);
				break;
			}
			case J_MB_SysCfg_Id: 
			{
				//j_debug("Get device J_MB_SysCfg_Id");
				J_SysCfg_T SysCfg;
				if(*parm->size < sizeof(SysCfg))
				{
					j_warning("The size is to small.");
					return MB_NOTIFY_ERR_FAILED;
				}
				memset(&SysCfg, 0, sizeof(SysCfg));
				nvr_jxj_get_sys_cfg(0, 0, &SysCfg, NULL);
				*parm->size = sizeof(SysCfg);
				memcpy(parm->data, &SysCfg, sizeof(SysCfg));
				*parm->size = sizeof(J_SysCfg_T);
				break;
			}
			case J_MB_NetCfg_Id:
			{	
				//j_debug("Get device J_MB_NetCfg_Id");
				JNetworkInfo    NetworkInfo;
				if(*parm->size < sizeof(NetworkInfo))
				{
					j_warning("The size is to small.");
					return MB_NOTIFY_ERR_FAILED;
				}
				memset(&NetworkInfo, 0, sizeof(NetworkInfo));
				nvr_jxj_get_network_info(0, 0, &NetworkInfo, NULL);
				*parm->size = sizeof(NetworkInfo);
				memcpy(parm->data, &NetworkInfo, sizeof(NetworkInfo));
				*parm->size = sizeof(JNetworkInfo);
				break;
			}
			case J_MB_DvrCommCfg_Id:
				break;
			case J_MB_ManufCfg_Id:
			{	
				J_ManufCfg_T    ManufCfg;
				if(*parm->size < sizeof(ManufCfg))
				{
					j_warning("The size is to small.");
					return MB_NOTIFY_ERR_FAILED;
				}
				memset(&ManufCfg, 0, sizeof(ManufCfg));
				nvr_jxj_get_manuf_cfg(0, 0, &ManufCfg, NULL);
				*parm->size = sizeof(ManufCfg);
				memcpy(parm->data, &ManufCfg, sizeof(ManufCfg));
				*parm->size = sizeof(J_ManufCfg_T);
				break;
			}
			case J_MB_DHCPCtl_Id:
			{	
				J_DhcpCtl_T    DhcpCtl;
				if(*parm->size < sizeof(DhcpCtl))
				{
					j_warning("The size is to small.");
					return MB_NOTIFY_ERR_FAILED;
				}
				memset(&DhcpCtl, 0, sizeof(DhcpCtl));
				nvr_jxj_get_dhcp_cfg(0, 0, &DhcpCtl, NULL);
				*parm->size = sizeof(DhcpCtl);
				memcpy(parm->data, &DhcpCtl, sizeof(DhcpCtl));
				*parm->size = sizeof(J_DhcpCtl_T);
				break;
			}
			case J_MB_SysStatus_Id:
			{
				J_SysStatus_T SysStatus;
				struct sysinfo info;
				
				if(*parm->size < sizeof(SysStatus))
				{
					j_warning("The size is to small.");
					return MB_NOTIFY_ERR_FAILED;
				}
				memset(&info, 0, sizeof(info));
				sysinfo(&info);
				j_debug("run time : %ld", info.uptime);
				memset(&SysStatus, 0, sizeof(SysStatus));
				SysStatus.run_time = info.uptime;
				*parm->size = sizeof(SysStatus);
				memcpy(parm->data, &SysStatus, sizeof(SysStatus));
				*parm->size = sizeof(J_SysStatus_T);
				break;
			}
			case J_MB_Jpf_Search_Id:
			{
				//j_debug("Get device J_MB_Jpf_Search_Id");
				Jpf_Search *pPfSearch = (Jpf_Search *)parm->data;
				NVR_DEV_NET_CFG_S stNetCfg = {0};
				NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg = {0};

				if(0 > nvr_logon_get_net_cfg(0, &stNetCfg))
					return -1;
				if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg))
					return -1;
				pPfSearch->dev_info.pu_type = PU_TYPE;
				pPfSearch->dev_info.av_mun = 1;
				size = sizeof(pPfSearch->dev_info.mnfct);
				strncpy((char *)pPfSearch->dev_info.mnfct, "JXJ", size-1);
				pPfSearch->dev_info.mnfct[size-1] = '\0';

				size = sizeof(pPfSearch->dev_info.pu_id);
				strncpy(pPfSearch->dev_info.pu_id, (char *)stJxjPlatformCfg.szPuId, size-1);
				pPfSearch->dev_info.pu_id[size-1] = '\0';

				size = sizeof(pPfSearch->dev_info.dev_ip);
				snprintf((char *)pPfSearch->dev_info.dev_ip, size - 1, "%d.%d.%d.%d",
						(stNetCfg.struEtherCfg[0].u32IpAddr >> 24) & 0xff,
						(stNetCfg.struEtherCfg[0].u32IpAddr >> 16) & 0xff,
						(stNetCfg.struEtherCfg[0].u32IpAddr >> 8) & 0xff,
						(stNetCfg.struEtherCfg[0].u32IpAddr >> 0) & 0xff);
						
				pPfSearch->dev_info.dev_ip[size -1] = '\0';

				size = sizeof(pPfSearch->jpf_plt.cms_ip);
				strncpy(pPfSearch->jpf_plt.cms_ip, stJxjPlatformCfg.szCmsIp, size-1);
				pPfSearch->jpf_plt.cms_ip[size-1] = '\0';
				
				/*
				size = sizeof(pPfSearch->jpf_plt.mds_ip);
				strncpy(pPfSearch->jpf_plt.mds_ip, stJxjPlatformCfg.szCmsIp, size-1);
				pPfSearch->jpf_plt.mds_ip[size-1] = '\0';
				*/

				pPfSearch->jpf_plt.cms_port = stJxjPlatformCfg.u16CmsPort;
				pPfSearch->jpf_plt.mds_port = 0;
				pPfSearch->jpf_plt.conn_cms = stJxjPlatformCfg.u8Enable;
				*parm->size = sizeof(Jpf_Search);
				break;
			}
			case J_MB_Jpf_Redirect_Id:
			{
				Jpf_Redirect *pRedirect = (Jpf_Redirect *)parm->data;
				NVR_DEV_JXJ_PLATFORM_CFG_S stJxjPlatformCfg = {0};
				
				if(0 > nvr_logon_get_jxj_platform_cfg(0, &stJxjPlatformCfg))
					return -1;
				
				size= sizeof(pRedirect->pu_id);
				strncpy(pRedirect->pu_id, (char *)stJxjPlatformCfg.szPuId, size-1);
				pRedirect->pu_id[size-1] = '\0';

				size = sizeof(pRedirect->cms_ip);
				strncpy(pRedirect->cms_ip, stJxjPlatformCfg.szCmsIp, size-1);
				pRedirect->cms_ip[size-1] = '\0';

				pRedirect->cms_port = stJxjPlatformCfg.u16CmsPort;
				pRedirect->conn_cms = stJxjPlatformCfg.u8Enable;
				*parm->size = sizeof(Jpf_Redirect);
				break;
			}
			default:
				break;
		}
	}
	return 0;
}


static void nvr_jxj_param_log(const char *msg)
{
	return;
}


pu_process_cb_t pu_process_cb =
{
	.user_arg = NULL,
	.callback = nvr_jxj_search_device_cb,
};

struct device_ops dev_ops = 
{
	.get_device_param = nvr_jxj_get_device_param,
	.set_device_param = nvr_jxj_set_device_param,
	.log = nvr_jxj_param_log,
};



