SHELL := /bin/bash
CONFIG_MODULE_SIG=n
CFLAGS_hello_world.o := -DDEBUG
KERNEL ?= /lib/modules/$(shell uname -r)/build
obj-m += coding_style.o

all:
	make -C $(KERNEL) M=$(PWD) modules;

clean:
	make -C $(KERNEL) M=$(PWD) clean;
