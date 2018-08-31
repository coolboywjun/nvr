


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <time.h>

#include "net_api.h"
#include "hi_dianxin_turn.h"
#include "hi_platform_dianxin.h"
#include "stun_buffer.h"
#include "nvr_net_config.h"


#define TURN_SERVER_LEFTTIME	60

#define NET_TIMEOUT		3000

#define THREAD_OK		((void *)0)
#define THREAD_FAIL		((void *)-1)

typedef struct _StunSession
{
	ioa_addr local_addr;
	char	lsaddr[129];
	ioa_addr remote_addr;
	char rsaddr[129];
	char ifname[129];
	ioa_addr peer_addr;
	int sock;
	int broken;
	u08bits nonce[STUN_MAX_NONCE_SIZE+1];
	u08bits realm[STUN_MAX_REALM_SIZE+1];
	/* RFC 6062 */
	//app_tcp_conn_info **tcp_conn;
	//size_t tcp_conn_number;
	int is_peer;
	SHATYPE shatype;
	char s_mobile_id[33];
	int haveRelayAddr;
	ioa_addr relay_addr;
	int haveMapAddr;
	ioa_addr map_addr;
	u08bits connectId[4];
	
	////////////////////////////// message
	int		isAllocation;
	int		sendRefreshTime;
	
	int 		startClient;
	pthread_t thid;
	char 	turnServerIp[NVR_IP_ADDR_LEN];
	int 		turnServerPort;
	char		turnServerUser[NVR_USER_NAME_LEN];
	char		turnServerPass[NVR_USER_NAME_LEN];
	
	volatile int quit;
} StunSession;

typedef struct _StunClientSession
{
	u08bits nonce[STUN_MAX_NONCE_SIZE+1];
	u08bits realm[STUN_MAX_REALM_SIZE+1];
	SHATYPE shatype;

	int isUsed;
	int sock;
	int rtspSock;

	char turnServerIp[NVR_IP_ADDR_LEN];
	int turnServerPort;
	
	char remoteIp[NVR_IP_ADDR_LEN];
	int  remotePort;
	
}StunClientSession;


typedef struct 
{
      uint16_t msgType;
      uint16_t msgLength;
      uint8_t id[16];
} StunMsgHdr;
#define STUN_MSG_HDR_LEN		(sizeof(StunMsgHdr))

static StunSession gStunSession;

//#define STUN_CLIENT_SESSION_NUM	4
//static StunClientSession gStunClientSession[STUN_CLIENT_SESSION_NUM];


extern int hi_turn_connect_server();

/*
static int alloc_client_session_id()
{
	int i;
	for(i = 0; i < STUN_CLIENT_SESSION_NUM; i++)
	{
		if(!gStunClientSession[i].isUsed)
		{
			memset(&gStunClientSession[i], 0, sizeof(gStunClientSession[i]));
			gStunClientSession[i].isUsed = 1;
			return i;
		}
	}
	
	return -1;
}

static void free_client_session_id(int id)
{
	if(id < 0 || id >= STUN_CLIENT_SESSION_NUM)
		return;
	gStunClientSession[id].isUsed = 0;
}

static void* client_session_thread_func(void *data)
{
	int ret;
	int id = (int)data;
	int fds[2];
	int sock, rtspSock;
	int recvLen, sendLen;
	unsigned char buff[2048];
	StunClientSession *pClientSession;
	
	
	if(id < 0 || id >= STUN_CLIENT_SESSION_NUM)
	{
		printf("The client session id %d is wrong.\n", id);
		return THREAD_FAIL;
	}
	pClientSession = &gStunClientSession[id];
	pClientSession->rtspSock = net_tcp_noblock_connect(NULL, NULL, "192.168.123.185", "554", NET_TIMEOUT);
	if(pClientSession->rtspSock < 0)
	{
		printf("Failed to connect rtsp server.\n");
		free_client_session_id(id);
		return THREAD_FAIL;
	}
	
	sock = pClientSession->sock;
	rtspSock = pClientSession->rtspSock;
	
	fds[0] = sock;
	fds[1] = rtspSock;
	
	while(!gStunSession.quit)
	{
		ret = net_select(fds, 2, 0x1, NET_TIMEOUT);
		if(ret < 0)
			break;
		else if(ret == 0)
			continue;
		else if (ret == (sock | 0x10000))
		{
			recvLen = net_tcp_noblock_recv(sock, (char *)buff, sizeof(buff), 0, NET_TIMEOUT);
			if(recvLen < 0)
			{
				printf("Receive from turn server failed.\n");
				break;
			}
			sendLen = net_tcp_noblock_send(rtspSock, (char *)buff, recvLen, NULL, NET_TIMEOUT);
			if(sendLen != recvLen)
			{
				printf("Send to rtsp server failed.\n");
				break;
			}
		}
		else if(ret == (rtspSock | 0x10000))
		{
			recvLen = net_tcp_noblock_recv(rtspSock, (char *)buff, sizeof(buff), 0, NET_TIMEOUT);
			if(recvLen < 0)
			{
				printf("Receive from rtsp server failed.\n");
				break;
			}
			sendLen = net_tcp_noblock_send(sock, (char *)buff, recvLen, NULL, NET_TIMEOUT);
			if(sendLen != recvLen)
			{
				printf("Send to turn server failed.\n");
				break;
			}
		}
	}

	net_close_socket(&pClientSession->rtspSock);
	net_close_socket(&pClientSession->sock);
	free_client_session_id(id);
	
	return THREAD_OK;
}
*/

int hi_send_new_session_to_local_rtsp(StunClientSession *pClientSession)
{
	int sock;
	int nLen, nRecvLen = 0;
	char szSendBuff[1024];
	char szRecvBuff[1024];
	char *pTemp = szSendBuff;
	char szStr[64];

	sprintf(szStr, "%u", stDianxinCtrl.stPlatformCfg.localRtspPort);
	sock = net_tcp_noblock_connect(NULL, NULL, "127.0.0.1", szStr, NET_TIMEOUT);
	if(sock < 0)
	{
		printf("Connect to local rtsp server failed.\n");
		return -1;
	}

	pTemp += sprintf(pTemp, "OPTIONS rtsp://localhost/0 RTSP/1.0\r\n");
	pTemp += sprintf(pTemp, "CSeq: 2\r\n");
	pTemp += sprintf(pTemp, "User-Agent: mine\r\n");
	pTemp += sprintf(pTemp, "Param: Socket=%d;TurnServerIp=%s;TurnServerPort=%d;RemoteIp=%s;RemotePort=%d\r\n", 
						pClientSession->sock, 
						pClientSession->turnServerIp, pClientSession->turnServerPort, 
						pClientSession->remoteIp, pClientSession->remotePort);
	pTemp += sprintf(pTemp, "\r\n");
	if(net_tcp_noblock_send(sock, szSendBuff, strlen(szSendBuff), NULL, NET_TIMEOUT) < 0)
	{
		printf("Send option to rtsp server failed.\n");
		net_close_socket(&sock);
		return -1;
	}
	
	memset(szRecvBuff, 0, sizeof(szRecvBuff));
	do
	{		
		if(net_select(&sock, 1, 0x1, NET_TIMEOUT) < 0)
		{
			printf("Get localhost rtsp server response failed.\n");
			net_close_socket(&sock);
			return -1;
		}
		nLen = net_tcp_noblock_recv(sock, szRecvBuff+nRecvLen, sizeof(szRecvBuff)-nRecvLen, 0, NET_TIMEOUT);
		if(nLen < 0)
			break;
		nRecvLen += nLen;
		if(nRecvLen > 4)
		{
			if(strcmp(&szRecvBuff[nRecvLen - 4], "\r\n\r\n") == 0)
				break;
		}
	}while(1);
	net_close_socket(&sock);

	if(nRecvLen <= 4 && strstr(szRecvBuff, "RTSP/1.0 200 OK") == NULL)
	{
		printf("The RTSP server response failed.\n");
		return -1;
	}
	
	return 0;
}

static inline int hi_turn_send_message(int sock, stun_buffer *message)
{
	if(net_tcp_noblock_send(sock, (char *)message->buf, message->len, NULL, NET_TIMEOUT) != message->len)
	{
		printf("Send message to turn server failed.\n");
		return -1;
	}
	return message->len;
}

int inline hi_turn_recv_message(int sock, stun_buffer *message)
{
	int msgLen;
	StunMsgHdr msgHdr;
	
	if(net_tcp_noblock_recv(sock, (char *)message->buf, STUN_BUFFER_SIZE, STUN_MSG_HDR_LEN, NET_TIMEOUT) != STUN_MSG_HDR_LEN)
	{
		printf("Receive message from turn server failed.\n");
		return -1;
	}
	memcpy(&msgHdr, message->buf, sizeof(msgHdr));
	msgLen = ntohs(msgHdr.msgLength);
	printf("Receive message body length %d\n", msgLen);

	if(net_tcp_noblock_recv(sock, (char *)message->buf + STUN_MSG_HDR_LEN, STUN_BUFFER_SIZE - STUN_MSG_HDR_LEN,
									msgLen, NET_TIMEOUT) != msgLen)
	{
		printf("Receive message body failed.\n");
		return -1;
	}
	message->len = msgLen + STUN_MSG_HDR_LEN;
	
	return msgLen + STUN_MSG_HDR_LEN;
}


static int hi_turn_message_add_integrity(stun_buffer *message)
{
	if(stun_attr_add_integrity_by_user_str(message->buf, (size_t*)&(message->len), 
					(u08bits*)gStunSession.turnServerUser, gStunSession.realm, 
					(u08bits*)gStunSession.turnServerPass, gStunSession.nonce, gStunSession.shatype)<0) 
	{
		printf(" Cannot add integrity to the message\n");
		return -1;
	}
	return 0;
}

int hi_turn_client_allocate()
{
	stun_buffer message;
	int err_code = 0;
	int isAllocateRecv = 0;
	u08bits err_msg[129];
	
	memset(gStunSession.realm, 0, sizeof(gStunSession.realm));
	memset(gStunSession.nonce, 0, sizeof(gStunSession.nonce));
	do
	{
		stun_set_allocate_request(&message, TURN_SERVER_LEFTTIME, 
						STUN_ATTRIBUTE_REQUESTED_ADDRESS_FAMILY_VALUE_IPV4, 
						STUN_ATTRIBUTE_TRANSPORT_TCP_VALUE, 0);
		if(isAllocateRecv == 1)
			hi_turn_message_add_integrity(&message);
		if(hi_turn_send_message(gStunSession.sock, &message) < 0)
			return -1;
		if(hi_turn_recv_message(gStunSession.sock, &message) < 0)
			return -1;
		if (stun_is_success_response(&message))
		{
			printf("success response.\n");
			u08bits addrStr[64];
			if (stun_attr_get_first_addr(&message,
									STUN_ATTRIBUTE_XOR_RELAYED_ADDRESS, &gStunSession.relay_addr,
									NULL) < 0)
			{
				printf("Get relay address failed.\n");
				return -1;
			}
			addr_to_string(&gStunSession.relay_addr, addrStr);
			gStunSession.haveRelayAddr = 1;
			printf("relay address: %s\n", (char *)addrStr);
			return 0;
		}
		else if(stun_is_challenge_response_str(message.buf, (size_t)message.len,
										&err_code, err_msg,sizeof(err_msg),
										gStunSession.realm, gStunSession.nonce))
		{
			printf("challenge response, realm: %s, nonce: %s.\n", (char *)gStunSession.realm, (char *)gStunSession.nonce);
			//if(err_code == SHA_TOO_WEAK && (clnet_info->shatype == SHATYPE_SHA1)) {
			//					clnet_info->shatype = SHATYPE_SHA256;
			//				}
			if(isAllocateRecv == 0 && strlen((char *)gStunSession.realm) != 0 && strlen((char *)gStunSession.nonce) != 0)
			{
				printf("Receive allocate realm and nonce.\n");
				isAllocateRecv = 1;
				continue;
			}
			return -1;
		}
		else if (stun_is_error_response(&message, &err_code,err_msg,sizeof(err_msg)))
		{
			printf("error response.\n");
			return -1;
		}
	}while(1);
	
	return 0;
}

int hi_turn_connect_server()
{
	int sock;
	char port[16];

	printf("connect turn server %s:%d\n", gStunSession.turnServerIp, gStunSession.turnServerPort);
	sprintf(port, "%d", gStunSession.turnServerPort);
	// connect to turn server
	sock = net_tcp_noblock_connect(NULL, NULL, gStunSession.turnServerIp, port, 1000);
	if(sock < 0)
	{
		printf("Connect to turn server failed.\n");
		return sock;
	}
	return sock;
}

void hi_turn_disconnect_server()
{
	net_close_socket(&gStunSession.sock);
	gStunSession.isAllocation = 0;
	gStunSession.haveRelayAddr = 0;
}

int hi_turn_send_refresh_message()
{
	stun_buffer message;
	uint32_t lefttime = htonl(TURN_SERVER_LEFTTIME - 200);
	
	stun_init_request(STUN_METHOD_REFRESH, &message);
	stun_attr_add(&message, STUN_ATTRIBUTE_LIFETIME, (const char*) &lefttime, 4);
	if(hi_turn_message_add_integrity(&message) < 0)
		return -1;
	if(hi_turn_send_message(gStunSession.sock, &message) < 0)
	{
		hi_turn_disconnect_server();
		return -1;
	}
	return 0;
}

int hi_turn_send_create_permission()
{
	stun_buffer message;
	ioa_addr addr;

	addr_set_any(&addr);
	stun_init_request(STUN_METHOD_CREATE_PERMISSION, &message);
	stun_attr_add_addr(&message, STUN_ATTRIBUTE_XOR_PEER_ADDRESS, &addr);
	if(hi_turn_message_add_integrity(&message)<0) 
		return -1;
	//if(use_fingerprints)
	//	stun_attr_add_fingerprint_str(message.buf, (size_t*) &(message.len));
	return hi_turn_send_message(gStunSession.sock, &message);
}

int hi_turn_send_bind_request(StunClientSession *pClientSess)
{
	stun_buffer message;
	ioa_addr addr;
	int isRecv = 0;
	int err_code = 0;
	u08bits err_msg[129];
	
	do
	{
		printf("Send bind request.\n");
		stun_init_request(STUN_METHOD_CONNECTION_BIND, &message);
		if(isRecv == 0)
			hi_turn_message_add_integrity(&message);
		else
		{
			if(stun_attr_add_integrity_by_user_str(message.buf, (size_t*)&(message.len), 
					(u08bits*)gStunSession.turnServerUser, pClientSess->realm, (u08bits*)gStunSession.turnServerPass, pClientSess->nonce, pClientSess->shatype)<0) 
			{
				printf(" Cannot add integrity to the message\n");
				return -1;
			}
		}
		addr_set_any(&addr);
		stun_attr_add_addr(&message, STUN_ATTRIBUTE_XOR_PEER_ADDRESS, &addr);
		stun_attr_add(&message, STUN_ATTRIBUTE_CONNECTION_ID, (s08bits *)gStunSession.connectId, 4);
		
		if(hi_turn_send_message(pClientSess->sock, &message) < 0)
		{
			printf("Send connection bind messgae failed.\n");
			return -1;
		}
		if(hi_turn_recv_message(pClientSess->sock, &message) < 0)
		{
			printf("Receive connection bind messgae failed.\n");
			return -1;
		}
		
		if(stun_is_success_response(&message))
		{
			printf("Bind request successfully.\n");
			return 0;
		}
		else if(stun_is_challenge_response_str(message.buf, (size_t)message.len,
										&err_code, err_msg,sizeof(err_msg),
										pClientSess->realm, pClientSess->nonce))
		{
			printf("challenge response, realm: %s, nonce: %s.\n", (char *)pClientSess->realm, (char *)pClientSess->nonce);
			if(isRecv == 0 && strlen((char *)pClientSess->realm) != 0 && strlen((char *)pClientSess->nonce) != 0)
			{
				printf("Receive bind realm and nonce.\n");
				isRecv = 1;
				continue;
			}
			return -1;
		}
		else if (stun_is_error_response(&message, &err_code,err_msg,sizeof(err_msg)))
		{
			printf("error response.\n");
			return -1;
		}
	}while(1);

	return 0;
}


int hi_turn_mothod_connection_attempt(stun_buffer *message)
{
	stun_attr_ref sar;
	ioa_addr addr;
	StunClientSession stClientSession;
	StunClientSession *pClientSession = &stClientSession;
	
	sar = stun_attr_get_first_by_type(message, STUN_ATTRIBUTE_CONNECTION_ID);
	if(!sar)
		return -1;
	if(stun_attr_get_value(sar) == NULL)
		return -1;
	memcpy(gStunSession.connectId, stun_attr_get_value(sar), 4);
	if(stun_attr_get_first_addr(message, STUN_ATTRIBUTE_XOR_PEER_ADDRESS, &addr, NULL) < 0)
		return -1;
	//send_bind_request();
	memset(pClientSession, 0, sizeof(StunClientSession));
	strcpy(pClientSession->turnServerIp, gStunSession.turnServerIp);
	pClientSession->turnServerPort = gStunSession.turnServerPort;
	inet_ntop(AF_INET, &addr.s4.sin_addr, pClientSession->remoteIp, sizeof(pClientSession->remoteIp));
	pClientSession->remotePort= addr_get_port(&addr);
	printf("Remote ip %s:%d\n", pClientSession->remoteIp, pClientSession->remotePort);
	pClientSession->sock = hi_turn_connect_server();
	if(pClientSession->sock < 0)
	{
		printf("Connect turn server failed.\n");
		return -1;
	}
	if(hi_turn_send_bind_request(pClientSession) < 0)
	{
		printf("Send bind request failed.\n");
		net_close_socket(&pClientSession->sock);
		//free_client_session_id(id);
		return -1;
	}
	if(hi_send_new_session_to_local_rtsp(pClientSession) < 0)
	{
		printf("Send new session to rtsp server failed.\n");
		net_close_socket(&pClientSession->sock);
		//free_client_session_id(id);
		return -1;
	}
	//pthread_create(&thid, NULL, client_session_thread_func, (void *)id);

	return 0;
}

int hi_turn_decode_unknow_request(int msgType, stun_buffer *message)
{
	int ret = 0;
	
	switch(msgType)
	{
		case STUN_METHOD_CONNECTION_ATTEMPT:
			ret = hi_turn_mothod_connection_attempt(message);
			break;
		default:
			break;
	}

	return ret;
}


////////////////////////////////////////////////////////
static inline int hi_turn_send_udp_message(int sock, stun_buffer *message, struct sockaddr *addr)
{
	if(net_udp_send(sock, (char *)message->buf, message->len, addr) != message->len)
	{
		printf("Send message to turn server failed.\n");
		return -1;
	}
	return message->len;
}

int hi_turn_recv_udp_message(int sock, stun_buffer *message, struct sockaddr *addr)
{
	int nLen;

	if(net_select(&sock, 1, 0x1, NET_TIMEOUT) <= 0)
	{
		printf("Receive message from turn server timeout.\n");
		return -1;
	}
	nLen = sizeof(struct sockaddr);
	message->len = sizeof(message->buf);
	if((message->len = net_udp_recv(sock, (char *)message->buf, message->len, 0, addr, &nLen)) < 0)
	{
		printf("Receive message from turn server failed.\n");
		return -1;
	}
	return 0;
}
/*
int hi_rtsp_get_mapping(int port)
{
	int sock;
	stun_buffer message;
	stun_attr_ref attr;
	ioa_addr addr;
	char natIp[32];
	int natPort;
	struct sockaddr_in sockAddr = {0};
	

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(3478);
	sockAddr.sin_addr.s_addr = inet_addr("202.102.101.30");
	
	sock = net_create_sock(2);
	if(sock < 0)
		return -1;
	net_bind_sock(sock, INADDR_ANY, port);
	memset(&message, 0, sizeof(message));
	stun_set_binding_request(&message);
	if(hi_turn_send_udp_message(sock, &message, (struct sockaddr*)&sockAddr) < 0)
	{
		printf("Send message to turn server failed.\n");
		net_close_socket(&sock);
		return -1;
	}
	if(hi_turn_recv_udp_message(sock, &message, (struct sockaddr*)&sockAddr) < 0)
	{
		printf("Recv message from turn server.\n");
		net_close_socket(&sock);
		return -1;
	}
	net_close_socket(&sock);
	printf("message len: %d\n", message.len);
	if(!stun_is_binding_response(&message))
	{
		printf("The response is not binding response.\n");
		return -1;
	}
	attr = stun_attr_get_first_by_type(&message, STUN_ATTRIBUTE_MAPPED_ADDRESS);
	if(attr == NULL)
	{
		printf("Get response address attribute failed.\n");
		return -1;
	}
	stun_attr_get_addr(&message, attr, &addr, NULL);
	
	addr_to_string(&addr, (u08bits*)natIp);
	natPort = addr_get_port(&addr);

	printf("@@@@@@@@ mapping ip address ==> %s:%d\n", natIp, natPort);
	
	return 0;
}
*/
/////////////////////////////////


static void *hi_turn_client_thread_func()
{
	int nRet;
	int msgType;
	stun_buffer message;
	struct timespec tp;
	int err_code = 0;
	u08bits err_msg[129];

	//hi_rtsp_get_mapping(8888);
	//return 0;
	
	while(!gStunSession.quit)
	{		
		if(gStunSession.sock <= 0)
		{
			if((gStunSession.sock = hi_turn_connect_server()) < 0)
			{
				sleep(3);
				continue;
			}
		}
		
		if(gStunSession.isAllocation == 0)
		{
			if(hi_turn_client_allocate() < 0)
			{
				hi_turn_disconnect_server();
				sleep(3);
				continue;
			}
			if(hi_turn_send_create_permission() < 0)
			{
				hi_turn_disconnect_server();
				sleep(3);
				continue;
			}
			hi_turn_send_refresh_message();
			gStunSession.isAllocation = 1;
			gStunSession.sendRefreshTime = 0;
		}
		
		if((nRet = net_select(&gStunSession.sock, 1, 0x1, 3000)) < 0)
		{
			printf("Select turn socket failed.\n");
			hi_turn_disconnect_server();
			continue;
		}

		clock_gettime(CLOCK_MONOTONIC, &tp);
		if(tp.tv_sec - gStunSession.sendRefreshTime > TURN_SERVER_LEFTTIME/2)
		{
			if(tp.tv_sec - gStunSession.sendRefreshTime >  TURN_SERVER_LEFTTIME*2 &&
				gStunSession.sendRefreshTime != 0)
			{
				hi_turn_disconnect_server();
				continue;
			}
			hi_turn_send_refresh_message();
		}

		if(nRet == 0)
			continue;

		if(hi_turn_recv_message(gStunSession.sock, &message) < 0)
		{
			printf("Failed to receive message.\n");
			hi_turn_disconnect_server();
			continue;
		}
		
		//printf("########################################\n");
		msgType = stun_get_method_str(message.buf, message.len);
		//printf("msgType: %x\n", msgType);
		
		if(stun_is_success_response(&message))
		{
			//printf("Receive response.\n");
			if(msgType == STUN_METHOD_REFRESH)
				gStunSession.sendRefreshTime = tp.tv_sec;
		}
		else if (stun_is_challenge_response_str(message.buf, (size_t)message.len,
													&err_code,err_msg,sizeof(err_msg),
													gStunSession.realm, gStunSession.nonce)) 
		{
		}
		else if(stun_is_error_response(&message, &err_code,err_msg,sizeof(err_msg)))
		{
			
		}
		else
		{
			hi_turn_decode_unknow_request(msgType, &message);
			printf("unknown response\n");
		}
	}
	hi_turn_disconnect_server();
	
	return 0;
}

int hi_turn_get_relay_info(char *pRelayIp, int nIpLen, int *pRelayPort)
{
	if(pRelayIp == NULL || pRelayPort == NULL)
		return -1;
	if(!gStunSession.haveRelayAddr)
		return -1;
	inet_ntop(AF_INET, &gStunSession.relay_addr.s4.sin_addr, pRelayIp, nIpLen);
	*pRelayPort = addr_get_port(&gStunSession.relay_addr);
	return 0;
}

int hi_turn_start_client(const char *pTurnServerIp, int port, const char *turnServerUser, const char *turnServerPass)
{
	if(pTurnServerIp == NULL || strlen(pTurnServerIp) == 0 || port <= 0)
		return -1;
	if(gStunSession.startClient)
		return 0;
	strcpy(gStunSession.turnServerIp, pTurnServerIp);
	gStunSession.turnServerPort = port;
	strcpy(gStunSession.turnServerUser, turnServerUser);
	strcpy(gStunSession.turnServerPass, turnServerPass);
	gStunSession.quit = 0;
	if(pthread_create(&gStunSession.thid, NULL, hi_turn_client_thread_func, NULL) != 0)
	{
		printf("Create turn server failed.\n");
		return -1;
	}
	gStunSession.startClient = 1;
	
	return 0;
}


void hi_turn_stop_client()
{
	if(!gStunSession.startClient)
		return;
	gStunSession.quit = 1;
	pthread_join(gStunSession.thid, NULL);
}


static int hi_turn_send_test(int nSock, const char *pTurnServer, int nTurnPort, int nChangeIp, int nChangePort, unsigned char id)
{
	stun_buffer message;
	struct sockaddr_in sockAddr;
	StunMsgHdr *pMsgHdr;
	memset(&message, 0, sizeof(message));
	stun_set_binding_request(&message);
	stun_attr_add_change_request_str(message.buf, (size_t *)&message.len, nChangeIp, nChangePort);
	pMsgHdr = (StunMsgHdr *)message.buf;
	pMsgHdr->id[0] = id;
	
	bzero(&sockAddr, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(nTurnPort);
	sockAddr.sin_addr.s_addr = inet_addr(pTurnServer);

	return hi_turn_send_udp_message(nSock, &message, (struct sockaddr *)&sockAddr);
}

NatType hi_turn_get_nat_type(char *pNetName, const char *pTurnServer, int nTurnPort)
{
	int i, nRet = 0;
	int nSock, nSock1, nSock2;
	int nPort1, nPort2;
	int nCount;
	char szNetIp[32];
	char szIp[32];
	char szTurnServer2[32];
	int nTurnServer2 = 0;
	int nTurnPort2 = 0;
	int nMapIp1 = 0, nMapIp2 = 0;
	int nMapPort1 = 0, nMapPort2 = 0;
	int mappedIpSame = 0;
	int nIsNat = 0;
	int szFd[2];
	int respTestI = 0;
	int respTestII = 0;
	int respTestIII = 0;
	int respTestI2 = 0;
	int type = 0;
	stun_buffer message;
	StunMsgHdr msgHdr;
	stun_attr_ref sar;
	ioa_addr addr;
	struct sockaddr_in sockAddr;
	
	if(pNetName == NULL || pTurnServer == NULL)
		return StunTypeUnknown;
		
	if(nvr_netcfg_get_ip_addr(pNetName, szNetIp) < 0)
	{
		printf("Get %s ip address failed.\n", pNetName);
		return StunTypeUnknown;
	}
	printf("%s ip address: %s\n", pNetName, szNetIp);
	nSock1 = net_create_sock(2);
	if(nSock1 < 0)
	{
		printf("Create socket1 failed.\n");
		return StunTypeUnknown;
	}
	nSock2 = net_create_sock(2);
	if(nSock2 < 0)
	{
		printf("Create socket2 failed.\n");
		net_close_socket(&nSock1);
		return StunTypeUnknown;
	}
	nPort1 = 22222;
	nPort2 = 22223;
	if(net_bind_sock(nSock1, net_ip_a2n(szNetIp), nPort1) < 0)
	{
		printf("bind port1 failed.\n");
		nRet = -1;
		goto eexit;
	}
	if(net_bind_sock(nSock2, net_ip_a2n(szNetIp), nPort2) < 0)
	{
		printf("bind port2 failed.\n");
		nRet = -1;
		goto eexit;
	}
	nCount = 0;
	while(nCount < 20)
	{
		szFd[0] = nSock1;
		szFd[1] = nSock2;
		nRet = net_select(szFd, 2, 0x1, 150);
		if(nRet < 0)
		{
			printf("Select socket failed.\n");
			goto eexit;
		}
		else if(nRet == 0)		// timeout
		{
			nCount++;
			if(!respTestI)
			{
				hi_turn_send_test(nSock1, pTurnServer, nTurnPort, 0, 0, 1);
			}
			if ( (!respTestI2) && respTestI ) 
			{
				if(nTurnServer2 != 0 && nTurnPort2 != 0)
				{
					net_ip_n2a(nTurnServer2, szTurnServer2, sizeof(szTurnServer2));
					printf("turn server another address %s:%d\n", szTurnServer2, nTurnPort2);
					hi_turn_send_test(nSock1, szTurnServer2, nTurnPort2, 0, 0, 10);
				}
			}
			if ( !respTestII )
			{
				hi_turn_send_test(nSock2, pTurnServer, nTurnPort, 1, 0, 2);
			}
			if ( !respTestIII )
			{
				hi_turn_send_test(nSock2, pTurnServer, nTurnPort, 0, 1, 3);
			}
		}
		for(i = 0; i < 2; i++)
		{
			if (nRet == (szFd[i] | 0x10000))
			{
				nSock = szFd[i];
				if(hi_turn_recv_udp_message(nSock, &message, (struct sockaddr *)&sockAddr) < 0)
				{
					printf("Receive turn message failed.\n");
					break;
				}
				if(!stun_is_success_response(&message))
				{
					printf("The response is not sucess.\n");
					break;
				}
				memcpy(&msgHdr, message.buf, sizeof(msgHdr));
				printf("octor id: %d\n", msgHdr.id[0]);
				switch(msgHdr.id[0])
				{
					case 1:
					{
						if ( !respTestI )
						{	
							sar = stun_attr_get_first_by_type(&message, STUN_ATTRIBUTE_MAPPED_ADDRESS);
							if(sar == NULL)
							{
								printf("Get response address attribute failed.\n");
								break;
							}
							stun_attr_get_addr(&message, sar, &addr, NULL);
							nMapIp1 = ntohl(addr.s4.sin_addr.s_addr);
							nMapPort1 = addr_get_port(&addr);
							sar = stun_attr_get_first_by_type(&message, OLD_STUN_ATTRIBUTE_CHANGED_ADDRESS);
							if(!sar)
								break;
							stun_attr_get_addr(&message, sar, &addr, NULL);
							nTurnServer2 = ntohl(addr.s4.sin_addr.s_addr);
							nTurnPort2 = addr_get_port(&addr);
						}
						respTestI = 1;
						break;
					}
					case 2:
					{
						respTestII = 1;
						break;
					}
					case 3:
					{
						respTestIII = 1;
						break;
					}
					case 10:
					{
						 if ( !respTestI2 )
						 {
						 	sar = stun_attr_get_first_by_type(&message, STUN_ATTRIBUTE_MAPPED_ADDRESS);
							if(sar == NULL)
							{
								printf("Get response address attribute failed.\n");
								break;
							}
							stun_attr_get_addr(&message, sar, &addr, NULL);
							nMapIp2 = ntohl(addr.s4.sin_addr.s_addr);
							nMapPort2 = addr_get_port(&addr);
							if(nMapIp1 == nMapIp2 && nMapPort1 == nMapPort2)
							{
								mappedIpSame = 1;
							}
						 }
						 respTestI2 = 1;
					}
				}
			}
		}
	}
	nSock = net_create_sock(2);
	if(nSock < 0)
	{
		nIsNat = 1;
	}
	else
	{
		if(net_bind_sock(nSock, nMapIp1, nMapPort1) < 0)
			nIsNat = 1;
		else
			nIsNat = 0;
		net_close_socket(&nSock);
	}
	printf("Turn server address1 %s:%d\n", pTurnServer, nTurnPort);
//	printf("Turn server address2 %s:%d\n", szTurnServer2, nTurnPort2);
	printf("respTestI: %d\n", respTestI);
	printf("respTestII: %d\n", respTestII);
	printf("respTestIII: %d\n", respTestIII);
	printf("respTestI2: %d\n", respTestI2);
	printf("is nat: %d\n", nIsNat);
	printf("mappedIpSame: %d\n", mappedIpSame);
	net_ip_n2a(nMapIp1, szIp, sizeof(szIp));
	printf("map address1 %s:%d ==> %s:%d\n", szNetIp, nPort1, szIp, nMapPort1);
	net_ip_n2a(nMapIp2, szIp, sizeof(szIp));
	printf("map address2 %s:%d ==> %s:%d\n", szNetIp, nPort2, szIp, nMapPort2);

	if ( respTestI ) // not blocked 
	{
		if ( nIsNat )
		{
			if ( mappedIpSame )
			{
				if (respTestII)
				{
					type = StunTypeIndependentFilter;
				}
				else
				{
					if ( respTestIII )
					{
						type = StunTypeDependentFilter;
					}
					else
					{
						type = StunTypePortDependedFilter;
					}
				}
			}
			else // mappedIp is not same 
			{
				type = StunTypeDependentMapping;
			}
		}
		else  // isNat is false
		{
			if (respTestII)
			{
				type = StunTypeOpen;
			}
			else
			{
				type = StunTypeFirewall;
			}
		}
	}
	else
	{
		type = StunTypeBlocked;
	}
	
	switch(type)
	{   
		case StunTypeUnknown:
			/*break;*/
		case StunTypeFailure:
			nRet = 0;
			break;
		case StunTypeOpen:
			nRet = 6;
			break;
		case StunTypeBlocked:
			nRet = 5;
			break;

		case StunTypeIndependentFilter:
			nRet = 1;
			break;
		case StunTypeDependentFilter:
			nRet = 2;
			break;
		case StunTypePortDependedFilter:
			nRet = 3;
			break;
		case StunTypeDependentMapping:
			nRet = 4;
			break;
		default:
			nRet = 0;
			break;
	}

	printf("nat type: %d\n", nRet);
	
eexit:	
	net_close_socket(&nSock1);
	net_close_socket(&nSock2);

	return nRet;
}

int hi_turn_client_init()
{
	memset(&gStunSession, 0, sizeof(gStunSession));

	return 0;
}

void hi_turn_client_uninit()
{
	
}


