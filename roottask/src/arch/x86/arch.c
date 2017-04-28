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
#include <sel4platsupport/plat/pit.h>
#include <sel4platsupport/plat/timer.h>
#include <platsupport/plat/serial.h>
#include <utils/fence.h>
#include <platsupport/arch/tsc.h>

/* copy the caps required to set up the sel4platsupport default timer */
void
arch_copy_timer_caps(init_data_t *init, env_t env, sel4utils_process_t *test_process)
{
    /* io port cap (since the default timer on ia32 is the PIT) */
    init->io_port = sel4utils_copy_cap_to_process(test_process, &env->vka, env->io_port_cap);
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


void
init_serial_caps(env_t env)
{
    /* get the irq control cap */
    seL4_CPtr cap;
    int error = vka_cspace_alloc(&env->vka, &cap);
    if (error != 0) {
        ZF_LOGF("Failed to allocate cslot, error %d", error);
    }
    vka_cspace_make_path(&env->vka, cap, &env->serial_irq);
    error = simple_get_IRQ_handler(&env->simple, SERIAL_CONSOLE_COM1_IRQ, env->serial_irq);
    if (error != 0) {
        ZF_LOGF("Failed to get IRQ control, error %d", error);
    }
    error = seL4_IRQHandler_Ack(env->serial_irq.capPtr);
    if (error != 0) {
        ZF_LOGF("Failed to ack IRQ , error %d", error);
    }

}

void
init_timer_caps(env_t env)
{
    /* get the irq control cap */
    seL4_CPtr cap;
    int error = vka_cspace_alloc(&env->vka, &cap);
    if (error != 0) {
        ZF_LOGF("Failed to allocate cslot, error %d", error);
    }
    vka_cspace_make_path(&env->vka, cap, &env->irq_path);
    error = simple_get_IRQ_handler(&env->simple, PIT_INTERRUPT, env->irq_path);
    if (error != 0) {
        ZF_LOGF("Failed to get IRQ control, error %d", error);
    }

    env->io_port_cap = simple_get_IOPort_cap(&env->simple, PIT_IO_PORT_MIN, PIT_IO_PORT_MAX);
    if (env->io_port_cap == 0) {
        ZF_LOGF("Failed to get IO port cap for range %x to %x\n", PIT_IO_PORT_MIN, PIT_IO_PORT_MAX);
    }
}
