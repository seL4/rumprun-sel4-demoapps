#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps susan)

if("${APP}" STREQUAL "susan")

    set(
        RumprunCommandLine "susan /input_small.pgm /tmp/output_small.smoothing.pgm -s"
        CACHE STRING ""
    )
    set(RumprunCookfsDir "projects/rumprun-packages/susan/build/input_small.pgm" CACHE STRING "")

    set(LibSel4UtilsCSpaceSizeBits 16 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 16 CACHE STRING "" FORCE)

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property susan)

endif()
