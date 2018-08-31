

#ifndef _CTRL_IPADDRESS_H
#define _CTRL_IPADDRESS_H
#ifdef __cplusplus
extern "C"
{
#endif

#define CTRL_IP_ADDRESS   "CtrlIpAddress"



#define IPS_CENTER    (BS_CENTER)
#define IPS_VCENTER   (BS_VCENTER)
#define IPS_NOTIFY    (BS_NOTIFY)

#define IP_SETFOCUS   (EN_SETFOCUS)
#define IP_KILLFOCUS  (EN_KILLFOCUS)
#define IP_CHANGE     (EN_CHANGE)
#define IPST_FOCUSED  (0x00000001L)

#define IPM_IP_ERROR  (MSG_USER + 11)
#define IPM_SET_IPADDRESS  (MSG_USER + 12)
#define IPM_GET_IPADDRESS  (MSG_USER + 13) 

#define IP_LENGTH_MAX         16



    BOOL RegisterIpAddressControl (void);
    void IpAddressControlCleanup (void);
	
#if 0
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

