#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(DATA61_BSD)
#
lib-dirs:=libs .

ifneq ($(RTARGET), hw)
override RTARGET = sel4
endif
export RTARGET

# The main target we want to generate
all: app-images
export PROJECT_BASE = $(PWD)

-include .config
include tools/common/project.mk
PATH_OLD := $(shell printenv PATH)
ifeq ($(RTARGET), sel4)
export PATH = $(PWD)/build2/$(SEL4_ARCH)/rumprun/bin:$(shell printenv PATH)
else
export PATH = $(PWD)/build2/rumprun/bin:$(shell printenv PATH)
endif


simulate:
	qemu-system-x86_64 \
		-m 512 -nographic -net nic,model=e1000 -net tap,script=no,ifname=tap0 -kernel images/kernel-$(SEL4_ARCH)-$(PLAT) \
		-initrd images/${apps}-image-$(SEL4_ARCH)-$(PLAT) -cpu Haswell

update_config:
	cp .config  configs/$(roottask-components-y)-$(SEL4_ARCH)-$(CONFIG_RUMPRUN_PLATFORM:"%"=%)_defconfig

ifeq ($V,)
SUPPRESS_OUTPUT = > /dev/null
endif
hw_redis_x64:
	@echo [Creating bmk rumprun rootfs with redis config]
	$(Q)mkdir -p build2/bmk/hw-obj/rootfs
	$(Q)rsync -av projects/rumprun-packages/redis/images/data \
	rumprun/lib/librumprunfs_base/rootfs/ build2/bmk/hw-obj/rootfs/ --delete $(SUPPRESS_OUTPUT)
	@echo [Building] bmk rumprun packages
	$(Q)(cd rumprun && \
		env -i PATH=$(PATH_OLD) \
		CC=gcc \
		./build-rr.sh -d ../build2/bmk/rumprun -o ../build2/bmk/hw-obj hw -- -r) $(SUPPRESS_OUTPUT)
	@echo [Building] redis
	$(Q)mkdir -p build2/bmk/redis
	$(Q)(cd projects/rumprun-packages/redis && \
		env -i PATH=$(PATH_OLD):$(PWD)/build2/bmk/rumprun/bin/ \
		BUILD_DIR=$(PWD)/build2/bmk/redis/ \
		RUMPRUN_TOOLCHAIN_TUPLE=x86_64-rumprun-netbsd \
		make redis-server) $(SUPPRESS_OUTPUT)
	@echo [Packaging redis rumprun bmk unikernel]
	$(Q)(cd projects/rumprun-packages/redis && \
		env -i PATH=$(PATH_OLD):$(PWD)/build2/bmk/rumprun/bin/ \
		RUMPRUN_BASEDIR=$(PWD)/build2/bmk/hw-obj/ \
		rumprun-bake hw_ethernet $(PWD)/build2/bmk/redis/redis.bin $(PWD)/build2/bmk/redis/bin/redis-server) $(SUPPRESS_OUTPUT)

hw_simulate_x64:
	qemu-system-x86_64 -nographic -net nic,model=e1000 -net tap,script=no,ifname=tap0  -m 3072 -kernel $(PWD)/build2/bmk/redis/redis.bin -append  \
	'{,, "net" :  {,, "if":"wm0",, "type":"inet",,"method":"static",, "addr":"10.0.120.101",,"mask":"24",,},, "cmdline": "redis.bin /data/conf/redis.conf",,},,'
