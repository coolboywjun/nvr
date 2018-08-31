
#ifndef __JXJ_CU_SEARCH_H__
#define __JXJ_CU_SEARCH_H__

#include "ipnc_lib.h"

//搜索初始化
int jxj_cu_search_init();
int jxj_cu_search_uninit();

/*开始搜索:
	成功返回分配用户句柄, 失败返回0;
*/
int jxj_cu_search_start(long userDefId);

/*获得搜索:
	id: 用户句柄
	pIpcnDev: 存放设备空间
*/
int jxj_cu_search_get(long userDefId, IPNC_NET_SEARCH_RES_S *pIpncRes);

/*获取搜索设备数*/
int jxj_cu_search_get_dev_cnt(void);
/*设置搜索设备配置*/
int jxj_cu_search_set(IPNC_NET_SEARCH_CFG_REQ_S *pIpncReqCfg);

/*停止搜索*/
int jxj_cu_search_stop(long userDefId);

//请求搜索包复制到配置包
void jxj_cu_search_cpy_res2req(IPNC_NET_SEARCH_CFG_REQ_S *pIpncCfgReq,
							   IPNC_NET_SEARCH_RES_S *pIpncRes);




#endif




