#ifndef __NVR_PLATFORM_H__
#define __NVR_PLATFORM_H__

#include "j_sdk.h"
#include "j_sdk_inters.h"
#include "mb_api.h"

#include <glib.h>
#include <glib-unix.h>
#include <glib/gprintf.h>

#define PLATFORM_JXJ_DEBUG

#define j_message(fmt, args...) g_message("\33[33m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_error(fmt, args...)	g_error("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_warning(fmt, args...)	g_warning("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_critical(fmt, args...)	g_critical("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#if defined(__DEBUG__) && defined(PLATFORM_JXJ_DEBUG)
#define j_debug(fmt, args...)	g_debug("\33[32m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#else
#define j_debug(fmt, args...)
#endif


#define JXJ_RTSP_PORT		7554
#define JXJ_SDK_PORT    		3321
#define JXJ_TALK_PORT    	3322
#define JXJ_UPGRADE_PORT	15600
#define DEV_TYPE 			J_Dev_NVR_1_3520
#define PU_TYPE 				J_SDK_NVR
#define JXJ_CHN_MAX			4

int nvr_jxj_pf_init();
int nvr_jxj_pf_uninit();


#endif


