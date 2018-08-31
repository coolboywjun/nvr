CURDIR = $(shell pwd)
ROOTDIR = ..
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

SrcDir = ./
ObjDir = obj/
Src = $(wildcard $(SrcDir)*.c)
SrcFile = $(notdir $(Src))
ObjFile = $(patsubst %.c,%.o,$(SrcFile))
Obj = $(addprefix $(ObjDir),$(ObjFile))
CC := $(CROSS_PREFIX)gcc
AR := $(CROSS_PREFIX)ar
LANGUAGE_RES :=

ifeq ($(CHARSET), GB2312)
LANGUAGE_RES := gb2312
endif

ifeq ($(CHARSET), BIG5)
LANGUAGE_RES := big5
endif

CFLAGS := $(CFLAGS_EXTRA) -DGUI_VERSION=\"$(GUI_VERSION)\" -I./include -I./private/inc
LDFLAGS := $(LDFLAGS_EXTRA) -lrt -lminigui -lz -lpng -lqrencode -ljpeg -lpthread -lmgext -lvcongui -lnet -lstorage -llog -lgpio  -lplayer -lserver -lupdate -lavpool -lprotocolclient\
			   -lstorage_ext -lnet -lavformat -lavcodec -lavutil -lsyscfg 

OUTPUT	:= appui

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
	echo $(LANGUAGE_RES)
	make -C res/$(LANGUAGE_RES) install
	install $(OUTPUT) $(TARGET_APP_DIR)
#	cp $(PWD)/lib/lib* $(TARGET_LIB_DIR)
	make -C res/minigui-res-1.6.10 install
	cp res/MiniGUI.cfg $(TARGET_DIR)/etc
	rm -rf $(TARGET_APP_DIR)/nvr_res
	cp -rf res/nvr_res $(TARGET_APP_DIR)
