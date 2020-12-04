#
# Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)

if("${APP}" STREQUAL "redis")

    RedisRumprunPackages(redis_external rumprun_toplevel_support)
    CreateRumprunBakeCommand(redis CONFIG sel4_ethernet RUMP_TARGETS redis_external)

endif()
