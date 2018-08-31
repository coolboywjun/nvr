
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ipnc_lib_client.h>

void usage_main(void)
{
	printf("**************************************************\n");
	printf("1. param get and set.\n");
	printf("2. search get and set.\n");
	printf("3. ptz control.\n");
	printf("4. rtsp control.\n");
	printf("0. exit.\n");
	printf("**************************************************\n");
	printf("please test val: \n");
}

int test_ptz(long handle, long logonHandle)
{
	int quit = 0;
	int i = 0, j = 0;
	int ret = 0;
	int chn = 0;
	IPNC_PTZ_CTRL_S ptz;

	if(0 == handle)
		return -1;

	while(i < 4)
	{
		if(0 == i)
			ptz.sdkPtzCtrl.action = PTZ_CMD_UP;
		else
			ptz.sdkPtzCtrl.action = PTZ_CMD_SENSE + i;
		ptz.sdkPtzCtrl.param = 50;
		for(j = 0; j < 10; j++)
		{
			if((0 > ipnc_client_send_ptz_cmd(handle, 
									   logonHandle, 
									   &ptz, 
									   sizeof(IPNC_PTZ_CTRL_S))) < 0)
					printf("ptz control failed.\n");
		}
		i++;
		sleep(2);
	}
	
	i = 0;
	while(i < 4)
	{
		if(3 == i)
			ptz.sdkPtzCtrl.action = PTZ_CMD_UP;
		else
			ptz.sdkPtzCtrl.action = PTZ_CMD_RIGHT - i;
		ptz.sdkPtzCtrl.param = 50;
		for(j = 0; j < 10 ;j++)
		{
			if((0 > ipnc_client_send_ptz_cmd(handle, 
								   logonHandle, 
								   &ptz, 
								   sizeof(IPNC_PTZ_CTRL_S))) < 0)
				printf("ptz control failed.\n");
		}
		i++;
		sleep(2);
	}

	for(j = 0; j < 5 ;j++)
	{
		ptz.sdkPtzCtrl.action = PTZ_CMD_DOWN;
		ptz.sdkPtzCtrl.param = 50;
		if((0 > ipnc_client_send_ptz_cmd(handle, 
								   logonHandle, 
								   &ptz, 
								   sizeof(IPNC_PTZ_CTRL_S))) < 0)
				printf("ptz control failed.\n");
	}
	
	return 0;
}


int test_param(long handle, long logonHandle)
{
	int quit = 0;
	int ret = 0;
	int val = 0;
	IPNC_NTP_CFG_S ntpCfg;

	if(0 == logonHandle)
	{
		printf("logon failed.\n");
		return -1;
	}

	memset(&ntpCfg, 0, sizeof(ntpCfg));

	while(!quit)
	{
		printf("**************************************************\n");
		printf("1.get ntp; 2.set ntp.\n");
		printf("0. exit.\n");
		printf("**************************************************\n");
		printf("please test val: \n");

		scanf("%d", &val);
		switch(val)
		{
			case 1://获取ipnc ntp参数
			{
				IPNC_NTP_CFG_S ntpInfo;
				memset(&ntpInfo, 0, sizeof(IPNC_NTP_CFG_S));
				if((ret = ipnc_client_get_param(handle,
										logonHandle,
										IPNC_CMD_NTP_CFG,
										&ntpInfo, 
										sizeof(IPNC_NTP_CFG_S))) < 0)
				{
					printf("Get ipnc ntp param failed.\n");
				}
				else
				{
					printf("ntpInfo.enable:%d\n", ntpInfo.enable);
					printf("ntpInfo.ntpRefTime:%d\n", ntpInfo.ntpRefTime);
					printf("ntpInfo.ntpServer:%s\n", ntpInfo.ntpServer);
					printf("ntpInfo.timeZone:%d\n", ntpInfo.timeZone);
					printf("ntpInfo.euroTime:%d\n", ntpInfo.euroTime);
				}
				break;
			}
			case 2://设置ipnc ntp参数
			{
				IPNC_NTP_CFG_S ntpInfo;

				memset(&ntpInfo, 0, sizeof(IPNC_NTP_CFG_S));

				strcpy(ntpInfo.ntpServer, "cn.pool.ntp.org");

				//端口号
				ntpInfo.timeZone = 4*3600;
				ntpInfo.euroTime = 1;
				ntpInfo.enable = 1;
				ntpInfo.ntpRefTime = 4;
				
				if((ret = ipnc_client_set_param(handle,
										logonHandle,
										IPNC_CMD_NTP_CFG,
										&ntpInfo, 
										sizeof(IPNC_NTP_CFG_S))) < 0)
				{
					printf("set ipnc ntp param failed.\n");
				}
				
				break;
			}
			case 0:
			{
				quit = 1;
				break;
			}
			default:
				break;
		}
	}

	return 0;
}


int main(int argc, char *argv[])
{
	long handle;
	long ipncHandle;
	long logonHandle = 0;
	int sock = -1;
	long streamHandle;

	int quit = 0;
	int testVal = 0;
	int port = 3321;
	int chn  = 0;
	char proNm[32] = {0};
	char ipAddr[32] = {0};

	/*printf("please protocol name :\n");
	scanf("%s", proNm);
	printf("\n");

	printf("please ip addr :\n");
	scanf("%s", ipAddr);
	printf("\n");

	printf("please port :\n");
	scanf("%d", &port);
	printf("\n");

	printf("please chn :\n");
	scanf("%d", &chn);
	printf("\n");*/

	if(NULL == argv[1] || NULL == argv[2] ||
	   NULL == argv[3] || NULL == argv[4])
	
	printf("%d\n", __LINE__);
	if(ipnc_client_connect_server(argv[1], &handle) < 0)
	{
		printf("Failed to connect to %s server.\n", proNm);
		return -1;
	}
	printf("%d\n", __LINE__);
	if(ipnc_client_connect_ipnc(handle, atoi(argv[4]), argv[2], atoi(argv[3]), "admin", "admin", &ipncHandle) < 0)
	{
		printf("Failed to connect to ipnc %s\n", ipAddr);
		goto _OUT;
	}

	printf("%d\n", __LINE__);
	if((sock = ipnc_client_logon_ipnc(handle, ipncHandle, &logonHandle)) < 0)
	{
		printf("Failed to logon to ipnc\n");
		goto _OUT;
	}
	printf("sock:%d\n", sock);
	printf("%d\n", __LINE__);

	while(!quit)
	{
		usage_main();
		scanf("%d", &testVal);
		switch(testVal)
		{
			case 1://param
			{
				test_param(handle, logonHandle);
				break;
			}
			case 2://search
			{
				//test_search();
				break;
			}
			case 3://ptz
			{
				test_ptz(handle, logonHandle);
				break;
			}
			case 4://rtsp
			{
				//test_rtsp();
				break;
			}
			case 0:
			{
				quit = 1;
				break;
			}
			default:
				break;
		}		
	}


	/*if(ipnc_client_start_get_stream(handle, ipncHandle, 0, 0, &streamHandle) < 0)
	{
		printf("Failed to get %s main stream.\n", pIpAddr);
		return -1;
	}
	printf("%d\n", __LINE__);*/

_OUT:	
	ipnc_client_logout_ipnc(handle, logonHandle);
	ipnc_client_disconnect_ipnc(handle, ipncHandle);
	ipnc_client_disconnect_server(handle);
	
	return 0;
}

