#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <av_pool.h>
#include <ipnc_lib_client.h>

volatile int quit = 0;

static void sig_quit(int signo)
{
	quit = 1;
}

static void usage(const char *appname)
{
	printf("usage: %s ip port poolchn poolstream\n", appname);
}


static void *thread_run(void *data)
{
	int nCh = 0;
	int nStream = 0;
	long lReadId;
	long lHandle;
	long lIpncHandle;
	long lLogonHandle;
	long lStreamHandle;
	nCh = (int)data;
	
	if(ipnc_client_connect_server("jxj_cu", &lHandle) < 0)
	{
		printf("Connect to jxj_cu server failed.\n");
		return NULL;
	}
	
	while(!quit)
	{
		if(ipnc_client_connect_ipnc(lHandle, nCh, "192.168.5.185", 3321, "admin", "admin", &lIpncHandle) < 0)
		{
			printf("Connect to jxj_cu ipnc failed.\n");
			sleep(1);
			continue;
		}
		if(ipnc_client_logon_ipnc(lHandle, lIpncHandle, &lLogonHandle) < 0)
		{
			printf("logon to jxj_cu failed.\n");
			ipnc_client_disconnect_ipnc(lHandle, lIpncHandle);
			sleep(1);
			continue;
		}
		if(ipnc_client_start_get_stream(lHandle, lIpncHandle, 0, nStream, &lStreamHandle) < 0)
		{
			printf("start get jxj_cu stream failed.\n");
			ipnc_client_logout_ipnc(lHandle, lLogonHandle);
			ipnc_client_disconnect_ipnc(lHandle, lIpncHandle);
			sleep(1);
			continue;
		}
		
		sleep(random() % 10);
		
		ipnc_client_stop_get_stream(lHandle, lIpncHandle, lStreamHandle);
		ipnc_client_logout_ipnc(lHandle, lLogonHandle);
		ipnc_client_disconnect_ipnc(lHandle, lIpncHandle);
		
	}
	
	ipnc_client_disconnect_server(lHandle);
	return NULL;
}
#if 0
int main(int argc, char *argv[])
{
	pthread_t thid1, thid2, thid3;
	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, sig_quit);
	signal(SIGTERM, sig_quit);

	pthread_create(&thid1, NULL, thread_run, (void *)0);
	pthread_create(&thid2, NULL, thread_run, (void *)1);
	pthread_create(&thid3, NULL, thread_run, (void *)2);
	//pthread_create(&thid4, NULL, thread_run, (void *)3);
	thread_run((void *)3);
	pthread_join(thid1, NULL);
	pthread_join(thid2, NULL);
	pthread_join(thid3, NULL);

	return 0;
}
#endif
#if 0
int main(int argc, char *argv[])
{
	char *pbuf =  NULL;
	int buflen = 0;
	int frameLen;
//	int fd;
	int nCh = 0;
	int nStream = 0;
	long lReadId;
	long lHandle;
	long lIpncHandle;
	long lLogonHandle;
	long lStreamHandle;
	char ip[64] = {0};
	int port;
	if(argc != 5)
	{
		usage(argv[0]);
		exit(1);
	}
	strncpy(ip, argv[1], sizeof(ip)-1);
	port = atoi(argv[2]);
	nCh = atoi(argv[3]);
	nStream = atoi(argv[4]);
	printf(">>>>>>>>>>>>>>>chno: %d, streamno: %d\n", nCh, nStream);
	
	if(nCh >=8 || nStream >= 3)
	{
		printf("input parameter error.\n");
		return -1;
	}
	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, sig_quit);
	signal(SIGTERM, sig_quit);
//	fd = open("/opt/video.h264", O_RDWR | O_CREAT | O_TRUNC);
//	if(fd < 0)
//	{
//		printf("open file failed.\n");
//		return -1;
//	}
	av_pool_init();
	printf("&&&&&&&&&&&&&&&%s %d\n", __func__, __LINE__);
	if(ipnc_client_connect_server("jxj_cu", &lHandle) < 0)
	{
		printf("Connect to jxj_ti server failed.\n");
		return -1;
	}
	printf("&&&&&&&&&&&&&&&%s %d\n", __func__, __LINE__);
	if(ipnc_client_connect_ipnc(lHandle, nCh, ip, port, "admin", "admin", &lIpncHandle) < 0)
	{
		printf("Connect to jxj_ti ipnc failed.\n");
		ipnc_client_disconnect_server(lHandle);
		return -1;
	}
	printf("&&&&&&&&&&&&&&&%s %d\n", __func__, __LINE__);
	if(ipnc_client_logon_ipnc(lHandle, lIpncHandle, &lLogonHandle) < 0)
	{
		printf("logon to jxj_ti failed.\n");
		ipnc_client_disconnect_ipnc(lHandle, lIpncHandle);
		ipnc_client_disconnect_server(lHandle);
		return -1;
	}
	printf("&&&&&&&&&&&&&&&%s %d\n", __func__, __LINE__);
	if(ipnc_client_start_get_stream(lHandle, lIpncHandle, 0, nStream, &lStreamHandle) < 0)
	{
		printf("start get jxj_ti stream failed.\n");
		ipnc_client_logout_ipnc(lHandle, lLogonHandle);
		ipnc_client_disconnect_ipnc(lHandle, lIpncHandle);
		ipnc_client_disconnect_server(lHandle);
		return -1;
	}
	printf("&&&&&&&&&&&&&&&%s %d\n", __func__, __LINE__);
	if(av_pool_start_read_frame(nCh, nStream, 300, &lReadId) < 0)
	{
		printf("Start read av pool failed.\n");
		ipnc_client_logout_ipnc(lHandle, lLogonHandle);
		ipnc_client_stop_get_stream(lHandle, lIpncHandle, lStreamHandle);
		ipnc_client_disconnect_ipnc(lHandle, lIpncHandle);
		ipnc_client_disconnect_server(lHandle);
		return -1;
	}
	printf("======================\n");
	while(!quit)
	{
#if 1	
		pbuf = NULL;
		buflen = 0;
		frameLen = av_pool_read_frame(lReadId, &pbuf, &buflen, 0);
		if(frameLen < 0)
		{
			usleep(10000);
			continue;
		}
		if(frameLen > 0 && buflen > 0 && pbuf != NULL)
		{
			AV_FRAME_HEAD_S *pFrameHdr = (AV_FRAME_HEAD_S *)pbuf;
			if(pFrameHdr->u8FrameType == AV_AUDIO_FRAME_A)
				printf("audio frame\n");
			
			else if(pFrameHdr->u8FrameType == AV_VIDEO_FRAME_I)
				printf("video  iiiiiii frame\n");
			else
				printf("video ppppp frame\n");
/*				
			if(pbuf != NULL && buflen > 0)
			{
				//printf("Get a frame.\n");
				if(pbuf[32] != 0
				    || pbuf[33] != 0
				    || pbuf[34] != 0
				    || pbuf[35] != 1)
				{
				    printf("nal error =============\n");
				    printf("framelen = %d\n", frameLen);
				}
				//write(fd, pbuf+32, frameLen-32);
			}
*/
		}	
#endif
	}
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	av_pool_stop_read_frame(lReadId);
	ipnc_client_logout_ipnc(lHandle, lLogonHandle);
	ipnc_client_disconnect_ipnc(lHandle, lIpncHandle);
	ipnc_client_disconnect_server(lHandle);
	av_pool_uninit();
//	close(fd);
	return 0;
}
#endif
#if 0

int main(int argc, char *argv[])
{
	char *pbuf =  NULL;
	int buflen = 0;
	int frameLen;
	int nCh = 0;
	int nStream = 0;
	long lReadId;
	long lHandle;
	long lIpncHandle;
	long lStreamHandle;
	char szProtocolName[128];
	char ip[64] = {0};
	int port;
	if(argc != 5)
	{
		usage(argv[0]);
		exit(1);
	}
	strncpy(ip, argv[1], sizeof(ip)-1);
	port = atoi(argv[2]);
	nCh = atoi(argv[3]);
	nStream = atoi(argv[4]);
	printf("chno: %d, streamno: %d\n", nCh, nStream);
	
	if(nCh >=4 || nStream >= 3)
	{
		printf("input parameter error.\n");
		return -1;
	}
	
	signal(SIGINT, sig_quit);

	av_pool_init();
	
	if(av_pool_get_pool_writing_cnt(nCh, nStream) <= 0)
	{
		// 此pool没有数据写入,直接返回错误
		printf("There is no data in the pool ch%d stream%d\n", nCh, nStream);
		return -1;
	}
	if(av_pool_get_pool_ipnc_handle(nCh, &lIpncHandle) < 0)
	{
		printf("Can't get ipnc handle.\n");
		return -1;
	}
	if(av_pool_get_pool_stream_handle(nCh, nStream, &lStreamHandle) < 0)
	{
		printf("Can't get stream handle.\n");
		return -1;
	}
	if(lIpncHandle <= 0 || lStreamHandle <= 0)
	{
		printf("The ipnc handle %ld and stream handle %ld of getting from av pool module is wrong\n", 
				lIpncHandle, lStreamHandle);
		return -1;
	}
	if(av_pool_get_pool_info(nCh, NULL, szProtocolName, NULL, NULL) < 0)
	{
		printf("Failed to get pool ch%d stream%d info.\n", nCh, nStream);
		return -1;
	}
	if(ipnc_client_connect_server(szProtocolName, &lHandle) < 0)
	{
		printf("Failed to connect to server: %s\n", szProtocolName);
		return -1;
	}
	
	if(ipnc_client_request_iframe(lHandle, lIpncHandle, lStreamHandle) < 0)
	{
		printf("failed to request iframe\n");
		return -1;
	}
	if(av_pool_start_read_frame(nCh, nStream, 300, &lReadId) < 0)
	{
		printf("Start read av pool failed.\n");
		ipnc_client_disconnect_server(lHandle);
		return -1;
	}
	

	while(!quit)
	{
		pbuf = NULL;
		buflen = 0;
		frameLen = av_pool_read_frame(lReadId, &pbuf, &buflen, 0);
		if(frameLen <= 0)
		{
			usleep(10000);
			continue;
		}
		if(frameLen > 0 && buflen > 0 && pbuf != NULL)
		{
			if(pbuf != NULL && buflen > 0)
			{
				printf("Get a frame.\n");
				if(pbuf[32] != 0
				    || pbuf[33] != 0
				    || pbuf[34] != 0
				    || pbuf[35] != 1)
				{
				    printf("nal error =============\n");
				    printf("framelen = %d\n", frameLen);
				}
				//write(fd, pbuf+32, frameLen-32);
				//usleep(5000);
			}
		}		
	}
	av_pool_stop_read_frame(lReadId);
	ipnc_client_disconnect_server(lHandle);
	av_pool_uninit();

	return 0;
}
#endif

#if 1
int main(int argc, char *argv[])
{
	char *pbuf =  NULL;
	int buflen = 0;
	int frameLen;
//	int fd;
	int nCh = 0;
	int nStream = 0;
	long lReadId;
	long lHandle;
	long lIpncHandle;
	long lLogonHandle;
	long lStreamHandle;
	char ip[64] = {0};
	int port;
	FILE *fp;
	if(argc != 5)
	{
		usage(argv[0]);
		exit(1);
	}
	strncpy(ip, argv[1], sizeof(ip)-1);
	port = atoi(argv[2]);
	nCh = atoi(argv[3]);
	nStream = atoi(argv[4]);
	printf("chno11111: %d, streamno: %d\n", nCh, nStream);
	
	if(nCh >=8 || nStream >= 3)
	{
		printf("input parameter error.\n");
		return -1;
	}
	fp = fopen("/tmp/video.h264", "wb");
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, sig_quit);
	signal(SIGTERM, sig_quit);
//	fd = open("/opt/video.h264", O_RDWR | O_CREAT | O_TRUNC);
//	if(fd < 0)
//	{
//		printf("open file failed.\n");
//		return -1;
//	}
	av_pool_init();
	if(av_pool_start_read_frame(nCh, nStream, 1000, &lReadId) < 0)
	{
		printf("Start read av pool failed.\n");
		return -1;
	}
	while(!quit)
	{
		pbuf = NULL;
		buflen = 0;
		frameLen = av_pool_read_frame(lReadId, &pbuf, &buflen, 100);
		if(frameLen > 0)
		{
			int len = frameLen - 32;
			printf("video frameLen: %d\n", frameLen);
			fwrite(&len, sizeof(len), 1, fp);
			fwrite(pbuf+32, frameLen-32, 1, fp);
		}
	}
	av_pool_stop_read_frame(lReadId);
	av_pool_uninit();
	fclose(fp);
//	close(fd);
	return 0;
}
#endif

