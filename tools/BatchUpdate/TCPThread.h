//---------------------------------------------------------------------------

#ifndef TCPThreadH
#define TCPThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <winsock.h>
#include "TCPSendThread.h"
//---------------------------------------------------------------------------
class TTCPServer : public TThread
{            
private:
    volatile int devNum;
    SOCKET tcpSock;
    TTCPSendThread *sendThread;
protected:
    void __fastcall Execute();
public:
    __fastcall TTCPServer(bool CreateSuspended);
    void __fastcall TTCPServer::ResetSetNo();
};
//---------------------------------------------------------------------------
#endif
