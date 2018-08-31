#ifndef __HI_PLATFORM_ONVIF_H__
#define __HI_PLATFORM_ONVIF_H__


#define HI_PLATFORM_ONVIF_SUBVERSION 14

typedef struct _HI_ONVIF_SOCKET_S_
{
	int eth0Socket;
	int wifiSocket;
}HI_ONVIF_SOCKET_S, *LPHI_ONVIF_SOCKET_S;

HI_ONVIF_SOCKET_S stOnvifSocket;

int hi_onvif_init();
int hi_onvif_uninit();

void hi_onvif_hello();

void hi_onvif_bye(int nType);

#endif

