#ifndef __GPIO_H__
#define __GPIO_H__

// direction: 0, output; 1, input
int gpio_set_direction(long handle, int direction);

int gpio_set_value(long handle, int value);
	
int gpio_get_value(long handle, int *value);

long gpio_open(int gpio_no);

void gpio_close(long handle);

#endif


