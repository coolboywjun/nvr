
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct
{
	int gpio;
	int status;	// -1: δ֪, 0: ouput, 1: input
	int is_my_export;
}GPIO_STRUCT;

#define GPIO_EXPORT_PATH	"/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH	"/sys/class/gpio/unexport"



// direction: 0, output; 1, input
int gpio_set_direction(long handle, int direction)
{
	int fd;
	char path[128];
	GPIO_STRUCT *gpio = (GPIO_STRUCT *)handle;

	if(gpio == NULL)
		return -1;
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", gpio->gpio);
	fd = open(path, O_WRONLY);
	if(fd < 0)
	{
		printf("Open %s file failed\n", path);
		return -1;
	}
	if(direction == 0)
		strcpy(path, "out");
	else
		strcpy(path, "in");
	if(write(fd, path, strlen(path)+1) < 0)
	{
		printf("Write gpio direction failed\n");
		close(fd);
		return -1;
	}
	close(fd);
	
	gpio->status = !!direction;

	return 0;
}


int gpio_set_value(long handle, int value)
{
	int fd;
	char buf[32];
	char path[128];
	GPIO_STRUCT *gpio = (GPIO_STRUCT *)handle;

	if(gpio == NULL)
		return -1;
	if(gpio->status != 0)
	{
		if(gpio_set_direction(handle, 0) < 0)
		{
			printf("Set direction to out failed\n");
			return -1;
		}
	}

	snprintf(buf, sizeof(buf), "%d", !!value);
	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio->gpio);
	fd = open(path, O_WRONLY);
	if(fd < 0)
	{
		printf("Open %s file failed\n", path);
		return -1;
	}
	if(write(fd, buf, strlen(buf)+1) < 0)
	{
		printf("Write to gpio value failed\n");
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}


int gpio_get_value(long handle, int *value)
{
	int fd;
	char buf[32];
	char path[128];
	GPIO_STRUCT *gpio = (GPIO_STRUCT *)handle;
	
	if(value == NULL)
		return -1;
	if(gpio->status != 1)
	{
		if(gpio_set_direction(handle, 1) < 0)
		{
			printf("Set direction to out failed\n");
			return -1;
		}
	}

	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio->gpio);
	fd = open(path, O_RDONLY);
	if(fd < 0)
	{
		printf("Open %s file failed\n", path);
		return -1;
	}
	if(read(fd, buf, sizeof(buf)) < 0)
	{
		printf("Read to gpio value failed\n");
		close(fd);
		return -1;
	}
	close(fd);

	*value = atoi(buf);
	
	return 0;
}

long gpio_open(int gpio_no)
{
	int fd;
	char path[128];
	char buf[32];
	GPIO_STRUCT *gpio;
	int my_export = 0;

	snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", gpio_no);
	if(access(path, F_OK) != 0)
	{
		fd = open(GPIO_EXPORT_PATH, O_WRONLY);
		if(fd < 0)
		{
			printf("Open %s failed\n", GPIO_EXPORT_PATH);
			return 0;
		}
		snprintf(buf, sizeof(buf), "%d", gpio_no);
		if(write(fd, buf, strlen(buf)+1) < 0)
		{
			printf("Write to gpio export failed\n");
			close(fd);
			return 0;
		}
		close(fd);
		my_export = 1;
	}
	gpio = malloc(sizeof(GPIO_STRUCT));
	gpio->gpio = gpio_no;
	gpio->status = -1;
	gpio->is_my_export = my_export;
	
	return (long)gpio;
}

void gpio_close(long handle)
{
	int fd;
	char buf[32];
	GPIO_STRUCT *gpio = (GPIO_STRUCT *)handle;
	
	if(gpio == NULL)
		return;
	if(gpio->is_my_export)
	{
		fd = open(GPIO_UNEXPORT_PATH, O_WRONLY);
		snprintf(buf, sizeof(buf), "%d", gpio->gpio);
		write(fd, buf, strlen(buf)+1);
		close(fd);
	}
	free(gpio);
}

