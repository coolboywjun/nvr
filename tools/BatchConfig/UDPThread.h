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
    int clientNum;
    SOCKET udpSock;
    net_struct netInfo;
    AnsiString errStr;
protected:
    void __fastcall Execute();
public:
    __fastcall TUDPThread(bool CreateSuspended);
    __fastcall TUDPThread::~TUDPThread();
    int __fastcall SendBroadCastPacket();
    int __fastcall SendBroadCastNetInfo(net_struct *pNetInfo);
    int __fastcall GetClientNum(); 
    void __fastcall ResetClientNum();
    int __fastcall GetNetInfo(net_struct *pNetInfo);
    void __fastcall UpdateFrom();
    void __fastcall UpdateEdit();
};
//---------------------------------------------------------------------------
#endif

