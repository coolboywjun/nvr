#ifndef _HI_COMM_H
#define _HI_COMM_H

#include <nvr_server_headers.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif 

#define COMM_485_NO 0
#define COMM_232_NO 1

#define NVR_MCU_COMM_INDEX NVR_MAX_SUPPORT_COMM_NO

typedef int (*CommCapCb)(int nPort, char *szData, int nLen);

typedef enum _COMM_ERR_E
{
	COMM_SUCCESS     =  0,
	COMM_PARAM_ERR   = -1,	
	COMM_OPEN_ERR    = -2,
	COMM_GET_OPT_ERR = -3,
	COMM_SET_OPT_ERR = -4,
	COMM_WR_DATA_ERR = -5,
	COMM_RD_DATA_ERR = -6,
	COMM_NOT_OPEN_ERR= -7,
}COMM_ERR_S;

typedef struct _COMM_PARA_S_
{
	unsigned int	baudrate;	//300---115200
	unsigned char	databit;	//串口数据位长度0=5位 1=6位 2=7位 3=8位	其他=8
	unsigned char	stopbit;	//停止位位数	0=1位 1=2位 其他=1位
	unsigned char	checkType;	//校验	0=无  1=奇  2=偶  3=恒1  4=恒0
	unsigned char	flowctrl; 	//软流控/硬流控/无流控		备用	
}COMM_PARA_S;

typedef struct _NVR_COMM_S_
{
	int			init;
	int   		fd;
	pthread_mutex_t mutexLock;
	COMM_PARA_S stCommPara;
}NVR_COMM_S, *LPNVR_COMM_S;

typedef struct _NVR_COMM_TASK_S_
{
	unsigned char	u8Exit;
	unsigned char	u8Run;
	unsigned char	u8Cap;
	unsigned char	u8Res;
	
	int		s32MsgId;
	int		s32MsgNum;

	CommCapCb	capCb;
	pthread_t	thdId;
	
	pthread_mutex_t mutexTask;
	pthread_cond_t   condTask;	
}NVR_COMM_TASK_S, *LPNVR_COMM_TASK_S;

typedef struct _NVR_COMM_MSG_S_
{
	unsigned char	u8Port;
	unsigned char	u8Len;
	unsigned char	u8Data[254];
}NVR_COMM_MSG_S, *LPNVR_COMM_MSG_S;
	
int nvr_comm_open(int commNo, char *szDev, COMM_PARA_S *pStComm, int nVTime, int nVMin);

int nvr_comm_write_data(unsigned char nCommNo, char *data, int nDataLen);

int nvr_comm_read_data(unsigned char nCommNo, char *pBuf, int nSize, int nTimeOut, int nReadWaitTime);

void nvr_comm_close(unsigned char commNo);

int nvr_comm_get_fd(int commNo);

int nvr_comm_init();

int nvr_comm_uninit();

int nvr_comm_update(int nPort, NVR_DEV_COMM_CFG_S *pStComm);

int nvr_comm_send_data(int nPort, char *data, int nDataLen);

int nvr_comm_start_capture(int nPort, CommCapCb capCb);

int nvr_comm_stop_capture(int nPort);

int nvr_comm_send_ptz_cmd(char *data, int nDataLen);

#ifdef __cplusplus
}
#endif 

#endif


