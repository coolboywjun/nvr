CURDIR = $(shell pwd)
ROOTDIR = ../..
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

SrcDir = src/
ObjDir = obj/
Src = $(wildcard $(SrcDir)*.c) ../jxj/src/nvr_param_jxj.c ../jxj/src/nvr_vod_jxj.c ../jxj/src/nvr_character_conv.c
SrcFile = $(notdir $(Src))
ObjFile = $(patsubst %.c,%.o,$(SrcFile))
Obj = $(addprefix $(ObjDir),$(ObjFile))
CC := $(CROSS_PREFIX)gcc
AR := $(CROSS_PREFIX)ar

ifeq ($(ARCH), x86)
PlatformLdFlags := -L./libs -ljdevsdk_x86 -lJP2PDev_x86 -lJSocket_x86 -lIOTCAPIs_x86 -lRDTAPIs_x86
else
PlatformLdFlags := -L./libs -ljdevsdk -lJP2PDev -lJSocket -lIOTCAPIs -lRDTAPIs
endif


CFLAGS := $(CFLAGS_EXTRA) `$(PKG_CONFIG) --cflags glib-2.0` -I./inc -I../jxj/inc
LDFLAGS := $(LDFLAGS_EXTRA) `$(PKG_CONFIG) --libs glib-2.0` -lm -lpthread -lrt -lnet \
	-lavpool -lstorage -lserver -lplayer $(PlatformLdFlags) -lupdate


OUTPUT	:= p2p_tutk_platform

all:main

main:$(Obj)
	$(CC) -o $(OUTPUT) $^ $(LDFLAGS)

.depend:$(Src)
	@$(CC) $(CFLAGS) -MM $(Src) > $@
sinclude .depend

$(ObjDir)%.o:$(SrcDir)%.c
	echo "src=$(Src)"
	$(CC) -c $(CFLAGS) $< -o $@

$(ObjDir)nvr_param_jxj.o:../jxj/src/nvr_param_jxj.c
	$(CC) -c $(CFLAGS) $< -o $@

$(ObjDir)nvr_vod_jxj.o:../jxj/src/nvr_vod_jxj.c
	$(CC) -c $(CFLAGS) $< -o $@

$(ObjDir)nvr_character_conv.o:../jxj/src/nvr_character_conv.c
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean install

clean:
	rm -f $(OUTPUT)
	rm -f $(ObjDir)*.o
	rm -f .depend

install:
	install $(OUTPUT) $(TARGET_APP_DIR)
	install ./libs/*.so $(TARGET_LIB_DIR)
