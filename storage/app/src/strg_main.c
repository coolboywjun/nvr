
#undef G_LOG_DOMAIN
#define G_LOG_DOMAIN	"strg_main"

#include <strg_header.h>
#include <strg_disk.h>
#include <strg_server.h>
#include <strg_mng.h>
#include <strg_record.h>


GMainLoop *mloop;
//static int quit = 0;

static gboolean sig_quit(gpointer user_data)
{
	GMainLoop *mloop = (GMainLoop *)user_data;
	j_message("Receive quit signal to exit main loop!");
	g_main_loop_quit(mloop);
	return G_SOURCE_REMOVE;
}

void storage_default_handle(const gchar *log_domain, GLogLevelFlags log_level,
				 const gchar *message, gpointer user_data)
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
	printf("Storage version: %s\n", STORAGE_VERSION);
	printf("Build Time: %s %s\n", __DATE__, __TIME__);
	//if(!g_thread_supported())
	//	g_thread_init(NULL);
	g_log_set_always_fatal(0);
	g_log_set_fatal_mask(NULL, 0);
	g_log_set_default_handler(storage_default_handle, NULL);
	mloop = g_main_loop_new(NULL, FALSE);
	
	signal(SIGPIPE, SIG_IGN);
	g_unix_signal_add(SIGINT, sig_quit, mloop);
	g_unix_signal_add(SIGTERM, sig_quit, mloop);

	log_init();
	if (strg_mng_init() < 0) 
	{
		j_warning("Storage manager initialize failed!");
		g_main_loop_unref(mloop);
		return -1;
	}
	if (strg_record_init() < 0)
	{
		j_warning("Storage record initialize failed!");
		strg_mng_uninit();
		g_main_loop_unref(mloop);
		return -1;
	}
	if (strg_server_init() < 0)
	{
		j_warning("Storage server initialize failed!");
		strg_record_uninit();
		strg_mng_uninit();
		g_main_loop_unref(mloop);
		return -1;
	}

	g_main_loop_run(mloop); 

	// ÏÈ°Ñserver¹Ø±Õ
	strg_server_uninit();
	
	strg_record_uninit();
	strg_mng_uninit();
	log_uninit();
	g_main_loop_unref(mloop);

	return 0;
}

