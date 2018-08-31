
#ifndef __HI_ONVIF_H__
#define __HI_ONVIF_H__

#include <stdio.h>
//#define DEBUG_ONVIF  1
//#define LOG_ERR(fmt, args...)

#define DEFAULT_ONVIF_SERVER_PORT	80
#define DEFAULT_ONVIF_WSD_PORT 		3702

#ifdef DEBUG_ONVIF
	#define __D(fmt, args...) fprintf(stderr, "ONVIF DEBUG(%s:%d): " fmt, __func__, __LINE__, ## args)
	#define __FUN_BEGIN(fmt, args...) fprintf(stderr, "ONVIF FUNCTION BEGIN(%s, %d): " fmt, __func__, __LINE__, ##args);
	#define __FUN_END(fmt, args...) fprintf(stderr, "ONVIF FUNCTION END(%s, %d): " fmt, __func__, __LINE__, ##args);
#else
	#define __D(fmt, args...) 
	#define __FUN_BEGIN(fmt, args...)
	#define __FUN_END(fmt, args...)
#endif

#define __E(fmt, args...) //hi_platform_log(LOG_ERR, "ONVIF ERROR:(line:%d) " fmt, __LINE__, ## args)

#endif

