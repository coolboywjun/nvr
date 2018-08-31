#ifndef	__T9_h__ 
#define	__T9_h__

//HWND T9IMEWindow(HWND hosting);
//HWND T9IMEWindow(HWND hosting, int lx, int ty, int rx, int by, int imemode);

/*数字键定义*/
#define myKEY_0	48
#define myKEY_1	49
#define myKEY_2	50
#define myKEY_3	51
#define myKEY_4	52
#define myKEY_5	53
#define myKEY_6	54
#define myKEY_7	55
#define myKEY_8	56
#define myKEY_9	57

/*常量定义*/
#define MAX_PY_SAME	4	//一组按键时出现的最多拼音个数
#define MAX_PY_LENGTH	6	//拼音长度最大为6个字母
#define MAX_KEY_CHAR	8	//每个按键最多对应8个字母
#define MAX_HZ_COUNT	110	//一个拼音对应的汉字数量最多为110个
#define MAX_ROW_COUNTS	5	//每页显示汉字个数
#define MAX_PAGES_COUNTS (MAX_HZ_COUNT%MAX_ROW_COUNTS==0)?(MAX_HZ_COUNT/MAX_ROW_COUNTS):((MAX_HZ_COUNT/MAX_ROW_COUNTS)+1)	//最大页数

#endif

