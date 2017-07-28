/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(DATA61_BSD)
 */


#pragma once
#include <autoconf.h>
#include <sel4/bootinfo.h>

#include <vka/vka.h>
#include <vka/object.h>
#include <sel4utils/elf.h>
#include <sel4utils/process.h>
#include <simple/simple.h>
#include <vspace/vspace.h>
#include <rumprun/init_data.h>
#include <sel4platsupport/serial.h>
#include <sel4platsupport/timer.h>
#include <roottask_platform_config/config.h>

typedef struct env *env_t;

#define INIT_DATA_NUM_FRAMES 2




/* TODO replace this with querying PCI_config to find correct hardware info */
static const struct mmio mmios[] = CONFIG_RUMPRUN_PLATFORM_MMIOS;


typedef struct rump_process_data {
    init_data_t *init;
    vka_object_t untypeds[CONFIG_MAX_NUM_BOOTINFO_UNTYPED_CAPS];
    int num_untypeds_devram;
    int num_untypeds;
    int num_untypeds_dev;
} rump_process_data_t;

struct env {
    /* An initialised vka that may be used by the test. */
    vka_t vka;
    /* virtual memory management interface */
    vspace_t vspace;
    /* abtracts over kernel version and boot environment */
    simple_t simple;
    timer_objects_t timer_objects;
    serial_objects_t serial_objects;
    rump_process_data_t rump_process;
    /* extra cap to the init data frame for mapping into the remote vspace */
    seL4_CPtr init_frame_cap_copy[INIT_DATA_NUM_FRAMES];
};
#ifdef CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER
extern void *log_buffer;
#endif //CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER

extern uint64_t ccount;

extern struct env env;
void init_timer_caps(env_t env);
void init_serial_caps(env_t env);
void count_idle(void*, void*, void*);

void arch_copy_IOPort_cap(init_data_t *init, env_t env, sel4utils_process_t *test_process);
seL4_CPtr copy_cap_to_process(sel4utils_process_t *process, seL4_CPtr cap);
void serial_interrupt(void*, void *, void *);
#ifdef CONFIG_ARM_SMMU
seL4_SlotRegion arch_copy_iospace_caps_to_process(sel4utils_process_t *process, env_t env);
#endif
