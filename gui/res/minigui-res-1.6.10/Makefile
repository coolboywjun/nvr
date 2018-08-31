include config.linux

SUBDIRS=bmp cursor icon font imetab

all:	nothing

depend .depend:	nothing

clean:	nothing

nothing:
	@echo Please run \"make install\" as root to install.

install:
	rm -rf $(resdir)/res/font
	rm -rf $(resdir)/res/imetab
	for i in $(SUBDIRS);\
	do $(INSTALL_DIR) $(resdir)/res/$$i;\
	$(INSTALL_DATA) $$i/*.* $(resdir)/res/$$i;\
	done

uninstall:
	rm -rf $(resdir)/res

