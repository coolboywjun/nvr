#ifndef _HI_RTSP_DEBUG_H_
#define _HI_RTSP_DEBUG_H_

#include <stdio.h>

//#define RTSP_DEBUG

#ifdef RTSP_DEBUG
#define RTSP_PRINT(fmt, args...)		\
	fprintf(stderr, "%s : %s : %d | "fmt, __FILE__, __func__, __LINE__, ##args)
#else
#define RTSP_PRINT(fmt, args...)
#endif

#endif

