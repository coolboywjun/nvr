
#ifndef __ONVIF_IPNC_PREVIEW_H__
#define __ONVIF_IPNC_PREVIEW_H__
#include "ipnc_lib.h"

int onvif_ipnc_start_write_frame(long ipncHandle, int chn, int nStream, long *pStreamHandle);

int onvif_ipnc_stop_write_frame(long ipncHandle, long streamHandle);

int onvif_ipnc_get_request_iframe(long ipncHandle, long streamHandle);

#endif



