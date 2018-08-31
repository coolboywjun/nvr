
#ifndef _OSA_DEBUG_H_
#define _OSA_DEBUG_H_

#include <stdio.h>
#include <osa_mutex.h>
#include<syslog.h>

extern OSA_MutexHndl gOSA_printfLock;
           
// printf wrapper that can be turned on and off by defining/undefining
#ifdef OSA_DEBUG_MODE
           
  #define OSA_printf(...)  \
  { \
    OSA_mutexLock(&gOSA_printfLock); \
    printf(__VA_ARGS__); \
    OSA_mutexUnlock(&gOSA_printfLock); \
  }

  #define OSA_assert(x)  \
    if( (x) == 0) { \
      fprintf(stderr, " ASSERT (%s|%s|%d)\r\n", __FILE__, __func__, __LINE__); \
      while (getchar()!='q')  \
        ; \
    } 

#define OSA_DEBUG \
  fprintf(stderr, " %s:%s:%d Press Any key to continue !!!", __FILE__, __func__, __LINE__); 


#define OSA_DEBUG_WAIT \
  OSA_DEBUG \
  getchar();
            
#else
  
  #define OSA_printf(...)
  #define OSA_assert(x)
  #define OSA_DEBUG    
  #define OSA_DEBUG_WAIT  
#endif

// printf wrapper that can be used to display errors. Prefixes all text with
// "ERROR" and inserts the file and line number of where in the code it was
// called
#define OSA_ERROR(fmt,args...) \
do\
{\
	if(0)\
	{\
		syslog(LOG_ERR,"(%s|%s|%d):" fmt,__FILE__, __func__, __LINE__,##args);\
	}\
	else\
	{\
		fprintf(stderr, "player_ser ERROR(%s|%s|%d): ", __FILE__, __func__, __LINE__);\
		fprintf(stderr, fmt"\r\n",##args); \
	}\
}\
while(0);

#define DEBUG_PF(fmt,args...)\
do\
{\
	if(1)\
	{\
		syslog(LOG_DEBUG,"(%s|%s|%d):" fmt,__FILE__, __func__, __LINE__,##args);\
	}\
	else\
	{\
		fprintf(stderr, "player_ser(%s|%d):", __FILE__,__LINE__);\
		fprintf(stderr, fmt"\r\n",##args); \
	}\
}while(0);
#define OSA_assertSuccess(ret)  OSA_assert(ret==OSA_SOK)


#endif  //_OSA_DEBUG_H_ 



