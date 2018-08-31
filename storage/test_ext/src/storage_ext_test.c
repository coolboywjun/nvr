


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h> 
#include <fcntl.h>

#include <storage_lib.h>
#include <av_pool.h>

#include <storage_ext_lib.h>

#define STREAM_NUM	0

static int quit = 0;
static void sig_quit(int signo)
{
	quit = 1;
}

static time_t make_time(int year, int mon, int day, int hour, int min, int sec)
{
	struct tm tm_str;

	tm_str.tm_year = year - 1900;
	tm_str.tm_mon = mon - 1;
	tm_str.tm_mday = day;
	tm_str.tm_hour = hour;
	tm_str.tm_min = min;
	tm_str.tm_sec = sec;
	
	return mktime(&tm_str);
}

static int storage_ext_test_write_avi_file()
{
	time_t begin_time, end_time;
	int percent;
	int ret;
	
	begin_time = make_time(1999, 1, 1, 0, 0, 0);
	end_time = make_time(2020, 1, 1, 0, 0, 0);
	if((ret = storage_ext_avi_file_by_detail(1, begin_time, end_time, RECORD_ALL_TYPE)) < 0)
	{
		printf("Failed to write avi file by detail, ret: %d.\n", ret);
		return -1;
	}

	printf("#######111\n");

	while(!quit)
	{
		percent = storage_ext_get_percent();
		printf("percent: %d\n", percent);
		if(percent >= 100 || percent < 0)
			break;
		sleep(1);
	}
	printf("#######222\n");
	return 0;
}



int main(int argc, char *argv[])
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, sig_quit);
	signal(SIGTERM, sig_quit);

	if(storage_ext_init() < 0)
	{
		printf("Failed to initialize storage lib\n");
		return -1;
	}
	if(storage_ext_test_write_avi_file() < 0)
	{
		storage_ext_uninit();
		return -1;
	}
	
	//while (!quit) pause();
	
	storage_ext_uninit(); 

	return 0;
}
