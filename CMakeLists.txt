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

include(${KERNEL_HELPERS_PATH})

GenerateSimulateScript()

find_file(
    RUMPRUN_PATH rumprun
    PATHS ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/projects
    CMAKE_FIND_ROOT_PATH_BOTH
)
mark_as_advanced(FORCE RUMPRUN_PATH)
if("${RUMPRUN_PATH}" STREQUAL "RUMPRUN_PATH-NOTFOUND")
    message(FATAL_ERROR "Failed to find rumprun. Consider cmake -DRUMPRUN_PATH=/path/to/rumprun")
endif()

find_file(
    RUMPRUN_PACKAGES_PATH rumprun-packages
    PATHS ${CMAKE_SOURCE_DIR}/projects
    CMAKE_FIND_ROOT_PATH_BOTH
)
mark_as_advanced(FORCE RUMPRUN_PACKAGES_PATH)
if("${RUMPRUN_PACKAGES_PATH}" STREQUAL "RUMPRUN_PACKAGES_PATH-NOTFOUND")
    message(
        FATAL_ERROR
            "Failed to find rumprun-packages. Consider cmake -DRUMPRUN_PATH=/path/to/rumprun-packages"
    )
endif()

include(${RUMPRUN_PATH}/helpers.cmake)
include(${RUMPRUN_PACKAGES_PATH}/helpers.cmake)

include(ExternalProject)

# Import apps with CMake support
add_subdirectory(userapps/jpeg)
add_subdirectory(userapps/memcached)
add_subdirectory(userapps/multicore_pthread)
add_subdirectory(userapps/hello)
add_subdirectory(userapps/leveldb)
add_subdirectory(userapps/susan)
add_subdirectory(userapps/redis)
add_subdirectory(userapps/nginx)
add_subdirectory(userapps/iperf3)
add_subdirectory(userapps/rust)

add_subdirectory(roottask)

get_property(rumprun_apps GLOBAL PROPERTY AvailableRumprunApps)

set_property(CACHE APP PROPERTY STRINGS ${rumprun_apps})
