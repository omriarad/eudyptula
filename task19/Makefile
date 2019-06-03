SHELL := /bin/bash
CONFIG_MODULE_SIG=n
CFLAGS_netfilter.o := -DDEBUG
KERNEL ?= /lib/modules/$(shell uname -r)/build
obj-m += netfilter.o

all:
	make -C $(KERNEL) M=$(PWD) modules;

clean:
	make -C $(KERNEL) M=$(PWD) clean;
