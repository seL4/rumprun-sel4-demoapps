#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
# @TAG(D61_BSD)
#

ARCHIVE = python
BAKE_TARGET = $(PROJECT_BASE)/projects/rumprun-packages/python3/build/python
include $(PROJECT_BASE)/apps/rumpcommon.mk

.PHONY: $(BAKE_TARGET)_
$(BAKE_TARGET)_:
	cd $(PROJECT_BASE)/projects/rumprun-packages/python3 && make
