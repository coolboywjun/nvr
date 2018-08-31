#ifndef _CTRL_DATE_H
#define _CTRL_DATE_H
#ifdef __cplusplus
extern "C"
{
#endif

#define CTRL_DATE   "CtrlDate"

#define DE_DATE_MODE   0x00000001L  // 日期  2012-12-31
#define DE_TIME_MODE    0x00000002L  //时间 23:59:59
#define DE_DATE_TIME	0x00000004L     //显示日期和时间
#define DE_TIME_SEG		0x00000008L   //时间段 12:00--13-00

#define DATE_CENTER    (BS_CENTER)
#define DATE_VCENTER   (BS_VCENTER)
#define DATE_NOTIFY    (BS_NOTIFY)

#define DATE_SETFOCUS   (EN_SETFOCUS)
#define DATE_KILLFOCUS  (EN_KILLFOCUS)
#define DATE_CHANGE     (EN_CHANGE)

#define DATE_FOCUSED  (0x00000001L)

#define MSG_DATE_ERROR  (MSG_USER + 14)
#define MSG_SET_DATE  (MSG_USER + 15)
#define MSG_GET_DATE  (MSG_USER + 16) 

#define MSG_SET_TIMESEG  (MSG_USER + 17)
#define MSG_GET_TIMESEG    (MSG_USER + 18) 

#define DATE_LENGTH_MAX         16

/*
 * 时间结构体定义
 */
typedef struct ctrl_time_s{
    unsigned int  year;
    unsigned int mon;
    unsigned int day;
    unsigned int hour;
    unsigned int min;
    unsigned int sec;
}ctrl_time_t;


/*
 * 时间段结构体定义
 */
typedef struct ctrl_time_seg{
	unsigned char enable;	    //激活
	unsigned char res[3];	   
     unsigned char start_hour; //开始时间
	unsigned char start_min;
	unsigned char stop_hour;  //结束时间
	unsigned char stop_min;
}time_segment;

    BOOL RegisterDateControl (void);
    void DateControlCleanup (void);
	
#if 0


      SendMessage(GetDlgItem(hDlg, IDC), MSG_SET_DATE, 0, (LPARAM)(*ctrl_time_t) );
      SendMessage(GetDlgItem(hDlg, IDC), MSG_GET_DATE, 0, (LPARAM)(*ctrl_time_t) );

      SendMessage(GetDlgItem(hDlg, IDC), MSG_SET_TIMESEG, 0, (LPARAM)(*time_segment) );
      SendMessage(GetDlgItem(hDlg, IDC), MSG_GET_TIMESEG, 0, (LPARAM)(*time_segment) );
    //Notice: Usage
    /*
      {
      CTRL_IP_ADDRESS,
      WS_TABSTOP | WS_VISIBLE | IPS_CENTER | IPS_VCENTER | IPS_NOTIFY,
      10, 273, 250, 30,
      IDC_MY_IP, "",
      38, WS_EX_NONE
      }

      char ip[IP_LENGTH_MAX];
      in_addr_t in_addr;
      
      SetWindowText(GetDlgItem(hDlg, IDC_MY_IP), ip);
      GetWindowText(GetDlgItem(hDlg, IDC_MY_IP), ip, sizeof(ip) - 1);

      or

      SendMessage(GetDlgItem(hDlg, IDC_IP), IPM_SET_IPADDRESS, 0, (LPARAM)"192.0.10.135");
      SendMessage(GetDlgItem(hDlg, IDC_IP), IPM_SET_IPADDRESS, 1, (LPARAM)in_addr);
      
      SendMessage(GetDlgItem(hDlg, IDC_IP), IPM_GET_IPADDRESS, sizeof(ip) - 1, (LPARAM)ip);
      SendMessage(GetDlgItem(hDlg, IDC_IP), IPM_GET_IPADDRESS, 0, (LPARAM)&in_addr);
    */
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _CTL_IPADDRESS_H */
