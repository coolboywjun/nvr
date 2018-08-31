#ifndef __NVR_FAN_H__
#define __NVR_FAN_H__

/*风扇控制
	gpio_val: 写入参数值.
	成功返回0, 失败返回-1.
*/
int nvr_fan_ctrl(int gpio_val);


#endif

