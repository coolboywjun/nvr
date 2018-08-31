#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include "linux_list.h"
#include "hi_ftp_api.h"
#include "net_api.h"

#define FTP_CONNECT_TIMEOUT 10000
#define FTP_MAX_CONFIRM_TIMEOUT 15000
#define FTP_RECV_TIMEOUT    3000
#define FTP_SEND_TIMEOUT    3000
#define FTP_READ_SEND_BLOCK 4096

/*----------------------------------------------*
 * 函数实现                                     *
 *----------------------------------------------*/
void  clear_buf(LPHI_FTP_INFO pFtp)
{
	if (pFtp)
	{
		memset(pFtp->cmd_buffer, 0, CMD_BUF_SIZE+1);
		memset(pFtp->cmd_line, 0, CMD_BUF_SIZE+1);
		pFtp->cmd_ptr = pFtp->cmd_buffer + CMD_BUF_SIZE;
	}	
	return ;
}

void store_line(char* line, struct line_data* d) 
{
    int i = d->count;
    d->lines = realloc(d->lines, sizeof(char*) * (++d->count));
    d->lines[i] = malloc(CMD_BUF_SIZE - 9);
    strncpy(d->lines[i], line, CMD_BUF_SIZE - 10);
    d->lines[i][CMD_BUF_SIZE - 10] = 0; 
}

static char *recv_line_next(char *from, char *endBuf)
{
    char *new_ptr;

    if ((new_ptr = strchr(from + 1, '\n')))
	{
		if (new_ptr[-1] == '\r')
	    	new_ptr--;

    } 
	else if (!(new_ptr = strchr(from + 1, '\r'))) 
	{
		new_ptr = endBuf;
    }
    return new_ptr;
}

char* recv_line(LPHI_FTP_INFO pFtp) 
{
	char* new_ptr;
	int  n; 
	size_t len;

	if (*(pFtp->cmd_ptr)== 0) 
	{
		if (net_select(&pFtp->ctrlfd, 0x1, 0x1, FTP_RECV_TIMEOUT) != 
			(pFtp->ctrlfd | 0x10000))
		{
#ifdef DEBUG
			printf("recv line select error %s\n", strerror(errno));
#endif
			return 0;	
		}
	
		ssize_t bytes = net_tcp_noblock_recv(pFtp->ctrlfd, pFtp->cmd_buffer,
							CMD_BUF_SIZE, 0, FTP_RECV_TIMEOUT);
		if (bytes <= 0)
		    return 0;

		pFtp->cmd_buffer[bytes] = 0;
		pFtp->cmd_ptr = pFtp->cmd_buffer;
		if (*(pFtp->cmd_ptr) == '\n')
		    pFtp->cmd_ptr++;
    }

	new_ptr = recv_line_next(pFtp->cmd_ptr + 1, pFtp->cmd_buffer + CMD_BUF_SIZE);
	n = new_ptr - pFtp->cmd_ptr;
	
	strncpy(pFtp->cmd_line, pFtp->cmd_ptr, n); 
	pFtp->cmd_line[n] = 0;
	pFtp->cmd_ptr = new_ptr;
	
   	 if (pFtp->cmd_ptr == pFtp->cmd_buffer + CMD_BUF_SIZE) 
	{
		ssize_t bytes = net_tcp_noblock_recv(pFtp->ctrlfd, pFtp->cmd_buffer,
							CMD_BUF_SIZE, 0, FTP_RECV_TIMEOUT);
		if (bytes <= 0) 
			return 0;

		pFtp->cmd_buffer[bytes] = 0;
		pFtp->cmd_ptr = pFtp->cmd_buffer;

		new_ptr = recv_line_next(pFtp->cmd_ptr + 1, pFtp->cmd_buffer + CMD_BUF_SIZE);
		n = new_ptr - pFtp->cmd_ptr; 
		len = strlen(pFtp->cmd_line);

		strncat(pFtp->cmd_line, pFtp->cmd_ptr, n);
		pFtp->cmd_line[len + n] = 0;
		pFtp->cmd_ptr = new_ptr;
    }
	
    if (*pFtp->cmd_ptr == '\r' && pFtp->cmd_ptr[1] == '\n')
		pFtp->cmd_ptr += 2;
    else if (*pFtp->cmd_ptr == '\n')
		pFtp->cmd_ptr++;
	
    return pFtp->cmd_line;
}

int recv_answer(LPHI_FTP_INFO pFtp, struct line_data* d) 
{
    int len; char* answer; int code; 

	clear_buf(pFtp);

    answer = recv_line(pFtp);

    if (!answer || (len = strlen(answer)) < 4)
		return 0;

    while (*(pFtp->cmd_ptr) != 0 || len < 4 || 
	   !isdigit(answer[0]) || !isdigit(answer[1]) || 
	   !isdigit(answer[2]) || answer[3] == '-') 
	{
		if (!(answer = recv_line(pFtp)))
		    return 0;

		len = strlen(answer);
		if (d && *(pFtp->cmd_ptr) != 0)
	    	store_line(answer, d);		
    }

    if (sscanf(answer, "%i", &code) != 1)
		return 0;

    return code;
}

int send_commands(LPHI_FTP_INFO pFtp)
{
	int nLen = strlen(pFtp->cmd_buffer)+2;
	int retVal = 0;

	strcat(pFtp->cmd_buffer, "\r\n");
	
	if (nLen != net_tcp_noblock_send(pFtp->ctrlfd, pFtp->cmd_buffer, 
				nLen, NULL, FTP_SEND_TIMEOUT))
	{
#if DEBUG
		printf("hi_ftp_login send cmd error %s\n", strerror(errno));
#endif
		return 0;
	}
	else
	{
		retVal = recv_answer(pFtp, NULL);
	}
	return retVal;
}
//***************************************************************
//ftp 操作函数

//改变远程工作目录
int remote_chdir(LPHI_FTP_INFO pFtp, char* arg) 
{
    int rv;

    snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "CWD %s", arg);
    if (!(rv = send_commands(pFtp)))
		return -2;			/* bad err, intr */
  
    if (rv == 250) 
		return 0;	/* ok */
    return -1;			/* fail */
}
//创建远程目录
int remote_mkdir(LPHI_FTP_INFO pFtp, char* arg)
{
    int rv;
	char szDirName[256] = {0}; 
	char *p = NULL;
	char *del="/";

	strncpy(szDirName, arg, sizeof(szDirName)-1);
	
    snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "MKD %s", arg);
    if (!(rv = send_commands(pFtp)))
	{
		return -2;			/* bad err, intr */
    }

    if (rv == 257) 
		return 0;	/* ok */

	p = strtok(szDirName, del);
	printf("p:%s\n", p);
	while (p)
	{
		if (remote_chdir(pFtp, p) != 0)
		{//没有该目录
		    snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "MKD %s", p);
		    if (!(rv = send_commands(pFtp)))
			{
				return -2;			/* bad err, intr */
		    }
			
		    if (rv != 257) 
			{	
				return -2;	/* ok */
		    }

			if (remote_chdir(pFtp, p) != 0)
				return -2;			/* bad err, intr */
		}	
		
		p = strtok(NULL, del); 
		printf("p:%s\n", p);
	}
	return 0;
}
//删除远程目录
int remote_rmdir(LPHI_FTP_INFO pFtp, char* arg)
{
    snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "RMD %s", arg);
    return (send_commands(pFtp) == 250);
}
//获取ftp服务器的当前目录
char *remote_getcwd(LPHI_FTP_INFO pFtp)
{
    char* start, *end;

	snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "PWD");
    if (!send_commands(pFtp))
		return 0;

	if ((end = strrchr(pFtp->cmd_buffer, '"')) == 0 ||
	(start = strchr(pFtp->cmd_buffer, '"')) == 0) 
		return 0;
    *end = 0; start++;
    return start;
}

long long local_size(LPHI_FTP_INFO pFtp, char *fileName)
{
	FILE *fp      = NULL;	
	long long   fileLen = 0;
	
	if (NULL == (fp = fopen(fileName, "r+b")))
	{
		return -1;
	}
	
	fseek(fp, 0, SEEK_END);
	
	if((fileLen= ftell(fp)) < 0)
	{
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return fileLen;
}

long long remote_size(LPHI_FTP_INFO pFtp, char *fileName)
{
    long long size; 
	int rv;
	
    snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "SIZE %s", fileName);
	rv = send_commands(pFtp);	
    if (!rv) 
	{
#ifdef DEBUG	
		printf("get remote file size error %d\n", rv);
#endif		
		return -2;

    }	
    else if (rv != 213) 
    {
#ifdef DEBUG	
		printf("get remote file size error %d\n", rv);
#endif
		return -1;

    }  
    sscanf(pFtp->cmd_buffer + 4, "%lld", &size);
#ifdef DEBUG
	printf("remote file size length %d\n", size);
#endif
    return size;
}

int get_data_trans_type(LPHI_FTP_INFO pFtp)
{
    unsigned int b[6]; 
    int i, answer, len; 
	char* port_str;

	snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "PASV");
    if ((answer = send_commands(pFtp)) != 227) 
	{
#ifdef DEBUG	
		printf("get data transfer type: PASV Error %d\n", answer);
#endif
		return 0;

    }
    port_str = pFtp->cmd_buffer + 4;
    len = strlen(port_str += 4);

    for (i = 0; i < len; i++) 
		if (!isdigit(port_str[i])) 
			port_str[i] = ' ';
  
    if (sscanf(port_str, "%u %u %u %u %u %u", 
	       b, b + 1, b + 2, b + 3, b + 4, b + 5) != 6)
		return 0;

    pFtp->bPasv    = 1;
	pFtp->dataPort = b[4] * 256 + b[5];
#ifdef DEBUG
	printf("data transfer port: %d\n", pFtp->dataPort);
#endif
	return 1;
}


int hi_ftp_create_port(LPHI_FTP_INFO pFtp)
{
	int     dataSvrSock = 0;
	char    szPort[16]  = {0};
	int     addrLen     = sizeof(struct sockaddr_in);
    int     startPort   = pFtp->startPort;
    int     endPort     = pFtp->endPort;	
	
	for(pFtp->dataSvrPort = startPort; pFtp->dataSvrPort <= endPort; 
									   pFtp->dataSvrPort++)
	{
		memset(szPort, 0, sizeof(szPort));
		net_itoa(pFtp->dataSvrPort, szPort);
		
		if((dataSvrSock = net_tcp_listen(NULL, szPort, &addrLen)) > 0)
		{
			break;
		}
	}

   if(dataSvrSock <= 0)
   {
#ifdef DEBUG   
   		printf("create ftp server port error\n");
#endif
		return -1;
   }

   return dataSvrSock;
}

int hi_ftp_start_trans_data(LPHI_FTP_INFO pFtp, int bDownLoad, char* fileName,
							long long *pStartPos, FILE *pFile)
{
	int answer = 0;
    snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "%s %s",
		bDownLoad ? "RETR" : "STOR", fileName);
	
	if ((answer = send_commands(pFtp)) != 150 &&
			answer != 125 && answer != 226)
    {
#ifdef DEBUG	
		printf("Send RETR|STOR Cmd error %d\n", answer);
#endif
		return -1;
	}
	return 0;
}

int recv_confirm(LPHI_FTP_INFO pFtp)
{
	int answer = 0;
	
	if ((answer = recv_answer(pFtp, NULL)) != 226)
	{
#ifdef DEBUG			
		printf("recv_confirm recv transfer complete error %d\n", answer);
#endif
		return -1;	
	}
	return 0;
}

int hi_mkdir(LPHI_FTP_INFO pFtp, char *dst)
{
	char szDirName[256] = {0};
	char *pDir = NULL;
	
	strncpy(szDirName, dst, sizeof(szDirName) - 1);
	pDir = strrchr(szDirName, '/');
	if (pDir != NULL && pDir != szDirName)
	{
		*pDir = '\0';

		if (remote_chdir(pFtp, szDirName) == 0)
			return 0;	/* ok */

		if (remote_mkdir(pFtp, szDirName) != 0)
		{
#ifdef DEBUG	
			printf("create remote dir %s error\n", dst);
#endif
			return -1;
		}
	}
	return 0;
}


int hi_ftp_create_data_channel(LPHI_FTP_INFO pFtp, int bDownLoad, 
				long long *pPos, char *fileName, FILE *pFile)
{
	char szPort[8] = {0};
	
	unsigned long nIp = 0;
	struct timeval  timeOut;
	
	if ((pFtp->bPasv = get_data_trans_type(pFtp)) == 0)
	{
		if ((pFtp->datafd = hi_ftp_create_port(pFtp)) <= 0)
		{
			net_close_socket(&pFtp->datafd);
			net_close_socket(&pFtp->portfd);			
			return -1;
		}			
		nIp = net_get_sock_ip(pFtp->ctrlfd);
		
		sprintf(pFtp->cmd_buffer, "PORT %d,%d,%d,%d,%d,%d\r\n", 
			   (nIp & 0xff000000) >> 24, (nIp & 0xff0000)   >> 16,
			   (nIp & 0xff00)     >> 8,  (nIp & 0xff),
			   (pFtp->dataSvrPort & 0xff00) >> 8,
			   (pFtp->dataSvrPort & 0xff));

		if (send_commands(pFtp) != 200)
		{
			net_close_socket(&pFtp->datafd);
			net_close_socket(&pFtp->portfd);				
			return -1;
		}	
	}
	else
	{
		sprintf(szPort, "%d", pFtp->dataPort);
		if ((pFtp->datafd = net_tcp_noblock_connect(NULL, NULL, 
			pFtp->serverIp, szPort, FTP_CONNECT_TIMEOUT)) <= 0)
		{
#ifdef DEBUG
			printf("connect ftp data channel %s:%s error %s\n", 
					pFtp->serverIp, szPort, strerror(errno));
#endif
			net_close_socket(&pFtp->datafd);
			net_close_socket(&pFtp->portfd);
			return -1;
		}
	}

	if (hi_ftp_start_trans_data(pFtp, bDownLoad, fileName, pPos, pFile) != 0)
	{
		net_close_socket(&pFtp->datafd);
		net_close_socket(&pFtp->portfd);	
		return -1;
	}

	if (pFtp->bPasv == 0)
	{
		if ((pFtp->portfd | 0x10000) == 
				net_select(&pFtp->portfd, 0x1, 0x1, FTP_CONNECT_TIMEOUT))
		{
			timeOut.tv_sec = 3;
			timeOut.tv_usec= 0;
			if ((pFtp->datafd = net_tcp_noblock_accept(pFtp->portfd, 
					NULL, NULL, &timeOut)) < 0)
			{
				net_close_socket(&pFtp->datafd);
				net_close_socket(&pFtp->portfd);
				return -1;
			}
		}
		else
		{
			net_close_socket(&pFtp->datafd);
			net_close_socket(&pFtp->portfd);
			return -1;
		}
	}
	
	return pFtp->datafd;
}

//从远程服务器下载文件
int hi_ftp_download_file(LPHI_FTP_INFO pFtp, char *src, UpdateCb funcUpdateCb)
{
	//send type command
	long long fileSize = 0, start_pos = 0;
	int answer = 0;
	int nPackNo = 0;
	
	snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "TYPE I");
	if (!(answer = send_commands(pFtp)))	
	{
#if DEBUG
		printf("TYPE I command retVal %d error\n", answer);
#endif
		return HI_FTP_DL_TYPE_ERR;
	}

	if ((fileSize = remote_size(pFtp, src)) <= 0)
	{
		return HI_FTP_DL_FILE_SIZE_ERR;
	}

	if (hi_ftp_create_data_channel(pFtp, 1, &start_pos, src, NULL) <= 0)
	{
		unlink(src);
		return HI_FTP_DL_CREATE_DATA_CHN_ERR;
	}

    	while (start_pos < fileSize) 
	{
		int written;

		if (net_select(&pFtp->datafd, 0x1, 0x1, FTP_RECV_TIMEOUT) != 
			(pFtp->datafd | 0x10000))
			break;

		if ((written = net_tcp_noblock_recv(pFtp->datafd, pFtp->cmd_buffer,
				CMD_BUF_SIZE+1, 0, FTP_RECV_TIMEOUT)) <= 0)
	    	break;
		if (funcUpdateCb)
		{
			if (funcUpdateCb(pFtp->cmd_buffer, 
				written, nPackNo++, fileSize) < 0)
			{
				printf("Write update file to flash fail.\n");
				break;
			}	
		}	 

		start_pos += written;
    	}
	if (funcUpdateCb)
		funcUpdateCb(pFtp->cmd_buffer, 0, nPackNo++, fileSize);

	net_close_socket(&pFtp->datafd);
	net_close_socket(&pFtp->portfd);
	
	if (start_pos >= fileSize) 
	{
		if ((answer = recv_confirm(pFtp)) != 0 )
		{
			return HI_FTP_DL_RECV_CONFIRM_ERR;
		}
#ifdef DEBUG
		printf("download file %s size %d ok\n", src, start_pos);
#endif		
		return HI_FTP_SUCCESS;
	}

#ifdef DEBUG
	printf("download file %s error download size %d total size %d\n",
				src, start_pos, fileSize);
#endif
	unlink(src);
	return HI_FTP_DL_RECV_DATA_ERR;	
}

int hi_ftp_upload_file(LPHI_FTP_INFO pFtp, char *src, char *dst)
{
	long long fileSize = 0, start_pos = 0;
	int answer = 0;
	int readn  = 0;
	int ret    = 0;
	FILE *pFile;
	char *pDir = NULL;
	
	if ((pDir= remote_getcwd(pFtp)) != NULL)
		strncpy(pFtp->root_dir, pDir, CMD_DIR_SIZE);
			
	snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "TYPE I");
	if (!(answer = send_commands(pFtp)))	
	{
#ifdef DEBUG
		printf("TYPE I command retVal %d error\n", answer);
#endif	
		return HI_FTP_UL_TYPE_ERR;
	}

	if ((fileSize = local_size(pFtp, src)) <= 0)
	{
#ifdef DEBUG
		printf("hi_ftp_upload_file get file size error %lld\n", fileSize);
#endif
		return HI_FTP_UL_FILE_SIZE_ERR;
	}
	
	if ((pFile = fopen(src, "r+b")) == NULL)
	{
#ifdef DEBUG	
		printf("fopen file %s error\n", src);
#endif
		return HI_FTP_UL_OPEN_FILE_ERR;
	}

	if (remote_chdir(pFtp, pFtp->root_dir) || hi_mkdir(pFtp, dst))
	{
		fclose(pFile);
		return HI_FTP_UL_CHK_REMOTE_DIR_ERR;
	}
	
	if (hi_ftp_create_data_channel(pFtp, 0, &start_pos, dst, pFile) <= 0)
	{
		fclose(pFile);
		return HI_FTP_UL_CREATE_DATA_CHN_ERR;
	}
	
    	while (start_pos < fileSize) 
	{
		readn = ((CMD_BUF_SIZE+1) > (fileSize - start_pos)) ?
				(fileSize - start_pos) : (CMD_BUF_SIZE+1);
		
		if ((readn = fread(pFtp->cmd_buffer, 1, readn, pFile)) <= 0)
	    	break;
		
		if (net_tcp_noblock_send(pFtp->datafd, pFtp->cmd_buffer,
				readn, NULL, FTP_SEND_TIMEOUT) != readn)
	    {
#ifdef DEBUG	
			printf("hi_ftp_upload_file error %s\n", strerror(errno));
#endif	    
	    	break;
		}
		start_pos += readn;
    }
	
	fclose(pFile);
	net_close_socket(&pFtp->datafd);
	net_close_socket(&pFtp->portfd);

	if (start_pos >= fileSize && (ret = recv_confirm(pFtp)) == 0) 
	{
#ifdef DEBUG
		printf("upload file %s to %s ok upload size %lld total size %lld\n",
				src, dst, start_pos, fileSize);
#endif	
		return HI_FTP_SUCCESS;
	}

#ifdef DEBUG
	printf("upload file %s to %s err upload size %lld total size %lld\n",
				src, dst, start_pos, fileSize);
#endif
	return HI_FTP_UL_SEND_DATA_CHN_ERR;		
}
int hi_ftp_upload_file_begin(LPHI_FTP_INFO pFtp, char *dstPath)
{
	long long start_pos = 0;
	int answer = 0;
	char *pDir = NULL;
	
	if ((pDir= remote_getcwd(pFtp)) != NULL)
		strncpy(pFtp->root_dir, pDir, CMD_DIR_SIZE);
			
	snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "TYPE I");
	if (!(answer = send_commands(pFtp)))	
	{
#ifdef DEBUG
		printf("TYPE I command retVal %d error\n", answer);
#endif	
		return HI_FTP_UL_TYPE_ERR;
	}

	if (remote_chdir(pFtp, pFtp->root_dir) || hi_mkdir(pFtp, dstPath))
	{
		return HI_FTP_UL_CHK_REMOTE_DIR_ERR;
	}
	
	if (hi_ftp_create_data_channel(pFtp, 0, &start_pos, dstPath, NULL) <= 0)
	{
		return HI_FTP_UL_CREATE_DATA_CHN_ERR;
	}
	return 0;
}
int hi_ftp_uploading_file(LPHI_FTP_INFO pFtp, char *buf, int bufsize)
{
	if (net_tcp_noblock_send(pFtp->datafd, buf,
				bufsize, NULL, FTP_SEND_TIMEOUT) != bufsize)
		return -1;
	return 0;
}
int hi_ftp_upload_file_end(LPHI_FTP_INFO pFtp)
{
	net_close_socket(&pFtp->datafd);
	net_close_socket(&pFtp->portfd);

	if (recv_confirm(pFtp) == 0) 
	{
		return HI_FTP_SUCCESS;
	}

	return HI_FTP_UL_SEND_DATA_CHN_ERR;	
}
int hi_ftp_connect(LPHI_FTP_INFO pFtp)
{
	char szPort[8] = {0};
	
	if (pFtp == NULL || pFtp->serverIp== NULL || pFtp->serverPort <= 0)
	{
#if DEBUG
		printf("hi_ftp_connect parameter error\n");
#endif
		return HI_FTP_CONN_PARAM_ERR;
	}

	pFtp->bLogin = 0;
	sprintf(szPort, "%d", pFtp->serverPort);
	if ((pFtp->ctrlfd = net_tcp_noblock_connect(NULL, NULL, pFtp->serverIp, szPort,
				FTP_CONNECT_TIMEOUT)) < 0)
	{
#if DEBUG
		printf("hi_ftp_connect connect server %s %s failed %s\n",
				pFtp->serverIp, szPort, strerror(errno));
#endif
		return HI_FTP_CONN_SVR_ERR;
	}
	
	if ((net_select(&pFtp->ctrlfd, 0x1, 0x1, FTP_RECV_TIMEOUT) != 
		(pFtp->ctrlfd | 0x10000)) || recv_answer(pFtp, NULL) != 220)
	{
#if DEBUG
		printf("hi_ftp_connect recv welcome message error\n");
#endif
		net_close_socket(&pFtp->ctrlfd);
		return HI_FTP_CONN_SVR_ERR;
	}

	return pFtp->ctrlfd;	
}

int hi_ftp_login(LPHI_FTP_INFO pFtp)
{
	int retVal = 0;
	
	if (pFtp == NULL || pFtp->ctrlfd == 0)
	{
#if DEBUG
		printf("hi_ftp_login parameter error\n");
#endif
		return HI_FTP_LOGIN_PARAM_ERR;
	}

	memset(pFtp->cmd_buffer, 0, CMD_BUF_SIZE+1);

	if (strlen(pFtp->userName) <= 0)
		strcpy(pFtp->userName, "anonymous");
	snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "USER %s", pFtp->userName);

	retVal = send_commands(pFtp);
	if (retVal == 230) //return ok
		return 0;

	if (retVal != 331) //username error
	{
#if DEBUG
		printf("hi_ftp_login username %s error retVal %d \n", pFtp->userName, retVal);
#endif
		return HI_FTP_LOGIN_USER_ERR;
	}
	
	memset(pFtp->cmd_buffer, 0, CMD_BUF_SIZE+1);
	if (strlen(pFtp->passWord) <= 0)
		strcpy(pFtp->passWord, "anonymous");	
	snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "PASS %s", pFtp->passWord);
	
	if ((retVal = send_commands(pFtp)) != 230) //password error
	{
#if DEBUG
		printf("hi_ftp_login password %s error retVal %d\n", pFtp->passWord);
#endif
		return HI_FTP_LOGIN_PSW_ERR;		
	}
	return 0;
}

int hi_ftp_disconnect(LPHI_FTP_INFO pFtp)
{
	net_close_socket(&pFtp->ctrlfd);
	net_close_socket(&pFtp->datafd);
	net_close_socket(&pFtp->portfd);
	return 0;
}

//=======================================================
#if 0
typedef struct _FTP_FRAGMENT_LIST
{
	struct	list_head attach_list;		/*用于维护链表*/
	fragment_s	stFrag;	
}FTP_FRAGMENT_LIST;

int hi_ftp_start_trans_disk_data(LPHI_FTP_INFO pFtp, int bDownLoad, char* fileName,
							long long *pStartPos, record_read_s *pRecRead)
{
	int answer = 0;

	if (bDownLoad)
	{
		/*ftp 下载写硬盘文件不支持断点续传
		*/
	}
       snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "%s %s",
			bDownLoad ? "RETR" : "STOR", fileName);
	
	if ((answer = send_commands(pFtp)) != 150 &&
			answer != 125 && answer != 226)
    {
#ifdef DEBUG	
		printf("Send RETR|STOR Cmd error %d\n", answer);
#endif
		return -1;
	}
	return 0;
}

int hi_ftp_create_disk_data_channel(LPHI_FTP_INFO pFtp, int bDownLoad, 
				long long *pPos, char *fileName, record_read_s *pRecRead)
{
	char szPort[8] = {0};
	
	unsigned long nIp = 0;
	struct timeval  timeOut;
	
	if ((pFtp->bPasv = get_data_trans_type(pFtp)) == 0)
	{
		if ((pFtp->datafd = hi_ftp_create_port(pFtp)) <= 0)
		{
			hi_close_socket(&pFtp->datafd);
			hi_close_socket(&pFtp->portfd);			
			return -1;
		}			
		nIp = hi_get_sock_ip(pFtp->ctrlfd);
		
		sprintf(pFtp->cmd_buffer, "PORT %d,%d,%d,%d,%d,%d\r\n", 
			   (nIp & 0xff000000) >> 24, (nIp & 0xff0000)   >> 16,
			   (nIp & 0xff00)     >> 8,  (nIp & 0xff),
			   (pFtp->dataSvrPort & 0xff00) >> 8,
			   (pFtp->dataSvrPort & 0xff));

		if (send_commands(pFtp) != 200)
		{
			hi_close_socket(&pFtp->datafd);
			hi_close_socket(&pFtp->portfd);				
			return -1;
		}	
	}
	else
	{
		sprintf(szPort, "%d", pFtp->dataPort);
		if ((pFtp->datafd = hi_tcp_noblock_connect(NULL, NULL, 
			pFtp->serverIp, szPort, FTP_CONNECT_TIMEOUT)) <= 0)
		{
#ifdef DEBUG
			printf("connect ftp data channel %s:%s error %s\n", 
					pFtp->serverIp, szPort, strerror(errno));
#endif
			hi_close_socket(&pFtp->datafd);
			hi_close_socket(&pFtp->portfd);
			return -1;
		}
	}

	if (hi_ftp_start_trans_disk_data(pFtp, bDownLoad, fileName, pPos, pRecRead) != 0)
	{
		hi_close_socket(&pFtp->datafd);
		hi_close_socket(&pFtp->portfd);	
		return -1;
	}

	if (pFtp->bPasv == 0)
	{
		if ((pFtp->portfd | 0x10000) == 
				hi_select(&pFtp->portfd, 0x1, 0x1, FTP_CONNECT_TIMEOUT))
		{
			timeOut.tv_sec = 3;
			timeOut.tv_usec= 0;
			if ((pFtp->datafd = hi_tcp_noblock_accept(pFtp->portfd, 
					NULL, NULL, &timeOut)) < 0)
			{
				hi_close_socket(&pFtp->datafd);
				hi_close_socket(&pFtp->portfd);
				return -1;
			}
		}
		else
		{
			hi_close_socket(&pFtp->datafd);
			hi_close_socket(&pFtp->portfd);
			return -1;
		}
	}
	
	return pFtp->datafd;
}

long long hi_ftp_query_disk_file_size(FTP_FRAGMENT_LIST *pListHdr)
{
	 struct list_head	*pos = NULL, *q = NULL;
	 FTP_FRAGMENT_LIST *pTmpAtta = NULL;
	 long long u64TotalSize = 0;
	//循环stRecFrag链表 取大小
	 list_for_each_safe(pos, q, &(pListHdr->attach_list))
	{
		pTmpAtta = list_entry(pos, FTP_FRAGMENT_LIST, attach_list);
		if (NULL != pTmpAtta)
		{	
			u64TotalSize += pTmpAtta->stFrag.fragment_info.data_end - 
						    pTmpAtta->stFrag.fragment_info.data_start;
		}
	}	
	return u64TotalSize;
}

int hi_ftp_query_disk_cb(fragment_s *fragment_info, void *user_arg, int pic_flag)
{
	if (user_arg == NULL)
		return -1;

	FTP_FRAGMENT_LIST *pRecFrag = NULL;
	FTP_FRAGMENT_LIST  *pListHdr  = (FTP_FRAGMENT_LIST*)(user_arg);
	
	if ((pRecFrag = (FTP_FRAGMENT_LIST *)malloc(sizeof(FTP_FRAGMENT_LIST))) != NULL)
	{
		memcpy(&pRecFrag->stFrag, fragment_info, sizeof(fragment_s));
		list_add_tail(&(pRecFrag->attach_list), &pListHdr->attach_list);
	}	
	return 0;
}

int hi_ftp_upload_disk_search(int nCh,  int nType, int startTime, int stopTime, void *arg)
{
	record_query_s stQuery;
	memset(&stQuery, 0, sizeof(record_query_s));
	stQuery.ch 		= nCh;
	stQuery.pic_flag	= nType;
	stQuery.typed		= 0xFFFF;
	stQuery.user_arg	= arg;
	stQuery.start_time = startTime;
	stQuery.end_time  = stopTime;
	stQuery.fragment_infoCB = hi_ftp_query_disk_cb;
	return sdk_record_query(&stQuery);
}

int hi_ftp_upload_disk_file(LPHI_FTP_INFO pFtp, int nCh, int nType, int startTime, 
						int stopTime, char *dst)
{
	long long fileSize = 0, start_pos = 0;
	int answer = 0,readn  = 0,ret    = 0, nErr = 0;
	char *pDir = NULL;
	FTP_FRAGMENT_LIST stFragList;
	record_read_s		*pRecRead = NULL;
	
	memset(&stFragList, 0, sizeof(FTP_FRAGMENT_LIST));
	if ((pDir= remote_getcwd(pFtp)) != NULL)
		strncpy(pFtp->root_dir, pDir, CMD_DIR_SIZE);
			
	snprintf(pFtp->cmd_buffer, CMD_BUF_SIZE, "TYPE I");
	if (!(answer = send_commands(pFtp)))	
	{
#ifdef DEBUG
		printf("TYPE I command retVal %d error\n", answer);
#endif	
		nErr = HI_FTP_UL_TYPE_ERR;
		goto FTP_UPLOAD_DISK_FILE_ERR;
	}

	hi_ftp_upload_disk_search(nCh, nType, startTime, stopTime, &stFragList);
	if ((fileSize = hi_ftp_query_disk_file_size(&stFragList)) <= 0)
	{
#ifdef DEBUG
		printf("hi_ftp_upload_disk_file get file size error %lld\n", fileSize);
#endif
		nErr = HI_FTP_UL_FILE_SIZE_ERR;
		goto FTP_UPLOAD_DISK_FILE_ERR;
	}

		
	FTP_FRAGMENT_LIST *pRecFrag = list_entry(stFragList.attach_list.next, FTP_FRAGMENT_LIST, attach_list); 
	if ((pRecRead = sdk_record_open(&(pRecFrag->stFrag.fragment_point))) == NULL)
	{
		nErr = HI_FTP_UL_OPEN_FILE_ERR;
		goto FTP_UPLOAD_DISK_FILE_ERR;
	}
		
	if (remote_chdir(pFtp, pFtp->root_dir) || hi_mkdir(pFtp, dst))
	{
		nErr = HI_FTP_UL_CHK_REMOTE_DIR_ERR;
		goto FTP_UPLOAD_DISK_FILE_ERR;	
	}
	
	if (hi_ftp_create_disk_data_channel(pFtp, 0, &start_pos, dst, pRecRead) <= 0)
	{
		nErr = HI_FTP_UL_CREATE_DATA_CHN_ERR;
		goto FTP_UPLOAD_DISK_FILE_ERR;
	}
	
    	while (start_pos < fileSize) 
	{
		readn = ((CMD_BUF_SIZE+1) > (fileSize - start_pos)) ?
				(fileSize - start_pos) : (CMD_BUF_SIZE+1);
		if ((readn = sdk_record_read(pRecRead, pFtp->cmd_buffer, readn)) <= 0)
	    		break;
		if (hi_tcp_noblock_send(pFtp->datafd, pFtp->cmd_buffer,
				readn, NULL, FTP_SEND_TIMEOUT) != readn)
	    	{
#ifdef DEBUG	
			printf("hi_ftp_upload_file error %s\n", strerror(errno));
#endif	    
	    		break;
		}
		start_pos += readn;
    }

	hi_close_socket(&pFtp->datafd);
	hi_close_socket(&pFtp->portfd);

	if (start_pos >= fileSize && (ret = recv_confirm(pFtp)) == 0) 
	{
#ifdef DEBUG
		printf("upload file to %s ok upload size %lld total size %lld\n",
				dst, start_pos, fileSize);
#endif	
		nErr = HI_FTP_SUCCESS;
	}

#ifdef DEBUG
	printf("upload file to %s err upload size %lld total size %lld\n",
				dst, start_pos, fileSize);
#endif
	
	nErr = HI_FTP_UL_SEND_DATA_CHN_ERR;
FTP_UPLOAD_DISK_FILE_ERR:
	if (pRecRead)
	{
		sdk_record_close(pRecRead);
		pRecRead = NULL;
	}
	struct list_head	*pos = NULL, *q = NULL;
	FTP_FRAGMENT_LIST *pTmpAtta = NULL;
	//循环attachment链表 释放空间
	list_for_each_safe(pos, q, &stFragList.attach_list)
	{
		pTmpAtta = list_entry(pos, FTP_FRAGMENT_LIST, attach_list);
		if (NULL != pTmpAtta)
		{
			list_del(&(pTmpAtta->attach_list));//从链表中删除
			free(pTmpAtta);//释放内存
			pTmpAtta = NULL; //清空指针，以免产生野指针
		}  
	}	
	return nErr;
}
#endif
