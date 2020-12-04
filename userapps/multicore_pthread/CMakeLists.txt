#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps "multicore_pthread")

if("${APP}" STREQUAL "multicore_pthread")
    BakeExternalRumpkernelCMakeProject(
        multicore_pthread
        SOURCE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        OUTPUT_BIN
        bin/multicore_pthread
    )
endif()
