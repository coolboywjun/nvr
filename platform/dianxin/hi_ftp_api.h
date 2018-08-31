#ifndef _HI_FTP_API_H
#define _HI_FTP_API_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "nvr_net_interface.h"


typedef enum _FTP_ERR_E
{
	HI_FTP_SUCCESS				  =  0,
	HI_FTP_CONN_PARAM_ERR 		  = -1,
	HI_FTP_CONN_SVR_ERR   		  = -2,
	HI_FTP_LOGIN_PARAM_ERR		  = -3,
	HI_FTP_LOGIN_USER_ERR 		  = -4,
	HI_FTP_LOGIN_PSW_ERR  		  = -5,
	HI_FTP_UL_TYPE_ERR		      = -6,
	HI_FTP_UL_FILE_SIZE_ERR      = -7,
	HI_FTP_UL_OPEN_FILE_ERR      = -8,
	HI_FTP_UL_CHK_REMOTE_DIR_ERR = -9,
	HI_FTP_UL_CREATE_DATA_CHN_ERR= -10,
	HI_FTP_UL_SEND_DATA_CHN_ERR  = -11,
	HI_FTP_DL_TYPE_ERR		      = -12,
	HI_FTP_DL_FILE_SIZE_ERR      = -13,
	HI_FTP_DL_OPEN_FILE_ERR      = -14,
	HI_FTP_DL_CREATE_DATA_CHN_ERR= -15,	
	HI_FTP_DL_RECV_CONFIRM_ERR   = -16,
	HI_FTP_DL_RECV_DATA_ERR      = -17,
}FTP_ERR_E;

#define CMD_BUF_SIZE 1023
#define CMD_DIR_SIZE 255

struct line_data 
{
  int count;
  char** lines;
};

typedef struct _HI_FTP_INFO
{
	char	serverIp[64];
	int     serverPort;
	char    userName[64];
	char    passWord[64];
	int		ctrlfd;   //控制连接fd
	int		datafd;   //数据连接fd
	int     portfd;   //主动服务下数据监听fd
	int 	bLogin;   //0 还没有登陆 1已经登陆了
	int 	bPasv;    //==:被动  0:主动
	int     dataPort; //被动发送数据端口
	int     startPort;
	int     endPort;
	int     dataSvrPort;
	char    root_dir[CMD_DIR_SIZE + 1];
	char 	cmd_buffer[CMD_BUF_SIZE + 1];     /* in/out commands */
	char 	cmd_line[CMD_BUF_SIZE + 1];       /* in     line sep */
	char* 	cmd_ptr;                         /* ->cmd_buffer */
}HI_FTP_INFO, *LPHI_FTP_INFO;

typedef int (*UpdateCb)(char *pData, int nDataLen, int nPackNo, int nTotalLen);

int hi_ftp_connect(LPHI_FTP_INFO pFtp);

int hi_ftp_login(LPHI_FTP_INFO pFtp);


int hi_ftp_upload_file(LPHI_FTP_INFO pFtp, char *src, char *dst);

int hi_ftp_download_file(LPHI_FTP_INFO pFtp, char *src, UpdateCb funcUpdateCb);

int hi_ftp_disconnect(LPHI_FTP_INFO pFtp);

int hi_ftp_upload_file_begin(LPHI_FTP_INFO pFtp, char *dstPath);

int hi_ftp_uploading_file(LPHI_FTP_INFO pFtp, char *buf, int bufsize);

int hi_ftp_upload_file_end(LPHI_FTP_INFO pFtp);


#ifdef __cplusplus
}
#endif 

#endif

