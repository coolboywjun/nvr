#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "dhcp.h"
#include "dhcpcd.h"
#include "client.h"

int   BroadcastResp = 1;
int   TokenRingIf = 0;
int   BeRFC1541 = 0;
char	HostName[128];
int	DebugFlag = 0;
int	DoCheckSum	= 1;
char ClassID[128] = {"CNVRDHCP"};
char	ClientID[128];
int nleaseTime;
int LeaseTime;
int SendSecondDiscover = 0;
char HostName[128] = {"CNVR"};


static dhcpEtherInfo *etherInfo = NULL;
static int quit;
static pthread_t refreshThid;
static pthread_mutex_t lock;

static void *dhcpRefreshThreadFunc(void *data);

int dhcpLibInit(const char *IfName)
{
    pthread_mutex_init(&lock, NULL);
    nleaseTime = htonl(DEFAULT_LEASETIME);
    LeaseTime = htonl(DEFAULT_LEASETIME);
    etherInfo = dhcpStart(IfName);
    if(etherInfo == NULL)
    {
        printf("dhcp start failed.\n");
        return -1;
    }
    if(pthread_create(&refreshThid, NULL, dhcpRefreshThreadFunc, NULL) != 0)
        refreshThid = 0;
    
    return 0;
}

void dhcpLibUninit()
{
    dhcpClientInfo *clientInfo = etherInfo->ClientInfo;
    dhcpClientInfo *clientInfoTmp;
    pthread_mutex_lock(&lock);
    if(refreshThid != 0)
    {
        quit = 1;
        pthread_join(refreshThid, NULL);
    }
    pthread_mutex_unlock(&lock);
    while(clientInfo != NULL)
    {
        clientInfoTmp = clientInfo->next;
        dhcpRelease(clientInfo);
        clientInfo = clientInfoTmp;
    }
    dhcpStop(etherInfo);
}

static void *dhcpRefreshThreadFunc(void *data)
{
    time_t curTime;
    int leaseTime;
    dhcpClientInfo *clientInfo;
    while(!quit)
    {
        curTime = time(NULL);
        pthread_mutex_lock(&lock);
        clientInfo = etherInfo->ClientInfo;
        while(clientInfo  != NULL)
        {
            if(clientInfo->DhcpOptions.val[dhcpIPaddrLeaseTime] == NULL)
                leaseTime = nleaseTime;
            else
                memcpy(&leaseTime,clientInfo->DhcpOptions.val[dhcpIPaddrLeaseTime],sizeof(leaseTime));
            leaseTime = ntohl(leaseTime);
            //printf("leaseTime: %d, ReqSentTime: %ld, curTime: %ld\n", leaseTime, clientInfo->ReqSentTime, curTime);
            if(clientInfo->ReqSentTime > curTime || curTime - clientInfo->ReqSentTime >= leaseTime / 2)
                dhcpRebind(clientInfo);
            clientInfo = clientInfo->next;
        }
        pthread_mutex_unlock(&lock);
        sleep(3);
    }

    return NULL;
}

int dhcpLibFindDhcpServer()
{
    dhcpClientInfo *clientInfo;

    clientInfo = dhcpInit(etherInfo, NULL);
    if(clientInfo == NULL)
        return 0;
    dhcpRelease(clientInfo);
    
    return 1;
}

dhcpHandle dhcpLibRequest(unsigned char *ClientHwAddr,
                        unsigned long *ip, unsigned long *netmask, 
                        unsigned long *gatewayip, unsigned long *dns1, unsigned long *dns2)
{
    dhcpClientInfo *clientInfo;
    dhcpOptions *DhcpOptions;
    clientInfo = dhcpInit(etherInfo, ClientHwAddr);
    if(clientInfo == NULL)
        return NULL;
    if(dhcpRequest(clientInfo) < 0)
    {
        dhcpRelease(clientInfo);
        return NULL;
    }
    if(ip)  *ip = 0;
    if(netmask) *netmask = 0;
    if(gatewayip) *gatewayip = 0;
    if(dns1) *dns1 = 0;
    if(dns2) *dns2 = 0;
    DhcpOptions = &clientInfo->DhcpOptions;
    if(ip && clientInfo->DhcpIface.ciaddr)
    {
        *ip = clientInfo->DhcpIface.ciaddr;
        *ip = ntohl(*ip);
    }
    if(netmask && DhcpOptions->val[subnetMask])
    {
        memcpy(netmask, DhcpOptions->val[subnetMask], sizeof(unsigned long));
        *netmask = ntohl(*netmask);
    }
    if(gatewayip && DhcpOptions->val[routersOnSubnet])
    {
        memcpy(gatewayip, DhcpOptions->val[routersOnSubnet], sizeof(unsigned long));
        *gatewayip = ntohl(*gatewayip);
    }
    if(dns1 && DhcpOptions->val[dns])
    {
        memcpy(dns1, DhcpOptions->val[dns], sizeof(unsigned long));
        *dns1 = ntohl(*dns1);
    }
    if(dns2 && DhcpOptions->val[dns] && DhcpOptions->len[dns] == 8)
    {
        memcpy(dns2, DhcpOptions->val[dns]+sizeof(unsigned long), sizeof(unsigned long));
        *dns2 = ntohl(*dns2);
    }
    pthread_mutex_lock(&lock);
    if(etherInfo->ClientInfo == NULL)
        etherInfo->ClientInfo = clientInfo;
    else
    {
        clientInfo->next = etherInfo->ClientInfo;
        etherInfo->ClientInfo = clientInfo;
    }
    pthread_mutex_unlock(&lock);
    return (dhcpHandle)clientInfo;
}

void dhcpLibRelease(dhcpHandle handle)
{
    dhcpClientInfo *clientInfo1;
    dhcpClientInfo *clientInfo = (dhcpClientInfo *)handle;
    if(handle == NULL)
        return;
    pthread_mutex_lock(&lock);
    clientInfo1 = etherInfo->ClientInfo;
    if(clientInfo == etherInfo->ClientInfo)
    {
        etherInfo->ClientInfo = clientInfo->next;
    }
    else
    {
        while(clientInfo1->next != NULL)
        {
            if(clientInfo1->next == clientInfo)
            {
                clientInfo1->next = clientInfo->next;
                break;
            }
            clientInfo1 = clientInfo1->next;
        }
    }
    pthread_mutex_unlock(&lock);
    dhcpRelease(clientInfo);
}

#if 1

int main(int argc, char *argv[])
{
    dhcpClientInfo *clientInfo[4];
    unsigned char ClientHwAddr[ETH_ALEN];
    int i;
  
    dhcpLibInit("eth0");
    
    printf("======================= send dhcp discovery\n");    
    memcpy(ClientHwAddr, etherInfo->EtherHwAddr, ETH_ALEN);
    for(i = 0; i < 4; i++)
    {
        // send discovery
        ClientHwAddr[5] += i;
        clientInfo[i] = dhcpInit(etherInfo, ClientHwAddr);
        if(clientInfo[i] == NULL)
        {
            printf("Send dhcp discovery failed.\n");
            return -1;
        }
        printf("Send dhcp discovery successfully.\n");
        printf("======================= send dhcp request\n");    
        if(dhcpRequest(clientInfo[i]) < 0)
        {
            printf("Send dhcp request failed.\n");
            return -1;
        }
    }
    sleep(10);
    printf("======================= send release\n"); 
    for(i = 0; i < 4; i++)
        dhcpRelease(clientInfo[i]);
        
    dhcpLibUninit();
    return 0;
}

#endif

