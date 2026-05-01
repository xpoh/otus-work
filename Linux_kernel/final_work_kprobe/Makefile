PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

C_INCLUDE_PATH=$(KERNEL_DIR)

DRV_NAME := kprobe_traffic
include Kbuild

.PHONY: all build run remove install uninstall clean format check restart test

all: build

build:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

run:
	insmod $(PWD)/$(DRV_NAME).ko

remove:
	rmmod $(DRV_NAME)

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
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i

check:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
	@echo "Module built successfully"

restart: remove build run clean

test: build
	@echo "Testing kprobe_traffic module..."
	sudo insmod $(PWD)/$(DRV_NAME).ko
	@echo "Module loaded"
	@sleep 2
	@echo "Checking kprobe output..."
	sudo dmesg | tail -20
	sudo rmmod $(DRV_NAME)
	@echo "Test completed"