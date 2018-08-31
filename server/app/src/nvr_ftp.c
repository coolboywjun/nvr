#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/un.h>
#include <sys/time.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <net_api.h>
//GLIB
#include <glib.h>
#include <glib-unix.h>
#include <glib/gprintf.h>

#include "nvr_ftp.h"
#include "nvr_net_common.h"
#include "nvr_param_proc.h"

#undef	G_LOG_DOMAIN
#define	G_LOG_DOMAIN "ftp"

#ifdef __DEBUG_FTP__
#define f_deg(fmt, args...)	g_debug("[%s %d] "fmt, __func__, __LINE__, ##args)
#else
#define f_deg(fmt, args...)
#endif


#define FLAG	  O_RDWR|O_CREAT
#define MODE	  0777

 /*TCP Socket连接 ：返回socket*/
static int nvr_ftp_connect_server(char *nvr_ftp_server, unsigned int port)  
{  
	struct hostent *hp;
	struct sockaddr_in server;
	int    sok = 0;
	/* 初始化socket */
	sok = socket(AF_INET, SOCK_STREAM, 0);
	if(sok < 0)
	{	
		j_warning("create socket is error:%d\n", sok);
		return -1;
	}
	
	hp = gethostbyname(nvr_ftp_server);
	memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	
	//连接到服务器控制端
	if(0 != connect(sok,(struct sockaddr *)&server, sizeof(server)))
	{
		j_warning("connect is error\n");
		close(sok);
		return -1;
	}
	
	return sok;	
}

/*获得数据端口
返回值：成功返回数据端口，失败返回-1*/
static int nvr_ftp_get_data_port(char *str)
{
	char buf[256] = {0};
	char *p       = NULL;
	int data_port = 0;
	int cnt       = 0;
	
	if(NULL == str)
		return -1;
	//"227 Entering passive mode (h1,h2,h3,h4,p1,p2)"	
	memcpy(buf, str, strlen(str));
	//获取每个分割
	p = strtok(buf,"(,)");
	while(p)
	{
		cnt++;
		if(6 == cnt)
			data_port = atoi(p)*256;
		else if(7 == cnt)
			data_port += atoi(p);
			
		p = strtok(NULL,"(,)");//下个域
	}		
	
	return data_port;
}

//读写
static int nvr_ftp_safe_write(int fd, const void *buf, size_t count)
{
	ssize_t n;

	do {
		n = write(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}
static int nvr_ftp_safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

/*获取文件大小

static int nvr_ftp_get_file_size(int ctrl_sock, char *pfilename, char* buf, int buf_len)
{
	//获取文件大小
	memset(buf, 0, buf_len);
	sprintf(buf,"SIZE %s\r\n", pfilename);
	nvr_ftp_safe_write(ctrl_sock, buf, strlen(buf));
	//客户端接收服务器的响应码
	memset(buf, 0, buf_len);
	nvr_ftp_safe_read(ctrl_sock, buf, buf_len);
	f_deg("%s", buf);
	return -1;
}
*/
/*上传数据:
   data_sock:套接字
   filename:文件名
   filepath:文件路径(不带"/")
 返回值：成功返回发送大小，失败返回-1.
*/
static int nvr_ftp_stor_data(int data_sock, char *filename, char *filepath)
{
	int r_ret  = -1;
	int w_ret  = -1;
	int fd	   = -1;
	int size   = 0;
	int filesize = -1;
	int sendsize = -1;
	char send_buf[1024*8] = {0};
	char filenametmp[512] = {0};//全路径
	
	struct stat fileinfo_s = {0};
	
	//检查文件是否存在
	strcat(filenametmp, filepath);
	strcat(filenametmp, "/");
	strcat(filenametmp, filename);
	f_deg("%s\n", filenametmp);
	if(0 != access(filenametmp, 0))
	{	
		j_warning("file is not exit!");
		return -1;
	}
	
	//open file
	fd = open(filenametmp, FLAG, MODE);
	if(-1 != fd)
	{
		//read
		if(0 > stat(filenametmp, &fileinfo_s))
			goto _FAILED;
			
		filesize = fileinfo_s.st_size;
		sendsize = sizeof(send_buf);

		f_deg("filesize:%d ...", filesize);
		while(1)
		{
			memset(send_buf, 0, sizeof(send_buf));
			if((0 <= (filesize - sizeof(send_buf))) || (0 < filesize))
			{
				if(0 == filesize)
					break;
					
				if(0 > (filesize - sizeof(send_buf)))
					sendsize = filesize;
					
				r_ret = nvr_ftp_safe_read(fd, send_buf, sendsize);
				//f_deg("r_ret:%d ...", r_ret);
				if(0 > r_ret)
				{
					j_warning("read failed!");
					goto _FAILED;
				}
				else
				{
					size += r_ret;
					filesize -= r_ret;
					w_ret = nvr_ftp_safe_write(data_sock, send_buf, r_ret);
					if(-1 == w_ret)
					{	
						j_warning("write failed!");
						goto _FAILED;
					}
				}
			}
			else
				break;	
		}
	}
	else
	{
		j_warning("open failed!");
		goto _FAILED;
	}
	
	close(fd);
	return size;
	
_FAILED:
	close(fd);
	return -1;
}

//文件传送控制
static void nvr_ftp_transfer_ctrl(int sok, char *buf, int buf_len)
{
	nvr_ftp_safe_write(sok, buf, strlen(buf));
	memset(buf, 0, buf_len);
	nvr_ftp_safe_read(sok, buf, buf_len);

	return;
}

//获取服务器响应值
static int nvr_ftp_get_response_val(const char *str)
{
	char buf[256] = {0};
	char *p = NULL;
	
	if(NULL == str)
		return -1;
	
	memcpy(buf, str, strlen(str));
	buf[4] = '\0';
	p = buf;
	
	return atoi(p);
}

//创建目录
static int nvr_ftp_mkdir(int sok, const char *str)
{
	char dir_buf[512] = {0};
	char snd_buf[128] = {0};
	char *p = NULL;
	int  ret = 0;
	
	if(NULL == str)
		return -1;
	
	memcpy(dir_buf, str, strlen(str));
	//获取每个分割
	p = strtok(dir_buf,"/");
	while(p)
	{
		//命令"CWD path\r\n":服务器是windows靛:/盘符:/目录
		f_deg("p:%s\n", p);
		memset(snd_buf, 0, sizeof(snd_buf));
		sprintf(snd_buf,"CWD %s\r\n",p);
		nvr_ftp_transfer_ctrl(sok, snd_buf, sizeof(snd_buf));
		ret = nvr_ftp_get_response_val(snd_buf);
		if(250 != ret)
		{
			//创建目录
			memset(snd_buf, 0, sizeof(snd_buf));
			sprintf(snd_buf,"MKD %s\r\n",p);
			nvr_ftp_transfer_ctrl(sok, snd_buf, sizeof(snd_buf));
			ret = nvr_ftp_get_response_val(snd_buf);
			if(257 != ret)
				return -1;
			
			//跳转到该目录下
			memset(snd_buf, 0, sizeof(snd_buf));
			sprintf(snd_buf,"CWD %s\r\n",p);
			nvr_ftp_transfer_ctrl(sok, snd_buf, sizeof(snd_buf));
			ret = nvr_ftp_get_response_val(snd_buf);
			if(250 != ret)
				return -1;
		}

		p = strtok(NULL,"/");//下个域
	}
	
	return 0;
}


//文件上传
int nvt_ftp_upload(NVR_FTP_CFG_S* pftpcfg ,char* pfilename, char* pfilepath)
{
	int    ctrl_sock = -1;
	int    data_sock = -1;
	int    data_port = 0;
	int    ret       = 0;
	
	char   buf[2048] = {0};
	int    buf_len = sizeof(buf);
	
	//链接ftp服务器控制端口
	ctrl_sock = nvr_ftp_connect_server(pftpcfg->szFtpSvr, pftpcfg->u16FtpPort);
	while(0 > ctrl_sock)
	{
		sleep(1);
		ctrl_sock = nvr_ftp_connect_server(pftpcfg->szFtpSvr, pftpcfg->u16FtpPort);
	}
	//客户端接收服务器端信息
	nvr_ftp_safe_read(ctrl_sock, buf, buf_len);
	f_deg("<--%s", buf);
	
	//发送命令"USER username\r\n"
	memset(buf, 0, buf_len);
	sprintf(buf,"USER %s\r\n",pftpcfg->szFtpUsr);
	nvr_ftp_transfer_ctrl(ctrl_sock, buf, buf_len);
	f_deg("<--%s", buf);
	
	//命令"PASS password\r\n"
	memset(buf, 0, buf_len);
	sprintf(buf,"PASS %s\r\n",pftpcfg->szFtpPsw);
	nvr_ftp_transfer_ctrl(ctrl_sock, buf, buf_len);
	f_deg("<--%s", buf);
	
	//命令"CWD path\r\n":服务器是windows靛:/盘符:/目录
	memset(buf, 0, buf_len);
	if('/' == pftpcfg->szFtpPath[0])
		sprintf(buf,"CWD %s\r\n",pftpcfg->szFtpPath+1);
	else
		sprintf(buf,"CWD %s\r\n",pftpcfg->szFtpPath);
	nvr_ftp_transfer_ctrl(ctrl_sock, buf, buf_len);
	f_deg("<--%s", buf);
	if(250 != nvr_ftp_get_response_val(buf))
	{	
		if(0 > nvr_ftp_mkdir(ctrl_sock,pftpcfg->szFtpPath))
			j_warning("CWD cmd failed!!!");
	}
	
	//命令"PASV\r\n"-->被动模式
	memset(buf, 0, buf_len);
	sprintf(buf,"PASV\r\n");
	nvr_ftp_transfer_ctrl(ctrl_sock, buf, buf_len);
	f_deg("<--%s", buf);
	//获取数据端口
	data_port = nvr_ftp_get_data_port(buf);
	if(0 > data_port)
		goto _OUT;
	f_deg("data_port%d\n", data_port);
	
	//链接ftp服务器数据端口
	data_sock = nvr_ftp_connect_server(pftpcfg->szFtpSvr, data_port);
	while(0 > data_sock)
	{
		sleep(1);
		data_sock = nvr_ftp_connect_server(pftpcfg->szFtpSvr, data_port);
	}
	
	//上传文件STOR
	memset(buf, 0, buf_len);
	f_deg("%s\n", pfilename);
	sprintf(buf,"STOR %s\r\n", pfilename);
	nvr_ftp_transfer_ctrl(ctrl_sock, buf, buf_len);
	f_deg("%s", buf);
	ret = nvr_ftp_stor_data(data_sock, pfilename, pfilepath);
	//关闭数据端
	close(data_sock);
	memset(buf, 0, buf_len);	
	nvr_ftp_safe_read(ctrl_sock, buf, buf_len);
	ret = nvr_ftp_get_response_val(buf);
	f_deg("%s", buf);
	
_OUT:
	//命令"QUIT\r\n"
	memset(buf, 0, buf_len);
	sprintf(buf,"QUIT\r\n");
	nvr_ftp_transfer_ctrl(ctrl_sock, buf, buf_len);
	f_deg("%s", buf);
	//客户端关闭连接
	close(ctrl_sock);

	return ret;
}

void nvr_ftp_upload_ctrl(char* data[])
{
	int ret = 0;
	NVR_FTP_CFG_S ftpCfg;
	char *pfilename = data[0];
	char *pfilepath = data[1];
	
	prctl(PR_SET_NAME, "NVR_FTP");
	pthread_detach(pthread_self());	
	
	if(NULL == pfilename || NULL == pfilepath)
		return;
	
	memset(&ftpCfg, 0, sizeof(NVR_FTP_CFG_S));

	nvr_get_ftp_cfg(-1, &ftpCfg);
	
	//f_deg("pfilename: %s", pfilename);
	//f_deg("pfilepath: %s", pfilepath);	
	//f_deg("%s, %d, %s", ftpCfg.szFtpSvr, ftpCfg.u16FtpPort, ftpCfg.szFtpPath);
	while(226 != ret)
	{
		ret = nvt_ftp_upload(&ftpCfg, pfilename, pfilepath);
		f_deg("nvr_ftp_upload_ctrl:ret=%d\n", ret);
	}

	
}

/*
int main(int argc, char* argv[])
{
	NVR_FTP_CFG_S ftpcfg;
	
	memset(&ftpcfg, 0, sizeof(NVR_FTP_CFG_S));
	
	strcpy(ftpcfg.szFtpSvr, "192.168.73.1");
	strcpy(ftpcfg.szFtpUsr, "lihanwei");
	strcpy(ftpcfg.szFtpPsw, "080629");
	strcpy(ftpcfg.szFtpPath, "/test");
	ftpcfg.u16FtpPort = 21;
	
	if((NULL != argv[1]) && (NULL != argv[2]))
		nvt_ftp_upload(&ftpcfg, argv[1], argv[2]);
	else	
		nvt_ftp_upload(&ftpcfg, "Serv-U.rar", "/mnt/hgfs/share/test");	
	
	return 0;
}
*/

