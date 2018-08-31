
#undef	G_LOG_DOMAIN
#define	G_LOG_DOMAIN "comm"

#ifndef _SAMPLE_SOURCE 

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "nvr_comm.h"
#include "nvr_param_proc.h"
#include "nvr_net_common.h"
#include "nvr_ipc_api.h"
//#include "ipc_gpio_interface.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
void nvr_comm_capture_task(void * argv);

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define NVR_COMM_TASK_NUM 2

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
NVR_COMM_S gComm[NVR_MAX_SUPPORT_COMM_NO] = {{0}};
NVR_COMM_TASK_S gCommTask[NVR_COMM_TASK_NUM] = {{0}, {0}};

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
 // 不支持76800波特率,相应的位置填为B0
static const int speed_arr[] = {B50, B75, B110, B150, B300, B600, B1200,  B2400, B4800,
				     B9600, B19200, B38400, B57600, B0, B115200};
#define COMM_SPEED_NUM	(sizeof(speed_arr)/sizeof(speed_arr[0]))

/*----------------------------------------------*
 * 函数实现                                     *
 *----------------------------------------------*/
int nvr_comm_set_opt(int fd, COMM_PARA_S *pStComm, int nVTime, int nVMin)
{
	struct termios opt_new;
	if (tcgetattr (fd, &opt_new) != 0)
	{
		//perror("tcsetattr fd1");
		return COMM_GET_OPT_ERR;
	}
	
	tcflush(fd,  TCIOFLUSH);	//刷新输入输出缓冲
	cfsetispeed(&opt_new, speed_arr[pStComm->baudrate%COMM_SPEED_NUM]);	//这里分别设置                 
	cfsetospeed(&opt_new, speed_arr[pStComm->baudrate%COMM_SPEED_NUM]); 

	opt_new.c_cflag |= CLOCAL | CREAD;
	opt_new.c_cflag &= ~CSIZE;	//这是设置c_cflag选项不按位数据位掩码
	switch (pStComm->databit) /*设置数据位数*/                     
	{
	case 0:
		opt_new.c_cflag |= CS5;					//设置c_cflag选项数据位为5位
		break;
	case 1:
		opt_new.c_cflag |= CS6;					//设置c_cflag选项数据位为6位
		break;
	case 2:
		opt_new.c_cflag |= CS7;					//设置c_cflag选项数据位为7位
		break;
	case 3:
		opt_new.c_cflag |= CS8;					//设置c_cflag选项数据位为8位
		break;
	default:
	    opt_new.c_cflag |= CS8;			//设置c_cflag选项数据位为8位
	    break;
    }

	switch (pStComm->checkType)								//设置奇偶校验，c_cflag和c_iflag有效
    {   
	case 0:	//'n'								//无校验  当然都不选
		opt_new.c_cflag &= ~PARENB;				/* Clear parity enable */
		opt_new.c_iflag &= ~INPCK;				/* Enable parity checking */
		break;
	case 1:		   								//奇校验 其中PARENB校验位有效；PARODD奇校验,INPCK检查校验
		opt_new.c_cflag |= (PARODD | PARENB);	/* 设置为奇效验*/ 
		opt_new.c_iflag |= INPCK;				/* Disnable parity checking */
		break;
		
	case 2:									//偶校验，奇校验不选就是偶校验了
		opt_new.c_cflag |= PARENB;				/* Enable parity */
		opt_new.c_cflag &= ~PARODD;				/* 转换为偶效验*/  
		opt_new.c_iflag |= INPCK;				/* Disnable parity checking */
		break;
	default:
		opt_new.c_cflag &= ~PARENB;				/* Clear parity enable */
		opt_new.c_iflag &= ~INPCK;				/* Enable parity checking */
		break;
    }	

	/* 设置停止位*/   
	switch (pStComm->stopbit)			//这是设置停止位数，影响的标志是c_cflag
    {
	case 0:
		opt_new.c_cflag &= ~CSTOPB;//不指明表示一位停止位
		break;
	case 1:
		opt_new.c_cflag |= CSTOPB;//指明CSTOPB表示两位，只有两种可能
		break;
	default:
		opt_new.c_cflag &= ~CSTOPB;	//不指明表示一位停止位
		break;
    }
	//这个地方是用来设置控制字符和超时参数的，一般默认即可。
	//稍微要注意的是c_cc数组的VSTART 和 VSTOP 元素被设定成DC1 和 DC3,
	//ASCII 标准的XON和XOFF字符。所以如果在传输这两个字符的时候就传不过去，
	//这时需要把软件流控制屏蔽 options.c_iflag &= ~(IXON | IXOFF | IXANY);
    opt_new.c_iflag &= ~(IGNBRK | IGNCR | INLCR | ICRNL | IUCLC | IXANY | IXON | IXOFF | INPCK | ISTRIP);
    opt_new.c_iflag |= (BRKINT | IGNPAR);
    opt_new.c_oflag &= ~OPOST;
    opt_new.c_lflag &= ~(XCASE|ECHONL|NOFLSH);
    opt_new.c_lflag &= ~(ICANON | ISIG | ECHO);	
//    opt_new.c_lflag |= CRTSCTS;
	
	opt_new.c_cc[VTIME] = nVTime;
	opt_new.c_cc[VMIN]  = nVMin;

	tcflush (fd, TCIFLUSH);
	if ((tcsetattr (fd, TCSANOW, &opt_new)) != 0)
	{
		//nvr_log_write(LOG_ERR, "comm_set_opt set error\n");
		j_warning("comm_set_opt set error!");
		return COMM_SET_OPT_ERR;
	}	

	return 0;
}

int nvr_comm_open(int nCommNo, char *szDev, COMM_PARA_S *pStCommPara, int nVTime, int nVMin)
{
	NVR_COMM_S *pComm = NULL;
	int	nRet = 0;

	if (nCommNo < 0 || nCommNo > NVR_MAX_SUPPORT_COMM_NO || gComm[nCommNo].init != 1)
	{
		//nvr_log_write(LOG_ERR, "comm open parameter error\n");
		j_warning("comm open parameter error!");
		return COMM_PARAM_ERR;
	}

	pComm = &gComm[nCommNo];

	if (memcmp(&pComm->stCommPara, pStCommPara, sizeof(COMM_PARA_S)) == 0 && 
		pComm->fd > 0)
		return 0;

	nvr_comm_close(nCommNo);

	pthread_mutex_lock(&pComm->mutexLock);
	if ((pComm->fd = open(szDev, O_RDWR | O_NOCTTY | O_NDELAY)) <= 0)
	{
		//nvr_log_write(LOG_ERR, "open tty %s failed\n", szDev);
		j_warning("open tty %s failed!", szDev);
		pthread_mutex_unlock(&pComm->mutexLock);
		return COMM_OPEN_ERR;
	}	

	memcpy(&pComm->stCommPara, pStCommPara, sizeof(COMM_PARA_S));
	nRet = nvr_comm_set_opt(pComm->fd, &pComm->stCommPara, nVTime, nVMin);
	pthread_mutex_unlock(&pComm->mutexLock);

	return nRet;
}	

void nvr_comm_close(unsigned char nCommNo)
{
	NVR_COMM_S *pComm = NULL;
	if (nCommNo > NVR_MAX_SUPPORT_COMM_NO || gComm[nCommNo].init != 1)
	{
		//nvr_log_write(LOG_ERR, "comm close parameter error\n");
		j_warning("comm close parameter error!");
		return ;
	}	
	pComm = &gComm[nCommNo];
	pthread_mutex_lock(&pComm->mutexLock);
	
	if (pComm->fd > 0)
		close(pComm->fd);
	pComm->fd = 0;

	pthread_mutex_unlock(&pComm->mutexLock);
	return ;
}

int nvr_comm_write_data(unsigned char nCommNo, char *data, int nDataLen)
{
	NVR_COMM_S *pComm = NULL;
	int nWrite      = 0;
	int nAllWriteLen= nDataLen;
	int nWriteTimes = 0;
	int nOffset     = 0;
	
	if (nCommNo > NVR_MAX_SUPPORT_COMM_NO || data == NULL || nDataLen <= 0)
	{
		//nvr_log_write(LOG_ERR, "comm write data parameter error\n");
		j_warning("comm write data parameter error!");
		return -1;
	}

	pComm = &gComm[nCommNo];
	if (pComm->fd <= 0)
	{
		//nvr_log_write(LOG_ERR, "comm write data err comm %d isn't open\n", nCommNo);
		j_warning("comm write data err comm %d isn't open!", nCommNo);
		return COMM_PARAM_ERR;
	}

	while (nWriteTimes++ < 5 && nDataLen > 0)
	{
		nWrite = write(pComm->fd, data+nOffset, nDataLen-nOffset);
		if (nWrite >= (nDataLen - nOffset))
			return nAllWriteLen;
		else if (nWrite > 0)
		{
			nDataLen    -= nWrite;
			nOffset += nWrite;
		}
	}
	fsync(pComm->fd);
	if (nDataLen != nAllWriteLen)
		return nDataLen;
	return COMM_WR_DATA_ERR;	
}

/********************************************************
*从COMM口中读数据
*
*fd: COMM口号
*
*pBuf: 数据缓存
*
*nSize : 数据缓存长度
*
*nTimeOut:等待读取的最大时间(ms)
*
*nReadWaitTime:读前等待时间(ms)
*
*成功返回所读的数据长度 否则返回-1
*********************************************************/
int  nvr_comm_read_data(unsigned char nCommNo, char *pBuf, int nSize, int nTimeOut, int nReadWaitTime)
{
	int ret = 0;
	int total_read = 0;
	int total_len  = 0;
	NVR_COMM_S *pComm = NULL;
	
	if (nCommNo > NVR_MAX_SUPPORT_COMM_NO || pBuf == NULL || nSize <= 0)
	{
		//nvr_log_write(LOG_ERR, "comm read data parameter error\n");
		j_warning("comm read data parameter error!");
		return COMM_PARAM_ERR;
	}

	if ((pComm = &gComm[nCommNo]) <= 0)
	{
		//nvr_log_write(LOG_ERR, "comm read data comm %d isn't open\n", nCommNo);
		j_warning("comm read data comm %d isn't open!", nCommNo);
		return COMM_NOT_OPEN_ERR;
	}
	
	ret = net_select(&pComm->fd , 0x1, 0x1, nTimeOut);
	if (ret == (pComm->fd | 0x10000))
	{
		if (nReadWaitTime)
			usleep(nReadWaitTime*1000);
		ioctl(pComm->fd, FIONREAD, &total_len);
		total_read = 0;
		ret        = 0;
		
		while (total_read < total_len && nSize > total_read)
		{
 			ret = read(pComm->fd, pBuf + ret, nSize - total_read);
			if (ret <= 0)
			{
				break;
			}
			total_read += ret;	
		}	
		return total_read;
	}
	else if (ret == 0)
		return 0;
	return COMM_RD_DATA_ERR;
}

int nvr_comm_get_fd(int commNo)
{
	return gComm[commNo%NVR_MAX_SUPPORT_COMM_NO].fd;
}

int nvr_comm_update(int nPort, NVR_DEV_COMM_CFG_S *pStComm)
{
#ifdef TI_ARCH
	char szCommName[16] = {"/dev/tts/0"};
#else
	char szCommName[16] = {"/dev/ttyAMA1"};
#endif	
	if (nPort >=  NVR_MAX_SUPPORT_COMM_NO)
		return -1;
#ifdef TI_ARCH
	return nvr_comm_open(nPort, szCommName, (COMM_PARA_S *)&pStComm->u32BaudRate, 0, 0);
#else
	return nvr_comm_open(nPort, szCommName, (COMM_PARA_S *)&pStComm->u32BaudRate, 0, 0);
#endif
}

int nvr_comm_send_ptz_cmd(char *data, int nDataLen)
{
	return nvr_comm_send_data(COMM_485_NO, data, nDataLen);
}

int nvr_comm_send_data(int nPort, char *data, int nDataLen)
{
	int nRet = 0;
	NVR_MSG_S stMsg = {0};
	NVR_COMM_MSG_S *pStMsg = (NVR_COMM_MSG_S *)&stMsg.szMsg[0];

	if (gCommTask[0].s32MsgId < 0)
	{
		return NVR_FAILURE;
	}

	if (gCommTask[0].s32MsgNum > NVR_MAX_SYS_IPC_MSG) 
	{
		//nvr_log_write(LOG_WARNING, "comm too more ipc msg, Cleaning...\n");
		j_warning("comm too more ipc msg, Cleaning...");
		nvr_ipc_clear_msg(gCommTask[0].s32MsgId, &stMsg, sizeof(NVR_COMM_MSG_S));
		gCommTask[0].s32MsgNum = 0;
	}

	memset(pStMsg, 0, sizeof(NVR_COMM_MSG_S));
	
	stMsg.ulMsgType = NVR_COMM_MSG;
	pStMsg->u8Port = nPort;
	if (nDataLen > 254)
		pStMsg->u8Len  = 254;
	else
		pStMsg->u8Len  = nDataLen;
	memcpy(pStMsg->u8Data, data, pStMsg->u8Len);

	if ((nRet = nvr_ipc_msg_send(gCommTask[0].s32MsgId, &stMsg, sizeof(NVR_COMM_MSG_S), 0)) != 0)
	{
		//nvr_log_write(LOG_ERR, "comm send ipc msg err %d\n", nRet);
		j_warning("comm send ipc msg err %d !", nRet);
		return NVR_FAILURE;
	}	

	++gCommTask[0].s32MsgNum;
	return NVR_SUCCESS;		
}

int nvr_comm_start_capture(int nPort, CommCapCb capCb)
{
	gCommTask[1].capCb = capCb;
	gCommTask[1].u8Cap = 1;
	if (gCommTask[1].u8Run == 0)
	{
		gCommTask[1].u8Run = 1;
		if (0 != pthread_create(&gCommTask[1].thdId, NULL, (void *)&nvr_comm_capture_task, (void *)NULL))
			//nvr_log_write(LOG_ERR, "comm send task create err\n");	
			j_warning("comm send task create err!");
	}
	return 0;
}

int nvr_comm_stop_capture(int nPort)
{
	gCommTask[1].u8Cap = 0;
	return 0;
}

static int	nvr_comm_create_msg_list()
{
	if ((gCommTask[0].s32MsgId = nvr_ipc_msg_init(NVR_COMM_KEY)) < 0)
	{
		//nvr_log_write(LOG_ERR, "comm init msg err [%s %d]\n", __FILE__, __LINE__);
		j_warning("comm init msg err [%s %d]\n", __FILE__, __LINE__);
		return -1;
	}
	return gCommTask[0].s32MsgId;
}

void nvr_comm_send_task(void * argv)
{
	NVR_MSG_S stMsg;
	NVR_COMM_MSG_S *pCommMsg = (NVR_COMM_MSG_S *)(&stMsg.szMsg[0]);
	int nMsgLen = 0;
	
	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "CommSend");
	j_debug("comm send task active !");
	gCommTask[0].u8Exit = 0;
	gCommTask[0].u8Run = 1;
	
	while (gCommTask[0].u8Exit == 0)
	{
		if (gCommTask[0].s32MsgId < 0 && nvr_comm_create_msg_list() < 0)
		{
			sleep(5);
			continue;
		}

		if ((nMsgLen = nvr_ipc_msg_recv(gCommTask[0].s32MsgId, &stMsg, sizeof(NVR_COMM_MSG_S), 1)) < 1)
		{
			//nvr_log_write(LOG_ERR, "comm recv ipc msg err %d\n", nMsgLen);
			j_warning("comm recv ipc msg err %d !", nMsgLen);
			nvr_ipc_del_msq(gCommTask[0].s32MsgId);
			gCommTask[0].s32MsgId = -1;
			continue ;
		}

		if (stMsg.ulMsgType != NVR_COMM_MSG)
		{
			//nvr_log_write(LOG_ERR, "comm recv err msg type%d\n", stMsg.ulMsgType);
			j_warning("comm recv err msg type %ld\n", stMsg.ulMsgType);
			continue;
		}

		gCommTask[0].s32MsgNum -= 1;
		//nvr_log_write(LOG_DEBUG, "write data to comm %d data len %d\n", pCommMsg->u8Port, pCommMsg->u8Len);
		j_warning("write data to comm %d data len %d !", pCommMsg->u8Port, pCommMsg->u8Len);
#ifndef TI_ARCH
		if (pCommMsg->u8Port == COMM_485_NO)
		{
			;//ipc_gpio_set_rs485_status(1);
		}
#endif		
		nvr_comm_write_data(pCommMsg->u8Port, (char *)pCommMsg->u8Data, pCommMsg->u8Len);

		if (pCommMsg->u8Port == COMM_485_NO)
		{
//  2.日    期   : 2011年9月22日
//    作    者   : 梁锦明
//    修改内容   : 写485串口后增加40ms延迟		
			usleep(40000);
#ifndef TI_ARCH            
			;//c_gpio_set_rs485_status(0);
#endif
		}				
	}
	gCommTask[0].u8Run = 0;
	pthread_cond_signal(&gCommTask[0].condTask);
}

void nvr_comm_capture_task(void * argv)
{
	char szData[256] = {0};
	int nLen = 0;

	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, "CommCapture");
	//nvr_log_write(LOG_WARNING, "comm capture task active\n");
	j_warning("comm capture task active !");
	gCommTask[1].u8Run = 1;
	while (gCommTask[1].u8Exit == 0)
	{
		if (gCommTask[1].u8Cap != 0 && gCommTask[1].capCb != NULL)
		{
			if ((nLen = nvr_comm_read_data(COMM_485_NO, szData, 256, 100, 0)) >= 0)
			{
				if (nLen > 0)
					gCommTask[1].capCb(COMM_485_NO, szData, nLen);
				continue ;
			}
		}
		sleep(1);
	}
}

int nvr_comm_init()
{
	
#ifdef TI_ARCH
	char szCommName[16] = {"/dev/tts/0"};
#else
	char szCommName[16] = {"/dev/ttyAMA1"};
#endif
	int i = 0;
	NVR_DEV_COMM_CFG_S struCommCfg;

	for (i = 0; i < NVR_MAX_SUPPORT_COMM_NO; ++i)
	{
		memset(&gComm[i], 0, sizeof(NVR_COMM_S));
		memset(&struCommCfg, 0, sizeof(NVR_DEV_COMM_CFG_S));

		pthread_mutex_init(&gComm[i].mutexLock, NULL);
		gComm[i%NVR_MAX_SUPPORT_COMM_NO].init = 1;
		
		nvr_get_comm_cfg(i, &struCommCfg);
		
		nvr_comm_open(i, szCommName, (COMM_PARA_S *)&struCommCfg.u32BaudRate, 0, 0);
		
	}
	
	//init 
	for (i = 0; i < NVR_COMM_TASK_NUM; ++i)
	{
		memset(&gCommTask[i], 0, sizeof(NVR_COMM_TASK_S));
		pthread_mutex_init(&gCommTask[i].mutexTask, NULL);
		pthread_cond_init(&gCommTask[i].condTask, NULL);
		
		gCommTask[i].s32MsgId = -1;
	}
	
	if (0 != pthread_create(&gCommTask[0].thdId, NULL, (void *)&nvr_comm_send_task, (void *)NULL))
		j_warning("comm send task create err!");
	

	j_warning("comm init ok .......[%s %d]", __FILE__, __LINE__);
	return 0;
	
}

int nvr_comm_uninit()
{
	
	int i = 0;

	j_warning("comm uninit.");

	for (i = 0; i < NVR_COMM_TASK_NUM; ++i)
	{
		gCommTask[i].u8Exit = 1;
		if (gCommTask[i].u8Run == 1)
			nvr_pthread_wait(&gCommTask[i].mutexTask, &gCommTask[i].condTask, 1000);
		pthread_mutex_destroy(&gCommTask[i].mutexTask);
		pthread_cond_destroy(&gCommTask[i].condTask);			
	}
	
	for (i = 0; i < NVR_MAX_SUPPORT_COMM_NO; ++i)
	{
		if (gComm[i].init)
		{
			nvr_comm_close(i);
			gComm[i].init = 0;
			pthread_mutex_destroy(&gComm[i].mutexLock);
		}	
	}

	j_warning("comm uninit finish");
	return 0;
}

#endif
