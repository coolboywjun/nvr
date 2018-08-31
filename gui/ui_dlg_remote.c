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

#include "ui_config.h"
#include "nvr_lib_interface.h"

#include "ipnc_lib_client.h"

#include "gui_core.h"
#include "net_api.h"
#include "ui_dlg_head.h"

#define UI_REMOTE_DLG_W		700
#define UI_REMOTE_DLG_H		500

#define REMOTE_DEV_DLG_WIDITH	UI_REMOTE_DLG_W
#define REMOTE_DEV_DLG_HEIGHT	UI_REMOTE_DLG_H

#define REMOTE_DEV_DLG_L		((NVR_SCREEN_WIDTH - REMOTE_DEV_DLG_WIDITH)/2)
#define REMOTE_DEV_DLG_T		((NVR_SCREEN_HEIGHT - REMOTE_DEV_DLG_HEIGHT)/2)
#define REMOTE_DEV_DLG_R		(REMOTE_DEV_DLG_L + REMOTE_DEV_DLG_WIDITH)
#define REMOTE_DEV_DLG_B		(REMOTE_DEV_DLG_T + REMOTE_DEV_DLG_HEIGHT)


#define _DEV_LIST_W		(REMOTE_DEV_DLG_WIDITH*9/10)	
#define _DEV_LIST_H		(REMOTE_DEV_DLG_HEIGHT*3/10)
#define _DEV_LIST_L1	((REMOTE_DEV_DLG_WIDITH - _DEV_LIST_W)/2)
#define _DEV_LIST_T1	65
#define _DEV_LIST_R1	(_DEV_LIST_L1 + _DEV_LIST_W)
#define _DEV_LIST_B1	(_DEV_LIST_T1 + _DEV_LIST_H)

#define _DEV_LIST_L2	(_DEV_LIST_L1)
#define _DEV_LIST_T2	(_DEV_LIST_B1 + 70)
#define _DEV_LIST_R2	_DEV_LIST_R1	
#define _DEV_LIST_B2	(_DEV_LIST_T2 + _DEV_LIST_H)

#define GUI_MAX_DEV_NUM 	512

typedef enum
{
	IDC_LISTVIEW1 = 2800,
	IDC_DEV_SEARCH,
	IDC_DEV_ADD,
	IDL_SHAIXUAN,
	IDC_LISTVIEW2,
	IDC_DEV_DELET,
	IDC_ADD_MANUAL,
	IDC_DEV_REFRESH,
	IDC_OK,
	IDC_CANCEL,
	IDC_SEARCH_AA,
	ID_SEARCH_TIMER,
	IDC_REMOTE_PREV,
	IDC_REMOTE_NEXT,
	IDC_REMOTE_WIZARD_CANCEL,
}UI_REMOTE_ID;



typedef struct tag_SearchMan_s
{	
	int 	supportProtocalNum; //支持协议数
	long 	lHandleArray[MAX_PROTOCAL_NUM];	//lHandle == 0表示无效
	IPNC_PROTOCOL_S stIpncProtocol[MAX_PROTOCAL_NUM];
	int     nItemNum;
	int		tryTime;
}SearchMan_t;


typedef struct _RemoteDevMan
{
	int UserNum;
	IPNC_NET_SEARCH_RES_S  stUserInfo[GUI_MAX_DEV_NUM];
	IPNC_PROTOCOL_S		   stDevProtocal[GUI_MAX_DEV_NUM];
}RemoteDevMan_t;

typedef struct _LivSerialNum
{
	int livNum;
	int devNum;
}LivSerialNum_t;

static RemoteDevMan_t g_stDevMan;
static SearchMan_t g_stSearch;
static RECT  remote_title_rect = {0,0,UI_REMOTE_DLG_W,50};


int AddUserList(IPNC_NET_SEARCH_RES_S *pstUser, IPNC_PROTOCOL_S *pstProtocal)
{	
	if (g_stDevMan.UserNum >= GUI_MAX_DEV_NUM) return -1;
	memcpy(&g_stDevMan.stUserInfo[g_stDevMan.UserNum], pstUser, sizeof(IPNC_NET_SEARCH_RES_S));
	memcpy(&g_stDevMan.stDevProtocal[g_stDevMan.UserNum], pstProtocal, sizeof(IPNC_PROTOCOL_S));
	g_stDevMan.UserNum++;

	return 0;
}

void ClearUserList()
{	
	memset(&g_stDevMan, 0, sizeof(RemoteDevMan_t));
}

IPNC_NET_SEARCH_RES_S *GetUserByIndex(int index)
{
	if (index > g_stDevMan.UserNum)
		return NULL;
	return &g_stDevMan.stUserInfo[index];
}

IPNC_PROTOCOL_S *GetProtocalByIndex(int index)
{
	if (index > g_stDevMan.UserNum)
		return NULL;
	return &g_stDevMan.stDevProtocal[index];
}

char *GetProtoclaAliasName(char *protocalName)
{
	if (strcmp("jxj_ti", protocalName) == 0)
		return "v1";
	else if (strcmp("jxj_cu", protocalName) == 0)
		return "v2";
	else if(strcmp("onvif_cu", protocalName) == 0)
		return "ONVIF";
	else 
		return protocalName;
}

char *GetProtoclaName(char *AliasName)
{
	if (strcmp("v1", AliasName) == 0)
		return "jxj_ti";
	else if (strcmp("v2", AliasName) == 0)
		return "jxj_cu";
	else if(strcmp("ONVIF", AliasName) == 0)
		return "onvif_cu";
	else 
		return AliasName;
}


static HWND hListView1, hListView2, gRemoteWin;
/* 定义皮肤元素数组 */
static CTRLDATA ui_remote_control[] =
{
	{
		GUI_LISTVIEW,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_BORDER | LVS_AUTOCHECKBOX | LVS_NOTIFY,//test
        _DEV_LIST_L1, _DEV_LIST_T1, _DEV_LIST_W, _DEV_LIST_H,
        IDC_LISTVIEW1,
        "所有设备",
        0
    },
    CONFIG_FOUR_WORDS_BUTTON("设备搜索", IDC_DEV_SEARCH, _DEV_LIST_L1 , _DEV_LIST_B1 + 5),
	CONFIG_TWO_WORDS_BUTTON("添加", IDC_DEV_ADD, _DEV_LIST_L1 +90, _DEV_LIST_B1 + 5),

	{
        CTRL_STATIC,
        WS_CHILD | SS_SIMPLE | WS_VISIBLE,
        _DEV_LIST_R1 - 190, _DEV_LIST_B1 + 5, 70, 21,
        IDC_STATIC,
        "显示筛选",
        0,
		WS_EX_TRANSPARENT
    },
    {
        GUI_BUTTON,
		WS_VISIBLE | BS_AUTOCHECKBOX, 
        _DEV_LIST_R1 - 120, _DEV_LIST_B1 + 40, 100+40, 26,
        IDC_SEARCH_AA,
        "智能IP控制",
        0,
		WS_EX_TRANSPARENT
    },
    {
		GUI_COMBOX,
		WS_CHILD| WS_VISIBLE |CBS_DROPDOWNLIST | CBS_READONLY |WS_TABSTOP|CBS_NOTIFY,
       	_DEV_LIST_R1 - 110,  _DEV_LIST_B1 + 5, 100, GUI_COMBOX_H,
        IDL_SHAIXUAN,//筛选
        "",
        0,
        WS_EX_USEPARENTFONT
       },
      
	{
        CTRL_STATIC,
        WS_CHILD | SS_LEFT | WS_VISIBLE,
        _DEV_LIST_L1, _DEV_LIST_B1 + 43, 80+60, 20,
        IDC_STATIC,
        "已选设备",
        0,
		WS_EX_TRANSPARENT
    },
	///////////////////////////////////////////////////////////

	 {
		GUI_LISTVIEW,
        WS_BORDER | WS_CHILD | WS_VISIBLE | LVS_AUTOCHECKBOX | LVS_NOTIFY,//test
        _DEV_LIST_L2, _DEV_LIST_T2, _DEV_LIST_W, _DEV_LIST_H,
        IDC_LISTVIEW2,
        "已添加设备",
        0
    },
    CONFIG_TWO_WORDS_BUTTON("删除", IDC_DEV_DELET, _DEV_LIST_L2, _DEV_LIST_B2 + 5),
	CONFIG_FOUR_WORDS_BUTTON("手动添加", IDC_ADD_MANUAL, _DEV_LIST_L2 + 70, _DEV_LIST_B2 + 5),
	CONFIG_TWO_WORDS_BUTTON("刷新", IDC_DEV_REFRESH, _DEV_LIST_L2+165, _DEV_LIST_B2 + 5),

	CONFIG_THREE_WORDS_BUTTON("确定", IDC_OK,    _DEV_LIST_R2 - 140 , REMOTE_DEV_DLG_HEIGHT - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_CANCEL, _DEV_LIST_R2 - 65 , REMOTE_DEV_DLG_HEIGHT - 50),

		
	CONFIG_THREE_WORDS_BUTTON("上一步", IDC_REMOTE_PREV,	 _DEV_LIST_R2 - 215 , REMOTE_DEV_DLG_HEIGHT - 50),
	CONFIG_THREE_WORDS_BUTTON("下一步", IDC_REMOTE_NEXT, _DEV_LIST_R2 - 140 , REMOTE_DEV_DLG_HEIGHT - 50),
	CONFIG_THREE_WORDS_BUTTON("取消", IDC_REMOTE_WIZARD_CANCEL, _DEV_LIST_R2 - 65 , REMOTE_DEV_DLG_HEIGHT - 50),
};


static char * caption[] =
{
    "全选","序号", "设备名", "IP地址", "端口" , "协议"
};

static char * caption2[] =
{
    "启用","通道", "IP地址", "端口", "智能增加", "协议"
};

static int capw[] = {4, 5, 13, 11, 5, 6};
static int capw2[] = {4, 5, 12, 6, 7, 8};


#define COL_NR       TABLESIZE(caption)
#define COL_NR2       TABLESIZE(caption2)

static int FindIpncIndex(HWND hDlg, int nItem)
{
	int nSelch,ret, i=0,j=0;
	char protocalAliasName[32];

	nSelch = SendDlgItemMessage (hDlg, IDL_SHAIXUAN, CB_GETCURSEL, 0, 0);
	
	if (nSelch < 0) return 0;
	SendDlgItemMessage(hDlg, IDL_SHAIXUAN, CB_GETLBTEXT, nSelch, (LPARAM)protocalAliasName);

	if(strcmp(protocalAliasName,LTR("所有")) == 0)
	{
		ret = nItem;
	}
	else
	{
		for(i=0; i<g_stDevMan.UserNum; i++)
		{
			if(strcmp(g_stDevMan.stDevProtocal[i].aliasName,protocalAliasName) == 0)
			{
				if(j == nItem)
				{
					break;
				}
				j++;
			}
		}
		ret = i;
	}
	return ret;
}

static int get_free_ch_index()
{
	int ch = 0;
	NVR_CHANNEL_CFG_S stNvrChan;
	for (ch = 0; ch < NVR_MAX_CHANNEL_NUM; ch++)
	{
		memset(&stNvrChan, 0, sizeof(NVR_CHANNEL_CFG_S));
		nvr_logon_get_channel_cfg(ch, &stNvrChan);
	//	if (stNvrChan.u8Enable == 0)  //是否勾选
		if(stNvrChan.u32Size == 0)//通道中没有数据
			break;
	}

	if (ch == NVR_MAX_CHANNEL_NUM)	return -1;
	return ch;
}


static int get_free_ch_number()
{
	int count = 0;
	int ch = 0;
	NVR_CHANNEL_CFG_S stNvrChan;
	
	for (ch = 0; ch < NVR_MAX_CHANNEL_NUM; ch++)
	{
		memset(&stNvrChan, 0, sizeof(NVR_CHANNEL_CFG_S));
		nvr_logon_get_channel_cfg(ch, &stNvrChan);
		//if (stNvrChan.u8Enable == 0)  //是否勾选
		if(stNvrChan.u32Size == 0)//通道中没有数据
			count++;
	}

	return count;
}


int compare_string(HLVITEM nItem1, HLVITEM nItem2, PLVSORTDATA sortData)
{

	char acText[64];
	int index1 = 0,index2 = 0;
	LVSUBITEM lvSubitem;
	
	lvSubitem.subItem = 1;  //序号项
	lvSubitem.pszText = acText;	
	SendMessage (hListView1, LVM_GETSUBITEMTEXT, nItem1, (LPARAM)&lvSubitem);	//获取到列表中的index	
	index1 = atoi(lvSubitem.pszText);
	
	SendMessage (hListView1, LVM_GETSUBITEMTEXT, nItem2, (LPARAM)&lvSubitem);	//获取到列表中的index	
	index2 = atoi(lvSubitem.pszText);
	
	return (index1 - index2);
}

static int init_device_list_header(HWND hListView)
{
	int i;
    LVCOLUMN lvcol;
	int sum_w = 0;
	printf("###############COL_NR: %d,COL_NR2: %d\r\n",COL_NR,COL_NR2);
	for (i = 0; i < COL_NR; i++)
		sum_w += capw[i];
    for (i = 0; i < COL_NR; i++) 
	{
        lvcol.nCols = i;
        lvcol.pszHeadText = LTR(caption[i]);
        lvcol.width = capw[i]*_DEV_LIST_W/sum_w;
		if(i == 1)
			lvcol.pfnCompare = compare_string; //序号项，按照序号的大小排序
		else
        	lvcol.pfnCompare = NULL;//其它选项按照默认的按字符串排序
        lvcol.colFlags = 0;

        if(i == 0)
        	lvcol.checkFlags = LVI_HEADER_BLANK; //listview 头显示复选框标志
		else
			lvcol.checkFlags = LVI_HEADER_UNCHECK;

        SendMessage (hListView, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
    }
	
	return 0;
}


static int init_ch_list_header(HWND hListView)
{
	int i;
    LVCOLUMN lvcol;
	int sum_w = 0;
	for (i = 0; i < COL_NR2; i++)
		sum_w += capw2[i];
    for (i = 0; i < COL_NR2; i++) 
	{
        lvcol.nCols = i;
        if(0 == i)
        	lvcol.checkFlags = LVI_HEADER_BLANK;
        else
			lvcol.checkFlags = LVI_HEADER_UNCHECK;
        lvcol.pszHeadText = LTR(caption2[i]);
        lvcol.width = capw2[i]*_DEV_LIST_W/sum_w;
		if(i == 1)
			lvcol.pfnCompare = compare_string;
		else
        	lvcol.pfnCompare = NULL;
        lvcol.colFlags = 0;

        SendMessage (hListView, LVM_ADDCOLUMN, 0, (LPARAM) &lvcol);
    }
	
	return 0;
}

static GHANDLE add_device_item_list (HWND hlist, PLVITEM lvItem, IPNC_NET_SEARCH_RES_S *pDevInfo, IPNC_PROTOCOL_S *pstSearchProtocol)
{
    char buff[20] = {0};
	char IP[32];
	char port[8];
    LVSUBITEM subdata;
    GHANDLE item = SendMessage (hlist, LVM_ADDITEM, 0, (LPARAM)lvItem);
    int i = lvItem->nItem;

	sprintf(buff, "%d", i+1);

    subdata.flags = LVFLAG_BLANK;//0;
    subdata.image = 0;
    subdata.nItem = lvItem->nItem;
	subdata.nTextColor = 0;

	subdata.subItem = 0;
	subdata.pszText = "";
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);
    
    subdata.flags = 0;//恢复初始值

	//序号
	subdata.subItem = 1;
	subdata.pszText = (char *)buff;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);
    
	//dev_name
	subdata.subItem = 2;
	subdata.pszText = (char *)pDevInfo->u8SerialId;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);
	
	//IP
	net_ip_n2a(pDevInfo->u32DevIp, IP, 32);
	subdata.subItem = 3;
	subdata.pszText = IP;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//port
	sprintf(port, "%d", pDevInfo->u16DevPort);
	subdata.subItem = 4;
	subdata.pszText = port;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//type
	subdata.subItem = 5;
	subdata.pszText = GetProtoclaAliasName(pstSearchProtocol->name);
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

    return item;
}

static GHANDLE add_ch_item_list(HWND hlist, PLVITEM lvItem,  NVR_CHANNEL_CFG_S *chCfg, int ch)
{
	char buff[20];
	char IP[32];
	char port[8];
    LVSUBITEM subdata;
    GHANDLE item = SendMessage (hlist, LVM_ADDITEM, 0, (LPARAM)lvItem);
  
	sprintf(buff, "%d", ch+1);

	if(chCfg->u8Enable)
    	subdata.flags = LVFLAG_CHECKED;
    else
   		subdata.flags = LVFLAG_BLANK;
    subdata.image = 0;
    subdata.nItem = lvItem->nItem;
	subdata.nTextColor = 0;

	//启用
	subdata.subItem = 0;
	subdata.pszText = "";
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	subdata.flags = 0; //恢复初始值
	
	//通道
	subdata.subItem = 1;
	subdata.pszText = (char *)buff;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//IP地址	
	net_ip_n2a(chCfg->u32IpAddr, IP, 32);
	subdata.subItem = 2;
	subdata.pszText = IP;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	//端口
	sprintf(port, "%d", chCfg->u16DataPort);
	subdata.subItem = 3;
	subdata.pszText = port;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

	
	//是否只能添加	
	subdata.subItem = 4;
	if (chCfg->u8SearchA == 1)
		subdata.pszText = LTR("是");
	else	
		subdata.pszText = LTR("否");
	SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);
#if 0
	//设备名
	subdata.flags = 0;
    subdata.image = 0;
	subdata.subItem = 1;
	subdata.pszText = chCfg->szDevName;
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);
#endif
	subdata.flags = 0;
	subdata.image = 0;
	//协议
	sprintf(port, "%d", chCfg->u16DataPort);
	subdata.subItem = 5;
	subdata.pszText = GetProtoclaAliasName(chCfg->szProtocolName);
    SendMessage (hlist, LVM_SETSUBITEM, item, (LPARAM) & subdata);

    return item;
}



static int init_ch_list(HWND hDlg, HWND hListView)
{
	int ch;
	int i = 0, ret = 0;
	LVITEM item;
	int nsearchA = 0;
	
	NVR_CHANNEL_CFG_S nvrChan[NVR_MAX_CHANNEL_NUM];
	
	SendMessage (hListView, LVM_DELALLITEM, 0, 0);
	SendMessage (hListView, MSG_FREEZECTRL, TRUE, 0);
	item.nItemHeight = 20;
	for (ch = 0; ch < NVR_MAX_CHANNEL_NUM; ch++)
	{
		memset(&nvrChan[ch], 0, sizeof(NVR_CHANNEL_CFG_S));
		ret = nvr_logon_get_channel_cfg(ch, &nvrChan[ch]);
		if (ret < 0) break;

		if (nvrChan[ch].u8Enable && nvrChan[ch].u8SearchA == 1) nsearchA++;
		
		//if (nvrChan[ch].u8Enable)
		if(nvrChan[ch].u32Size != 0)
		{
			item.nItem = i;
			add_ch_item_list(hListView, &item, &nvrChan[ch], ch);
			i++;
		}
	}
	
	SendMessage (hListView, MSG_FREEZECTRL, FALSE, 0);
	NVR_DEV_AUTO_CTRL_CFG_S autoPcfg;

    nvr_logon_get_auto_ctrl_cfg(0, &autoPcfg);

	
	if (autoPcfg.u8Enable)
		SendDlgItemMessage(hDlg, IDC_SEARCH_AA ,BM_SETCHECK ,BST_CHECKED ,0);
	else
		SendDlgItemMessage(hDlg, IDC_SEARCH_AA ,BM_SETCHECK ,BST_UNCHECKED ,0);
	return 0;
}


static void init_combobox_info(HWND hWnd)
{
	int i = 0;
	int supportProtocal;
	
	g_stSearch.nItemNum = 0;
	g_stSearch.supportProtocalNum =	0;
	g_stSearch.tryTime = 0;
	supportProtocal = nvr_lib_get_pro_table(g_stSearch.stIpncProtocol, MAX_PROTOCAL_NUM);
	if (supportProtocal <= 0)	supportProtocal = 0;
	g_stSearch.supportProtocalNum = supportProtocal;
	
	SendDlgItemMessage(hWnd,IDL_SHAIXUAN, CB_RESETCONTENT, 0, (LPARAM)0);
	SendDlgItemMessage(hWnd, IDL_SHAIXUAN, CB_ADDSTRING, 0, (LPARAM)LTR("所有"));
	for(i = 0; i < g_stSearch.supportProtocalNum; i++) 
	{
		SendDlgItemMessage(hWnd, IDL_SHAIXUAN, CB_ADDSTRING, 0, (LPARAM)GetProtoclaAliasName(g_stSearch.stIpncProtocol[i].name));
	}
	SendDlgItemMessage(hWnd, IDL_SHAIXUAN, CB_SETCURSEL, 0, 0);	
}


static DLGTEMPLATE DlgRemoteDevice =
{
    WS_NONE,
    WS_EX_NONE,
    REMOTE_DEV_DLG_L, REMOTE_DEV_DLG_T, REMOTE_DEV_DLG_WIDITH, REMOTE_DEV_DLG_HEIGHT,
    "远程设备",
    0, 0,
    TABLESIZE(ui_remote_control), NULL,
    0
};


static void add_ch_list(HWND hListview, int nItem, int ch)
{	
	LVITEM item;
	IPNC_NET_SEARCH_RES_S *pstDevInfo = NULL;
	IPNC_PROTOCOL_S		  *pstProtocal = NULL;
	pstDevInfo = GetUserByIndex(nItem);
	if (pstDevInfo == NULL)	
		return;	
	pstProtocal = GetProtocalByIndex(nItem);
	if (pstProtocal == NULL)	
		return;	

	NVR_CHANNEL_CFG_S stChannelCfg;
	memset(&stChannelCfg, 0, sizeof(NVR_CHANNEL_CFG_S));
	stChannelCfg.u32Size = sizeof(NVR_CHANNEL_CFG_INFO_S);
	strncpy(stChannelCfg.szFactoryName, pstProtocal->factoryName, sizeof(stChannelCfg.szFactoryName));
	strncpy(stChannelCfg.szProtocolName, pstProtocal->name, sizeof(stChannelCfg.szProtocolName));
	snprintf(stChannelCfg.szDevName, sizeof(stChannelCfg.szDevName), "Ch%d", ch);
	strncpy(stChannelCfg.szUser, pstProtocal->usrPwd[0].usr, sizeof(stChannelCfg.szUser));
	strncpy(stChannelCfg.szPasswd, pstProtocal->usrPwd[0].pwd, sizeof(stChannelCfg.szPasswd));
	stChannelCfg.u32IpAddr = pstDevInfo->u32DevIp;
	stChannelCfg.u16DataPort = pstDevInfo->u16DevPort;
	stChannelCfg.u16WebPort = 80;
	stChannelCfg.u16RtspPort = 554;
	stChannelCfg.u8RemoteChn = 1;
	stChannelCfg.u8UseChn = 0;
	stChannelCfg.u8Enable = 1;
	stChannelCfg.u8SearchA = 0;//智能搜索标记、
	
	nvr_logon_set_channel_cfg(ch, &stChannelCfg);	
	SendMessage (hListview, MSG_FREEZECTRL, TRUE, 0);
	item.nItemHeight = 20;
	item.nItem = ch;	
	add_ch_item_list(hListview, &item, &stChannelCfg, ch);	
	SendMessage (hListview, MSG_FREEZECTRL, FALSE, 0);	
	
}

static void delete_ch_list(HWND hListview, int ch)
{	
	HLVITEM hlvi;
	NVR_CHANNEL_CFG_S chCfg;
	//nvr_logon_get_channel_cfg(ch, &chCfg);
	//chCfg.u8Enable = 0;
	memset(&chCfg, 0, sizeof(NVR_CHANNEL_CFG_S));
	nvr_logon_set_channel_cfg(ch, &chCfg);
	
	hlvi = SendMessage (hListview, LVM_GETSELECTEDITEM, 0, 0);
	SendMessage (hListview, LVM_DELITEM, 0, (LPARAM)hlvi );
}

static void set_listview_all_check(HWND hWnd, int status)
{
	int count = 0;
	count = SendMessage(hWnd, LVM_GETITEMCOUNT, 0, 0);

	LVSUBITEM lvSubitem;
	int i;
	for(i=0; i<count; i++)
	{
		lvSubitem.subItem = 0;
		lvSubitem.nItem = i;
		lvSubitem.flags = status;
		SendMessage (hWnd, LVM_SETCHECKMARK, 0, (LPARAM)&lvSubitem);
	}

}

static void lv1_notify_process (HWND hwnd, int id, int code, DWORD addData)
{
	int listIndex;
	if (code == LVN_ITEMDBCLK) 
	{   
		HLVITEM hlvi = SendMessage (hwnd, LVM_GETSELECTEDITEM, 0, 0);
        if (hlvi > 0) 
		{
			int index = 0;
			LVSUBITEM lvSubitem;
			lvSubitem.subItem = 1;
		    lvSubitem.pszText = (char *)malloc (128);
		    if ( lvSubitem.pszText == NULL) 
		        return ;
			SendMessage (hwnd, LVM_GETSUBITEMTEXT, hlvi, (LPARAM)&lvSubitem);	//获取到列表中的index	
			index = atoi(lvSubitem.pszText)-1;
			if (lvSubitem.pszText) free(lvSubitem.pszText);
			
			listIndex = FindIpncIndex(gRemoteWin, index);
			
			CreateIpncCfgDlgWin(hwnd, listIndex);
        }
	}
	else if(code == LVN_CHECKBOX_SELECTALL)
	{
		set_listview_all_check(hListView1, LVFLAG_CHECKED);//全选
		
	}
	else if(code == LVN_CHECKBOX_UNSELECTALL)
	{
		set_listview_all_check(hListView1, LVFLAG_BLANK);//全不不选
	}

}

static void lv2_notify_process (HWND hwnd, int id, int code, DWORD addData)
{
	char acText[64];
	int ch = 0;
	int count = 0, i = 0;
	HLVITEM hItem; 
	LVSUBITEM lvSubitem;
	NVR_CHANNEL_CFG_S chCfg;
	
	if (code == LVN_ITEMDBCLK) 
	{
		HLVITEM hlvi = SendMessage (hwnd, LVM_GETSELECTEDITEM, 0, 0);
        if (hlvi > 0) 
		{
			int chn = 0;
			LVSUBITEM lvSubitem;
			lvSubitem.subItem = 1;
		    lvSubitem.pszText = (char *)malloc (128);
		    if ( lvSubitem.pszText == NULL) 
		        return ;
			SendMessage (hwnd, LVM_GETSUBITEMTEXT, hlvi, (LPARAM)&lvSubitem);		
			chn = atoi(lvSubitem.pszText);
			if (lvSubitem.pszText) free(lvSubitem.pszText);

			CreateDevCfgDlgWin(hwnd, (chn-1));
			init_ch_list(GetParent(hwnd), hListView2);
			InvalidateRect(hwnd, NULL, TRUE);
        }
        
	}
	else if(code == LVN_CHECKBOX_SELECT)/*启用连接*/
	{
		hItem = SendMessage(hListView2, LVM_GETEDITEM, 0, addData);

		lvSubitem.subItem = 1;
		lvSubitem.pszText = acText;
		SendMessage (hListView2, LVM_GETSUBITEMTEXT, hItem, (LPARAM)&lvSubitem);		
		ch = atoi(lvSubitem.pszText)-1;
		
		nvr_logon_get_channel_cfg(ch, &chCfg);
		chCfg.u8Enable = 1;
		nvr_logon_set_channel_cfg(ch, &chCfg);
		
	}
	else if(code == LVN_CHECKBOX_SELCANCEL)/*断开连接*/
	{
		hItem = SendMessage(hListView2, LVM_GETEDITEM, 0, addData);

		lvSubitem.subItem = 1;
		lvSubitem.pszText = acText;
		SendMessage (hListView2, LVM_GETSUBITEMTEXT, hItem, (LPARAM)&lvSubitem);		
		ch = atoi(lvSubitem.pszText)-1;
		
		nvr_logon_get_channel_cfg(ch, &chCfg);
		chCfg.u8Enable = 0;
		nvr_logon_set_channel_cfg(ch, &chCfg);
	}
	else if(code == LVN_CHECKBOX_SELECTALL)
	{
		set_listview_all_check(hListView2, LVFLAG_CHECKED);//全选
		count = SendMessage(hListView2, LVM_GETITEMCOUNT, 0, 0);
		//printf("$$$$$$$$$$$count: %d\r\n",count);
		
		for(i=0; i<count; i++)
		{
			lvSubitem.subItem = 1;
			lvSubitem.nItem = i;
			lvSubitem.pszText = acText;
			SendMessage (hListView2, LVM_GETSUBITEMTEXT, 0, (LPARAM)&lvSubitem); 	
			ch = atoi(lvSubitem.pszText) -1;
			//printf("$$$$$$$$$$$ch: %d\r\n",ch);

			nvr_logon_get_channel_cfg(ch, &chCfg);
			chCfg.u8Enable = 1;
			nvr_logon_set_channel_cfg(ch, &chCfg);
		}
		
	}
	else if(code == LVN_CHECKBOX_UNSELECTALL)
	{
		set_listview_all_check(hListView2, LVFLAG_BLANK);//全不不选

		count = SendMessage(hListView2, LVM_GETITEMCOUNT, 0, 0);
		//printf("$$$$$$$$$$$count: %d\r\n",count);
		
		for(i=0; i<count; i++)
		{
			lvSubitem.subItem = 1;
			lvSubitem.nItem = i;
			lvSubitem.pszText = acText;
			SendMessage (hListView2, LVM_GETSUBITEMTEXT, 0, (LPARAM)&lvSubitem); 	
			ch = atoi(lvSubitem.pszText) -1;
			//printf("$$$$$$$$$$$ch: %d\r\n",ch);

			nvr_logon_get_channel_cfg(ch, &chCfg);
			chCfg.u8Enable = 0;
			nvr_logon_set_channel_cfg(ch, &chCfg);
		}
	}

}

void static CheckSearchAAProc(HWND hDlg)
{
	int ret = 0;
	
	NVR_DEV_AUTO_CTRL_CFG_S autoPcfg;

	ret = SendDlgItemMessage(hDlg, IDC_SEARCH_AA, BM_GETCHECK, 0 , 0);
	nvr_logon_get_auto_ctrl_cfg(0, &autoPcfg);
	autoPcfg.u8Enable = (ret == BST_CHECKED)?1:0;
	nvr_logon_set_auto_ctrl_cfg(0, &autoPcfg);

	//init_ch_list(hDlg, hListView2);
}
#if 0
static void set_listview_all_status(HWND hWnd, char *status)
{
	int count = 0;
	count = SendMessage(hWnd, LVM_GETITEMCOUNT, 0, 0);

	LVSUBITEM lvSubitem;
	int i;
	for(i=0; i<count; i++)
	{
		lvSubitem.subItem = 4;
		lvSubitem.nItem = i;
		lvSubitem.pszText = status;
		SendMessage (hWnd, LVM_SETSUBITEMTEXT, 0, (LPARAM)&lvSubitem);
	}

}
#endif
static void UISearchStop(HWND hDlg)
{
	int i = 0;	
	if (IsTimerInstalled(hDlg, ID_SEARCH_TIMER))
		KillTimer (hDlg, ID_SEARCH_TIMER);	
	if ((g_stSearch.supportProtocalNum <= 0) || 
		(g_stSearch.supportProtocalNum > MAX_PROTOCAL_NUM))
		return;
	
	for(i = 0; i < g_stSearch.supportProtocalNum; i++)
	{
		if (g_stSearch.lHandleArray[i] == 0) continue;
		//nvr_lib_search_stop(g_stSearch.lHandleArray[i]);
		ipnc_client_stop_search(g_stSearch.lHandleArray[i]);
		ipnc_client_disconnect_server(g_stSearch.lHandleArray[i]);
		g_stSearch.lHandleArray[i] = 0;
	}

	printf("UISearchStop##########\r\n");
}

static void UISearchTimerProc(HWND hDlg)
{
	int ret = 0, i = 0;	
	LVITEM item;

	SendMessage (hListView1, MSG_FREEZECTRL, TRUE, 0);
	item.nItemHeight = 20;
	
	IPNC_NET_SEARCH_RES_S stSearchRes;
	for (i = 0; i < g_stSearch.supportProtocalNum; i++)
	{
		if (g_stSearch.lHandleArray[i] == 0)	
			continue;		
		ret = ipnc_client_start_search(g_stSearch.lHandleArray[i]);
		if(ret < 0)	continue;
		
		int n1 = 0;
		while(1)
		{			
			//ret = nvr_lib_search_get(g_stSearch.lHandleArray[i], &stSearchRes);
			ret = ipnc_client_get_search(g_stSearch.lHandleArray[i], &stSearchRes);
			if (ret <= 0)	break;
			ret = AddUserList(&stSearchRes, &g_stSearch.stIpncProtocol[i]);
			if(ret < 0)
				break;
			item.nItem = g_stSearch.nItemNum++;		
			add_device_item_list(hListView1, &item, &stSearchRes, &g_stSearch.stIpncProtocol[i]);
			n1++;
			if (n1> 10) break;
		}
	}
	
	SendMessage (hListView1, MSG_FREEZECTRL, FALSE, 0);
	
	g_stSearch.tryTime++;
	if (g_stSearch.tryTime >= 40)
	{
		UISearchStop(hDlg);
	}

}

const RECT comboxRect={_DEV_LIST_R1-110, _DEV_LIST_B1+5, _DEV_LIST_R1-10,_DEV_LIST_B1+26};

static void UISearchStart(HWND hDlg)
{
	long lHandle;
	int i = 0;
	int nSelch;
	int ret;
	
	ClearUserList();
	SendMessage (hListView1, LVM_DELALLITEM, 0, 0);
	g_stSearch.nItemNum = 0;	
	g_stSearch.tryTime = 0;;

	nSelch = SendDlgItemMessage (hDlg, IDL_SHAIXUAN, CB_GETCURSEL, 0, 0);
	if (nSelch != 0)
	{
		SendDlgItemMessage(hDlg, IDL_SHAIXUAN, CB_SETCURSEL, 0, 0);	//设备搜索后显示"所有"
		InvalidateRect(hDlg, &comboxRect, FALSE);
	}
	
	if (IsTimerInstalled(hDlg, ID_SEARCH_TIMER))
		UISearchStop(hDlg);
	//支持协议数大于最大协议书数
	if (g_stSearch.supportProtocalNum >= MAX_PROTOCAL_NUM)	
		return;
	
	for (i = 0; i < g_stSearch.supportProtocalNum; i++)
	{
		g_stSearch.lHandleArray[i] = 0;
		ret = ipnc_client_connect_server(g_stSearch.stIpncProtocol[i].name, &lHandle);
		if(ret < 0)	continue;
		
		//lHandle = nvr_lib_search_start(g_stSearch.stIpncProtocol[i].name);	//开始搜索
		//if (lHandle == 0) //失败
		//	continue;
		g_stSearch.lHandleArray[i] = lHandle;
	}	
	SetTimer (hDlg, ID_SEARCH_TIMER, 10);	
}


#ifdef CUSTOM_DRAW
static void my_draw_hdr_bk (HWND hWnd, HLVHDR hlvhdr, HDC hdc, RECT *rcDraw)
{
    SetBrushColor (hdc, LISTVIEW_HEAD_BKC);
    FillBox (hdc, rcDraw->left, rcDraw->top, RECTWP(rcDraw), RECTHP(rcDraw));
}

static void my_draw_hdr_item (HWND hWnd, int idx, HDC hdc, RECT *rcDraw)
{
	DWORD dwStyle = GetWindowStyle(hWnd);
	
	SetPenColor(hdc,0xff575859);
	if(dwStyle & LVS_AUTOCHECKBOX)
	    Rectangle (hdc, rcDraw->left - 21, rcDraw->bottom, rcDraw->right, rcDraw->bottom);
    else 
	    Rectangle (hdc, rcDraw->left, rcDraw->bottom, rcDraw->right, rcDraw->bottom);
	    
    Rectangle (hdc, rcDraw->right, rcDraw->top, rcDraw->right, rcDraw->bottom);
    SetBkMode (hdc, BM_TRANSPARENT);
	SetTextColor(hdc, PIXEL_lightwhite);
	if (hWnd == hListView1)
    	TextOut (hdc, rcDraw->left+4, rcDraw->top+4, LTR(caption[idx]));
	else if(hWnd == hListView2)
		TextOut (hdc, rcDraw->left+4, rcDraw->top+4, LTR(caption2[idx]));
}
#endif


static void RemoteDlgInit(HWND hDlg)
{
	HWND hCtrl;
	ClearUserList();

	if (NeedWizard())
	{
		hCtrl = GetDlgItem(hDlg, IDC_OK);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hDlg, IDC_CANCEL);
		ShowWindow(hCtrl,SW_HIDE);
	}
	else
	{
		hCtrl = GetDlgItem(hDlg, IDC_REMOTE_PREV);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hDlg, IDC_REMOTE_NEXT);
		ShowWindow(hCtrl,SW_HIDE);
		hCtrl = GetDlgItem(hDlg, IDC_REMOTE_WIZARD_CANCEL);
		ShowWindow(hCtrl,SW_HIDE);
	}

	hListView1 = GetDlgItem (hDlg, IDC_LISTVIEW1);
	hListView2 = GetDlgItem (hDlg, IDC_LISTVIEW2);		
    SetNotificationCallback (hListView1, lv1_notify_process);		
    SetNotificationCallback (hListView2, lv2_notify_process);
	init_combobox_info(hDlg);		
	init_device_list_header(hListView1);
	init_ch_list_header(hListView2);		
	init_ch_list(hDlg, hListView2);	
#ifdef CUSTOM_DRAW
    LVCUSTOMDRAWFUNCS myfuncs = {my_draw_hdr_bk, my_draw_hdr_item};

    SendMessage (hListView1, LVM_SETCUSTOMDRAW, 0, (LPARAM) &myfuncs);	
    SendMessage (hListView2, LVM_SETCUSTOMDRAW, 0, (LPARAM) &myfuncs);
#endif
}

static void UpdateRemoteDevList(HWND hDlg, int nSelch)
{
	int i;
	char protocalAliasName[32];
	LVITEM item;
	
	SendDlgItemMessage(hDlg, IDL_SHAIXUAN, CB_GETLBTEXT, nSelch, (LPARAM)protocalAliasName);
	SendMessage (hListView1, LVM_DELALLITEM, 0, 0);
	item.nItemHeight = 20;
	item.nItem = 0;
	
	if (strcmp(protocalAliasName, LTR("所有")) == 0)
	{			
		SendMessage (hListView1, MSG_FREEZECTRL, TRUE, 0);
		for (i = 0; i < g_stDevMan.UserNum; i++)
		{
			add_device_item_list(hListView1, &item, &g_stDevMan.stUserInfo[i], &g_stDevMan.stDevProtocal[i]);	
			item.nItem++;	
		}		
		SendMessage (hListView1, MSG_FREEZECTRL, FALSE, 0);
		return;
	}
	
	SendMessage (hListView1, MSG_FREEZECTRL, TRUE, 0);
	for (i = 0; i < g_stDevMan.UserNum; i++)
	{
		if (strcmp(protocalAliasName, g_stDevMan.stDevProtocal[i].aliasName) == 0)
		{
			add_device_item_list(hListView1, &item, &g_stDevMan.stUserInfo[i], &g_stDevMan.stDevProtocal[i]);		
			item.nItem++;	
		}
	}	
	SendMessage (hListView1, MSG_FREEZECTRL, FALSE, 0);
}

static void GetCheckmarkNumAndRow(HWND hDlg, int rowNum, int *chNum, LivSerialNum_t *checkArray)
{
	int i = 0, status = 0;
	LVSUBITEM lvSubitem;
	char acText[64] = {0};

	if((checkArray == NULL) || (chNum == NULL))
		return ;
		
	for(i=0; i<rowNum; i++)    //获取列表中选择复选框的序号和行数
	{
		lvSubitem.subItem = 0;
		lvSubitem.nItem = i;
		lvSubitem.pszText = acText;

		status = SendMessage (hDlg, LVM_GETCHECKMARK, 0, (LPARAM)&lvSubitem);
		if(status == LVFLAG_CHECKED)
		{
			lvSubitem.subItem = 1;//获取第二列显示的行号	
			SendMessage (hDlg, LVM_GETSUBITEMTEXT, 0, (LPARAM)&lvSubitem); 	
			checkArray[*chNum].devNum = atoi(lvSubitem.pszText)-1;
			checkArray[*chNum].livNum = i;
			*chNum += 1;
		}
	}
}
#if 0
static void CheckEnableConnect(HWND hDlg)
{
	int chanCount, i, status = 0, channel = 0;
	LVSUBITEM lvSubitem;
	NVR_CHANNEL_CFG_S chCfg;
	char acText[64] = {0};

	chanCount = SendMessage(hDlg, LVM_GETITEMCOUNT, 0, 0);
	if((chanCount < 0) || (chanCount > NVR_MAX_CHANNEL_NUM))
		return;

	for(i=0; i<chanCount; i++)
	{
		lvSubitem.subItem = 0;
		lvSubitem.nItem = i;
		lvSubitem.pszText = acText;

		status = SendMessage (hDlg, LVM_GETCHECKMARK, 0, (LPARAM)&lvSubitem);
		
		lvSubitem.subItem = 1;//获取第二列的通道号
		SendMessage (hDlg, LVM_GETSUBITEMTEXT, 0, (LPARAM)&lvSubitem); //获取第一列显示的行号	
		channel = atoi(lvSubitem.pszText);
		memset(&chCfg, 0, sizeof(NVR_CHANNEL_CFG_S));
		nvr_logon_get_channel_cfg(channel, &chCfg);

		if(status == LVFLAG_CHECKED)
		{
			chCfg.u8Enable = 1;
		}
		else
		{
			chCfg.u8Enable = 0;
		}
		nvr_logon_set_channel_cfg(channel, &chCfg);

	}
	
}
#endif
static int RemoteDeviceWinProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int i = 0,checkNum = 0;
	
    switch (message)
    {

    case MSG_INITDIALOG:
    {		
    	gRemoteWin = hDlg;
		RemoteDlgInit(hDlg);

    	push_open_dlg(&headOpenDlg, hDlg);
    	//show_open_dlg(headOpenDlg);
        return 0;
    }
#if 0	
	case MSG_PAINT:
	{
		HDC hdc;		
        hdc = BeginPaint (hDlg);		
        EndPaint (hDlg, hdc);		
		Gui_DrawCaption(hDlg, "远程设备");
        return 0;
	}
#endif	
	case MSG_ERASEBKGND:
	{
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
		Gui_WriteTitleName(hdc, &remote_title_rect, LTR("远程设备"));
		if (fGetDC) 
			ReleaseDC (hdc); 
		return 0; 
	}
	break;
	case MSG_RBUTTONUP:
    {
		PostMessage(hDlg, MSG_CLOSE, 0, 0);
		
        return 0;
    }

    case MSG_COMMAND:
    {	
		int id   = LOWORD(wParam);
        int code = HIWORD(wParam);
		
		if (id == IDL_SHAIXUAN)
		{		
			if (code == CBN_SELCHANGE)
			{	
				int nSelch;				
				nSelch = SendDlgItemMessage (hDlg, IDL_SHAIXUAN, CB_GETCURSEL, 0, 0);
				if (nSelch < 0) return 0;
				UpdateRemoteDevList(hDlg, nSelch);
				return 0;
			}
			break;
		}
		
		
       	switch (wParam) 
		{
	        case IDC_DEV_SEARCH:
			{	
				UISearchStart(hDlg);
	        }
			break;
			case IDC_SEARCH_AA:
			{
				CheckSearchAAProc(hDlg);
			}
			return 0;
			case IDC_DEV_ADD:
			{
				int ch = 0, freeChNum = 0, index = 0, nNum = 0;
				LivSerialNum_t checkArray[1024] = {{0,0}};

				nNum = SendMessage (hListView1, LVM_GETITEMCOUNT, 0, 0);
				if (nNum <= 0)
					return 0;
				
				GetCheckmarkNumAndRow(hListView1, nNum, &checkNum, checkArray);
				freeChNum = get_free_ch_number();//获取nvr空闲的通道数	 		
				if((freeChNum <= 0)/* || (freeChNum < checkNum)*/)
				{
#ifdef OWN_MESSAGEBOX_WIN
					CreateMessageBoxWin(hDlg, LTR("通道数超过限制!"), LTR("消息"), IDOK);
#else
					MessageBox(hDlg, LTR("通道数超过限制!"), LTR("消息"), IDOK);
#endif
					return 0;
				}

				for(i=0; i < checkNum; i++) //逐一的将勾选的设备信息添加到nvr通道
				{
					index = FindIpncIndex(hDlg, checkArray[i].devNum);
					ch = get_free_ch_index();
					if (ch < 0)
					{
#ifdef OWN_MESSAGEBOX_WIN
						CreateMessageBoxWin(hDlg, LTR("通道数超过限制!"), LTR("消息"), IDOK);
#else
						MessageBox(hDlg, LTR("通道数超过限制!"), LTR("消息"), IDOK);
#endif
						return 0;
					}
					
					add_ch_list(hListView2, index, ch);	
				}
				return 0;
			}
			return 0;
			case IDC_DEV_DELET:
			{
				char acText[64];
				int ch = 0;
				HLVITEM hItem; 
				LVSUBITEM lvSubitem;
				
				hItem = SendMessage (hListView2, LVM_GETSELECTEDITEM, 0, 0);
				
				lvSubitem.subItem = 1;
				lvSubitem.pszText = acText;
				SendMessage (hListView2, LVM_GETSUBITEMTEXT, hItem, (LPARAM)&lvSubitem);		
				ch = atoi(lvSubitem.pszText);
			
				delete_ch_list(hListView2, ch-1); 
				
				return 0;
			}
			case IDC_ADD_MANUAL:
			{
				int ch = 0;
				ch  = get_free_ch_index();
				if (ch < 0)
				{
#ifdef OWN_MESSAGEBOX_WIN
					CreateMessageBoxWin(hDlg, LTR("通道数超过限制!"), LTR("消息"), IDOK);
#else
					MessageBox(hDlg, LTR("通道数超过限制!"), LTR("消息"), IDOK);
#endif
					return 0;
				}
				
				CreateDevCfgDlgWin(hDlg, ch);
				init_ch_list(hDlg, hListView2);
				InvalidateRect(hListView2, NULL, TRUE);
			}
			return 0;
			case IDC_DEV_REFRESH://刷新
			{
				init_ch_list(hDlg, hListView2);
				InvalidateRect(hListView2, NULL, TRUE);
				return 0;
			}
			case IDC_OK:
				//CheckSearchAAProc(hDlg);
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
				//CheckEnableConnect(hListView2);
				return 0;
			case IDC_REMOTE_PREV:	
			case IDC_CANCEL:
				PostMessage(hDlg, MSG_CLOSE, 0, 0);
				return 0;
				
			case IDC_REMOTE_NEXT:
				CreateDiskManageDlgWin(hDlg);
				break;

			case IDC_REMOTE_WIZARD_CANCEL:
				BroadcastMessage(MSG_WIZARD_EXIT, 0 , 0);
				break;
        }
        break;
    }
	case MSG_TIMER:
	{
		switch(wParam)
		{
			case ID_SEARCH_TIMER:
			{
				UISearchTimerProc(hDlg);
				return 0;
			}
		}
		break;
	}
	
	case MSG_WIZARD_EXIT:
	{		
		PostMessage(hDlg, MSG_CLOSE, 0, 0);
	}
	break;

    case MSG_CLOSE:
    {	
    	HWND hWin;
		UISearchStop(hDlg);
		pop_open_dlg(&headOpenDlg, &hWin);
        EndDialog (hDlg, 0);
        return 0;
    }

    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}


void CreateRemoteDeviceDlgWin(HWND hOwner)
{	
	PCTRLDATA ctrolTmp;
	
	ctrolTmp = (PCTRLDATA)malloc(sizeof(ui_remote_control));
	if(NULL == ctrolTmp)
		return ;	
	memcpy(ctrolTmp, ui_remote_control, sizeof(ui_remote_control));

	DLG_CAPTION_LANGUAGE_TRANS(ui_remote_control)
	DlgRemoteDevice.controls = ui_remote_control;
    DialogBoxIndirectParam (&DlgRemoteDevice, hOwner, RemoteDeviceWinProc, 0L);
	memcpy(ui_remote_control, ctrolTmp, sizeof(ui_remote_control));
	free(ctrolTmp);
	ctrolTmp = NULL;
}



