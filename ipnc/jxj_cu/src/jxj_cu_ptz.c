#include "jxj_cu_logon.h"
#include "jxj_cu_ptz.h"

int jxj_cu_ptz_set(long logonHandle, JPTZControl *cfg)
{
	int len = sizeof(JPTZControl);
	if(0 > jxj_cu_cfg_set(logonHandle, PARAM_CONTROL_PTZ,
						  cfg, len))
		return -1;
	return 0;
}

/* ptzøÿ÷∆ */
int jxj_cu_client_crtl_ptz(long logonHandle,  JPTZAction action, uint8_t u8Speed)
{
	JPTZControl ptz;

	memset(&ptz, 0, sizeof(JPTZControl));	
	ptz.action = action;
	ptz.param  = u8Speed;

	return jxj_cu_ptz_set(logonHandle, &ptz);

}
/* …Ë÷√◊‘∂Ø—≤∫Ω */
int jxj_cu_client_ptz_cruise(long logonHandle, uint8_t u8CruisePresetNo, uint8_t u8CruisePresetEnable, 
							 uint8_t u8CruisePresetSec, uint8_t u8CruisePresetSpeed)
{
	JPTZControl ptz;
	JCruisePoint stCruisePoint;
	
	memset(&ptz, 0, sizeof(JPTZControl));	
	memset(&stCruisePoint, 0, sizeof(JCruisePoint));

	if(!!u8CruisePresetEnable)
		ptz.action = PTZ_SET_CRZ;//…Ë÷√—≤∫Ω
	else
		ptz.action = PTZ_DEL_CRZ;//…æ≥˝—≤∫Ω

	stCruisePoint.preset = u8CruisePresetNo;
	stCruisePoint.speed = u8CruisePresetSpeed;
	stCruisePoint.dwell = u8CruisePresetSec;

	memcpy(&ptz.param, &stCruisePoint, sizeof(ptz.param));
	
	return jxj_cu_ptz_set(logonHandle, &ptz);
	
}


