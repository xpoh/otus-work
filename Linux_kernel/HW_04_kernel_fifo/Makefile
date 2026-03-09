PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

C_INCLUDE_PATH=$(KERNEL_DIR)

DRV_NAME := kernel_fifo

$(DRV_NAME)-y := src/main.o src/fifo_ops.o src/params.o

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
	@echo "Testing kernel FIFO module..."
	sudo insmod $(PWD)/$(DRV_NAME).ko initial_size=10
	@echo "Module loaded, testing parameters..."
	@echo "Adding elements..."
	echo 10 | sudo tee /sys/module/$(DRV_NAME)/parameters/enqueue
	echo 20 | sudo tee /sys/module/$(DRV_NAME)/parameters/enqueue
	echo 30 | sudo tee /sys/module/$(DRV_NAME)/parameters/enqueue
	@echo "Current size:"
	sudo cat /sys/module/$(DRV_NAME)/parameters/size
	@echo "Peek value:"
	sudo cat /sys/module/$(DRV_NAME)/parameters/peek
	@echo "Dequeue value:"
	sudo cat /sys/module/$(DRV_NAME)/parameters/dequeue
	@echo "New size:"
	sudo cat /sys/module/$(DRV_NAME)/parameters/size
	@echo "Is empty:"
	sudo cat /sys/module/$(DRV_NAME)/parameters/is_empty
	@echo "Available space:"
	sudo cat /sys/module/$(DRV_NAME)/parameters/available
	@echo "Clearing queue..."
	echo 1 | sudo tee /sys/module/$(DRV_NAME)/parameters/clear
	@echo "Is empty after clear:"
	sudo cat /sys/module/$(DRV_NAME)/parameters/is_empty
	@echo "Unloading module..."
	sudo rmmod $(DRV_NAME)
	@echo "Test completed"
