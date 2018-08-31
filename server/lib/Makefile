CURDIR = $(shell pwd)
ROOTDIR = ../..
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

SrcDir = src/
ObjDir = obj/
IncDir = inc/
Src = $(wildcard $(SrcDir)*.c)
SrcFile = $(notdir $(Src))
ObjFile = $(patsubst %.c,%.o,$(SrcFile))
Obj = $(addprefix $(ObjDir),$(ObjFile))
CC := $(CROSS_PREFIX)gcc
AR := $(CROSS_PREFIX)ar
ALL_HEADERS := $(notdir $(wildcard $(IncDir)*.h))

CFLAGS := $(CFLAGS_EXTRA) -I$(PRI_INC_DIR) -I./inc
LDFLAGS := $(LDFLAGS_EXTRA) -L$(PRI_LIB_DIR) -L./ -lnet
OUTPUT	:= libserver.so

all:main
main:$(Obj)
	$(CC) -shared -o $(OUTPUT) $^ $(LDFLAGS)
	$(CC) -o test_server_lib test/test_server_lib.c $(CFLAGS) $(LDFLAGS) -lpthread -lrt -lserver

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
#	$(foreach hdr,$(ALL_HEADERS),$(shell ln -sf $(CURDIR)/inc/$(hdr) $(PRI_INC_DIR)/$(hdr)))
	ln -sf $(CURDIR)/inc/nvr_struct.h $(PRI_INC_DIR)/nvr_struct.h
	ln -sf $(CURDIR)/inc/nvr_headers.h $(PRI_INC_DIR)/nvr_headers.h
	ln -sf $(CURDIR)/inc/net_struct.h $(PRI_INC_DIR)/net_struct.h
	ln -sf $(CURDIR)/inc/nvr_net_struct.h $(PRI_INC_DIR)/nvr_net_struct.h
	ln -sf $(CURDIR)/inc/nvr_net_interface.h $(PRI_INC_DIR)/nvr_net_interface.h
	ln -sf $(CURDIR)/inc/nvr_lib_interface.h $(PRI_INC_DIR)/nvr_lib_interface.h


test:
	$(CC) -o test_server_lib test/test_server_lib.c $(CFLAGS) $(LDFLAGS) -lpthread -lrt -lserver

test_install:
	install test_server_lib $(TARGET_APP_DIR)/

.PHONY: test test_install
