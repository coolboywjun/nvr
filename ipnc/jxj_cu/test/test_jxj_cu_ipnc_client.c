
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <net_api.h>
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

//请求搜索包复制到配置包
void jxj_cu_search_cpy_res2req(IPNC_NET_SEARCH_CFG_REQ_S *pIpncCfgReq,
							   IPNC_NET_SEARCH_RES_S *pIpncRes)
{
	//cu 搜索设备id 号
	memcpy(&pIpncCfgReq->u32CliMultiIp , &pIpncRes->addrClient, 
		   strlen((char*)&pIpncRes->addrClient));
	
	//序列号
	memcpy(pIpncCfgReq->u8SerialId, pIpncRes->u8SerialId, 
		   sizeof(pIpncCfgReq->u8SerialId));
	//软件版本
	pIpncCfgReq->u32SoftVer = pIpncRes->u32SoftVer;
	//设备类型
	pIpncCfgReq->u16DevType = pIpncRes->u16DevType;
	//设备端口
	pIpncCfgReq->u16DevPort = pIpncRes->u16DevPort;
	//ip
	pIpncCfgReq->u32DevIp = pIpncRes->u32DevIp;
	//mac
	memcpy(pIpncCfgReq->u8MacAddr, pIpncRes->u8MacAddr, 
		   sizeof(pIpncCfgReq->u8MacAddr));
	//DNS\DHCP
	pIpncCfgReq->u8Dns = pIpncRes->u8Dns;
	pIpncCfgReq->u8DHCP = pIpncRes->u8DHCP;

	//dns
	pIpncCfgReq->u32DevDns1 = pIpncRes->u32DevDns1;
	pIpncCfgReq->u32DevDns2 = pIpncRes->u32DevDns2;
}


int test_ptz(long handle, long logonHandle)
{
	//int quit = 0;
	int i = 0, j = 0;
	//int ret = 0;
	//int chn = 0;
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
		printf("1.get ntp;   2.set ntp.\n");
		printf("3.get vmain; 4.set vmain.\n");
		printf("5.get vsub;  6.set vsub.\n");
		printf("7.get audio; 8.set audio.\n");
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
			case 3:
			{
				IPNC_DEV_VENC_CFG_S cfg;
				int dataSize = sizeof(IPNC_DEV_VENC_CFG_S);
				
				memset(&cfg, 0, dataSize);
				if((ret = ipnc_client_get_param(handle,
										logonHandle,
										IPNC_CMD_VMAIN_CFG,
										&cfg, 
										dataSize)) < 0)
				{
					printf("Get ipnc vmain param failed.\n");
				}
				else
				{
					printf("cfg.u16Height:%d\n", cfg.u16Height);
					printf("cfg.ntpRefTime:%d\n", cfg.u16Width);
					printf("cfg.u32BitrateRate:%d\n", cfg.u32BitrateRate);
					printf("cfg.u8FlowType:%d\n", cfg.u8FlowType);
					printf("cfg.u8FrameRate:%d\n", cfg.u8FrameRate);
					printf("cfg.u8Gop:%d\n", cfg.u8Gop);
					printf("cfg.u8PicQuilty:%d\n", cfg.u8PicQuilty);
					printf("cfg.u8VideoEncType:%d\n", cfg.u8VideoEncType);
				}
				break;
			}
			case 4:
			{
				break;
			}
			case 5:
			{	IPNC_DEV_VENC_CFG_S cfg;
				int dataSize = sizeof(IPNC_DEV_VENC_CFG_S);
				
				memset(&cfg, 0, dataSize);
				if((ret = ipnc_client_get_param(handle,
										logonHandle,
										IPNC_CMD_VSUB_CFG,
										&cfg, 
										dataSize)) < 0)
				{
					printf("Get ipnc vsub param failed.\n");
				}
				else
				{
					printf("cfg.u16Height:%d\n", cfg.u16Height);
					printf("cfg.ntpRefTime:%d\n", cfg.u16Width);
					printf("cfg.u32BitrateRate:%d\n", cfg.u32BitrateRate);
					printf("cfg.u8FlowType:%d\n", cfg.u8FlowType);
					printf("cfg.u8FrameRate:%d\n", cfg.u8FrameRate);
					printf("cfg.u8Gop:%d\n", cfg.u8Gop);
					printf("cfg.u8PicQuilty:%d\n", cfg.u8PicQuilty);
					printf("cfg.u8VideoEncType:%d\n", cfg.u8VideoEncType);
				}
				break;
			}
			case 6:
			{
				break;
			}
			case 7:
			{
				IPNC_AUDIO_CFG_S cfg;
				int dataSize = sizeof(IPNC_AUDIO_CFG_S);
				
				memset(&cfg, 0, dataSize);
				if((ret = ipnc_client_get_param(handle,
										logonHandle,
										IPNC_CMD_AUDIO_CFG,
										&cfg, 
										dataSize)) < 0)
				{
					printf("Get ipnc audio param failed.\n");
				}
				else
				{
					printf("cfg.u8Open:%d\n", cfg.u8Open);
					printf("cfg.u8AEncType:%d\n", cfg.u8AEncType);
					printf("cfg.u8Type:%d\n", cfg.u8Type);
					printf("cfg.u32SampleRate:%d\n", cfg.u32SampleRate);
				}
				break;
			}
			case 8:
			{
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

int test_search(long handle)
{
	int ret = 0;
	int quit = 0;
	int i = 0;
	int val = 0;
	int devCnt = 0;
	long userDefId = 0;

	IPNC_NET_SEARCH_RES_S *pDevTmp = NULL;
	IPNC_NET_SEARCH_RES_S devInfoArr[1024];
	while(!quit)
	{
		printf("**************************************************\n");
		printf("1. start search.\n");
		printf("2. get search.\n");
		printf("3. set search.\n");
		printf("0. exit.\n");
		printf("**************************************************\n");
		printf("please test val: \n");
		
		scanf("%d", &val);
		switch(val)
		{
			case 1://start
			{
				userDefId = handle; //每次搜索都要用户自己传递一个标识本次搜索的标识值
				if(0 != (ret = ipnc_client_start_search(userDefId)))
				{
					printf("ipnc_client_start_search failed: %d\n", ret);
				}
				break;
			}
			case 2://get 可定时获取, 每次获取只返回一个设备信息.
			{
				if(0 == userDefId)
				{
					printf("start failed.\n");
					break;
				}
				
				memset(&devInfoArr, 0, sizeof(devInfoArr));
				pDevTmp = devInfoArr;
				while(ipnc_client_get_search(userDefId, pDevTmp))
				{
					printf("%-20s --- %-2d.%-2d.%-2d.%-2d ---- %d.%d.%d.%d ---- %02x:%02x:%02x:%02x:%02x:%02x -- %-4d\n", pDevTmp->u8SerialId,
											pDevTmp->u32SoftVer >> 24 & 0xff, pDevTmp->u32SoftVer >> 16 & 0xff,
										  	pDevTmp->u32SoftVer >> 8  & 0xff, pDevTmp->u32SoftVer & 0xff,
											pDevTmp->u32DevIp>>24 & 0xff, pDevTmp->u32DevIp>>16 & 0xff,
											pDevTmp->u32DevIp>>8  & 0xff, pDevTmp->u32DevIp & 0xff,
											pDevTmp->u8MacAddr[0], pDevTmp->u8MacAddr[1], pDevTmp->u8MacAddr[2],
											pDevTmp->u8MacAddr[3], pDevTmp->u8MacAddr[4], pDevTmp->u8MacAddr[5],
											pDevTmp->u16DevPort);
					pDevTmp++;
					devCnt++;
				}

				break;
			}
			case 3://set
			{
				IPNC_NET_SEARCH_CFG_REQ_S searchCfgPkg = {0};
				char ip[32];

				strcpy(ip, "192.168.88.253");

				pDevTmp = devInfoArr;
				for(i = 0; i < devCnt; i++)
				{
					if(net_ip_a2n(ip) == pDevTmp->u32DevIp)
					{
						printf(">>>>>>>>>jxj cu set dec cfg.\n");
						
						//获取响应包信息
						jxj_cu_search_cpy_res2req(&searchCfgPkg, pDevTmp);

						//更改信息
						searchCfgPkg.u16DevPort = 4080;
						searchCfgPkg.u32DevIp = net_ip_a2n("192.168.88.254");
						searchCfgPkg.u32DevIpMask = net_ip_a2n("255.255.0.0"); 
						searchCfgPkg.u32DevGwAddr = net_ip_a2n("192.168.88.1");
						searchCfgPkg.u32DevDns1 = net_ip_a2n("202.96.134.133");
						searchCfgPkg.u32DevDns2 = net_ip_a2n("202.96.128.166");
						searchCfgPkg.u8MacAddr[0] = 0x11;
						searchCfgPkg.u8MacAddr[1] = 0x22;
						searchCfgPkg.u8MacAddr[2] = 0x55;
						searchCfgPkg.u8MacAddr[3] = 0x88;
						searchCfgPkg.u8MacAddr[4] = 0x99;
						searchCfgPkg.u8MacAddr[5] = 0xaa;
						
						strcpy(searchCfgPkg.szUsrName, "admin");
						strcpy(searchCfgPkg.szPsw, "admin");

						ipnc_client_set_search_cfg(userDefId, &searchCfgPkg);

						break;
					}
					else
						pDevTmp++;
					
				}
				
				break;
			}
			case 0:
			{
				quit = 1;
				ipnc_client_stop_search(userDefId);
				break;
			}
			default:
				break;
		}
	}

	return 0;
}

int test_rtsp(long handle, long ipncHandle)
{
	int quit = 0;
	long rtspCtrlHdl = {0};

	if(ipnc_client_start_get_stream(handle, ipncHandle, 0, 0, &rtspCtrlHdl) < 0)
	{
		printf("Failed ipnc_client_start_get_stream.\n");
		return -1;
	}
	 
	printf("rtspCtrlHdl: %lx\n", rtspCtrlHdl);
	while(!quit)
	{
		printf("exit, please input 1\n");
		scanf("%d", &quit);
	}

	ipnc_client_stop_get_stream(handle, 0, rtspCtrlHdl);
	
	return 0;
}


int main(int argc, char *argv[])// jxj_cu ip chn poolchn
{
	long handle;
	long ipncHandle;
	long logonHandle = 0;
	int sock = -1;
	//long streamHandle;

	int quit = 0;
	int testVal = 0;
	//int port = 3321;
	//int chn  = 0;
	char proNm[32] = {0};
	char ipAddr[32] = {0};
	IPNC_DEV_VENC_CFG_S stMainStreamCfg;
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

	/*usleep(500);
	IPNC_NTP_CFG_S ntpInfo;
	memset(&ntpInfo, 0, sizeof(IPNC_NTP_CFG_S));
	if((ipnc_client_get_param(handle,
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
	}*/
				
	if(ipnc_client_get_param(handle, logonHandle, IPNC_CMD_VMAIN_CFG, 
										&stMainStreamCfg, sizeof(stMainStreamCfg)) < 0)
	{
		printf("Failed to get  vmain param\n");
		goto _OUT;
	}
	
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
				test_search(handle);
				break;
			}
			case 3://ptz
			{
				test_ptz(handle, logonHandle);
				break;
			}
			case 4://rtsp
			{
				test_rtsp(handle, ipncHandle);
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

