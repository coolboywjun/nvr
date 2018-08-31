
#include <strg_header.h>

// storage读写闪烁灯使用的是pb3, pb3对应的gpio no为27
#define STORAGE_LED_NO		27
#define GPIO_EXPORT	"/sys/class/gpio/export"
#define GPIO_UNEXPORT	"/sys/class/gpio/unexport"

