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

cmake_minimum_required(VERSION 3.7.2)

if("${APP}" STREQUAL "cjpeg")

    CjpegRumprunPackages(cjpeg_external rumprun_toplevel_support)
    CreateRumprunBakeCommand(cjpeg RUMP_TARGETS cjpeg_external)

endif()

if("${APP}" STREQUAL "djpeg")

    DjpegRumprunPackages(djpeg_external rumprun_toplevel_support)
    CreateRumprunBakeCommand(djpeg RUMP_TARGETS djpeg_external)

endif()
