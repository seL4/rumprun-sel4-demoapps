#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps leveldb)

if("${APP}" STREQUAL "leveldb")

    set(RumprunCommandLine "db_bench" CACHE STRING "")
    set(RumprunMemoryMiB 1024 CACHE STRING "")
    set(LibSel4UtilsCSpaceSizeBits 18 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 18 CACHE STRING "" FORCE)
    set(UseLargePages ON CACHE BOOL "" FORCE)

    SetSimulationScriptProperty(MEM_SIZE "3072M")

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property leveldb)

endif()
