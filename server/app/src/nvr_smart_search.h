
#ifndef __NVR_SMART_SEARCH_H__
#define __NVR_SMART_SEARCH_H__

#include <nvr_struct.h>

int nvr_smart_send_channel_info();

int nvr_smart_is_ipc_remote_used(const uint32_t u32IpcIp);

int nvr_smart_update_channel(int port, NVR_CHANNEL_CFG_S *pChannelCfgInfo);

int nvr_smart_search_init();

void nvr_smart_search_uninit();


#endif

