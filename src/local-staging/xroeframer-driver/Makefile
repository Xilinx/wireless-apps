obj-m := framer.o
framer-y := xroe_framer.o sysfs_xroe.o sysfs_xroe_framer_stats.o sysfs_xroe_framer_shared.o sysfs_xroe_framer_deframer.o sysfs_xroe_framer_cfg.o

ccflags-y := -IInclude

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
