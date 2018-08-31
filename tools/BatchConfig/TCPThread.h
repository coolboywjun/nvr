//---------------------------------------------------------------------------

#ifndef TCPThreadH
#define TCPThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <winsock.h>
//#include "TCPSendThread.h"
//---------------------------------------------------------------------------
class TTCPServer : public TThread
{
private:
    int devNum;
    SOCKET tcpSock;
    //TTCPSendThread *sendThread;
    AnsiString errStr;
protected:
    void __fastcall Execute();
public:
    __fastcall TTCPServer(bool CreateSuspended);
    void __fastcall ResetSetNo();
    int __fastcall RecvTcpData(SOCKET sockfd, char *rcvBuf, int bufSize, int rcvSize);
    int __fastcall SendTcpData(SOCKET sockfd, char *sendBuf, int sendSize);
    int __fastcall DisposeTcpCmd(SOCKET sockfd);
    void __fastcall UpdateEdit();
};
//---------------------------------------------------------------------------
#endif

