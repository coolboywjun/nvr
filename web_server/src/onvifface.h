
#ifndef __ONVIF_FACE_H__
#define __ONVIF_FACE_H__

int ocheckusername(struct soap *);

char* oget_ipaddress(void);
int oget_encoding(void);
int oget_bitrate(void);
int oget_framerate(void);
int oget_ratecontrol(void);
int oget_ipratio(void); //Govlength
int oget_resolutionwidth(void);
int oget_resolutionheight(void);
int oget_dhcpstatus(void);
char *oget_timezone(int);

int oset_encoding(int res, int encode);
int oset_bitrate(int);
int oset_framerate(int,int,int);
int oset_ratecontrol(int);
int oset_ipratio(int); //Govlength
int oset_resolution(int,int,int);
int oset_dhcpstatus(int);
int oset_timezone(char *TZ);

int onvif_fault(struct soap *,char *value1,char *value2);
int checkhostname(char *hostname);
int isValidIp4 (char *str);
int isValidHostname (char *str);

#endif

