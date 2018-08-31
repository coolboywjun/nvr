
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <signal.h>
#include <sys/msg.h>
#include <errno.h>

#include <nvr_lib_interface.h>

// p2p vivi sdk
#include "libt2usvr.h"

#define P2P_VIVI_ID_FILE_PATH		"/tmp/vivi.id"

#define VV_P2P_SERVER_ADDR "p2p.ip2p001.com"
#define VV_P2P_SERVER_PORT 8000
#define VV_P2P_SERVER_KEY ""
// ./T2uSvr JXJIPCWW008 p2p.ip2p001.com 8000

typedef struct _P2P_VIVI_MSG
{
	long msgtype;
	long cmdtype;
	char data[64];
}P2P_VIVI_MSG;

static int msgid;

int p2p_get_id_from_file(char *pViviIdBuf, int nBufLen)
{
	FILE *fp;
	fp = fopen(P2P_VIVI_ID_FILE_PATH, "r");
	if(fp == NULL)
		return -1;
	if(fgets(pViviIdBuf, nBufLen, fp) == NULL)
	{
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

int p2p_set_id_to_file(char *pViviIdBuf)
{
	FILE *fp;
	fp = fopen(P2P_VIVI_ID_FILE_PATH, "w");
	if(fp == NULL)
		return -1;
	if(fputs(pViviIdBuf, fp) < 0)
	{
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

static void p2p_signal_func(int signo)
{
	P2P_VIVI_MSG stViviMsg = {0};
	stViviMsg.msgtype = 1;
	stViviMsg.cmdtype = 0xff;
	msgsnd(msgid, &stViviMsg, sizeof(stViviMsg)-sizeof(long), 0);
}

int main(int argc, char *argv[])
{
#if 1	
	NVR_DEV_STABLE_INFO_S stStableInfo = {0};
	
	nvr_lib_init();
	if(nvr_logon_get_stable_cfg(0, &stStableInfo) < 0)
	{
		syslog(LOG_ERR, "Get stable config failed.\n");
		nvr_lib_uninit();
		return -1;
	}
	nvr_lib_uninit();

	printf("execute T2uSvr for p2p vivi: %s %s p2p.ip2p001.com 8000.\n", "/opt/nvr/T2uSvr", stStableInfo.szSerialId);
	// ./T2uSvr <uuid> <server addr> <server port> [server key]
	execlp("/opt/nvr/T2uSvr", argv[0], stStableInfo.szSerialId, "p2p.ip2p001.com", "8000", NULL);
	
#else
	int ret;
	char viviid[64] = {0};
	key_t keyval;
	P2P_VIVI_MSG stViviMsg;

	signal(SIGQUIT, p2p_signal_func);
	signal(SIGTERM, p2p_signal_func);
	keyval=ftok("/opt/nvr/p2p_vivi_platform", 0xabab);
	msgid=msgget(keyval,IPC_CREAT|0666);

	if(p2p_get_id_from_file(viviid, sizeof(viviid)) == 0)
	{
		printf("Start p2p vivi with id %s\n", viviid);
		t2u_svr_init(VV_P2P_SERVER_ADDR, VV_P2P_SERVER_PORT, VV_P2P_SERVER_KEY, viviid, 10);
		strncpy(viviid, stViviMsg.data, sizeof(viviid)-1);
	}
	
	do
	{
		memset(&stViviMsg, 0, sizeof(stViviMsg));
		ret = msgrcv(msgid, &stViviMsg, sizeof(stViviMsg)-sizeof(long), 0, 0);
		if(ret < 0)
		{
			if(errno == EINTR)
				continue;
			break;
		}
		if(stViviMsg.cmdtype == 1)	// open vivi
		{
			if(strlen(viviid) != 0 && strcmp(viviid, stViviMsg.data) != 0)
			{
				printf("Exit p2p vivi.\n");
				t2u_svr_exit();
				memset(viviid, 0, sizeof(viviid));
			}
			printf("Start p2p vivi with id %s\n", stViviMsg.data);
			t2u_svr_init(VV_P2P_SERVER_ADDR, VV_P2P_SERVER_PORT, VV_P2P_SERVER_KEY, stViviMsg.data, 10);
			strncpy(viviid, stViviMsg.data, sizeof(viviid)-1);
			p2p_set_id_to_file(viviid);
		}
		else if(stViviMsg.cmdtype == 2)// close vivi
		{
			if(strlen(viviid))
			{
				printf("Exit p2p vivi.\n");
				t2u_svr_exit();
				memset(viviid, 0, sizeof(viviid));
			}
		}
		else
			break;
	}while(1);
	if(strlen(viviid))
	{
		printf("Exit p2p vivi.\n");
		t2u_svr_exit();
	}
#endif

	return 0;
}

