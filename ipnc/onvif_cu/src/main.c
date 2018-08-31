#include <stdio.h>
#include <string.h>
#include "onvif_api.h"
#include "onvif_ipnc_search.h"

enum
{
	ONVIF_CAPABILITY_MEDIA,
	ONVIF_CAPABILITY_ALL,
};

#if 0

static void cbSearchFunc(int nIndex, discover_info_t *pInfo)
{	
	printf(">>>>>>>>>>>>>>>>>>>>>>>>\r\n");	
	printf(">>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	printf ("nIndex = %d\r\n", nIndex);
	printf("devIP = %s\r\n", pInfo->devIP);
	printf("devServiceAddr = %s\r\n", pInfo->devServiceAddr);
	printf("devType = %s\r\n", pInfo->devType);
	printf("EPAddr = %s\r\n", pInfo->EPAddr);	
	printf("scopes = %s\r\n", pInfo->scopes);
	printf("\r\n\r\n");
}



int main(int argc, char *argv[])
{
	int cmd;
	cmd = ONVIF_CMD_DISCOVER;
	
	switch(cmd)
	{
		case ONVIF_CMD_DISCOVER:			
			nvr_onvif_discover(4, cbSearchFunc);
			break;
		case ONVIF_CMD_GET_CAPABILITY:
		{
			device_capability_t capability;
			nvr_onvif_get_capability("192.168.5.244", "80", &capability);
		}
			break;
		case ONVIF_CMD_GET_PROFILES:
		{
			//char streamUrl[128];
			//const char *Token;
			onvif_profile_list_t profile;
			memset(&profile, 0, sizeof(onvif_profile_list_t));
			INIT_LIST_HEAD(&profile.list);			
			nvr_onvif_get_profiles("192.168.5.123", "80", &profile);
			//Token = onvif_get_profile_token(&profile, 0);
			//nvr_onvif_get_stream_url("192.168.5.123", "80", Token ,streamUrl);
			//printf("############%s\r\n", Token);			
			//printf("stream url %s\r\n",streamUrl);
		}
			break;
		case ONVIF_CMD_GET_NETWORK_INTERFACE:
			break;
		case ONVIF_CMD_GET_DNS:
			break;
		case ONVIF_CMD_GET_SCOPES:
		{
			onvif_scopes_list_t scopes;
			memset(&scopes, 0, sizeof(onvif_scopes_list_t));
			INIT_LIST_HEAD(&scopes.list);			
			nvr_onvif_get_scopes("192.168.5.215", "80", &scopes);
		}
			break;
		case ONVIF_CMD_GET_URL:
		{
			//char acUrl[128];
			//nvr_onvif_get_stream_url("192.168.5.244", "80", acUrl);
		}
			break;

		case ONVIF_CMD_START_STREAM:
		{
			char streamUrl[128];
			const char *Token;
			onvif_profile_list_t profile;
			long lStreamHandle = 0;
			memset(&profile, 0, sizeof(onvif_profile_list_t));
			INIT_LIST_HEAD(&profile.list);			
			nvr_onvif_get_profiles("192.168.5.244", "80", &profile);
			Token = onvif_get_profile_token(&profile, 0);
			nvr_onvif_get_stream_url("192.168.5.244", "80", Token ,streamUrl);
			//printf("############%s\r\n", Token);			
			//printf("stream url %s\r\n",streamUrl);
			
			//onvif_rtsp_user_date_t userData;
			//userData.chn = 0;
			//userData.streamID = 0;

			//rtspClientOpenUrl(streamUrl, NULL, NULL, 1, 
			//					jxj_cu_write_frame, jxj_cu_begin_write_frame,
			//					jxj_cu_end_write_frame,
			//					(void *)&userData, &lStreamHandle);
		}
		break;
		
		case ONVIF_CMD_STOP_STREAM:
		{			
			//long lStreamHandle = 0;
			//rtspClientCloseUrl(lStreamHandle);
		}
		break;
		
		default:
			break;
	}

	printf("$$$$$$$$$$$$$$$$$$$$$$$$$\r\n");
	sleep(15);
	return 0;
}

#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <av_pool.h>
#include <net_api.h>
#include <ipnc_lib.h>
#include <ipnc_lib_server.h>
#include "onvif_ipnc_realplay.h"
#include "onvif_ipnc_mng.h"
#include "onvif_ipnc_logon.h"

static volatile int quit = 0;


static IpncProtocolCbTable cbTable = 
{
	.connectCb = onvif_ipnc_connect,
	.disconnectCb = onvif_ipnc_disconnect,

	.searchCfgCb = onvif_ipnc_search_cfg,
	.startSearchCb = onvif_ipnc_start_search,
	.stopSearchCb = onvif_ipnc_stop_search,
	.getSearchCb = onvif_ipnc_get_search,

	.logonCb = onvif_ipnc_logon,
	.logoutCb = onvif_ipnc_logout,

	.startGetStreamCb = onvif_ipnc_start_get_stream,
	.stopGetStreamCb = onvif_ipnc_stop_get_stream,
	.requestIFrameCb = onvif_ipnc_request_iframe,

	.getParamCb = onvif_ipnc_get_param,
	.setParamCb = onvif_ipnc_set_param,

	.ptzControl = onvif_ipnc_ptz_ctrl,
	
	.testIpcStatus = onvif_ipnc_test_ipc,
};


void sig_usr(int signo)
{
	// print the process information here
	//ti_ipnc_mng_print_list();
}

void sig_quit(int signo)
{
	quit = 1;
}

int main(int argc, char *argv[])
{	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, sig_quit);
	signal(SIGINT,  sig_quit);
	signal(SIGUSR1, sig_usr);
	Onvif_Net_Search_init();

	if(av_pool_init() < 0)
	{
		printf("open av pool failed.\n");
		exit(1);
	}
	
	if(ipnc_server_init(JXJ_ONVIF_PROTOCOL_NAME, &cbTable) < 0)
	{
		printf("ipnc protocol server init failed.\n");
		exit(1);
	}
	
	while(!quit)	sleep(1);
	
	ipnc_server_uninit();
	av_pool_uninit();
	Onvif_Net_Search_uninit();
	
	return 0;
}

#endif
