
#include <nvr_server_headers.h>
#include <nvr_ddns_api.h>
#include <nvr_param_proc.h>


typedef enum _NVR_DDNS_CTRL_CMD_E_
{
	NVR_DDNS_REFRESH = 0x01,//Ë¢ÐÂ
	NVR_DDNS_WAN_REFRESH =  0x02,
}NVR_DDNS_CTRL_CMD_E;


static GThreadPool *gDdnsThreadPool;
static char gCurWanIp[64] = {0};


static int nvr_ddns_refresh()
{
	NVR_DDNS_CFG_S stDdnsCfg;
	int nErr = 0;
	char szSvrIp[2][32]  = {DYNDNS_DEFAULT_SERVER, DDNS3322_DEFAULT_SERVER};
	char szRequest[2][32]= {DYNDNS_REQUEST, DDNS3322_REQUEST};
	//char szLocalIp[32] = {0};
	
	nvr_get_ddns_cfg(0, &stDdnsCfg);
	if(!stDdnsCfg.u8DDNSOpen || strlen(stDdnsCfg.szDDNSUsr) == 0 
		|| strlen(stDdnsCfg.szDDNSAccount) == 0 || stDdnsCfg.u16DDNSPort == 0)
		return 0;
	
	if (NVR_UPDATERES_OK != (nErr = nvr_ddns_update_entry(
											 szSvrIp[stDdnsCfg.u8DDNSType%2], 
											 stDdnsCfg.szDDNSAccount,
											 stDdnsCfg.szDDNSUsr,
											 stDdnsCfg.szDDNSPsw,
											 NULL,
											 stDdnsCfg.u16DDNSPort,
											 szRequest[stDdnsCfg.u8DDNSType%2],
											 0)))
	{
		j_warning("Do Ddns fail(%d).", nErr);
		return -1;
	}
	nvr_ddns_get_wan_ip(gCurWanIp, sizeof(gCurWanIp));
	j_debug("Do DDNS Success...");
	
	return 0;
}

static int nvr_ddns_wan_refresh()
{
	NVR_DDNS_CFG_S stDdnsCfg;
	char szWanIp[64] = {0};
	
	nvr_get_ddns_cfg(0, &stDdnsCfg);
	if(!stDdnsCfg.u8DDNSOpen)
		return 0;
	if(nvr_ddns_get_wan_ip(szWanIp, sizeof(szWanIp)) < 0)
		return -1;
	if(g_strcmp0(szWanIp, gCurWanIp) == 0)
		return 0;
	return nvr_ddns_refresh();
}

static void nvr_ddns_server_func(gpointer data, gpointer user_data)
{
	NVR_DDNS_CTRL_CMD_E cmdVal = GPOINTER_TO_INT(data);

	switch(cmdVal)
	{
		case NVR_DDNS_REFRESH:
			nvr_ddns_refresh();
			break;
		case NVR_DDNS_WAN_REFRESH:
			nvr_ddns_wan_refresh();
			break;
		default:
			g_assert_not_reached();
			break;
	}
}

static inline gboolean nvr_ddns_send_cmd(NVR_DDNS_CTRL_CMD_E cmdVal)
{
	return g_thread_pool_push(gDdnsThreadPool, GINT_TO_POINTER(cmdVal), NULL);
}

static gboolean nvr_ddns_timeout(gpointer user_data)
{
	nvr_ddns_send_cmd(NVR_DDNS_REFRESH);
	return G_SOURCE_CONTINUE;
}

static gboolean nvr_wan_refresh_timeout(gpointer user_data)
{
	nvr_ddns_send_cmd(NVR_DDNS_WAN_REFRESH);
	return G_SOURCE_CONTINUE;
}

int nvr_ddns_update()
{
	NVR_DDNS_CFG_S stDdnsCfg;
	static guint timeid = 0;
	static guint wanRefreshId = 0;
	
	nvr_get_ddns_cfg(0, &stDdnsCfg);
	if(stDdnsCfg.u8DDNSOpen)
	{
		nvr_ddns_send_cmd(NVR_DDNS_REFRESH);
		if(timeid > 0)
			g_source_remove(timeid);
		timeid = g_timeout_add_seconds(stDdnsCfg.u32DDNSTimes * 3600, nvr_ddns_timeout, NULL);
		if(wanRefreshId > 0)
			g_source_remove(wanRefreshId);
		wanRefreshId = g_timeout_add_seconds(15 * 60, nvr_wan_refresh_timeout, NULL);
	}
	else
	{
		if(timeid > 0)
		{
			g_source_remove(timeid);
			timeid = 0;
		}
		if(wanRefreshId > 0)
		{
			g_source_remove(wanRefreshId);
			wanRefreshId = 0;
		}
	}
	return 0;
}

int nvr_ddns_init()
{
	gDdnsThreadPool = g_thread_pool_new(nvr_ddns_server_func, NULL, 1, FALSE, NULL);
	nvr_ddns_update();
	return 0;
}

void nvr_ddns_uninit()
{
	g_thread_pool_free(gDdnsThreadPool, FALSE, TRUE);
}

