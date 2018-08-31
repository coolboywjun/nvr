//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UDPThread.h"
#include "MainFrm.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TUDPThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TUDPThread::TUDPThread(bool CreateSuspended)
: TThread(CreateSuspended)
{
	SOCKADDR_IN fromAddr;

	udpSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSock == INVALID_SOCKET)
	{
		errStr = "Failed to create socket.";
		Synchronize(UpdateEdit);
		return;
	}

	BOOL yes = TRUE;
	int ret = setsockopt(udpSock, SOL_SOCKET, SO_BROADCAST, (char FAR *)&yes, sizeof(BOOL));
	if (ret == SOCKET_ERROR)
	{
		errStr = "Failed to set udp broadcase.";
		Synchronize(UpdateFrom);
		return;
	}
	char ipStr[32];
	Form1->GetHostIP(ipStr);
	memset(&fromAddr,0,sizeof(fromAddr));
	fromAddr.sin_family = AF_INET;
	//fromAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	fromAddr.sin_addr.S_un.S_addr = inet_addr(ipStr);
	fromAddr.sin_port = htons(33090);
	if (bind(udpSock,(SOCKADDR*)&fromAddr,sizeof(fromAddr))<0)
	{
		Form1->SetEditText("Failed to bind server socket.");
		return;
	}
}
__fastcall TUDPThread::~TUDPThread()
{
	closesocket(udpSock);
}
void __fastcall TUDPThread::UpdateFrom()
{
	AnsiString str = "获取IP地址失败!!!";
	Form1->lblInfo->Caption = str;
	Form1->SetEditText(str);
	Form1->btnUpdate->Enabled = false;
	Form1->ComboBox1->Enabled = false;
	Form1->ckHWID->Enabled = false;
	Form1->ckMAC->Enabled = false;
	Form1->btnOpenComm->Enabled = false;
}
void __fastcall TUDPThread::UpdateEdit()
{
	Form1->SetEditText(errStr);
}
//---------------------------------------------------------------------------
void __fastcall TUDPThread::Execute()
{
	char buffer[1024];
	SOCKADDR_IN udpAddr;
	int addrLen;
	int recvLen;
	PSTJMsgHeader pHead;
	net_node_struct *pNetNode;
	char ipStr[32];
	int ip, ip1, ip2, ip3, ip4;

	//---- Place thread code here ----
	Form1->GetHostIP(ipStr);
	if (sscanf(ipStr, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4) != 4)
	{
		errStr = "Host IP is wrong.";
		Synchronize(UpdateFrom);
		return;
	}

	while (!this->Terminated)
	{
		addrLen = sizeof(udpAddr);
		recvLen = recvfrom(udpSock, buffer, sizeof(buffer), 0,
						   (SOCKADDR *)&udpAddr, &addrLen);
		if (recvLen == SOCKET_ERROR)
		{
			Sleep(5);
			continue;
		}
		pHead = (PSTJMsgHeader)buffer;
		if (pHead->uMagic != 33080)
			continue;
		pNetNode = (net_node_struct*)pHead->dat;
		netInfo.node[clientNum].random = pNetNode->random;
		memcpy(netInfo.node[clientNum].mac, pNetNode->mac, sizeof(netInfo.node[clientNum].mac));
		ip = ((ip4 + clientNum + 1) % 200) + 1;
		snprintf(ipStr, sizeof(ipStr), "%d.%d.%d.%d", ip1, ip2, ip3, ip);
		netInfo.node[clientNum].ip_addr.S_un.S_addr = inet_addr(ipStr);
		this->clientNum ++;
	}
}
//---------------------------------------------------------------------------
int __fastcall TUDPThread::GetClientNum()
{
	return this->clientNum;
}
void __fastcall TUDPThread::ResetClientNum()
{
	this->clientNum = 0;
}
int __fastcall TUDPThread::SendBroadCastPacket()
{
	struct sockaddr_in udpAddr;
	char buffer[1024];
	PSTJMsgHeader pHead = (PSTJMsgHeader)buffer;

	memset(buffer, 0, sizeof(buffer));
	pHead->uMagic = HEAD_MAGIC;
	pHead->uType = PSTJ_BROADCASE_SCAN;

	memset(&udpAddr, 0, sizeof(udpAddr));
	udpAddr.sin_family = AF_INET;
	udpAddr.sin_port = htons(UDP_SERVER_PORT);
	udpAddr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);

	int sendLen = sendto(udpSock, buffer, sizeof(buffer), 0,
						 (struct sockaddr *)&udpAddr,
						 sizeof(udpAddr));
	if (sendLen == SOCKET_ERROR)
	{
		Form1->SetEditText("Failed to send broadcase udp packet.");
		return false;
	}
	return true;
}
int __fastcall TUDPThread::SendBroadCastNetInfo(net_struct *pNetInfo)
{
	char buffer[1024];
	PSTJMsgHeader pHead;
	SOCKADDR_IN udpAddr;

	if (pNetInfo == NULL)
	{
		return false;
	}
	pHead = (PSTJMsgHeader)buffer;
	pHead->uMagic = HEAD_MAGIC;
	pHead->uType = PSTJ_BROADCASE_IP_TABLE_TO_SET_MAC_AND_HWID;
	pHead->ret = 0;
	memcpy(pHead->dat, pNetInfo, sizeof(*pNetInfo));

	memset(&udpAddr, 0, sizeof(udpAddr));
	udpAddr.sin_family = AF_INET;
	udpAddr.sin_port = htons(UDP_SERVER_PORT);
	udpAddr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);
	int sendLen = sendto(udpSock, buffer,
						 sizeof(buffer), 0,
						 (struct sockaddr FAR *)&udpAddr, sizeof(udpAddr));
	if (sendLen == SOCKET_ERROR)
	{
		errStr = "发送IP_TABLE UDP广播包失败";
		Synchronize(UpdateFrom);
		return false;;
	}
	return true;
}
int __fastcall TUDPThread::GetNetInfo(net_struct *pNetInfo)
{
	SOCKADDR_IN udpAddr;

	if (pNetInfo == NULL)
		return -1;
	memcpy(pNetInfo, &netInfo, sizeof(netInfo));
	return 0;
}

