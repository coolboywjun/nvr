CURDIR = $(shell pwd)
ROOTDIR = ../../..
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

CFLAGS := $(CFLAGS_EXTRA) -I$(PRI_INC_DIR) -I../inc -I./inc
LDFLAGS := -L$(PRI_LIB_DIR) -lnet
OUTPUT	:= libprotocolclient.so

all:main
main:$(Obj)
	$(CC) -shared -o $(OUTPUT) $^ $(LDFLAGS)

.depend:$(Src)
	@$(CC) $(CFLAGS) -MM $(Src) > $@
sinclude .depend

$(ObjDir)%.o:$(SrcDir)%.c
	$(CC) -c $(CFLAGS) -fPIC  $< -o $@

.PHONY: clean install
clean:
	rm -f $(OUTPUT)
	rm -f $(ObjDir)*.o
	rm -f .depend

install:
	install $(OUTPUT) $(PRI_LIB_DIR)
	ln -sf $(CURDIR)/../inc/ipnc_lib.h $(PRI_INC_DIR)/ipnc_lib.h
	ln -sf $(CURDIR)/inc/ipnc_lib_client.h $(PRI_INC_DIR)/ipnc_lib_client.h
