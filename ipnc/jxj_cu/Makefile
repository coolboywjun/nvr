CURDIR = $(shell pwd)
ROOTDIR = ../..
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

SrcDir = src/
ObjDir = obj/
Src = $(wildcard $(SrcDir)*.c)
SrcFile = $(notdir $(Src))
ObjFile = $(patsubst %.c,%.o,$(SrcFile))
Obj = $(addprefix $(ObjDir),$(ObjFile))
CC := $(CROSS_PREFIX)gcc
AR := $(CROSS_PREFIX)ar

ifeq ($(ARCH), x86)
JxjPlatformSo := jdevsdk_x86
else
JxjPlatformSo := jdevsdk
endif

CFLAGS := $(CFLAGS_EXTRA) `$(PKG_CONFIG) --cflags libxml-2.0` -I./inc -I../lib/inc -I../lib/server/inc
LDFLAGS := $(LDFLAGS_EXTRA) -lrt  -lpthread $(LOCAL_LIB) `$(PKG_CONFIG) --libs libxml-2.0` -lnet -lavpool -l$(JxjPlatformSo) -L../lib_cu/server/ -lprotocolserver -lrtspclient


OUTPUT	:= jxj_cu

all:main

main:$(Obj)
	$(CC) -o $(OUTPUT) $^ $(LDFLAGS)

.depend:$(Src)
	@$(CC) $(CFLAGS) -MM $(Src) > $@
sinclude .depend

$(ObjDir)%.o:$(SrcDir)%.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean install

clean:
	-rm -f $(OUTPUT)
	rm -f $(ObjDir)*.o
	rm -f .depend

install:
	install $(OUTPUT) $(TARGET_APP_DIR)
