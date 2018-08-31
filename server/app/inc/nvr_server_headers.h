#ifndef __NVR_SERVER_HEADERS_H__
#define __NVR_SERVER_HEADERS_H__


#include <nvr_headers.h>

#include <glib.h>
#include <glib-unix.h>
#include <glib/gprintf.h>

#include <log.h>

#define NVR_SERVER_DEBUG

#define j_message(fmt, args...) g_message("\33[33m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_error(fmt, args...)	g_error("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_warning(fmt, args...)	g_warning("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_critical(fmt, args...)	g_critical("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#if defined(__DEBUG__) && defined(NVR_SERVER_DEBUG)
#define j_debug(fmt, args...)	g_debug("\33[32m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#else
#define j_debug(fmt, args...)
#endif


#ifndef NVR_SOFT_VERSION
#define NVR_SOFT_VERSION	"v1.0.0"
#endif

#define ARRAY_SIZE(array)	(sizeof(array)/sizeof(array[0]))

#endif

