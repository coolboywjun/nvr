#ifndef __NVR_ALARM_H__
#define __NVR_ALARM_H__

#include <nvr_server_headers.h>

//报警触发处理
int nvr_alarm_proc(int nPort);

//初始化函数
int nvr_alarm_init(void);
void nvr_alarm_uninit(void);



#endif

