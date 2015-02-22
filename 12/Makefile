KERNEL_TREE_PATH?=/lib/modules/$(shell uname -r)/build

EXTRA_CFLAGS = -DDEBUG

obj-m += list_test.o

all: list_test.ko

list_test.ko: list_test.c
	make -C $(KERNEL_TREE_PATH) M=$(PWD) modules

clean:
	make -C $(KERNEL_TREE_PATH) M=$(PWD) clean

.PHONY: all clean
