obj-m += task2_module.o
SHELL=/bin/bash

K_VER=$(shell uname -r)
PWD := $(CURDIR)
MODULE=task2_module
DRIVER=customdevice


all: build load

build:
	make -C /lib/modules/$(K_VER)/build M=$(PWD) modules

load:
	@sudo insmod $(MODULE).ko

	@if ! grep -q $(DRIVER) /proc/devices; then \
		echo "Failed to load $(DRIVER)"; \
		sudo rmmod $(MODULE); \
		exit 1; \
	fi;  \

	#@sudo mknod /dev/$(DRIVER) c $$(grep -oP "\d+(?=\ $(DRIVER))" /proc/devices) 0
	@sudo chmod ugo+rw /dev/$(DRIVER)

unload:
	@sudo rmmod $(MODULE)
	#@sudo unlink /dev/$(DRIVER)

clean:
	make -C /lib/modules/$(K_VER)/build M=$(PWD) clean

test:
	gcc -o test_prog test.c
	@./test_prog
