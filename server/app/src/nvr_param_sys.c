
#undef	G_LOG_DOMAIN
#define	G_LOG_DOMAIN "param"

#include <nvr_server_headers.h>

#include <nvr_param_sys.h>
#include <nvr_param_default.h>
#include <nvr_param_check.h>

#define	NVR_PARAM_SYS_FILE						"/mnt/nand/sysconf"
#define	NVR_PARAM_SYS_BACKUP_FILE				"/mnt/nand/sysconf_bak"

#define	NVR_PARAM_STABLE_FILE					"/mnt/nand/devStable"
#define	NVR_PARAM_STABLE_BACKUP_FILE			"/mnt/nand/devStable_bak"

NVR_PARAM_MAN_S  			gParamMan;
NVR_PARAM_SYS_INFO_S		gSysInfo;
NVR_DEV_STABLE_INFO_S	gStableInfo;
gint						gIsDefaultFileSystem = 0;

static gboolean nvr_param_write_file(char *szFileName, char *szData, int nLen)
{
	int i = 0;
    //int s32Ret = 0;
	FILE *pFile = NULL;
	gpointer *pTmpBuf;
	int flags;
		
	if ((pFile = fopen(szFileName, "wb" )) == NULL)
	{
		j_warning("param write file %s err.", szFileName);
		return FALSE;
	}
	else
	{
		flags = fcntl(fileno(pFile),F_GETFL,0);
		flags |= O_SYNC;
		fcntl(fileno(pFile),F_SETFL,flags);
		fwrite(szData, 1, nLen, pFile);
		fflush(pFile);
		fclose(pFile);
		pFile = NULL;
	}

	pTmpBuf = g_malloc(nLen);
	//检查参数是否写成功
	for(i = 0; i < 5; i++)
	{
		if((pFile = fopen(szFileName, "rb" )))
		{
			//s32Ret = fread(pTmpBuf, 1, nLen, pFile);
			fread(pTmpBuf, 1, nLen, pFile);
			fclose(pFile);
			pFile = NULL;     
		}
		else
		{
			usleep(10 * 1000);
			continue;
		}

		if(memcmp(pTmpBuf, szData, nLen) != 0)
		{    
			if((pFile = fopen(szFileName, "wb" )) != NULL)
			{
				fwrite(szData, 1, nLen, pFile); 
				fflush(pFile);
				fclose(pFile);
				pFile = NULL;      
			}							
		}
		else
		{
			j_message("param write file %s ok.", szFileName);
			g_free(pTmpBuf);
			return TRUE;
		}
	}
	g_free(pTmpBuf);
	j_warning("param write file %s err.", szFileName);	
	return FALSE;
}

gint nvr_param_read_sys_param_info()
{
	FILE   *pFile 	     = NULL;
	FILE   *pFileBack = NULL;
	gint	    nRet	     = 0;
	
	NVR_PARAM_SYS_INFO_S	 struSysInfo;
	NVR_PARAM_SYS_INFO_S	 struBackSysInfo;
	
	do{
		pFile 	 = fopen(NVR_PARAM_SYS_FILE, 	      "rb");
		pFileBack = fopen(NVR_PARAM_SYS_BACKUP_FILE, "rb");

		nvr_param_default_sys_cfg();
		memcpy(&struSysInfo, 	    &gSysInfo, sizeof(NVR_PARAM_SYS_INFO_S));
		memcpy(&struBackSysInfo,  &gSysInfo, sizeof(NVR_PARAM_SYS_INFO_S));
		
		if (pFile)
		{
			fread((char *)(&struSysInfo), 1,  sizeof(NVR_PARAM_SYS_INFO_S), pFile);
			fclose(pFile);
		}

		if (pFileBack)
		{
			fread((char *)(&struBackSysInfo), 1,  sizeof(NVR_PARAM_SYS_INFO_S), pFileBack);
			fclose(pFileBack);
		}

		if (pFile == NULL && pFileBack == NULL)
		{
			nRet= -1;

			break;
		}

		//pthread_rwlock_wrlock(&gParamMan.paramLock);
		if (pFile != NULL && pFileBack == NULL)
		{
			memcpy(&gSysInfo, &struSysInfo, sizeof(NVR_PARAM_SYS_INFO_S));	
		}
		else if (pFile == NULL && pFileBack != NULL)
		{
			memcpy(&gSysInfo, &struBackSysInfo, sizeof(NVR_PARAM_SYS_INFO_S));	
		}
		else
		{
			if (memcmp(&struSysInfo, &struBackSysInfo, sizeof(NVR_PARAM_SYS_INFO_S)) != 0)
			{
				if (struSysInfo.u32Size == sizeof(NVR_PARAM_SYS_INFO_S) ||
				    struBackSysInfo.u32Size != sizeof(NVR_PARAM_SYS_INFO_S))
				{
					memcpy(&gSysInfo, &struSysInfo, sizeof(NVR_PARAM_SYS_INFO_S));
				}
				else
				{
					memcpy(&gSysInfo, &struBackSysInfo, sizeof(NVR_PARAM_SYS_INFO_S));
				}
			}
			else
				memcpy(&gSysInfo, &struSysInfo, sizeof(NVR_PARAM_SYS_INFO_S));
		}
		//pthread_rwlock_unlock(&gParamMan.paramLock);
	}while(0);
	return nRet;
}

gboolean nvr_param_write_sys_param_info()
{
	if (nvr_param_write_file(NVR_PARAM_SYS_FILE, (char *)&gSysInfo, sizeof(gSysInfo)) 
		== TRUE &&
		nvr_param_write_file(NVR_PARAM_SYS_BACKUP_FILE, (char *)&gSysInfo, sizeof(gSysInfo)) 
		== TRUE)
	{
		nvr_param_no_change();
		j_message("sys param save ok!");
		return TRUE;
	}
	
	return FALSE;
}


static void   nvr_param_init_sys_param_info()
{
	nvr_param_default_net_cfg();
	nvr_param_default_sys_cfg();
}

gint	nvr_param_read_stable_param_info()
{
	FILE   *pFile 	     = NULL;
	FILE   *pFileBack = NULL;
	gint	    s32Ret	      = 0;
	
	NVR_DEV_STABLE_INFO_S 	struStableInfo;
	NVR_DEV_STABLE_INFO_S	struBackStableInfo;

	memset(&struStableInfo, 		0, sizeof(NVR_DEV_STABLE_INFO_S));
	memset(&struBackStableInfo,  	0, sizeof(NVR_DEV_STABLE_INFO_S));
	
	do{
		pFile 	 = fopen(NVR_PARAM_STABLE_FILE, 		    "rb");
		pFileBack = fopen(NVR_PARAM_STABLE_BACKUP_FILE,	    "rb");

		memset(&struStableInfo, 	       0, sizeof(NVR_DEV_STABLE_INFO_S));
		memset(&struBackStableInfo,  	0, sizeof(NVR_DEV_STABLE_INFO_S));
		
		if (pFile)
		{
			fread((char *)(&struStableInfo), 1,  sizeof(NVR_DEV_STABLE_INFO_S), pFile);
			fclose(pFile);
		}

		if (pFileBack)
		{
			fread((char *)(&struBackStableInfo), 1,  sizeof(NVR_DEV_STABLE_INFO_S), pFileBack);
			fclose(pFileBack);
		}

		if (pFile == NULL && pFileBack == NULL)
		{
			s32Ret= -1;

			break;
		}

		//pthread_rwlock_wrlock(&gParamMan.paramLock);
		
		if (pFile != NULL && pFileBack == NULL)
		{
			memcpy(&gStableInfo, &struStableInfo, sizeof(NVR_DEV_STABLE_INFO_S));	
		}
		else if (pFile == NULL && pFileBack != NULL)
		{
			memcpy(&gStableInfo, &struBackStableInfo, sizeof(NVR_DEV_STABLE_INFO_S));	
		}
		else
		{
			if (memcmp(&struStableInfo, &struBackStableInfo, sizeof(NVR_DEV_STABLE_INFO_S)) != 0)
			{
				if (struStableInfo.u32Size == sizeof(NVR_DEV_STABLE_INFO_S) ||
				    struBackStableInfo.u32Size != sizeof(NVR_DEV_STABLE_INFO_S))
				{
					memcpy(&gStableInfo, &struStableInfo, sizeof(NVR_DEV_STABLE_INFO_S));
				}
				else
				{
					memcpy(&gStableInfo, &struBackStableInfo, sizeof(NVR_DEV_STABLE_INFO_S));
				}
			}
			else
				memcpy(&gStableInfo, &struStableInfo, sizeof(NVR_DEV_STABLE_INFO_S));
		}

		//pthread_rwlock_unlock(&gParamMan.paramLock);
	}while(0);

	return s32Ret;
}

static gboolean nvr_param_write_stable_param_info()
{
	if (nvr_param_write_file(NVR_PARAM_STABLE_FILE, (char *)&gStableInfo, sizeof(gStableInfo)) 
		== TRUE &&
		nvr_param_write_file(NVR_PARAM_STABLE_BACKUP_FILE, (char *)&gStableInfo, sizeof(gStableInfo)) 
		== TRUE)
	{
		j_message("stable param save ok!");
		return TRUE;
	}

	return FALSE;
}

static gboolean   nvr_param_init_stable_param_info()
{
	int i;
	memset(&gStableInfo, 0, sizeof(gStableInfo));
	DEV_SystemCfg *pCfg = get_system_cfg();
	
	gStableInfo.u32Size = sizeof(NVR_DEV_STABLE_INFO_S);
	g_stpcpy(gStableInfo.szSerialId, (char *)pCfg->u8SerialId);
	g_stpcpy(gStableInfo.szHwVer, (char *)pCfg->u8HwVer);
	g_stpcpy(gStableInfo.szSoftVer, NVR_SOFT_VERSION);
	
	for (i = 0; i < NVR_ETHERNET_NUM; ++i)
	{
		gStableInfo.u8MacAddr[i][0] = pCfg->u8Mac[0];
		gStableInfo.u8MacAddr[i][1] = pCfg->u8Mac[1];
		gStableInfo.u8MacAddr[i][2] = pCfg->u8Mac[2];
		gStableInfo.u8MacAddr[i][3] = pCfg->u8Mac[3];
		gStableInfo.u8MacAddr[i][4] = pCfg->u8Mac[4];
		gStableInfo.u8MacAddr[i][5] = pCfg->u8Mac[5] + i;

		gStableInfo.u32IPAddr[i] = pCfg->defaultIP + i;
		gStableInfo.u32NetMask[i]= pCfg->defaultMask;
		gStableInfo.u32GateWay[i]= pCfg->defaultGateWay;
	}

	gStableInfo.u8SupportAudio = pCfg->u8SupportAudio;
	gStableInfo.u8SupportHDMI = pCfg->u8SupportHDMI;
	gStableInfo.u8supportVGA = pCfg->u8supportVGA;
	
	gStableInfo.u8NetNum = (pCfg->u8NetCardNum <= NVR_ETHERNET_NUM)
							? pCfg->u8NetCardNum : NVR_ETHERNET_NUM;

	gStableInfo.u8SataNum = pCfg->u8SataNum;
	
	gStableInfo.u8ChnNum = (pCfg->u8ChnNum <= NVR_MAX_CHANNEL_NUM)
							? pCfg->u8ChnNum : NVR_MAX_CHANNEL_NUM;
	
	gStableInfo.u8RS232Num = (pCfg->u8Rs232Num <= NVR_MAX_SUPPORT_RS232)
							  ? pCfg->u8Rs232Num : NVR_MAX_SUPPORT_RS232;

	gStableInfo.u8RS485Num = (pCfg->u8Rs485Num <= NVR_MAX_SUPPORT_RS485)
							  ? pCfg->u8Rs485Num : NVR_MAX_SUPPORT_RS485;

	gStableInfo.u8AlarmInputNum = (pCfg->u8DiNum <= NVR_MAX_SUPPORT_DI)
								   ? pCfg->u8DiNum :  NVR_MAX_SUPPORT_DI;

	gStableInfo.u8AlarmOutputNum = (pCfg->u8DoNum <= NVR_MAX_SUPPORT_DO)
									? pCfg->u8DoNum : NVR_MAX_SUPPORT_DO;
	
	return TRUE;
}

int nvr_param_set_sys_change_flag(int bFlag)
{
	PL;
	gParamMan.sysParamChange = bFlag;
	PNL;
	
	return NVR_TRUE;
}

int nvr_param_get_sys_change_flag()
{
	int bFlag;
	
	PL;
	bFlag = gParamMan.sysParamChange;
	PNL;
	
	return bFlag;
}

static void nvr_param_check_default_filesystem()
{
	FILE *fp;
	char cmdline[1024];

	fp = fopen("/proc/cmdline", "r");
	if(fp != NULL)
	{
		fgets(cmdline, sizeof(cmdline), fp);
		if(strstr(cmdline, "default"))
			g_atomic_int_set(&gIsDefaultFileSystem, 1);
		fclose(fp);
	}
}

#define NVR_PARAM_EXPORT_HEAD_MAGIC		('J' | ('E' << 8) | ('X' << 16) | ('P' << 24))
typedef struct _NVR_PARAM_EXPORT_HEAD_S_
{
	uint32_t u32Magic;
	uint32_t u32Version;
	uint32_t u32Length;
	gchar 	szMd5Sum[128];
}NVR_PARAM_EXPORT_HEAD_S;

int nvr_param_export(const char *pExportPath)
{
	FILE *fp;
	gchar *pMd5Sum;
	NVR_PARAM_EXPORT_HEAD_S stExportHead;
	if(pExportPath == NULL)
		return -1;
	
	stExportHead.u32Magic = NVR_PARAM_EXPORT_HEAD_MAGIC;
	stExportHead.u32Version = 1;
	stExportHead.u32Length = sizeof(gSysInfo);
	pMd5Sum = g_compute_checksum_for_data(G_CHECKSUM_MD5, (guchar *)&gSysInfo, sizeof(gSysInfo));
	if(pMd5Sum == NULL)
		return -1;
	g_strlcpy(stExportHead.szMd5Sum, pMd5Sum, sizeof(stExportHead.szMd5Sum));
	g_free(pMd5Sum);
	fp = fopen(pExportPath, "wb");
	if(fp == NULL)
	{
		j_warning("Open export file %s failed.", pExportPath);
		return -1;
	}
	if(fwrite(&stExportHead, sizeof(stExportHead), 1, fp) != 1)
	{
		j_warning("Write export head failed.");
		fclose(fp);
		return -1;
	}
	PL;
	if(fwrite(&gSysInfo, sizeof(gSysInfo), 1, fp) != 1)
	{
		j_warning("Write sysinfo failed.\n");
		PNL;
		fclose(fp);
		return -1;
	}
	PNL;
	fclose(fp);
	return 0;
}

int nvr_param_import(const char *pImportPath)
{
	FILE *fp;
	NVR_PARAM_EXPORT_HEAD_S stExportHead;
	NVR_DEV_NET_CFG_S struNetCfg;
	guchar *pParam;
	gchar *pMd5Sum;
	
	if(pImportPath == NULL)
		return -1;
	fp = fopen(pImportPath, "rb");
	if(fp == NULL)
	{
		j_warning("Open import file %s failed.", pImportPath);
		return -1;
	}
	if(fread(&stExportHead, sizeof(stExportHead), 1, fp) != 1)
	{
		j_warning("Read export head failed.");
		fclose(fp);
		return -1;
	}
	// 参数不可能大于16M
	if(stExportHead.u32Magic != NVR_PARAM_EXPORT_HEAD_MAGIC || stExportHead.u32Length > 0x1000000)
	{
		j_warning("The export head magic is wrong.");
		fclose(fp);
		return -1;
	}
	pParam = g_malloc(stExportHead.u32Length);
	if(fread(pParam, stExportHead.u32Length, 1, fp) != 1)
	{
		j_warning("Read export param failed.");
		g_free(pParam);
		fclose(fp);
		return -1;
	}
	pMd5Sum = g_compute_checksum_for_data(G_CHECKSUM_MD5, (guchar *)pParam, stExportHead.u32Length);
	if(pMd5Sum == NULL)
	{
		j_warning("Export param calc md5sum failed.");
		g_free(pParam);
		fclose(fp);
		return -1;
	}
	if(g_strcmp0(pMd5Sum, stExportHead.szMd5Sum) != 0)
	{
		j_warning("Export param check md5sum failed.");
		g_free(pMd5Sum);
		g_free(pParam);
		fclose(fp);
		return -1;
	}
	g_free(pMd5Sum);
	PL;
	memcpy(&struNetCfg, &gSysInfo.struNetCfg, sizeof(struNetCfg));
	if(memcmp(&gSysInfo, pParam, stExportHead.u32Length) != 0)
	{
		memcpy(&gSysInfo, pParam, stExportHead.u32Length);
		memcpy(&gSysInfo.struNetCfg, &struNetCfg, sizeof(gSysInfo.struNetCfg));
		PC;
	}
	PNL;
	g_free(pParam);
	fclose(fp);

	return 0;
}

gboolean	nvr_param_sw_reset()
{//软复位网络ip mac 地址不恢复默认参数
	nvr_param_default_sys_cfg();
	return nvr_param_write_sys_param_info();
}

gboolean	nvr_param_hw_reset()
{//硬复位网络ip mac 地址恢复默认参数
	nvr_param_init_sys_param_info();
	return nvr_param_write_sys_param_info();	
}

static gboolean nvr_param_timeout_fxn(gpointer user_data)
{
	//j_message("param timeout, check the param is changed.\n");
	if(g_atomic_int_get(&gParamMan.sysParamChange))
	{
		nvr_param_lock();
		if(nvr_param_write_sys_param_info() < 0)
			j_warning("Write sys param to flash error.\n");
		nvr_param_unlock();
	}
	return G_SOURCE_CONTINUE;
}

int nvr_param_init()
{
	j_message("Start nvr param init.");
	memset(&gParamMan, 0, sizeof(gParamMan));

	g_mutex_init(&gParamMan.paramLock);

	j_message("ready read stable info param");
	if (nvr_param_read_stable_param_info() == -1)
	{
		j_warning("read stable info param fail get default param");
		nvr_param_init_stable_param_info();
		nvr_param_write_stable_param_info();
	}
	else
	{
		nvr_param_check_stable_param_info();
	}
	
	j_message("ready read sys info param");
	if (nvr_param_read_sys_param_info() == -1)
	{
		j_warning("ready read sys param fail get default param");	
		nvr_param_init_sys_param_info();
		nvr_param_write_sys_param_info();
	}
	else
	{
		nvr_param_check_sys_param_info(&gSysInfo);
	}
	j_message("ready read param finish");
	j_message("nvr param init ok");
	
	nvr_param_check_default_filesystem();
	g_timeout_add_seconds(15, nvr_param_timeout_fxn, NULL);

	j_message("Param init finish.");
	return 0;
}


void nvr_param_uninit()
{
	j_message("nvr param uninit.");
	g_mutex_clear(&gParamMan.paramLock);
	j_message("nvr param uninit finish.");
}



