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

.PHONY: susan
susan:
	cd $(PROJECT_BASE)/projects/rumprun-packages/susan && \
	env -i PATH=${PATH} RUMPRUN_TOOLCHAIN_TUPLE=$(RUMPRUN_TOOLTUPLE) make
