#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mount.h>


#include <syscfg.h>

#define SYSCFG_FILE_PATH	"/dev/nandb"

static int g_filesize = 0;
static int g_offset1 = 0;
static int g_offset2 = 0;
static int g_kdoffset = 0;
static int g_initflag = 0;
static int g_initkdflag = 0;
static DEV_SystemCfg g_syscfg = {0};
static DEV_KanDianCfg g_kdcfg;



static DEV_SystemCfg g_defsyscfg =
{
	.Magic = SYSTEM_CFG_MAGIC,
	.u8SerialId = {"CNVR0000000000"},
	.u8DiNum = 0,
	.u8DoNum = 0,
	.u8ChnNum = 4,
	.u8Rs232Num = 0,
	.u8Rs485Num = 0,
	.u8NetCardNum = 1,
	.u8SupportAudio = 1,
	.u8SupportHDMI = 1,
	.u8supportVGA = 1,
	.u8SataNum = 1,
	.u16DevType = 0,
	.u8HwVer = "v1.1",
	.u8Mac = {0xe0, 0x3c, 0x5b, 0x11, 0x22, 0x33},
	.u8FactoryChecked = 0,
	.u16PlatformFlag = 0,
	.defaultIP = 0xC0A801F0,
	.defaultMask = 0xFFFFFF00,
	.defaultGateWay = 0xC0A80101,
	.platformServer = ~0UL,
	.platformClient = ~0UL,
	.supportLang = ~0UL,
	.u8DefLang = LANG_SIMPLIFIED_CHINESE,
	.u8SoftType = 0,
};

static int write_lock_file(int fd)
{
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
	if(fcntl(fd, F_SETLK, &lock) < 0)
	{
		printf("The update is busy.\n");
		return -1;
	}
	return 0;
}

static int read_lock_file(int fd)
{
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
	if(fcntl(fd, F_SETLK, &lock) < 0)
	{
		printf("The update is busy.\n");
		return -1;
	}
	return 0;
}

static int unlock_file(int fd)
{
	struct flock lock;
	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
	return fcntl(fd, F_SETLK, &lock);
}

static void calc_cfg_checksum(DEV_SystemCfg *pCfg)
{
	int i;
	unsigned long checksum = 0;
	unsigned long *pPtr = (unsigned long *)pCfg;
	
	pCfg->checksum = 0;
	for(i = 0; i < DEV_SYSTEM_CFG_LEN/4; i++)
	{
		checksum += pPtr[i];
	}
	pCfg->checksum = checksum;
}

static int check_cfg_checksum(DEV_SystemCfg *pCfg)
{
	int i;
	unsigned long checksum = 0;
	unsigned long *pPtr = (unsigned long *)pCfg;
	
	for(i = 0; i < DEV_SYSTEM_CFG_LEN/4 - 1; i++)
	{
		checksum += pPtr[i];
	}
	if(pCfg->checksum == checksum)
		return 1;
	return 0;
}

static void calc_kdcfg_checksum(DEV_KanDianCfg *pCfg)
{
	int i;
	unsigned long checksum = 0;
	unsigned long *pPtr = (unsigned long *)pCfg;
	
	pCfg->checksum = 0;
	for(i = 0; i < DEV_KANDIAN_CFG_LEN/4; i++)
	{
		checksum += pPtr[i];
	}
	pCfg->checksum = checksum;
}

static int check_kdcfg_checksum(DEV_KanDianCfg *pCfg)
{
	int i;
	unsigned long checksum = 0;
	unsigned long *pPtr = (unsigned long *)pCfg;
	
	for(i = 0; i < DEV_KANDIAN_CFG_LEN/4 - 1; i++)
	{
		checksum += pPtr[i];
	}
	if(pCfg->checksum == checksum)
		return 1;
	return 0;
}

static int open_file(const char *path)
{
	int fd;
	unsigned long size;
	//struct stat buf;
	
	fd = open(path, O_RDWR | O_DSYNC);
	if(fd < 0)
	{
		fprintf(stderr, "Open %s failed\n", path);
		return -1;
	}
	if(ioctl(fd, BLKGETSIZE, &size) < 0)
	{
		close(fd);
		return -1;
	}
	g_filesize = size * 512;
	printf("%s block size: %x\n", path, g_filesize);
	g_offset1 = g_filesize - 512 * 1024;
	g_offset1 = (g_offset1 + 256 * 1024 - 1) & (~(256 * 1024 - 1));
	g_offset2 = g_offset1 - 256 * 1024;
	g_kdoffset = g_offset1 + 64 * 1024;
	
	return fd;
}

static int write_cfg_file(int fd, DEV_SystemCfg *pCfg)
{
	// write first part
	calc_cfg_checksum(pCfg);
	lseek(fd, g_offset1, SEEK_SET);
	write(fd, pCfg, sizeof(DEV_SystemCfg));
	// write second part
	lseek(fd, g_offset2, SEEK_SET);
	write(fd, pCfg, sizeof(DEV_SystemCfg));
	return 0;
}

static int read_cfg_file(int fd, DEV_SystemCfg *pCfg)
{
	int ret1, ret2;
	DEV_SystemCfg cfg1, cfg2;
	DEV_SystemCfg *pUseCfg = NULL;
	memset(&cfg1, 0, sizeof(cfg1));
	memset(&cfg2, 0, sizeof(cfg2));

	lseek(fd, g_offset1, SEEK_SET);
	if((ret1= read(fd, &cfg1, DEV_SYSTEM_CFG_LEN)) != DEV_SYSTEM_CFG_LEN)
		printf("Read system cfg1 failed.\n");
	lseek(fd, g_offset2, SEEK_SET);
	if((ret2 = read(fd, &cfg2, DEV_SYSTEM_CFG_LEN)) != DEV_SYSTEM_CFG_LEN)
		printf("Read system cfg2 failed.\n");
	if(ret1 == DEV_SYSTEM_CFG_LEN && ret2 != DEV_SYSTEM_CFG_LEN)
	{
		if(check_cfg_checksum(&cfg1) && cfg1.Magic == SYSTEM_CFG_MAGIC)
			pUseCfg = &cfg1;
	}
	else if(ret1 != DEV_SYSTEM_CFG_LEN && ret2 == DEV_SYSTEM_CFG_LEN)
	{
		if(check_cfg_checksum(&cfg2) && cfg2.Magic == SYSTEM_CFG_MAGIC)
			pUseCfg = &cfg2;
	}
	else if(ret1 == DEV_SYSTEM_CFG_LEN && ret2 == DEV_SYSTEM_CFG_LEN)
	{
		if(check_cfg_checksum(&cfg1) && cfg1.Magic == SYSTEM_CFG_MAGIC)
			pUseCfg = &cfg1;
		else if(check_cfg_checksum(&cfg2) && cfg2.Magic == SYSTEM_CFG_MAGIC)
			pUseCfg = &cfg2;
		else
			pUseCfg = NULL;
	}
	else
		pUseCfg = NULL;
	if(pUseCfg == NULL)
	{
		memcpy(&cfg1, &g_defsyscfg, sizeof(cfg1));
		write_cfg_file(fd, &cfg1);
		pUseCfg = &g_defsyscfg;
	}
	if(pUseCfg->platformServer == 0)
		pUseCfg->platformServer = g_defsyscfg.platformServer;
	if(pUseCfg->platformClient == 0)
		pUseCfg->platformClient = g_defsyscfg.platformClient;
	if(pUseCfg->supportLang == 0)
		pUseCfg->supportLang = g_defsyscfg.supportLang;
	memcpy(pCfg, pUseCfg, DEV_SYSTEM_CFG_LEN);
	
	return 0;
}

static int read_kandian_cfg_file(int fd, DEV_KanDianCfg *pCfg)
{
	lseek(fd, g_kdoffset, SEEK_SET);
	read(fd, pCfg, DEV_KANDIAN_CFG_LEN);
	if(check_kdcfg_checksum(pCfg) == 0)
		memset(pCfg, 0, sizeof(DEV_KanDianCfg));
	return 0;
}

static int write_kandian_cfg_file(int fd, DEV_KanDianCfg *pCfg)
{
	calc_kdcfg_checksum(pCfg);
	lseek(fd, g_kdoffset, SEEK_SET);
	write(fd, pCfg, DEV_KANDIAN_CFG_LEN);
	return 0;
}

DEV_SystemCfg * get_system_cfg()
{
	if(g_initflag == 0)
	{
		int fd;
		if((fd = open_file(SYSCFG_FILE_PATH)) < 0)
		{
			printf("Open system cfg file failed\n");
			return &g_syscfg;
		}
		if(read_lock_file(fd) < 0)
		{
			printf("read lock failed.\n");
			return &g_syscfg;
		}
		read_cfg_file(fd, &g_syscfg);
		unlock_file(fd);
		close(fd);
		g_initflag = 1;
	}
	return &g_syscfg;
}

int save_system_cfg(DEV_SystemCfg *pCfg)
{
	int fd, ret;
	if((fd = open_file(SYSCFG_FILE_PATH)) < 0)
	{
		printf("Open system cfg file failed\n");
		return -1;
	}
	if(write_lock_file(fd) < 0)
	{
		printf("write lock failed\n");
		return -1;
	}
	memcpy(&g_syscfg, pCfg, sizeof(g_syscfg));
	ret = write_cfg_file(fd, pCfg);
	unlock_file(fd);
	close(fd);
	return ret;
}

int default_system_cfg()
{
	DEV_SystemCfg cfg;
	memcpy(&cfg, &g_defsyscfg, DEV_SYSTEM_CFG_LEN);
	return save_system_cfg(&cfg);
}

DEV_KanDianCfg *get_kandian_cfg()
{
	if(g_initkdflag == 0)
	{
		int fd;
		if((fd = open_file(SYSCFG_FILE_PATH)) < 0)
		{
			printf("Open system cfg file failed\n");
			return &g_kdcfg;
		}
		if(read_lock_file(fd) < 0)
		{
			printf("read lock failed.\n");
			return &g_kdcfg;
		}
		read_kandian_cfg_file(fd, &g_kdcfg);
		unlock_file(fd);
		close(fd);
		g_initkdflag = 1;
	}
	return &g_kdcfg;
}

int save_kandian_cfg(DEV_KanDianCfg *pCfg)
{
	int fd, ret;
	if((fd = open_file(SYSCFG_FILE_PATH)) < 0)
	{
		printf("Open system cfg file failed\n");
		return -1;
	}
	if(write_lock_file(fd) < 0)
	{
		printf("write lock failed\n");
		return -1;
	}
	memcpy(&g_kdcfg, pCfg, sizeof(g_kdcfg));
	ret = write_kandian_cfg_file(fd, pCfg);
	unlock_file(fd);
	close(fd);
	return ret;
}

