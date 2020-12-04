#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps cjpeg)
set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps djpeg)

if("${APP}" STREQUAL "cjpeg")

    set(
        RumprunCommandLine
        "cjpeg -dct int -progressive -opt -outfile tmp/output_large_encode.jpeg /input_large.ppm"
        CACHE STRING ""
    )
    set(RumprunCookfsDir "projects/rumprun-packages/jpeg/build/input_large.ppm" CACHE STRING "")

    set(LibSel4UtilsCSpaceSizeBits 16 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 16 CACHE STRING "" FORCE)
    set_property(GLOBAL APPEND PROPERTY rumprunapps_property cjpeg)

endif()

if("${APP}" STREQUAL "djpeg")

    set(
        RumprunCommandLine
        "djpeg -dct int -ppm -outfile tmp/output_large_decode.ppm /input_large.jpg"
        CACHE STRING ""
    )
    set(RumprunCookfsDir "projects/rumprun-packages/jpeg/build/input_large.jpg" CACHE STRING "")

    set(LibSel4UtilsCSpaceSizeBits 16 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 16 CACHE STRING "" FORCE)

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property djpeg)

endif()
