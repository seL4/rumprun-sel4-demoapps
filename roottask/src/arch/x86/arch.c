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

#include "../../common.h"
#include <rumprun/init_data.h>
#include <utils/fence.h>
#include <platsupport/arch/tsc.h>

/* The current simple we use doesn't break up the ioport cap */
#define IO_PORT_MIN 0
#define IO_PORT_MAX 0
/* copy the caps required to set up the sel4platsupport default timer */
void
arch_copy_IOPort_cap(init_data_t *init, env_t env, sel4utils_process_t *test_process)
{
    seL4_CPtr io_port_cap = simple_get_IOPort_cap(&env->simple, IO_PORT_MIN, IO_PORT_MAX);
    if (io_port_cap == 0) {
        ZF_LOGF("Failed to get IO port cap for range %x to %x\n", IO_PORT_MIN, IO_PORT_MAX);
    }
    /* io port cap (since the default timer on ia32 is the PIT) */
    init->io_port = sel4utils_copy_cap_to_process(test_process, &env->vka, io_port_cap);
}

uint64_t ccount = 0;
uint64_t prev;
uint64_t ts;

void count_idle(void *_arg1, void *_arg2, void *_arg3)
{
    prev = rdtsc_pure();
    while (true) {
        ts = rdtsc_pure();
        if ((ts - prev) < 150) {
            COMPILER_MEMORY_FENCE();
            ccount += (unsigned long long)(ts - prev);
            COMPILER_MEMORY_FENCE();
        }
        prev = ts;
    }
}
