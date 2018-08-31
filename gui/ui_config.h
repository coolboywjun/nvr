#ifndef __UI_CONFIG_H__
#define __UI_CONFIG_H__

#define NVR_SCREEN_WIDTH	1280
#define NVR_SCREEN_HEIGHT	720

#define MY_PIXEL_transparent		0x0022566a

#define PIXEL_EDIT_BKG_NORMAL	0xFFFFFFFF
#define PIXEL_EDIT_BKG_HILIGHT	0xFFFFFFFF
#define PIXEL_EDIT_BKG_DISABLE	0xFFBEBCBB



#define MSG_REPLAY_CLOSE	(MSG_USER+170)
#define MSG_FORMAT_EXIT		(MSG_USER+171)
#define MSG_FORMAT_START	(MSG_USER+172)
#define MSG_LOGIN			(MSG_USER+173)
#define MSG_WIZARD			(MSG_USER+174)
#define MSG_WIZARD_EXIT		(MSG_USER+175)

#define MSG_FACTORY_TEST	(MSG_USER+176)

#define PORT_TEXT_LIMIT     	5
#define USERINFO_TEXT_LIMIT		16

#define MAX_PROTOCAL_NUM 	8


extern BITMAP button_pic[4];

enum
{
	SIZE_1280x720 = 0,
	SIZE_700x500,
	SIZE_600x400,
	SIZE_490x350,
	SIZE_420x180,
	SIZE_500x150,
	BK_NUM_MAX
};


extern BITMAP bk_pic[BK_NUM_MAX];

extern BITMAP main_menu_pic[10];

extern BITMAP advance_menu_pic[10];

extern BITMAP login_pic[3];
extern BITMAP devidmap, androidmap_vivi, androidmap_tutk;

extern unsigned int devIdQrCodeWidth,devIdQrCodeHeight;

enum
{
	EN_COPY_PIC_PARAM = 0,		//¿½±´Í¼Ïñ²ÎÊý
	EN_COPY_ENCODE_PARAM,		//¿½±´±àÂë²ÎÊý
	EN_COPY_STORAGE_STRATEGY,	//¿½±´Â¼Ïñ²ßÂÔ
	EN_COPY_IPC_OSD,			//IPC OSD²ÎÊý
};

int NeedWizard();


#endif
