#ifndef _NVR_ARP_SEARCH_H_
#define _NVR_ARP_SEARCH_H_

typedef void (*ArpIpConflictFunc)(uint32_t u32DevIp, gpointer userData);

int nvr_arp_check_ip_conflict(uint32_t u32DevIp);

int nvr_arp_get_mac(uint32_t u32DevIp, uint8_t *pDevMac);

void nvr_arp_add_listen_ip(uint32_t u32DevIp, ArpIpConflictFunc func, gpointer userData);

void nvr_arp_remove_listen_ip(uint32_t u32DevIp);

int nvr_arp_init();

void nvr_arp_uninit();


#endif


