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

CFLAGS := $(CFLAGS_EXTRA) -I./inc -I../driver
LDFLAGS := $(LDFLAGS_EXTRA)

OUTPUT	:= libavpool.so 

all:main
main:$(Obj)
	$(CC) $(LDFLAGS) -shared -o $(OUTPUT) $^
$(ObjDir)%.o:$(SrcDir)%.c
	$(CC) -c -fPIC  $< -o $@ $(CFLAGS)
.depend:$(Src)
	@$(CC) $(CFLAGS) -MM $(Src) > $@
sinclude .depend

install:
	install $(OUTPUT) $(PRI_LIB_DIR)
	ln -sf $(CURDIR)/inc/av_pool.h $(PRI_INC_DIR)/av_pool.h

clean:
	-rm -f $(OUTPUT)
	rm -f $(ObjDir)*.o
	-rm -f .depend

.PHONY: clean install

