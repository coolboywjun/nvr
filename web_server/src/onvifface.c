#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
//#include <onvif.h>

#include "stdsoap2.h"
#include "soapH.h"


//#include <sysctrl.h>




int ocheckusername(struct soap *soap)
{
#if 0
	int i;
	char *_Nonce;	
	char *_Created;
	SysInfo* oSysInfo = GetSysInfo();

	for(i = 0; i < ACOUNT_NUM; i++) 
	{			
		if (strcmp(oSysInfo->acounts[i].user,soap->header->wsse__Security->UsernameToken->Username)==0) 
		{
			//Check Password include here

			return oSysInfo->acounts[i].authority;
			// 1: admin , 2: operator, 3: viewer, 4: supervisor
		}
	}
#endif
	return 0;
}

char* oget_ipaddress(void)
{
#if 0
	NET_IPV4 ip;
	ip.int32 = net_get_ifaddr(ETH_NAME);
	char *_IPv4Address = (char*)malloc(20*sizeof(char)); 
	sprintf(_IPv4Address, "http://%03d.%03d.%03d.%03d", ip.str[0], ip.str[1], ip.str[2], ip.str[3]);
	
	return _IPv4Address;
#endif
	return NULL;
}
int oget_encoding(void)
{
	//ipnc  {H264 = 0, MPEG4 = 1, MegaPixel = 2};	
	//onvif {JPEG = 0, MPEG4 = 1, H264 = 2};
#if 0
	SysInfo* oSysInfo = GetSysInfo();
	int encode=oSysInfo->lan_config.nVideocombo;
	if(encode==0)	
		encode = 2;
	else if(encode==1) 
		encode = 1;
	else if(encode==2)
	{
		if((oSysInfo->lan_config.nVideocodecres == 0) || (oSysInfo->lan_config.nVideocodecres == 2) || (oSysInfo->lan_config.nVideocodecres == 4))//H264
			encode = 2; 
		else encode = 0;
	}
	return encode;
#endif	
	return 0;
}
int oget_bitrate(void)
{
#if 0
	SysInfo* oSysInfo = GetSysInfo();
	return oSysInfo->lan_config.nMpeg41bitrate/1000;
#endif
	return 0;
}
int oget_framerate(void)
{
#if 0
	SysInfo* oSysInfo = GetSysInfo();
	if((oSysInfo->lan_config.nVideocombo == 0) || (oSysInfo->lan_config.nVideocombo == 1)) // H264 or MPEG4
	{
		if((oSysInfo->lan_config.nVideocodecres == 0) || (oSysInfo->lan_config.nVideocodecres == 1) || (oSysInfo->lan_config.nVideocodecres == 2)) //720/D1/SXVGA
		{
			if(oSysInfo->lan_config.nFrameRate1 == 0)	return 30;
			else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
			else if(oSysInfo->lan_config.nFrameRate1 == 2)	return 15;
			else return 8;
		}
		else if(oSysInfo->lan_config.nVideocodecres == 3) //1080
		{
			if(oSysInfo->lan_config.nFrameRate1 == 0)	return 23;
			else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 15;
			else return 8;
		}
		else // 720MAX60 - AUTO
		{
			//sprintf(StreamingChannel.maxFrameRate,"AUTO");			
		}
	}

	if(oSysInfo->lan_config.nVideocombo == 2) // MegaPixel
	{
		if((oSysInfo->lan_config.nVideocodecres == 0) || (oSysInfo->lan_config.nVideocodecres == 1)) // 2MP
		{
			if(oSysInfo->lan_config.nFrameRate1 == 0)		return 30;
			else if(oSysInfo->lan_config.nFrameRate1 == 1)  return 24;
		    else if(oSysInfo->lan_config.nFrameRate1 == 2)	return 15;
		    else return 8;
		}
		else if((oSysInfo->lan_config.nVideocodecres == 2) || (oSysInfo->lan_config.nVideocodecres == 3)) // 3MP
		{			
			if(oSysInfo->lan_config.nFrameRate1 == 1)  return 15;
		    else return 8;
		}
		else // 5MP
		{
			//sprintf(StreamingChannel.maxFrameRate,"AUTO");			
		}
	}

	#endif

	return 0;
}

int oget_ratecontrol(void)
{
	//SysInfo* oSysInfo = GetSysInfo();
	//return oSysInfo->lan_config.nRateControl1;
	return 0;
}

int oget_ipratio(void)
{
	//SysInfo* oSysInfo = GetSysInfo();
	//return oSysInfo->codec_advconfig[0].ipRatio;

	return 0;
}

int oget_resolutionwidth(void)
{
#if 0
	SysInfo* oSysInfo = GetSysInfo();
	
	if(oSysInfo->lan_config.nVideocombo == 0) //--------------- 264 --------------//
	{
		  if(oSysInfo->lan_config.nVideocodecres == 0)		// 720
		  {
		    return 1280; //return 720;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 1) // D1
		  {
		    return 720;	//return 480;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 2) // H264:SXVGA
		  {
		    return 1280; //return 960;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 3) // H264:1080
		  {
		    return 1920; //return 1080;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 4) // H264:720MAX60
		  {
		    return 1280;	//return 720;
		  }
	}
	
	else if (oSysInfo->lan_config.nVideocombo == 1) //--------------- MPEG4 --------------//
	{
		  if(oSysInfo->lan_config.nVideocodecres == 0)		// MPEG4:720
		  {
		    return 1280;	//return 720;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 1) // MPEG4:D1
		  {
		    return 720;		//return 480;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 2) // MPEG4:SXVGA
		  {
		    return 1280;	//return 960;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 3) // MPEG4:1080
		  {
		    return 1920;	//return 1080;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 4) // MPEG4:720MAX60
		  {
		    return 1280;	//return 720;
		  }
	}
	
	else if (oSysInfo->lan_config.nVideocombo == 2) //--------------- MegaPixel --------------// 
	{
		  if((oSysInfo->lan_config.nVideocodecres == 0) || (oSysInfo->lan_config.nVideocodecres == 1))	// JPG:1600X1200
		  {
			return 1600;	//return 1200;
		  }
		  else if((oSysInfo->lan_config.nVideocodecres == 2) || (oSysInfo->lan_config.nVideocodecres == 3))// JPG:2048X1536
		  {
			return 2048;	//return 1536;
		  }
		  else if((oSysInfo->lan_config.nVideocodecres == 4) || (oSysInfo->lan_config.nVideocodecres == 5))// JPG:2048X1536
		  {
			return 2592;	//return 1920;
		  }
     }
  #endif

  return 0;
}
int oget_resolutionheight(void)
{
#if 0
	SysInfo* oSysInfo = GetSysInfo();

	if(oSysInfo->lan_config.nVideocombo == 0) //--------------- 264 --------------//
	{
		  if(oSysInfo->lan_config.nVideocodecres == 0)		// 720
		  {
		    //return 1280; //
			  return 720;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 1) // D1
		  {
		    //return 720;	//
			  return 480;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 2) // H264:SXVGA
		  {
		    //return 1280; //
			  return 960;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 3) // H264:1080
		  {
		    //return 1920; //
			  return 1080;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 4) // H264:720MAX60
		  {
		    //return 1280; //
			  return 720;
		  }
	}
	
	else if (oSysInfo->lan_config.nVideocombo == 1) //--------------- MPEG4 --------------//
	{
		  if(oSysInfo->lan_config.nVideocodecres == 0)		// MPEG4:720
		  {
		    //return 1280;	//
			  return 720;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 1) // MPEG4:D1
		  {
		    //return 720;	//
			  return 480;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 2) // MPEG4:SXVGA
		  {
		    //return 1280;	//
			  return 960;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 3) // MPEG4:1080
		  {
		    //return 1920;	//
			  return 1080;
		  }
		  else if(oSysInfo->lan_config.nVideocodecres == 4) // MPEG4:720MAX60
		  {
		    //return 1280; //
			  return 720;
		  }
	}
	
	else if (oSysInfo->lan_config.nVideocombo == 2) //--------------- MegaPixel --------------// 
	{
		  if((oSysInfo->lan_config.nVideocodecres == 0) || (oSysInfo->lan_config.nVideocodecres == 1))	// JPG:1600X1200
		  {
			//return 1600;	//
			  return 1200;
		  }
		  else if((oSysInfo->lan_config.nVideocodecres == 2) || (oSysInfo->lan_config.nVideocodecres == 3))// JPG:2048X1536
		  {
			//return 2048;	//
			  return 1536;
		  }
		  else if((oSysInfo->lan_config.nVideocodecres == 4) || (oSysInfo->lan_config.nVideocodecres == 5))// JPG:2048X1536
		  {
			//return 2592;	//
			  return 1920;
		  }
     }
#endif
     	return 0;
}



int oget_dhcpstatus(void)
{
	//SysInfo* oSysInfo = GetSysInfo();
	//return oSysInfo->lan_config.net.dhcp_enable;
	return 0;
}


/***************************************************************************/
/****                                SET                                ****/
/***************************************************************************/
int oset_encoding(int width,int height, int encode)
{
	return 0;
}

int oset_bitrate(int bitrate)
{
	//SysInfo* oSysInfo = GetSysInfo();
	//ControlSystemData(SFIELD_SET_MPEG41_BITRATE, (void *)&bitrate, sizeof(bitrate));
	return 0;
}

int oset_framerate(int encoding,int resolution,int framerate)
{
#if 0
	__u8 _Framerate; 
	SysInfo* oSysInfo = GetSysInfo();
	if((encoding==0) || (encoding==1)) // H264 || MPEG4
	{
		if((resolution==0) || (resolution==1) || (resolution==2)) 
		{
			if(framerate==30)  _Framerate = 0;
			else if(framerate==24) _Framerate = 1;
			else if(framerate==15) _Framerate = 2;
			else _Framerate = 3;//if(framerate==8)
		}
		else if(resolution==3)
		{
			if(framerate==23) _Framerate = 0;
			else if(framerate==15) _Framerate = 1;
			else _Framerate = 2; //if(framerate==8) 
		}
		else _Framerate = 0;
	}
	else if(encoding==2) // MegaPixel
	{
		if((resolution==0) || (resolution==2) || (resolution==4)) 
		{
			if(framerate==30)  _Framerate = 0;
			else if(framerate==24) _Framerate = 1;
			else if(framerate==15) _Framerate = 2;
			else _Framerate = 3;//if(framerate==8) 
		}
		else if((resolution==1) || (resolution==3)) 
		{
			if(framerate==15)  _Framerate = 0;
			else _Framerate = 1; //if(framerate==8) 
		}
		else if(resolution==5) _Framerate = 0;
	}
	if(oSysInfo->lan_config.nFrameRate1 != _Framerate)
	ControlSystemData(SFIELD_SET_FRAMERATE1, (void *)&_Framerate, sizeof(_Framerate));
#endif	
	return 0;
}

int oset_ratecontrol(int ratecontrol)
{
	//SysInfo* oSysInfo = GetSysInfo();
	return 0;
}

unsigned char  oset_resolution(unsigned char encoding,int width, int height)
{
#if 0
	SysInfo* oSysInfo = GetSysInfo();
	//onvif {JPEG = 0, MPEG4 = 1, H264 = 2};
	//ipnc  {H264 = 0, MPEG4 = 1, MegaPixel = 2};		
									   // Onvif -> IPNC
		 if(encoding==0) encoding = 2; // jpeg  -> Megapixel
	else if(encoding==1) encoding = 1; // MPEG4 -> MPEG4
	else if(encoding==2) encoding = 0; // H264  -> H264

	if(encoding == 0) // H264
	{
	}
	else if(encoding==1) // MPEG4
	{
	}
	else if(encoding==2) // MegaPixel
	{
	}

#endif
	return 0;
}

int oset_dhcpstatus(int status)
{
	return 1;
}

int oset_timezone(char *TZ)
{
	if (strncmp(TZ, "IDLW", 4)==0)return 0;//-12
	else if (strncmp(TZ, "NT", 2)==0)return 1;//-11
	else if ((strncmp(TZ, "AHST", 4)==0)||(strncmp(TZ, "CAT", 3)==0)||(strncmp(TZ, "HST", 3)==0)||(strncmp(TZ, "HDT", 3)==0))return 2;//-10
	else if ((strncmp(TZ, "YST", 3)==0)||(strncmp(TZ, "YDT", 3)==0))return 3;//-9
	else if ((strncmp(TZ, "PST", 3)==0)||(strncmp(TZ, "PDT", 3)==0))return 4;//-8
	else if ((strncmp(TZ, "MST", 3)==0)||(strncmp(TZ, "MDT", 3)==0))return 5;//-7
	else if ((strncmp(TZ, "CST", 3)==0)||(strncmp(TZ, "CDT", 3)==0))return 6;//-6
	else if ((strncmp(TZ, "EST", 3)==0)||(strncmp(TZ, "EDT", 3)==0))return 7;//-5
	else if ((strncmp(TZ, "AST", 3)==0)||(strncmp(TZ, "ADT", 3)==0))return 8;//-4
	else if (strncmp(TZ, "GMT-03", 6)==0)return 9;//-3
	else if (strncmp(TZ, "AT", 2)==0)return 10;//-2
	else if (strncmp(TZ, "WAT", 3)==0)return 11;//-1
	else if ((strncmp(TZ, "GMT", 3)==0)||(strncmp(TZ, "UT", 2)==0)||(strncmp(TZ, "UTC", 3)==0)||(strncmp(TZ, "BST", 3)==0))return 12;//-0
	else if ((strncmp(TZ, "CET", 3)==0)||(strncmp(TZ, "FWT", 3)==0)||(strncmp(TZ, "MET", 3)==0)\
		||(strncmp(TZ, "MEWT", 4)==0)||(strncmp(TZ, "SWT", 3)==0)||(strncmp(TZ, "MEST", 4)==0)\
		||(strncmp(TZ, "MESZ", 4)==0)||(strncmp(TZ, "SST", 3)==0)||(strncmp(TZ, "FST", 3)==0))return 13;//1
	else if (strncmp(TZ, "EET", 3)==0)return 14;//2
	else if (strncmp(TZ, "BT", 2)==0)return 15;//3
	else if (strncmp(TZ, "ZP4", 3)==0)return 16;//4
	else if (strncmp(TZ, "ZP5", 3)==0)return 17;//5
	else if (strncmp(TZ, "ZP6", 3)==0)return 18;//6
	else if (strncmp(TZ, "ZP7", 3)==0)return 19;//7
	else if (strncmp(TZ, "WAST", 4)==0)return 20;//8
	else if (strncmp(TZ, "JST", 3)==0)return 21;//9
	else if (strncmp(TZ, "ACT", 3)==0)return 22;//10
	else if (strncmp(TZ, "EAST", 4)==0)return 23;//11
	else if (strncmp(TZ, "IDLE", 4)==0)return 24;//12
	else return 100;//ERROR
	
	
}

int onvif_fault(struct soap *soap,char *value1,char *value2)
{
	soap->fault = (struct SOAP_ENV__Fault*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Fault)));
	if(soap->fault == NULL)
	{
		printf("Failed to malloc for fault.\n");
		return SOAP_FAULT;
	}
	soap->fault->SOAP_ENV__Code = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));
	if(soap->fault->SOAP_ENV__Code == NULL)
	{
		printf("Failed to malloc for SOAP_ENV__Code.\n");
		return SOAP_FAULT;
	}
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Value = "SOAP-ENV:Sender";
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));;
	if(soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode == NULL)
	{
		printf("Failed to malloc for SOAP_ENV__Subcode.\n");
		return SOAP_FAULT;
	}
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Value = value1;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode = (struct SOAP_ENV__Code*)soap_malloc(soap,(sizeof(struct SOAP_ENV__Code)));
	if(soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode == NULL)
	{
		printf("Failed to malloc for SOAP_ENV__Code.\n");
		return SOAP_FAULT;
	}
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode->SOAP_ENV__Value = value2;
	soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Subcode->SOAP_ENV__Subcode = NULL;
	soap->fault->faultcode = NULL;
	soap->fault->faultstring = NULL;
	soap->fault->faultactor = NULL;
	soap->fault->detail = NULL;
	soap->fault->SOAP_ENV__Reason = NULL;
	soap->fault->SOAP_ENV__Node = NULL;
	soap->fault->SOAP_ENV__Role = NULL;
	soap->fault->SOAP_ENV__Detail = NULL;
	return SOAP_OK;
}

int checkhostname(char *hostname)
{
	while(*hostname != '\0')
	{
		if(*hostname=='_')return 1;
		hostname++;
	}
	return 0; //No error
}

/* @brief Check if IP is valid */
int isValidIp4 (char *str) 
{
	int segs = 0;   /* Segment count. */     
	int chcnt = 0;  /* Character count within segment. */     
	int accum = 0;  /* Accumulator for segment. */      
	/* Catch NULL pointer. */      
	if (str == NULL) return 0;      
	/* Process every character in string. */      
	while (*str != '\0') 
	{         
		/* Segment changeover. */          
		if (*str == '.') 
		{             
			/* Must have some digits in segment. */              
			if (chcnt == 0) return 0;              
			/* Limit number of segments. */              
			if (++segs == 4) return 0;              
			/* Reset segment values and restart loop. */              
			chcnt = accum = 0;             
			str++;             
			continue;         
		}  

		/* Check numeric. */          
		if ((*str < '0') || (*str > '9')) return 0;
		/* Accumulate and check segment. */          
		if ((accum = accum * 10 + *str - '0') > 255) return 0;
		/* Advance other segment specific stuff and continue loop. */          
		chcnt++;         
		str++;     
	}      
	/* Check enough segments and enough characters in last segment. */      
	if (segs != 3) return 0;      
	if (chcnt == 0) return 0;      
	/* Address okay. */      
	return 1; 
} 

/* @brief Check if a hostname is valid */
int isValidHostname (char *str) 
{
	/* Catch NULL pointer. */      
	if (str == NULL) 
	{
		return 0;      
	}
	/* Process every character in string. */      
	while (*str != '\0') 
	{         
		if ((*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z') || (*str >= '0' && *str <= '9') || (*str == '.') || (*str == '-') )
		{
			str++;
		}
		else
		{
			return 0;
		}
	}
	return 1; 
}

