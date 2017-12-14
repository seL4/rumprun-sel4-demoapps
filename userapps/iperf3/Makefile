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

include ${PWD}/libs/rumprun/platform/sel4/rumprunlibs.mk

.PHONY: $(BAKE_TARGET)_
$(BAKE_TARGET)_: src/iperf_config.h
	CC=$(RUMPRUN_CC) make || true


src/iperf_config.h:
	cd $(PWD)/projects/iperf && git checkout refs/tags/3.1.3 && git am $(SOURCE_DIR)/patches/*
	cd $(BUILD_DIR) && env -i PATH=${PATH} CC=$(RUMPRUN_CC) ac_cv_header_netinet_sctp_h=no \
	$(PROJECT_BASE)/projects/iperf/configure --host=$(RUMPRUN_TOOLTUPLE) --enable-shared=no
