#ifndef __NVR_FTP_H__
#define __NVR_FTP_H__

#include "nvr_struct.h"

#define NVR_FTP_PORT_STR		"21"


/*上传文件
	pftpcfg:FTP信息配置结构体指针
	pfilename：文件名(不带路径)
	pfilepath：文件所在目录全路径(末尾不带“/”)
成功返回:226
*/
int nvt_ftp_upload(NVR_FTP_CFG_S* pftpcfg ,char* pfilename, char* pfilepath);

/*
	文件上传控制函数
*/
void nvr_ftp_upload_ctrl(char* data[]);
#endif

