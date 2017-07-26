#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
# @TAG(DATA61_BSD)
#

ifeq ($(RTARGET), sel4)
OBJECTBASE = sel4-obj
ifeq ($(SEL4_ARCH), ia32)
RRTOOLCHAIN= i486-rumprun-netbsdelf
endif
ifeq ($(SEL4_ARCH), x86_64)
RRTOOLCHAIN= x86_64-rumprun-netbsd
endif
else
OBJECTBASE = hw-obj
RRTOOLCHAIN= i486-rumprun-netbsdelf
endif
include ${PWD}/rumprun/platform/sel4/rumprunlibs.mk

-include .config

ifdef CONFIG_RUMPRUN_USE_PCI_ETHERNET
RR_CONFIG ?= sel4_ethernet
else
RR_CONFIG ?= sel4_generic
endif

$(STAGE_DIR)/bin/$(RTARGET)/$(ARCHIVE): $(BAKE_TARGET) $(PROJECT_BASE)/build2/$(SEL4_ARCH)/$(OBJECTBASE)/rumprun.o | $(BAKE_TARGET)_
	mkdir -p $(STAGE_DIR)/bin/$(RTARGET)
ifeq ($(RTARGET), sel4)
	RUMPRUN_BASEDIR=$(dir $(BASEFILE)) rumprun-bake $(RR_CONFIG) $@ $<
else
	rumprun-bake hw_generic $@ $<
	cp -a $@ $(PROJECT_BASE)/images/$(ARCHIVE)-hw
endif
	@echo " [bin] created $@"

$(BAKE_TARGET):
	true
