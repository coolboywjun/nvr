#ifndef _CTRL_SKBKEY_H_
#define _CTRL_SKBKEY_H_

#ifdef __cplusplus
extern "C" {
#endif 

#define CTRL_SKBK_CUSTOM         ("SKBkey")		//softkeybord key

#define SKBKS_CHECKED		   0x00000001L

#define SKBKS_TYPEMASK         0x00000001L

#define SKBKS_NOTIFY           0x00000080L

#define SKBKS_CENTERIMAGE      0x00000200L

#define SKBKS_REALSIZEIMAGE    0x00000800L

 
#define SKBKM_SETICON         0xF170

#define SKBKM_GETICON         0xF171

#define SKBKM_SETIMAGE        0xF172

#define SKBKM_GETIMAGE        0xF173

#define SKBKM_SETCHECKED		  0xF174

#define SKBKM_PUSHDOWN		  0xF175
#define SKBKM_PUSHUP		  0xF176

#define SKBKM_MSGMAX          0xF180



#define SKBKN_DBLCLK          1

#define SKBKN_ENABLE          2

#define SKBKN_DISABLE         3

#define SKBKN_CLICKED         4

#define SKBKN_PUSHED		  5

#define SKBKN_UNPUSHED		  6


BOOL RegisterSKBKeyControl (void);
void SKBKeyControlCleanup (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif
