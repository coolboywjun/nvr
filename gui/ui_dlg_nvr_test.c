#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mgext.h>
#include <minigui/skin.h>

#include <gpio.h>

#include "ui_config.h"
#include "ctrl_date.h"
#include "myedit.h"
#include "my_trackbar.h"
#include "ctrl_ipaddress.h"
#include "nvr_lib_interface.h"
#include "gui_core.h"

#include "storage_interface.h"
#include "player_interface.h"
#include "syscfg.h"

#include "ui_dlg_head.h"


#define NVR_TEST_DLG_W	700
#define NVR_TEST_DLG_H	500
#define NVR_TEST_DLG_X	((NVR_SCREEN_WIDTH - NVR_TEST_DLG_W)>>1)
#define NVR_TEST_DLG_Y	((NVR_SCREEN_HEIGHT - NVR_TEST_DLG_H)>>1)

#define NVR_TEST_LIST_W		((NVR_TEST_DLG_W*9)/10)
#define NVR_TEST_LIST_H		((NVR_TEST_DLG_H*1)/2)
#define NVR_TEST_LIST_X 	((NVR_TEST_DLG_W - NVR_TEST_LIST_W)>>1)
#define NVR_TEST_LIST_Y 	(60)

#define NVR_TEST_STATIC_X	50

#define UI_HWRESET_GPIO		26
#define UI_SYSTEM_LED_GPIO	27
#define UI_FAN_GPIO			28
#define UI_STORAGE_LED_GPIO	77


enum UI_NVR_TEST_ID{
	IDC_NVR_TEST = 500,
	IDC_NVR_TEST_LISTVIEW,
	IDC_NVR_TEST_START,
	IDC_NVR_TEST_RESULT,
	IDC_NVR_TEST_EXIT,
	IDC_NVR_TEST_TIMER1,
	IDC_NVR_TEST_TIMER2,
	IDC_NVR_TEST_TIMER3,
	IDC_NVR_TEST_HELP,
};

static CTRLDATA ui_nvr_test_control[] =
{
	{
		GUI_LISTVIEW,
        WS_BORDER | WS_CHILD | WS_VISIBLE/* | LVS_AUTOCHECKBOX */| LVS_NOTIFY,//test
        NVR_TEST_LIST_X, NVR_TEST_LIST_Y, NVR_TEST_LIST_W, NVR_TEST_LIST_H,
        IDC_NVR_TEST_LISTVIEW,
        "测试",
        0
    },
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | SS_GROUPBOX | WS_VISIBLE,
        NVR_TEST_LIST_X, NVR_TEST_LIST_Y+NVR_TEST_LIST_H+10, NVR_TEST_LIST_W, 80,
        IDC_STATIC,
        "注意",
        0,
		WS_EX_TRANSPARENT
    },	
    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        NVR_TEST_LIST_X+20, NVR_TEST_LIST_Y+NVR_TEST_LIST_H+30, NVR_TEST_LIST_W-20, 60,
        IDC_NVR_TEST_HELP,
        "测试USB时,两个USB接口都需要先插入USB鼠标;测试硬盘时,需要先接入硬盘.",
        0,
		WS_EX_TRANSPARENT
    },	

    {
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        NVR_TEST_LIST_X, NVR_TEST_LIST_Y+NVR_TEST_LIST_H+120, 80, 20,
        IDC_STATIC,
        "测试结果",
        0,
		WS_EX_TRANSPARENT
    },

	{
        CTRL_MYEDIT,
		WS_CHILD | WS_VISIBLE |ES_LEFT|WS_TABSTOP|ES_READONLY,
		NVR_TEST_LIST_X+80, NVR_TEST_LIST_Y+NVR_TEST_LIST_H+120, 60, 22,
        IDC_NVR_TEST_RESULT,
        "",
        0,        
        WS_EX_USEPARENTFONT	
    },
    
    CONFIG_TWO_WORDS_BUTTON("开始", IDC_NVR_TEST_START, NVR_TEST_DLG_W-100 , NVR_TEST_DLG_H- 40),
    //////////// GUI_BUTTON ////////////////
   // CONFIG_TWO_WORDS_BUTTON("退出", IDC_EXIT, NVR_TEST_DLG_W-100 , NVR_TEST_DLG_H- 40),
    
};


static DLGTEMPLATE NvrTestDlg =
{
    WS_NONE,
    WS_EX_NONE,
    NVR_TEST_DLG_X, NVR_TEST_DLG_Y, NVR_TEST_DLG_W, NVR_TEST_DLG_H,
    "时间设置",
    0, 0,
    TABLESIZE(ui_nvr_test_control), NULL,
    0
};

static listCaption captionNvrTest[] = 
{
	{20, "测试"},		
	{20, "结果"},
};

static char *testProg[]=
{
	"USB测试",
	"硬盘测试",
	"音频输出测试",
	"音频输入测试",
	"系统LED测试",
	"存储LED测试",
	"风扇测试",
	"复位按键测试",
	"VGA测试",
	"HDMI测试",
//	"设置标志位",
};

enum {
	//CMD_TEST_NETWORK = 100,
	CMD_TEST_USB = 0,
	CMD_TEST_DISK,
	CMD_TEST_AUDIOOUT,
	//CMD_TEST_AUDIOOUT_END,
	CMD_TEST_AUDIOIN, 
	//CMD_TEST_AUDIOIN_END,

	CMD_TEST_LED_SYSTEM,
	CMD_TEST_LED_STORAGE,
	CMD_TEST_FAN,
	//CMD_TEST_GPIO,
	CMD_TEST_RESET_KEY,
	CMD_TEST_VGA,
	CMD_TEST_HDMI,
	CMD_TEST_FINISH,
	
	CMD_TEST_END,
};

static HWND hListView = HWND_INVALID;
static long sysLedHandle = 0;

static long storageLedHandle = 0;

static HWND gTestWin= HWND_INVALID;

void setNvrTestWin(HWND hWnd)
{
	gTestWin = hWnd;
}

HWND getNvrTestWin(void)
{
	return gTestWin;
}


#ifdef CUSTOM_DRAW
static void my_draw_hdr_bk (HWND hWnd, HLVHDR hlvhdr, HDC hdc, RECT *rcDraw)
{
    SetBrushColor (hdc, LISTVIEW_HEAD_BKC);
    FillBox (hdc, rcDraw->left, rcDraw->top, RECTWP(rcDraw), RECTHP(rcDraw));
}

static void my_draw_hdr_item (HWND hWnd, int idx, HDC hdc, RECT *rcDraw)
{
	SetPenColor(hdc,0xff575859);
    Rectangle (hdc, rcDraw->left, rcDraw->bottom, rcDraw->right, rcDraw->bottom);	
    Rectangle (hdc, rcDraw->right, rcDraw->top, rcDraw->right, rcDraw->bottom);
    SetBkMode (hdc, BM_TRANSPARENT);
	SetTextColor(hdc, PIXEL_lightwhite);
    TextOut (hdc, rcDraw->left+4, rcDraw->top+4, LTR(captionNvrTest[idx].Text));
}
#endif


static int InitNvrTestListHeader(HWND hDlg)
{
	int i;
    LVCOLUMN lvcol = {0};
	int sum_w = 0;
	int col_num = TABLESIZE(captionNvrTest);
	
	for (i = 0; i < col_num; i++)
		sum_w += captionNvrTest[i].col_w;
	
    for (i = 0; i < col_num; i++) 
	{
        lvcol.nCols = i;
        lvcol.pszHeadText = captionNvrTest[i].Text;
        lvcol.width = captionNvrTest[i].col_w*NVR_TEST_LIST_W/sum_w;
	//	if(i == 0)
	//		lvcol.pfnCompare = NULL;//compare_string; //序号项，按照序号的大小排序
	//	else
        	lvcol.pfnCompare = NULL;//其它选项按照默认的按字符串排序
        lvcol.colFlags = 0;
		SendDlgItemMessage(hDlg, IDC_NVR_TEST_LISTVIEW, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
    }

#ifdef CUSTOM_DRAW
	HWND hListView;
	hListView = GetDlgItem(hDlg,IDC_NVR_TEST_LISTVIEW);
    LVCUSTOMDRAWFUNCS myfuncs = {my_draw_hdr_bk, my_draw_hdr_item};
    SendMessage (hListView, LVM_SETCUSTOMDRAW, 0, (LPARAM) &myfuncs);
#endif
	
	return 0;
}

static GHANDLE AddNvrTestItemList(HWND hDlg, PLVITEM lvItem, char *testName)
{
    LVSUBITEM subdata;
    GHANDLE item = SendDlgItemMessage (hDlg, IDC_NVR_TEST_LISTVIEW, LVM_ADDITEM, 0, (LPARAM)lvItem);
  
    subdata.flags = 0;
    subdata.image = 0;
    subdata.nItem = lvItem->nItem;
	subdata.nTextColor = 0;

	//测试项
	//sprintf(Text, "%d", lvItem->nItem);
	if(NULL == testName)
		return 0;
	subdata.subItem = 0;
	subdata.pszText = testName;
    SendDlgItemMessage (hDlg, IDC_NVR_TEST_LISTVIEW, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//结果
	subdata.subItem = 1;
	subdata.pszText = " ";
    SendDlgItemMessage (hDlg, IDC_NVR_TEST_LISTVIEW, LVM_SETSUBITEM, item, (LPARAM) & subdata);

    return item;
}

static int DisplayNvrTestListItem(HWND hDlg)
{
	int i = 0;
	LVITEM item;

	SendDlgItemMessage (hDlg, IDC_NVR_TEST_LISTVIEW, LVM_DELALLITEM, 0, 0);
	SendDlgItemMessage (hDlg, IDC_NVR_TEST_LISTVIEW, MSG_FREEZECTRL, TRUE, 0);	
	item.nItemHeight = 20;
	
	for(i = 0; i < TABLESIZE(testProg); i++)
	{		
		item.nItem = i;
		AddNvrTestItemList(hDlg, &item, LTR(testProg[i]));
	}
	SendDlgItemMessage (hDlg, IDC_NVR_TEST_LISTVIEW, MSG_FREEZECTRL, FALSE, 0);
	SendDlgItemMessage (hDlg, IDC_NVR_TEST_LISTVIEW, LVM_SELECTITEM, 0, (LPARAM)0);
	HWND retWnd;
	retWnd = GetDlgItem (hDlg, IDC_NVR_TEST_RESULT);
	SetWindowText(retWnd, " ");
	
	return 0;
}

static void setTestResult(HWND hWnd, int rows, int status)
{
	LVSUBITEM lvSubitem;

	lvSubitem.subItem = 1;
	lvSubitem.nItem = rows;
	if(status == 0xff)
		lvSubitem.pszText = "";
	else if(status == 0)
		lvSubitem.pszText = LTR("成功");
	else
		lvSubitem.pszText = LTR("失败");
		
	SendMessage (hListView, LVM_SETSUBITEMTEXT, 0, (LPARAM)&lvSubitem);
	InvalidateRect(hListView, NULL, TRUE);
}

BOOL sysLedTimerProc(HWND hDlg, int id)
{
	static int count = 0;
	int value;

	value = (count++ % 2) ? 1 : 0;
	if(gpio_set_value(sysLedHandle, value) < 0)
	 return FALSE;

	return TRUE;
}

BOOL storageLedTimerProc(HWND hDlg, int id)
{
	static int count = 0;
	int value;

	value = (count++ % 2) ? 1 : 0;
	if(gpio_set_value(storageLedHandle, value) < 0)	
		return FALSE;
	
	return TRUE;
}

static int audioQuit = 0;
void *gui_play_audio_thread(void *data)
{
	long readId = 0, writeId = 0;
	char *pAudioBuf = NULL;
	int nBufLen = 0;
	int nFrameLen;
	AV_AUDIO_INFO_S audio_par = {0};
	AV_FRAME_HEAD_S *pFrameHdr;
	
	if(player_start_audio_record() < 0)
		return NULL;
	audio_par.u8AudioSamples = 0;
	audio_par.u16EncodeType = 1;
	audio_par.u8AudioChannels = 1;
	audio_par.u8AudioBits = 16;
	if(player_set_record_par(&audio_par) < 0)
	{
		player_stop_audio_record();
		return NULL;
	}
	if(player_start_phone_audio() < 0)
	{
		player_stop_audio_record();
		return NULL;
	}
	if(av_pool_start_read_frame(AV_POOL_TALK_CHN, AV_POOL_TALK_OUT_STREAM, 1000, &readId) < 0)
	{
		player_stop_phone_audio();
		player_stop_audio_record();
		return NULL;
	}
	if(av_pool_start_write_frame(AV_POOL_TALK_CHN, AV_POOL_TALK_IN_STREAM, &writeId) < 0)
	{
		av_pool_stop_read_frame(readId);
		player_stop_phone_audio();
		player_stop_audio_record();
		return NULL;
	}
	while(!audioQuit)
	{
		pAudioBuf = NULL;
		nBufLen = 0;
		if((nFrameLen = av_pool_read_frame(readId, &pAudioBuf, &nBufLen, 100)) <= 0)
			continue;
		pFrameHdr = (AV_FRAME_HEAD_S *)pAudioBuf;
		if(av_pool_write_frame(writeId, (unsigned char *)(pAudioBuf+AV_FRAME_HEAD_LEN), nFrameLen-AV_FRAME_HEAD_LEN,
							(unsigned char *)pFrameHdr, AV_FRAME_HEAD_LEN,
							pFrameHdr->u32FrameNo, pFrameHdr->u8FrameType,
							pFrameHdr->u64TimeStamp) < 0)
			break;
		
	}
	av_pool_stop_write_frame(writeId);
	av_pool_stop_read_frame(readId);
	player_stop_phone_audio();
	player_stop_audio_record();
	
	return NULL;
}


static void showHelpInfo(HWND hDlg, char *sCh)
{
	HWND hWind = HWND_INVALID;

	if(sCh == NULL)
		return ;
		
	hWind = GetDlgItem (hDlg, IDC_NVR_TEST_HELP);
	SetWindowText(hWind, sCh);
	InvalidateRect(hWind, NULL, TRUE);
}

static void *testHwResetGpioFunc(void *data)
{
	void *ret = 0;
	int value = 0;
	int first_value = -1;
	long handle;
	volatile int *quit = (int *)data;

	if((handle = gpio_open(UI_HWRESET_GPIO)) == 0)
		return ret;
	while(!*quit)
	{
		if(gpio_get_value(handle, &value) < 0)
			break;
		if(first_value == -1)
		{
			first_value = value;
		}
		else
		{
			if(value != first_value)
			{
				ret = (void*)1;
				break;
			}
		}
		usleep(50000);
	}
	gpio_close(handle);
	return ret;
}

static int testNvrFunc(HWND hDlg, int cmd)
{
	int ret = -1;

	if(cmd < CMD_TEST_USB || cmd >= CMD_TEST_END)
	{
		CreateMessageBoxWin(hDlg, LTR("没有此测试项!"), LTR("消息"), IDOK);
		return -1;
	}

	switch(cmd)
	{
		case CMD_TEST_USB:
		{
			if(access("/dev/input/mouse0", F_OK) == 0
			&& access("/dev/input/mouse1", F_OK) == 0)
			ret = 0;
			
			setTestResult(hDlg, CMD_TEST_USB, ret);
		}
		break;
		
		case CMD_TEST_DISK:
		{
			if(access("/dev/sda", F_OK) != 0)
			{
				setTestResult(hDlg, CMD_TEST_DISK, ret);
				break;
			}
			
			ret = 0;
			setTestResult(hDlg, CMD_TEST_DISK, ret);
		}
		break;
		
		case CMD_TEST_AUDIOIN:
		{
			showHelpInfo(hDlg, \
			LTR("测试音频输入时,音频输入接口需要接入拾音器(如麦克风),音频输出接口需要接入耳麦或音响等设备.若能听到回声表示测试通过,反之表示测试失败."));
			pthread_t thid;
			audioQuit = 0;
			pthread_create(&thid, NULL, gui_play_audio_thread, NULL);
			pthread_detach(thid);
			int result = 0;

			result = CreateMessageBoxWin(hDlg, LTR("请确认是否有音频输入!"), LTR("消息"), MB_YESNO);
			ret = (result == IDYES) ? 0 : -1;
			setTestResult(hDlg, CMD_TEST_AUDIOIN, ret);

			audioQuit = 1;
		}	
		break;
		
		case CMD_TEST_AUDIOOUT:
			showHelpInfo(hDlg, \
			LTR("测试音频输出时,音频输出接口需要接入耳麦或音响等设备.若能听到报警声表示测试通过,反之表示测试失败."));
			ret = player_alarm_audio("nvr_res/B.wav");
			if(0 == ret)
			{
			 	int result = 0;
			 	
				result = CreateMessageBoxWin(hDlg, LTR("请确认是否有音频输出!"), LTR("消息"), MB_YESNO);
				ret = (result == IDYES) ? 0 : -1;
				setTestResult(hDlg, CMD_TEST_AUDIOOUT, ret);
				
				player_stop_alarm_audio();
				break;
			}
			else
			{
				player_stop_alarm_audio();
				setTestResult(hDlg, CMD_TEST_AUDIOOUT, ret);  //打开文件失败
			}
		break;
		case CMD_TEST_LED_SYSTEM:
		{
			if((sysLedHandle = gpio_open(UI_SYSTEM_LED_GPIO)) == 0)
			{
				setTestResult(hDlg, CMD_TEST_LED_SYSTEM, ret);
				break;
			}
			if(!IsTimerInstalled(hDlg, IDC_NVR_TEST_TIMER1))
				SetTimer(hDlg, IDC_NVR_TEST_TIMER1, 20);

			int result = 0;
			result = CreateMessageBoxWin(hDlg, LTR("请确认系统LED是否在闪烁!"), LTR("消息"), MB_YESNO);

			ret = (result == IDYES) ? 0 : -1;
			setTestResult(hDlg, CMD_TEST_LED_SYSTEM, ret);

			if(IsTimerInstalled(hDlg, IDC_NVR_TEST_TIMER1))
				KillTimer(hDlg, IDC_NVR_TEST_TIMER1);
			gpio_close(sysLedHandle);

		}			
		break;
		
		case CMD_TEST_LED_STORAGE:
		{
			if((storageLedHandle = gpio_open(UI_STORAGE_LED_GPIO)) == 0)
			{
				setTestResult(hDlg, CMD_TEST_LED_STORAGE, ret);
				break;
			}
			if(!IsTimerInstalled(hDlg, IDC_NVR_TEST_TIMER2))
				SetTimer(hDlg, IDC_NVR_TEST_TIMER2, 20);
				
			int result = 0;
			result = CreateMessageBoxWin(hDlg, LTR("请确认存储LED是否在闪烁!"), LTR("消息"), MB_YESNO);

			ret = (result == IDYES) ? 0 : -1;
			setTestResult(hDlg, CMD_TEST_LED_STORAGE, ret);
			
			if(IsTimerInstalled(hDlg, IDC_NVR_TEST_TIMER2))
				KillTimer(hDlg, IDC_NVR_TEST_TIMER2);
			gpio_close(storageLedHandle);
		}
		break;
		
		case CMD_TEST_FAN:
		{
			long handle;
			unsigned char val = 1;
			
			if((handle = gpio_open(UI_FAN_GPIO)) == 0)
			{
				setTestResult(hDlg, CMD_TEST_FAN, ret);
				break;
			}
				
			if(gpio_set_value(handle, val) < 0)
			{
				gpio_close(handle);
				setTestResult(hDlg, CMD_TEST_FAN, ret);
				break;
			}
			
			int result = 0;
			result = CreateMessageBoxWin(hDlg, LTR("请确认风扇是否在工作!"), LTR("消息"), MB_YESNO);

			ret = (result == IDYES) ? 0 : -1;
			setTestResult(hDlg, CMD_TEST_FAN, ret);

			gpio_close(handle);
		}
		break;

		case CMD_TEST_VGA:
		{
			int result = 0;
			showHelpInfo(hDlg, LTR("测试VGA输出时,VGA接口需要接显示器."));
			result = CreateMessageBoxWin(hDlg, LTR("请确认VGA输出是否正常!"), LTR("消息"), MB_YESNO);
			ret = (result == IDYES) ? 0 : -1;
			setTestResult(hDlg, CMD_TEST_VGA, ret);
		}
		break;
		case CMD_TEST_HDMI:
		{
			int result = 0;
			showHelpInfo(hDlg, LTR("测试HDMI输出时,HDMI接口需要接显示器."));
			result = CreateMessageBoxWin(hDlg, LTR("请确认HDMI输出是否正常!"), LTR("消息"), MB_YESNO);
			ret = (result == IDYES) ? 0 : -1;
			setTestResult(hDlg, CMD_TEST_HDMI, ret);
		}
		break;
		case CMD_TEST_RESET_KEY:
		{
			void *result;
			volatile int quit = 0;
			pthread_t thid;
			showHelpInfo(hDlg,LTR("测试复位键时,需要先按下复位键,等测试完此项再松手."));
			pthread_create(&thid, NULL, testHwResetGpioFunc, (void*)&quit);
			CreateMessageBoxWin(hDlg, LTR("请确认按下了复位键!"), LTR("消息"), MB_OK);
			quit = 1;
			pthread_join(thid, &result);
			ret = ((int)result == 1) ? 0 : -1;
			setTestResult(hDlg, CMD_TEST_RESET_KEY, ret);
		}		
		break;
		case CMD_TEST_FINISH:
		{
			DEV_SystemCfg *pSysCfg;

			ret = 0;
			pSysCfg = get_system_cfg();
			if(pSysCfg->u8FactoryChecked == 0)
			{
				pSysCfg->u8FactoryChecked = 1;
				ret = save_system_cfg(pSysCfg);
			}
			ret = ret>=0 ? 0 : -1;
			//setTestResult(hDlg, CMD_TEST_FINISH, ret);
		}
		break;
		default:
			break;
		
	}
	
	return ret;
}

static int startTestNvrFunc(HWND hDlg)
{
	int nCnt = CMD_TEST_FINISH;
	int i, ret = 0;

	for(i=CMD_TEST_USB; i <= nCnt; i++)
	{
		setTestResult(hDlg, i, 0xff);
	}
	nvr_set_hwgpio_check(0);
	for(i=CMD_TEST_USB; i < nCnt; i++)
	{		
		ret |= testNvrFunc(hDlg, i);
	}
	if((i == CMD_TEST_FINISH) && (ret == 0))
		ret |= testNvrFunc(hDlg, i);
	else
		setTestResult(hDlg, CMD_TEST_FINISH, -1);
	nvr_set_hwgpio_check(1);
	
	return ret;
}

static int NvrTestWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static gal_pixel tmpColor;

    switch (message)
    {

    case MSG_INITDIALOG:
    {	
    	InitNvrTestListHeader(hDlg);
    	DisplayNvrTestListItem(hDlg);
    	
    	hListView = GetDlgItem (hDlg, IDC_NVR_TEST_LISTVIEW);
    	setNvrTestWin(hDlg);	

		HWND retWnd;
		retWnd = GetDlgItem (hDlg, IDC_NVR_TEST_RESULT);
		tmpColor = GetWindowBkColor(retWnd);

    	//push_open_dlg(&headOpenDlg, hDlg);
        break;
    }
#if 0    
	case MSG_PAINT:
	{
		HDC hdc;
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);
		Gui_DrawCaption(hDlg,  "NVR 测试");
        break;
	}
#endif
	case MSG_ERASEBKGND:
	{
		RECT title = {0, 0, 700, 50};
		HDC hdc = (HDC)wParam; 
		const RECT* clip = (const RECT*) lParam; 
		BOOL fGetDC = FALSE; 
		RECT rcTemp; 
		if (hdc == 0) 
		{ 
			hdc = GetClientDC (hDlg); 
			fGetDC = TRUE; 
		} 
		if (clip) 
		{ 
			rcTemp = *clip; 
			ScreenToClient (hDlg, &rcTemp.left, &rcTemp.top); 
			ScreenToClient (hDlg, &rcTemp.right, &rcTemp.bottom); 
			IncludeClipRect (hdc, &rcTemp); 
		} 
		/* 用图片填充背景*/ 
		FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bk_pic[SIZE_700x500]); 
		Gui_WriteTitleName(hdc, &title, LTR("NVR测试"));
		if (fGetDC) 
			ReleaseDC (hdc); 
		return 0; 
	}
	break;
	
	case MSG_RBUTTONUP:
    {
		PostMessage(hDlg, MSG_CLOSE, 0, 0);
        break;
    }

    case MSG_COMMAND:
    {	
	//	int id = LOWORD(wParam);
	//	int code = HIWORD(wParam);
		int ret;
		
		switch(wParam)
		{
			case IDC_NVR_TEST_START:
			{
				HWND retWnd;
				retWnd = GetDlgItem (hDlg, IDC_NVR_TEST_RESULT);
				SetWindowText(retWnd, "");
				
				SetWindowBkColor(retWnd, tmpColor);
				DisplayNvrTestListItem(hDlg);
				InvalidateRect(hDlg, NULL, TRUE);
				
				ret = startTestNvrFunc(hDlg);
				if(ret < 0)
				{
					SetWindowText(retWnd, LTR("失败"));
					SetWindowBkColor(retWnd, 0xFFFF0000);//背景为红色
				}
				else
				{
					SetWindowText(retWnd, LTR("成功"));
					SetWindowBkColor(retWnd, 0xFF00FF00);//背景为绿色
				}

				showHelpInfo(hDlg, LTR("NVR 测试完成之后,系统需要重启才能正常工作."));

				InvalidateRect(hDlg, NULL, TRUE);
			}
			break;
		}

		break;
	}
	case MSG_TIMER:
	{
		switch(wParam)
		{
			case IDC_NVR_TEST_TIMER1:
			{
				sysLedTimerProc(hDlg, wParam);
			}
			break;
			case IDC_NVR_TEST_TIMER2:
			{
				storageLedTimerProc(hDlg, wParam);
			}
			break;
		}
		break;
	}

	case MSG_CLOSE:
    {		
    	//HWND hWind;
    	//pop_open_dlg(&headOpenDlg, &hWind);
    	setNvrTestWin(HWND_INVALID);
        EndDialog (hDlg, 0);
        break;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

//远程设备配置
void CreateNvrTestDlgWin(HWND hParent)
{
#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER , "NvrTestDlg" , 0 , 0);
#endif
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_nvr_test_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_nvr_test_control, sizeof(ui_nvr_test_control));

	DLG_CAPTION_LANGUAGE_TRANS(ui_nvr_test_control)
    NvrTestDlg.controls = ui_nvr_test_control;
    
    DialogBoxIndirectParam (&NvrTestDlg, hParent, NvrTestWinProc, 0L);
	memcpy(ui_nvr_test_control, ctrolTmp, sizeof(ui_nvr_test_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}

