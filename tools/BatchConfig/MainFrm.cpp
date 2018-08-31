//---------------------------------------------------------------------------

#include <vcl.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "MainFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "MSCommLib_OCX"
#pragma link "YbCommDevice"
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
	pReqHead = NULL;
	pReqTail = NULL;
	pNoAckHead = NULL;
	pNoAckTail = NULL;
	reqListNodeNum = 0;
	noAckListNodeNum = 0;
	pResponseHead = NULL;
	responseNum = 0;
}
__fastcall TForm1::~TForm1()
{
	WSACleanup();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
	char ipStr[128];
	if (GetHostIP(ipStr))
	{
		AnsiString str = ipStr;
		lblInfo->Caption = "局域网IP:" + str;
		this->udpThread = new TUDPThread(0);
		this->tcpServerThread = new TTCPServer(0);
	}
	else
	{
		this->btnUpdate->Enabled = false;
		this->ComboBox1->Enabled = false;
		//this->ckHWID->Enabled = false;
		this->ckMAC->Enabled = false;
		this->btnOpenComm->Enabled = false;
		AnsiString str = "请设置192.168.1.*的网络地址";
		this->lblInfo->Caption = str;
		this->SetEditText(str);
	}
	for (int i=0; i<16; i++)
	{
		this->cbComm->Items->Add("COM" + IntToStr(i+1));
	}
	this->cbComm->ItemIndex = 0;
	ReadNoUsedFile();
	lblProgress = new TLabel(this->ProgressBar1);
	lblProgress->Parent = this->ProgressBar1;
	lblProgress->AutoSize = false;
	lblProgress->Width = 30;
	lblProgress->Height = 15;
	lblProgress->Top = 5;
	lblProgress->Left = (this->ProgressBar1->Width - lblProgress->Width) / 2;
	//lblProgress->Caption = "100%";
	lblProgress->Transparent = true;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (this->udpThread != NULL)
	{
		TerminateThread((void*)this->udpThread->Handle, 0);
		delete this->udpThread;
	}
	if (this->tcpServerThread != NULL)
	{
		TerminateThread((void*)this->tcpServerThread->Handle, 0);
		delete this->tcpServerThread;
	}
	if (this->mscm1->Active == true)
		this->mscm1->Active = false;
	WriteNoUsedFile();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::btnUpdateClick(TObject *Sender)
{
	this->ProgressBar1->Position = 0;
	this->lblProgress->Caption = "";
	this->lblProgress->Color = clBtnFace;
	this->lblInfo->Caption = "";
	MacNode *pNode = pResponseHead;
	while (pNode != NULL)
	{
		pNode = pNode->next;
		delete pNode;
	}
	pResponseHead = NULL;
	responseNum = 0;
	if (this->mscm1->Active == false)
	{
		btnOpenCommClick(NULL);
		if (this->mscm1->Active == false)
		{
			return;
		}
	}

	if (!this->ckHWID->Checked && !this->ckMAC->Checked)
	{
		Application->MessageBoxA("请选择设置项", "错误", MB_OK | MB_ICONERROR);
		return;
	}

	TestSetBeginVariable();// 做调试使用,非调试时关闭

	char ipStr[128];
	if (!this->GetHostIP(ipStr))
		return;
	this->lblInfo->Tag = 0;

	this->btnUpdate->Enabled = false;
	this->ComboBox1->Enabled = false;
	//this->ckHWID->Enabled = false;
	this->ckMAC->Enabled = false;
	this->btnOpenComm->Enabled = false;

	int ret;
	int tryNum = 0;
	int clientNum;
	this->lblClientNum->Caption = "";
	this->lblInfo->Caption = "";
	this->Update();
	CopyNoAckToReqList();
	do
	{
		this->udpThread->ResetClientNum();
		this->udpThread->Resume();
		BeginToScan();
		this->udpThread->Suspend();
		clientNum = this->udpThread->GetClientNum();
		this->lblClientNum->Caption = "扫描到设备数量：" + IntToStr(clientNum);

		this->Update();
		if (clientNum == 0)
		{
			ret = false;
			break;
		}
		else if (clientNum != this->ComboBox1->ItemIndex + 1)
		{
			AnsiString str;
			str = "选择的设备数量(" + IntToStr(this->ComboBox1->ItemIndex + 1)
				  + ")与实际接入局域网设备数量(" + IntToStr(clientNum) + ")不符合,是否进行升级?";
			if (Application->MessageBoxA(str.c_str(),
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
		if (analyStatus == -1)
		{
			ret = false;
			break;
		}
		else if (analyStatus == 0)
		{
			// IP地址和random都重复,需要重新进行扫描
			AnsiString str = "IP地址和random都重复,重新进行扫描.";
			this->lblInfo->Caption = str;
			this->SetEditText(str);
			this->Update();
			continue;
		}
		else
		{
			ret = true;
			break;
		}
	}while (tryNum++ < 5);

	if (ret == true)
	{
		int requestNum = clientNum - reqListNodeNum;
		if(requestNum > 0)
		{
			int requestNumBak = requestNum;
			int result = this->RequestMac(&requestNum);
			if (result == false || requestNum != requestNumBak)
			{
				this->btnUpdate->Enabled = true;
				this->ComboBox1->Enabled = true;
				//this->ckHWID->Enabled = true;
				this->ckMAC->Enabled = true;
				this->btnOpenComm->Enabled = true;
				return;
			}
		}
		this->tcpServerThread->ResetSetNo();
		AnsiString str = "扫描成功,正在设置中...";
		this->lblInfo->Caption = str;
		this->SetEditText(str);
		this->Update();
		this->clientDevNum = clientNum;
		this->finishDevNum = 0;
		this->udpThread->SendBroadCastNetInfo(&netInfo);
		//成功扫描到了设备,所以需要等待到设备升级后才能再次使能按键
		this->Timer1->Interval = 10000;
		this->Timer1->Enabled = true;
	}
	else
	{
		if (tryNum >= 5)
		{
			AnsiString str = "扫描5次,但是IP和MAC总是重复!!!";
			this->lblInfo->Caption = str;
			this->SetEditText(str);
		}
		this->btnUpdate->Enabled = true;
		this->ComboBox1->Enabled = true;
		//this->ckHWID->Enabled = true;
		this->ckMAC->Enabled = true;
		this->btnOpenComm->Enabled = true;
	}
}
//---------------------------------------------------------------------------
int __fastcall TForm1::IsMacRepeat(net_struct *pNetInfo)
{
	for (int i=0; i<pNetInfo->dev_num; i++)
	{
		for (int j=i+1; j<pNetInfo->dev_num; j++)
		{
			if (memcmp(pNetInfo->node[i].mac, pNetInfo->node[j].mac,
					   sizeof(pNetInfo->node[i].mac)) == 0)
			{
				return 1;	// MAC repeat
			}
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
int __fastcall TForm1::IsRandomRepeat(net_struct *pNetInfo)
{
	for (int i=0; i<pNetInfo->dev_num; i++)
	{
		for (int j=i+1; j<pNetInfo->dev_num; j++)
		{
			if (pNetInfo->node[i].random == pNetInfo->node[j].random)
			{
				return 1;	// MAC repeat
			}
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
int __fastcall TForm1::AnalyNetInfo(net_struct *pNetInfo)
{
	if (pNetInfo == NULL)
		return -1;
	int macRepeat = IsMacRepeat(pNetInfo);
	int randomRepeat = IsRandomRepeat(pNetInfo);
	if (macRepeat && randomRepeat)
	{
		// MAC和random都重复,我们需要重新发送广播包再进行扫描
		return 0;
	}
	else if (macRepeat && randomRepeat == 0)
	{
		const unsigned char mac[6] = {0x00, 0x05, 0x3B, 0xF7, 0x33, 0x11};
		for (int i=0; i<pNetInfo->dev_num; i++)
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
	if (this->udpThread->SendBroadCastPacket() == SOCKET_ERROR)
	{
		AnsiString str = "发送UDP广播包失败";
		this->lblInfo->Caption = str;
		this->SetEditText(str);
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
	if (Computer_Name[0] == '\0')
	{
		MessageBox(NULL, "本机网络故障，无法接入局域网，请联系网络管理员","提示",MB_ICONASTERISK);
		return false;
	}

	P_Host = gethostbyname(Computer_Name);
	if (P_Host == NULL)
	{
		MessageBox(NULL, "本机网络故障，无法获取IP，请联系管理员","提示",MB_ICONASTERISK);
		return false;
	}
	char **pptr = P_Host->h_addr_list;
	for (; *pptr != NULL; pptr++)
	{
		//ShowMessage(inet_ntoa(*(in_addr *)*pptr));
		if (strncmp(inet_ntoa(*(in_addr *)*pptr), "192.168.1.", strlen("192.168.1.")) == 0)
		{
			strcpy(ipStr, inet_ntoa(*((in_addr *)P_Host->h_addr)));
			return true;
		}
	}
	return false;
}
int __fastcall TForm1::GetMacFromIp(char *ipStr, char *mac)
{
	if (ipStr == NULL || mac == NULL)
		return false;
	for (int i=0; i<sizeof(netInfo.dev_num); i++)
	{
		if (strcmp(inet_ntoa(netInfo.node[i].ip_addr), ipStr) == 0)
		{
			memcpy(mac, netInfo.node[i].mac, sizeof(netInfo.node[i].mac));
			return true;
		}
	}
	return false;
}
void __fastcall TForm1::SetEditText(AnsiString str)
{
	AnsiString text;
	text = DateToStr(Date()) + " " + TimeToStr(Time()) + ": " + str;
	RichEdit1->Lines->Add(text);
	RichEdit1-> Perform(EM_SCROLLCARET,0,0);
	RichEdit1-> SelStart   =   RichEdit1-> Lines-> Text.Length();
	RichEdit1-> SelLength   =   0;

	FILE *fp;
	AnsiString path = ExtractFilePath(Application->ExeName) + "Log.txt";
	fp = fopen(path.c_str(), "a+");
	if(fp != NULL)
	{
		fputs(text.c_str(), fp);
		fputs("\r\n", fp);
		fclose(fp);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::btnOpenCommClick(TObject *Sender)
{
	if (this->mscm1->Active == true)
	{
		this->mscm1->Active = false;
		this->btnOpenComm->Caption = "打开";
		this->SetEditText("串口已经关闭");
	}
	else
	{
		try
		{
			this->mscm1->PortNo = this->cbComm->ItemIndex + 1;
			this->mscm1->Active = true;
		}
		catch (...)
		{
			Application->MessageBoxA("串口不存在或者已经打开", "错误", MB_OK | MB_ICONERROR);
			return;
		}
		if (this->TestCommDeviceConnect() == false)
		{
			this->mscm1->Active = false;
			this->btnOpenComm->Caption = "打开";
			this->SetEditText("MAC设备不存在");
		}
		else
		{
			this->btnOpenComm->Caption = "关闭";
			this->SetEditText("已连接MAC设备");
		}
	}
}
//---------------------------------------------------------------------------
int __fastcall TForm1::CommWrite(unsigned char *buff, int sendSize)
{
	if (this->mscm1->Active == false)
	{
		return -1;
	}
	return this->mscm1->Write(buff, sendSize);
}
int __fastcall TForm1::CommRead(unsigned char *buff, int readSize)
{
	int tryNum = 0;
	int readLen;
	int hasRead = 0;
	if (this->mscm1->Active == false)
	{
		return -1;
	}

	do
	{
		readLen = this->mscm1->Read(buff + hasRead, readSize - hasRead);
		if (readLen == 0)
		{
			Sleep(500);
		}
		else
		{
			hasRead += readLen;
		}
	}while (tryNum++ < 10 && hasRead < readSize);
	return hasRead;
}
unsigned char __fastcall TForm1::CalcCheckSum(unsigned char *buff, int bufLen)
{
	unsigned char checkSum = 0;

	for (int i=0; i<bufLen; i++)
	{
		checkSum += buff[i];
	}
	return checkSum;
}
int __fastcall TForm1::TestCommDeviceConnect()
{
	unsigned char  buff[16];
	unsigned char readBuff[16];

	memset(buff, 0, sizeof(buff));
	buff[0] = 0xaa;
	buff[1] = 0x55;
	buff[2] = COMM_DEVICE_TEST;
	strcpy((char*)&buff[3], "connect");
	buff[15] = CalcCheckSum(&buff[2], 13);
	if (CommWrite(buff, sizeof(buff)) == 0)
	{
		Application->MessageBoxA("写测试命令失败", "串口错误", MB_OK | MB_ICONERROR);
		return false;
	}
	if (CommRead(readBuff, sizeof(readBuff)) != sizeof(readBuff)
		|| (memcmp(readBuff, buff, sizeof(buff)) != 0))
	{
		Application->MessageBoxA("无法与MAC设备正常通信", "错误", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}
int __fastcall TForm1::RequestMac(int *requestNum)
{
	unsigned char buff[16];
	int readLen;
	unsigned int returnNum;

	if(*requestNum < 0)
		return false;
	if(*requestNum == 0)
		return true;
	// 申请内存
	MacNode *pNodeHead = NULL;
	MacNode *pNode;
	for (int i=0; i<*requestNum; i++)
	{
		pNode = new MacNode;
		if (pNode == NULL)
		{
			pNode = pNodeHead;
			while (pNode != NULL)
			{
				MacNode *pTmpNode = pNode;
				pNode = pNode->next;
				delete pTmpNode;
			}
			return false;
		}
		if (pNodeHead == NULL)
		{
			pNodeHead = pNode;
			pNode->next = NULL;
		}
		else
		{
			pNode->next = pNodeHead;
			pNodeHead = pNode;
		}
	}
	pNode = pNodeHead;
	while (pNode != NULL)
	{
		pNode = pNode->next;
	}
	do
	{
		// 申请到需要的内存后发送请求MAC命令
		memset(buff, 0, sizeof(buff));
		buff[0] = 0xaa;
		buff[1] = 0x55;
		buff[2] = COMM_REQUEST_MAC;
		buff[3] = *requestNum & 0xff;
		buff[4] = (*requestNum >> 8) & 0xff;
		buff[15] = CalcCheckSum(&buff[2], 13);
		CommWrite(buff, sizeof(buff));

		memset(buff, 0, sizeof(buff));
		readLen = CommRead(buff, sizeof(buff));
		if (readLen != sizeof(buff))
		{
			Application->MessageBoxA("读取串口数据错误", "串口错误", MB_OK | MB_ICONERROR);
			break;
		}
		unsigned char checkSum = CalcCheckSum(&buff[2], 13);
		if (checkSum != buff[15])
		{
			Application->MessageBoxA("检验和错误", "串口错误", MB_OK | MB_ICONERROR);
			break;
		}
		returnNum = buff[3] | (buff[4] << 8);
		if (returnNum == 0)
		{
			AnsiString str = "MAC设备已经没有MAC地址可以申请了,请联系MAC地址管理员";
			this->SetEditText(str);
			this->lblInfo->Caption = str;
			break;
		}
		else if (returnNum < (unsigned int)*requestNum)
		{
			AnsiString str = "申请的MAC地址已不足,请联系MAC地址管理员";
			this->SetEditText(str);
		}

		pNode = pNodeHead;
		while (pNode != NULL)
		{
			pNode = pNode->next;
		}
		unsigned long u32Mac = (buff[8] << 16) | (buff[9] << 8) | buff[10];
////////////////////////////////////////
//		returnNum = *requestNum;
//		buff[5] = 0xaa;
//		buff[6] = 0xbb;
//		buff[7] = 0xcc;
//		u32Mac = 0x00123456UL;
////////////////////////////////////////
		pNode = pNodeHead;
		for (unsigned int i=0; i<returnNum && pNode != NULL; i++)
		{
			pNode->mac[0] = buff[5];
			pNode->mac[1] = buff[6];
			pNode->mac[2] = buff[7];
			pNode->mac[3] = (u32Mac >> 16) & 0xff;
			pNode->mac[4] = (u32Mac >> 8) & 0xff;
			pNode->mac[5] = (u32Mac >> 0) & 0xff;
			u32Mac++;
			if (pReqHead == NULL)
			{
				pReqHead = pNode;
				pReqTail = pNode;
			}
			else
			{
				pReqTail->next = pNode;
				pReqTail = pNode;
			}
			pNode = pNode->next;
			pReqTail->next = NULL;
			reqListNodeNum++;
		}
		AnsiString str;
		str.printf("已申请MAC: %02x:%02x:%02x:%02x:%02x:%02x - %02x:%02x:%02x:%02x:%02x:%02x",
				   buff[5], buff[6], buff[7], buff[8], buff[9], buff[10],
				   pReqTail->mac[0], pReqTail->mac[1], pReqTail->mac[2],
				   pReqTail->mac[3], pReqTail->mac[4], pReqTail->mac[5]);
		this->SetEditText(str);
		return true;
	}while (0);

	//发生了错误,释放内存.
	pNode = pNodeHead;
	while (pNode != NULL)
	{
		MacNode *pTmpNode = pNode;
		pNode = pNode->next;
		delete pTmpNode;
	}
	return false;
}
int __fastcall TForm1::ReadNoUsedFile()
{
	FILE *fp;
	char lineStr[128];
	MacNode node;
	MacNode *pMacNode;
	int readNum = 0;

	AnsiString path = ExtractFilePath(Application->ExeName) + "noUseMAC.txt";
	fp = fopen(path.c_str(), "r");
	if (fp == NULL)
		return 0;
	while (fgets(lineStr, sizeof(lineStr), fp) != NULL)
	{
		if (strlen(lineStr) < 17)
			continue;
		if (sscanf(lineStr, "%02x:%02x:%02x:%02x:%02x:%02x",
				   &node.mac[0], &node.mac[1], &node.mac[2],
				   &node.mac[3], &node.mac[4], &node.mac[5]) != 6)
		{
			continue;
		}
		pMacNode = new MacNode;
		memcpy(pMacNode, &node, sizeof(node));
		if (pReqHead == NULL)
		{
			pReqHead = pMacNode;
			pReqTail = pMacNode;
		}
		else
		{
			pReqTail->next = pMacNode;
			pReqTail = pMacNode;
		}
		pReqTail->next = NULL;
		reqListNodeNum++;
		readNum++;
	}
	fclose(fp);
	DeleteFile(path.c_str());
	return readNum;
}
int __fastcall TForm1::WriteNoUsedFile()
{
	FILE *fp;
	MacNode *pNode;
	int writeNum = 0;

	if (pReqHead == NULL && pNoAckHead == NULL)
	{
		return 0;
	}
	AnsiString path = ExtractFilePath(Application->ExeName) + "noUseMAC.txt";
	fp = fopen(path.c_str(), "w");
	if (fp == NULL)
	{
		return 0;
	}
	pNode = pReqHead;
	while (pNode != NULL)
	{
		fprintf(fp, "%02x:%02x:%02x:%02x:%02x:%02x\r\n",
				pNode->mac[0], pNode->mac[1], pNode->mac[2],
				pNode->mac[3], pNode->mac[4], pNode->mac[5]);
		pNode = pNode->next;
		writeNum++;
	}
	pNode = pNoAckHead;
	while (pNode != NULL)
	{
		fprintf(fp, "%02x:%02x:%02x:%02x:%02x:%02x\r\n",
				pNode->mac[0], pNode->mac[1], pNode->mac[2],
				pNode->mac[3], pNode->mac[4], pNode->mac[5]);
		pNode = pNode->next;
		writeNum++;
	}
	fclose(fp);
	return writeNum;
}
int __fastcall TForm1::WriteUsedFile(const MacNode *pNode)
{
	FILE *fp;

	if (pNode == NULL)
		return false;
	AnsiString path = ExtractFilePath(Application->ExeName) + "usedMAC.txt";
	fp = fopen(path.c_str(), "a+");
	if (fp == NULL)
	{
		return 0;
	}
	fprintf(fp, "%02x:%02x:%02x:%02x:%02x:%02x\r\n",
			pNode->mac[0], pNode->mac[1], pNode->mac[2],
			pNode->mac[3], pNode->mac[4], pNode->mac[5]);
	fclose(fp);
	return true;
}
int __fastcall TForm1::RequestMacFromList(unsigned char *pMac)
{
	MacNode *pTmpNode;
	if (pMac == NULL)
		return false;
	if(!this->ckMAC->Checked)
		return false;
	if (pReqHead == NULL)
	{
		Application->MessageBoxA("从列表中申请MAC地址失败!", "ERROR", MB_OK | MB_ICONERROR);
		return false;
	}
	pTmpNode = pReqHead;
	pReqHead = pReqHead->next;
	if (pReqHead == NULL)
	{
		pReqTail = NULL;
	}

	if (pNoAckHead == NULL)
	{
		pNoAckHead = pTmpNode;
		pNoAckTail = pTmpNode;
	}
	else
	{
		pNoAckTail->next = pTmpNode;
		pNoAckTail = pTmpNode;
	}
	pNoAckTail->next = NULL;
	memcpy(pMac, pTmpNode->mac, sizeof(pTmpNode->mac));
	reqListNodeNum--;
	noAckListNodeNum++;
	return true;
}
int __fastcall TForm1::WriteToDevSuccess(unsigned char *pMac)
{
	MacNode *pNode, *pTmpNode;

	if (pNoAckHead == NULL)
	{
		Application->MessageBoxA("ERROR: noAckHead链表错误", "ERROR", MB_OK | MB_ICONERROR);
		return false;
	}
	if (memcmp(pMac, pNoAckHead->mac, sizeof(pNoAckHead->mac)) == 0)
	{
		pNode = pNoAckHead;
		pNoAckHead = pNoAckHead->next;
		if (pNoAckHead == NULL)
		{
			pNoAckTail = NULL;
		}
		delete pNode;
		noAckListNodeNum--;
	}
	else
	{
		pNode = pNoAckHead;
		while (pNode->next != NULL)
		{
			if (memcmp(pMac, pNode->next->mac, sizeof(pNode->next->mac)) == 0)
			{
				pTmpNode = pNode->next;
				pNode->next = pTmpNode->next;
				if (pTmpNode == pNoAckTail)
					pNoAckTail = pNode;
				delete pTmpNode;
				noAckListNodeNum--;
				break;
			}
			pNode = pNode->next;
		}
	}
	AnsiString str;
	str.printf("成功写入MAC: %02x:%02x:%02x:%02x:%02x:%02x",
			   pMac[0], pMac[1], pMac[2],
			   pMac[3], pMac[4], pMac[5]);
	this->SetEditText(str);
	return true;
}
int __fastcall TForm1::RequestHwidFromList(unsigned char *pHwid, const unsigned char *mac)
{
	int len;
	int szMac[6];
	char *pStr = (char *)pHwid;
	time_t curTimer;
	struct tm *curTm;
       if(!this->ckHWID->Checked)
	   	return false;
	memset(pHwid, 0, 48);
	szMac[0] = mac[0];
	szMac[1] = mac[1];
	szMac[2] = mac[2];
	szMac[3] = mac[3];
	szMac[4] = mac[4];
	szMac[5] = mac[5];
	strcpy(pStr, "CNVR");
	curTimer = time(NULL);
	curTm = localtime(&curTimer);
	sprintf(pStr, "%s%02d%02d%02d%02x%02x%02x", 
				"CNVR",
				curTm->tm_year-100, curTm->tm_yday / 7, curTm->tm_wday,
				szMac[3], szMac[4], szMac[5]);
	return true;
}
void __fastcall TForm1::CopyNoAckToReqList()
{
	if (pNoAckHead == NULL)
		return;
	if (pReqHead == NULL)
	{
		pReqHead = pNoAckHead;
		pReqTail = pNoAckTail;
		reqListNodeNum = noAckListNodeNum;
	}
	else
	{
		pReqTail->next = pNoAckHead;
		pReqTail = pNoAckTail;
		reqListNodeNum += noAckListNodeNum;
	}
	pNoAckHead = NULL;
	pNoAckTail = NULL;
	noAckListNodeNum = 0;
}
void __fastcall TForm1::DeviceWriteMacResponse(char *pMac)
{
	MacNode *pNode;
	pNode = new MacNode;
	if (pNode == NULL)
	{
		this->SetEditText("申请设备回应节点内存失败");
		return;
	}
	memcpy(pNode->mac, pMac, sizeof(pNode->mac));
	pNode->next = pResponseHead;
	pResponseHead = pNode;
	responseNum++;
	this->ProgressBar1->Position = responseNum * 100 / clientDevNum;
	this->lblProgress->Caption = IntToStr(this->ProgressBar1->Position) + "%";
	AnsiString str;
	str.printf("已经成功写入设备: %d", responseNum);
	lblInfo->Caption = str;
	if (responseNum == clientDevNum)
	{
		pNode = pResponseHead;
		while (pNode != NULL)
		{
			MacNode *pTmpNode = pNode;
			WriteToDevSuccess(pNode->mac);
			WriteUsedFile(pNode);
			pNode = pNode->next;
			delete pTmpNode;
			responseNum--;
		}
		pResponseHead = NULL;
		this->Timer1->Enabled = false;
		this->btnUpdate->Enabled = true;
		this->ComboBox1->Enabled = true;
		//this->ckHWID->Enabled = true;
		this->ckMAC->Enabled = true;
		this->btnOpenComm->Enabled = true;
		AnsiString str;
		str.printf("已成功设置%d台设备", clientDevNum);
		this->SetEditText(str);
		TestSetBeginVariable();		// 调试时使用,不调试时关闭
	}
}

void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
	AnsiString str;
	str = "设备设置超时,现在连接的设备全部需要重新设置";
	SetEditText(str);
	this->lblInfo->Caption = str;
	this->lblProgress->Color = clRed;
	this->Timer1->Enabled = false;
	this->btnUpdate->Enabled = true;
	this->ComboBox1->Enabled = true;
	//this->ckHWID->Enabled = true;
	this->ckMAC->Enabled = true;
	this->btnOpenComm->Enabled = true;
}
// 检查点击"批量设备"按键前各个参数是否正确
// 这个函数的功能主要是做调试使用
void __fastcall TForm1::TestSetBeginVariable()
{
#if 0
	AnsiString str;
	str.printf("pReqHead: %p, pReqTail: %p, reqListNodeNum: %d",
			   pReqHead, pReqTail, reqListNodeNum);
	ShowMessage(str);
	str.printf("pNoAckHead: %p, pNoAckTail: %p, noAckListNodeNum: %d",
			   pNoAckHead, pNoAckTail, noAckListNodeNum);
	ShowMessage(str);
	str.printf("pResponseHead: %p, responseNum: %d", pResponseHead, responseNum);
	ShowMessage(str);
#endif
}
//---------------------------------------------------------------------------


