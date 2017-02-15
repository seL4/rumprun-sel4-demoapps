#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
# @TAG(D61_BSD)
#

CFILES   := $(wildcard $(SOURCE_DIR)/src/*.c)


ARCHIVE = lmbench
BAKE_TARGET = /home/kentm/workspaces/lmbench3/bin/x86_64-linux-gnu/lmdd
include $(PROJECT_BASE)/apps/rumpcommon.mk

.PHONY: $(BAKE_TARGET)_
$(BAKE_TARGET)_:
