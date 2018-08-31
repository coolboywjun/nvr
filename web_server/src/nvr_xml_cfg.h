#ifndef __NVR_XML_H__
#define __NVR_XML_H__

#include <semaphore.h>

extern sem_t g_xmlLock;

//获取设备参数
int nvr_web_get_dev_cfg(int sock);
//获得nvr网络配置信息
int nvr_web_get_net_cfg(int sock);
//获取ftp参数
int nvr_web_get_ftp_cfg(int sock);
//获取smtp参数
int nvr_web_get_smtp_cfg(int sock);
//获取ddns参数
int nvr_web_get_ddns_cfg(int sock);
//获取upnp参数
int nvr_web_get_upnp_cfg(int sock);
//获取NTP参数
int nvr_web_get_ntp_cfg(int sock);
//获取用户参数
int nvr_web_get_user_cfg(int sock);
//获取TIME参数
int nvr_web_get_time_cfg(int sock);
//获取自动任务参数
int nvr_web_get_auto_cfg(int sock);
//获取通道信息参数
int nvr_web_get_channel_cfg(int sock);
//获取comm信息参数
int nvr_web_get_comm_cfg(int sock);
//获取rec信息参数
int nvr_web_get_rec_cfg(int sock);
//获取电信平台信息参数
int nvr_web_get_dianxin_platform_cfg(int sock);
//开始搜索
int nvr_web_get_search_start(int sock);
//获得搜索
int nvr_web_get_search_get(int sock, long lHandle);
//停止搜索
int nvr_web_get_search_stop(int sock, long lHandle);



//读xml文件
int nvr_web_read_xml(const char* xmlData, long lHandle);


#endif


