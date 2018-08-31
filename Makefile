### Makefile --- nvr main makefile

## Author: feidouqwer@163.com
## Version: $Id: Makefile,v 0.0 2013/07/19 01:22:19 xq Exp $
## Keywords: 
## X-URL: 

CURDIR = $(shell pwd)
ROOTDIR = .
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

.PHONY: app install clean
app:
	make net
	make gpio
	make upnp
	make update
	make syscfg
	make audio
	make log
	make rtspclient
	make dhcp
	make av
	make ipnc
	make rtsp
	make storage
ifeq ($(ARCH), arm)
	make player
endif
	make server
	make platform
	make web
	make tools
ifeq ($(ARCH), arm)
	make gui
	make gui_install
endif
#	make test
	make install

install:
	cp $(PRI_LIB_DIR)/*.so* $(TARGET_LIB_DIR)


clean:
	make net_clean
	make gpio_clean
	make upnp_clean
	make update_clean
	make syscfg_clean
	make audio_clean
	make rtspclient_clean
	make log_clean
	make dhcp_clean
	make av_clean
	make ipnc_clean
	make rtsp_clean
	make storage_clean
	make player_clean
	make server_clean
	make platform_clean
	make web_clean
	make tools_clean
	make gui_clean
#	make test_clean
	rm -f $(PRI_INC_DIR)/*.h

# av
.PHONY:av_driver av_driver_install av_driver_clean av_lib av_lib_install av_lib_clean av_test av_test_install av_test_clean av av_clean
av_driver:
	make -C av_pool/driver/
av_driver_install:
	make install -C av_pool/driver/
av_driver_clean:
	make clean -C av_pool/driver/
av_lib:
	make -C av_pool/lib/
av_lib_install:
	make install -C av_pool/lib/
av_lib_clean:
	make clean -C av_pool/lib/
av_test:
	make -C av_pool/test/read/
#	make -C av_pool/test/write/
av_test_install:
	make install -C av_pool/test/read/
#	make install -C av_pool/test//write/
av_test_clean:
	make clean -C av_pool/test/read/
#	make clean -C av_pool/test/write/
av:
	make av_driver
	make av_driver_install
	make av_lib
	make av_lib_install
av_clean:
	make av_driver_clean
	make av_lib_clean

# net library
.PHONY:net_lib net_lib_install net_lib_clean net net_clean
net_lib:
	make -C private/net/
net_lib_install:
	make install -C private/net/
net_lib_clean:
	make clean -C private/net
net:
	make net_lib
	make net_lib_install
net_clean:
	make net_lib_clean

# gpio library
.PHONY:gpio_lib gpio_lib_install gpio_lib_clean gpio gpio_clean
gpio_lib:
	make -C private/gpio/
gpio_lib_install:
	make install -C private/gpio/
gpio_lib_clean:
	make clean -C private/gpio
gpio:
	make gpio_lib
	make gpio_lib_install
gpio_clean:
	make gpio_lib_clean

# upnp library
.PHONY:upnp_lib upnp_lib_install upnp_lib_clean upnp upnp_clean
upnp_lib:
	make -C private/upnp/
upnp_lib_install:
	make install -C private/upnp/
upnp_lib_clean:
	make clean -C private/upnp
upnp:
	make upnp_lib
	make upnp_lib_install
upnp_clean:
	make upnp_lib_clean

# update library
.PHONY:update_lib update_lib_install update_lib_clean update update_clean
update_lib:
	make -C private/update/
update_lib_install:
	make install -C private/update/
update_lib_clean:
	make clean -C private/update
update:
	make update_lib
	make update_lib_install
update_clean:
	make update_lib_clean

# syscfg library
.PHONY:syscfg_lib syscfg_lib_install syscfg_lib_clean syscfg syscfg_clean
syscfg_lib:
	make -C private/syscfg/
syscfg_lib_install:
	make install -C private/syscfg/
syscfg_lib_clean:
	make clean -C private/syscfg
syscfg:
	make syscfg_lib
	make syscfg_lib_install
syscfg_clean:
	make syscfg_lib_clean

# audio library
.PHONY:audio_lib audio_lib_install audio_lib_clean audio audio_clean
audio_lib:
	make -C private/audio/
audio_lib_install:
	make install -C private/audio/
audio_lib_clean:
	make clean -C private/audio
audio:
	make audio_lib
	make audio_lib_install
audio_clean:
	make audio_lib_clean

# log library
.PHONY:log_lib log_lib_install log_lib_clean log log_clean
log_lib:
	make -C private/log/
log_lib_install:
	make install -C private/log/
log_lib_clean:
	make clean -C private/log
log:
	make log_lib
	make log_lib_install
log_clean:
	make log_lib_clean

# rtspclient library
.PHONY:rtspclient_lib rtspclient_lib_install rtspclient_lib_clean rtspclient rtspclient_clean
rtspclient_lib:
	make -C private/rtspclient/
rtspclient_lib_install:
	make install -C private/rtspclient/
rtspclient_lib_clean:
	make clean -C private/rtspclient
rtspclient:
	make rtspclient_lib
	make rtspclient_lib_install
rtspclient_clean:
	make rtspclient_lib_clean

# dhcp library
.PHONY:dhcp_lib dhcp_lib_install dhcp_lib_clean dhcp dhcp_clean
dhcp_lib:
	make -C private/dhcp/
dhcp_lib_install:
	make install -C private/dhcp/
dhcp_lib_clean:
	make clean -C private/dhcp
dhcp:
	make dhcp_lib
	make dhcp_lib_install
dhcp_clean:
	make dhcp_lib_clean

# ipnc protocol
.PHONY:ipnc_lib_server ipnc_lib_server_install ipnc_lib_server_clean ipnc_lib_client ipnc_lib_client_install ipnc_lib_cliean_clean ipnc_app ipnc_app_clean ipnc ipnc_clean
ipnc_lib_server:
	make -C ipnc/lib/server/
ipnc_lib_server_install:
	make install -C ipnc/lib/server/
ipnc_lib_server_clean:
	make clean -C ipnc/lib/server/
ipnc_lib_client:
	make -C ipnc/lib/client/
ipnc_lib_client_install:
	make install -C ipnc/lib/client/
ipnc_lib_client_clean:
	make clean -C ipnc/lib/client/
ipnc_app:
	make -C ipnc/jxj_ti/
	make -C ipnc/jxj_cu/
	make -C ipnc/onvif_cu/
ipnc_app_install:
	make install -C ipnc/jxj_ti/
	make install -C ipnc/jxj_cu/
	make install -C ipnc/onvif_cu/
ipnc_app_clean:
	make clean -C ipnc/jxj_ti/
	make clean -C ipnc/jxj_cu/
	make clean -C ipnc/onvif_cu/
ipnc:
	make ipnc_lib_server
	make ipnc_lib_server_install
	make ipnc_lib_client
	make ipnc_lib_client_install
	make ipnc_app
	make ipnc_app_install
ipnc_clean:
	make ipnc_lib_server_clean
	make ipnc_lib_client_clean
	make ipnc_app_clean

# rtsp
.PHONY: rtsp_server rtsp_server_install rtsp_server_clean rtsp_client rtsp_client_install rtsp_client_clean rtsp rtsp_clean
rtsp_server:
	make -C rtsp/server/
rtsp_server_install:
	make install -C rtsp/server/
rtsp_server_clean:
	make clean -C rtsp/server/
rtsp_client:
	make -C rtsp/client/
rtsp_client_install:
	make install -C rtsp/client/
rtsp_client_clean:
	make clean -C rtsp/client
rtsp:
	make rtsp_server
	make rtsp_server_install
#	make rtsp_client
#	make rtsp_client_install
rtsp_clean:
	make rtsp_server_clean
#	make rtsp_client_clean

# server
.PHONY: server_app server_app_install server_app_clean server_lib server_lib_install server_lib_clean server server_clean
server_app:
	make -C server/app/
server_app_install:
	make install -C server/app/
server_app_clean:
	make clean -C server/app/
server_lib:
	make -C server/lib/
server_lib_install:
	make install -C server/lib/
server_lib_clean:
	make clean -C server/lib/
server_test:
	make test -C server/lib/
server_test_install:
	make test_install -C server/lib/
server:
	make server_lib
	make server_lib_install
	make server_app
	make server_app_install
server_clean:
	make server_lib_clean
	make server_app_clean

# storage
.PHONY: storage_app storage_app_install storage_app_clean  storage_lib storage_lib_install storage_lib_clean \
	storage_ext_lib storage_ext_lib_install storage_ext_lib_clean storage storage_clean
storage_app:
	make -C storage/app/
storage_app_install:
	make install -C storage/app/
storage_app_clean:
	make clean -C storage/app/
storage_lib:
	make -C storage/lib/
storage_lib_install:
	make install -C storage/lib/
storage_lib_clean:
	make clean -C storage/lib
storage_ext_lib:
	make -C storage/libext/
storage_ext_lib_install:
	make install -C storage/libext/
storage_ext_lib_clean:
	make clean -C storage/libext/
storage_test:
	make -C storage/test/
storage_test_install:
	make install -C storage/test/
storage_test_clean:
	make clean -C storage/test/
storage_ext_test:
	make -C storage/test_ext/
storage_ext_test_install:
	make install -C storage/test_ext/
storage_ext_test_clean:
	make clean -C storage/test_ext/
storage:
	make storage_app
	make storage_app_install
	make storage_lib
	make storage_lib_install
	make storage_ext_lib
	make storage_ext_lib_install
storage_clean:
	make storage_app_clean
	make storage_lib_clean
	make storage_ext_lib_clean

# Web server
.PHONY: web web_clean
web:
	make -C web_server/
	make install -C web_server/
web_clean:
	make clean -C web_server/

.PHONY:	test test_clean
test:
	make av_test
	make av_test_install
	make storage_test
	make storage_test_install
	make storage_ext_test
	make storage_test_install
	make server_test
	make server_test_install

test_clean:
	make av_test_clean
	make storage_test_clean	

#player
.PHONY: player player_clean
player_osa:
	make -C decoder/osa
player_osa_install:
	make install -C decoder/osa
player_osa_clean:
	make clean -C decoder/osa
player_app:
	make -C decoder/player
player_app_clean:
	make clean -C decoder/player
player_app_install:
	make install -C decoder/player
player_test:
	make -C decoder/test_player
player_test_clean:
	make clean -C decoder/test_player
player_test_install:
	make install -C decoder/test_player
player_interface:
	make -C decoder/player_interface
player_interface_clean:
	make clean -C decoder/player_interface
player_interface_install:
	make install -C decoder/player_interface
player:
	make player_interface
	make player_interface_install
	make player_osa
	make player_osa_install
	make player_app
	make player_app_install
	make player_test
	make player_test_install
player_clean:
	make player_osa_clean
	make player_app_clean
	make player_interface_clean
	make player_test_clean


.PHONY: platform_app platform_app_clean platform_app_install platform platform_clean
platform_app:
	make -C platform
platform_app_clean:
	make clean -C platform
platform_app_install:
	make install -C platform
platform:
	make platform_app
	make platform_app_install
platform_clean:
	make platform_app_clean


# make sdk
.PHONY: sdk
sdk:
	make sun7ismp_jxj_defconfig -C $(KERNEL_DIR) ARCH=arm
	cd $(SDK_DIR) && ./build.sh -p sun7i

# make tools
.PHONY: tools tools_clean
tools:
	make -C tools/update
	make install -C tools/update
	make -C tools/JPingServer
	make install -C tools/JPingServer
	make -C tools/wdt
	make install -C tools/wdt
	make -C tools/showsyscfg
	make install -C tools/showsyscfg
tools_clean:
	make clean -C tools/update
	make clean -C tools/JPingServer
	make clean -C tools/wdt
	make clean -C tools/showsyscfg

# make gui
.PHONY:gui gui_clean
gui:
	make -C gui
gui_install:
	make install -C gui
gui_clean:
	make clean -C gui

# make squashfs filesystem
.PHONY: pack logo image import
pack:
	chmod 755 tools/mksquashfs
	chmod 755 tools/unsquashfs
	mkdir -p $(OUT_DIR)
	rm -rf $(TARGET_DIR)/var/www
	unzip tools/$(WEB_FILE) -d $(TARGET_DIR)/var/
	(cd $(TARGET_DIR)/var; mv `find . -type d -name web*` www)
	find $(TARGET_DIR)/ -name "*~" | xargs rm -rf
	find $(TARGET_DIR)/ -name ".svn" | xargs rm -rf
	find $(TARGET_DIR)/ -name ".nfs*" | xargs rm -rf
	find $(TARGET_DIR)/ -name "core-*" | xargs rm -rf
	rm -rf $(TARGET_DIR)/opt/nvr/*_tmp
	rm -rf $(TARGET_DIR)/root/*_history
	echo "version: $(VERSION)" > $(TARGET_APP_DIR)/version
	echo "gui version: $(GUI_VERSION)" >> $(TARGET_APP_DIR)/version
	echo "build time: `date --rfc-3339=seconds`" >> $(TARGET_APP_DIR)/version
	tools/mksquashfs $(TARGET_DIR) $(OUT_DIR)/nvr_target.squashfs -all-root -force-uid 0 -force-gid 0 -comp xz -noappend
	mkimage -A arm -O linux -T ramdisk -C none -a 41000000 -n "nvr ramdisk" -d $(OUT_DIR)/nvr_target.squashfs $(OUT_DIR)/nvr_target.ramdisk
	cp $(OUT_DIR)/nvr_target.ramdisk $(SDK_DIR)/out/linux/common/
	cp $(KERNEL_DIR)/arch/arm/boot/uImage $(SDK_DIR)/out/linux/common/
	cp $(KERNEL_DIR)/arch/arm/boot/uImage $(OUT_DIR)/uImage_nvr
	cp tools/update/packet.txt $(OUT_DIR)/
	cp tools/update/makeupdatefile $(OUT_DIR)/
	$(OUT_DIR)/makeupdatefile $(OUT_DIR)/packet.txt $(OUT_DIR)/nvr_update_$(VERSION).bin
	mkdir -p $(PRODUCT_DIR)
	cp tools/nvr_tools.rar $(PRODUCT_DIR)/
ifeq ($(JXJ_VERSION), y)
	cp $(OUT_DIR)/nvr_update_$(VERSION).bin $(PRODUCT_DIR)/nvr_update_JXJ_$(VERSION).bin
else
	cp $(OUT_DIR)/nvr_update_$(VERSION).bin $(PRODUCT_DIR)/nvr_update_N_$(VERSION).bin
endif
	-rm -rf $(PRODUCT_DIR)/file.md5
	(cd $(PRODUCT_DIR); md5sum -b * > file.md5)

logo:
	mkdir -p $(OUT_DIR)
	cp tools/update/packet_logo.txt $(OUT_DIR)/
	cp tools/update/makeupdatefile $(OUT_DIR)/
	cp tools/update/linux.bmp $(OUT_DIR)/
	$(OUT_DIR)/makeupdatefile $(OUT_DIR)/packet_logo.txt $(OUT_DIR)/nvr_update_logo.bin

image:
ifeq ($(JXJ_VERSION), y)
	cp $(SDK_DIR)/tools/pack/sun7i_linux_jxj-a20-nvr-100m.img $(PRODUCT_DIR)/sun7i_linux_jxj-a20-nvr-100m-JXJ-$(VERSION).img
else
	cp $(SDK_DIR)/tools/pack/sun7i_linux_jxj-a20-nvr-100m.img $(PRODUCT_DIR)/sun7i_linux_jxj-a20-nvr-100m-N-$(VERSION).img
endif

import: 
	-svn delete $(SVN_SERVER)/testversion/$(shell basename $(PRODUCT_DIR)) -m "delete $(shell basename $(PRODUCT_DIR))"
	svn import $(PRODUCT_DIR) $(SVN_SERVER)/testversion/$(shell basename $(PRODUCT_DIR)) -m "add $(VERSION)"

all: sdk app pack


