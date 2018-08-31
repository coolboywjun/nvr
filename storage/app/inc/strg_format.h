#ifndef __STRG_FORMAT_H__
#define __STRG_FORMAT_H__

#include <strg_disk.h>

int strg_format_create_flag_file(const gchar *mount_path);
int strg_format_detect_flag_file(const gchar *mount_path);
int strg_format_get_percent(int *percent);
int strg_format_disk_start(const gchar *dev_path, int disk_idx, int check_bad);
DiskMng* strg_format_disk_stop();
int strg_format_is_formatting();
int strg_format_get_format_disk_idx();

#endif

