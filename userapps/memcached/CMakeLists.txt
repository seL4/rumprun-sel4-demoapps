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

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps memcached)

if ("${APP}" STREQUAL "memcached")

    set(RumprunCommandLine "memcached -u root" CACHE STRING "")
    set(RumprunDHCPMethod "Static IP" CACHE STRING "")
    set(RumprunUsePCIEthernet ON CACHE STRING "" FORCE)
    set(RumprunMemoryMiB  128 CACHE STRING "")
    set(RumprunDHCPMethod "Static IP" CACHE STRING "")

    set(LibSel4UtilsCSpaceSizeBits 17 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 16 CACHE STRING "" FORCE)

    MemcachedRumprunPackages(memcached_external rumprun_toplevel_support)
    CreateRumprunBakeCommand(memcached
            CONFIG sel4_ethernet    	
            RUMP_TARGETS memcached_external)

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property memcached)

endif()
