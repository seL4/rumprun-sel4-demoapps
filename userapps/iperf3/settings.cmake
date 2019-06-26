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

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps iperf3)

if("${APP}" STREQUAL "iperf3")
    set(RumprunCommandLine "iperf3 -sJ" CACHE STRING "")
    set(RumprunDHCPMethod "Static IP" CACHE STRING "")
    set(RumprunUsePCIEthernet ON CACHE STRING "" FORCE)
    set(RumprunMemoryMiB 128 CACHE STRING "")
    set(LibSel4UtilsCSpaceSizeBits 18 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 18 CACHE STRING "" FORCE)

    SetSimulationScriptProperty(MEM_SIZE "1024M")

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property iperf)

endif()
