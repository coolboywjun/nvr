/*******************************************************************************
--                                                                            --
--                    CedarX Multimedia Framework                             --
--                                                                            --
--          the Multimedia Framework for Linux/Android System                 --
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                         Softwinner Products.                               --
--                                                                            --
--                   (C) COPYRIGHT 2011 SOFTWINNER PRODUCTS                   --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
*******************************************************************************/

#ifndef __CEDARV_OSAL_LINUX_H_
#define __CEDARV_OSAL_LINUX_H_

#include <dec_inc/cedarx_hardware.h>

extern char* getCodecVision(void);
extern int cedarv_wait_ve_ready();
extern void cedarv_disable_ve_core();
extern void cedarv_enable_ve_core();
extern void cedarv_reset_ve_core();
extern void cedarv_request_ve_core();
extern void cedarv_release_ve_core();
extern void cedarv_set_ve_freq(int freq); //MHz
extern unsigned int cedarv_get_macc_base_address();

extern void *cedara_phymalloc(unsigned int size, int align);
extern void *cedara_phymalloc_map(unsigned int size, int align);
extern void *cedar_sys_phymalloc(unsigned int size, int align);
extern void *cedar_sys_phymalloc_map(unsigned int size, int align);
extern void cedara_phyfree(void *buf);
extern void cedara_phyfree_map(void *buf);
extern void cedar_sys_phyfree(void *buf);
extern void cedar_sys_phyfree_map(void *buf);
extern unsigned int cedarv_address_vir2phy(void *addr);
extern unsigned int cedarv_address_phy2vir(void *addr);
extern void cedarx_cache_op(long start, long end, int flag);
extern void avs_counter_config();
extern void avs_counter_start();
extern void avs_counter_pause();
extern void avs_counter_reset();
extern void avs_counter_adjust_abs(int val);
extern void avs_counter_get_time_us(signed long long *curr);

#define PHY_MALLOC(size,align)	cedar_sys_phymalloc_map((size), (align))
#define PHY_FREE(pbuf)          cedar_sys_phyfree_map((void *)(pbuf))
#define VirAddr2PhyAddr(x)		cedarv_address_vir2phy((void *)(x))
#define VirAddr2PhyAddrBufferIn(x) x

#define MALLOC					malloc
#define MEM_SET					memset
#define FREE					free

#define WARNING					LOGW
#define WRN						LOGW
#define ERROR					LOGE
#define eLIBs_printf			LOGV
#define __inf					LOGV
#define __wrn					LOGV

#define eLIBs_FlushDCacheRegion(a, b) cedarx_cache_op(a, (a + b), CEDARX_DCACHE_FLUSH)
#define eLIBs_CleanFlushDCacheRegion(a, b) cedarx_cache_op(a, (a + b), CEDARX_DCACHE_CLEAN_FLUSH)

#endif
