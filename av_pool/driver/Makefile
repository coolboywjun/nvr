
ifneq ($(KERNELRELEASE),)

av_pool-objs := av_pool_module.o av_pool_mng.o mem_mng.o
obj-m := av_pool.o

else

CURDIR = $(shell pwd)
ROOTDIR = ../..
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

INCLUDES = -I.. -I../include
C_FLAGS = -fPIC

#CROSS_PREFIX = arm-none-linux-gnueabi-
CFLAGS += $(C_FLAGS) $(CFLAGS_EXTRA)

KERNELDIR = $(KERNEL_DIR)
DRIVER = av_pool.ko

all: $(DRIVER)

$(DRIVER): av_pool_module.c av_pool_module.h av_pool_mng.c av_pool_mng.h mem_mng.c mem_mng.h
ifeq ($(ARCH), arm)
	$(MAKE) -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_PREFIX) SUBDIRS=$(CURDIR) modules
else
	$(MAKE) -C $(KERNELDIR) SUBDIRS=$(CURDIR) modules
endif

install:
	install $(DRIVER) $(TARGET_MODULE_DIR)/

clean: 
	make -C $(KERNELDIR) ARCH=arm CROSS_PREFIX=$(CROSS_PREFIX) SUBDIRS=$(CURDIR) clean 

endif

