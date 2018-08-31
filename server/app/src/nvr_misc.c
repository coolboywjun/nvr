
#include <nvr_server_headers.h>

#include "nvr_misc.h"
#include "nvr_param_proc.h"
#include "nvr_param_sys.h"
#include "nvr_channel.h"

static gint	sysReboot = 0;
static gint	sysShutdown = 0;
static gint 	sysSetDefault = 0;
static gint 	sysHwGpioCheck = 1;

int	nvr_set_system_reboot_flag()
{
	j_message( "nvr set reboot flag.");
	g_atomic_int_set(&sysReboot, 1);
	return 0;
}

int	nvr_get_system_reboot_flag()
{
	return g_atomic_int_get(&sysReboot);
}

int	nvr_set_system_shutdown_flag()
{
	j_message( "nvr set reboot flag.");
	g_atomic_int_set(&sysShutdown, 1);
	return 0;
}

int	nvr_get_system_shutdown_flag()
{
	return g_atomic_int_get(&sysShutdown);
}

// nReset: 
// 1, 软复位
// 2, 硬复位
int	nvr_set_system_default_flag(int nReset)
{
	j_message( "nvr set reboot flag.");
	g_atomic_int_set(&sysSetDefault, nReset);
	return 0;
}

int	nvr_get_system_default_flag()
{
	return g_atomic_int_get(&sysSetDefault);
}

int	nvr_set_hwgpio_check_flag(int open)
{
	g_atomic_int_set(&sysHwGpioCheck, open);
	return 0;
}

int	nvr_get_hwgpio_check_flag()
{
	return g_atomic_int_get(&sysHwGpioCheck);
}

int	nvr_do_system_reset(int nSoftReset)
{
	if (nSoftReset == 1)
	{
		j_message( "SYSTEM SOFT RESET!!!!!!!");
		log_write(LOG_INFO, SYSTEM_LOG, "系统软复位");
		nvr_param_sw_reset();
	}
	else
	{
		j_message( "SYSTEM HW RESET!!!!!!!");
		log_write(LOG_INFO, SYSTEM_LOG, "系统硬复位");
		nvr_param_hw_reset();		
	}
	j_message( "SYSTEM Ready Reboot !");
	log_write(LOG_INFO, SYSTEM_LOG, "系统重启");
	//sleep(3); //wait 3 second 
	nvr_channel_default_all_ipc();
	system("hwclock -w");
	system("reboot &");
	
	return 0;
}

int 	nvr_do_system_reboot()
{
	j_message( "SYSTEM REBOOT!");
	if (nvr_param_get_sys_change_flag())
	{
		nvr_param_write_sys_param_info();
	}
	j_message( "SYSTEM Ready Reboot !");
	log_write(LOG_INFO, SYSTEM_LOG, "系统重启");
	//sleep(3); //wait a moment
	nvr_channel_reboot_all_ipc();
	system("hwclock -w");
	system("reboot &");
	
	return 0;
}

int 	nvr_do_system_shutdown()
{
	j_message( "SYSTEM REBOOT!");
	if (nvr_param_get_sys_change_flag())
	{
		nvr_param_write_sys_param_info();
	}
	j_message( "SYSTEM Ready Reboot !");
	log_write(LOG_INFO, SYSTEM_LOG, "系统关机");
	//sleep(3); //wait a moment
	nvr_channel_halt_all_ipc();
	system("hwclock -w");
	system("halt &");

	return 0;
}


