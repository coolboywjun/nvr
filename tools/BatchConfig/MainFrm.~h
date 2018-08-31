//---------------------------------------------------------------------------

#ifndef MainFrmH
#define MainFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <winsock.h>
#include "UDPThread.h"
#include "TCPThread.h"
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "MSCommLib_OCX.h"
#include <OleCtrls.hpp>
#include "YbCommDevice.h"

#define SUPPORT_DEVICE_MAX  16

//---------------------------------------------------------------------------
typedef struct _MacNode
{
    unsigned char mac[6];
    struct _MacNode *next;
}MacNode;
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TLabel *lblInfo;
    TButton *btnUpdate;
    TGroupBox *GroupBox1;
    TComboBox *ComboBox1;
    TLabel *Label1;
    TGroupBox *GroupBox2;
    TCheckBox *ckMAC;
    TCheckBox *ckHWID;
    TLabel *lblClientNum;
    TGroupBox *GroupBox3;
    TProgressBar *ProgressBar1;
    TRichEdit *RichEdit1;
    TGroupBox *GroupBox4;
    TComboBox *cbComm;
    TButton *btnOpenComm;
    TYbCommDevice *mscm1;
    TTimer *Timer1;
    TLabel *Label2;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall btnUpdateClick(TObject *Sender);
    void __fastcall btnOpenCommClick(TObject *Sender);
    void __fastcall Timer1Timer(TObject *Sender);
private:	// User declarations
    net_struct netInfo;
    TUDPThread *udpThread;
    TTCPServer *tcpServerThread;
    MacNode *pReqHead;
    MacNode *pReqTail;
    int reqListNodeNum;
    MacNode *pNoAckHead;
    MacNode *pNoAckTail;
    int noAckListNodeNum;
    MacNode *pResponseHead;
    int responseNum;
    TLabel *lblProgress;
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
    __fastcall TForm1::~TForm1();
    int __fastcall AnalyNetInfo(net_struct *pNetInfo);
    int __fastcall IsMacRepeat(net_struct *pNetInfo);
    int __fastcall IsRandomRepeat(net_struct *pNetInfo);
    int __fastcall BeginToScan();
    int __fastcall ShowProgressBar(net_struct *pNetInfo);
    bool __fastcall GetHostIP(char *ipStr);
    int __fastcall GetMacFromIp(char *ipStr, char *mac);
    void __fastcall SetEditText(AnsiString str);
    int __fastcall CommWrite(unsigned char *buff, int sendSize);
    int __fastcall CommRead(unsigned char *buff, int sendSize);
    unsigned char __fastcall CalcCheckSum(unsigned char *buff, int bufLen);
    int __fastcall TestCommDeviceConnect();
    int __fastcall RequestMac(int *requestNum);
    int __fastcall ReadNoUsedFile();
    int __fastcall WriteNoUsedFile();
    int __fastcall WriteUsedFile(const MacNode *pNode);
    int __fastcall RequestMacFromList(unsigned char *pMac);
    int __fastcall WriteToDevSuccess(unsigned char *pMac);
    int __fastcall RequestHwidFromList(unsigned char *pHwid, const unsigned char *mac);
    void __fastcall CopyNoAckToReqList();
    void __fastcall DeviceWriteMacResponse(char *pMac);
    void __fastcall TestSetBeginVariable();
    int clientDevNum;
    int finishDevNum;
    AnsiString strBuff;
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
enum
{
    COMM_DEVICE_TEST = 0X01,
    COMM_REQUEST_MAC = 0X02,
    COMM_RETURN_MAC = 0X03,
};
#endif



