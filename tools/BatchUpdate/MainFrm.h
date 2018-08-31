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

#define SUPPORT_DEVICE_MAX  16
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TLabel *Label1;
        TEdit *editUpdateFile;
    TOpenDialog *openDlg;
    TButton *btnUpdate;
    TLabel *lblInfo;
    TLabel *lblClientNum;
    TGroupBox *GroupBox2;
    TLabel *Label3;
    TGroupBox *GroupBox3;
    TLabel *Label4;
    TComboBox *ComboBox1;
    TTimer *TimerTCP;
    TLabel *Label10;
    TEdit *editCfg;
    TButton *btnCfg;
        TButton *btnUpdateFile;
    TButton *btnHelp;
    TLabel *lblConfig;
    void __fastcall btnUpdateFileClick(TObject *Sender);
    void __fastcall btnUpdateClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall TimerTCPTimer(TObject *Sender);
    void __fastcall btnCfgClick(TObject *Sender);
    void __fastcall btnHelpClick(TObject *Sender);
    void __fastcall BatchUpdate();
private:	// User declarations
    net_struct netInfo;
    TUDPThread *udpThread;
    TTCPServer *tcpServerThread;

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
    int clientDevNum;
    int finishDevNum;
    TProgressBar *progressBar[SUPPORT_DEVICE_MAX];
    TLabel *lblProgress[SUPPORT_DEVICE_MAX];
    TLabel *lblMac[SUPPORT_DEVICE_MAX];
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
