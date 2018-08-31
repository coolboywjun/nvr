#include <stdio.h>  
#include <string.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <errno.h>  
#include <unistd.h>  
#include <sys/time.h> 

//SSL

//GLIB
#include <glib.h>
#include <glib-unix.h>
#include <glib/gprintf.h>

#include <net_api.h>
#include "nvr_smtp.h"
#include "nvr_net_common.h"
#include "nvr_param_proc.h"

#undef	G_LOG_DOMAIN
#define	G_LOG_DOMAIN "smtp"

#ifdef __DEBUG_SMTP__
#define s_deg(fmt, args...)	g_debug("[%s %d] "fmt, __func__, __LINE__, ##args)
#else
#define s_deg(fmt, args...)
#endif


#define NVR_SMTP_PORT_STR "25"

 /*TCP Socket连接 ：返回socket*/
static int nvr_stmp_connect_server(char *nvr_smtp_server, unsigned int port)  
{  
	char portStr[32] = {'0'};

	if(0 >=  port)
		strncpy(portStr, NVR_SMTP_PORT_STR, sizeof(NVR_SMTP_PORT_STR));
	else
		sprintf(portStr, "%d", port);
	
    return net_tcp_noblock_connect(NULL, NULL, nvr_smtp_server, portStr, 1000);  
}

/*发送接收控制
	成功返回0，失败返回-1*/
static int nvr_stmp_mail_control(int socket, char* buf, int bufsize, char* pflag)
{
	if(0 > net_tcp_noblock_send(socket, buf, strlen(buf), NULL, 1000))
	{
		j_warning("(%s:%s:%d)%s send failed!!",__FILE__, __FUNCTION__, __LINE__, pflag);
		return -1;
	}

	memset(buf, 0, bufsize);
	if(0 > net_tcp_noblock_recv(socket, buf, bufsize, bufsize, 1000))
	{
		j_warning("(%s:%s:%d)%s recv failed!!",__FILE__, __FUNCTION__, __LINE__, pflag);
		return -1;
	}
	
	return 0;
}

/*发送邮件
	pSmtpCfg:SMTP信息配置结构体指针
	pSubJect：发送邮件标题
	pBody：发送正文
*/
void __nvr_stmp_send_mail(NVR_SMTP_CFG_S* pSmtpCfg, char* pSubJect, char* pBody)
{
	int socket = -1;
	//int connetCnt = 0;
    char buf[1500] 		= {0};
    
   	char *pbase64 = NULL;
	
	if(NULL == pSubJect)
		pSubJect = "JXJ Device Alarm Warning";
	
	if(NULL == pBody)
		pBody = "Hey, Mr. or ms.:\r\n\
				This is a letter about JXJ monitoring\
				equipment to send out alarm messages!\r\n\
				Please check the following!";

	//链接smtp服务器
	s_deg("pSmtpCfg->szMailHub:%s", pSmtpCfg->szMailHub);
	s_deg("pSmtpCfg->u16MailPort:%d", pSmtpCfg->u16MailPort);
	socket = nvr_stmp_connect_server(pSmtpCfg->szMailHub, pSmtpCfg->u16MailPort);
	while(0 > socket)
	{
		/*if(100 < connetCnt++)
		{
			j_warning("nvr connect smtp server failed !");
			return;
		}*/
		sleep(3);
		socket = nvr_stmp_connect_server(pSmtpCfg->szMailHub, pSmtpCfg->u16MailPort);	
	}
	
	if(0 > net_tcp_noblock_recv(socket, buf, sizeof(buf), sizeof(buf), 1000))
	{
		j_warning("(%s:%s:%d) recv failed!!",__FILE__, __FUNCTION__, __LINE__ );
		goto _OUT;
	}	
	s_deg("<---%s", buf);
	
	//HELO
	memset(buf, 0, sizeof(buf));
	sprintf(buf,"HELO %s\r\n",pSmtpCfg->szMailHub);
	s_deg("--->%s", buf);
	if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "HELO"))
		goto _OUT;
	s_deg("<---%s", buf);
	
	//AUTH LOGIN
	memset(buf, 0, sizeof(buf));
	sprintf(buf,"AUTH LOGIN\r\n");
	s_deg("--->%s", buf);
	if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "AUTH LOGIN"))
		goto _OUT;
	s_deg("<---%s", buf);
	
	//USERNAME
	pbase64 = g_base64_encode((unsigned char*)pSmtpCfg->szMailUsr, strlen(pSmtpCfg->szMailUsr));
	memset(buf, 0, sizeof(buf));
	sprintf(buf,"%s\r\n", pbase64);
	g_free(pbase64);
	s_deg("--->%s", buf);
	if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "STMP USER"))
		goto _OUT;
	s_deg("<---%s", buf);
	
	//PASSWORD
	pbase64 = g_base64_encode((unsigned char*)pSmtpCfg->szMailPsw, strlen(pSmtpCfg->szMailPsw));
	memset(buf, 0, sizeof(buf));
	sprintf(buf,"%s\r\n", pbase64);
	g_free(pbase64);
	s_deg("--->%s", buf);
	if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "STMP PASSWORD"))
		goto _OUT;
	s_deg("<---%s", buf);
	
	//MAIL FROM
	memset(buf, 0, sizeof(buf));
	sprintf(buf,"MAIL FROM:<%s>\r\n", pSmtpCfg->szMailAddr);
	s_deg("--->%s", buf);
	if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "MAIL FROM"))
		goto _OUT;
	s_deg("<---%s", buf);
	
	//RCPT TO 1
	if(0 < strlen(pSmtpCfg->szMailRctp1))
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf,"RCPT TO:<%s>\r\n", pSmtpCfg->szMailRctp1);
		s_deg("--->%s", buf);
		if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "RCPT TO"))
			goto _OUT;
		s_deg("<---%s", buf);
	}
	else
	{
		j_warning("(%s:%s:%d) szMailRctp1 is null!!",__FILE__, __FUNCTION__, __LINE__ );
		goto _OUT;
	}
	
	//RCPT CC 1
	if(0 < strlen(pSmtpCfg->szMailRctp2))
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf,"RCPT CC:<%s>\r\n", pSmtpCfg->szMailRctp2);
		s_deg("--->%s", buf);
		if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "RCPT CC 1"))
			goto _OUT;
		s_deg("<---%s", buf);
	}
	
	//RCPT CC 2
	if(0 < strlen(pSmtpCfg->szMailRctp3))
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf,"RCPT CC:<%s>\r\n", pSmtpCfg->szMailRctp3);
		s_deg("--->%s", buf);
		if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "RCPT CC 2"))
			goto _OUT;
		s_deg("<---%s", buf);
	}
	
	//DATA
	memset(buf, 0, sizeof(buf));
	sprintf(buf,"DATA\r\n");
	s_deg("--->%s", buf);
	if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "DATA"))
			goto _OUT;
	s_deg("<---%s", buf);
	
	//MAIL BODY
	memset(buf, 0, sizeof(buf));
	if((0 < strlen(pSmtpCfg->szMailRctp2)) && (0 < strlen(pSmtpCfg->szMailRctp3)))
	{
		sprintf(buf, "FROM:<%s>\r\nTO:<%s>\r\nCC:<%s>\r\nCC:<%s>\r\nSUBJECT:%s\r\n\r\n%s\r\n.\r\n",
								 	pSmtpCfg->szMailAddr,
									pSmtpCfg->szMailRctp1,
									pSmtpCfg->szMailRctp2,
									pSmtpCfg->szMailRctp3,
									pSubJect,
									pBody);
	}
	else if((0 == strlen(pSmtpCfg->szMailRctp2)) && (0 < strlen(pSmtpCfg->szMailRctp3)))
	{
		sprintf(buf, "FROM:<%s>\r\nTO:<%s>\r\nCC:<%s>\r\nSUBJECT:%s\r\n\r\n%s\r\n.\r\n",
								 	pSmtpCfg->szMailAddr,
									pSmtpCfg->szMailRctp1,
									pSmtpCfg->szMailRctp3,
									pSubJect,
									pBody);
	}
	else if((0 < strlen(pSmtpCfg->szMailRctp2)) && (0 == strlen(pSmtpCfg->szMailRctp3)))
	{
		sprintf(buf, "FROM:<%s>\r\nTO:<%s>\r\nCC:<%s>\r\nSUBJECT:%s\r\n\r\n%s\r\n.\r\n",
								 	pSmtpCfg->szMailAddr,
									pSmtpCfg->szMailRctp1,
									pSmtpCfg->szMailRctp2,
									pSubJect,
									pBody);
	}
	else
	{
		sprintf(buf, "FROM:<%s>\r\nTO:<%s>\r\nSUBJECT:%s\r\n\r\n%s\r\n.\r\n",
								 	pSmtpCfg->szMailAddr,
									pSmtpCfg->szMailRctp1,
									pSubJect,
									pBody);
	}

	s_deg("--->%s", buf);
	if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "MAIL BODY"))
			goto _OUT;
	s_deg("<---%s", buf);
	
	//NOOP
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "NOOP\r\n");
	s_deg("--->%s", buf);
	if(0 > nvr_stmp_mail_control(socket, buf, sizeof(buf), "NOOP"))
			goto _OUT;
	s_deg("<---%s", buf);
	
	//QUIT
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "QUIT\r\n");
	s_deg("--->%s", buf);
	if(0 > net_tcp_noblock_send(socket, buf, strlen(buf), NULL, 1000))
	{
		j_warning("(%s:%s:%d)QUIT send failed!!",__FILE__, __FUNCTION__, __LINE__);
		goto _OUT;
	}
	memset(buf, 0, sizeof(buf));
	net_tcp_noblock_recv(socket, buf, sizeof(buf), sizeof(buf), 1000);
	s_deg("<---%s", buf);

_OUT:
	net_close_socket(&socket);
}

//smtp线程处理函数
static gpointer nvr_smtp_control(gpointer data)
{
	char **text = (char **)data;
	char* pSubJect = text[0];//主题 
	char* pBody	   = text[1];//内容

	NVR_SMTP_CFG_S smtpCfg;
	
	memset(&smtpCfg, 0, sizeof(NVR_SMTP_CFG_S));
	nvr_get_smtp_cfg(-1, &smtpCfg);

	j_debug("%s, %d, %s", smtpCfg.szMailHub, smtpCfg.u16MailPort, smtpCfg.szMailAddr);	

	//发送邮件
	__nvr_stmp_send_mail(&smtpCfg, pSubJect, pBody);
	g_free(pSubJect);
	g_free(pBody);
	g_free(text);
	
	return NULL;
}

void nvr_stmp_send_mail(const char* pSubJect, const char* pBody)
{
	GThread *thread;
	gpointer *text = g_new(gpointer, 2);
	
	text[0] = g_strdup(pSubJect);
	text[1] = g_strdup(pBody);
	//j_debug("%s, %s", text[0], text[1]);
	thread = g_thread_new("smtp", nvr_smtp_control, (gpointer)text);
	g_thread_unref(thread);
}

/*
int main()
{
	//SMTP
	NVR_SMTP_CFG_S smtpcfg;
	memset(&smtpcfg, 0, sizeof(NVR_SMTP_CFG_S));
	
	//smtp服务器
    strcpy(smtpcfg.szMailHub, "smtp.szjxj.net");
    //端口号
	smtpcfg.u16MailPort = 25;
	//发送邮箱
	strcpy(smtpcfg.szMailAddr, "lihanwei@szjxj.net");
	//接收邮箱
	strcpy(smtpcfg.szMailRctp1, "*********@qq.com");
	//抄送邮箱
	strcpy(smtpcfg.szMailRctp2, "*********@qq.com");
	strcpy(smtpcfg.szMailRctp3, "*********@qq.com");
	
	//账户与密码
	strcpy(smtpcfg.szMailUsr, "lihanwei@szjxj.net");
	strcpy(smtpcfg.szMailPsw, "********");
	
	//发送邮件
	nvr_stmp_send_mail(&smtpcfg, NULL, NULL);
	
	return 0;
}
*/




