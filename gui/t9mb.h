#ifndef	__T9MB_h__ 
#define	__T9MB_h__ 


/*英文输入查询码表*/
typedef struct
{
	const char* key;
	const char* Letter;

}T9EN_IDX;


/*拼音输入法查询码表*/
typedef struct 
{ 
	const char *T9; 
	const char *PY; 
	const unsigned char *MB; 
}T9PY_IDX; 

extern const T9EN_IDX t9EN_ex_index[];
extern const T9PY_IDX t9PY_ex_index[];
#endif 

