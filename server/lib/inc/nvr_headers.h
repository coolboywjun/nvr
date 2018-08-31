#ifndef __NVR_HEADERS_H__
#define __NVR_HEADERS_H__

// system headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
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
#include <sys/epoll.h>
#include <sys/sysinfo.h>
#include <sys/msg.h>

// my headers
#include <net_api.h>
#include <nvr_struct.h>
#include <nvr_net_struct.h>
#include <nvr_net_interface.h>
#include <syscfg.h>

typedef enum {
    NVR_FALSE    = 0,
    NVR_TRUE     = 1,
}NVR_BOOL;

#define NVR_SUCCESS  0
#define NVR_FAILURE  (-1)

#endif

