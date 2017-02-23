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
else
OBJECTBASE = hw-obj
endif


$(STAGE_DIR)/bin/$(RTARGET)/$(ARCHIVE): $(BAKE_TARGET) $(PROJECT_BASE)/build2/$(OBJECTBASE)/rumprun.o | $(BAKE_TARGET)_
	mkdir -p $(STAGE_DIR)/bin/$(RTARGET)
ifeq ($(RTARGET), sel4)
	rumprun-bake sel4_generic $@ $<
else
	rumprun-bake hw_generic $@ $<
	cp -a $@ $(PROJECT_BASE)/images/$(ARCHIVE)-hw
endif
	@echo " [bin] created $@"

$(BAKE_TARGET):
	true
