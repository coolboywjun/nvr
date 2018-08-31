CURDIR = $(shell pwd)
ROOTDIR = ../..
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

SrcDir = ./
ObjDir = ./
Src = $(wildcard $(SrcDir)*.c)
SrcFile = $(notdir $(Src))
ObjFile = $(patsubst %.c,%.o,$(SrcFile))
Obj = $(addprefix $(ObjDir),$(ObjFile))
CC := $(CROSS_PREFIX)gcc
AR := $(CROSS_PREFIX)ar

CFLAGS := $(CFLAGS_EXTRA) `$(PKG_CONFIG) --cflags glib-2.0`-I$(DECODER_DIR)/player/inc -I./ -I../osa/inc -I$(APP_DIR)/av_pool/lib/inc -I$(APP_DIR)/storage/inc -fPIC
LDFLAGS := $(LDFLAGS_EXTRA) -shared -fPIC -lstorage -L$(APP_DIR)/private/libs



OUTPUT	:= libplayer.so

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
	-rm -f .depend

install:
	install $(OUTPUT) $(CURDIR)/../test_player
	install $(OUTPUT) $(TARGET_LIB_DIR) 
	install $(OUTPUT) $(PRI_LIB_DIR) 
	ln -sf $(CURDIR)/player_interface.h $(PRI_INC_DIR)/player_interface.h