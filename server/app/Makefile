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

CFLAGS := $(CFLAGS_EXTRA) `$(PKG_CONFIG) --cflags glib-2.0` -DNVR_SOFT_VERSION=\"$(VERSION)\" -I./inc -I../lib/inc
LDFLAGS := $(LDFLAGS_EXTRA) `$(PKG_CONFIG) --libs glib-2.0` -lnet -lupnp -lsyscfg -lrt -lavpool \
			-lprotocolclient -lstorage -lpthread -lgpio -lupdate -llog -ldhcp


OUTPUT	:= server

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
	rm -f $(OUTPUT)
	rm -f $(ObjDir)*.o
	rm -f .depend

install:
	install $(OUTPUT) $(TARGET_APP_DIR)
