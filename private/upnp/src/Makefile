SrcDir =
ObjDir =
IncludeDir =
INSTALL_DIR = /usr/lib/
Src = $(wildcard $(SrcDir)*.c)
SrcFile = $(notdir $(Src))
ObjFile = $(patsubst %.c,%.o,$(SrcFile))
Obj = $(addprefix $(ObjDir),$(ObjFile))
CROSS  := arm_v5t_le-
#CROSS :=

CC := $(CROSS)gcc
AR := $(CROSS)ar

CFLAGS := -g -Wall -Werror  -I$(IncludeDir)
LDFLAGS := 


OUTPUT	:= libupnp.so
#OUTPUT_A	:= libupnp.a
TEST := test_upnp



all:main
main:$(Obj)
	$(CC) -shared -o $(OUTPUT) $^
#	$(AR) crvs $(OUTPUT_A) $^
#	$(CC) -o $(TEST) $^ $(LDFLAGS)
$(ObjDir)%.o:$(SrcDir)%.c
	$(CC) -c $(CFLAGS) -fpic  $< -o $@

.PHONY: clean install
clean:
	rm -f $(OUTPUT)
#	rm -f $(OUTPUT_A)
	rm -f $(ObjDir)*.o

install:
#	cp $(OUTPUT) $(INSTALL_DIR)
