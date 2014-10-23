KDIR :=/lib/modules/$(shell uname -r)/build
PWD :=$(shell pwd)

APPS := aqclient exhaust_memory

all: aqmo_module $(APPS)

EXTRA_CFLAGS += -O2 -g

install: aqmo_module
	./install_aqmo.sh

obj-m += aqmo.o
aqmo-objs := aqmodule.o

aqmo_module: aqmodule.c
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	objdump -d aqmo.ko > aqmo.asm

clean:
	rm -rf *.o .tmp_versions *.ko *.mod* *.asm .aqmo.* modules.* Module.* .*.o.cmd

