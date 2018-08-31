
#ifndef __JXJ_CU_PREVIEW_H__
#define __JXJ_CU_PREVIEW_H__
#include "ipnc_lib.h"

int jxj_cu_start_write_frame(long ipncHandle, int chn, int nStream, 
							 long *pStreamHandle);
int jxj_cu_stop_write_frame(long ipncHandle, long streamHandle);

int jxj_cu_get_request_iframe(long ipncHandle, long streamHandle);

#endif



