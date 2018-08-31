
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gpio.h>

static void print_usage(const char *proc_name)
{
	printf("%s usage:\n", proc_name);
	printf("\tget gpio value %s g gpio_no\n", proc_name);
	printf("\tset gpio value %s s gpio_no [0|1]\n", proc_name);
}

int main(int argc, char *argv[])
{
	int gpio_no;
	int gpio_handle;
	if(argc < 2)
	{
		print_usage(argv[0]);
		return -1;
	}
	if(!((argv[1][0] == 's' && argc >= 4) || (argv[1][0] == 'g' && argc >= 3)))
	{
		print_usage(argv[0]);
		return -1;
	}
	gpio_no = atoi(argv[2]);
	gpio_handle = gpio_open(gpio_no);
	if(gpio_handle == 0)
	{
		printf("open gpio failed\n");
		return -1;
	}
	if(argv[1][0] == 'g')
	{
		int value;
		if(gpio_get_value(gpio_handle, &value) < 0)
		{
			printf("Get value failed.\n");
			gpio_close(gpio_handle);
			return -1;
		}
		printf("gpio%d value: %d\n", gpio_no, value);
	}
	else if(argv[1][0] == 's')
	{
		int value;
		value = atoi(argv[3]);
		if(gpio_set_value(gpio_handle, value) < 0)
		{
			printf("Set value failed.\n");
			gpio_close(gpio_handle);
			return -1;
		}
		printf("Set gpio%d to %d successfully.\n", gpio_no, value);
	}
	gpio_close(gpio_handle);
	
	return 0;
}




