
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
 #include <unistd.h>
#include <ipnc_lib_client.h>

int main(int argc, char *argv[])
{
	long handle;
	long ipncHandle;
	long logonHandle = 0;
	int sock = -1;
	long streamHandle;

	int port = 3321;
	char *pProNm  = "jxj_cu";
	char *pIpAddr = "192.168.71.119";

	printf("%d\n", __LINE__);
	if(ipnc_client_connect_server(pProNm, &handle) < 0)
	{
		printf("Failed to connect to %s server.\n", pProNm);
		return -1;
	}
	printf("%d\n", __LINE__);
	if(ipnc_client_connect_ipnc(handle, 0, pIpAddr, port, "admin", "admin", &ipncHandle) < 0)
	{
		printf("Failed to connect to ipnc %s\n", pIpAddr);
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
	/*if(ipnc_client_start_get_stream(handle, ipncHandle, 0, 0, &streamHandle) < 0)
	{
		printf("Failed to get %s main stream.\n", pIpAddr);
		return -1;
	}
	printf("%d\n", __LINE__);*/

	sleep(30);
_OUT:	
	ipnc_client_logout_ipnc(handle, logonHandle);
	ipnc_client_disconnect_ipnc(handle, ipncHandle);
	ipnc_client_disconnect_server(handle);
	
	return 0;
}

