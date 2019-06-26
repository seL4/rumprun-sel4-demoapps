#
# Copyright 2019, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(DATA61_BSD)

cmake_minimum_required(VERSION 3.7.2)

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps "multicore_pthread")

if("${APP}" STREQUAL "multicore_pthread")
    set(RumprunCommandLine "pthread 16 2" CACHE STRING "")
    set(LibSel4UtilsCSpaceSizeBits 16 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 16 CACHE STRING "" FORCE)

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property multicore_pthread)
endif()
