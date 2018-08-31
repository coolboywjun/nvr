CURDIR = $(shell pwd)
ROOTDIR = ../..
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

SrcDir = src/
ObjDir = obj/
LibDir = lib/
Src = $(wildcard $(SrcDir)*.c)
SrcFile = $(notdir $(Src))
ObjFile = $(patsubst %.c,%.o,$(SrcFile))
Obj = $(addprefix $(ObjDir),$(ObjFile))
CC := $(CROSS_PREFIX)gcc
AR := $(CROSS_PREFIX)ar

CFLAGS := $(CFLAGS_EXTRA) `$(PKG_CONFIG) --cflags glib-2.0` -I$(DECODER_DIR)/player/inc -I../osa/inc -I../player_interface -I$(DECODER_DIR)/player/inc/include -I$(KERNEL_DIR)/include -I$(APP_DIR)/private/inc
LDFLAGS := $(LDFLAGS_EXTRA) `$(PKG_CONFIG) --libs glib-2.0 --libs libparted` -losa -lvdecoder -lvencoder -lvecore -lpthread -lavpool -lnet -lstorage -Llib -L$(APP_DIR)/private/libs -lcapture_play_audio


OUTPUT	:= player

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
	install $(OUTPUT) $(TARGET_APP_DIR)
	cp -ar $(LibDir)*.so* $(TARGET_LIB_DIR) 
