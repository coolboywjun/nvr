#ifndef __NVR_P2P_TUTK_H__
#define __NVR_P2P_TUTK_H__


#include <glib.h>
#include <glib-unix.h>
#include <glib/gprintf.h>


#define P2P_TUTK_DEBUG

#define j_message(fmt, args...) g_message("\33[33m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_error(fmt, args...)	g_error("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_warning(fmt, args...)	g_warning("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_critical(fmt, args...)	g_critical("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)

#if defined(__DEBUG__) && defined(P2P_TUTK_DEBUG)
#define j_debug(fmt, args...)	g_debug("\33[32m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#else
#define j_debug(fmt, args...)
#endif



int nvr_p2p_tutk_init();

int nvr_p2p_tutk_uninit();




#endif

