
#ifndef DHCP_H
#define DHCP_H


extern int   BroadcastResp;
extern int   TokenRingIf;
extern int   BeRFC1541;
extern int   TokenRingIf;
extern char	HostName[128];
extern int	DebugFlag;
extern int	DoCheckSum;
extern char ClassID[128];
extern char	ClientID[128];
extern int nleaseTime;
extern int LeaseTime;
extern int SendSecondDiscover;
extern char HostName[128];


typedef void *dhcpHandle;


int dhcpLibInit(const char *IfName);

void dhcpLibUninit();

int dhcpLibFindDhcpServer();

dhcpHandle dhcpLibRequest(unsigned char *ClientHwAddr,
						unsigned long *ip, unsigned long *netmask, 
						unsigned long *gatewayip, unsigned long *dns1, unsigned long *dns2);

void dhcpLibRelease(dhcpHandle handle);


#endif

