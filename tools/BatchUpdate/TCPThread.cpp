//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TCPThread.h"
#include "net_struct.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TTCPServer::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TTCPServer::TTCPServer(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    devNum = 0;
}
//---------------------------------------------------------------------------
void __fastcall TTCPServer::ResetSetNo()
{
    this->devNum = 0;
}
void __fastcall TTCPServer::Execute()
{
    //---- Place thread code here ----
    int sock;
    SOCKADDR_IN clientAddr;
    int addrLen;
    int result;
    SOCKADDR_IN tcpAddr;

    tcpSock = socket(AF_INET,SOCK_STREAM,0);
    if(tcpSock == INVALID_SOCKET)
    {
        Application->MessageBoxA("Socket Open failed","Error",MB_OK);
        return;
    }
    
    memset(&tcpAddr, 0, sizeof(tcpAddr));
    tcpAddr.sin_family = AF_INET;
    tcpAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    tcpAddr.sin_port = htons(TCP_SERVER_PORT);
    bind(tcpSock, (SOCKADDR *)&tcpAddr, sizeof(tcpAddr));
    listen(tcpSock, 1024);

    while(!this->Terminated)
    {
        addrLen = sizeof(clientAddr);
        sock = accept(tcpSock, (SOCKADDR *)&clientAddr, &addrLen);
        if(sock == SOCKET_ERROR)
        {
            //Application->MessageBoxA("TCP连接失败", "TCP线程错误", MB_OK);
            Sleep(10);
            continue;
        }
        sendThread = new TTCPSendThread(0, sock, devNum, &clientAddr);
        this->devNum++;
    }
}
//---------------------------------------------------------------------------
