#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
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

ifeq ($(RTARGET), sel4)
export PATH = $(PWD)/build2/rumprun2/bin:$(shell printenv PATH)
else
export PATH = $(PWD)/build2/rumprun/bin:$(shell printenv PATH)
endif

-include .config
include tools/common/project.mk


simulate-ia32:
	qemu-system-i386 \
		-m 512 -nographic -net nic,model=e1000 -net tap,script=no,ifname=tap0 -kernel images/kernel-ia32-pc99 \
		-initrd images/${apps}-image-ia32-pc99 ${GDB_ATTACH}
mqrun:
	mq.sh run -s sandy -c "All is well" -f images/kernel-ia32-pc99 -f images/${apps}-image-ia32-pc99
