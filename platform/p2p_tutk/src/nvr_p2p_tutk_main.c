
#include <glib.h>
#include <glib-unix.h>
#include <glib/gprintf.h>

#include "nvr_headers.h"
#include "nvr_p2p_tutk.h"
#include "av_pool.h"
#include "storage_lib.h"
#include "nvr_lib_interface.h"


static gboolean nvr_platform_sig_quit(gpointer user_data)
{
	GMainLoop *mloop = (GMainLoop *)user_data;
	j_message("Receive quit signal to exit main loop!");
	g_main_loop_quit(mloop);
	return G_SOURCE_REMOVE;
}

int nvr_platform_init()
{
	av_pool_init();
	storage_lib_init();
	nvr_lib_init();
	nvr_p2p_tutk_init();
	
	return 0;
}

void nvr_platform_exit()
{
	nvr_p2p_tutk_uninit();
	nvr_lib_uninit();
	storage_lib_uninit();
	av_pool_uninit();
}

static void nvr_log_default_handle (const gchar    *log_domain,
							             GLogLevelFlags  log_level,
							             const gchar    *message,
							             gpointer        user_data)
{
	int levels[] = {LOG_EMERG, LOG_ALERT, LOG_ERR, LOG_CRIT,
						LOG_WARNING, LOG_NOTICE, LOG_INFO, LOG_DEBUG}; 
	int i, level = LOG_INFO;

	for (i = 0; i < 8; i++)
	{
		if (log_level & (1<<i))
		{
			level = levels[i];
			break;
		} 
	}

#ifdef __DEBUG__
	printf("%s", message);
#endif
	// write to log system, the log message in the /var/log/message file
	if(level < LOG_DEBUG)
		syslog(level, message);
}

int main(int argc, char *argv[])
{
	GMainLoop *mloop = NULL;

	g_log_set_always_fatal(0);
	g_log_set_fatal_mask(NULL, 0);
	g_log_set_default_handler (nvr_log_default_handle, NULL);
	mloop = g_main_loop_new(NULL, FALSE);

	g_unix_signal_add(SIGINT, nvr_platform_sig_quit, mloop);
	g_unix_signal_add(SIGTERM, nvr_platform_sig_quit, mloop);
	signal(SIGPIPE, SIG_IGN);
	
	nvr_platform_init();
	
	g_main_loop_run(mloop);

	nvr_platform_exit();
	g_main_loop_unref(mloop);
	
	return 0;
}

