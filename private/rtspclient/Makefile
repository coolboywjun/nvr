CURDIR = $(shell pwd)
ROOTDIR = ../..
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

SrcDir = src/
ObjDir = obj/
IncludeDir = ./inc
Src = $(wildcard $(SrcDir)*.cpp)
SrcFile = $(notdir $(Src))
ObjFile = $(patsubst %.cpp,%.o,$(SrcFile))
Obj = $(addprefix $(ObjDir),$(ObjFile))
CC := $(CROSS_PREFIX)gcc
CPP := $(CROSS_PREFIX)g++
AR := $(CROSS_PREFIX)ar

CFLAGS := -fPIC $(CFLAGS_EXTRA) -I./inc -I/opt/a20/external-toolchain/arm-linux-gnueabi/libc/usr/include/liveMedia \
			-I/opt/a20/external-toolchain/arm-linux-gnueabi/libc/usr/include/BasicUsageEnvironment \
			-I/opt/a20/external-toolchain/arm-linux-gnueabi/libc/usr/include/groupsock \
			-I/opt/a20/external-toolchain/arm-linux-gnueabi/libc/usr/include/UsageEnvironment
#LDFLAGS := $(LDFLAGS_EXTRA) -lstdc++ -lpthread -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment
LDFLAGS := $(LDFLAGS_EXTRA) -lstdc++ -lpthread 


OUTPUT	:= librtspclient.so
#OUTPUT	:= rtspclient

all:main
main:$(Obj)
	$(CC) $(LDFLAGS) -shared -o $(OUTPUT) $^ ./lib/libliveMedia.a ./lib/libgroupsock.a ./lib/libBasicUsageEnvironment.a ./lib/libUsageEnvironment.a
#	$(CC) $(LDFLAGS) -o $(OUTPUT) $^ ./lib/libliveMedia.a ./lib/libgroupsock.a ./lib/libBasicUsageEnvironment.a ./lib/libUsageEnvironment.a



.depend:$(Src)
	@$(CC) $(CFLAGS) -MM $(Src) > $@
sinclude .depend

$(ObjDir)%.o:$(SrcDir)%.cpp
	$(CPP) $(CFLAGS) -c -I$(IncludeDir) -fPIC  $< -o $@

.PHONY: clean install
clean:
	-rm -f $(OUTPUT)
	rm -f $(ObjDir)*.o
	-rm -f .depend

install:
	install $(OUTPUT) $(PRI_LIB_DIR)
	ln -sf $(CURDIR)/inc/rtsp_client.h $(PRI_INC_DIR)/rtsp_client.h
