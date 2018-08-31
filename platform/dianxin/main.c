#include <stdio.h>
#include <signal.h>
#include <av_pool.h>
#include <storage_lib.h>
#include <nvr_lib_interface.h>

#include "hi_rtsp.h"
#include "hi_rtsp_global.h"
#include "stun_buffer.h"
#include "nvr_lib_interface.h"
#include "hi_platform_dianxin.h"
#include "hi_ftp_api.h"
#include "hi_ftp.h"
#include "update.h"
#include "storage_lib.h"
static volatile int quit = 0;
void sig_quit(int signo)
{
	quit = 1;
}

int main (int argc, char *argv[])
{			
	int ret = 0;
	NVR_DEV_DIANXIN_PLATFORM_CFG_S stDevPlatformCfg = {0};
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, sig_quit);
	signal(SIGINT,  sig_quit);
	
	nvr_lib_init();
	hi_ftp_init(nvr_update_write);
	if(av_pool_init() < 0)
	{
		__E("av pool init failed.\n");
		return -1;
	}

	if(nvr_logon_get_dianxin_platform_cfg(0, &stDevPlatformCfg) < 0)
	{
		__E("Get dianxin platform config failed.\n");
		av_pool_uninit();
		nvr_lib_uninit();
		return -1;
	}
	if(stDevPlatformCfg.localRtspPort == 0)
		stDevPlatformCfg.localRtspPort = LOCAL_RTSP_PORT;
	if(stDevPlatformCfg.upnpRtspPort == 0)
		stDevPlatformCfg.upnpRtspPort = UPNP_RTSP_PORT;
	storage_lib_init();
	hi_rtsp_init(stDevPlatformCfg.localRtspPort);
	ret = hi_dianxin_pf_init(&stDevPlatformCfg);
#if 0
	sleep(20);
	NVR_FTP_CFG_S stFtpCfg;
	strcpy(stFtpCfg.szFtpPath, "");
	strcpy(stFtpCfg.szFtpPsw, "jxj123");
	strcpy(stFtpCfg.szFtpSvr, "192.168.5.116");
	strcpy(stFtpCfg.szFtpUsr, "jxj");
	stFtpCfg.u16FtpPort = 21;
	stFtpCfg.u32Size = sizeof(stFtpCfg);
	nvr_logon_set_ftp_cfg(0, &stFtpCfg);
	hi_ftp_send("nvr_update_v1.1.8.bin", NULL, 1);
#endif		
	while(!quit){
		pause();
	}
	if(ret >= 0)
		hi_dianxin_pf_uninit();
	hi_rtsp_uninit();	
	storage_lib_uninit();
	av_pool_uninit();
	hi_ftp_uninit();
	nvr_lib_uninit();
	return 0;
}
