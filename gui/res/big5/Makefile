#include config.linux
#
#SUBDIRS=bmp cursor icon font imetab

all:	nothing

depend .depend:	nothing

clean:	nothing

nothing:
	@echo Please run \"make install\" as root to install.

install:
	rm -rf ../minigui-res-1.6.10/font/*big5*.bin
	rm -rf ../minigui-res-1.6.10/font/*gb2312*.bin
	cp font/*big5*.bin ../minigui-res-1.6.10/font/
	cp font/song-*-gbk.bin ../minigui-res-1.6.10/font/
	cp language/* ../nvr_res/language/
	cp MiniGUI.cfg ../
#	for i in $(SUBDIRS);\
#	do $(INSTALL_DIR) $(resdir)/res/$$i;\
#	$(INSTALL_DATA) $$i/*.* $(resdir)/res/$$i;\
#	done

#uninstall:
#	rm -rf $(resdir)/res

