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
#

cmake_minimum_required(VERSION 3.7.2)

# Define our top level settings. Whilst they have doc strings for readability here
# Users should initialize a build directory by doing something like
# mkdir build
# cd build
# ../init-build.sh -DPLATFORM=x86_64 -DSIMULATION=TRUE -DAPP=Hello
set(SIMULATION OFF CACHE BOOL "Include only simulation compatible tests")
set(RELEASE OFF CACHE BOOL "Performance optimized build")
set(PLATFORM "x86_64" CACHE STRING "Platform to test")
set(supported_platforms x86_64 ia32)
set_property(CACHE PLATFORM PROPERTY STRINGS ${supported_platforms})
set(APP "hello" CACHE STRING "Application to build")

# Determine the platform/architecture
if(${PLATFORM} IN_LIST supported_platforms)
    set(KernelArch x86 CACHE STRING "" FORCE)
    set(KernelX86Sel4Arch ${PLATFORM} CACHE STRING "" FORCE)
else()
    message(FATAL_ERROR "Unknown PLATFORM. Initial configuration may not work")
endif()

if(SIMULATION)
    ApplyCommonSimulationSettings(${KernelArch})
endif()

ApplyCommonReleaseVerificationSettings(${RELEASE} OFF)

file(GLOB result ${CMAKE_CURRENT_LIST_DIR}/userapps/*/settings.cmake)

foreach(file ${result})
    include("${file}")
endforeach()
