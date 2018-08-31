
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <log.h>


#define LOG_PATH	"/mnt/nand/nvr.log"
#define LOG_FLOCK_PATH	"/tmp/log_flock"

static int lock_fd = 0;

char *log_read(FILE *fp, char *buff, int buf_len)
{
	struct flock lock;
	char *ret;
	
	lock.l_type = F_RDLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
	fcntl(lock_fd, F_SETLK, &lock);
	
	ret = fgets(buff, buf_len, fp);
	
	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
	fcntl(lock_fd, F_SETLK, &lock);

	return ret;
}

void log_delete()
{	
	struct flock lock;
	FILE *fp = NULL;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
	fcntl(lock_fd, F_SETLK, &lock);
	
	fp = fopen(LOG_PATH, "w");
	if (fp != NULL)
		fclose(fp);

	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
	fcntl(lock_fd, F_SETLK, &lock);
}

FILE *log_read_start()
{
	FILE *fp;

	fp = fopen(LOG_PATH, "r");
	if(fp == NULL)
	{
		printf("Open %s failed.\n", LOG_PATH);
		return NULL;
	}
	
	return fp;
}

void log_read_stop(FILE *fp)
{
	fclose(fp);
}

const char *LOG_STR[] = 
{
	"EMERG",
	"ALERT",
	"CRIT",
	"ERR",
	"WARNING",
	"NOTICE",
	"INFO",
	"DEBUG"
};

int log_write(int log_level, LOG_TYPE log_type, const char *fmt, ...)
{
	FILE *fp;
	struct flock lock;
	struct stat st;
	char buff[1024];
	time_t t;
	struct tm stm, *ptm;
	va_list args;
	const char *level_str;
	
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
	fcntl(lock_fd, F_SETLK, &lock);
	if(stat(LOG_PATH, &st) < 0)
		fp = fopen(LOG_PATH, "w");
	else
	{
		if(st.st_size < 1024 * 1024)		// 日志文件不大于1M
			fp = fopen(LOG_PATH, "a");
		else
			fp = fopen(LOG_PATH, "w");
	}	
	if(fp == NULL)
	{
		printf("Open %s filed failed.\n", LOG_PATH);
		lock.l_type = F_UNLCK;
		fcntl(lock_fd, F_SETLK, &lock);
		return -1;
	}

	if(log_level >=0 && log_level < sizeof(LOG_STR)/sizeof(LOG_STR[0]))
		level_str = LOG_STR[log_level];
	else
		level_str = "UNKNOW";
		
	t = time(NULL);
	ptm = localtime_r(&t, &stm);
	snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02d %8s %d    ",
			ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
			level_str, log_type);
	fputs(buff, fp);
	
	va_start(args, fmt);
	vsnprintf(buff, sizeof(buff), fmt, args);
	va_end(args);
	if(buff[strlen(buff)-1] != '\n')
		strcat(buff, "\n");
	fputs(buff, fp);
	
	fclose(fp);
	
	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_pid = getpid();
	fcntl(lock_fd, F_SETLK, &lock);
	
	return 0;
}

int log_init()
{
	lock_fd = open(LOG_FLOCK_PATH, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0666);
	if(lock_fd < 0)
	{
		printf("Open %s failed.\n", LOG_FLOCK_PATH);
		return -1;
	}
	return 0;
}

void log_uninit()
{
	close(lock_fd);
}

