
# create by xq
# Thu Jul 18 09:28:56 CST 2013

# CURWORKDIR define in the subdir Makefile
# include this Rules.make file must define the CURWORKDIR before

# publish debug or release
PUBLISH ?= debug

# 版本规则
# 第一个数字：代表主版本号，代表整体架构发生改变
# 第二个数字：代表支持的平台协议个数 
# 第三个数字：次版本号
VERSION ?=v1.3.2
GUI_VERSION ?=v1.1.17

# 品牌版本还是中性版本，y:jxj品牌版本，n:中性版本
JXJ_VERSION ?=n

# 系统使用文件系统目录
DEST_DIR ?= a20_target_dbg

APP_DIR := /at/a20/nvr
SDK_DIR := $(APP_DIR)/../a20_svn
CHARSET := BIG5
#CHARSET := GB2312
WEB_FILE_JXJ := web_NVR_JXJ_V3.25.0.0.zip
WEB_FILE_N := web_NVR_N_V3.25.0.0.zip

# dm8107
# ARCH := arm
# CROSS_PREFIX := arm-none-linux-gnueabi-
# PKG_CONFIG := $(CROSS_PREFIX)/pkg-config
# PKG_CONFIG_PATH := /opt/arm-2009q1/arm-none-linux-gnueabi/lib/pkgconfig

# a20
ARCH := arm
CROSS_PREFIX := arm-linux-gnueabi-
PKG_CONFIG := $(CROSS_PREFIX)pkg-config
PKG_CONFIG_PATH := /opt/a20/external-toolchain/arm-linux-gnueabi/libc/usr/lib/pkgconfig
KERNEL_DIR := $(SDK_DIR)/linux-3.4
TARGET_DIR := $(APP_DIR)/../$(DEST_DIR)

# x86
# ARCH := x86
# CROSS_PREFIX :=
# PKG_CONFIG := pkg-config
# PKG_CONFIG_PATH :=
# KERNEL_DIR := /usr/src/linux-3.4.71-gentoo
# TARGET_DIR := /opt/nvr_target

UBOOT_DIR := $(SDK_DIR)/u-boot
OUT_DIR := $(APP_DIR)/out
ifeq ($(JXJ_VERSION), y)
PRODUCT_DIR := $(OUT_DIR)/product_JXJ_$(VERSION)
WEB_FILE := $(WEB_FILE_JXJ)
else
PRODUCT_DIR := $(OUT_DIR)/product_N_$(VERSION)
WEB_FILE := $(WEB_FILE_N)
endif
AV_POOL_DIR := $(APP_DIR)/av_pool
DECODER_DIR := $(APP_DIR)/decoder
GUI_DIR := $(APP_DIR)/gui
IPNC_DIR := $(APP_DIR)/ipnc
PRI_DIR := $(APP_DIR)/private
PRI_LIB_DIR := $(PRI_DIR)/libs
PRI_INC_DIR := $(PRI_DIR)/inc
PRI_TEST_DIR := $(PRI_DIR)/test
PUB_DIR := $(APP_DIR)/public
RTSP_DIR := $(APP_DIR)/rtsp
SERVER_DIR := $(APP_DIR)/server
STORAGE_DIR := $(APP_DIR)/storage
WEB_SERVER_DIR := $(APP_DIR)/web_server

TARGET_APP_DIR := $(TARGET_DIR)/opt/nvr
TARGET_LIB_DIR := $(TARGET_DIR)/usr/lib
TARGET_WEB_DIR := $(TARGET_DIR)/var/www
TARGET_MODULE_DIR := $(TARGET_APP_DIR)/modules

SVN_SERVER := svn://192.168.1.130/nvr

CFLAGS_EXTRA := -Wall -g -D__DEBUG__ -pipe 

ifeq ($(ARCH), arm)
CFLAGS_EXTRA := $(CFLAGS_EXTRA) -DARCH_ARM
else
CFLAGS_EXTRA := $(CFLAGS_EXTRA) -DARCH_X86
endif


CFLAGS_EXTRA := $(CFLAGS_EXTRA) -I$(PRI_INC_DIR)
#CFLAGS_EXTRA := -Wall -O2 -fno-strict-aliasing  -I$(PRI_INC_DIR)
LDFLAGS_EXTRA := -L$(PRI_LIB_DIR)

export VERSION
export GUI_VERSION
export JXJ_VERSION
export WEB_FILE
export CROSS_PREFIX
export PKG_CONFIG
export PKG_CONFIG_PATH
export SDK_DIR
export APP_DIR
export UBOOT_DIR
export KERNEL_DIR
export OUT_DIR
export PRODUCT_DIR
export AV_POOL_DIR
export DECODER_DIR
export GUI_DIR
export IPNC_DIR
export PRI_DIR
export PUB_DIR
export RTSP_DIR
export SERVER_DIR
export STORAGE_DIR
export WEB_SERVER_DIR
export TARGET_DIR
export TARGET_APP_DIR
export TARGET_LIB_DIR
export TARGET_WEB_DI
export SVN_SERVER
export WEB_SVN_PATH
export WEB_DIR
export CFLAGS_EXTRA
export LFLAGS_EXTRA

