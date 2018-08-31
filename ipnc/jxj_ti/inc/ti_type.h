#ifndef __TI_TYPE_H__
#define __TI_TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/

typedef unsigned char           TI_U8;
typedef unsigned char           TI_UCHAR;
typedef unsigned short          TI_U16;
typedef unsigned int            TI_U32;

typedef signed char             TI_S8;
typedef short                   TI_S16;
typedef int                     TI_S32;

#ifndef _M_IX86
typedef unsigned long long      TI_U64;
typedef long long               TI_S64;
#else
typedef __int64                   TI_U64;
typedef __int64                   TI_S64;
#endif

typedef char                    TI_CHAR;
typedef char*                   TI_PCHAR;

typedef float                   TI_FLOAT;
typedef double                  TI_DOUBLE;
typedef void                    TI_VOID;

typedef unsigned long           TI_SIZE_T;
typedef unsigned long           TI_LENGTH_T;


/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
typedef enum {
    TI_FALSE    = 0,
    TI_TRUE     = 1,
} TI_BOOL;

#ifndef NULL
#define NULL             0L
#endif
#define TI_NULL          0L
#define TI_NULL_PTR      0L

#define TI_SUCCESS          0
#define TI_FAILURE          (-1)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TI_TYPE_H__ */

