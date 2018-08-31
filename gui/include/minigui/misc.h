/*
** $Id: misc.h 7337 2007-08-16 03:44:29Z xgwang $
**
** misc.h: the head file for Miscellous module.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/01/03
*/

#ifndef GUI_MISC_H
    #define GUI_MISC_H

/* Function definitions */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#ifndef HAVE_SYS_TIME_H

struct timeval {
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* microseconds */
};

struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

#else
  #include <sys/time.h>
#endif

unsigned int __mg_os_get_random_seed (void);
void __mg_os_time_delay (int ms);

#define NR_KEYS_INIT_ALLOC      8
#define NR_KEYS_INC_ALLOC       4

#define NR_SECTS_INIT_ALLOC     16
#define NR_SECTS_INC_ALLOC      8

extern GHANDLE hMgEtc;

BOOL InitMgEtc (void);
void TerminateMgEtc (void);

BOOL InitMisc (void);
void TerminateMisc (void);

BOOL InitSystemRes (void);
void TerminateSystemRes (void);

int __mg_lookfor_unused_slot (unsigned char* bitmap, int len_bmp, int set);
void __mg_slot_set_use (unsigned char* bitmap, int index);
int __mg_slot_clear_use (unsigned char* bitmap, int index);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_MISC_H */

