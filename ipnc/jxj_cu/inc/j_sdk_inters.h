#ifndef __SDK_INTERFACES_H__
#define __SDK_INTERFACES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "j_sdk.h"
#include "media_structs.h"

#define CTL_CMD_IFRAME          1


enum
{
	NOTYPE  = 0x0,
	UPGRADE = 0x10,
	AUDIO   = 0x20,
};
enum
{
	UPGRADE_ERROR  = -1,
	UPGRADE_NORMAL = 0,
	UPGRADE_START  = 1,
	UPGRADE_END    = 2
};
enum
{
	FORBID_CONNECT = 0,            //禁止连接
	PERMIT_CONNECT = 1,            //允许连接
};

typedef struct _user
{
	char ip[J_SDK_MAX_IP_LEN];
	char name[J_SDK_MAX_NAME_LEN];
    char local[J_SDK_MAX_IP_LEN];
}user_t;

typedef struct __avs_media avs_media;

/* 实时流接口 */
typedef struct __ls_avs_ops ls_avs_ops;
struct __ls_avs_ops
{
    int32_t (*probe)(int32_t channel, int32_t level, media_info_t *mi);     /* 获取媒体信息,可能被反复调用 */
    int32_t (*open)(avs_media *m, int32_t channel, int32_t level);          /* 初始化 */
    int32_t (*play)(avs_media *m);                                          /* 就绪，之后可以使用avs_media_fill()发送实时流 */
    int32_t (*ctrl)(avs_media *m, uint32_t cmd, void *data);                /* 控制：cmd=1,data=NULL 强制I帧; */
    void    (*close)(avs_media *m);                                         /* 关闭 */
};

/* 历史流接口 */
typedef struct __hs_avs_ops hs_avs_ops;
struct __hs_avs_ops
{
    int32_t (*probe)(int32_t channel, int32_t level, int32_t type,       
        uint8_t *start_time, uint8_t *end_time, uint8_t *property,          /* 时间格式:YYYYMMDDHHMMSS */
        media_info_t *mi);                                                  /* 获取媒体信息,可能被反复调用 */
    int32_t (*open)(avs_media *m, int32_t channel, int32_t level,
        int32_t type, uint8_t *start_time, uint8_t *end_time, uint8_t *property); /* 初始化 */
    int32_t (*play)(avs_media *m);                                          /* SDK即将开始回调.pull(), 辅助user实现相关策略（如预读等），可为NULL */
    int32_t (*pause)(avs_media *m);                                         /* SDK即将暂停回调.pull(), 辅助user实现相关策略（如预读等），可为NULL */
    int32_t (*lseek)(avs_media *m, uint32_t ts);                            /* 定位 */
    void    (*close)(avs_media *m);                                         /* 关闭 */
    int32_t (*pull)(avs_media *m, frame_t **frm);                           /* SDK获取历史视频流HOOK */
};

/* 流句柄操作接口 */
void avs_media_set_u(avs_media *m, void *u);                 /* 设置用户参数 */
void *avs_media_get_u(avs_media *m);                         /* 获取用户参数 */
int avs_media_fill(avs_media *m, frame_t *frm);                 /* 实时流推送 */
void avs_media_kill(avs_media *m);                             /* 故障通告AVS */
avs_media *avs_media_ref(avs_media *m);                       /* 增对对句柄的引用 */
void avs_media_unref(avs_media *m);                          /* 减少对句柄的引用 */

/* 帧分配接口，用于历史流pull */
frame_t *avs_alloc_frame(uint32_t data_size, uint32_t nal_count);   /* 分配一帧的空间 */
void avs_free_frame(frame_t *frame);                                /* 释放一帧空间，当填充帧数据失败时使用 */


/* 设备参数接口 */
struct device_ops
{
	int (*get_device_param)(int param_id, int channel, void *param, user_t *u);    /* 获取设备参数 */
	int (*set_device_param)(int param_id, int channel, void *param, user_t *u);    /* 设置设备参数 */
	
	void (*log)(const char *msg);                                                  /* 日志输入接口 */
};

struct platfrom_param
{
	char pu_id[J_SDK_MAX_ID_LEN];     //设备PUID
	char cms_host[J_SDK_MAX_IP_LEN];  //平台主机地址
	int pu_type;                      //设备类型
	int cms_port;                     //平台主机端口
	int data_port;                    //流媒体端口
	int protocol;                     //1-UDP, 0-TCP
	int conn_cms;                     //1-连接 0-不连接
};

struct cloud_param
{
    char dev_code[J_SDK_MAX_ID_LEN];        //设备出厂编号
    char software[J_SDK_MAX_VERSION_LEN];   //设备软件版本
    char dispense_host[J_SDK_MAX_IP_LEN];   //分派服务器地址
    int dispense_port;                      //分派服务器端口
	int conn_cloud;                         //1-连接 0-不连接
};

/* SDK 基本操作接口 */
int  j_dev_sdk_init(int listen_port);                                      /* 初始化SDK */
int  j_dev_sdk_register_device_ops(struct device_ops *ops);          /* 注册设备参数接口 */
int  j_dev_sdk_register_avs_ops(ls_avs_ops *lso, hs_avs_ops *hso);    /* 注册流媒体接口 */
int  j_dev_sdk_set_platfrom_param(struct platfrom_param *plt_pm);    /* 设置平台信息 */
int  j_dev_sdk_set_cloud_param(struct cloud_param *cld_pm);          /* 设置云平台信息 */
int  j_dev_sdk_run_server();                                           /* 启动相关服务 */
void j_dev_sdk_stop_server();                                          /* 停止服务 */
void j_dev_sdk_cleanup();                                               /* 清理SDK */

int j_dev_sdk_submit_param(int parm_id, int channel, void *parm);     /* 上报信息接口 */

#ifdef __cplusplus
}
#endif

#endif	/* __SDK_INTERFACES_H__ */

