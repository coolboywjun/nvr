#ifndef __NVR_NET_MAN_H__
#define __NVR_NET_MAN_H__

#include <nvr_server_headers.h>

int nvr_net_update_cfg();
int nvr_net_get_net_info(NVR_DEV_NET_CFG_S *pNetCfg);
int nvr_net_man_init();
void nvr_net_man_uninit();

#endif

