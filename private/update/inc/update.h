#ifndef __NVR_UPDATE_H__
#define __NVR_UPDATE_H__


#define UPDATE_FILE_HDR_MAGIC 0x415159a2UL

typedef struct _NVR_UPGRADE_FILE_HDR_S
{
	uint32_t		u32Magic;		//update file magic flag 
	uint32_t		u32SuitMachine;  //flag all the  suitable machine 
	uint32_t		u32FileNum;		//update file num
	uint32_t		u32TotalLen;		//update file total len
	uint8_t		u8RebootFlag;	//the reboot flag
	uint8_t		u8Res[3];
	char			szSoftVersion[16];
	uint8_t		u8Res1[12];
}NVR_UPGRADE_FILE_HDR_S, *LPNVR_UPGRADE_FILE_HDR_S;

typedef struct _NVR_UPGRADE_FILE_INFO_S
{
	char			szFileName[128];
	uint32_t		u32FileSize;
	uint32_t		u32FileAttr;
	uint8_t		u8Res[120];
}NVR_UPGRADE_FILE_INFO_S, *LPNVR_UPGRADE_FILE_INFO_S;



int nvr_update_write(char *pData, int nLen, int nPackNo, int nTotalLen);

int nvr_update_get_percent();


int nvr_update_udisk_get_file(char *filepath, char *filename);

void nvr_update_udisk_release_file(char *filepath, char *filename);





#endif

