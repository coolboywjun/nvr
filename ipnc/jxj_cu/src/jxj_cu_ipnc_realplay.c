#include "jxj_cu_heads.h"
#include "jxj_cu_ipnc_realplay.h"
#include "jxj_cu_preview.h" 
#include "jxj_cu_ipnc_mng.h" 

int jxj_cu_ipnc_start_get_stream(long handle, int chn, int streamNo, long *pStreamHandle)
{
	if(0 == handle ||
	   NULL == pStreamHandle)
		return -1;

	return jxj_cu_start_write_frame(handle, chn, streamNo, pStreamHandle);
}

int jxj_cu_ipnc_stop_get_stream(long handle, long streamHandle)
{
	if(0 == handle || 0 == streamHandle)
		return -1;
	
	return jxj_cu_stop_write_frame(handle, streamHandle);
}

int jxj_cu_ipnc_request_iframe(long handle, long streamHandle)
{
	return jxj_cu_get_request_iframe(handle, streamHandle);
}



