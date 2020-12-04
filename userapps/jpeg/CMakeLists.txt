#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

if("${APP}" STREQUAL "cjpeg")

    CjpegRumprunPackages(cjpeg_external rumprun_toplevel_support)
    CreateRumprunBakeCommand(cjpeg RUMP_TARGETS cjpeg_external)

endif()

if("${APP}" STREQUAL "djpeg")

    DjpegRumprunPackages(djpeg_external rumprun_toplevel_support)
    CreateRumprunBakeCommand(djpeg RUMP_TARGETS djpeg_external)

endif()
