/* gbase64.h - Base64 coding functions
 *
 *  Copyright (C) 2005  Alexander Larsson <alexl@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __G_BASE64_H__
#define __G_BASE64_H__

#include <limits.h>


typedef char   gchar;
typedef short  gshort;
typedef long   glong;
typedef int    gint;
typedef gint   gboolean;

typedef unsigned char   guchar;
typedef unsigned short  gushort;
typedef unsigned long   gulong;
typedef unsigned int    guint;

typedef float   gfloat;
typedef double  gdouble;

typedef signed int gssize;
typedef unsigned int gsize;

#define G_MININT	INT_MIN
#define G_MAXINT	INT_MAX
#define G_MAXUINT	UINT_MAX


#define G_MAXSIZE	G_MAXUINT
#define G_MINSSIZE	G_MININT
#define G_MAXSSIZE	G_MAXINT

#ifndef	FALSE
#define	FALSE (0)
#endif

#ifndef	TRUE
#define	TRUE (!FALSE)
#endif


gsize   g_base64_encode_step    (const guchar *in,
                                 gsize         len,
                                 gboolean      break_lines,
                                 gchar        *out,
                                 gint         *state,
                                 gint         *save);

gsize   g_base64_encode_close   (gboolean      break_lines,
                                 gchar        *out,
                                 gint         *state,
                                 gint         *save);

gchar*  g_base64_encode         (const guchar *data,
                                 gsize         len);

gsize   g_base64_decode_step    (const gchar  *in,
                                 gsize         len,
                                 guchar       *out,
                                 gint         *state,
                                 guint        *save);

guchar *g_base64_decode         (const gchar  *text,
                                 gsize        *out_len);

guchar *g_base64_decode_inplace (gchar        *text,
                                 gsize        *out_len);



#endif /* __G_BASE64_H__ */


