KDIR :=/lib/modules/$(shell uname -r)/build
PWD :=$(shell pwd)

APPS := aqclient exhaust_memory

all: mimo_module $(APPS)

EXTRA_CFLAGS += -O2 -g

install: mimo_module
	./install_mimo.sh

obj-m += mimo.o
mimo-objs := mimodule.o

mimo_module: mimodule.c
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	objdump -d mimo.ko > mimo.asm

clean:
	rm -rf *.o .tmp_versions *.ko *.mod* *.asm .mimo.* modules.* Module.* .*.o.cmd

