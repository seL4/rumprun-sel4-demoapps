
CFILES   := $(wildcard $(SOURCE_DIR)/src/*.c)

PATH2 := ${PATH}

ARCHIVE = iperf3
BAKE_TARGET = $(BUILD_DIR)/$(RTARGET)/src/iperf3

include $(PROJECT_BASE)/apps/rumpcommon.mk

.PHONY: $(BAKE_TARGET)_
$(BAKE_TARGET)_: $(RTARGET)/src/iperf_config.h
	mkdir -p $(BUILD_DIR)/$(RTARGET)/
	cd $(BUILD_DIR)/$(RTARGET) && CC=$(RRTOOLCHAIN)-gcc make || true


$(RTARGET)/src/iperf_config.h:
	mkdir -p $(BUILD_DIR)/$(RTARGET)/
	cd $(PROJECT_BASE)/projects/iperf && git checkout refs/tags/3.1.3 && git am $(SOURCE_DIR)/patches/*
	cd $(BUILD_DIR)/$(RTARGET) && env -i PATH=${PATH2} CC=$(RRTOOLCHAIN)-gcc ac_cv_header_netinet_sctp_h=no \
	$(PROJECT_BASE)/projects/iperf/configure --host=$(RRTOOLCHAIN) --enable-shared=no
