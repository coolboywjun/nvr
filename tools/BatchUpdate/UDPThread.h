//---------------------------------------------------------------------------

#ifndef UDPThreadH
#define UDPThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <winsock.h>

#include "net_struct.h"
//---------------------------------------------------------------------------
class TUDPThread : public TThread
{
private:
    volatile int clientNum;
    volatile int cfgNum;
    SOCKET udpSock;
    net_struct netInfo;
    char ipStr[32];
protected:
    void __fastcall Execute();
public:
    __fastcall TUDPThread(bool CreateSuspended);
    __fastcall TUDPThread::~TUDPThread();
    int __fastcall SendBroadCastPacket();
    int __fastcall SendBroadCastNetInfo(net_struct *pNetInfo);
    int __fastcall SendBroadPlatType(unsigned short platType);
    int __fastcall GetClientNum(); 
    void __fastcall ResetClientNum();
    int __fastcall GetNetInfo(net_struct *pNetInfo);
    void __fastcall UpdateFrom();
    void __fastcall UpdateCfgLable();
    int __fastcall GetCfgNum();
    void __fastcall ResetCfgNum();
};
//---------------------------------------------------------------------------
#endif
