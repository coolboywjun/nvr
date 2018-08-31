

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/watchdog.h>
#include <sys/vfs.h>
#include <sys/ioctl.h>
#include <time.h>
#include <signal.h>
#include <stdarg.h>

#include <syscfg.h>

#include "proc.h"

#define	USE_EXT_WDT
#define	WDT_LOG_PATH	"/mnt/log/wdt.log"

volatile int quit = 0;
volatile int useWdt = 1;



static int write_log(const char *fmt, ...)
{
	char logstr[4096];
	char timestr[64];
	va_list args;
	int len;
	FILE *fp;
	struct stat st;
	const char *open_flag;
	struct tm time_info;
	time_t cur_time;

	cur_time =  time(NULL);
	localtime_r(&cur_time, &time_info);
	strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S==> ", &time_info);
	
	va_start(args, fmt);
	len = vsnprintf(logstr, sizeof(logstr)-1, fmt, args);
	logstr[sizeof(logstr)-1] = '\0';
	va_end(args);
	if(logstr[len-1] != '\n')
		strcat(logstr, "\n");
		
	if(stat(WDT_LOG_PATH, &st) < 0)
	{
		if(errno == ENOENT)
			open_flag = "w";
		else
			return -1;
	}
	else
	{
		if(st.st_size >= 0x100000)	// 1M
			open_flag = "w";
		else
			open_flag = "a";
	}
	fp = fopen(WDT_LOG_PATH, open_flag);
	if(fp == NULL)
		return -1;
	fputs(timestr, fp);
	fputs(logstr, fp);
	fclose(fp);

	return 0;
}

#ifdef USE_EXT_WDT


//==============================================================================
#define  DEVICE_ADDR   0x56    //i2c device address of mcu
#define  TIMEOUT       1       // 1 * 10ms
#define  RETRES        3       //

#define I2C_MCUREQ    0x00
#define I2C_CCD_NUM   0x1d
#define I2C_VPFE_NUM  0x1e
#define I2C_OTHER_NUM 0x1f
#define I2C_INFO      0x20
#define I2C_DSPREQ    0x13
#define I2C_SDSTAT    0x12
#define I2C_CURNUML   0x0e
#define I2C_SECOND    0x07



int fd;
struct i2c_msg msgs[2];
struct i2c_rdwr_ioctl_data i2cmcu_data =
{
	.msgs = msgs,
};
//==================================================================================
//

int i2cmcu_open_device(void)
{
	fd = open("/dev/i2c-1",O_RDWR);
	if(fd<0) {
		printf("can not open device!\n");
		return -1;
	}
	ioctl(fd,I2C_TIMEOUT,TIMEOUT);
	ioctl(fd,I2C_RETRIES,RETRES);
	return 0;
}
//////////////////////////////////////////////////
//
void i2cmcu_close_device()
{
	close(fd);
	fd = -1;
}

//==================================================================================
//write more byte
//len < 128
int i2cmcu_write_byte(
			unsigned short reg_addr,
			unsigned char  *data,
			int len
			)
{
	int i;
	unsigned char buff[128];
	buff[0] = 0xaa;  buff[1] = 0x55;
	buff[2]=(unsigned char) reg_addr;
	for(i=0;i<len;i++) buff[i+3]=data[i];

	i2cmcu_data.nmsgs = 1;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[0].flags = 0; //0,for write
	i2cmcu_data.msgs[0].len = len + 4;
	i2cmcu_data.msgs[0].buf = buff;
	//printf("write:reg=0x%02X, len=0x%02X",reg_addr,len);
	return ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data);
}
//==================================================================================
//read more byte
int i2cmcu_read_byte(
			unsigned short reg_addr,
			unsigned char *data,
			int len
			)
{
	unsigned char buff;
	buff = (unsigned char)reg_addr;
	i2cmcu_data.nmsgs = 2;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[0].flags = 0; //0,for write
	i2cmcu_data.msgs[0].len = 1;
	i2cmcu_data.msgs[0].buf = &buff;

	i2cmcu_data.msgs[1].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[1].flags = I2C_M_RD; //read
	i2cmcu_data.msgs[1].len = len;
	i2cmcu_data.msgs[1].buf = data; 
	
	return ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data);
}
//=================================================================================
//read bytes
int i2c_read(unsigned char *data, int len)
{
	i2cmcu_data.nmsgs = 1;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR;
	i2cmcu_data.msgs[0].flags = I2C_M_RD;
	i2cmcu_data.msgs[0].len = len;
	i2cmcu_data.msgs[0].buf = data;
	return ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data);
}
//=================================================================================
//
unsigned int key(unsigned int random)
{
	unsigned char ret[5];
	unsigned char buff[10];
	unsigned int tmp;
	//buff[0]=(unsigned char) 0x01;
	//for(i=0;i<len;i++) buff[i+1]=data[i];
	buff[0] = 0xaa;
	buff[1] = 0x55;
	buff[2] = 0x01;
	buff[3] = (random >> 24)&0xff;
	buff[4] = (random >> 16)&0xff;
	buff[5] = (random >> 8)&0xff;
	buff[6] = (random &0xff);

	i2cmcu_data.nmsgs = 2;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[0].flags = 0; //0,for write
	i2cmcu_data.msgs[0].len = 8;
	i2cmcu_data.msgs[0].buf = buff;
	
	i2cmcu_data.msgs[1].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[1].flags = I2C_M_RD; //read
	i2cmcu_data.msgs[1].len = 4;
	i2cmcu_data.msgs[1].buf = ret;
	
	if(ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data) < 0) return 0xffffffff;
	
	tmp = (ret[0]<<24) + (ret[1]<<16) + (ret[2]<<8) + ret[3];
	return tmp;
}
//=================================================================================
unsigned char const random_tab[256] =
{
0xED,0xD9,0x9E,0xE5,0xBD,0x8D,0x87,0xC5,0x35,0x59,0xC2,0x7D,0x19,0xD1,0x34,0xD6,
0x3C,0x1B,0x7E,0xC3,0x56,0x52,0xF8,0xE3,0xCE,0xF1,0xF5,0x8E,0x08,0x37,0x88,0xEC,
0xEA,0xDF,0xE8,0x02,0x40,0x53,0x69,0x2A,0x0F,0x55,0xCD,0x28,0xCB,0xFE,0x91,0x1D,
0xB5,0xA7,0xB9,0x7B,0xC7,0xD8,0x63,0xCF,0xB0,0x9C,0x65,0x6E,0x05,0x6F,0x47,0xAA,
0x5A,0xB1,0xA5,0x51,0xD4,0xB3,0xFF,0x0D,0x0A,0xBE,0xC9,0x1F,0x68,0xFA,0x18,0x3F,
0xFC,0x38,0x17,0xF4,0xE9,0x13,0x84,0xD2,0x8F,0x14,0x41,0x79,0xC6,0xAF,0x78,0xEF,
0x83,0xCA,0xE0,0x4A,0x46,0xF6,0x94,0x66,0x5C,0x50,0x62,0x10,0x5F,0x2E,0x2C,0x6C,
0x8C,0xA1,0x70,0x86,0x4C,0x81,0x42,0x45,0x03,0x06,0x29,0xB8,0xA6,0x54,0x8B,0x0C,
0x48,0x23,0x3D,0x7A,0xD0,0x98,0x16,0xEE,0xE6,0x5E,0x0E,0xB2,0x74,0xC1,0x2F,0x25,
0x0B,0x39,0xF0,0x7C,0x7F,0x27,0xC4,0xE4,0x92,0x49,0x5B,0xBC,0xBF,0xE7,0xF9,0xD5,
0x9A,0x73,0xDB,0x61,0xA4,0xAD,0x85,0xDA,0x1A,0x1C,0xAE,0xA2,0xFB,0xB6,0x1E,0x6A,
0x99,0x3B,0x43,0x89,0x76,0x71,0xEB,0xF7,0xA8,0x95,0x44,0x01,0xB7,0xAC,0xC8,0xF3,
0xDC,0xA9,0x04,0x36,0xB4,0x9D,0xDE,0x9F,0x4D,0x4E,0x90,0xA3,0x22,0x32,0x2B,0x93,
0x00,0x20,0xAB,0xBA,0x58,0x77,0xFD,0x24,0xE1,0x2D,0x64,0x30,0x12,0x3E,0xE2,0x80,
0x72,0x96,0x75,0x09,0x60,0x6D,0x3A,0x82,0x11,0x4B,0x31,0x26,0xDD,0x97,0x6B,0x9B,
0xBB,0x5D,0xC0,0xD3,0x15,0xA0,0x57,0x21,0x33,0x8A,0x4F,0xF2,0xCC,0x67,0xD7,0x07
};

#define RAND_V1  0x43
#define RAND_V2  0xC7
#define RAND_V3  0x38
#define RAND_V4  0x5B

unsigned int  create_key(unsigned int random)
{
	unsigned int ret;
	unsigned char v[4];
	unsigned char retcode[4];

	v[0] = (random>>24)&0xff;
	v[1] = (random>>16)&0xff;
	v[2] = (random>>8)&0xff;
	v[3] = (random&0xff);

	v[0] = random_tab[v[0]];
	v[1] = random_tab[v[1]];
	v[2] = random_tab[v[2]];
	v[3] = random_tab[v[3]];

	retcode[0] = v[0] + v[1] + RAND_V1;
	retcode[1] = v[1] + v[2] + RAND_V2;
	retcode[2] = v[2] + v[3] + RAND_V3;
	retcode[3] = v[0] + v[3] + RAND_V4;

	ret = (retcode[0]<<24) + (retcode[1]<<16) + (retcode[2]<<8) + retcode[3]; 
	return ret;
}
////////////////////////////////////////////////////////

///////////////////////////////////////////
int Check_Key()
{	
	unsigned int key_val, key_val1, tmp;
	srand((int)time(NULL));
	
	tmp = (unsigned int)rand();
	//DBG("rand:%x\r\n", tmp);
	key_val = key(tmp);
	key_val1 = create_key(tmp);
	if(key_val != key_val1) return -1;

	return 0;
}
//=================================================================================
//
int clr_wdt(void)
{
	unsigned char buff[10];
	//buff[0]=0x02;
	buff[0] = 0xaa;
	buff[1] = 0x55;
	buff[2] = 0x02;
	i2cmcu_data.nmsgs = 1;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[0].flags = 0; //0,for write
	i2cmcu_data.msgs[0].len = 4;
	i2cmcu_data.msgs[0].buf = buff;
	return ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data);
}
//=================================================================================
//
int reset(void)
{
	unsigned char buff[10];

	//buff[0]=0x03;
	buff[0] = 0xaa;
	buff[1] = 0x55;
	buff[2] = 0x03;

	i2cmcu_data.nmsgs = 1;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[0].flags = 0; //0,for write
	i2cmcu_data.msgs[0].len = 4;
	i2cmcu_data.msgs[0].buf = buff;
	return ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data);
}
//=================================================================================
//
int poweroff(void)
{
	unsigned char buff[10];

	//buff[0]=0x03;
	buff[0] = 0xaa;
	buff[1] = 0x55;
	buff[2] = 0x08;

	i2cmcu_data.nmsgs = 1;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[0].flags = 0; //0,for write
	i2cmcu_data.msgs[0].len = 4;
	i2cmcu_data.msgs[0].buf = buff;
	return ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data);
}
//==================================================================================
int inc_count(void)
{
	unsigned char buff[10];
	//buff[0]=0x03;
	buff[0] = 0xaa;
	buff[1] = 0x55;
	buff[2] = 0x04;

	i2cmcu_data.nmsgs = 1;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[0].flags = 0; //0,for write
	i2cmcu_data.msgs[0].len = 4;
	i2cmcu_data.msgs[0].buf = buff;
	return ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data);
}
//==================================================================================
int read_count(void)
{
	unsigned char ret[5];
	unsigned char buff[10];
	unsigned int tmp;
	//buff[0]=(unsigned char) 0x01;
	//for(i=0;i<len;i++) buff[i+1]=data[i];
	buff[0] = 0xaa;
	buff[1] = 0x55;
	buff[2] = 0x06;


	i2cmcu_data.nmsgs = 2;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[0].flags = 0; //0,for write
	i2cmcu_data.msgs[0].len = 4;
	i2cmcu_data.msgs[0].buf = buff;
	
	i2cmcu_data.msgs[1].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[1].flags = I2C_M_RD; //read
	i2cmcu_data.msgs[1].len = 1;
	i2cmcu_data.msgs[1].buf = ret;
	
	if(ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data) < 0) return -1;
	
	tmp = ret[0];
	return tmp;
}
//==================================================================================
int reset_count(void)
{
	unsigned char buff[10];
	//buff[0]=0x03;
	buff[0] = 0xaa;
	buff[1] = 0x55;
	buff[2] = 0x05;

	i2cmcu_data.nmsgs = 1;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[0].flags = 0; //0,for write
	i2cmcu_data.msgs[0].len = 4;
	i2cmcu_data.msgs[0].buf = buff;
	return ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data);

}
//==================================================================================
int shutdown_wdt(void)
{
	unsigned char buff[10];
	//buff[0]=0x03;
	buff[0] = 0xaa;
	buff[1] = 0x55;
	buff[2] = 0x07;

	i2cmcu_data.nmsgs = 1;
	i2cmcu_data.msgs[0].addr = DEVICE_ADDR; // device addr
	i2cmcu_data.msgs[0].flags = 0; //0,for write
	i2cmcu_data.msgs[0].len = 4;
	i2cmcu_data.msgs[0].buf = buff;
	return ioctl(fd,I2C_RDWR,(unsigned long)&i2cmcu_data);
}

int check_process(int processCnt, char *processName[])
{
	int i, sta = 0;
	int checkProcPID[32];
	int checkProcNum = 0;
	int resetCountFlag = 0;
	int tryNum;

	for(tryNum = 0; tryNum < 10; tryNum++)
	{
		if(Check_Key() < 0)
		{
			printf("Check key failed.\n");
			continue;
		}
		break;
	}
	if(tryNum == 10)
		return -1;
		
	clr_wdt();
	sleep(3);		// 延时一段时间，保证其它进行已经开启
	for(i=0; i<processCnt; i++)
	{
		int pid;
		if(processName[i])
		{
			pid = 0;
			tryNum = 0;
			while(tryNum++ < 5)
			{
				printf("Get %s pid\n", processName[i]);
				pid = GetProcPid(processName[i]);
				if(pid != -1)
				{
					printf("check pid:%d\r\n", pid);
					checkProcPID[checkProcNum++] = pid;
					break;
				}else
				{
					//reset();
					sleep(1);
					continue;
				}
			}
			if(pid < 0)
			{
				printf("check program %s failed.\n", processName[i]);
				return -1;
			}
		}
	}

	clr_wdt();

	// 等待5S后再判断进程状态
	// 避免第一次判断进程状态是OK的,
	// 然后再狗计数清除了,不能自动进入默认文件系统
	// 如果在系统启动时间和这个5S内手动断电重启3次后,
	// 系统重启后会自动进入到默认文件系统
	sleep(5);
	
	while(!quit)
	{
		for(i=0; i<checkProcNum; i++)
		{
			if(!CheckProcStat(checkProcPID[i]))
			{
				write_log("Check program %s failed\n", processName[i]);
				sta = -1;
			}
		}

		if(sta) 
		{
			//system("reboot &");
			//sleep(30);
			//reset();
			//exit(-1);
			return -1;
		}	
		if(resetCountFlag == 0)
		{
			resetCountFlag = 1;
			reset_count();
		}
		
		sleep(10);
		clr_wdt();
	}

	return 0;
}

#else

int dog_fd = -1;

static int open_watchdog()
{
	dog_fd = open("/dev/watchdog", O_WRONLY);
	if(dog_fd < 0)
	{
		printf("Open watdog failed\n");
		return -1;
	}
	return dog_fd;
}

static void close_watchdog()
{
	if(dog_fd >= 0)
		close(dog_fd);
}

static int clear_watchdog()
{
	if(dog_fd >= 0)
	{
		char buf[2] = {'V', 0};
		write(dog_fd, &buf, sizeof(buf));
	}
	return -1;
}

int check_process(int processCnt, char *processName[])
{
	int i, sta = 0;
	int checkProcPID[32];
	int checkProcNum = 0;
	int tryNum = 0;
	
	for(i=0; i<processCnt; i++)
	{
		int pid;
		if(processName[i])
		{
			pid = 0;
			tryNum = 0;
			while(tryNum++ < 5)
			{
				printf("Get %s pid\n", processName[i]);
				pid = GetProcPid(processName[i]);
				if(pid != -1)
				{
					printf("check pid:%d\r\n", pid);
					checkProcPID[checkProcNum++] = pid;
					break;
				}else
				{
					//reset();
					sleep(1);
					continue;
				}
			}
			if(pid < 0)
			{
				printf("check program %s failed.\n", processName[i]);
				return -1;
			}
		}
	}
	
	open_watchdog();
	while(!quit)
	{
		for(i=0; i<checkProcNum; i++)
		{
			if(!CheckProcStat(checkProcPID[i]))
				sta = -1;
		}

		if(sta) 
		{
			printf("watchdog reboot ################\n");
			break;
		}
		
		sleep(8);
		clear_watchdog();
	}
	close_watchdog();
	
	if(sta)
		return -1;
		
	return 0;
}

#endif

int watch_process(int processCnt, char *processName[])
{
	int i;
	int checkProcPID[32];
	int checkProcNum = 0;
	int tryNum;
		
	sleep(3);		// 延时一段时间，保证其它进行已经开启
	for(i=0; i<processCnt; i++)
	{
		int pid;
		if(processName[i])
		{
			pid = 0;
			tryNum = 0;
			while(tryNum++ < 5)
			{
				printf("Get %s pid\n", processName[i]);
				pid = GetProcPid(processName[i]);
				if(pid != -1)
				{
					printf("check pid:%d\r\n", pid);
					checkProcPID[checkProcNum++] = pid;
					break;
				}else
				{
					//reset();
					sleep(1);
					continue;
				}
			}
			if(pid < 0)
			{
				printf("check program %s failed.\n", processName[i]);
				return -1;
			}
		}
	}

	while(!quit)
	{
		for(i=0; i<checkProcNum; i++)
		{
			if(!CheckProcStat(checkProcPID[i]))
			{
				char cmdline[128];
				write_log("Watch program %s failed.\n", processName[i]);
				system("rm -rf /tmp/core*");
				sprintf(cmdline, "./%s &", processName[i]);
				system(cmdline);
				sleep(3);
				checkProcPID[i] = GetProcPid(processName[i]);
			}
		}
		
		sleep(10);
	}

	return 0;
}

static void sigint(int signo)
{
	quit = 1;
}

void usage(const char *appname)
{
#ifdef USE_EXT_WDT
	printf("usage: %s {check|inc|reset|poweroff|restart|clr|shutdown|read|default}\n", appname);
	printf("\t %s -p process0 process1 process2...\n", appname);
	printf("\t check: check the key.\n");
	printf("\t inc: increase the wdt count.\n");
	printf("\t reset: reset the wdt count.\n");
	printf("\t poweroff: power off the system.\n");
	printf("\t restart: restart the system.\n");
	printf("\t shutdown: shutdown wdt function.\n");
	printf("\t read: print the wdt count.\n");
	printf("\t default: force the system reboot and then run default filesystem.\n");
	printf("\t %s -p process0 process1 process2...: if the process dump, we restart system\n", appname);
	printf("\t %s -w process0 process1 process2...: if the process dump, we restart the application\n", appname);
#else
	printf("usage: \n");
	printf("\t %s -p process0 process1 process2...\n", appname);
#endif
}

int main(int argc, char *argv[])
{
	DEV_SystemCfg *pCfg;
	pCfg = get_system_cfg();
	useWdt = !pCfg->u8NoWdt;
	if(argc < 2)
	{
		usage(argv[0]);
		exit(1);
	}
#ifdef USE_EXT_WDT
	if(i2cmcu_open_device() < 0) 
	{
		printf("Failed to open i2c device.\n");
		exit(1);
	}
	
	if(strcmp(argv[1], "check") == 0)
	{
		if(useWdt)
		{
			if(Check_Key() < 0)
				exit(1);
		}
		else
		{
			shutdown_wdt();
			printf("The Rules.make don't enable watchdog, shutdown watchdog!!!\n");
		}
	}
	else if(strcmp(argv[1], "inc") == 0)
	{
		inc_count();
	}
	else if(strcmp(argv[1], "reset") == 0)
	{
		reset_count();
	}
	else if(strcmp(argv[1], "poweroff") == 0)
	{
		poweroff();
	}
	else if(strcmp(argv[1], "restart") == 0)
	{
		reset();
	}
	else if(strcmp(argv[1], "clr") == 0)
	{
		if(useWdt)
		{
			clr_wdt();
			printf("Clear the wdt.\n");
		}
		else
		{
			shutdown_wdt();
			printf("The syscfg don't enable watchdog, shutdown watchdog!!!\n");
		}
	}
	else if(strcmp(argv[1], "shutdown") == 0)
	{
		shutdown_wdt();
	}
	else if(strcmp(argv[1], "read") == 0)
	{
		int count;
		if((count = read_count()) < 0)
			exit(1);
		printf("count: %d\n", count);
	}
	else if(strcmp(argv[1], "default") == 0)
	{
		int i;

		// 强制重启运行默认出厂程序
		for(i = 0; i<5; i++)
			inc_count();
		system("reboot &");
	}
	else 
#endif	
	if(strcmp(argv[1], "-p") == 0)
	{
		if(argc < 3)
		{
			printf("Please input the process name.\n");
			exit(1);
		}
		signal(SIGINT, sigint);
		if(useWdt)
		{
			if(check_process(argc-2, &argv[2]) < 0)
			{
				int trynum = 0;
				write_log("Check process failed, reboot system.\n");
				system("reboot &");
				while(trynum++ < 30) sleep(1);
				// 强制重启机器
				while(1)
				{
					reset();
					sleep(1);
				}
				exit(1);
			}
		}
		else
		{
			shutdown_wdt();
			printf("The syscfg don't enable watchdog, shutdown watchdog!!!\n");
		}
	}
	else if(strcmp(argv[1], "-w") == 0)
	{
		if(argc < 3)
		{
			printf("Please input the process name.\n");
			exit(1);
		}
		signal(SIGINT, sigint);
		if(useWdt)
		{
			if(watch_process(argc-2, &argv[2]) < 0)
			{
				int trynum = 0;
				write_log("Watch process failed, reboot system.\n");
				system("reboot &");
				while(trynum++ < 30) sleep(1);
				// 强制重启机器
				while(1)
				{
					reset();
					sleep(1);
				}
				exit(1);
			}	
		}
		else
		{
			shutdown_wdt();
			printf("The syscfg don't enable watchdog, shutdown watchdog!!!\n");
		}
	}
	else
	{
		usage(argv[0]);
		exit(1);
	}
#ifdef USE_EXT_WDT
	i2cmcu_close_device();
#endif	
	return 0;
}


