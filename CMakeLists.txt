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

cmake_minimum_required(VERSION 3.7.2)

include(${KERNEL_HELPERS_PATH})

GenerateSimulateScript()

find_file(RUMPRUN_PATH rumprun PATHS ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/projects CMAKE_FIND_ROOT_PATH_BOTH)
mark_as_advanced(FORCE RUMPRUN_PATH)
if("${RUMPRUN_PATH}" STREQUAL "RUMPRUN_PATH-NOTFOUND")
    message(FATAL_ERROR "Failed to find rumprun. Consider cmake -DRUMPRUN_PATH=/path/to/rumprun")
endif()

find_file(RUMPRUN_PACKAGES_PATH rumprun-packages PATHS ${CMAKE_SOURCE_DIR}/projects CMAKE_FIND_ROOT_PATH_BOTH)
mark_as_advanced(FORCE RUMPRUN_PACKAGES_PATH)
if("${RUMPRUN_PACKAGES_PATH}" STREQUAL "RUMPRUN_PACKAGES_PATH-NOTFOUND")
    message(FATAL_ERROR "Failed to find rumprun-packages. Consider cmake -DRUMPRUN_PATH=/path/to/rumprun-packages")
endif()

include(${RUMPRUN_PATH}/helpers.cmake)
include(${RUMPRUN_PACKAGES_PATH}/helpers.cmake)

# Define our top level settings. Whilst they have doc strings for readability here
# Users should initialize a build directory by doing something like
# mkdir build
# cd build
# ../init-build.sh -DPLATFORM=x86_64 -DSIMULATION=TRUE -DAPP=Hello
set(SIMULATION OFF CACHE BOOL "Include only simulation compatible tests")
set(RELEASE OFF CACHE BOOL "Performance optimized build")
set(PLATFORM "x86_64" CACHE STRING "Platform to test")
set_property(CACHE PLATFORM PROPERTY STRINGS ${KernelX86Sel4Arch_all_strings})
set(APP "Hello" CACHE STRING "Application to build")

# Determine the platform/architecture
if (${PLATFORM} IN_LIST KernelX86Sel4Arch_all_strings)
    set(KernelArch x86 CACHE STRING "" FORCE)
    set(KernelX86Sel4Arch ${PLATFORM} CACHE STRING "" FORCE)
else()
    message(FATAL_ERROR "Unknown PLATFORM. Initial configuration may not work")
endif()

if(SIMULATION)
    ApplyCommonSimulationSettings()
endif()

ApplyCommonReleaseVerificationSettings(${RELEASE} OFF)

include(ExternalProject)

# Import apps with CMake support
add_subdirectory(userapps/hello)
add_subdirectory(userapps/redis)
add_subdirectory(userapps/iperf3)

add_subdirectory(roottask)

get_property(rumprun_apps GLOBAL PROPERTY AvailableRumprunApps)

set_property(CACHE APP PROPERTY STRINGS ${rumprun_apps})

