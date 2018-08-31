#ifndef __UI_DLG_HEAD_H__
#define __UI_DLG_HEAD_H__

#include "nvr_lib_interface.h"

typedef struct _listCaption
{
	int col_w;
	char *Text;
}listCaption;

typedef struct _loginInfo
{
	int 	login;
	char 	acUser[NVR_USER_NAME_LEN];
}loginInfo_t;

typedef enum _PLAY_MODE{
	REALTIME_PLAY = 0,
	RECORD_PLAY,
}PLAY_MODE;

typedef struct ZOOM_PLAYERSRC_S{
	short x;
	short y; 
	short w;
	short h;
	int zoomWidth;
	int zoomHeight;
	int initFlag;
}ZOOM_PLAYERSRC;

/*ÉùÃ÷uiµÄº¯Êý*/
void CreateAdvanceMainMenuDlgWin(HWND hParent);
void CreateAutoMaintainDlgWin(HWND hParent);
void CreateCloseSysDlgWin(HWND hParent);
void CreateColorDlgWin(HWND hParent, int chn);
void CreateCopyDlgWin(HWND hParent, int chn, int cmd);

void CreateDdnsSetDlgWin(HWND hParent);
void CreateUpnpSetDlgWin(HWND hParent);
void CreatePppoeSetDlgWin(HWND hParent);
void CreateDevCfgDlgWin(HWND hParent, int chn);
void CreateDiskManageDlgWin(HWND hParent);
void CreateEncodeDlgWin(HWND hParent, int chn);
void CreateIpncCfgDlgWin(HWND hParent, int chn);
void CreateLoginDlgWin(HWND hParent);
void CreateMainMenuDlgWin(HWND hParent, int dis_mode, int cur_ch);

void CreateNetCfgDlgWin(HWND hParent);
void CreateNetServeDlgWin(HWND hParent);
void CreateNtpsetDlgWin(HWND hParent);
void CreatePtzDlgWin(HWND hParent, int chn);
void CreateRecordDlgWin(HWND hParent);
void CreateRecordTimeDlgWin(HWND hParent, int chn);
void CreateRemoteDeviceDlgWin(HWND hWnd);
void CreateReplayDlgWin(HWND parent, int dis_mode, int real_ch);
void CreateStorageBackupDlgWin(HWND hParent, time_t dayStartsec);
void CreateSystemInfoDlgWin(HWND hParent);
void CreateSetTimer(HWND hParent, int chn);
void CreateUpdateDlgWin(HWND hParent, char *filepath, char *filename);
void CreateUserDlgWin(HWND hParent);
void CreateUserModifyDlgWin(HWND hParent, int nUser, int wFlag);
void CreateAlphaColorkeyDlgWin(HWND hParent);
void CreateNvrTestDlgWin(HWND hParent);

void CreateLogDlgWin(HWND hParent);
void CreateWizardDlgWin(HWND hParent);
void CreateNormalParamWin(HWND hParent);
void CreateIpcOsdDlgWin(HWND hParent, int chn);

void CreateConfigBakupWin(HWND hDlg);
void CreateQrcodeDlgWin(HWND hParent);


typedef struct _link_open_dlg{
	HWND openWind;
	struct _link_open_dlg *next;
}link_opendlg,*plink_opendlg;

typedef struct _IPC_OSD_PARAM{
	Uint8 u8EnableTimeOsd;
	Uint8 u8EnableBitrateOsd;
	Uint32 u32TimeColor;
	Uint32 u32ChnColor;
	char szChnName[IPNC_USER_NAME_LEN];
}osdParam, *pOsdParam;

extern plink_opendlg headOpenDlg;
extern BOOL push_open_dlg(plink_opendlg *head, HWND hDlg);
extern BOOL pop_open_dlg(plink_opendlg *head, HWND *hDlg);
extern void show_open_dlg(plink_opendlg head);

int GUIAPI CreateMessageBoxWin(HWND hParent, const char* pszText, 
                      const char* pszCaption, DWORD dwStyle);

void SwitchPlayChan(HWND hWnd, int ch);


#define DLG_CAPTION_LANGUAGE_TRANS(dlg_control) \
{\
	{\
		int i;\
		for(i = 0; i < TABLESIZE(dlg_control); i++)\
			dlg_control[i].caption = LTR(dlg_control[i].caption);\
	}\
}\

#endif
