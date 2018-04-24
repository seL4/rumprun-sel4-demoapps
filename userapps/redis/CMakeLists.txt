#
# Copyright 2018, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(DATA61_BSD)

cmake_minimum_required(VERSION 3.7.2)

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps redis)

if ("${APP}" STREQUAL "redis")

    set(RumprunCommandLine "redis /data/conf/redis.conf" CACHE STRING "")
    set(RumprunCookfsDir "projects/rumprun-packages/redis/images/data" CACHE STRING "")
    set(RumprunMemoryMiB  128 CACHE STRING "")
    set(RumprunDHCPMethod "Static IP" CACHE STRING "")
    set(LibSel4UtilsCSpaceSizeBits 18 CACHE STRING "" FORCE)
    set(RumprunUsePCIEthernet ON CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 18 CACHE STRING "" FORCE)

    SetSimulationScriptProperty(MEM_SIZE "1024M")
    RedisRumprunPackages(redis_external rumprun_toplevel_support)
    CreateRumprunBakeCommand(redis
            CONFIG sel4_ethernet
            RUMP_TARGETS redis_external)

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property redis)

endif()
