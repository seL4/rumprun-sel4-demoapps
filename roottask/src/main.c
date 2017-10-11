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
#include <allocman/bootstrap.h>
#include <allocman/vka.h>
#include <simple/simple.h>
#include <simple-default/simple-default.h>
#include <sel4platsupport/platsupport.h>
#include <sel4platsupport/serial.h>
#include <platsupport/local_time_manager.h>
#include <sel4platsupport/arch/io.h>
#include <sel4platsupport/io.h>
#include <sel4debug/register_dump.h>
#include <cpio/cpio.h>
#include <sel4/bootinfo.h>
#include <sel4utils/stack.h>
#include <sel4utils/util.h>
#include <sel4utils/time_server/client.h>
#include <serial_server/parent.h>
#include <vka/object.h>
#include <platsupport/io.h>
#include <vka/object_capops.h>

#include <vspace/vspace.h>
#include "common.h"
#include <rumprun/init_data.h>

#define RUMP_UNTYPED_MEMORY (BIT(25))
/* Number of untypeds to try and use to allocate the driver memory. */
#define RUMP_NUM_UNTYPEDS 16

#ifndef CONFIG_HOG_BANDWIDTH
#define CONFIG_HOG_BANDWIDTH 100
#endif

/* ammount of dev_ram memory to give to Rump kernel */
#define RUMP_DEV_RAM_MEMORY MiB_TO_BYTES(CONFIG_RUMPRUN_MEMORY_MiB)
/* Number of untypeds to try and use to allocate the driver memory.
 * if we cannot get 32mb with 16 untypeds then something is probably wrong */
#define RUMP_NUM_DEV_RAM_UNTYPEDS 20

/* dimensions of virtual memory for the allocator to use */
#define ALLOCATOR_VIRTUAL_POOL_SIZE (BIT(seL4_PageBits) * 1000)

/* static memory for the allocator to bootstrap with */
#define ALLOCATOR_STATIC_POOL_SIZE (BIT(seL4_PageBits) * 20)
static char allocator_mem_pool[ALLOCATOR_STATIC_POOL_SIZE];

/* static memory for virtual memory bootstrapping */
static sel4utils_alloc_data_t data;


/* environment encapsulating allocation interfaces etc */
static rump_env_t env = {0};

extern vspace_t *muslc_this_vspace;
extern reservation_t muslc_brk_reservation;
extern void *muslc_brk_reservation_start;
extern char _cpio_archive[];


static inline rump_process_t *process_from_id(int id)
{
    return &env.processes[id - 1];
}

/* initialise our runtime environment */
static void
init_env(rump_env_t *env)
{
    allocman_t *allocman;
    reservation_t virtual_reservation;
    int error;

    /* create an allocator */
    allocman = bootstrap_use_current_simple(&env->simple, ALLOCATOR_STATIC_POOL_SIZE, allocator_mem_pool);
    ZF_LOGF_IF(allocman == NULL, "Failed to create allocman");

    /* create a vka (interface for interacting with the underlying allocator) */
    allocman_make_vka(&env->vka, allocman);

    /* create a vspace (virtual memory management interface). We pass
     * boot info not because it will use capabilities from it, but so
     * it knows the address and will add it as a reserved region */
    error = sel4utils_bootstrap_vspace_with_bootinfo_leaky(&env->vspace,
                                                           &data, simple_get_pd(&env->simple),
                                                           &env->vka, platsupport_get_bootinfo());
    ZF_LOGF_IF(error, "Failed to bootstrap vspace");

    sel4utils_res_t muslc_brk_reservation_memory;
    error = sel4utils_reserve_range_no_alloc(&env->vspace, &muslc_brk_reservation_memory, 1048576, seL4_AllRights, 1, &muslc_brk_reservation_start);
    ZF_LOGF_IF(error, "Failed to reserve_range");

    muslc_this_vspace = &env->vspace;
    muslc_brk_reservation.res = &muslc_brk_reservation_memory;

    /* fill the allocator with virtual memory */
    void *vaddr;
    virtual_reservation = vspace_reserve_range(&env->vspace,
                                               ALLOCATOR_VIRTUAL_POOL_SIZE, seL4_AllRights, 1, &vaddr);
    ZF_LOGF_IF(virtual_reservation.res == 0, "Failed to provide virtual memory for allocator");

    bootstrap_configure_virtual_pool(allocman, vaddr,
                                     ALLOCATOR_VIRTUAL_POOL_SIZE, simple_get_pd(&env->simple));
}

/* Allocate untypeds till either a certain number of bytes is allocated
 * or a certain number of untyped objects */
static unsigned int
allocate_untypeds(vka_object_t *untypeds, size_t bytes, unsigned int max_untypeds, bool can_use_dev)
{
    unsigned int num_untypeds = 0;
    size_t allocated = 0;

    /* try to allocate as many of each possible untyped size as possible */
    for (uint8_t size_bits = seL4_MaxUntypedBits; size_bits > PAGE_BITS_4K; size_bits--) {
        /* keep allocating until we run out, or if allocating would
         * cause us to allocate too much memory*/
        while (num_untypeds < max_untypeds &&
                allocated + BIT(size_bits) <= bytes &&
                vka_alloc_object_at_maybe_dev(&env.vka, seL4_UntypedObject, size_bits, VKA_NO_PADDR,
                                              can_use_dev, &untypeds[num_untypeds]) == 0) {
            allocated += BIT(size_bits);
            num_untypeds++;
        }
    }
    return num_untypeds;
}

/* copy untyped caps into a processes cspace, return the cap range they can be found in */
static int
copy_untypeds_to_process(rump_process_t *process)
{
    seL4_SlotRegion range = {0};
    int total_caps = process->num_untypeds_devram + process->num_untypeds + process->num_untypeds_dev;
    for (int i = 0; i < total_caps; i++) {
        seL4_CPtr slot = sel4utils_copy_cap_to_process(&process->process, &env.vka, process->untypeds[i].cptr);
        /* ALLOCMAN_UT_KERNEL, ALLOCMAN_UT_DEV, ALLOCMAN_UT_DEV_MEM */
        uint8_t untyped_is_device;
        if (i < process->num_untypeds_devram) {
            untyped_is_device = ALLOCMAN_UT_DEV_MEM;
        } else if (i < process->num_untypeds_devram + process->num_untypeds) {
            untyped_is_device = ALLOCMAN_UT_KERNEL;
        } else {
            untyped_is_device = ALLOCMAN_UT_DEV;
        }
        process->init->untyped_list[i].size_bits = process->untypeds[i].size_bits;
        process->init->untyped_list[i].is_device = untyped_is_device;
        process->init->untyped_list[i].paddr = vka_utspace_paddr(&env.vka, process->untypeds[i].ut, seL4_UntypedObject, process->untypeds[i].size_bits);
        /* set up the cap range */
        if (i == 0) {
            range.start = slot;
        }
        range.end = slot;
    }
    range.end++;
    ZF_LOGF_IF((range.end - range.start) != total_caps, "Invalid number of caps");
    process->init->untypeds = range;
    return 0;
}


/* map the init data into the process, and send the address via ipc */
static void *
send_init_data(rump_env_t *env, seL4_CPtr endpoint, rump_process_t *process)
{
    /* map the cap into remote vspace */
    void *remote_vaddr = vspace_share_mem(&env->vspace, &process->process.vspace, process->init,
                                          INIT_DATA_NUM_FRAMES, PAGE_BITS_4K, seL4_AllRights, true);

    ZF_LOGF_IF(remote_vaddr == NULL, "Failed to share memory with launched process");

    /* now send a message telling the process what address the data is at */
    seL4_MessageInfo_t info = seL4_MessageInfo_new(seL4_Fault_NullFault, 0, 0, 1);
    seL4_SetMR(0, (seL4_Word) remote_vaddr);
    seL4_Send(endpoint, info);

    return remote_vaddr;
}

int alloc_untypeds(rump_process_t *process)
{

    // Allocate DEV_MEM memory.
    bool can_be_dev = true;
    process->num_untypeds_devram = allocate_untypeds(process->untypeds, RUMP_DEV_RAM_MEMORY, RUMP_NUM_DEV_RAM_UNTYPEDS, can_be_dev);
    int current_index = process->num_untypeds_devram;
    can_be_dev = false;
    process->num_untypeds = allocate_untypeds(process->untypeds + current_index, RUMP_UNTYPED_MEMORY, RUMP_NUM_UNTYPEDS, can_be_dev);
    current_index += process->num_untypeds;
    return 0;
}

int alloc_devices(rump_process_t *process) {
    device_t *devices = get_devices();
    process->num_untypeds_dev = 0;
    int current_index = process->num_untypeds_devram + process->num_untypeds;
    for (int i = 0; i < get_num_devices(); i++) {
        if ((strlen(CONFIG_RUMPRUN_NETWORK_IFNAME) > 0) &&
            (strcmp(CONFIG_RUMPRUN_NETWORK_IFNAME, devices[i].name) == 0)) {
            int j;
            for (j = 0; j < devices[i].num_mmios; j++) {
                int error = vka_alloc_object_at_maybe_dev(&env.vka, seL4_UntypedObject,
                                                          devices[i].mmios[j].size_bits,
                                                          devices[i].mmios[j].paddr,
                                                          true, process->untypeds + current_index + j);
                ZF_LOGF_IF(error, "Could not allocate untyped");
            }
            process->num_untypeds_dev = j;
            process->init->interrupt_list[0].bus = devices[i].pci.bus;
            process->init->interrupt_list[0].dev = devices[i].pci.dev;
            process->init->interrupt_list[0].function = devices[i].pci.function;

            ps_irq_t irq = {
                .type = PS_IOAPIC,
                .ioapic.vector = devices[i].irq_num,
                .ioapic.ioapic = 0

            };
            if (irq.ioapic.vector >= 16) {
                irq.ioapic.level = 1;
                irq.ioapic.polarity = 1;
            }
            process->init->interrupt_list[0].irq = irq;
        }
    }
    return 0;
}

void launch_process(const char *bin_name, int id)
{
    /* create a frame that will act as the init data, we can then map
     * into target processes */
    rump_process_t *process = process_from_id(id);
    process->init = (init_data_t *) vspace_new_pages(&env.vspace, seL4_AllRights, INIT_DATA_NUM_FRAMES, PAGE_BITS_4K);
    ZF_LOGF_IF(process->init == NULL, "Could not create init_data frame");

    int error = tm_alloc_id_at(&env.time_manager, id);
    ZF_LOGF_IF(error, "Failed to allocate timeout id");

    /* setup init priority.  Reduce by 2 so that we can have higher priority serial thread
       for benchmarking */
    process->init->priority = seL4_MaxPrio - 2;
    process->init->rumprun_memory_size = RUMP_DEV_RAM_MEMORY;

    error = vka_alloc_notification(&env.vka, &process->timer_signal);
    ZF_LOGF_IF(error, "Failed to allocate client notification");

    /* badge the fault endpoint to use for messages such that we can distinguish them */
    cspacepath_t badged_ep_path;
    error = vka_cspace_alloc_path(&env.vka, &badged_ep_path);
    ZF_LOGF_IF(error, "Failed to allocate path");
        cspacepath_t ep_path;
    vka_cspace_make_path(&env.vka, env.ep.cptr, &ep_path);
    error = vka_cnode_mint(&badged_ep_path, &ep_path, seL4_AllRights, seL4_CapData_Badge_new(id));
    ZF_LOGF_IF(error, "Failed to badge ep");

    sel4utils_process_config_t config = process_config_default_simple(&env.simple, bin_name, process->init->priority);
    config = process_config_mcp(config, process->init->priority);
    config = process_config_fault_cptr(config, badged_ep_path.capPtr);

    /* Set up rumprun process */
    error = sel4utils_configure_process_custom(&process->process, &env.vka, &env.vspace, config);
    ZF_LOGF_IF(error, "Failed to configure process");

    /* set up init_data process info */
    process->init->stack_pages = CONFIG_SEL4UTILS_STACK_SIZE / PAGE_SIZE_4K;
    process->init->stack = process->process.thread.stack_top - CONFIG_SEL4UTILS_STACK_SIZE;

#ifdef CONFIG_IOMMU
    process->init->io_space = sel4utils_copy_cap_to_process(&process->process, &env.vka, simple_get_init_cap(&env.simple, seL4_CapIOSpace));
#endif /* CONFIG_IOMMU */
#ifdef CONFIG_ARM_SMMU
    process->init->io_space_caps = arch_copy_iospace_caps_to_process(&process->process, &env);
#endif
    cspacepath_t path;
    vka_cspace_make_path(&env.vka, simple_get_irq_ctrl(&env.simple), &path);
    process->init->irq_control = sel4utils_move_cap_to_process(&process->process, path, NULL);
    process->init->sched_control = sel4utils_copy_cap_to_process(&process->process, &env.vka, simple_get_sched_ctrl(&env.simple, 0));
    process->init->timer_signal = sel4utils_copy_cap_to_process(&process->process, &env.vka,
                                                                        process->timer_signal.cptr);

    arch_copy_IOPort_cap(process->init, &env, &process->process);

    /* setup data about untypeds */
    alloc_untypeds(process);
    alloc_devices(process);
    copy_untypeds_to_process(process);
    process->init->tsc_freq = simple_get_arch_info(&env.simple);

    /* copy the rpc endpoint - we wait on the endpoint for a message
     * or a fault to see when the process finishes */
    process->init->rpc_ep = sel4utils_copy_path_to_process(&process->process, badged_ep_path);
    ZF_LOGF_IF(process->init->rpc_ep == 0, "Failed to copy rpc ep to process");

    /* set up a serial server ep */
    process->init->serial_ep = serial_server_parent_mint_endpoint_to_process(&process->process);
    ZF_LOGF_IF(process->init->serial_ep == 0, "Failed to copy rpc serial ep to process");

    /* allocate an EP just for this process which we use to send the init data */
    vka_alloc_endpoint(&env.vka, &process->init_ep_obj);
    seL4_CPtr init_ep = sel4utils_copy_cap_to_process(&process->process, &env.vka, process->init_ep_obj.cptr);

    /* WARNING: DO NOT COPY MORE CAPS TO THE PROCESS BEYOND THIS POINT,
     * AS THE SLOTS WILL BE CONSIDERED FREE AND OVERRIDDEN BY THE PROCESS. */
    /* set up free slot range */
    process->init->cspace_size_bits = CONFIG_SEL4UTILS_CSPACE_SIZE_BITS;
    process->init->free_slots.start = init_ep + 1;
    process->init->free_slots.end = BIT(CONFIG_SEL4UTILS_CSPACE_SIZE_BITS);
    assert(process->init->free_slots.start < process->init->free_slots.end);
    strncpy(process->init->cmdline, RUMPCONFIG, RUMP_CONFIG_MAX);
    NAME_THREAD(process->process.thread.tcb.cptr, bin_name);

    /* set up args for the process */
    char endpoint_string[WORD_STRING_SIZE];
    char *argv[] = {(char *)bin_name, endpoint_string};
    snprintf(endpoint_string, WORD_STRING_SIZE, "%lu", (unsigned long) init_ep);
    /* spawn the process */
    error = sel4utils_spawn_process_v(&process->process, &env.vka, &env.vspace,
                                      ARRAY_SIZE(argv), argv, 1);
    assert(error == 0);
    printf("process spawned\n");
    /* send env.init_data to the new process */
    send_init_data(&env, process->init_ep_obj.cptr, process);

}

static int timer_callback(uintptr_t id)
{
    assert(id < N_RUMP_PROCESSES);
    /* wake up the client */
    seL4_Signal(process_from_id(id)->timer_signal.cptr);
    return 0;
}

static seL4_MessageInfo_t handle_timer_rpc(rump_process_t *process, int id, seL4_MessageInfo_t info)
{
    seL4_Word op = seL4_GetMR(0);
    uint64_t time = 0;
    seL4_Word error = 0;

    switch (op) {
    case SET_TIMEOUT:
        time = sel4utils_64_get_mr(2);
        timeout_type_t type = seL4_GetMR(1);
        ZF_LOGF_IF(seL4_MessageInfo_get_length(info) != 2 + SEL4UTILS_64_WORDS, "invalid message length");
        error = tm_register_cb(&env.time_manager, type, time, 0, id, timer_callback, id);
        ZF_LOGF_IF(error, "Failed to set timeout");
        info = seL4_MessageInfo_new(0, 0, 0, 1);
        break;

    case GET_TIME:
        error = tm_get_time(&env.time_manager, &time);
        ZF_LOGF_IF(error, "Failed to get time!");
        sel4utils_64_set_mr(1, time);
        info = seL4_MessageInfo_new(0, 0, 0, 1 + SEL4UTILS_64_WORDS);
        break;
    default:
        ZF_LOGF("Unknown timer operation");
    }

    return info;
}

/* Boot Rumprun process. */
int
run_rr(void)
{
    struct cpio_info info2;
    cpio_info(_cpio_archive, &info2);
    const char* bin_name;
    for (int i = 0; i < info2.file_count; i++) {
        unsigned long size;
        cpio_get_entry(_cpio_archive, i, &bin_name, &size);
        ZF_LOGV("name %d: %s\n", i, bin_name);
    }

    char* a = RUMPCONFIG;
    printf("%zd %s\n", sizeof(RUMPCONFIG), a);
    /* Test intro banner. */
    printf("  starting app\n");

    launch_process(bin_name, 1);

    /* wait on it to finish, rpc or fault, report result */
    seL4_Word result = 0;
    bool reply = false;
    int error = 0;
    seL4_MessageInfo_t info;

    while (result == 0) {
        seL4_Word badge = 0;

        if (reply) {
            seL4_SetMR(0, error);
            info = api_reply_recv(env.ep.cptr, info, &badge, env.reply_obj.cptr);
            reply = false;
        } else {
            info = api_recv(env.ep.cptr, &badge, env.reply_obj.cptr);
        }

        seL4_Word label = seL4_MessageInfo_get_label(info);
        if (badge > 0 && badge <= N_RUMP_PROCESSES) {
            reply = true;
            rump_process_t *rump_process = process_from_id(badge);
            if (label == TIMER_LABEL) {
                info = handle_timer_rpc(rump_process, badge, info);
            } else if (label != seL4_Fault_NullFault) {
                /* it's a fault */
                sel4utils_print_fault_message(info, "rumprun");
                sel4debug_dump_registers(rump_process->process.thread.tcb.cptr);
                result = -1;
            } else {
                ZF_LOGE("unknown message\n");
                result = -1;
            }
        } else {
           /* it's an irq */
            sel4platsupport_handle_timer_irq(&env.timer, badge);
            error = tm_update(&env.time_manager);
            ZF_LOGF_IF(error, "failed to update time manager");
        }
    }
    return result;
}

static int
create_thread_handler(sel4utils_thread_entry_fn handler, int priority, int UNUSED timeslice)
{
    sel4utils_thread_config_t thread_config = thread_config_default(&env.simple,
        seL4_CapInitThreadCNode, seL4_NilData, seL4_CapNull, priority);
    if(config_set(CONFIG_KERNEL_RT)) {
        thread_config.sched_params = sched_params_periodic(thread_config.sched_params, &env.simple,
                        0, CONFIG_BOOT_THREAD_TIME_SLICE * US_IN_MS,
                        timeslice * CONFIG_BOOT_THREAD_TIME_SLICE * (US_IN_MS /100),
                        0, 0);
    }
    sel4utils_thread_t new_thread;
    int error = sel4utils_configure_thread_config(&env.vka, &env.vspace, &env.vspace, thread_config, &new_thread);
    if (error) {
        return error;
    }
    error = sel4utils_start_thread(&new_thread, handler, NULL, NULL,
                                   1);
    return error;
}

#ifdef CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER
void *log_buffer;
#endif

void *main_continued(void *arg UNUSED)
{
    /* Print welcome banner. */
    printf("\n");
    printf("Rump on seL4\n");
    printf("============\n");
    printf("\n");

#ifdef CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER
    /* Create 1MB page to use for benchmarking and give to kernel */
    log_buffer = vspace_new_pages(&env.vspace, seL4_AllRights, 1, seL4_LargePageBits);
    ZF_LOGF_IF(log_buffer == NULL, "Could not map 1MB page");
    seL4_CPtr buffer_cap = vspace_get_cap(&env.vspace, log_buffer);
    ZF_LOGF_IF(buffer_cap == NULL, "Could not get cap");
    int res_buf = seL4_BenchmarkSetLogBuffer(buffer_cap);
    ZF_LOGF_IFERR(res_buf, "Could not set log buffer");
#endif //CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER

    int error = vka_alloc_endpoint(&env.vka, &env.ep);
    ZF_LOGF_IF(error, "Failed to allocate endpoint");

    /* allocate reply object */
    if (config_set(CONFIG_KERNEL_RT)) {
        error = vka_alloc_reply(&env.vka, &env.reply_obj);
        ZF_LOGF_IF(error, "Failed to allocate reply object");
    }

    /* create a notification for the timer */
    error = vka_alloc_notification(&env.vka, &env.timer_ntfn);
    ZF_LOGF_IF(error, "Failed to allocate timer notification");

    /* start the serial server thread */
    error = serial_server_parent_spawn_thread(&env.simple, &env.vka, &env.vspace, seL4_MaxPrio - 1);
    ZF_LOGF_IF(error, "Failed to spawn serial server thread");

    /* get the caps we need to set up a timer and serial interrupts */
    sel4platsupport_init_default_serial_caps(&env.vka, &env.vspace, &env.simple, &env.serial_objects);


    ps_io_ops_t ops = {0};
    error = sel4platsupport_new_io_ops(env.vspace, env.vka, &ops);
    ZF_LOGF_IF(error, "Failed to init io ops");

    error = sel4platsupport_new_arch_ops(&ops, &env.simple);
    ZF_LOGF_IF(error, "Failed to init arch ops");

    error = sel4platsupport_init_default_timer_ops(&env.vka, &env.vspace, &env.simple, ops,
                                                   env.timer_ntfn.cptr, &env.timer);
    ZF_LOGF_IF(error, "Failed init default timer");

    error = seL4_TCB_BindNotification(simple_get_tcb(&env.simple), env.timer_ntfn.cptr);
    ZF_LOGF_IF(error, "Failed to bind timer notification and endpoint\n");

    error = tm_init(&env.time_manager, &env.timer.ltimer, &ops, N_RUMP_PROCESSES);
    ZF_LOGF_IF(error, "Failed to init time manager");

    int err = seL4_TCB_SetPriority(simple_get_tcb(&env.simple), seL4_MaxPrio);
    ZF_LOGF_IFERR(err, "seL4_TCB_SetPriority thread failed");
    /* Create serial thread */
    err = create_thread_handler(serial_interrupt, seL4_MaxPrio, 100);
    ZF_LOGF_IF(err, "Could not create serial thread");
    /* Create idle thread */
    err = create_thread_handler(count_idle, 0, 100);
    ZF_LOGF_IF(err, "Could not create idle thread");

    if (config_set(CONFIG_USE_HOG_THREAD)) {
        /* Create hog thread */
        err = create_thread_handler(hog_thread, seL4_MaxPrio - 1, CONFIG_HOG_BANDWIDTH);
        ZF_LOGF_IF(err, "Could not create hog thread thread");
    }

    /* now set up and run rumprun */
    run_rr();

    return NULL;
}

/* entry point of root task */
int main(void)
{
    seL4_BootInfo *info;
    info = platsupport_get_bootinfo();

    NAME_THREAD(seL4_CapInitThreadTCB, "roottask");

    /* Check rump kernel config string length */
    compile_time_assert(rump_config_is_too_long, sizeof(RUMPCONFIG) < RUMP_CONFIG_MAX);

    /* initialise libsel4simple, which abstracts away which kernel version
     * we are running on */
    simple_default_init_bootinfo(&env.simple, info);

    /* initialise the environment - allocator, cspace manager, vspace manager, timer */
    init_env(&env);

    /* enable serial driver */
    platsupport_serial_setup_simple(NULL, &env.simple, &env.vka);

    /* switch to a bigger, safer stack with a guard page
     * before starting Rumprun, resume on main_continued() */
    ZF_LOGI("Switching to a safer, bigger stack... ");
    fflush(stdout);
    void *res;
    sel4utils_run_on_stack(&env.vspace, main_continued, NULL, &res);

    return 0;

}
