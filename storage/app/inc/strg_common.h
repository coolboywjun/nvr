#ifndef __STRG_COMMON_H__
#define __STRG_COMMON_H__


#include <strg_header.h>

//#define READ_STRUCT(fd, ptr)		(read(fd, ptr, sizeof(*ptr))
//#define WRITE_STRUCT(fd, ptr)	(write(fd, ptr, sizeof(*ptr))

//#define MALLOC_STRUCT(struc_type)	(g_malloc0(sizeof(struct_type))
//#define FREE_STRUCT(ptr)	(g_free(ptr))


//#define ZERO_STRUCT(ptr)	(memset(ptr, 0, sizeof(*ptr))

glong calc_checksum(gpointer buff, gint len);

void display_time(time_t t, const gchar *prefix);

int strg_write(int fd, const void *data, size_t len, off_t offset);

int strg_read(int fd, void *data, size_t len, off_t offset);



#endif

