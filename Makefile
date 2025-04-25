obj-m += mychardev.o

KERNEL_DIR := /usr/src/linux-headers-5.15.0-138-generic
all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean