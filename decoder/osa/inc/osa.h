

#ifndef _OSA_H_
#define _OSA_H_

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#define OSA_DEBUG_MODE // enable OSA_printf, OSA_assert
#define OSA_DEBUG_FILE // enable printf's during OSA_fileXxxx
#define OSA_PRF_ENABLE // enable profiling APIs

#define OSA_SOK      0  ///< Status : OK
#define OSA_EFAIL   -1  ///< Status : Generic error

#ifndef xdc_std__include

#ifndef _TI_STD_TYPES
#define _TI_STD_TYPES

#ifndef TRUE

typedef short   Bool;                 ///< Boolean type

#define TRUE		((Bool) 1)        ///< Boolean value : TRUE
#define FALSE		((Bool) 0)        ///< Boolean value : FALSE

#endif

/* unsigned quantities */
typedef unsigned long long Uint64;      ///< Unsigned 64-bit integer
typedef unsigned int Uint32;            ///< Unsigned 32-bit integer
typedef unsigned short Uint16;          ///< Unsigned 16-bit integer
typedef unsigned char Uint8;            ///< Unsigned  8-bit integer

/* signed quantities */
typedef long long Int64;               ///< Signed 64-bit integer
typedef int Int32;                     ///< Signed 32-bit integer
typedef short Int16;                   ///< Signed 16-bit integer
typedef char Int8;                     ///< Signed  8-bit integer

#endif /* _TI_STD_TYPES */

#endif

#ifndef KB
#define KB ((Uint32)1024)
#endif

#ifndef MB
#define MB (KB*KB)
#endif

#define OSA_TIMEOUT_NONE        ((Uint32) 0)  // no timeout
#define OSA_TIMEOUT_FOREVER     ((Uint32)-1)  // wait forever

#define OSA_memAlloc(size)      (void*)malloc((size))
#define OSA_memFree(ptr)        free(ptr)

#define OSA_align(value, align)   ((( (value) + ( (align) - 1 ) ) / (align) ) * (align) )

#define OSA_floor(value, align)   (( (value) / (align) ) * (align) )
#define OSA_ceil(value, align)    OSA_align(value, align) 

#define OSA_max(a, b)    ( (a) > (b) ? (a) : (b) )
#define OSA_min(a, b)    ( (a) < (b) ? (a) : (b) )

#include <osa_debug.h>
//#include <osa_prf.h>

Uint32 OSA_getCurTimeInMsec();
Uint32 OSA_getCurTimeInUsec();
Uint32 OSA_getRunTimeInMsec();
Uint32 OSA_getRunTimeInUsec();
void   OSA_waitMsecs(Uint32 msecs);
int    OSA_attachSignalHandler(int sigId, void (*handler)(int ) );
int    OSA_getHostName(char *hostname, int maxHostNameLen);
void getTimeOfDayFirst(struct timeval *begin_tv);
void getTimeOfDayEnd(const char *name, struct timeval *begin_tv, char print_mode);
void get_average_value(unsigned int*in,unsigned int *out,unsigned int num);
int xstrtoi(char *hex);

int OSA_init();
int OSA_exit();

#endif /* _OSA_H_ */



