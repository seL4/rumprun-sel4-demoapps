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
libc=libmuslc

# arg1 : component name
# arg2 : CONFIG_Variable to test
# arg3 : PATH to binary produced by recursive make
# arg4 : (optional) Rumprun config
#
# Example:
# RUMPRUN_BAKE_DEPENDENCY(cjpeg,CONFIG_APP_CJPEG,
#	${PWD}/projects/rumprun-packages/jpeg/build/jpeg-6a/cjpeg,
#	sel4_generic)

define RUMPRUN_BAKE_DEPENDENCY

components-$$($2) += $1
roottask-components-$$($2) += $1
$1-y = rumprun-toplevel-support
$1_bin_name := $3
ifeq ($4,)
ifdef CONFIG_RUMPRUN_USE_PCI_ETHERNET
$1_rr_config := sel4_ethernet
else
$1_rr_config := sel4_generic
endif
else
$1_rr_config := $4
endif

$1: $$($1-y)

endef


-include $(patsubst apps/roottask/Kbuild,,$(wildcard apps/*/Kbuild))
-include apps/roottask/Kbuild
-include $(wildcard libs/*/Kbuild)
