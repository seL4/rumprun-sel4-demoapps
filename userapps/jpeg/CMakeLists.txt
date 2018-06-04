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

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps cjpeg)
set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps djpeg)

if ("${APP}" STREQUAL "cjpeg")

    set(RumprunCommandLine "cjpeg -dct int -progressive -opt -outfile tmp/output_large_encode.jpeg /input_large.ppm" CACHE STRING "")
    set(RumprunCookfsDir "projects/rumprun-packages/jpeg/build/input_large.ppm" CACHE STRING "")

    set(LibSel4UtilsCSpaceSizeBits 16 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 16 CACHE STRING "" FORCE)

    CjpegRumprunPackages(cjpeg_external rumprun_toplevel_support)
    CreateRumprunBakeCommand(cjpeg
            RUMP_TARGETS cjpeg_external)

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property cjpeg)

endif()

if ("${APP}" STREQUAL "djpeg")

    set(RumprunCommandLine "djpeg -dct int -ppm -outfile tmp/output_large_decode.ppm /input_large.jpg" CACHE STRING "")
    set(RumprunCookfsDir "projects/rumprun-packages/jpeg/build/input_large.jpg" CACHE STRING "")

    set(LibSel4UtilsCSpaceSizeBits 16 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 16 CACHE STRING "" FORCE)

    DjpegRumprunPackages(djpeg_external rumprun_toplevel_support)
    CreateRumprunBakeCommand(djpeg
            RUMP_TARGETS djpeg_external)

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property djpeg)

endif()
