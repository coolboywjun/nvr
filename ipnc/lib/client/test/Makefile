CURDIR = $(shell pwd)
ROOTDIR = ../../../..
CURWORKDIR = $(CURDIR)/$(ROOTDIR)
include $(ROOTDIR)/Rules.make

CFLAGS:= -I./../inc -I./../../inc
LDFLAGS:= -L../ -lprotocolclient -L$(PRI_LIB_DIR) -lnet

test_ipnc_client:test_ipnc_client.c
	arm-linux-gnueabi-gcc -o test_ipnc_client test_ipnc_client.c $(CFLAGS) $(LDFLAGS) -g -Wall
	cp test_ipnc_client /nvr_bin -rf

#test_client_func:test_client_func.c
#	arm-linux-gnueabi-gcc -o test_client_func test_client_func.c $(CFLAGS) $(LDFLAGS) -g -Wall
#	 cp test_client_func /home/hong/mnt/test

#test_client_interface:test_client_interface.c
#	arm-linux-gnueabi-gcc -o test_client_interface test_client_interface.c $(CFLAGS) $(LDFLAGS) -g -Wall
#	sudo cp test_client_interface /home/hong/mnt/test

.PHONY:clean

clean:
	rm -f test_ipnc_client
	rm -f test_client_interface
