PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

C_INCLUDE_PATH=$KERNEL_DIR

DRV_NAME := work_module

$(DRV_NAME)-y := module.o idx_param.o ch_val_param.o my_str_param.o update.o

obj-m := $(DRV_NAME).o

.PHONY: build run remove install uninstall clean format check

build: format
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

run:
	insmod $(PWD)/$(DRV_NAME).ko

remove:
	rmmod $(DRV_NAME).ko

install:
	cp $(DRV_NAME).ko /lib/modules/$(shell uname -r)
	/sbin/depmod -a
	/sbin/modprobe $(DRV_NAME)

uninstall:
	/sbin/modprobe -r $(DRV_NAME)
	rm -f /lib/modules/$(shell uname -r)/$(DRV_NAME).ko
	/sbin/depmod -a

clean: 
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

format:
	clang-format -i *.c *.h

check:
	sh check.sh

restart: remove build run clean
