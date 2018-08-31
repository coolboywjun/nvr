//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <winbase.h>

#include "MainFrm.h"


// 平台定义
#define DEV_PLATFORM_ZSYH_OPEN		(1<<0)
#define DEV_PLATFORM_JXJ_OPEN	            (1<<1)
#define DEV_PLATFORM_NVR_OPEN		(1<<2)
#define DEV_PLATFORM_ONVIF_OPEN		(1<<3)
#define DEV_PLATFORM_HXHT_OPEN		(1<<4)
#define DEV_PLATFORM_GB28181_OPEN	(1<<5)

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    WSADATA wsd;

    if (WSAStartup(MAKEWORD(2,2),&wsd)!=0)
	{
		return;//失败
	}
}
__fastcall TForm1::~TForm1()
{
    WSACleanup();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnUpdateFileClick(TObject *Sender)
{
    this->openDlg->FileName = ExtractFilePath(Application->ExeName) + "nvr_update.bin";
    if(openDlg->Execute())
    {
        this->editUpdateFile->Text = this->openDlg->FileName;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnCfgClick(TObject *Sender)
{
    this->openDlg->FileName = ExtractFilePath(Application->ExeName) + "cfg.bin";
    if(openDlg->Execute())
    {
        this->editCfg->Text = this->openDlg->FileName;
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    AnsiString path;
    AnsiString fileName;
    WIN32_FIND_DATA findFileData;
    path = ExtractFilePath(Application->ExeName) + "\*.*";
    //ShowMessage(ExtractFilePath(Application->ExeName));
    //ShowMessage(path.c_str());
    HANDLE hFind = ::FindFirstFile(path.c_str(), &findFileData);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        unsigned long maxVer = 0;
        unsigned long curVer;
        int ver[4];
        fileName = "";
        do
        {
            if(/*(findFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
                && */(strstr(findFileData.cFileName, "nvr_update_wdt")))
            {
                if(sscanf(findFileData.cFileName, "nvr_update_wdt_v%d.%d.%d.bin",
                       &ver[0], &ver[1], &ver[2]) == 3)
                {
                    curVer = (ver[0] << 16) | (ver[1] << 8) | ver[2];
                    if(maxVer < curVer)
                    {
                        maxVer = curVer;
                        fileName = findFileData.cFileName;
                    }
                }
            }
            if(!::FindNextFile(hFind, &findFileData))
                break;
        }while(1);
        FindClose(hFind);
        if(fileName.Length() != 0)
            this->editUpdateFile->Text = ExtractFilePath(Application->ExeName) + fileName;
        else
            this->editUpdateFile->Text = ExtractFilePath(Application->ExeName) + "nvr_update_wdt.bin";
    }
    else
    {
        this->editUpdateFile->Text = ExtractFilePath(Application->ExeName) + "nvr_update_wdt.bin";
    }
    //this->editCfg->Text = ExtractFilePath(Application->ExeName)+ "jxj_ipnc_cfg.bin";
    for(int i=0; i<SUPPORT_DEVICE_MAX; i++)
    {
        this->progressBar[i] = new TProgressBar(GroupBox2);
        this->progressBar[i]->Width = 100;
        this->progressBar[i]->Height = 18;
        this->progressBar[i]->Parent = GroupBox2;
        this->progressBar[i]->Position = 0;
        this->progressBar[i]->Smooth = true;
        this->progressBar[i]->Position = 0;

        this->lblProgress[i] = new TLabel(this->progressBar[i]);
        this->lblProgress[i]->AutoSize = false;
        this->lblProgress[i]->Width = 25;
        this->lblProgress[i]->Height = 12;
        this->lblProgress[i]->Parent = this->progressBar[i];
        this->lblProgress[i]->Left = 40;
        this->lblProgress[i]->Top = 2;
        this->lblProgress[i]->Transparent = true;
        //this->lblProgress[i]->Caption = "100%";

        this->lblMac[i] = new TLabel(GroupBox2);
        this->lblMac[i]->Width = 100;
        this->lblMac[i]->Height = 15;
        this->lblMac[i]->Parent = GroupBox2;
        this->lblMac[i]->AutoSize = false;
        //this->lblMac[i]->Caption = "12:34:56:78:90:ab";

        this->progressBar[i]->Left = this->lblMac[i]->Left + this->lblMac[i]->Width + 5;
        this->progressBar[i]->Top = 10 + (this->progressBar[i]->Height + 5) * (i);
        this->lblMac[i]->Left = 5;
        this->lblMac[i]->Top = this->progressBar[i]->Top;

        this->lblMac[i]->Hide();
        this->progressBar[i]->Hide();
        this->lblProgress[i]->Hide();
    }
    char ipStr[128];
    if(GetHostIP(ipStr))
    {
        AnsiString str = ipStr;
        lblInfo->Caption = "局域网IP:" + str;
        this->udpThread = new TUDPThread(0);
        this->tcpServerThread = new TTCPServer(0);
    }
    else
    {
        this->btnUpdateFile->Enabled = false;
        this->btnCfg->Enabled = false;
        this->btnUpdate->Enabled = false;
        this->ComboBox1->Enabled = false;
        this->lblInfo->Caption = "请设置192.168.1.*的网络地址";
    }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
    if(this->udpThread != NULL)
    {        
        TerminateThread((void*)this->udpThread->Handle, 0);
        delete this->udpThread;
    }
    if(this->tcpServerThread != NULL)
    {
        TerminateThread((void*)this->tcpServerThread->Handle, 0);
        delete this->tcpServerThread;
    }
    for(int i=0; i<SUPPORT_DEVICE_MAX; i++)
    {   
        if(this->lblProgress[i] != NULL)
        {
            delete this->lblProgress[i];
            this->lblProgress[i] = NULL;
        }
        if(this->progressBar[i] != NULL)
        {
            delete this->progressBar[i];
            this->progressBar[i] = NULL;
        }
        if(this->lblMac[i] != NULL)
        { 
            delete this->lblMac[i];
            this->lblMac[i] = NULL;
        }
    }
}
//---------------------------------------------------------------------------
int __fastcall TForm1::ShowProgressBar(net_struct *pNetInfo)
{
    if(pNetInfo->dev_num > sizeof(this->progressBar)/sizeof(TProgressBar *))
    {
        ShowMessage("不能创建这么多的进度条");
        return false;
    }
    for(int i=0; i<sizeof(this->progressBar)/sizeof(TProgressBar *); i++)
    {
        //if(this->progressBar[i]->Visible == false)
        //    break;
        this->progressBar[i]->Hide();
        this->lblProgress[i]->Hide();
        this->lblMac[i]->Hide();
    }
    for(int i=0; i<pNetInfo->dev_num; i++)
    {
        this->progressBar[i]->Position = 0;
        this->lblProgress[i]->Color = clBtnFace;
        this->lblMac[i]->Color = clBtnFace;
        this->progressBar[i]->Show();
        this->lblProgress[i]->Show();
    }
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::BatchUpdate()
{
    FILE *kernel, *cfg= NULL;
    char ipStr[128];

    if(!this->GetHostIP(ipStr))
        return;
    this->lblInfo->Tag = 0;

    if(this->editUpdateFile->Text == "")
    {
        Application->MessageBoxA("请选择升级文件的路径!!!", "错误", MB_OK);
        return;
    }

    kernel = fopen(this->editUpdateFile->Text.c_str(), "rb");
    if(kernel == NULL)
    {
        if(kernel == NULL)
            Application->MessageBoxA("升级文件路径错误", "文件路径错误", MB_OK | MB_ICONERROR);

        if(kernel != NULL)
            fclose(kernel);
        if(cfg != NULL)
            fclose(cfg);
        return;
    }
    fclose(kernel);
    
    this->btnUpdateFile->Enabled = false;
    this->btnCfg->Enabled = false;
    this->btnUpdate->Enabled = false;
    this->ComboBox1->Enabled = false;

    int ret;
    int tryNum = 0;
    int clientNum;
    this->lblClientNum->Caption = "";
    this->lblInfo->Caption = "";
    this->Update();
    //this->udpThread = new TUDPThread(1);
    do
    {
        this->udpThread->ResetClientNum();
        //this->udpThread->Resume();
        BeginToScan();
        clientNum = this->udpThread->GetClientNum();
        //this->udpThread->Suspend();
        this->lblClientNum->Caption = "扫描到设备数量：" + IntToStr(clientNum);
        this->Update();
        if(clientNum == 0)
        {
            ret = false;
            break;
        }
        else if(clientNum != this->ComboBox1->ItemIndex + 1)
        {
            AnsiString str;
            str = "选择的设备数量(" + IntToStr(this->ComboBox1->ItemIndex + 1)
                    + ")与实际接入局域网设备数量(" + IntToStr(clientNum) + ")不符合,是否进行升级?";
            if(Application->MessageBoxA(str.c_str(),
                    "设备数量错误", MB_OKCANCEL | MB_ICONQUESTION) == IDCANCEL)
            {
                ret = false;
                break;
            }
        }
        this->udpThread->GetNetInfo(&netInfo);
        netInfo.dev_num = clientNum;
        netInfo.server_addr.S_un.S_addr = inet_addr(ipStr);
        netInfo.server_port = TCP_SERVER_PORT;
        int analyStatus = this->AnalyNetInfo(&netInfo);
        if(analyStatus == -1)
        {
            ret = false;
            break;
        }
        else if(analyStatus == 0)
        {
            // IP地址和random都重复,需要重新进行扫描
            this->lblInfo->Caption = "IP地址和random都重复,重新进行扫描.";
            this->Update();
            continue;
        }
        else
        {
            ret = true;
            break;
        }
    }while(tryNum++ < 5);

    if(ret == true)
    {
        this->ShowProgressBar(&netInfo);
        this->tcpServerThread->ResetSetNo();
        this->lblInfo->Caption = "扫描成功,正在升级中...";
        this->Update();
        this->clientDevNum = clientNum;
        this->finishDevNum = 0;
        this->udpThread->SendBroadCastNetInfo(&netInfo);
        this->TimerTCP->Enabled = true;
        //成功扫描到了设备,所以需要等待到设备升级后才能再次使能按键
        // 或者超时60s后仍然有设备没有升级成功将重新使能按键
    }
    else
    {
        if(tryNum >= 5)
        {
            this->lblInfo->Caption = "扫描5次,但是IP和MAC总是重复!!!";
        }
        this->btnUpdateFile->Enabled = true;
        this->btnCfg->Enabled = true;
        this->btnUpdate->Enabled = true;
        this->ComboBox1->Enabled = true;
    }
    //if(this->udpThread != NULL)
    //{        
    //    TerminateThread((void*)this->udpThread->Handle, 0);
    //    delete this->udpThread;
    //}
}

//---------------------------------------------------------------------------
void __fastcall TForm1::btnUpdateClick(TObject *Sender)
{
    BatchUpdate();
}
//---------------------------------------------------------------------------
int __fastcall TForm1::IsMacRepeat(net_struct *pNetInfo)
{
    for(int i=0; i<pNetInfo->dev_num; i++)
    {
        for(int j=i+1; j<pNetInfo->dev_num; j++)
        {
            if(memcmp(pNetInfo->node[i].mac, pNetInfo->node[j].mac,
                    sizeof(pNetInfo->node[i].mac)) == 0)
            {
                return 1;   // MAC repeat
            }
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
int __fastcall TForm1::IsRandomRepeat(net_struct *pNetInfo)
{
    for(int i=0; i<pNetInfo->dev_num; i++)
    {
        for(int j=i+1; j<pNetInfo->dev_num; j++)
        {
            if(pNetInfo->node[i].random == pNetInfo->node[j].random)
            {
                return 1;   // MAC repeat
            }
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
int __fastcall TForm1::AnalyNetInfo(net_struct *pNetInfo)
{
    if(pNetInfo == NULL)
        return -1;
    int macRepeat = IsMacRepeat(pNetInfo);
    int randomRepeat = IsRandomRepeat(pNetInfo);
    if(macRepeat && randomRepeat)
    {
        // MAC和random都重复,我们需要重新发送广播包再进行扫描
        return 0;
    }
    else if(macRepeat && randomRepeat == 0)
    {
        const unsigned char mac[6] = {0x00, 0x05, 0x3B, 0xF7, 0x33, 0x11};
        for(int i=0; i<pNetInfo->dev_num; i++)
        {
            memcpy(pNetInfo->node[i].mac, mac, sizeof(pNetInfo->node[i].mac));
            pNetInfo->node[i].mac[5] += i;
        }
        pNetInfo->use_which_compare = 1; //设备采用random进行比对来选择自己的MAC地址
    }
    else
    {
        pNetInfo->use_which_compare = 0; // 设备采用MAC进行比对
    }
    return 1;
}
//---------------------------------------------------------------------------
int __fastcall TForm1::BeginToScan()
{
    if(this->udpThread->SendBroadCastPacket() == SOCKET_ERROR)
    {
         this->lblInfo->Caption = "发送UDP广播包失败";
         return false;
    }
    Sleep(3000);

    return true;
}
bool __fastcall TForm1::GetHostIP(char *ipStr)
{
    struct hostent *P_Host;
    char Computer_Name[128] = "";

    gethostname(Computer_Name, sizeof(Computer_Name));//获取指定计算机的名字
    if(Computer_Name[0] == '\0')
    {
        MessageBox(NULL, "本机网络故障，无法接入局域网，请联系网络管理员","提示",MB_ICONASTERISK);
        return false;
    }

    P_Host = gethostbyname(Computer_Name);
    if(P_Host == NULL)
    {
        MessageBox(NULL, "本机网络故障，无法获取IP，请联系管理员","提示",MB_ICONASTERISK);
        return false;
    }
    char **pptr = P_Host->h_addr_list;
    for(; *pptr != NULL; pptr++)
    {
        //ShowMessage(inet_ntoa(*(in_addr *)*pptr));
        if(strncmp(inet_ntoa(*(in_addr *)*pptr), "192.168.1.", strlen("192.168.1.")) == 0)
        {
            strcpy(ipStr, inet_ntoa(*((in_addr *)P_Host->h_addr)));
            return true;
        }
    }
    return false;
}
int __fastcall TForm1::GetMacFromIp(char *ipStr, char *mac)
{
    if(ipStr == NULL || mac == NULL)
        return false;
    for(int i=0; i<netInfo.dev_num; i++)
    {
        if(strcmp(inet_ntoa(netInfo.node[i].ip_addr), ipStr) == 0)
        {
            memcpy(mac, netInfo.node[i].mac, sizeof(netInfo.node[i].mac));
            return true;
        }
    }
    return false;
}

void __fastcall TForm1::TimerTCPTimer(TObject *Sender)
{
    this->TimerTCP->Enabled = false;
    this->btnUpdateFile->Enabled = true;
    this->btnCfg->Enabled = true;
    this->btnUpdate->Enabled = true;
    this->ComboBox1->Enabled = true;
    this->lblInfo->Caption = "设备升级超时,请重新升级!!!";
}

//---------------------------------------------------------------------------

void __fastcall TForm1::btnHelpClick(TObject *Sender)
{
    MessageBox(this->Handle, "使用说明:\n"
                     "\t1,启动所有设备\n"
                     "\t2,选择局域网设备数量\n"
                     "\t3,点击\"开始批量\"按钮\n"
                     "\t4,等待所有设备升级完成", "使用说明", MB_OK);
}
//---------------------------------------------------------------------------



