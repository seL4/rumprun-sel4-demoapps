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
#include <autoconf.h>
#include <rumprun/init_data.h>
#include <platsupport/plat/pit.h>
#include <platsupport/io.h>
#include <platsupport/arch/tsc.h>
#include <sel4platsupport/platsupport.h>
#include <sel4platsupport/arch/io.h>
#include <sel4utils/benchmark_track.h>
#include "common.h"
#include <sel4/benchmark_track_types.h>
#include <arch_stdio.h>
/* This file is for dumping benchmark results over serial to an external receiver */

/* Print out a summary of what has been tracked */
#ifdef CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER
static inline void
seL4_BenchmarkTrackDumpSummary_pri(benchmark_track_kernel_entry_t *logBuffer, uint32_t logSize)
{
    uint32_t index = 0;
    uint32_t syscall_entries[8];
    uint32_t interrupt_entries = 0;
    uint32_t userlevelfault_entries = 0;
    uint32_t vmfault_entries = 0;
    uint64_t syscall_time[8];
    uint32_t cap_entries[32];
    uint64_t cap_time[32];
    uint64_t interrupt_time = 0;
    uint64_t userlevelfault_time = 0;
    uint64_t vmfault_time = 0;
    uint64_t ksTotalKernelTime = 0;
    uint64_t ksOtherKernelTime = 0;

    memset((void *)syscall_entries, 0, 32);
    memset((void *)syscall_time, 0, 64);
    memset((void *)cap_entries, 0, 128);
    memset((void *)cap_time, 0, 256);
    /* Default driver to use for output now is serial.
     * Change this to use other drivers than serial, i.e ethernet
     */
    while (logBuffer[index].start_time != 0 && index < logSize) {
        if (logBuffer[index].entry.path == Entry_Interrupt) {
            ksOtherKernelTime += logBuffer[index].duration;
        } else {
            ksTotalKernelTime += logBuffer[index].duration;
        }
        if (logBuffer[index].entry.path == Entry_Syscall) {
            int syscall_no = logBuffer[index].entry.syscall_no;
            if (syscall_no == 7 ) {
                cap_entries[logBuffer[index].entry.cap_type]++;
                cap_time[logBuffer[index].entry.cap_type] += logBuffer[index].duration;
                if (logBuffer[index].entry.cap_type == 2) {
                    printf("invoc: %"PRId32"\n", logBuffer[index].entry.invocation_tag);
                }

            }

            syscall_entries[syscall_no]++;
            syscall_time[syscall_no] += logBuffer[index].duration;

        } else if (logBuffer[index].entry.path == Entry_Interrupt) {
            interrupt_entries++;
            interrupt_time += logBuffer[index].duration;

        } else if (logBuffer[index].entry.path == Entry_UserLevelFault) {
            userlevelfault_entries++;
            userlevelfault_time += logBuffer[index].duration;

        } else if (logBuffer[index].entry.path == Entry_VMFault) {
            vmfault_entries++;
            vmfault_time += logBuffer[index].duration;

        }
        index++;
    }
    printf("kt: %"PRIx64"\n ot: %"PRIx64"\n", ksTotalKernelTime, ksOtherKernelTime);

    for (int i = 0; i < 8; i++) {
        printf("sc: %"PRId32" i: %"PRId32" c: %"PRIx64"\n", i, syscall_entries[i], syscall_time[i]);
    }
    for (int i = 0; i < 32; i++) {
        printf("scc: %"PRId32" i: %"PRId32" c: %"PRIx64"\n", i, cap_entries[i], cap_time[i]);
    }
    printf("int: %"PRId32" c: %"PRIx64"\n", interrupt_entries, interrupt_time);
    printf("ulf: %"PRId32" c: %"PRIx64"\n", userlevelfault_entries, userlevelfault_time);
    printf("vmf: %"PRId32" c: %"PRIx64"\n", vmfault_entries, vmfault_time);
}
#endif // CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER

void handle_char(rump_env_t *env, int c)
{
    /* stateful vars for input char processing */
    static int pos = 0;
    static uint64_t cpucount = 0;
    static uint64_t cpucount2 = 0;
    static uint64_t start_idle_count = 0;
    static char reset_buffer[] = "reset";

    /* Reset machine if "reset" received over serial */
    if (c == reset_buffer[pos]) {
        pos++;
        if (pos == strlen(reset_buffer)) {
            ps_io_port_out(&env->ops.io_port_ops, 0x64, 1, 0xFE);
            /* DOES NOT RETURN - resets machine */
        }
    } else {
        pos = 0;
    }

    switch (c) {
    case -1:
        return;
    case 'a':
#ifdef CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER
        /* Reset log buffer */
        ZF_LOGF_IF(log_buffer == NULL,
                   "A user-level buffer has to be set before resetting benchmark.\
                   Use seL4_BenchmarkSetLogBuffer\n");
        int error = seL4_BenchmarkResetLog();
        ZF_LOGF_IF(error, "Could not reset log");
#endif /* CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER */
        /* Record tsc count */
        cpucount = rdtsc_pure();
        start_idle_count = ccount;
        break;
    case 'b':
        /* Stop benchmarking when 'b' character */
        /* Record finish time */
        cpucount2 = rdtsc_pure();
#ifdef CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER
        /* Stop recording kernel entries */
        logIndexFinalized = seL4_BenchmarkFinalizeLog();
#endif /* CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER */
        printf("tot: %"PRIu64"\n idle: %"PRIu64"\n", cpucount2 - cpucount, ccount-start_idle_count);
        break;
    case 'c':
#ifdef CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER
        benchmark_track_kernel_entry_t *ksLog = (benchmark_track_kernel_entry_t *) log_buffer;
        printf("dumping log: %"PRId32", %zd %zd\n", logIndexFinalized,
                sizeof(benchmark_track_kernel_entry_t), sizeof(kernel_entry_t));
        seL4_BenchmarkTrackDumpSummary_pri(ksLog, logIndexFinalized);//ksLogIndexFinalized);
#endif /* CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER */
        break;
    default:
        break;
    }

    /* print char received */
    if (c == '\r') {
        __arch_putchar('\n');
    } else {
       __arch_putchar(c);
    }
}
