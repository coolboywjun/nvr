

#include <osa.h>
#include <osa_thr.h>
#include <osa_mutex.h>
#include <sys/time.h>
#include <unistd.h>

OSA_MutexHndl gOSA_printfLock;

int OSA_init()
{
  int status;
  
  status = OSA_mutexCreate(&gOSA_printfLock);
  if(status!=OSA_SOK)
    OSA_ERROR("OSA_mutexCreate()\n");
    
  #if 0    
  OSA_printf(" OSA: THR_PRI = %d .. %d\n", OSA_THR_PRI_MIN, OSA_THR_PRI_MAX);  
  #endif
  
  return status;  
}

int OSA_exit()
{
  int status;
  
  status = OSA_mutexDelete(&gOSA_printfLock);

  return status;  
}

int OSA_attachSignalHandler(int sigId, void (*handler)(int ) )
{
  struct sigaction sigAction;

  /* insure a clean shutdown if user types ctrl-c */
  sigAction.sa_handler = handler;
  sigemptyset(&sigAction.sa_mask);
  sigAction.sa_flags = 0;
  sigaction(sigId, &sigAction, NULL);
  
  return OSA_SOK;
}

Uint32 OSA_getCurTimeInMsec()
{
  struct timeval tv;

  if (gettimeofday(&tv, NULL) < 0) 
    return 0;

  return tv.tv_sec * 1000 + tv.tv_usec/1000;
}
Uint32 OSA_getCurTimeInUsec()
{
  struct timeval tv;

  if (gettimeofday(&tv, NULL) < 0) 
    return 0;

  return tv.tv_sec * 1000000 + tv.tv_usec;
}
Uint32 OSA_getRunTimeInMsec()
{
  struct timespec tp;
  
  if (clock_gettime(CLOCK_MONOTONIC, &tp) < 0) 
    return 0;

  return tp.tv_sec * 1000 + tp.tv_nsec/1000000;
}
Uint32 OSA_getRunTimeInUsec()
{
  struct timespec tp;
  
  if (clock_gettime(CLOCK_MONOTONIC, &tp) < 0) 
    return 0;

  return tp.tv_sec * 1000000 + tp.tv_nsec/1000;
}

void   OSA_waitMsecs(Uint32 msecs)
{
  struct timespec delayTime, elaspedTime;
  
  delayTime.tv_sec  = msecs/1000;
  delayTime.tv_nsec = (msecs%1000)*1000000;

  nanosleep(&delayTime, &elaspedTime);
}

static char xtod(char c) {
  if (c>='0' && c<='9') return c-'0';
  if (c>='A' && c<='F') return c-'A'+10;
  if (c>='a' && c<='f') return c-'a'+10;
  return c=0;        // not Hex digit
}
  
static int HextoDec(char *hex, int l)
{
  if (*hex==0) 
    return(l);

  return HextoDec(hex+1, l*16+xtod(*hex)); // hex+1?
}
  
int xstrtoi(char *hex)      // hex string to integer
{
  return HextoDec(hex,0);
}

int OSA_getHostName(char *hostname, int maxHostNameLen)
{
  int status;
  
  hostname[0]=0;

  status = gethostname(hostname, maxHostNameLen);
  if(status<0)
    return OSA_EFAIL;
  
  return OSA_SOK;
}
void getTimeOfDayFirst(struct timeval *begin_tv)
{
	gettimeofday(begin_tv, NULL);
}
void getTimeOfDayEnd(const char *name, struct timeval *begin_tv, char print_mode)
{
	struct timeval tv;
	long interval;
	gettimeofday(&tv, NULL);
	if(print_mode == 'm')
	{
		interval = (tv.tv_sec - begin_tv->tv_sec) * 1000 + (tv.tv_usec - begin_tv->tv_usec) / 1000;
		printf("%s:  interval_ms = %ld\n", name, interval);
	}
	else
	{
		interval = (tv.tv_sec - begin_tv->tv_sec) * 1000000 + (tv.tv_usec - begin_tv->tv_usec);
		printf("%s:  interval_us = %ld\n", name, interval);
	}
}
void get_average_value(unsigned int*in,unsigned int *out,unsigned int num)
{
	unsigned int i,max=0,min=0xffffffff,sum=0;
	if(num<=2)
	{
		printf("get average value num %d small than 2\r\n",num);
	}
	for(i=0;i<num;i++)
	{
		max=*(in+i)>max?*(in+i):max;
		min=*(in+i)<min?*(in+i):min;
		sum+=*(in+i);
	}
	sum-=(max+min);
	*out=sum/(num-2);
}
