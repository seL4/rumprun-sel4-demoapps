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

set_property(GLOBAL APPEND PROPERTY AvailableRumprunApps iperf3)
set(PatchDirectory ${CMAKE_CURRENT_LIST_DIR})

if ("${APP}" STREQUAL "iperf3")
    find_file(IPERF_PATH iperf PATHS ${CMAKE_SOURCE_DIR}/projects CMAKE_FIND_ROOT_PATH_BOTH)
    mark_as_advanced(FORCE IPERF_PATH)
    if("${IPERF_PATH}" STREQUAL "IPERF_PATH-NOTFOUND")
        message(FATAL_ERROR "Failed to find iperf. Consider cmake -DIPERF_PATH=/path/to/iperf")
    endif()

    set(RumprunCommandLine "iperf3 -sJ" CACHE STRING "")
    set(RumprunDHCPMethod "Static IP" CACHE STRING "")
    set(RumprunUsePCIEthernet ON CACHE STRING "" FORCE)
    set(RumprunMemoryMiB  128 CACHE STRING "")
    set(LibSel4UtilsCSpaceSizeBits 18 CACHE STRING "" FORCE)
    set(KernelRootCNodeSizeBits 18 CACHE STRING "" FORCE)

    SetSimulationScriptProperty(MEM_SIZE "1024M")

    add_custom_target(iperf3_rumptools_target)
    add_dependencies(iperf3_rumptools_target rumprun_toplevel_support)

    set(source_dir ${IPERF_PATH})
    set(bin_dir ${CMAKE_CURRENT_BINARY_DIR}/iperf3-build)
    set(bin_file ${bin_dir}/src/iperf3)
    set(tuple $<TARGET_PROPERTY:rumprun_toplevel_support,RUMPRUN_TOOLCHAIN_TUPLE>)
    set(tool_location $<TARGET_PROPERTY:rumprun_toplevel_support,RUMPRUN_TOOLCHAIN_PATH>)
    set(stamp_dir ${CMAKE_CURRENT_BINARY_DIR}/iperf_external-stamp)
    # TODO: Consider whether to download this in this stage once old buildsystem removed
    ExternalProject_Add(iperf_external
        SOURCE_DIR ${source_dir}
        BINARY_DIR ${bin_dir}
        STAMP_DIR ${stamp_dir}
        DEPENDS iperf3_rumptools_target
        UPDATE_COMMAND git checkout refs/tags/3.1.3
        PATCH_COMMAND git am ${CMAKE_CURRENT_LIST_DIR}/patches/0001-Iperf-Remove-mmap-write-flag.patch
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -E env PATH=$ENV{PATH}:${tool_location}
                            CC=${tuple}-gcc ac_cv_header_netinet_sctp_h=no
                            ${source_dir}/configure --host=${tuple} --enable-shared=no
        EXCLUDE_FROM_ALL
        BUILD_COMMAND  ${CMAKE_COMMAND} -E env PATH=$ENV{PATH}:${tool_location} make || true
        INSTALL_COMMAND true
        BUILD_ALWAYS ON
    )
    ExternalProject_Add_StepTargets(iperf_external install)

    # Add custom_command that depends on the install stamp file of ExternalProject to force stale checking of
    # output files until after the install step of ExternalProject has been run
    add_custom_command(
        OUTPUT ${bin_file}
        COMMAND true
        DEPENDS iperf_external-install ${stamp_dir}/iperf_external-install
        )

    # Save bin files as property on the ExternalProject target
    set_target_properties(iperf_external PROPERTIES RUMP_BINARIES ${bin_file})

    CreateRumprunBakeCommand(iperf
            CONFIG sel4_ethernet
            RUMP_TARGETS iperf_external)

    set_property(GLOBAL APPEND PROPERTY rumprunapps_property iperf)

endif()
