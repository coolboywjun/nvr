
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ipnc_lib_client.h>



int main(int argc, char *argv[])
{
	/*int i;
	long handle;
	IpncSearch search[256];
	int searchCnt = 0;

	if(ipnc_client_connect_server("jxj_ti", &handle) < 0)
	{
		printf("Failed to connect to jxj_ti server.\n");
		return -1;
	}

	if(ipnc_client_start_search(handle) < 0)
	{
		printf("Failed to search.\n");
		return -1;
	}
	usleep(10000);
	if(ipnc_client_get_search(handle, search, 256, &searchCnt) < 0)
	{
		printf("Failed to get search result.\n");
		return -1;
	}
	ipnc_client_stop_search(handle);
	for(i = 0; i < searchCnt; i++)
	{
		int addr[4];
		addr[0] = (search[i].ipAddr >> 24) & 0xff;
		addr[1] = (search[i].ipAddr >> 16) & 0xff;
		addr[2] = (search[i].ipAddr >> 8) & 0xff;
		addr[3] = (search[i].ipAddr >> 0) & 0xff;
		
		printf("%d: %d.%d.%d.%d:%d\n",
			i,
			addr[0], addr[1],
			addr[2], addr[3],
			search[i].port);
	}
	
	ipnc_client_disconnect_server(handle);
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!! end \n");*/
	return 0;
}


