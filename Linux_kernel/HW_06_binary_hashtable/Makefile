PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

C_INCLUDE_PATH=$(KERNEL_DIR)

DRV_NAME := kernel_hashtable_search

$(DRV_NAME)-y := src/main.o src/params.o src/build.o src/search.o

obj-m := $(DRV_NAME).o

.PHONY: build run remove install uninstall clean format check restart test

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

restart: remove build run

test: build
	@echo "Testing kernel hashtable search module..."
	sudo insmod $(PWD)/$(DRV_NAME).ko array_size=1000
	@echo "Searching for 42..."
	echo 42 | sudo tee /sys/module/$(DRV_NAME)/parameters/search
	@echo "Result:"
	sudo cat /sys/module/$(DRV_NAME)/parameters/result
	@echo "Bucket 5 dump:"
	echo 5 | sudo tee /sys/module/$(DRV_NAME)/parameters/bucket_id
	sudo cat /sys/module/$(DRV_NAME)/parameters/bucket_dump
	@echo "Rebuilding..."
	echo 1 | sudo tee /sys/module/$(DRV_NAME)/parameters/rebuild
	@echo "Unloading module..."
	sudo rmmod $(DRV_NAME)
	@echo "Test completed"
