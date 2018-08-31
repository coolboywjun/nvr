#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/route.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mount.h>

#include <net_api.h>
#include <syscfg.h>
#include <update.h>
//#include "flash.h"
#include "ping_server.h"
#include <nvr_lib_interface.h>
#include <gpio.h>
#include <storage_lib.h>
#include <player_interface.h>

typedef struct _tcp_thread_struct
{
	int thread_run;
	struct in_addr server_addr;
	int server_port;
}tcp_thread_struct;

enum
{
	REQUEST_UPDATE = 0,
	RECV_AND_WRITE_FILE,
	REQUEST_CFG,
	RECV_AND_WRITE_CFG,
	UPDATE_FINISH,
};


#define HWRESET_GPIO		26
#define SYSTEM_LED_GPIO	27
#define FAN_GPIO			28
#define STORAGE_LED_GPIO	77

static volatile int quit = 0;

static void sigint(int dummy)
{
	quit = 1;
}

int init_signal()
{
	struct sigaction sigAction;

	sigAction.sa_handler = sigint;
	sigemptyset(&sigAction.sa_mask);
	sigAction.sa_flags = 0;
	sigaction(SIGINT, &sigAction, NULL);
	return 0;
}
/*
static int is_mtdblock_mount(char *mount_point)
{
	char line[256];
	FILE *fp;
	
	if(mount_point == NULL)
		return 0;
	fp = fopen("/proc/mounts", "r");
	if(fp == NULL)
	{
		return 0;
	}
	while(fgets(line, sizeof(line), fp) != NULL)
	{
		if(strstr(line, mount_point) != NULL)
		{
			fclose(fp);
			return 1;
		}
	}
	fclose(fp);
	return 0;
}
*/
static int create_udp_server()
{
	int sock;
	struct sockaddr_in fromAddr;

	sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (sock < 0)
	{
		printf("Failed to create ping server socket.\r\n");
		return -1;
	}

	memset(&fromAddr,0,sizeof(fromAddr));
	fromAddr.sin_family = AF_INET;
	fromAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	fromAddr.sin_port = htons(UDP_SERVER_PORT);
	if (bind(sock,(struct sockaddr*)&fromAddr,sizeof(fromAddr))<0)
	{
		printf("Failed to bind server socket.\r\n");
		close(sock);
		return -1;
	}
	return sock;
}

void *tcp_update_thr(void *data)
{
	int tcp_socket;
	tcp_thread_struct *tcp_thread_s = (tcp_thread_struct *)data;
	char port[8];
	unsigned char buffer[256];
	cmd_info *cmd_info_ptr;
	int cur_status = REQUEST_UPDATE;
	int file_size;
	int recv_len, total_len = 0;
	//char *file_buf;

	printf("TCP server ip is %s\n", inet_ntoa(tcp_thread_s->server_addr));
	snprintf(port, sizeof(port), "%d", tcp_thread_s->server_port);
	tcp_socket = net_tcp_block_connect(NULL, NULL, inet_ntoa(tcp_thread_s->server_addr), port);
	if(tcp_socket < 0)
	{
		printf("Failed to connect tcp server.\n");
		return THREAD_FAIL;
	}

	while(tcp_thread_s->thread_run)
	{
		switch(cur_status)
		{
		case REQUEST_UPDATE:
			{
				cmd_info_ptr = (cmd_info*)buffer;
				cmd_info_ptr->magic = CMD_MAGIC;
				cmd_info_ptr->cmd = CMD_REQUEST_FILE;
				strcpy(cmd_info_ptr->filename, "update");
				printf("cmd magic: 0x%08lx\n", cmd_info_ptr->magic);
				if(net_tcp_block_send(tcp_socket, buffer, sizeof(buffer)) < 0)
				{
					printf("Failed to send request command.\n");
					net_close_socket(&tcp_socket);
					return THREAD_FAIL;
				}
				if(net_tcp_block_recv(tcp_socket, (char *)buffer, sizeof(buffer), sizeof(buffer)) < 0)
				{
					printf("Failed to recv cmd response.\n");
					net_close_socket(&tcp_socket);
					return THREAD_FAIL;
				}
				cmd_info_ptr = (cmd_info*)buffer;
				file_size = cmd_info_ptr->filelen;
				printf("update file size: %d\n", file_size);
				//flash_erase(2);
				cur_status = RECV_AND_WRITE_FILE;
				break;
			}
		case RECV_AND_WRITE_FILE:
			{
				int packet_no = 0;
				unsigned char data_buff[4096];
				total_len = 0;
				do
				{
					recv_len = (file_size - total_len)>sizeof(data_buff) ? sizeof(data_buff) : (file_size - total_len);
					if(net_tcp_block_recv(tcp_socket, (char *)data_buff, sizeof(data_buff), recv_len) != recv_len)
					{
						printf("Failed to recv update file.\n");
						net_close_socket(&tcp_socket);
						return THREAD_FAIL;
					}
					if(nvr_update_write((char *)data_buff, recv_len, packet_no++, file_size) < 0)
					{
						printf("Failed to write update file.\n");
						net_close_socket(&tcp_socket);
						return THREAD_FAIL;
					}
					total_len += recv_len;
				}while(total_len < file_size);
				nvr_update_write((char *)data_buff, 0, packet_no++, file_size);
				cur_status = UPDATE_FINISH;
				break;
			}
		case REQUEST_CFG:
			{
			#if 0
				cmd_info_ptr = (cmd_info*)buffer;
				cmd_info_ptr->magic = CMD_MAGIC;
				cmd_info_ptr->cmd = CMD_REQUEST_FILE;
				strcpy(cmd_info_ptr->filename, "cfg");
				if(tcp_block_send(tcp_socket, buffer, sizeof(buffer)) < 0)
				{
					printf("Failed to send request command.\n");
					cur_status = UPDATE_FINISH;
					break;
				}
				if(tcp_block_recv(tcp_socket, (char *)buffer, sizeof(buffer), sizeof(buffer)) < 0)
				{
					printf("Failed to recv cmd response.\n");
					cur_status = UPDATE_FINISH;
					break;
				}
				cmd_info_ptr = (cmd_info*)buffer;
				file_size = cmd_info_ptr->filelen;
				printf("cfg file size: %d\n", file_size);
				cur_status = RECV_AND_WRITE_CFG;
			#endif	
				break;
			}
		case RECV_AND_WRITE_CFG:
			{
			#if 0
				if(file_size < 0)	// 批量升级工具没有配置文件
				{
					printf("The server don't support cfg.\n");
					cur_status = UPDATE_FINISH;
					break;
				}
				file_buf = malloc(file_size);
				if(file_buf == NULL)
				{
					printf("Failed to alloc for cfg file buffer.\n");
					cur_status = UPDATE_FINISH;
					break;
				}
				if((recv_len = net_tcp_block_recv(tcp_socket, file_buf, file_size, file_size)) < 0)
				{
					printf("Failed to recv cfg file.\n");
					free(file_buf);
					break;
				}
				if(recv_len == file_size)
				{
					update_cfg(file_buf, file_size);
				}
				free(file_buf);
				cur_status = UPDATE_FINISH;
			#endif	
				break;
			}
		case UPDATE_FINISH:
			{
				cmd_info_ptr = (cmd_info*)buffer;
				cmd_info_ptr->magic = CMD_MAGIC;
				cmd_info_ptr->cmd = CMD_UPDATE_FILNISH;
				if(net_tcp_block_send(tcp_socket, buffer, sizeof(buffer)) < 0)
				{
					printf("Failed to send request command.\n");
					net_close_socket(&tcp_socket);
					return THREAD_FAIL;
				}
				net_close_socket(&tcp_socket);
				return THREAD_SUCCESS;
			}
		default:
			break;
		}
	}
	return THREAD_SUCCESS;
}

void *tcp_set_mac_and_hwid_thr(void *data)
{
	int tcp_socket;
	unsigned char buffer[256];
	unsigned char mac[6];
	unsigned char hwid[DEV_SERIALNO_LEN];
	char port[8];
	cmd_info *cmd_info_ptr;
	DEV_SystemCfg *pCfg;
	//int tryNum = 0;
	//int ret;
	int validResult = 0;
	int i;
	tcp_thread_struct *tcp_thread_s = (tcp_thread_struct *)data;

	printf("TCP server ip is %s\n", inet_ntoa(tcp_thread_s->server_addr));
	pCfg = get_system_cfg();
	if(pCfg == NULL)
	{
		printf("get system config err\r\n");
		return THREAD_FAIL;
	}
	
	snprintf(port, sizeof(port), "%d", tcp_thread_s->server_port);
	tcp_socket = net_tcp_block_connect(NULL, NULL, inet_ntoa(tcp_thread_s->server_addr), port);
	if(tcp_socket < 0)
	{
		printf("Failed to connect tcp server.\n");
		return THREAD_FAIL;
	}

	memset(buffer, 0, sizeof(buffer));
	cmd_info_ptr = (cmd_info*)buffer;
	cmd_info_ptr->magic = CMD_MAGIC;
	cmd_info_ptr->cmd = CMD_REQUEST_MAC_AND_HWID;
	if(net_tcp_block_send(tcp_socket, buffer, sizeof(buffer)) < 0)
	{
		printf("Failed to send request command.\n");
		net_close_socket(&tcp_socket);
		return THREAD_FAIL;
	}
	if(net_tcp_block_recv(tcp_socket, (char *)buffer, sizeof(buffer), sizeof(buffer)) < 0)
	{
		printf("Failed to recv cmd response.\n");
		net_close_socket(&tcp_socket);
		return THREAD_FAIL;
	}
/*	
	if(is_mtdblock_mount("/mnt/nand"))
	{
		while(tryNum++ < 10)
		{
			ret = umount("/mnt/nand");
			if(ret == 0)	break;
		}
		if(ret < 0)
		{
			printf("Failed to umount /mnt/nand.\n");
			net_close_socket(tcp_socket);
			return THREAD_FAIL;
		}
	}
*/
	memcpy(mac, cmd_info_ptr->data, sizeof(mac));
	memcpy(hwid, cmd_info_ptr->data + sizeof(mac), sizeof(hwid));
	for(i=0; i<sizeof(mac); i++)
	{
		if(mac[i] != 0)
		{
			memcpy(pCfg->u8Mac, mac, sizeof(mac));
			validResult = 1;
			break;
		}
	}
	for(i=0; i<sizeof(hwid); i++)
	{
		if(hwid[i] != 0)
		{
			memcpy(pCfg->u8SerialId, hwid, sizeof(pCfg->u8SerialId));
			validResult = 1;
			break;
		}
	}
	if(validResult)
	{
		system("rm /mnt/nand/* -rf");		
		system("rm /mnt/log/* -rf");
		//flash_erase(4);	//擦除 保存配置文件的分区
		if(save_system_cfg(pCfg) < 0)
		{
			printf("Failed to write system configure.\n");
			net_close_socket(&tcp_socket);
			return THREAD_FAIL;
		}
		
		memset(buffer, 0, sizeof(buffer));
		cmd_info_ptr = (cmd_info*)buffer;
		cmd_info_ptr->magic = CMD_MAGIC;
		cmd_info_ptr->cmd = CMD_UPDATE_FILNISH;
		memcpy(cmd_info_ptr->data, mac, sizeof(mac));
		memcpy(cmd_info_ptr->data + sizeof(mac), hwid, sizeof(hwid));
		
		if(net_tcp_block_send(tcp_socket, buffer, sizeof(buffer)) < 0)
		{
			printf("Failed to send request command.\n");
			net_close_socket(&tcp_socket);
			return THREAD_FAIL;
		}
	}
	else
	{
		memset(buffer, 0, sizeof(buffer));
		cmd_info_ptr = (cmd_info*)buffer;
		cmd_info_ptr->magic = CMD_MAGIC;
		cmd_info_ptr->cmd = CMD_UPDATE_FAIL;
		
		if(net_tcp_block_send(tcp_socket, buffer, sizeof(buffer)) < 0)
		{
			printf("Failed to send request command.\n");
			net_close_socket(&tcp_socket);
			return THREAD_FAIL;
		}
	}
	net_close_socket(&tcp_socket);

	return THREAD_SUCCESS;
}

unsigned int random_number()
{
	long ms;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	ms = tv.tv_sec * 1000 + (tv.tv_usec + 500) / 1000;
	srand(ms);
	return rand();
}
int broadcast_ip_table(const net_struct *pNet, const net_node_struct *pLocalNetNode)
{
	int i, setResult = 0;
	
	printf("==================== udp set ip table ==================\n");
	printf("net->dev_num = %d\n", pNet->dev_num);
	printf("server ip: %s\n", inet_ntoa(pNet->server_addr));
	printf("server port: %d\n", pNet->server_port);
	if(pNet->use_which_compare == 0) // use MAC to compare
	{
		for(i=0; i<pNet->dev_num; i++)
		{
			printf("ipaddr table: %s %02x:%02x:%02x:%02x:%02x:%02x %ld\n", 
					inet_ntoa(pNet->node[i].ip_addr), 
					pNet->node[i].mac[0], pNet->node[i].mac[1], pNet->node[i].mac[2], 
					pNet->node[i].mac[3], pNet->node[i].mac[4], pNet->node[i].mac[5], 
					pNet->node[i].random);
			if(memcmp(pNet->node[i].mac, pLocalNetNode->mac, sizeof(pNet->node[i].mac)) == 0)
			{
				printf("find: set ipaddr: %s\n", inet_ntoa(pNet->node[i].ip_addr));
				net_set_ip_addr("eth0", inet_ntoa(pNet->node[i].ip_addr));
				setResult = 1;
				break;
			}
		}
	}
	else		// use random to compare
	{
		for(i=0; i<pNet->dev_num; i++)
		{
			printf("ipaddr table: %s %02x:%02x:%02x:%02x:%02x:%02x %ld\n", 
					inet_ntoa(pNet->node[i].ip_addr), 
					pNet->node[i].mac[0], pNet->node[i].mac[1], pNet->node[i].mac[2], 
					pNet->node[i].mac[3], pNet->node[i].mac[4], pNet->node[i].mac[5], 
					pNet->node[i].random);
			if(pNet->node[i].random == pLocalNetNode->random)
			{
				printf("find: set ipaddr: %s\n", inet_ntoa(pNet->node[i].ip_addr));
				net_set_ip_addr("eth0", inet_ntoa(pNet->node[i].ip_addr));
				net_set_mac_addr("eth0", (char *)pNet->node[i].mac);
				setResult = 1;
				break;
			}
		}
	}
	return setResult;
}

static int audioQuit = 0;
void *play_audio_thread(void *data)
{
	long readId = 0, writeId = 0;
	char *pAudioBuf = NULL;
	int nBufLen = 0;
	int nFrameLen;
	AV_AUDIO_INFO_S audio_par = {0};
	AV_FRAME_HEAD_S *pFrameHdr;
	
	if(player_start_audio_record() < 0)
		return NULL;
	audio_par.u8AudioSamples = 0;
	audio_par.u16EncodeType = 1;
	audio_par.u8AudioChannels = 1;
	audio_par.u8AudioBits = 16;
	if(player_set_record_par(&audio_par) < 0)
	{
		player_stop_audio_record();
		return NULL;
	}
	if(player_start_phone_audio() < 0)
	{
		player_stop_audio_record();
		return NULL;
	}
	if(av_pool_start_read_frame(AV_POOL_TALK_CHN, AV_POOL_TALK_OUT_STREAM, 1000, &readId) < 0)
	{
		player_stop_phone_audio();
		player_stop_audio_record();
		return NULL;
	}
	if(av_pool_start_write_frame(AV_POOL_TALK_CHN, AV_POOL_TALK_IN_STREAM, &writeId) < 0)
	{
		av_pool_stop_read_frame(readId);
		player_stop_phone_audio();
		player_stop_audio_record();
		return NULL;
	}
	while(!audioQuit)
	{
		pAudioBuf = NULL;
		nBufLen = 0;
		if((nFrameLen = av_pool_read_frame(readId, &pAudioBuf, &nBufLen, 100)) <= 0)
			continue;
		pFrameHdr = (AV_FRAME_HEAD_S *)pAudioBuf;
		if(av_pool_write_frame(writeId, (unsigned char *)(pAudioBuf+AV_FRAME_HEAD_LEN), nFrameLen-AV_FRAME_HEAD_LEN,
							(unsigned char *)pFrameHdr, AV_FRAME_HEAD_LEN,
							pFrameHdr->u32FrameNo, pFrameHdr->u8FrameType,
							pFrameHdr->u64TimeStamp) < 0)
			break;
		
	}
	av_pool_stop_write_frame(writeId);
	av_pool_stop_read_frame(readId);
	player_stop_phone_audio();
	player_stop_audio_record();
	
	return NULL;
}

static void *test_hw_reset_thread(void *data)
{
	void *ret = 0;
	int value = 0;
	int first_value = -1;
	long handle;
	volatile int *quit = (int *)data;

	if((handle = gpio_open(HWRESET_GPIO)) == 0)
		return ret;
	while(!*quit)
	{
		if(gpio_get_value(handle, &value) < 0)
			break;
		if(first_value == -1)
		{
			first_value = value;
		}
		else
		{
			if(value != first_value)
			{
				ret = (void*)1;
				break;
			}
		}
		usleep(50000);
	}
	gpio_close(handle);
	return ret;
}


int main(int argc, char *argv[])
{
	int sock = 0;
	struct sockaddr_in toAddr;
	int recvLen;
	int addrLen;
	char recvBuffer[1024];
	IcmpHeader  *pStIcmpHeader;			// ICMP头
	PSTJMsgHeader pHead;
	pthread_t tcp_thread_id;
	tcp_thread_struct tcp_thread;
	cmd_info *cmd_info_ptr;
	net_node_struct net_node;
	pthread_t hwreset_thread_id;
	volatile int hwreset_quit = 0;
	
	memset(&tcp_thread, 0, sizeof(tcp_thread));

	init_signal();
	av_pool_init();
	nvr_lib_init();
	player_msg_init();
	while (!quit)
	{
		sock = create_udp_server();
		if (sock < 0)
		{
			printf("Failed to create server.\r\n");
			usleep(10000);
			continue;
		}

		while (!quit)
		{
			addrLen = sizeof(toAddr);
			if ((recvLen = recvfrom(sock,recvBuffer,
									sizeof(recvBuffer),0,
									(struct sockaddr*)&toAddr,
									(socklen_t*)&addrLen))<0)
			{
				printf("Failed to recv udp date.\r\n");
				close(sock);
				sock = 0;
				break;
			}
			//printf("ip: %s\n", inet_ntoa(toAddr.sin_addr));
			//printf("port: %d\n", ntohs(toAddr.sin_port));
			if (recvLen > 0)
			{
				pHead = (PSTJMsgHeader)recvBuffer;
				if (pHead->uMagic != 33080)
				{
					printf("resv Magic err\r\n");
					continue;
				}
				switch (pHead->uType)
				{
				case PSTJ_PING_CMD:
					{
						pStIcmpHeader = (IcmpHeader *) pHead->dat;
						pHead->ret  = 0;
						pStIcmpHeader->i_type = 0;
						/*
						printf("Recv From []:\r\n  Type=%d,Code=%d,CKSum=%d,ID=%d,Seq=%d\r\n",
							pStIcmpHeader->i_type,
							pStIcmpHeader->i_code,
							pStIcmpHeader->i_cksum,
							pStIcmpHeader->i_id,
							pStIcmpHeader->i_seq
							);
						*/
						if (sendto(sock,recvBuffer,
								   recvLen,0,(struct sockaddr*)&toAddr,
								   sizeof(toAddr))!=recvLen)
						{
							printf("Failed to send data to client.\r\n");
							close(sock);
							break;
						}
					}
					break;
				case PSTJ_BROADCASE_SCAN:
					{
						int i;
						unsigned char mac[16];
						net_node_struct *net_node_ptr = (net_node_struct *)pHead->dat;
						
						printf("==================== udp scan.\n");
						net_node_ptr->random = random_number();
						printf("random number is %ld\n", net_node_ptr->random);
						if(net_get_mac_addr("eth0", (char *)mac) < 0)
						{
							printf("Failed to get eth0 mac addr.\n");
							break;
						}
						for(i=0; i<6; i++)
						{
							printf("%02X ", mac[i]);
						}
						printf("\n");
						memcpy(net_node_ptr->mac, mac, sizeof(net_node_ptr->mac));
						
						if(net_udp_send(sock, recvBuffer, sizeof(recvBuffer), (struct sockaddr*)&toAddr) < 0)
						{
							printf("Failed to send data to client.\r\n");
							close(sock);
							break;
						}
						memcpy(&net_node, net_node_ptr, sizeof(net_node));
						break;
					}
				case PSTJ_BROADCASE_IP_TABLE_TO_UPDATE:
					{
						net_struct *net;
						int setResult;
						
						net = (net_struct *)pHead->dat;
						setResult = broadcast_ip_table(net, &net_node);
						if(setResult)
						{
			                            if(tcp_thread.thread_run)                
			                            {     
			                                tcp_thread.thread_run = 0;
			                                pthread_cancel(tcp_thread_id); 
			                                pthread_join(tcp_thread_id, NULL);               
			                            }                                        

							memset(&tcp_thread, 0, sizeof(tcp_thread));
							tcp_thread.thread_run = 1;
							tcp_thread.server_port = net->server_port;
							tcp_thread.server_addr.s_addr = net->server_addr.s_addr;
							if(pthread_create(&tcp_thread_id, NULL, tcp_update_thr, &tcp_thread) < 0)
							{
								printf("Failed to create tcp thread.\n");
								break;
							}
						}
						break;
					}
				case PSTJ_BROADCASE_IP_TABLE_TO_SET_MAC_AND_HWID:
					{
						net_struct *net;
						int setResult;
						
						net = (net_struct *)pHead->dat;
						setResult = broadcast_ip_table(net, &net_node);
						if(setResult)
						{
			                            if(tcp_thread.thread_run)                
			                            {     
			                                tcp_thread.thread_run = 0;
			                                pthread_cancel(tcp_thread_id); 
			                                pthread_join(tcp_thread_id, NULL);               
			                            }                                        

							memset(&tcp_thread, 0, sizeof(tcp_thread));
							tcp_thread.thread_run = 1;
							tcp_thread.server_port = net->server_port;
							tcp_thread.server_addr.s_addr = net->server_addr.s_addr;
							if(pthread_create(&tcp_thread_id, NULL, tcp_set_mac_and_hwid_thr, &tcp_thread) < 0)
							{
								printf("Failed to create tcp thread.\n");
								break;
							}
						}
						break;
					}
				case PSTJ_SEND_EXE_FILE_TO_RUN:
					{
						FILE *fp;
						int file_len = 0;
						int pid;
						
						printf("===================================== PSTJ_SEND_EXE_FILE_TO_RUN\n");
						cmd_info_ptr = (cmd_info*)pHead->dat;
						if(cmd_info_ptr->magic != CMD_MAGIC)
						{
							printf("the cmd information's magic is wrong.\n");
							break;
						}
						fp = fopen(cmd_info_ptr->filename, "wb");
						if(fp == NULL)
						{
							printf("Failed to open exe file.\n");
							break;
						}
						while(file_len < cmd_info_ptr->filelen)
						{
							addrLen = sizeof(toAddr);
							if ((recvLen = recvfrom(sock,recvBuffer,
										sizeof(recvBuffer),0,
										(struct sockaddr*)&toAddr,
										(socklen_t*)&addrLen))<0)
							{
								printf("Failed to recv udp date.\r\n");
								break;
							}
							fwrite(recvBuffer, recvLen, 1, fp);
							file_len += recvLen;
						}
						fclose(fp);
						chmod(cmd_info_ptr->filename, 0777);
						pid = fork();
						if(pid < 0)
						{
							printf("Failed to fork.\n");
							break;
						}
						else if(pid == 0)
						{
							execl(cmd_info_ptr->filename, cmd_info_ptr->filename, NULL);
						}
						break;
					}
				case PSTJ_GET_DEVICE_INFO:
					{
					#if 0
						dev_info devInfo = {0};
						if(ipnc_get_dev_info(&devInfo) < 0)
						{
							printf("Failed to get device information.\n");
							break;
						}
						
						if(udp_send(sock, (char*)&devInfo, sizeof(devInfo), (struct sockaddr*)&toAddr) < 0)
						{
							printf("Failed to send device information to client.\r\n");
							close(sock);
							break;
						}
					#endif	
						break;
					}
				case PSTJ_SET_PLATFORM_TYPE:
					{
						DEV_SystemCfg * pCfg;
						unsigned long platType = 0;
						memcpy(&platType, pHead->dat, sizeof(platType));
						printf("Recv platform type: 0x%lx\n", (unsigned long)platType);
						pCfg = get_system_cfg();
						if(pCfg->platformServer != platType)
						{
							pCfg->platformServer = platType;
							save_system_cfg(pCfg);
						}
						pHead->ret = 0;
						if(net_udp_send(sock, recvBuffer, sizeof(recvBuffer), (struct sockaddr*)&toAddr) < 0)
						{
							printf("Faile dto send platform type to client.\n");
							break;
						}
						break;
					}
				case PSTJ_SET_SERIAL_ID:
					{
						int i;
						unsigned long checksum = 0;
						cmd_dev_serial *pSerial;
						unsigned long *pData;
						DEV_SystemCfg * pCfg;
						pSerial = (cmd_dev_serial *) pHead->dat;
						pData = (unsigned long *)pSerial;
						if(pSerial->magic != SERIAL_MAGIC)
						{
							printf("%s %d\n", __func__, __LINE__);
							pHead->ret = -1;
							net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
							break;
						}
						for(i = 0; i < (sizeof(cmd_dev_serial) - sizeof(unsigned long))/4; i++)
						{
							checksum += pData[i];
						}
						if(checksum != pSerial->checksum)
						{
							printf("%s %d\n", __func__, __LINE__);
							pHead->ret = -2;
							net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
							break;
						}
						pCfg = get_system_cfg();
						strcpy((char *)pCfg->u8SerialId, pSerial->serial);
						save_system_cfg(pCfg);
						pHead->ret = 0;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						printf("%s %d successfully\n", __func__, __LINE__);
						break;
					}
				case PSTJ_SET_KANDIAN_INFO:
					{
						int ret = 0;
						int chn = 0;
						cmd_kandian_info kandianInfo;
						NVR_DEV_DIANXIN_PLATFORM_CFG_S stDianxinCfg = {0};
						NVR_DEV_REC_STRC_S stRecCfg = {0};
						memcpy(&kandianInfo, pHead->dat, sizeof(kandianInfo));
						do
						{
							if(nvr_logon_get_dianxin_platform_cfg(0, &stDianxinCfg) < 0)
							{
								printf("Get Dianxin config failed.\n");
								ret = -1;
								break;
							}
							printf("Group Url: %s\n", kandianInfo.szGroupUrl);
							printf("Provincial Url: %s\n", kandianInfo.szProvincialUrl);
							printf("Key: %s\n", kandianInfo.szKey);
							printf("Mode: %s\n", kandianInfo.szMode);
							printf("DevID: %s\n", kandianInfo.szDevId);
							strncpy(stDianxinCfg.szGroupUrl, kandianInfo.szGroupUrl, sizeof(stDianxinCfg.szGroupUrl)-1);
							stDianxinCfg.szGroupUrl[sizeof(stDianxinCfg.szGroupUrl)-1] = '\0';
							strncpy(stDianxinCfg.szProvincialUrl, kandianInfo.szProvincialUrl, sizeof(stDianxinCfg.szProvincialUrl)-1);
							stDianxinCfg.szProvincialUrl[sizeof(stDianxinCfg.szProvincialUrl)-1] = '\0';
							strncpy(stDianxinCfg.szDevId, kandianInfo.szDevId, sizeof(stDianxinCfg.szDevId)-1);
							stDianxinCfg.szDevId[sizeof(stDianxinCfg.szDevId)-1] = '\0';
							strncpy(stDianxinCfg.szKey, kandianInfo.szKey, sizeof(stDianxinCfg.szKey)-1);
							stDianxinCfg.szKey[sizeof(stDianxinCfg.szKey)-1] = '\0';
							strncpy(stDianxinCfg.szMode, kandianInfo.szMode, sizeof(stDianxinCfg.szMode)-1);
							stDianxinCfg.szMode[sizeof(stDianxinCfg.szMode)-1] = '\0';
							stDianxinCfg.localRtspPort = kandianInfo.localRtspPort;
							stDianxinCfg.upnpRtspPort = kandianInfo.upnpRtspPort;
							if(nvr_logon_set_dianxin_platform_cfg(0, &stDianxinCfg) < 0)
							{
								printf("Get Dianxin config failed.\n");
								ret = -1;
								break;
							}
							for(chn = 0; chn < NVR_MAX_CHANNEL_NUM; chn++)
							{
								if(nvr_logon_get_rec_cfg(chn, &stRecCfg) < 0)
								{
									printf("Get Record config failed.\n");
									ret = -1;
									break;
								}
								stRecCfg.u8Enable = 1;	// 打开
								stRecCfg.u8OverWrite = 1;
								stRecCfg.u8RecStream = 0;
								stRecCfg.u8PreRecSecs = 0;
								stRecCfg.u8SaveMode = 0;
								stRecCfg.u8RecType = 0;	// 全天录像
								if(nvr_logon_set_rec_cfg(chn, &stRecCfg) < 0)
								{
									printf("Set Record config failed.\n");
									ret = -1;
									break;
								}
							}
							if(ret < 0)
								break;
						}while(0);
						pHead->ret = ret;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						if(ret == 0)
							printf("Set kan dian info successfully, reboot system now.\n");
						else
							printf("Set kan dian info failed.\n");
						break;
					}
				case PSTJ_GET_KANDIAN_INFO:
					{
						int ret = -1;
						cmd_kandian_info kandianInfo;
						NVR_DEV_DIANXIN_PLATFORM_CFG_S stDianxinCfg = {0};
						do
						{
							if(nvr_logon_get_dianxin_platform_cfg(0, &stDianxinCfg) < 0)
							{
								ret = -1;
								printf("Get kandian param failed.\n");
								break;
							}
							strncpy(kandianInfo.szGroupUrl, stDianxinCfg.szGroupUrl, sizeof(kandianInfo.szGroupUrl)-1);
							kandianInfo.szGroupUrl[sizeof(kandianInfo.szGroupUrl)-1] = '\0';
							strncpy(kandianInfo.szProvincialUrl, stDianxinCfg.szProvincialUrl, sizeof(kandianInfo.szProvincialUrl)-1);
							kandianInfo.szProvincialUrl[sizeof(kandianInfo.szProvincialUrl)-1] = '\0';
							strncpy(kandianInfo.szDevId, stDianxinCfg.szDevId, sizeof(kandianInfo.szDevId)-1);
							kandianInfo.szDevId[sizeof(kandianInfo.szDevId)-1] = '\0';
							strncpy(kandianInfo.szKey, stDianxinCfg.szKey, sizeof(kandianInfo.szKey)-1);
							kandianInfo.szKey[sizeof(kandianInfo.szKey)-1] = '\0';
							strncpy(kandianInfo.szMode, stDianxinCfg.szMode, sizeof(kandianInfo.szMode)-1);
							kandianInfo.szMode[sizeof(kandianInfo.szMode)-1] = '\0';
							kandianInfo.localRtspPort = stDianxinCfg.localRtspPort;
							kandianInfo.upnpRtspPort = stDianxinCfg.upnpRtspPort;
							ret = 0;
						}while(0);
						pHead->ret = ret;
						memcpy(pHead->dat, &kandianInfo, sizeof(kandianInfo));
						net_udp_send(sock, (char *)pHead, sizeof(*pHead)+sizeof(kandianInfo), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_REBOOT:
					{
						nvr_restart();
						pHead->ret = 0;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_HALT:
					{
						nvr_shutdown();
						pHead->ret = 0;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_SET_LANG:
					{
						DEV_SystemCfg * pCfg;
						unsigned long supportLang;
						memcpy(&supportLang, pHead->dat, sizeof(supportLang));
						pCfg = get_system_cfg();
						pCfg->supportLang = supportLang;
						pCfg->u8DefLang = pHead->dat[4];
						save_system_cfg(pCfg);
						pHead->ret = 0;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_NETWORK:
					{
						hwreset_quit = 0;
						pthread_create(&hwreset_thread_id, NULL, test_hw_reset_thread, (void *)&hwreset_quit);
						nvr_set_hwgpio_check(0);
						pHead->ret = 0;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_AUDIOIN:
					{
						pthread_t thid;
						audioQuit = 0;
						pHead->ret = 0;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						pthread_create(&thid, NULL, play_audio_thread, NULL);
						pthread_detach(thid);
						break;
					}
				case PSTJ_TEST_AUDIOIN_END:
					{
						audioQuit = 1;
						pHead->ret = 0;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_AUDIOOUT:
					{
						int ret;
						ret = player_alarm_audio("nvr_res/B.wav");
						pHead->ret = ret;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_AUDIOOUT_END:
					{
						int ret;
						ret = player_stop_alarm_audio();
						pHead->ret = ret;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_USB:
					{
						int ret;
						if(access("/dev/input/mouse0", F_OK) == 0
							&& access("/dev/input/mouse1", F_OK) == 0)
							ret = 0;
						else
							ret = -1;
						pHead->ret = ret;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_DISK:
					{
						int ret = -1;
						do
						{
							if(access("/dev/sda", F_OK) != 0)
								break;
							ret = 0;
						}while(0);
						pHead->ret = ret;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_LED_SYSTEM:
					{
						int ret = -1;
						unsigned char value = pHead->dat[0];
						long handle;
						do
						{
							if((handle = gpio_open(SYSTEM_LED_GPIO)) == 0)
								break;
							if(gpio_set_value(handle, value) < 0)
							{
								gpio_close(handle);
								break;
							}
							gpio_close(handle);
							ret = 0;
						}while(0);
						pHead->ret = ret;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_LED_STORAGE:
					{
						int ret = -1;
						unsigned char value = pHead->dat[0];
						long handle;
						do
						{
							if((handle = gpio_open(STORAGE_LED_GPIO)) == 0)
								break;
							if(gpio_set_value(handle, value) < 0)
							{
								gpio_close(handle);
								break;
							}
							gpio_close(handle);
							ret = 0;
						}while(0);
						pHead->ret = ret;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_FAN:
					{
						int ret = -1;
						unsigned char value = pHead->dat[0];
						long handle;
						do
						{
							if((handle = gpio_open(FAN_GPIO)) == 0)
								break;
							if(gpio_set_value(handle, value) < 0)
							{
								gpio_close(handle);
								break;
							}
							gpio_close(handle);
							ret = 0;
						}while(0);
						pHead->ret = ret;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_GPIO:
					{
						int ret = -1;
						unsigned char gpio = pHead->dat[0];
						unsigned char value = pHead->dat[1];
						long handle;
						do
						{
							if((handle = gpio_open(gpio)) == 0)
								break;
							if(gpio_set_value(handle, value) < 0)
							{
								gpio_close(handle);
								break;
							}
							gpio_close(handle);
							ret = 0;
						}while(0);
						pHead->ret = ret;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_HWRESET:
					{
						int ret = -1;
						void *result;
						hwreset_quit = 1;
						pthread_join(hwreset_thread_id, &result);
						ret = ((int)result == 1) ? 0 : -1;
						pHead->ret = ret;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						break;
					}
				case PSTJ_TEST_FINISH:
					{
						int ret = 0;
						DEV_SystemCfg *pSysCfg;
						pSysCfg = get_system_cfg();
						if(pSysCfg->u8FactoryChecked == 0)
						{
							pSysCfg->u8FactoryChecked = 1;
							ret = save_system_cfg(pSysCfg);
						}
						pHead->ret = ret >=0 ? 0:-1;
						net_udp_send(sock, (char *)pHead, sizeof(*pHead), (struct sockaddr*)&toAddr);
						nvr_set_hwgpio_check(1);
						break;
					}
				default:
					{
						printf("================================================\n");
						pHead->ret = -1;
						if (sendto(sock,recvBuffer,
								   sizeof(struct stJMsgHeader),0,
								   (struct sockaddr*)&toAddr,sizeof(toAddr))
							!=sizeof(struct stJMsgHeader))
						{
							printf("Failed to send data to client.\r\n");
							close(sock);
							break;
						}
					}
					break;
				}
			}
		}
	}
	if (sock > 0)
	{
		close(sock);
	}
	av_pool_uninit();
	nvr_lib_uninit();
	player_exit();
	
	return 0;
}

