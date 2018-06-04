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

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps leveldb)

if ("${APP}" STREQUAL "leveldb")
    find_file(LEVELDB_PATH leveldb PATHS ${CMAKE_SOURCE_DIR}/projects CMAKE_FIND_ROOT_PATH_BOTH)
    mark_as_advanced(FORCE LEVELDB_PATH)
    if("${LEVELDB_PATH}" STREQUAL "LEVELDB_PATH-NOTFOUND")
        message(FATAL_ERROR "Failed to find leveldb. Consider cmake -DLEVELDB_PATH=/path/to/leveldb")
    endif()

    set(RumprunCommandLine "db_bench" CACHE STRING "")
    set(RumprunMemoryMiB  1024 CACHE STRING "")
    set(LibSel4UtilsCSpaceSizeBits 18 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 18 CACHE STRING "" FORCE)
    set(UseLargePages ON CACHE BOOL "" FORCE)

    SetSimulationScriptProperty(MEM_SIZE "3072M")

    add_custom_target(leveldb_rumptools_target)
    add_dependencies(leveldb_rumptools_target rumprun_toplevel_support)

    set(source_dir ${LEVELDB_PATH})
    set(bin_dir ${CMAKE_CURRENT_BINARY_DIR}/leveldb-build)
    set(bin_file ${source_dir}/out-static/db_bench)
    set(tuple $<TARGET_PROPERTY:rumprun_toplevel_support,RUMPRUN_TOOLCHAIN_TUPLE>)
    set(tool_location $<TARGET_PROPERTY:rumprun_toplevel_support,RUMPRUN_TOOLCHAIN_PATH>)
    set(stamp_dir ${CMAKE_CURRENT_BINARY_DIR}/leveldb_external-stamp)
    # TODO: Consider whether to download this in this stage once old buildsystem removed
    ExternalProject_Add(leveldb_external
        SOURCE_DIR ${source_dir}
        BINARY_DIR ${bin_dir}
        STAMP_DIR ${stamp_dir}
        DEPENDS leveldb_rumptools_target
        EXCLUDE_FROM_ALL
        CONFIGURE_COMMAND true
        BUILD_COMMAND  cd ${source_dir} &&
          ${CMAKE_COMMAND} -E env PATH=$ENV{PATH}:${tool_location} TARGET_OS=NetBSD CXX=${tuple}-g++ CC=${tuple}-gcc make
        INSTALL_COMMAND true
        BUILD_ALWAYS ON
    )
    ExternalProject_Add_StepTargets(leveldb_external install)

    # Add custom_command that depends on the install stamp file of ExternalProject to force stale checking of
    # output files until after the install step of ExternalProject has been run
    add_custom_command(
        OUTPUT ${bin_file}
        COMMAND true
        DEPENDS leveldb_external-install ${stamp_dir}/leveldb_external-install
        )

    # Save bin files as property on the ExternalProject target
    set_target_properties(leveldb_external PROPERTIES RUMP_BINARIES ${bin_file})

    CreateRumprunBakeCommand(leveldb
             RUMP_TARGETS leveldb_external)

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property leveldb)

endif()
