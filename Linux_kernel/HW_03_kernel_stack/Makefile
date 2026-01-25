PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

C_INCLUDE_PATH=$(KERNEL_DIR)

.PHONY: all build run remove install uninstall clean format check defconfig

all: build

defconfig:
	@touch .config

build:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

run:
	insmod $(PWD)/kernel_stack.ko

remove:
	rmmod kernel_stack

install:
	cp kernel_stack.ko /lib/modules/$(shell uname -r)
	/sbin/depmod -a
	/sbin/modprobe kernel_stack

uninstall:
	/sbin/modprobe -r kernel_stack
	rm -f /lib/modules/$(shell uname -r)/kernel_stack.ko
	/sbin/depmod -a

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean

format:
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i

check:
	sh check.sh

restart: remove build run clean
