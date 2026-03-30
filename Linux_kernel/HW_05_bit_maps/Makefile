PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

C_INCLUDE_PATH=$(KERNEL_DIR)

DRV_NAME := kernel_alloc

$(DRV_NAME)-y := src/main.o src/allocator.o src/bitmap.o src/params.o

obj-m := $(DRV_NAME).o

.PHONY: build run remove install uninstall clean format check

all: build

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
	clang-format -i src/*.c src/*.h

check:
	@echo "Checking module build..."
	@$(MAKE) build
	@echo "Module built successfully"

restart: remove build run clean

test: build
	@echo "Load module"
	sudo dmesg -C
	sudo insmod $(PWD)/$(DRV_NAME).ko
	@echo "Get stats"
	cat /sys/module/$(DRV_NAME)/parameters/stats
	@echo "Allocate 8192 KiB"
	echo 8192 | sudo tee /sys/module/$(DRV_NAME)/parameters/alloc
	sudo dmesg | grep '$(DRV_NAME): allocated' | tail -1
	@echo "Allocate 16384"
	echo 16384 | sudo tee /sys/module/$(DRV_NAME)/parameters/alloc
	sudo dmesg | grep '$(DRV_NAME): allocated' | tail -1
	@echo "Get stats"
	cat /sys/module/$(DRV_NAME)/parameters/stats
	@echo "bitmap info"
	cat /sys/module/$(DRV_NAME)/parameters/bitmap_info
	@echo "Free 8 KiB"
	echo 0xffffd16ba58b9000 | sudo tee /sys/module/$(DRV_NAME)/parameters/free
	sudo dmesg | grep '$(DRV_NAME): freed' | tail -1
	@echo "Get stats"
	cat /sys/module/$(DRV_NAME)/parameters/stats
	@echo "unload module"
	sudo rmmod $(DRV_NAME)
	@echo "funish"
