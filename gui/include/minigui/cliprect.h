/*
** $Id: cliprect.h 7337 2007-08-16 03:44:29Z xgwang $
**
** cliprect.h: the head file of Clip Rect module.
** 
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/3/26
*/

#ifndef GUI_CLIPRECT_H
    #define GUI_CLIPRECT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct tagGCRINFO
{
#ifndef _LITE_VERSION
    pthread_mutex_t lock;
#endif
    CLIPRGN         crgn;
    unsigned int    age;
#if defined (_LITE_VERSION) && !(_STAND_ALONE)
    unsigned int    old_zi_age;
#endif
} GCRINFO;
typedef GCRINFO* PGCRINFO;

typedef struct tagINVRGN
{
#ifndef _LITE_VERSION
    pthread_mutex_t lock;
#endif
    CLIPRGN         rgn;
    int             frozen;
} INVRGN;
typedef INVRGN* PINVRGN;

/* Function definitions */

#undef _REGION_DEBUG

#ifdef _REGION_DEBUG
void dumpRegion (CLIPRGN* region);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_CLIPRECT_H */


