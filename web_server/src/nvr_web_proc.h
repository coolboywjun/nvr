#ifndef __NVR_WEBS_PROC_H__
#define __NVR_WEBS_PROC_H__

#include "webs.h"

/*BUGµ÷ÊÔ*/
#define DEBUG_CLIENT
#ifdef DEBUG_CLIENT
	#define __DBUG(fmt, args...) fprintf(stderr, "DEBUG(%s:%d):" fmt, __func__, __LINE__, ## args)
#else
	#define __DBUG(fmt, args...) 
#endif

#define __ERR(fmt, args...) fprintf(stderr, "ERROR(%s:%d):" fmt, __func__, __LINE__, ## args)


int getLoginInfo(int eid, webs_t wp, int argc, char_t **argv);
void loginInfo(webs_t wp, char_t *path, char_t *query);

int getMainInfo(int eid, webs_t wp, int argc, char_t **argv);
	
#endif

