#ifndef __NVR_SMTP_H__
#define __NVR_SMTP_H__

#include "nvr_struct.h"

/*发送邮件
	pSubJect：发送邮件标题
	pBody：发送正文
*/
void nvr_stmp_send_mail(const char* pSubJect, const char* pBody);

#endif

