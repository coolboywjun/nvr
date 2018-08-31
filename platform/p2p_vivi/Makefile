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


CFLAGS := $(CFLAGS_EXTRA) -I./inc 
LDFLAGS := $(LDFLAGS_EXTRA) -L./lib -lm -lrt -lpthread -lnet -lserver -lt2usvr

P2P_VIVI_BIN := T2uSvr

OUTPUT	:= p2p_vivi_platform

all:main

main:$(Obj)
	$(CC) -o $(OUTPUT) $^ $(LDFLAGS)

.depend:$(Src)
	@$(CC) $(CFLAGS) -MM $(Src) > $@
sinclude .depend

$(ObjDir)%.o:$(SrcDir)%.c
	echo "src=$(Src)"
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean install

clean:
	rm -f $(OUTPUT)
	rm -f $(ObjDir)*.o
	rm -f .depend

install:
	chmod 755 $(P2P_VIVI_BIN)
	install $(P2P_VIVI_BIN) $(TARGET_APP_DIR)
	install $(OUTPUT) $(TARGET_APP_DIR)
