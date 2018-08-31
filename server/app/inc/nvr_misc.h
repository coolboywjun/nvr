#ifndef __NVR_MISC_H__
#define __NVR_MISC_H__


int	nvr_set_system_reboot_flag();

int	nvr_get_system_reboot_flag();

int	nvr_set_system_shutdown_flag();

int nvr_get_system_shutdown_flag();
// nReset: 
// 1, »Ì∏¥Œª
// 2, ”≤∏¥Œª
int	nvr_set_system_default_flag(int nReset);

int	nvr_get_system_default_flag();

int	nvr_set_hwgpio_check_flag(int open);

int	nvr_get_hwgpio_check_flag();

int	nvr_do_system_reset(int nSoftReset);

int 	nvr_do_system_reboot();

int 	nvr_do_system_shutdown();



#endif

