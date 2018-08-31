CURDIR = $(shell pwd)
ROOTDIR = ..
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

WEBFLAG = -DWEBS -DUEMF -DOS="LINUX" -DLINUX -DUSER_MANAGEMENT_SUPPORT -DDIGEST_ACCESS_SUPPORT

CFLAGS := $(CFLAGS_EXTRA) $(WEBFLAG) `$(PKG_CONFIG) --cflags libxml-2.0` -I../../nvr/server/lib/inc/ -I./src/include/
LDFLAGS := $(LDFLAGS_EXTRA) `$(PKG_CONFIG) --libs libxml-2.0` -lpthread -lstorage -llog -lplayer -lupdate -lavpool -lrt -lpthread -lnet -lserver -lsyscfg libonvif.a


OUTPUT	:= webs

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
