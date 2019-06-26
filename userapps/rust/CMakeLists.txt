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

if("${APP}" STREQUAL "rust")

    set(tuple $<TARGET_PROPERTY:rumprun_toplevel_support,RUMPRUN_TOOLCHAIN_TUPLE>)
    set(tool_location $<TARGET_PROPERTY:rumprun_toplevel_support,RUMPRUN_TOOLCHAIN_PATH>)
    set(bin_file ${CMAKE_CURRENT_BINARY_DIR}/output/rust)
    add_custom_command(
        OUTPUT ${bin_file}
        COMMAND
            cd ${CMAKE_CURRENT_LIST_DIR} && ${CMAKE_COMMAND} -E env PATH=$ENV{PATH}:${tool_location}
            cargo build
            --target ${tuple}
            --target-dir ${CMAKE_CURRENT_BINARY_DIR} -Z unstable-options
            --out-dir ${CMAKE_CURRENT_BINARY_DIR}/output
        DEPENDS rumprun_toplevel_support
        COMMENT "[Invoking cargo]"
    )

    add_custom_target(rust_external DEPENDS ${bin_file})
    set_target_properties(rust_external PROPERTIES RUMP_BINARIES ${bin_file})

    CreateRumprunBakeCommand(rust CONFIG sel4_ethernet RUMP_TARGETS rust_external)

endif()
