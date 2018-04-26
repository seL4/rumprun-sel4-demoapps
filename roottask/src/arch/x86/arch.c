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
#define IO_PORT_MIN 0xcf8
#define IO_PORT_MAX 0xcff
/* copy the caps required to set up the sel4platsupport default timer */
void
arch_copy_IOPort_cap(init_data_t *init, rump_env_t *env, sel4utils_process_t *test_process)
{
    cspacepath_t path;
    seL4_Error error = vka_cspace_alloc_path(&env->vka, &path);
    if (error) {
        ZF_LOGF("Failed to allocate slot");
    }

    error = simple_get_IOPort_cap(&env->simple, IO_PORT_MIN, IO_PORT_MAX, path.root, path.capPtr, path.capDepth);
    if (error) {
        ZF_LOGF("Failed to get IO port cap for range %x to %x\n", IO_PORT_MIN, IO_PORT_MAX);
    }
    /* io port cap (since the default timer on ia32 is the PIT) */
    init->io_port = sel4utils_copy_path_to_process(test_process, path);
    ZF_LOGF_IF(init->io_port == 0, "copy cap failed");

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

void __attribute__((optimize("O0"))) hog_thread(void *_arg1, void *_arg2, void *_arg3)
{
    while (true) {
        COMPILER_MEMORY_FENCE();
        asm volatile ("nop");
    }
}
