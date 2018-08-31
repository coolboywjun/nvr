
#ifndef __NET_STRUCT_H__
#define __NET_STRUCT_H__

#define THREAD_SUCCESS ((void*)0)
#define THREAD_FAIL		((void*)-1)

#define UDP_SERVER_PORT 	33080
#define TCP_SERVER_PORT 	33081

#define HEAD_MAGIC			33080

enum
{
	PSTJ_PING_CMD = 1000,
	PSTJ_BROADCASE_SCAN,
	PSTJ_BROADCASE_IP_TABLE_TO_UPDATE,
	PSTJ_SEND_EXE_FILE_TO_RUN,
	PSTJ_BROADCASE_IP_TABLE_TO_SET_MAC_AND_HWID,
	PSTJ_GET_DEVICE_INFO,
	PSTJ_SET_PLATFORM_TYPE,
};

// ICMP header  
typedef struct _ihdr
{
	unsigned char   i_type; 
	unsigned char   i_code;					// type sub code
	unsigned short  i_cksum; 
	unsigned short  i_id; 
	unsigned short  i_seq; 
	// This is not the std header, but we reserve space for time
	unsigned long   timestamp; 
}IcmpHeader;

typedef struct stJMsgHeader
{
	unsigned int    uMagic;
	unsigned int    uType;
	unsigned int    ret;
	unsigned char   dat[];
}* PSTJMsgHeader;

typedef struct _net_node_struct
{
	unsigned long random;
	unsigned char mac[6];
	struct in_addr ip_addr;
}net_node_struct;

typedef struct _net_struct
{
	int dev_num;
	struct in_addr server_addr;
	int server_port;
	int use_which_compare;	// 使用随机数还是MAC进行设置，0:使用MAC，1:使用随机数
	net_node_struct node[32];
}net_struct;

typedef struct _cmd_info
{
	unsigned long magic;
	int cmd;
	char filename[32];
	int filelen;
	char data[];
}cmd_info;

enum
{
	CMD_REQUEST_FILE = 0x1000,
	CMD_RESPONSE,
	CMD_UPDATE_FILNISH,
	CMD_UPDATE_FAIL,
	CMD_REQUEST_MAC_AND_HWID,
};

#endif
