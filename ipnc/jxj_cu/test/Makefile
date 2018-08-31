CURDIR = $(shell pwd)
ROOTDIR = ../../..
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

CFLAGS:= -I./../../lib/client/inc -I./../../lib/inc -I/lhw/nvr/nvr/private/inc
LDFLAGS:= -L../../lib/client -lprotocolclient -L$(PRI_LIB_DIR) -lnet

test_jxj_cu_ipnc_client:test_jxj_cu_ipnc_client.c
	arm-linux-gnueabi-gcc -o test_jxj_cu_ipnc_client test_jxj_cu_ipnc_client.c $(CFLAGS) $(LDFLAGS) -g -Wall
	cp test_jxj_cu_ipnc_client /nvr_bin -rf

.PHONY:clean

clean:
	rm -f test_jxj_cu_ipnc_client
	rm -f test_client_interface
