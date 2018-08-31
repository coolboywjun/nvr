#ifndef HZ_INPUT_SOFTKEY_DEFINE
#define HZ_INPUT_SOFTKEY_DEFINE

//T9 键盘按键: 目前只实现数字键盘
typedef struct _T9SOFTKEYS
{
	HWND hT9Sk1;
	HWND hT9Sk2;
	HWND hT9Sk3;
	HWND hT9Sk4;
	HWND hT9Sk5;
	HWND hT9Sk6;
	HWND hT9Sk7;
	HWND hT9Sk8;
	HWND hT9Sk9;
	HWND hT9Sk0;
	HWND hT9SkClear;
	HWND hT9SkOk;
}
T_T9SOFTKEYS;

#if 1
//QWERTY 键盘按键 : 56个按键
typedef struct _TQWERTYKEYS
{
	HWND hNum1; 
	HWND hNum2; 
	HWND hNum3; 
	HWND hNum4; 
	HWND hNum5; 
	HWND hNum6; 
	HWND hNum7; 
	HWND hNum8; 
	HWND hNum9; 
	HWND hNum0;
	HWND hKeya; 
	HWND hKeyb; 
	HWND hKeyc; 
	HWND hKeyd; 
	HWND hKeye; 
	HWND hKeyf; 
	HWND hKeyg;
	HWND hKeyh; 
	HWND hKeyi; 
	HWND hKeyj;
	HWND hKeyk; 
	HWND hKeyl; 
	HWND hKeym; 
	HWND hKeyn; 
	HWND hKeyo; 
	HWND hKeyp; 
	HWND hKeyq; 
	HWND hKeyr; 
	HWND hKeys; 
	HWND hKeyt;
	HWND hKeyu; 
	HWND hKeyv; 
	HWND hKeyw; 
	HWND hKeyx; 
	HWND hKeyy; 
	HWND hKeyz; 
	
	HWND hGantanhao; 
	HWND hWenhao; 
	HWND hAt;
	HWND hJinhao; 
	HWND hDollar; 
	HWND hPercent; 
	HWND hEqual; 
	HWND hAdd; 
	HWND hChen; 
	HWND hSub; 
	HWND hPlinedown;
	HWND hBackspace; 
	HWND hChu; 
	HWND hMaohao; 
	HWND hEnter; 
	HWND hDouhao; 
	HWND hDot; 
	HWND hShift; 
	HWND hSpace;
	HWND hAddr;
}
T_QUERTYKEYS;
#else
//QWERTY 键盘按键 : 61个按键
typedef struct _TQWERTYKEYS
{
	HWND hNum1; 
	HWND hNum2; 
	HWND hNum3; 
	HWND hNum4; 
	HWND hNum5; 
	HWND hNum6; 
	HWND hNum7; 
	HWND hNum8; 
	HWND hNum9; 
	HWND hNum0;
	HWND hKeya; 
	HWND hKeyb; 
	HWND hKeyc; 
	HWND hKeyd; 
	HWND hKeye; 
	HWND hKeyf; 
	HWND hKeyg;
	HWND hKeyh; 
	HWND hKeyi; 
	HWND hKeyj;
	HWND hKeyk; 
	HWND hKeyl; 
	HWND hKeym; 
	HWND hKeyn; 
	HWND hKeyo; 
	HWND hKeyp; 
	HWND hKeyq; 
	HWND hKeyr; 
	HWND hKeys; 
	HWND hKeyt;
	HWND hKeyu; 
	HWND hKeyv; 
	HWND hKeyw; 
	HWND hKeyx; 
	HWND hKeyy; 
	HWND hKeyz; 
	HWND hBackSpace; 
	HWND hEnter; 
	HWND hDel;
	HWND hSpace; 
	HWND hShift; 
	HWND hEqual; 
	HWND hJian; 
	HWND hKuohao1; 
	HWND hKuohao2; 
	HWND hPline; 
	HWND hFenhao;
	HWND hYinhao; 
	HWND hDuhao; 
	HWND hDot; 
	HWND hChu; 
	HWND hEsc; 
	HWND hTab; 
	HWND hCaps; 
	HWND hCtrl; 
	HWND hPageDown; 
	HWND hPageUp;
	HWND hUp; 
	HWND hDown; 
	HWND hLeft; 
	HWND hRight;
}
T_QUERTYKEYS;
#endif
#endif
