#
# Copyright 2017, Data61
# Commonwealth Scientific and Industrial Research Organisation (CSIRO)
# ABN 41 687 119 230.
#
# This software may be distributed and modified according to the terms of
# the BSD 2-Clause license. Note that NO WARRANTY is provided.
# See "LICENSE_BSD2.txt" for details.
#
# @TAG(DATA61_BSD)
#

cmake_minimum_required(VERSION 3.7.2)

project(roottask C)

set(configure_string "")


config_string(RumprunMemoryMiB RUMPRUN_MEMORY_MiB
    "Amount of memory to initialise the Rumprun instance with."
    DEFAULT 16
    UNQUOTE)

config_string(RumprunCommandLine RUMPRUN_COMMAND_LINE
    "App Command line"
    DEFAULT " ")

config_string(RumprunEnvString RUMPRUN_ENV_STRING
    "Specify environment variables for your application.  Note: You need at least one environment variable
        for the config to parse correctly."
    DEFAULT "RUMPRUN_PLATFORM=seL4")

config_string(RumprunExtraConfig RUMPRUN_EXTRA_CONFIG
    "Extra configuration that will be forwarded to Rumprun."
    DEFAULT " ")

config_choice(RumprunPlatform RUMPRUN_PLATFORM "Specify which of the machines (in apps/roottask/include/roottask_platform_config) to use
            You can add additional configurations by adding a new .h file."
    "x86_64_qemu;RumprunPlatformx86_64_qemu;RUMPRUN_PLATFORMx86_64_qemu"
    "optiplex9020;RumprunPlatformxoptiplex9020;RUMPRUN_PLATFORMoptiplex9020")

config_option(RumprunUsePCIEthernet RUMPRUN_USE_PCI_ETHERNET
    "Whether to use PCI ethernet devices."
    DEFAULT OFF)

config_string(RumprunNetworkIFName RUMPRUN_NETWORK_IFNAME
    "Network Interface name such as \"wm0\""
    DEFAULT "wm0"
    DEPENDS RumprunUsePCIEthernet)

config_choice(RumprunDHCPMethod RUMPRUN_DCHP_METHOD "Whether to use dhcp to initialise the network interface"
    "DHCP;RumprunDHCP;RUMPRUN_DHCP;RumprunUsePCIEthernet"
    "Static IP;RumprunStatic;RUMPRUN_STATIC;RumprunUsePCIEthernet")

config_string(RumprunNetworkStaticIP RUMPRUN_NETWORK_STATIC_IP
    "Static IP Address to use"
    DEFAULT "10.0.120.101"
    DEPENDS RumprunStatic)

config_option(HogThread USE_HOG_THREAD
    "This enables a thread which tries to hog the CPU.  It spins in a
    loop and never yields.  Its purpose is for schedular related benchmarking"
    DEFAULT OFF)

config_string(HOGBandwidth HOG_BANDWIDTH
    "This sets the periodic budget of the hog thread on the mcs version of the kernel.
    The value is a percentage the period that the thread gets as its budget.
    25 means that the thread's budget will be 25% of the period"
    DEFAULT 25
    DEPENDS "KernelRT AND HogThread"
    UNQUOTE)



add_config_library(roottask "${configure_string}")

file(GLOB static
    src/*.c src/arch/${KernelArch}/*.c src/plat/${KernelPlatform}/*.c
    src/plat/${RumprunPlatform}/*.c
)
get_property(rumprunapps GLOBAL PROPERTY rumprunapps_property)
set(cpiostring "")
foreach(app_name IN LISTS rumprunapps)
    list(APPEND cpiostring $<TARGET_PROPERTY:${app_name},BAKED_LOCATION>)
endforeach()
MakeCPIO(archive.o "${cpiostring}" DEPENDS ${rumprunapps})

add_executable(roottask EXCLUDE_FROM_ALL ${static} archive.o)
target_include_directories(roottask PRIVATE "include")
target_link_libraries(roottask Configuration muslc sel4 sel4allocman sel4vka sel4utils sel4test sel4platsupport sel4muslcsys rumprun sel4serialserver)
add_dependencies(roottask ${rumprunapps})

# Set this image as the rootserver
DeclareRootserver(roottask)