#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps "hello")

if("${APP}" STREQUAL "hello")
    set(RumprunCommandLine "Hello world" CACHE STRING "")
    set(LibSel4UtilsCSpaceSizeBits 16 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 16 CACHE STRING "" FORCE)
    set_property(GLOBAL APPEND PROPERTY rumprunapps_property hello)
endif()
