#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <minigui/cliprect.h>
#include <minigui/internals.h>
#include <minigui/ctrlclass.h>

#include "ctrl_data.h"

#include "ctrl_date.h"
#include "gui_core.h"
#include "ui_config.h"
#ifdef _CTRL_DATE_H

#define DEFAULT_DATE_WIDTH   20
#define DEFAULT_DATE_HEIGHT  20

#define LEFTMARGIN    2  //leftMargin;
#define RIGHTMARGIN  2 // int rightMargin;

#define 	YEAR_WIDTH 8
#define DOT_WIDTH  2
#define LINE_WIDTH  8

typedef struct _tagDATEITEM
{
    int x;
    int y;
    int width;
    int height;
    int max;
    int min;
    char date[8];
    int isSel;
} DATEITEM, *PDATEITEM;

typedef struct _tagDATA
{
    DWORD status;
    DWORD dwStyle;
    HWND hWnd;
    int dspStyle;  //显示风格  默认是2011-12-31 |23:59:59
    gal_pixel color_bg;        // background color
    gal_pixel color_bg_hilight;
    gal_pixel color_ft;
    gal_pixel color_ft_hilight;
    gal_pixel color_boder;
    gal_pixel color_boder_hilight;

    DATEITEM items[6];
    int nItems;       // 总共有几个单元
    int nItemSel;     // 当前处在第几个单元
    RECT mark[5];
    char *caption;
    int data;

    /////// PLOGFONT pLogFont;      // pointer to logical font.
} DATEDATA;

typedef DATEDATA *PDATEDATA;



#define DATE_BE_VALID_ITEM(nPos)    (nPos < pData->nItems && nPos >= 0)

extern BOOL IsCustomImeOpen();

static int __GetNumberOfDays(int year, int month)
{
    switch (month)
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        return(31);
    case 4:
    case 6:
    case 9:
    case 11:
        return(30);
    case 2:
        if (year%4 == 0)
            return(29);
        else
            return(28);
    default:
        return(-1);
    }
}

static int GetDateItems(PDATEDATA pData, ctrl_time_t *time)
{
    //int ret;
    //int i,num;
    //char buf[12] = {0};
    if(pData == NULL || time == NULL)
    {
        return -1;
    }

    time->year = atoi(pData->items[0].date);
    time->mon = atoi(pData->items[1].date);
    time->day = atoi(pData->items[2].date);
    time->hour= atoi(pData->items[3].date);
    time->min = atoi(pData->items[4].date);
    time->sec = atoi(pData->items[5].date);


    printf(" <<<Get date  %d-%d-%d  %d:%d:%d \n"
           , time->year
           , time->mon
           , time->day
           , time->hour
           , time->min
           , time->sec);
    return 0;
}

//时间设置
static int SetDateItems(PDATEDATA pData, ctrl_time_t *time)
{
    //int ret;
    //int i,num;
    //char buf[12] = {0};
    //printf("SetDateItems   111111111111111111111  \n");
    if(pData == NULL || time == NULL)
    {
        return -1;
    }
    snprintf (pData->items[0].date,5,"%04d",time->year);
    snprintf (pData->items[1].date,3,"%02d",time->mon);
    snprintf (pData->items[2].date,3,"%02d",time->day);
    snprintf (pData->items[3].date,3,"%02d",time->hour);
    snprintf (pData->items[4].date,3,"%02d",time->min);
    snprintf (pData->items[5].date,3,"%02d",time->sec);


    printf(" <<<set date  %s-%s-%s  %s:%s:%s \n"
           , pData->items[0].date
           , pData->items[1].date
           , pData->items[2].date
           , pData->items[3].date
           , pData->items[4].date
           , pData->items[5].date);
    return 0;
}

/******时间段的设置********/
static int GetTimeSeg(PDATEDATA pData, time_segment *time_seg)
{
    //int ret;
    //int i,num;
    //char buf[12] = {0};
    if(pData == NULL || time_seg == NULL)
    {
        return -1;
    }

    time_seg->start_hour = atoi(pData->items[0].date);
    time_seg->start_min = atoi(pData->items[1].date);
    time_seg->stop_hour = atoi(pData->items[2].date);
    time_seg->stop_min= atoi(pData->items[3].date);


    printf(" <<<Get time segment  %d:%d -%d:%d \n"
           , time_seg->start_hour
           , time_seg->start_min
           , time_seg->stop_hour
           , time_seg->stop_min
          );
    return 0;
}

static int SetTimeSeg(PDATEDATA pData, time_segment *time_seg)
{
    //int ret;
    //int i,num;
    //char buf[12] = {0};
    if(pData == NULL || time_seg == NULL)
    {
        return -1;
    }
    snprintf (pData->items[0].date,3,"%02d",time_seg->start_hour);
    snprintf (pData->items[1].date,3,"%02d",time_seg->start_min);
    snprintf (pData->items[2].date,3,"%02d",time_seg->stop_hour);
    snprintf (pData->items[3].date,3,"%02d",time_seg->stop_min);

    printf(" <<<set time segment:  %s:%s - %s:%s \n"
           , pData->items[0].date
           , pData->items[1].date
           , pData->items[2].date
           , pData->items[3].date
          );
    return 0;
}


static void sDrawText (HDC hdc, int x, int y, int width, int height,
                       const char *psText, UINT format)
{
    RECT rect;

    if (psText != NULL)
    {
        SetRect (&rect, x, y, x + width, y + height);
        DrawText (hdc, psText, -1, &rect, format);
    }
}

static void DateOnDraw(HWND hwnd, HDC hdc)
{
    RECT rect;

    //gal_pixel oldBrushColor;
    gal_pixel oldPenColor = GetPenColor(hdc);
    gal_pixel oldTextColor;
    //int oldBkMode;

    int i, nItem;
    //int width, height;
    //int rx, ry;
    PDATEITEM  pItem;
    UINT text_format;
    int onFocus;

    PDATEDATA pData;
	PCONTROL pCtrl;
    pCtrl = Control(hwnd);

    pData = (PDATEDATA) GetWindowAdditionalData2 (hwnd);

    if(!pData)
        return;
    onFocus = (pData->status & DATE_FOCUSED)? 1 : 0;

    oldTextColor = GetTextColor(hdc);
    SetBkMode (hdc, BM_TRANSPARENT);


    text_format = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
#if 0
	SetBrushColor (hdc,PIXEL_lightwhite);//填充颜色
	FillBox(hdc,rect.left,rect.top,RECTW(rect),RECTH(rect));

    //width = rect.right-rect.left;
    //height = rect.bottom-rect.top;

    if(onFocus)
    {
        oldPenColor = SetPenColor (hdc, pData->color_boder_hilight);
    }
    else
    {
        oldPenColor = SetPenColor (hdc, pData->color_boder);
    }

    //画外框
    Rectangle(hdc, rect.left, rect.top,rect.right -1, rect.bottom -1);
    //RoundRect(hdc, rect.left , rect.top, width, height, rx, ry);
    //RoundRect (hdc, rect.left + 1, rect.top + 1, width - 2, height - 2, rx - 1, ry - 1);
#endif
	GetClientRect (hwnd, &rect);

	if  (pCtrl->dwStyle & WS_DISABLED)
	{
	   SetBrushColor(hdc, PIXEL_EDIT_BKG_DISABLE);
	}
	else
	{
	   SetBrushColor(hdc, PIXEL_EDIT_BKG_NORMAL);
	}
#ifdef _ROUND_RECT_STYLE
	RoundRect (hdc, rect.left,rect.top, RECTW(rect) - 1,RECTH(rect) - 1, 3, 3);
#else
	FillBox(hdc,rect.left , rect.top, rect.right ,rect.bottom);
#endif

    for(nItem = 0; nItem < pData->nItems; nItem++)
    {
		if (pCtrl->dwStyle & WS_DISABLED)
		{			
            SetTextColor(hdc, PIXEL_darkgray); // color front;
		}
		else
		{
	        if (nItem == pData->nItemSel)
	        {
	            if(onFocus)
	            {
	                SetTextColor (hdc, pData->color_ft_hilight); // color selected;
	                if (pData->items[nItem].isSel)
	                {
	                    SetBkColor (hdc, pData->color_bg_hilight);
	                    SetBkMode (hdc, BM_OPAQUE);
	                }
	            }
	            else
	            {
	                SetTextColor(hdc, pData->color_ft);
	                SetBkMode (hdc, BM_TRANSPARENT);
	            }
	        }
	        else
	        {
	            SetTextColor(hdc, pData->color_ft); // color front;
	            SetBkMode (hdc, BM_TRANSPARENT);
	        }
		}
        pItem = &pData->items[nItem];
        sDrawText (hdc, pItem->x , pItem->y, pItem->width, pItem->height, pItem->date, text_format);
    }
	//画内部符号
    /// SetBrushColor (hdc, COLOR_lightwhite);    
	if  (pCtrl->dwStyle & WS_DISABLED)
		SetBrushColor (hdc, PIXEL_darkgray);
	else
    	SetBrushColor (hdc, COLOR_black);
    if(pData->dwStyle&DE_TIME_SEG)
    {
        for(i = 0; i < pData->nItems  - 1; i++)
        {			
			SetPenColor(hdc,PIXEL_darkgray);	
            if( i == 1)
                //画个矩形表示一横线
                Rectangle(hdc, pData->mark[i].left + (LINE_WIDTH>>1) ,
                          (RECTH(pData->mark[i])>>1) -1,
                          pData->mark[i].right - (LINE_WIDTH>>1),
                          RECTH(pData->mark[i])>>1 );
            else
            {
                // //画圆点就用这个
                FillCircle (hdc, pData->mark[i].left + (RECTW(pData->mark[i])>>1)  , pData->mark[i].top+(RECTH(pData->mark[i])>>1) -3, 1);
                FillCircle (hdc, pData->mark[i].left + (RECTW(pData->mark[i])>>1), pData->mark[i].top+(RECTH(pData->mark[i])>>1) +3, 1);

            }


        }
    }
    else
    {

        for(i = 0; i < pData->nItems  - 1; i++)
        {
            if( i < 2)
            {
                //画个矩形表示一横线                
				SetPenColor(hdc,PIXEL_darkgray);	
                Rectangle(hdc, pData->mark[i].left ,
                          (RECTH(pData->mark[i])>>1) -1,
                          pData->mark[i].right ,
                          RECTH(pData->mark[i])>>1 );
				//printf("#####x0 = %d, y0 = %d, x1 = %d, y1 = %d\r\n", pData->mark[i].left,
				//	(RECTH(pData->mark[i])>>1) -1, pData->mark[i].right, RECTH(pData->mark[i])>>1);
            }
            else if(i == 2)
            {	
				continue;
				SetPenColor(hdc,PIXEL_darkgray);	
                Rectangle(hdc, pData->mark[i].left ,
                          pData->mark[i].top,
                          pData->mark[i].left +1,
                          pData->mark[i].bottom );
				//printf("@@@@x0 = %d, y0 = %d, x1 = %d, y1 = %d\r\n", pData->mark[i].left ,
                //          pData->mark[i].top, pData->mark[i].left +1, pData->mark[i].bottom);
            }
            else if(i > 2)
            {
                // //画圆点就用这个
                FillCircle (hdc, pData->mark[i].left + (RECTW(pData->mark[i])>>1)  , pData->mark[i].top+(RECTH(pData->mark[i])>>1) -3, 1);
                FillCircle (hdc, pData->mark[i].left + (RECTW(pData->mark[i])>>1  ), pData->mark[i].top+(RECTH(pData->mark[i])>>1) +3, 1);

            }


        }

    }
	
    /*oldBkMode = */SetBkMode (hdc, BM_OPAQUE);
    SetTextColor (hdc, oldTextColor);
    SetPenColor (hdc, oldPenColor);
    //SetBrushColor (hdc, oldBrushColor);
    //add by jason 20110726
    CreateCaret (hwnd, NULL, 1, pCtrl->pLogFont->size);

    return;
}



/*************************  data init  ************************/
static void InitDateData (HWND hwnd, PDATEDATA pData, DWORD dwStyle)
{
    int i;
    //int IntervalWidth;
    int  width_need = DEFAULT_DATE_WIDTH;//单个编辑框的最小宽度
    ////  int  date_height = height;
    RECT rect;
    PCONTROL pCtrl;
    pCtrl = Control(hwnd);
    CreateCaret (hwnd, NULL, 1, pCtrl->pLogFont->size);


    pData->hWnd = hwnd;
    pData->dwStyle = dwStyle;
    //pData->dwStyle = GetWindowStyle(hwnd);//风格
    pData->caption = (char *)GetWindowCaption(hwnd);
    pData->data = GetWindowAdditionalData(hwnd);

    pData->nItemSel = 0;//当前选中的

    if(pData->data != 0) //附加数据表示宽度
    {
        printf("InitIpAddressData  pData->data = %d pCtrl->dwAddData  = %ld\n",pData->data,pCtrl->dwAddData);
        //IntervalWidth = pData->data;
    }
    else
        ;//IntervalWidth  = DOT_WIDTH;





#if 0
#else
    //pData->color_bg = PIXEL_green;//BKC_EDIT_NORMAL);
    pData->color_bg =   PIXEL_lightwhite;
    pData->color_bg_hilight = 0xFFBEBEBE;

    pData->color_ft = PIXEL_black;
    pData->color_ft_hilight = PIXEL_black;

    pData->color_boder = BORDER_NORMAL_COLOR;
    pData->color_boder_hilight = BORDER_FOCUS_COLOR;
#endif
    //// SetWindowBkColor(hwnd, pData->color_bg);
/////   pData->IntervalWidth = IntervalWidth;
    GetClientRect (hwnd, &rect);

    // if(dwStyle & DE_DATE_TIME )
    {
        pData->nItems = 6;//总共的  年月日时分秒
        int date_width;//, rect_width,rect_height;

        date_width  =  (RECTW (rect)-YEAR_WIDTH - DOT_WIDTH*2 -LINE_WIDTH*2 -LEFTMARGIN - RIGHTMARGIN -2)/6 ;
        if(width_need > date_width)
        {
            printf(" current date_width is too small !!! width_need  =%d  date_width =%d \n",width_need , date_width);
            return ;
        }
        //pData->items[0].date ="1970";
        //pData->items[1].date ="01";
        //pData->items[2].date ="01";
        //pData->items[3].date ="23";
        //pData->items[4].date ="59";
        ///pData->items[5].date ="59";

        for(i = 0; i < 6; i++)
        {
            pData->items[i].y = 0;
            if(i == 0)
                pData->items[i].width = date_width + YEAR_WIDTH;
            else
                pData->items[i].width = date_width;
            pData->items[i].height = RECTH (rect);

            if(i < 5)
            {
                pData->mark[i].top  = 0;
                pData->mark[i].bottom = RECTH (rect);
            }
        }


        pData->items[0].x = LEFTMARGIN;      //年
        pData->items[1].x = LEFTMARGIN + date_width + YEAR_WIDTH + LINE_WIDTH;  //月
        pData->items[2].x = pData->items[1].x + date_width + LINE_WIDTH;  // 日
        pData->items[3].x = pData->items[2].x + date_width +2;
        pData->items[4].x = pData->items[3].x +date_width +DOT_WIDTH;
        pData->items[5].x = pData->items[4].x + date_width + DOT_WIDTH;

        pData->mark[0].left   = LEFTMARGIN + date_width+YEAR_WIDTH;
        pData->mark[0].right = LEFTMARGIN + date_width+YEAR_WIDTH + LINE_WIDTH;

        pData->mark[1].left    = pData->items[1].x + date_width;
        pData->mark[1].right  = pData->items[1].x + date_width + LINE_WIDTH;

        pData->mark[2].left   = pData->items[2].x+date_width;
        pData->mark[2].right = pData->items[2].x+date_width+2;

        pData->mark[3].left  = pData->items[3].x+date_width;
        pData->mark[3].right =pData->items[3].x+date_width+DOT_WIDTH;

        pData->mark[4].left  =  pData->items[4].x+date_width;
        pData->mark[4].right = pData->items[4].x+date_width+DOT_WIDTH;

    }


    return;
}

/**************init time seg**************************/
static void InitTimeSegData (HWND hwnd, PDATEDATA pData, DWORD dwStyle)
{
    int i;
    //int IntervalWidth;
    int  width_need = DEFAULT_DATE_WIDTH;//单个编辑框的最小宽度
    ////  int  date_height = height;
    RECT rect;
    PCONTROL pCtrl;
    pCtrl = Control(hwnd);
    CreateCaret (hwnd, NULL, 1, pCtrl->pLogFont->size);


    pData->hWnd = hwnd;
    pData->dwStyle = dwStyle;
    pData->caption = (char *)GetWindowCaption(hwnd);
    pData->data = GetWindowAdditionalData(hwnd);

    pData->nItemSel = 0;//当前选中的

    if(pData->data != 0) //附加数据表示宽度
    {
        printf("InitIpAddressData  pData->data = %d pCtrl->dwAddData  = %ld\n",pData->data,pCtrl->dwAddData);
        //IntervalWidth = pData->data;
    }
    else
        ;//IntervalWidth  = DOT_WIDTH;

    pData->color_bg =   PIXEL_black;
    pData->color_bg_hilight = 0xFFBEBEBE;

    pData->color_ft = PIXEL_black;
    pData->color_ft_hilight = PIXEL_black;

    pData->color_boder = BORDER_NORMAL_COLOR;
    pData->color_boder_hilight = BORDER_FOCUS_COLOR;


    GetClientRect (hwnd, &rect);

    // if(dwStyle & DE_DATE_TIME )
    {
        pData->nItems = 4;//有两个时间段
        int date_width;//, rect_width,rect_height;

        date_width  =  (RECTW (rect) - DOT_WIDTH*2 -LINE_WIDTH*2 -LEFTMARGIN - RIGHTMARGIN -2)/4 ;
        if(width_need > date_width)
        {
            printf(" current date_width is too small !!! width_need  =%d  date_width =%d \n",width_need , date_width);
            return ;
        }

        for(i = 0; i < 4; i++)
        {
            pData->items[i].y = 0;

            pData->items[i].width = date_width;
            pData->items[i].height = RECTH (rect);

            if(i < 3)
            {
                pData->mark[i].top  = 0;
                pData->mark[i].bottom = RECTH (rect);
            }
        }


        pData->items[0].x = LEFTMARGIN;
        pData->items[1].x = LEFTMARGIN + date_width + DOT_WIDTH;
        pData->items[2].x = pData->items[1].x + date_width + LINE_WIDTH*2;

        pData->items[3].x = pData->items[2].x + date_width + DOT_WIDTH;

        pData->mark[0].left   = LEFTMARGIN + date_width;
        pData->mark[0].right =  pData->items[1].x;

        pData->mark[1].left    = pData->items[1].x + date_width;
        pData->mark[1].right  = pData->items[2].x ;

        pData->mark[2].left   = pData->items[2].x+date_width;
        pData->mark[2].right = pData->items[3].x;

    }


    return;
}

//Destroies all the internal datas
static void DateDataDestory (PDATEDATA pData)
{
    if(pData)
        free (pData);
}


static int get_nItem_withxy(PDATEDATA pData, int x, int y)
{
    int i;
    PDATEITEM pItem;

    if(pData)
    {
        for(i = 0; i < pData->nItems; i++)
        {
            pItem = &pData->items[i];
            if( x >= pItem->x && x <= pItem->x + pItem->width &&
                    y >= pItem->y && y <= pItem->y + pItem->height)
                return i;
        }
    }

    return -1;
}

static void CheckDateValid(PDATEDATA pData,int *inc_flag)
{
    PDATEITEM pItem;
    int nItem;
    int i;

    nItem = pData->nItemSel;
    pItem = &pData->items[nItem];

    i = atoi(pItem->date);
    //printf(">>>>>>>>CheckDateValid  AddCharToDateItem current nItem = %d  \n",nItem);
    switch(nItem)
    {
    case 0:
    {
        ////if((pData->dwStyle &DE_DATE_MODE  || pData->dwStyle &DE_DATE_TIME )&&(strlen(pItem->date) >= 4))
        if(pData->dwStyle &DE_DATE_MODE  || pData->dwStyle &DE_DATE_TIME )  // 年
        {
            if(i < 1970 )
            {
                i = 1970;
                snprintf(pItem->date, 5, "%04d", i);
            }
            else if( i > 2999)
            {
                i = 2999;
                snprintf(pItem->date, 5, "%04d", i);
            }
            *inc_flag = 1;
        }
        ////else if((pData->dwStyle &DE_TIME_MODE)&&(strlen(pItem->date) >= 2))
        else if(pData->dwStyle &DE_TIME_MODE) //小时
        {

            if( i > 23)
            {
                i = 0;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if( i< 0)
            {
                i = 23;
                snprintf(pItem->date, 3, "%02d", i);

            }
            *inc_flag = 1;
        }
        break;
    }
    case 1:
    {
        ///if(strlen(pItem->date) <  2)
        ///	break;
        if((pData->dwStyle &DE_DATE_MODE  || pData->dwStyle &DE_DATE_TIME )) // 月
        {
            if(i < 1)
            {
                i = 12;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if(i > 12)
            {
                i = 1;
                snprintf(pItem->date, 3, "%02d", i);
            }

        }
        else if(pData->dwStyle &DE_TIME_MODE) // 分
        {

            if(i >= 60)
            {
                i = 0;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if( i< 0)
            {
                i = 59;
                snprintf(pItem->date, 3, "%2d", i);
            }

        }
        *inc_flag = 1;
        break;
    }
    case 2:
    {
        ///if(strlen(pItem->date) <  2)
        ////	break;

        if(pData->dwStyle &DE_DATE_MODE  || pData->dwStyle &DE_DATE_TIME )// 日
        {
            if(i <= 0)
            {
                i = __GetNumberOfDays(atoi(pData->items[0].date),atoi(pData->items[1].date));
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if(i > __GetNumberOfDays(atoi(pData->items[0].date),atoi(pData->items[1].date)))
            {
                i = 1;
                snprintf(pItem->date, 3, "%02d", i);
            }

        }
        else if(pData->dwStyle &DE_TIME_MODE)//秒
        {

            if(i >= 60)
            {
                i = 59;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if(i <  0)
            {
                i = 0;
                snprintf(pItem->date, 3, "%02d", i);

            }

        }
        *inc_flag = 1;
        break;
    }
    case 3:
    {

        ///if(strlen(pItem->date) <  2)
        ////	break;

        if(pData->dwStyle &DE_DATE_TIME) //小时
        {
            if( i > 23)
            {
                i = 0;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if( i < 0)
            {
                i = 23;
                snprintf(pItem->date, 3, "%02d", i);

            }

        }
        *inc_flag = 1;
        break;
    }
    case 4:
    {

        ///if(strlen(pItem->date) <  2)
        ////	break;
        if(pData->dwStyle &DE_DATE_TIME)  // 分钟
        {


            if(i >= 60)
            {
                i = 59;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if( i < 0)
            {
                i = 59;
                snprintf(pItem->date, 3, "%02d", i);

            }
        }
        *inc_flag = 1;
        break;
    }
    case 5:  // 秒
    {
        ///if(strlen(pItem->date) <  2)
        ///	break;
        if(pData->dwStyle &DE_DATE_TIME)
        {

            if(i >= 60)
            {
                i = 0;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if( i < 0)
            {
                i = 59;
                snprintf(pItem->date, 3, "%02d", i);

            }

        }
        *inc_flag = 1;
        break;
    }
    default :
    {
        break;
    }
    }
}

static void CheckTimeSegValid(PDATEDATA pData,int *inc_flag)
{
    PDATEITEM pItem;
    int nItem;
    int i;

    nItem = pData->nItemSel;
    pItem = &pData->items[nItem];

    i = atoi(pItem->date);
    printf(">>>>>>>> CheckTimeSegValid current nItem = %d  \n",nItem);
    switch(nItem)
    {
    case 0:
    {

        if( i >= 24)
        {
            i = 0;
            snprintf(pItem->date, 3, "%02d", i);
        }
        else if( i< 0)
        {
            i = 23;
            snprintf(pItem->date, 3, "%02d", i);

        }
        *inc_flag = 1;
        break;
    }
    case 1:
    {


        if(i >= 60)
        {
            i = 59;
            snprintf(pItem->date, 3, "%02d", i);
        }
        else if( i< 0)
        {
            i = 59;
            snprintf(pItem->date, 3, "%02d", i);
        }

        *inc_flag = 1;
        break;
    }
    case 2:
    {
        if( i >= 24)
        {
            i = 0;
            snprintf(pItem->date, 3, "%02d", i);
        }
        else if( i< 0)
        {
            i = 23;
            snprintf(pItem->date, 3, "%02d", i);

        }

        *inc_flag = 1;
        break;
    }
    case 3:
    {



        if(i >= 60)
        {
            i = 0;
            snprintf(pItem->date, 3, "%02d", i);
        }
        else if( i < 0)
        {
            i = 59;
            snprintf(pItem->date, 3, "%02d", i);

        }
        *inc_flag = 1;
        break;
    }

    default :
    {
        break;
    }
    }
}
static void CheckDataValid(PDATEDATA pData,int *inc_flag)
{
    if(pData->dwStyle&DE_TIME_SEG)
    {
        CheckTimeSegValid( pData,inc_flag);
    }
    else
    {
        CheckDateValid( pData,inc_flag);

    }

}
//自己加
static void DateItemSefAdd(PDATEDATA pData)
{
    int i;
    PDATEITEM pItem;
    int nItem;
    int inc_flag = 0;

    nItem = pData->nItemSel;

    pItem = &pData->items[nItem];

    i = atoi(pItem->date);
    i++;
	
    if(( nItem == 0)&&(pData->dwStyle &DE_DATE_MODE  || pData->dwStyle &DE_DATE_TIME ))
        snprintf(pItem->date, 5, "%04d", i);
    else
    {
        snprintf(pItem->date, 3, "%02d", i);
    }

    CheckDataValid(pData,&inc_flag);
    return;
}

//自减
static void DateItemSefDel(PDATEDATA pData)
{
    int i;
    PDATEITEM pItem;
    int nItem;

    int inc_flag = 0;

    nItem = pData->nItemSel;

    pItem = &pData->items[nItem];

    i = atoi(pItem->date);
    i --;
    if(i < 0)
        i = -1;

    if(( nItem == 0)&&(pData->dwStyle &DE_DATE_MODE  || pData->dwStyle &DE_DATE_TIME ))
        snprintf(pItem->date, 5, "%04d", i);
    else
        snprintf(pItem->date, 3, "%02d", i);

    CheckDataValid(pData,&inc_flag);

    return;
}


static int  AddCharToDateItem(PDATEDATA pData, char c)
{
    PDATEITEM pItem;
    int nItem;
    int i, len;//,num;
    int ret = -1;
    int inc_flag = 0;
    //char buf[4] = { 0};

    if(!pData)
        return ret;
    nItem = pData->nItemSel;
    pItem = &pData->items[nItem];



    //buf[0] = c;
    ///buf[3] = '\0';
    ///num = atoi(pItem->date)* 10 + atoi(buf);
    if (pData->items[pData->nItemSel].isSel)
    {
        pData->items[pData->nItemSel].isSel = 0;
        len = 0;
    }
    else
        len = strlen(pItem->date);
	
    if(nItem == 0 && (pData->dwStyle &DE_DATE_MODE  || pData->dwStyle &DE_DATE_TIME )&& len < 4 )
    {

        // snprintf(pItem->date, 5, "%04d", num);
        pItem->date[len++] = c;//执行后加一
        pItem->date[len] = '\0';
    }
    else if( len < 2)
    {
        ///snprintf(pItem->date, 3, "%02d", num);
        pItem->date[len++] = c;//执行后加一
        pItem->date[len] = '\0';

    }

    while(1)
    {
        len = strlen(pItem->date);
        if(len <= 1 || pItem->date[0] != '0')
            break;

        for( i = 0; i < len; i++)
            pItem->date[i] = pItem->date[i+1];

    }

    len = strlen(pItem->date);
    i = atoi(pItem->date);
    inc_flag = 0;
    printf(">>>>>>>> AddCharToDateItem current nItem = %d  \n",nItem);
    switch(nItem)
    {
    case 0:
    {
        if((pData->dwStyle &DE_DATE_MODE  || pData->dwStyle &DE_DATE_TIME )&&(strlen(pItem->date) >= 4))
        {
            if(i < 1970 )
            {
                i = 1970;
                snprintf(pItem->date, 5, "%04d", i);
            }
            if( i > 2999)
            {
                i = 2999;
                snprintf(pItem->date, 5, "%04d", i);
            }
            inc_flag = 1;
        }
        else if((pData->dwStyle &DE_TIME_MODE || pData->dwStyle &DE_TIME_SEG )&&(strlen(pItem->date) >= 2))
        {

            if( i > 23)
            {
                i = 23;
                snprintf(pItem->date, 3, "%02d", i);
            }
            inc_flag = 1;
        }
        else if((pData->dwStyle &DE_TIME_SEG )&&(strlen(pItem->date) >= 2))
        {
            if( i > 24)
            {
                i = 24;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if(i <  0)
            {
                i = 0;
                snprintf(pItem->date, 3, "%02d", i);
            }
        }
        break;
    }
    case 1:
    {
        if(strlen(pItem->date) <  2)
            break;
        if((pData->dwStyle &DE_DATE_MODE  || pData->dwStyle &DE_DATE_TIME ))
        {
            if(i <= 0)
            {
                i = 1;
                snprintf(pItem->date, 3, "%02d", i);
            }
            if(i > 12)
            {
                i = 12;
                snprintf(pItem->date, 3, "%02d", i);
            }

        }
        else if(pData->dwStyle &DE_TIME_MODE)
        {

            if(i >= 60)
            {
                i = 59;
                snprintf(pItem->date, 3, "%02d", i);
            }

        }
        else if(pData->dwStyle &DE_TIME_SEG )
        {
            if(i >= 60)
            {
                i = 59;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if(i < 0)
            {
                i = 0;
                snprintf(pItem->date, 3, "%02d", i);

            }
        }
        inc_flag = 1;
        break;
    }
    case 2:
    {
        if(strlen(pItem->date) <  2)
            break;

        if(pData->dwStyle &DE_DATE_MODE  || pData->dwStyle &DE_DATE_TIME )
        {
            if(i <= 0)
            {
                i = 1;
                snprintf(pItem->date, 3, "%02d", i);
            }
            if(i > __GetNumberOfDays(atoi(pData->items[0].date),atoi(pData->items[1].date)))
            {
                i = __GetNumberOfDays(atoi(pData->items[0].date),atoi(pData->items[1].date));
                snprintf(pItem->date, 3, "%02d", i);
            }

        }
        else if(pData->dwStyle &DE_TIME_MODE)
        {

            if(i >= 60)
            {
                i = 59;
                snprintf(pItem->date, 3, "%02d", i);
            }

        }
        else if(pData->dwStyle &DE_TIME_SEG )
        {
            if( i > 23)
            {
                i = 23;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if(i <  0)
            {
                i = 0;
                snprintf(pItem->date, 3, "%02d", i);


            }
        }
        inc_flag = 1;
        break;
    }
    case 3:
    {

        if(strlen(pItem->date) <  2)
            break;

        if(pData->dwStyle &DE_DATE_TIME)
        {
            if( i > 23)
            {
                i = 23;
                snprintf(pItem->date, 3, "%02d", i);
            }

        }
        else if(pData->dwStyle &DE_TIME_SEG )
        {
            if(i >= 60)
            {
                i = 59;
                snprintf(pItem->date, 3, "%02d", i);
            }
            else if(i < 0)
            {
                i = 0;
                snprintf(pItem->date, 3, "%02d", i);

            }
        }
        inc_flag = 1;
        break;
    }
    case 4:
    {

        if(strlen(pItem->date) <  2)
            break;

        if(pData->dwStyle &DE_DATE_TIME)
        {

            if(i >= 60)
            {
                i = 59;
                snprintf(pItem->date, 3, "%02d", i);
            }

        }
        inc_flag = 1;
        break;
    }
    case 5:
    {
        if(strlen(pItem->date) <  2)
            break;
        if(pData->dwStyle &DE_DATE_TIME)
        {

            if(i >= 60)
            {
                i = 59;
                snprintf(pItem->date, 3, "%02d", i);
            }

        }
        inc_flag = 1;
        break;
    }
    default :
    {
        break;
    }
    }


    if(inc_flag ==1)
    {
        pData->nItemSel++;
        if(pData->nItemSel  >= pData->nItems)
            pData->nItemSel =  pData->nItems -1;
        else
            pData->items[pData->nItemSel].isSel = 1;
    }

    return   0;
}

static int DateItemDelLastChar(PDATEDATA pData)
{
    PDATEITEM pItem;
    int nItem;
    int len;
    int del_flag = 0;

    nItem = pData->nItemSel;
    pItem = &pData->items[nItem];

    len = strlen(pItem->date);
    if(len == 1)
    {
        if(pItem->date[0] != '0')
        {
            pItem->date[0] = '0';
            pItem->date[1] = '\0';
            del_flag = 1;
        }
    }

    else if(len > 1)
    {
        pItem->date[len - 1] = '\0';
        del_flag = 1;
    }

    return del_flag;
}

/**************************************************************************/
/* sets the current caret position in the virtual content window */
static void myDateSetCaretPos (HWND hWnd, PDATEDATA pData, int x,int y)
{
#if 1
    /*
        if (x >= 0)
           sled->caret_x = x;
        else
           x = sled->caret_x;
    */
//    if (sleContentToVisible(sled, &x) < 0)
    if(0)//这里需要判断插入符是不是已经超出范围
    {
        HideCaret (hWnd);
    }
    else
    {
        // sleVisibleToWindow (sled, &x);
        printf("myIPSetCaretPos  int x = %d ,int y = %d  \n",x,y);
        SetCaretPos (hWnd, x,y);
        if (pData->status & DATE_FOCUSED)
        {
            ActiveCaret (hWnd);
            ShowCaret (hWnd);
        }
    }
#endif
}
/*
 * set caret position according to the new desired x coordinate.
 */
static void DateSetCaretPos (HWND hWnd, PDATEDATA pData, int x, BOOL bIP)
{
#if 1
    //int out_chars;
    int  caret_x = 0,caret_y = 0;
    int  margin_x = 0;//,margin_y = 0;

    //SIZE txtsize;
    PCONTROL pCtrl;
    pCtrl = Control(hWnd);

    PDATEITEM pItem;
    int nItem;
    //int i, 
	int text_len = 0;

    if(!pData)
        return;

    nItem = pData->nItemSel;
    pItem = &pData->items[nItem];
    text_len = strlen(pItem->date);
    margin_x = (pItem->width - text_len*pCtrl->pLogFont->size)>>1;
    if(margin_x  < 0)
        margin_x  = 0;
    caret_x =  pItem->x + margin_x+text_len*pCtrl->pLogFont->size;
    if (caret_x > pItem->x +pItem->width  )
        caret_x =  pItem->x +pItem->width -1;
    caret_y = pItem->y  +(pItem->height >> 1) - 8;
    ////////printf("IPSetCaretPos  text_len = %d,pItem->width = %d,margin_x = %d,caret_x = %d,caret_y =  %d pCtrl->pLogFont->size = %d \n",text_len,pItem->width,margin_x,caret_x,caret_y,pCtrl->pLogFont->size);
    if (!bIP)
    {
        myDateSetCaretPos (hWnd, pData, caret_x,caret_y);
    }
    else
    {

    }

#endif
}


/**************  IpAddress_WinProc  *************/
static int Date_WinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{

    PDATEDATA pData = NULL;
    DWORD       dwStyle;

    dwStyle  = GetWindowStyle(hWnd);//获取风格

    if (message != MSG_CREATE)
        pData = (PDATEDATA) GetWindowAdditionalData2 (hWnd);

    switch (message)
    {
    case MSG_CREATE://创建控件
    {		
		SetWindowFont(hWnd, GetSystemFont(SYSLOGFONT_CONTROL));
        if ( !(pData = (PDATEDATA)calloc(1, sizeof(DATEDATA))) )
            return -1;
        if(dwStyle&DE_TIME_SEG)
        {
            InitTimeSegData(hWnd, pData, dwStyle);
        }
        else
            InitDateData(hWnd, pData, dwStyle);

        SetWindowAdditionalData2 (hWnd, (DWORD) pData);//作为私有数据
        //printf("Date_WinProc 111111111 \n");
        break;
    }

    case MSG_PAINT:
    {
        HDC hdc;

        hdc = BeginPaint (hWnd);
        DateOnDraw (hWnd, hdc);
        EndPaint (hWnd, hdc);
//	ShowCaret( hWnd);
        return 0;
    }
    break;

	case MSG_ENABLE:
	{
		PCONTROL	  pCtrl   = Control (hWnd);
        if (wParam && (pCtrl->dwStyle & WS_DISABLED))
            pCtrl->dwStyle &= ~WS_DISABLED;
        else if (!wParam && !(pCtrl->dwStyle & WS_DISABLED))
            pCtrl->dwStyle |= WS_DISABLED;
        else
            return 0;
        InvalidateRect (hWnd, NULL, TRUE);

        return 0;
	}

    case MSG_ERASEBKGND:
        return 0;

    case MSG_SETFOCUS:
        if(pData->status & DATE_FOCUSED)
            break;

        pData->status |= DATE_FOCUSED;

        pData->items[0].isSel = 1;
        pData->items[1].isSel = 1;
        pData->items[2].isSel = 1;
        pData->items[3].isSel = 1;
        pData->items[4].isSel = 1;
        pData->items[5].isSel = 1;

        ShowCaret(hWnd);//显示插入符

        InvalidateRect (hWnd, NULL, TRUE);

        if (pData->dwStyle & DATE_NOTIFY)
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), DATE_FOCUSED);
        break;

    case MSG_KILLFOCUS:
        HideCaret (hWnd);//隐藏插入符
        if(pData->status & DATE_FOCUSED)
        {
            pData->status &= (~DATE_FOCUSED);
            InvalidateRect (hWnd, NULL, TRUE);

            if (pData->dwStyle & DATE_NOTIFY)
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), DATE_KILLFOCUS);
        }

        break;

    case MSG_GETDLGCODE:
        return (DLGC_WANTCHARS | DLGC_WANTARROWS);
	case MSG_DOESNEEDIME:
        if (dwStyle & ES_READONLY)
            return FALSE;
        return TRUE;
		
	case MSG_LBUTTONDBLCLK:
    case MSG_LBUTTONDOWN://需要处理插入符
    {
        int x_pos = LOSWORD (lParam);
        int y_pos = HISWORD (lParam);
        int nItem;

        nItem = get_nItem_withxy(pData, x_pos, y_pos);//第几个编辑框
        /////////////////printf("msg MSG_LBUTTONDOWN x = %d, y = %d, nItem = %d\n", x_pos, y_pos, nItem);
        if(DATE_BE_VALID_ITEM(nItem))
        {
            //////////////////// printf("IPADDRESS_BE_VALID_ITEM  !!!!!!!!!!!\n");
            //检查前面数据
            int inc_flag;
            CheckDataValid(pData,&inc_flag);
            pData->nItemSel = nItem;
            pData->items[pData->nItemSel].isSel = 1; //移动了光标位置，需要将对应的状态为设为1

            InvalidateRect (hWnd, NULL, TRUE);

            DateSetCaretPos (hWnd, pData, LOSWORD(lParam), FALSE);
            ActiveCaret (hWnd);
            ShowCaret(hWnd);
			SetCapture(hWnd);
		#ifdef IMA_CLICK_SHOW
			if (!(dwStyle&ES_READONLY))
			{
				SendNotifyMessage (__mg_ime_wnd, MSG_IME_LBD_SWITCH, hWnd, 0);
				NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_CLICKED);
			}
		#endif
        }

    }
    break;
#if 0	
#ifndef IMA_CLICK_SHOW	
	case MSG_LBUTTONDBLCLK:
    {
        SendNotifyMessage (__mg_ime_wnd, MSG_IME_LBD_SWITCH, hWnd, 0);
        NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_DBLCLK);
        break;
    }
#endif
#endif
	case MSG_NCLBUTTONUP:
	case MSG_LBUTTONUP:
		   if (GetCapture() == hWnd)
			   ReleaseCapture();
		   NotifyParent (hWnd,	GetDlgCtrlID(hWnd), EN_CLICKED);
		 break;

    case MSG_MOUSEMOVEIN:
    {

        BOOL in_out = (BOOL)wParam;
        if(in_out)
        {
            SetFocusChild (hWnd);
        }

        else
        {
			if(!IsCustomImeOpen())
				SetNullFocus (GetParent(hWnd));
        }

        break;
    }
    case MSG_MOUSEMOVE:    //加入输入法，处理焦点方便，鼠标移动注掉 20130509 chenwz modify
    {
        int x_pos, y_pos;
        int nItem;

        x_pos = LOSWORD(lParam);
        y_pos = HISWORD(lParam);
        nItem = get_nItem_withxy(pData, x_pos, y_pos);//第几个编辑框
        /////////////////printf("msg MSG_MOUSEMOVE  x = %d, y = %d, nItem = %d\n", x_pos, y_pos, nItem);

        if(DATE_BE_VALID_ITEM(nItem))
        {
            int inc_flag;
            ////////////////////// printf("DATE_BE_VALID_ITEM  !!!!!!!!!!!\n");
            if(pData->nItemSel != nItem)
            {
                CheckDataValid(pData,&inc_flag);
                pData->nItemSel = nItem;
                pData->items[pData->nItemSel].isSel = 1;
                
                InvalidateRect (hWnd, NULL, TRUE);

                DateSetCaretPos (hWnd, pData, LOSWORD(lParam), FALSE);
                ActiveCaret (hWnd);
                ShowCaret(hWnd);
            }

        }
        break;
    }
	
    case MSG_KEYDOWN:
    {
        BOOL bChange = FALSE;
        int id = LOWORD (wParam);
        int nItem = -1;
        BOOL need_refresh = 1;
        //printf("MSG_KEYDOWN <><><><><><><><><><><><><><><><><><><><><><>  id =%d \n",id);
        switch (id)
        {
        case SCANCODE_HOME:
            nItem = 0;
            break;
        case SCANCODE_END:
            nItem = pData->nItems - 1;
            break;
        case SCANCODE_ENTER:
        {
            nItem = pData->nItemSel + 1;
            if(!(DATE_BE_VALID_ITEM(nItem)))
                nItem = 0;
        }
        break;

        case SCANCODE_BACKSPACE:
            if(DateItemDelLastChar(pData))
                bChange = TRUE;
            break;

        case SCANCODE_SPACE:
            nItem = pData->nItemSel;
            if(strcmp(pData->items[nItem].date, "0"))
            {
                snprintf(pData->items[nItem].date, 4, "0");
                bChange = TRUE;
            }
            break;

        case SCANCODE_CURSORBLOCKLEFT:
            nItem = pData->nItemSel - 1;
            break;
        case SCANCODE_CURSORBLOCKRIGHT:
            nItem = pData->nItemSel + 1;
            break;

        case SCANCODE_CURSORBLOCKUP:
            DateItemSefAdd(pData);
            bChange = TRUE;
            break;
        case SCANCODE_CURSORBLOCKDOWN:
            DateItemSefDel(pData);
            bChange = TRUE;
            break;

        default :
            need_refresh = 0;
            break;
        }

        //printf("need_refresh = %d, nItem = %d \n", need_refresh, nItem);

        if(need_refresh)
        {
            if(DATE_BE_VALID_ITEM(nItem))
            {
                pData->nItemSel = nItem;
                pData->items[pData->nItemSel].isSel = 1;
            }
            InvalidateRect (hWnd, NULL, TRUE);

            //add by jason 20110727
            //设置插入符的位置
            DateSetCaretPos (hWnd, pData, LOSWORD(lParam), FALSE);
            ActiveCaret (hWnd);
            ShowCaret(hWnd);

            if(bChange)
                NotifyParent (hWnd, GetDlgCtrlID(hWnd), DATE_CHANGE);
        }
    }
    break;

    case MSG_CHAR:
    {
        char c = LOBYTE(wParam);
        printf(">>>>>>>>>>>> IpAddress_WinProc  current c :%c \n",c);
        if(c >='0' &&  c <= '9' )
        {
            if(0 ==  AddCharToDateItem(pData, c))
            {

                NotifyParent (hWnd, GetDlgCtrlID(hWnd), MSG_DATE_ERROR);//提示超出范围

            }
            InvalidateRect (hWnd, NULL, TRUE);

            NotifyParent (hWnd, GetDlgCtrlID(hWnd), DATE_CHANGE);
        }

        else if(c == '.')
        {
            int nItem = pData->nItemSel + 1;

            int inc_flag;
            CheckDataValid(pData,&inc_flag);

			
            if(!(DATE_BE_VALID_ITEM(nItem)))
                nItem = 0;

            if(DATE_BE_VALID_ITEM(nItem))
            {
                pData->nItemSel = nItem;
                pData->items[pData->nItemSel].isSel = 1;
                InvalidateRect (hWnd, NULL, TRUE);
            }

        }

        if (wParam == 127) // BS
        {
            pData->items[pData->nItemSel].isSel = 0;
        }
        // 插入符的位置
        DateSetCaretPos (hWnd, pData, LOSWORD(lParam), FALSE);
        ActiveCaret (hWnd);
        ShowCaret(hWnd);
        InvalidateRect (hWnd, NULL, TRUE);
    }

    break;

    case MSG_DESTROY:
    {
        DateDataDestory (pData);
        break;
    }

    case MSG_GETTEXTLENGTH:
    {
#if 0
        char datebuf[DATE_LENGTH_MAX] = "";

        Items2Date(datebuf, DATE_LENGTH_MAX, pData->items, pData->nItems);

        return strlen(datebuf);
#endif
        return 0;
    }
    case MSG_SETTEXT:
    {
#if 0
        int len;
        char datebuf[DATE_LENGTH_MAX];

        if (lParam == 0)
            return -1;

        len = strlen ((char*)lParam);
        len = MIN (len, DATE_LENGTH_MAX - 1);


        memcpy (datebuf, (char*)lParam, len);

        datebuf[DATE_LENGTH_MAX - 1] = '\0';

        SetDateItems(pData->items, pData->nItems, 0, datebuf);
        InvalidateRect (hWnd, NULL, TRUE);
#endif
        return 0;
    }

    case MSG_SET_DATE:
    {
        //int flag = (int)wParam;

        printf("MSG_SET_DATE    MSG_SET_DATE ############################# \n");
        SetDateItems(pData, (ctrl_time_t *)lParam);

        InvalidateRect (hWnd, NULL, TRUE);
        break;
    }


    case MSG_GET_DATE:
    {
        //int flag = (int)wParam;
		ctrl_time_t temp;
        GetDateItems(pData, (ctrl_time_t *)lParam);
		memcpy(&temp,(ctrl_time_t *)lParam,sizeof(ctrl_time_t));
		InvalidateRect (hWnd, NULL, TRUE);
        break;
    }
    case MSG_SET_TIMESEG:
    {
        if(!(pData->dwStyle&DE_TIME_SEG))
            break;

        SetTimeSeg(pData, (time_segment *)lParam);
        InvalidateRect (hWnd, NULL, TRUE);
        break;
    }
    case MSG_GET_TIMESEG:
    {
        if(!(pData->dwStyle&DE_TIME_SEG))
            break;
        return GetTimeSeg(pData, (time_segment *)lParam);
        ////InvalidateRect (hWnd, NULL, TRUE);
        ////break;
    }

    case MSG_GETTEXT:
    {
#if 0
        char *buffer = (char*)lParam;
        int  len;

        in_addr_t tmp_ip;
        char datebuf[DATE_LENGTH_MAX];

        len = MIN ((int)wParam, DATE_LENGTH_MAX - 1);

        tmp_ip = Items2Date(datebuf, DATE_LENGTH_MAX, pData->items, pData->nItems);

        memcpy(buffer, datebuf, len);
        buffer[len] = '\0';

        return len;
#endif
        return 0;
    }


    default :
        break;
    }

    return DefaultControlProc (hWnd, message, wParam, lParam);
}

BOOL RegisterDateControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_DATE;
    WndClass.dwStyle = WS_NONE;
    WndClass.dwExStyle = WS_EX_NONE;
    WndClass.hCursor = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor = PIXEL_lightwhite;
    WndClass.WinProc = Date_WinProc;
    return RegisterWindowClass (&WndClass);
}

void DateControlCleanup (void)
{
    UnregisterWindowClass (CTRL_DATE);
}

#endif /* _UNDEFINE_CTRL_IPADDRESS_ */

