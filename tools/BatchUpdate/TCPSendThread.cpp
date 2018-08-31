//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TCPSendThread.h"
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
//      void __fastcall TTCPSendThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TTCPSendThread::TTCPSendThread(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    FreeOnTerminate = TRUE;
    kernel = NULL;
    this->devNo = 0;
    totalSendLen = 0;
}
__fastcall TTCPSendThread::TTCPSendThread(bool CreateSuspended, SOCKET sock, int devNo, SOCKADDR_IN *pClientSockAddr)
    : TThread(CreateSuspended)
{
    kernel = NULL;
    sendSock = sock;
    this->devNo = devNo;
    memcpy(&clientSockAddr, pClientSockAddr, sizeof(clientSockAddr));
    totalSendLen = 0;
}
void __fastcall TTCPSendThread::UpdateLable()
{
    Form1->lblInfo->Tag += 1;
    Form1->lblInfo->Caption = "成功升级设备: " + IntToStr(Form1->lblInfo->Tag);
    Form1->Update();
}
void __fastcall TTCPSendThread::UpdateMac()
{
    unsigned char mac[32];
    char ipStr[32];
    
    strcpy(ipStr, inet_ntoa(clientSockAddr.sin_addr));
    if(Form1->GetMacFromIp(ipStr, mac))
    {
        char macStr[32];
        snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
                        mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
        Form1->lblMac[devNo]->Caption = macStr;
        Form1->lblMac[devNo]->Color = clBtnFace;
        Form1->lblMac[devNo]->Show();
    }
}
void __fastcall TTCPSendThread::UpdateLabelAndProgressColor()
{
    Form1->lblMac[devNo]->Color = clRed;
    Form1->lblProgress[devNo]->Color = clRed;
}
void __fastcall TTCPSendThread::UpdateProgress()
{
    Form1->progressBar[devNo]->Position = progress;
    Form1->lblProgress[devNo]->Caption = IntToStr(Form1->progressBar[devNo]->Position) + "%";
}
void __fastcall TTCPSendThread::EnableButton()
{
    Form1->finishDevNum++;
    if(Form1->finishDevNum == Form1->clientDevNum)
    {
        Form1->TimerTCP->Enabled = false;
        Form1->btnUpdate->Enabled = true;
        Form1->btnUpdateFile->Enabled = true;
        Form1->btnCfg->Enabled = true;
        Form1->ComboBox1->Enabled = true;
    }
}
void __fastcall TTCPSendThread::GetUpdateFilePath()
{
    strcpy(updateFilePath, Form1->editUpdateFile->Text.c_str());
}
// make tcp header
void __fastcall TTCPSendThread::MakeTcpHeader(HI_NET_HDR_S *pNetHdr, HI_U32 u32PackLen, 
				HI_U32 u32PackCmd, HI_U32 u32PackNo, HI_S32 s32ErrNo )
{
    pNetHdr->u16Version    = HI_NET_VERSION;
    pNetHdr->u16CmdType  = u32PackCmd;
    pNetHdr->u32PacketSize= u32PackLen;
    pNetHdr->u32PacketNo  = u32PackNo;
    pNetHdr->u32MagicFlag = HI_NET_MAGIC_FLAG;
    pNetHdr->s32ErrNo	  = s32ErrNo;
    pNetHdr->u8Chn          = 0;
}
int __fastcall TTCPSendThread::ConnectToDevice()
{
    struct sockaddr_in addr;
    
    tcpSock = socket(AF_INET,SOCK_STREAM,0);
    if(tcpSock == INVALID_SOCKET)
    {
        Application->MessageBoxA("Socket Open failed","Error",MB_OK);
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(6080);
    memcpy(&addr.sin_addr, &clientSockAddr.sin_addr, sizeof(clientSockAddr.sin_addr));
    if(connect(tcpSock, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        closesocket(tcpSock);
        return -1;
    }
    return 0;
}
int __fastcall TTCPSendThread::SendTcpData(unsigned char *data, int sendlen)
{
    int nSendLen = 0;
    int len;

    while(nSendLen < sendlen)
    {
        if((len = send(tcpSock, data + nSendLen, sendlen - nSendLen, 0)) == SOCKET_ERROR)
        {
            if(WSAGetLastError() == WSAEINTR)
                continue;
            return -1;
        }
        nSendLen += len;
    }
    return nSendLen;
}
int __fastcall TTCPSendThread::RecvTcpData(unsigned char *data, int datalen, int recvlen)
{
    int nRecvLen = 0;
    int len;

    while(nRecvLen < recvlen)
    {
        if((len = recv(tcpSock, data + nRecvLen, recvlen - nRecvLen, 0)) < 0)
        {
            if(WSAGetLastError() == WSAEINTR)
                continue;
            return -1;
        }
        nRecvLen += len;
    }
    return nRecvLen;
}
int __fastcall TTCPSendThread::SendUpdateHeader()
{
        unsigned char buff[1024];
        HI_NET_HDR_S stNetHdr;
        HI_NET_UPGRADE_S stUpgrade;
        MakeTcpHeader(&stNetHdr, sizeof(HI_NET_UPGRADE_S), HI_CMD_START_UPGRADE, u32PackNo, HI_NET_ERR_SUCCESS);
        memset(&stUpgrade, 0, sizeof(HI_NET_UPGRADE_S));
        strcpy(stUpgrade.szUsr, "jxjadmin");
        strcpy(stUpgrade.szPsw, "jxj123");

        memcpy(buff, &stNetHdr, sizeof(stNetHdr));
        memcpy(&buff[sizeof(HI_NET_HDR_S)], &stUpgrade, sizeof(stUpgrade));
        if(SendTcpData(buff, sizeof(HI_NET_HDR_S)+sizeof(HI_NET_UPGRADE_S)) < 0)
            return -1;
        if(RecvTcpData((unsigned char *)&stNetHdr, sizeof(stNetHdr), sizeof(stNetHdr)) < 0)
            return -1;
        if(stNetHdr.u16CmdType != HI_CMD_START_UPGRADE 
            || stNetHdr.u32PacketNo != (u32PackNo | 0x80000000UL)
            || stNetHdr.s32ErrNo != HI_NET_ERR_SUCCESS)
            return -1;
        u32PackNo++;
        return 0;
}
int __fastcall TTCPSendThread::SendUpdateFileInfo(const char *szUpdateFileName)
{
    FILE *fp;
    unsigned char buff[1024];
    HI_NET_HDR_S *pNetHdr = (HI_NET_HDR_S*)buff;
    HI_NET_UPDATE_INFO_S *pUpdateInfo = (HI_NET_UPDATE_INFO_S*)(pNetHdr + 1);
    int nFileLen;

    fp = fopen(szUpdateFileName, "rb");
    if(fp == NULL)
        return -1;
    fseek(fp, 0, SEEK_END);
    nFileLen = ftell(fp);
    fclose(fp);
    
    MakeTcpHeader(pNetHdr, sizeof(HI_NET_UPDATE_INFO_S), HI_CMD_START_UPGRADE, u32PackNo++, HI_NET_ERR_SUCCESS);
    memset(pUpdateInfo, 0, sizeof(HI_NET_UPDATE_INFO_S));
    pUpdateInfo->u32UpdateInfoLen = nFileLen;
    if(SendTcpData(buff, sizeof(HI_NET_HDR_S)+sizeof(HI_NET_UPDATE_INFO_S)) < 0)
        return -1;
    return 0;
}
int __fastcall TTCPSendThread::RecvUpdateProgress()
{
    unsigned char buff[1024];
    HI_NET_HDR_S *pNetHdr = (HI_NET_HDR_S*)buff;
    HI_NET_UPDATE_PRECENT_RES_S *pUpdatePercent = (HI_NET_UPDATE_PRECENT_RES_S*)(pNetHdr+1);
    if(RecvTcpData((unsigned char *)pNetHdr, sizeof(HI_NET_HDR_S), sizeof(HI_NET_HDR_S)) < 0)
    {
        return -1;
    }
    if(RecvTcpData((unsigned char *)pUpdatePercent, sizeof(buff)-sizeof(*pNetHdr), pNetHdr->u32PacketSize) < 0)
    {
        return -1;
    }
    //TRACE("Update percent: %d\n", pUpdatePercent->u8Percent);
    progress = pUpdatePercent->u8Percent;
    Synchronize(UpdateProgress);
    return 0;
}
int __fastcall TTCPSendThread::SendUpdateFile(const char *szUpdateFileName)
{
    int nRet = 0;
    int nReadLen;
    FILE *fp;
    unsigned char buff[2048];
    HI_NET_HDR_S *pNetHdr = (HI_NET_HDR_S*)buff;
    fd_set readSet;
    struct timeval tv;
    
    fp = fopen(szUpdateFileName, "rb");
    if(fp == NULL)
    {
        return -1;
    }
    while((nReadLen = fread(&buff[sizeof(HI_NET_HDR_S)], 1, 1400, fp)) > 0)
    {
        MakeTcpHeader(pNetHdr, nReadLen, HI_CMD_START_UPGRADE, u32PackNo++, HI_NET_ERR_SUCCESS);
        if(SendTcpData(buff, sizeof(HI_NET_HDR_S) + nReadLen) < 0)
        {
            nRet = -1;
            break;
        }
        FD_ZERO(&readSet);
        FD_SET(tcpSock, &readSet);
        memset(&tv, 0, sizeof(tv));
        if(select(tcpSock + 1, &readSet, NULL, NULL, &tv) == 1)
        {
            if(FD_ISSET(tcpSock, &readSet))
            {
                if(RecvUpdateProgress() < 0)
                {
                    nRet = -1;
                    break;
                }
            }
        }
    }
    if(feof(fp) && nRet != -1 && progress != 100)
    {
        if(RecvUpdateProgress() < 0)
        {
            fclose(fp);
            return -1;
        }
    }
    fclose(fp);
    return nRet;
}
//---------------------------------------------------------------------------
void __fastcall TTCPSendThread::Execute()
{
    char buffer[256];
    cmd_info *pCmdInfo;

    //---- Place thread code here ----
    // Get update file path
    Synchronize(GetUpdateFilePath);
    Synchronize(UpdateMac);
    progress = 0;
    Synchronize(UpdateProgress);
    
    totalSendLen = 0;
    while(!this->Terminated)
    {
        int recvLen = recv(sendSock, buffer, sizeof(buffer), 0);
        if(recvLen == SOCKET_ERROR)
        {
            Application->MessageBoxA("TCP接收数据错误", "TCP接收错误", MB_OK);
            Synchronize(EnableButton);
            Synchronize(UpdateLabelAndProgressColor);
            return;
        }
        pCmdInfo = (cmd_info*)buffer;
        if(pCmdInfo->magic != 0x12341234)
        {
            continue;
        }
        switch(pCmdInfo->cmd)
        {
            case CMD_REQUEST_FILE:
            {
                int read_len;
                char file_buf[4 * 1024];
                FILE *fp;
				int file_len;

                if(strcmp(pCmdInfo->filename, "update") == 0)
                {
                    fp = fopen(updateFilePath, "rb");
                    if(fp == NULL)
                    {
                        ShowMessage("打开升级文件错误");
                        pCmdInfo->filelen = -1;
                    }
                    else
                    {
                        fseek(fp, 0, SEEK_END);
						file_len = ftell(fp);
                        pCmdInfo->filelen = file_len;
						fseek(fp, 0, SEEK_SET);
                    }
                }
                else
                {
                    ShowMessage("请求文件错误");
                    pCmdInfo->filelen = -1;
                }

                pCmdInfo->cmd =  CMD_RESPONSE;
                if(send(sendSock, buffer, sizeof(buffer), 0) == SOCKET_ERROR)
                {
                    Application->MessageBoxA("发送文件信息错误", "错误", MB_OK | MB_ICONERROR);
                    closesocket(sendSock);
                    Synchronize(EnableButton);
                    Synchronize(UpdateLabelAndProgressColor);
                    return;
                }
		   		if(fp == NULL || pCmdInfo->filelen <= 0)
                {
					if(fp != NULL)
						fclose(fp);
                    continue;
                }
				totalSendLen = 0;
				while((read_len = fread(file_buf, 1, sizeof(file_buf), fp)) > 0)
                {
                    if(send(sendSock, file_buf, read_len, 0) == SOCKET_ERROR)
                    {
                        Application->MessageBoxA("发送升级文件错误", "错误", MB_OK | MB_ICONERROR);
                        closesocket(sendSock);
                        fclose(fp);
                        Synchronize(EnableButton);
                        Synchronize(UpdateLabelAndProgressColor);
                        return;
                    }
                    totalSendLen += read_len;
                    progress = (long long)totalSendLen * 99 / file_len;
                    Synchronize(UpdateProgress);
                }
                fclose(fp);
                
                break;
            }
            case CMD_UPDATE_FILNISH:
            {
                Synchronize(UpdateLable);
                Synchronize(EnableButton);
                progress = 100;
                Synchronize(UpdateProgress);
                closesocket(sendSock);
                return;
            }
            defalt:
            {
                ShowMessage("Device command don't supoort");
                break;
            }
        }
    }
}  
//---------------------------------------------------------------------------

