#
# Copyright 2018, Data61, CSIRO (ABN 41 687 119 230)
#
# SPDX-License-Identifier: BSD-2-Clause
#

cmake_minimum_required(VERSION 3.7.2)
project(Multicore_pthread C)

add_executable(multicore_pthread main.c)
target_link_libraries(multicore_pthread m)

install(TARGETS multicore_pthread RUNTIME DESTINATION bin)
