#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <av_pool.h>

#include <ipnc_lib.h>
#include <ipnc_lib_server.h>
#include "onvif_ipnc_realplay.h"
#include "onvif_ipnc_mng.h"
#include "onvif_ipnc_preview.h"

int onvif_ipnc_start_get_stream(long handle, int chn, int streamNo, long *streamHandle)
{
	
	//printf("@@@@@@@@@@@@@@ onvif start get stream\r\n");
	return onvif_ipnc_start_write_frame(handle, chn, streamNo, streamHandle);
	
}

int onvif_ipnc_stop_get_stream(long handle, long streamHandle)
{		
	//printf("@@@@@@@@@@@@@@ onvif stop get stream\r\n");
	if(0 == handle || 0 == streamHandle)
		return -1;
	
	return onvif_ipnc_stop_write_frame(handle, streamHandle);
}

int onvif_ipnc_request_iframe(long handle, long streamHandle)
{	
	//printf("@@@@@@@@@@@@@@ onvif request iframe\r\n");	
	return onvif_ipnc_get_request_iframe(handle, streamHandle);
}

