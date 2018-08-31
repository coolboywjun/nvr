#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN	"log"

#include <nvr_server_headers.h>

static gboolean 
nvr_log_timeout(gpointer user_data)
{
	// check the log file size, if the size is to big, then we delete it.
	//j_message("log timeout, check the log file size.\n");
	return G_SOURCE_CONTINUE;
}

static void
nvr_log_default_handle (const gchar    *log_domain,
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


int nvr_log_init()
{
	g_log_set_always_fatal(0);
	g_log_set_fatal_mask(NULL, 0);
	g_log_set_default_handler (nvr_log_default_handle, NULL);

	// 定时检测LOG文件大小,过大则自动删除
	g_timeout_add_seconds(60, nvr_log_timeout, NULL);
	j_message("Log init finish.");
	return 0;
}

void nvr_log_uninit()
{
	j_message("log uninit.");
}

