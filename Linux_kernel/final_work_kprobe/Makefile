PWD := $(shell pwd)
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

C_INCLUDE_PATH=$(KERNEL_DIR)

DRV_NAME := kprobe_traffic
include Kbuild

.PHONY: all all-down build run remove install uninstall clean format check restart test test-params deploy deploy-down

all: build run deploy

all-down: deploy-down remove clean

defconfig: build

build:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

run:
	sudo insmod $(PWD)/$(DRV_NAME).ko

remove:
	sudo rmmod $(DRV_NAME)

install:
	sudo cp $(DRV_NAME).ko /lib/modules/$(shell uname -r)
	sudo /sbin/depmod -a
	sudo /sbin/modprobe $(DRV_NAME)

uninstall:
	sudo /sbin/modprobe -r $(DRV_NAME)
	sudo rm -f /lib/modules/$(shell uname -r)/$(DRV_NAME).ko
	sudo /sbin/depmod -a

clean:
	@cp stats_exporter/go.mod stats_exporter/go.mod.bak 2>/dev/null || true
	@cp stats_exporter/go.sum stats_exporter/go.sum.bak 2>/dev/null || true
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	@mv stats_exporter/go.mod.bak stats_exporter/go.mod 2>/dev/null || true
	@mv stats_exporter/go.sum.bak stats_exporter/go.sum 2>/dev/null || true

format:
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i

check:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
	@echo "Module built successfully"

restart: remove build run clean

test: build
	@echo "Testing kprobe_traffic module..."
	sudo insmod $(PWD)/$(DRV_NAME).ko
	@sleep 1
	sudo dmesg | tail -10
	sudo rmmod $(DRV_NAME)
	@echo "Test completed"

test-params: build
	@echo "Testing kprobe_traffic module params..."
	sudo insmod $(PWD)/$(DRV_NAME).ko
	@sleep 1
	cat /sys/module/$(DRV_NAME)/parameters/top_n
	echo 42 | sudo tee /sys/module/$(DRV_NAME)/parameters/top_n
	cat /sys/module/$(DRV_NAME)/parameters/top_n
	@sleep 2
	cat /sys/module/$(DRV_NAME)/parameters/top_addrs
	sudo dmesg | tail -10
	sudo rmmod $(DRV_NAME)
	@echo "Test completed"

deploy: run
	docker compose -f ./stats_exporter/docker-compose.yaml up -d

deploy-down:
	docker compose -f ./stats_exporter/docker-compose.yaml down