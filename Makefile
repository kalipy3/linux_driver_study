KERNELDIR := /home/kalipy/下载/linux-4.4.235/linux-4.4.235

CURRENT_PATH := $(shell pwd)

obj-m :=dtsof.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
