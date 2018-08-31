#include <stdio.h>

#include <syscfg.h>

int main()
{
    DEV_SystemCfg *pCfg;
    
    pCfg = get_system_cfg();

   if(pCfg == NULL)
   {
	printf("get system config err\r\n");
	return 0;
   }
   printf("Magic: 0x%08X\r\n", pCfg->Magic);
   printf("dev Serial ID:%s\r\n", pCfg->u8SerialId);
   printf("Alarm input number:%d\r\n", pCfg->u8DiNum);
   printf("Alarm output number:%d\r\n", pCfg->u8DoNum);
   printf("Channel number:%d\r\n", pCfg->u8ChnNum);
   printf("RS232 number:%d\r\n", pCfg->u8Rs232Num);
   printf("RS485 number:%d\r\n", pCfg->u8Rs485Num);
   printf("Netword card number:%d\r\n", pCfg->u8NetCardNum);
   printf("Support audio:%d\r\n", pCfg->u8SupportAudio);
   printf("Support hdmi: %d\r\n", pCfg->u8SupportHDMI);
   printf("Support vga: %d\r\n", pCfg->u8supportVGA);
   printf("Support sata num: %d\r\n", pCfg->u8SataNum);
   printf("Device type:0x%04X\r\n", pCfg->u16DevType);
   printf("Hardware version:%s\r\n", pCfg->u8HwVer);
   printf("Mac:%02x:%02x:%02x:%02x:%02x:%02x\r\n", pCfg->u8Mac[0], 
   	pCfg->u8Mac[1], pCfg->u8Mac[2], pCfg->u8Mac[3], pCfg->u8Mac[4], pCfg->u8Mac[5]);
   printf("Factory check:%u\n", pCfg->u8FactoryChecked);
   printf("Use watch dog:%u\n", !pCfg->u8NoWdt);
   printf("Default IP:%d.%d.%d.%d\r\n", ((pCfg->defaultIP>>24)&0xff), ((pCfg->defaultIP>>16)&0xff),((pCfg->defaultIP>>8)&0xff), ((pCfg->defaultIP)&0xff) );
   printf("Default Mask:%d.%d.%d.%d\r\n", ((pCfg->defaultMask>>24)&0xff), ((pCfg->defaultMask>>16)&0xff),((pCfg->defaultMask>>8)&0xff), ((pCfg->defaultMask)&0xff) );
   printf("Default Gateway:%d.%d.%d.%d\r\n", ((pCfg->defaultGateWay>>24)&0xff), ((pCfg->defaultGateWay>>16)&0xff),((pCfg->defaultGateWay>>8)&0xff), ((pCfg->defaultGateWay)&0xff) );
   
   // printf platform server type
   printf("Support platform server: JXJ ");
   if(pCfg->platformServer & PLATFORM_ONVIF_SERVER)
   	printf("ONVIF ");
   if(pCfg->platformServer & PLATFORM_TUTK_SERVER)
   	printf("TUTK ");
   if(pCfg->platformServer & PLATFORM_VIVI_SERVER)
   	printf("VIVI ");
   if(pCfg->platformServer & PLATFORM_KANDIAN_SERVER)
   	printf("KANDIAN ");
   printf("\n");   
   
   // printf platform client type
   printf("Support platform client: JXJV1 JXJV2 ");
   if(pCfg->platformClient& PLATFORM_ONVIF_CLIENT)
   	printf("ONVIF ");
   printf("\n");

   // printf language type
   printf("Support language: ");
   if(pCfg->supportLang & (1<<LANG_SIMPLIFIED_CHINESE))
   	printf("Simplified chinese, ");
   if(pCfg->supportLang & (1<<LANG_ENGLISH))
   	printf("English, ");
   if(pCfg->supportLang & (1<<LANG_TRADITIONAL_CHINESE))
   	printf("Traditional chinese, ");
   printf("\n");
   
   printf("Default language: ");
   if(pCfg->u8DefLang == LANG_SIMPLIFIED_CHINESE)
   	printf("Simplified chinese");
   else if(pCfg->u8DefLang == LANG_ENGLISH)
   	printf("English");
   else if(pCfg->u8DefLang == LANG_TRADITIONAL_CHINESE)
   	printf("Traditional chinese");
   printf("\n");
   
   DEV_KanDianCfg *pKdCfg = get_kandian_cfg();
   printf("KIANDIAN szGroupUrl:%s\n", pKdCfg->szGroupUrl);
   printf("KIANDIAN szProvincialUrl:%s\n", pKdCfg->szProvincialUrl);
   printf("KIANDIAN szDevId:%s\n", pKdCfg->szDevId);
   printf("KIANDIAN szKey:%s\n", pKdCfg->szKey);
   printf("KIANDIAN szMode:%s\n", pKdCfg->szMode);
   
   return 0;
}


