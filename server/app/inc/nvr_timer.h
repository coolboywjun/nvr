
#ifndef __NVR_TIMER_H__
#define __NVR_TIMER_H__

int nvr_timer_init();
void nvr_timer_uninit();
NVR_TIME_S *nvr_timer_tm_2_nvr(struct tm *pTmTime, NVR_TIME_S *pStruTime);
struct tm *nvr_timer_nvr_2_tm(NVR_TIME_S *pStruTime, struct tm *pTmTime);

/*设置系统时间*/
int	nvr_sys_timer_set(int nPort, void *pData);
/*获取系统时间*/
int	nvr_sys_timer_get(int nPort, void *pData);

#endif

