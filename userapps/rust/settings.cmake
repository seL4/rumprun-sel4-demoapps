#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps rust)

if("${APP}" STREQUAL "rust")

    set(RumprunCommandLine "./rust" CACHE STRING "")
    set(RumprunDHCPMethod "Static IP" CACHE STRING "")
    set(RumprunUsePCIEthernet ON CACHE STRING "" FORCE)
    set(RumprunMemoryMiB 128 CACHE STRING "")
    set(LibSel4UtilsCSpaceSizeBits 18 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 18 CACHE STRING "" FORCE)

    SetSimulationScriptProperty(MEM_SIZE "1024M")

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property rust)

endif()
