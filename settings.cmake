#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

set(project_dir "${CMAKE_CURRENT_LIST_DIR}")
get_filename_component(resolved_path ${CMAKE_CURRENT_LIST_FILE} REALPATH)
# repo_dir is distinct from project_dir as this file is symlinked.
# project_dir corresponds to the top level project directory, and
# repo_dir is the absolute path after following the symlink.
get_filename_component(repo_dir ${resolved_path} DIRECTORY)

include(${project_dir}/tools/seL4/cmake-tool/helpers/application_settings.cmake)

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
    set(KernelSel4Arch ${PLATFORM} CACHE STRING "" FORCE)
    set(KernelPlatform pc99 CACHE STRING "" FORCE)
else()
    message(FATAL_ERROR "Unknown PLATFORM. Initial configuration may not work")
endif()

include(${project_dir}/kernel/configs/seL4Config.cmake)

if(SIMULATION)
    ApplyCommonSimulationSettings(${KernelSel4Arch})
endif()

ApplyCommonReleaseVerificationSettings(${RELEASE} OFF)

file(GLOB result ${repo_dir}/userapps/*/settings.cmake)

foreach(file ${result})
    include("${file}")
endforeach()
