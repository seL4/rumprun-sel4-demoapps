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

ARCHIVE = netserver
BAKE_TARGET = $(BUILD_DIR)/$(RTARGET)/src/netserver
include $(PROJECT_BASE)/apps/rumpcommon.mk

.PHONY: $(BAKE_TARGET)_
$(BAKE_TARGET)_: $(RTARGET)/config.h
	mkdir -p $(BUILD_DIR)/$(RTARGET)/
	cd $(BUILD_DIR)/$(RTARGET) && CC=$(RRTOOLCHAIN)-gcc make


$(RTARGET)/config.h:
	mkdir -p $(BUILD_DIR)/$(RTARGET)/
	cd $(BUILD_DIR)/$(RTARGET) && CC=$(RRTOOLCHAIN)-gcc ac_cv_func_setpgrp_void=yes \
	$(PROJECT_BASE)/projects/benchmarks/netperf-2.7.0/configure --host=$(RRTOOLCHAIN) --enable-intervals \
