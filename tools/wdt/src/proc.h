//////////////////////////////////////////////////
//proc.h
/////////////////////////////////////////////////////
//made by zhx.
//
//
//
//



#ifndef __IPNC_PROC_H__
#define __IPNC_PROC_H__

	

#ifdef __cplusplus
extern "C" {
#endif



int GetProcPid(const char *cmd);
int CheckProcStat(int pid);


#ifdef __cplusplus
}
#endif

#endif

