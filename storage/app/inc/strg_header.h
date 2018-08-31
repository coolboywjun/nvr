#ifndef __STRG_HEADER_H__
#define __STRG_HEADER_H__


//#define _GNU_SOURCE

// system headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <syslog.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <stdarg.h>
#include <termios.h>    /*PPSIX ÷’∂Àøÿ÷∆∂®“Â*/ 
#include <sys/ioctl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <fcntl.h>
#include <libgen.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/shm.h>


// glib
#include <glib.h>
#include <glib-unix.h>
#include <glib/gprintf.h>

// other
#include <log.h>

#define NAME_LEN	64
#define PATH_LEN	128

#define MAX_CHN		4

#define STORAGE_DEBUG

#define j_message(fmt, args...) g_message("\33[33m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_error(fmt, args...)	g_error("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_warning(fmt, args...)	g_warning("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#define j_critical(fmt, args...)	g_critical("\33[31m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#if defined(__DEBUG__) && defined(STORAGE_DEBUG)
#define j_debug(fmt, args...)	g_debug("\33[32m[%s %d] "fmt"\33[37m\n", __func__, __LINE__, ##args)
#else
#define j_debug(fmt, args...)
#endif

#ifndef STORAGE_VERSION
#define STORAGE_VERSION	"v1.0.0"
#endif

#endif

