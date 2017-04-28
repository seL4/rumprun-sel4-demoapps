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


/* Include Kconfig variables. */
#include <autoconf.h>
#include <allocman/bootstrap.h>
#include <allocman/vka.h>
#include <simple/simple.h>
#include <simple-default/simple-default.h>
#include <sel4platsupport/platsupport.h>
#include <sel4platsupport/plat/timer.h>
#include <sel4platsupport/serial.h>
#include <sel4platsupport/arch/io.h>
#include <sel4debug/register_dump.h>
#include <cpio/cpio.h>
#include <sel4/bootinfo.h>
#include <sel4utils/stack.h>
#include <vka/object.h>
#include <platsupport/plat/pit.h>
#include <platsupport/io.h>
#include <vka/object_capops.h>

#include <vspace/vspace.h>
#include <cmdline.h>
#include "common.h"
#include <rumprun/init_data.h>

// #define TESTS_APP RUMPAPPNAME
#define SUCCESS true
#define FAILURE false

/* ammount of untyped memory to reserve for the driver (32mb) */
#define DRIVER_UNTYPED_MEMORY (1 << 25)
/* Number of untypeds to try and use to allocate the driver memory.
 * if we cannot get 32mb with 16 untypeds then something is probably wrong */
#define DRIVER_NUM_UNTYPEDS 16


/* dimensions of virtual memory for the allocator to use */
#define ALLOCATOR_VIRTUAL_POOL_SIZE ((1 << seL4_PageBits) * 1000)

/* static memory for the allocator to bootstrap with */
#define ALLOCATOR_STATIC_POOL_SIZE ((1 << seL4_PageBits) * 20)
static char allocator_mem_pool[ALLOCATOR_STATIC_POOL_SIZE];

/* static memory for virtual memory bootstrapping */
static sel4utils_alloc_data_t data;


/* environment encapsulating allocation interfaces etc */
struct env env;

/* the number of untyped objects we have to give out to processes */
static int num_untypeds;
seL4_BootInfo *info;
/* list of untypeds to give out to test processes */
static vka_object_t untypeds[CONFIG_MAX_NUM_BOOTINFO_UNTYPED_CAPS];
/* list of sizes (in bits) corresponding to untyped */
static uint8_t untyped_size_bits_list[CONFIG_MAX_NUM_BOOTINFO_UNTYPED_CAPS];
static uintptr_t untyped_paddr_list[CONFIG_MAX_NUM_BOOTINFO_UNTYPED_CAPS];


seL4_SlotRegion devices;
uint8_t device_size_bits_list[CONFIG_MAX_NUM_BOOTINFO_UNTYPED_CAPS];
uintptr_t device_paddr_list[CONFIG_MAX_NUM_BOOTINFO_UNTYPED_CAPS];

extern vspace_t *muslc_this_vspace;
extern reservation_t muslc_brk_reservation;
extern void *muslc_brk_reservation_start;
/* initialise our runtime environment */
static void
init_env(env_t env)
{
    allocman_t *allocman;
    reservation_t virtual_reservation;
    int error;

    /* create an allocator */
    allocman = bootstrap_use_current_simple(&env->simple, ALLOCATOR_STATIC_POOL_SIZE, allocator_mem_pool);
    if (allocman == NULL) {
        ZF_LOGF("Failed to create allocman");
    }

    /* create a vka (interface for interacting with the underlying allocator) */
    allocman_make_vka(&env->vka, allocman);

    /* create a vspace (virtual memory management interface). We pass
     * boot info not because it will use capabilities from it, but so
     * it knows the address and will add it as a reserved region */
    error = sel4utils_bootstrap_vspace_with_bootinfo_leaky(&env->vspace,
                                                           &data, simple_get_pd(&env->simple),
                                                           &env->vka, platsupport_get_bootinfo());
    if (error) {
        ZF_LOGF("Failed to bootstrap vspace");
    }

    sel4utils_res_t muslc_brk_reservation_memory;
    error = sel4utils_reserve_range_no_alloc(&env->vspace, &muslc_brk_reservation_memory, 1048576, seL4_AllRights, 1, &muslc_brk_reservation_start);
    if (error) {
        ZF_LOGF("Failed to reserve_range");
    }
    muslc_this_vspace = &env->vspace;
    muslc_brk_reservation.res = &muslc_brk_reservation_memory;

    /* fill the allocator with virtual memory */
    void *vaddr;
    virtual_reservation = vspace_reserve_range(&env->vspace,
                                               ALLOCATOR_VIRTUAL_POOL_SIZE, seL4_AllRights, 1, &vaddr);
    if (virtual_reservation.res == 0) {
        ZF_LOGF("Failed to provide virtual memory for allocator");
    }

    bootstrap_configure_virtual_pool(allocman, vaddr,
                                     ALLOCATOR_VIRTUAL_POOL_SIZE, simple_get_pd(&env->simple));
}


/* Free a list of objects */
static void
free_objects(vka_object_t *objects, unsigned int num)
{
    for (unsigned int i = 0; i < num; i++) {
        vka_free_object(&env.vka, &objects[i]);
    }
}

/* Allocate untypeds till either a certain number of bytes is allocated
 * or a certain number of untyped objects */
static unsigned int
allocate_untypeds(vka_object_t *untypeds, size_t bytes, unsigned int max_untypeds)
{
    unsigned int num_untypeds = 0;
    size_t allocated = 0;

    /* try to allocate as many of each possible untyped size as possible */
    for (uint8_t size_bits = seL4_WordBits - 1; size_bits > PAGE_BITS_4K; size_bits--) {
        /* keep allocating until we run out, or if allocating would
         * cause us to allocate too much memory*/
        while (num_untypeds < max_untypeds &&
                allocated + BIT(size_bits) <= bytes &&
                vka_alloc_untyped(&env.vka, size_bits, &untypeds[num_untypeds]) == 0) {
            allocated += BIT(size_bits);
            num_untypeds++;
        }
    }
    return num_untypeds;
}

/* extract a large number of untypeds from the allocator */
static unsigned int
populate_untypeds(vka_object_t *untypeds)
{
    /* First reserve some memory for the root task */
    vka_object_t reserve[DRIVER_NUM_UNTYPEDS];
    unsigned int reserve_num = allocate_untypeds(reserve, DRIVER_UNTYPED_MEMORY, DRIVER_NUM_UNTYPEDS);

    /* Now allocate everything else for rumprun */
    unsigned int num_untypeds = allocate_untypeds(untypeds, UINT_MAX, ARRAY_SIZE(untyped_size_bits_list));

    /* Fill out the size_bits list */
    uint32_t total_mem = 0;
    for (unsigned int i = 0; i < num_untypeds; i++) {
        untyped_size_bits_list[i] = untypeds[i].size_bits;
        total_mem += BIT(untypeds[i].size_bits);
        untyped_paddr_list[i] = vka_utspace_paddr(&env.vka, untypeds[i].ut, seL4_UntypedObject, untypeds[i].size_bits);
    }
    ZF_LOGI("Totalsize: 0x%"PRIx32", im mb: %zd\n", total_mem, BYTES_TO_SIZE_BITS_PAGES(total_mem, 20));
    /* Return reserve memory */
    free_objects(reserve, reserve_num);

    /* Return number of untypeds for tests */
    if (num_untypeds == 0) {
        ZF_LOGF("No untypeds for rump!");
    }
    return num_untypeds;
}

/* copy a cap to a process, returning the cptr in the process' cspace
        XXX: This currently copies the code directly out of libsel4utils
        but doesn't free the capslot because the vka isn't tracking the
        slots from the bootinfo.  Should probably find a way to add those
        slots so that they are tracked in order to call the proper sel4utils
        move cap to process function */
seL4_CPtr
move_cap_to_process(sel4utils_process_t *process, seL4_CPtr cap)
{
    seL4_CPtr copied_cap;
    cspacepath_t path;

    vka_cspace_make_path(&env.vka, cap, &path);
    cspacepath_t dest = { 0 };
    if (process->cspace_next_free >= (BIT(process->cspace_size))) {
        ZF_LOGE("Can't allocate slot, cspace is full.\n");
        return -1;
    }

    dest.root = process->cspace.cptr;
    dest.capPtr = process->cspace_next_free;
    dest.capDepth = process->cspace_size;
    int error = vka_cnode_move(&dest, &path);
    if (error != seL4_NoError) {
        ZF_LOGE("Failed to move cap\n");
        return 0;
    }


    /* success */
    assert(process->cspace_next_free < (1 << process->cspace_size));
    process->cspace_next_free++;
    copied_cap = dest.capPtr;
    // copied_cap = sel4utils_move_cap_to_process(process, path, &env.vka);
    if (copied_cap == 0) {
        ZF_LOGF("Failed to copy cap to process");
    }

    return copied_cap;
}
/* copy untyped caps into a processes cspace, return the cap range they can be found in */
static seL4_SlotRegion
copy_untypeds_to_process(sel4utils_process_t *process, vka_object_t *untypeds, int num_untypeds)
{
    seL4_SlotRegion range = {0};

    for (int i = 0; i < num_untypeds; i++) {
        seL4_CPtr slot = sel4utils_copy_cap_to_process(process, &env.vka, untypeds[i].cptr);

        /* set up the cap range */
        if (i == 0) {
            range.start = slot;
        }
        range.end = slot;
    }
    assert((range.end - range.start) + 1 == num_untypeds);
    return range;
}
static void
copy_device_frames_to_process(sel4utils_process_t *process)
{
    // printf("total frames: %d, captype: %d\n", info->numDeviceRegions, seL4_DebugCapIdentify(info->deviceRegions[0].frames.start));
    int k = 0;
    uint32_t num_regions = 0;
    seL4_CPtr copied_cap = 0;
    for (int i = 0; i < simple_get_untyped_count(&env.simple); i++) {
        size_t size_bits;
        uintptr_t paddr;
        bool device;
        seL4_CPtr cap = simple_get_nth_untyped(&env.simple, i, &size_bits, &paddr, &device);
        if (!device) {
            continue;
        }
        device_size_bits_list[num_regions] = size_bits;
        device_paddr_list[num_regions] = paddr;

        printf("basepaddr 0x%x framesize: %d, \n", paddr, size_bits);
        cspacepath_t path;

        vka_cspace_make_path(&env.vka, cap, &path);
        copied_cap = sel4utils_copy_path_to_process(process, path);

        if (copied_cap == 0) {
            printf("Failed to copy cap to process");
        }
        if (num_regions == 0) {
            devices.start = copied_cap;
        }
        num_regions++;
        k++;

    }
    devices.end = copied_cap + 1 ;

    printf("copied caps:%d\n", k);

}


/* map the init data into the process, and send the address via ipc */
static void *
send_init_data(env_t env, seL4_CPtr endpoint, sel4utils_process_t *process)
{
    /* map the cap into remote vspace */
    void *remote_vaddr = vspace_map_pages(&process->vspace, env->init_frame_cap_copy, NULL, seL4_AllRights, 2, PAGE_BITS_4K, 1);
    assert(remote_vaddr != 0);

    /* now send a message telling the process what address the data is at */
    seL4_MessageInfo_t info = seL4_MessageInfo_new(seL4_Fault_NullFault, 0, 0, 1);
    seL4_SetMR(0, (seL4_Word) remote_vaddr);
    seL4_Send(endpoint, info);

    return remote_vaddr;
}

/* copy the caps required to set up the sel4platsupport default timer */
static void
copy_timer_caps(init_data_t *init, env_t env, sel4utils_process_t *test_process)
{
    /* irq cap for the timer irq */
    init->timer_irq = sel4utils_copy_cap_to_process(test_process, &env->vka, env->irq_path.capPtr);
    assert(init->timer_irq != 0);

    arch_copy_timer_caps(init, env, test_process);
}

extern char _cpio_archive[];
const char* myname;

/* Run a single test.
 * Each test is launched as its own process. */
int
run_rr(void)
{
    /* elf region data */
    int num_elf_regions;
    sel4utils_elf_region_t elf_regions[MAX_REGIONS];

    /* Unpack elf file from cpio */
    struct cpio_info info2;
    cpio_info(_cpio_archive, &info2);
    for (int i = 0; i < info2.file_count; i++) {
        unsigned long size;
        cpio_get_entry(_cpio_archive, i, &myname, &size);
        ZF_LOGV("name %d: %s\n", i, myname);
    }

    /* parse elf region data about the test image to pass to the tests app */
    num_elf_regions = sel4utils_elf_num_regions(myname);
    if (num_elf_regions >= MAX_REGIONS) {
        ZF_LOGF("Invalid num elf regions");
    }
    sel4utils_elf_reserve(NULL, myname, elf_regions);
    /* copy the region list for the process to clone itself */
    memcpy(env.init->elf_regions, elf_regions, sizeof(sel4utils_elf_region_t) * num_elf_regions);
    env.init->num_elf_regions = num_elf_regions;

    /* setup init priority.  Reduce by 2 so that we can have higher priority serial thread
        for benchmarking */
    env.init->priority = seL4_MaxPrio - 2;

    UNUSED int error;
    sel4utils_process_t test_process;
    char* a = RUMPCONFIG;
    printf("%d %s\n", sizeof(RUMPCONFIG), a);
    /* Test intro banner. */
    printf("  starting app\n");

    /* Set up rumprun process */
    error = sel4utils_configure_process(&test_process, &env.vka, &env.vspace,
                                        env.init->priority, myname);
    if (error) {
        ZF_LOGF("Failed to configure process");
    }

    /* set up init_data process info */
    env.init->stack_pages = CONFIG_SEL4UTILS_STACK_SIZE / PAGE_SIZE_4K;
    env.init->stack = test_process.thread.stack_top - CONFIG_SEL4UTILS_STACK_SIZE;
    env.init->page_directory = sel4utils_copy_cap_to_process(&test_process, &env.vka, test_process.pd.cptr);

    env.init->root_cnode = SEL4UTILS_CNODE_SLOT;
    env.init->tcb = sel4utils_copy_cap_to_process(&test_process, &env.vka, test_process.thread.tcb.cptr);
    env.init->domain = sel4utils_copy_cap_to_process(&test_process, &env.vka, simple_get_init_cap(&env.simple, seL4_CapDomain));
    env.init->asid_pool = sel4utils_copy_cap_to_process(&test_process, &env.vka, simple_get_init_cap(&env.simple, seL4_CapInitThreadASIDPool));
    env.init->asid_ctrl = sel4utils_copy_cap_to_process(&test_process, &env.vka, simple_get_init_cap(&env.simple, seL4_CapASIDControl));

#ifdef CONFIG_IOMMU
    env.init->io_space = sel4utils_copy_cap_to_process(&test_process, &env.vka, simple_get_init_cap(&env.simple, seL4_CapIOSpace));
#endif /* CONFIG_IOMMU */
#ifdef CONFIG_ARM_SMMU
    env.init->io_space_caps = arch_copy_iospace_caps_to_process(&test_process, &env);
#endif
    env.init->irq_control = move_cap_to_process(&test_process, simple_get_init_cap(&env.simple, seL4_CapIRQControl ));
    /* setup data about untypeds */
    env.init->untypeds = copy_untypeds_to_process(&test_process, untypeds, num_untypeds);
    copy_timer_caps(env.init, &env, &test_process);
    copy_device_frames_to_process(&test_process);
    memcpy(env.init->device_size_bits_list, device_size_bits_list, sizeof(uint8_t) * (devices.end - devices.start));
    memcpy(env.init->device_paddr_list, device_paddr_list, sizeof(uintptr_t) * (devices.end - devices.start));
    env.init->devices = devices;
    env.init->tsc_freq = simple_get_arch_info(&env.simple);
    /* copy the fault endpoint - we wait on the endpoint for a message
     * or a fault to see when the test finishes */
    seL4_CPtr endpoint = sel4utils_copy_cap_to_process(&test_process, &env.vka, test_process.fault_endpoint.cptr);
    printf("endpoint: %d\n", endpoint);

    /* WARNING: DO NOT COPY MORE CAPS TO THE PROCESS BEYOND THIS POINT,
     * AS THE SLOTS WILL BE CONSIDERED FREE AND OVERRIDDEN BY THE TEST PROCESS. */
    /* set up free slot range */
    env.init->cspace_size_bits = CONFIG_SEL4UTILS_CSPACE_SIZE_BITS;
    env.init->free_slots.start = endpoint + 1;
    env.init->free_slots.end = (1u << CONFIG_SEL4UTILS_CSPACE_SIZE_BITS);
    assert(env.init->free_slots.start < env.init->free_slots.end);
    /* copy test name */
    strncpy(env.init->name, "blah", 12);
    strncpy(env.init->cmdline, RUMPCONFIG, RUMP_CONFIG_MAX);
#ifdef SEL4_DEBUG_KERNEL
    seL4_DebugNameThread(test_process.thread.tcb.cptr, env.init->name);
#endif
    /* set up args for the test process */
    char endpoint_string[WORD_STRING_SIZE];
    char *argv[] = {(char *)myname, endpoint_string};
    snprintf(endpoint_string, WORD_STRING_SIZE, "%lu", (unsigned long)endpoint);
    /* spawn the process */
    error = sel4utils_spawn_process_v(&test_process, &env.vka, &env.vspace,
                                      ARRAY_SIZE(argv), argv, 1);
    assert(error == 0);
    printf("process spawned\n");
    /* send env.init_data to the new process */
    void *remote_vaddr = send_init_data(&env, test_process.fault_endpoint.cptr, &test_process);

    /* wait on it to finish or fault, report result */
    seL4_MessageInfo_t info = seL4_Recv(test_process.fault_endpoint.cptr, NULL);

    int result = seL4_GetMR(0);
    if (seL4_MessageInfo_get_label(info) != seL4_Fault_NullFault) {
        sel4utils_print_fault_message(info, "rumprun");
        sel4debug_dump_registers(test_process.thread.tcb.cptr);
        result = FAILURE;
    }

    /* unmap the env.init data frame */
    vspace_unmap_pages(&test_process.vspace, remote_vaddr, 2, PAGE_BITS_4K, NULL);

    /* reset all the untypeds for the next test */
    for (int i = 0; i < num_untypeds; i++) {
        cspacepath_t path;
        vka_cspace_make_path(&env.vka, untypeds[i].cptr, &path);
        vka_cnode_revoke(&path);
    }

    /* destroy the process */
    sel4utils_destroy_process(&test_process, &env.vka);

    return result;
}


int
create_thread_handler(sel4utils_thread_entry_fn handler, int priority)
{
    sel4utils_thread_t new_thread;

    int error = sel4utils_configure_thread(&env.vka, &env.vspace, &env.vspace, seL4_CapNull,
                                           priority, seL4_CapInitThreadCNode, seL4_NilData,
                                           &new_thread);
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
    if (log_buffer == NULL) {
        ZF_LOGF("Could not map 1MB page");
    }
    seL4_CPtr buffer_cap = vspace_get_cap(&env.vspace, log_buffer);
    if (buffer_cap == NULL) {
        ZF_LOGF("Could not get cap");
    }
    int res_buf = seL4_BenchmarkSetLogBuffer(buffer_cap);
    if (res_buf) {
        ZF_LOGF("Could not set log buffer");
    }
#endif //CONFIG_BENCHMARK_USE_KERNEL_LOG_BUFFER

    /* allocate lots of untyped memory for tests to use */
    num_untypeds = populate_untypeds(untypeds);

    /* create a frame that will act as the init data, we can then map
     * into target processes */
    env.init = (init_data_t *) vspace_new_pages(&env.vspace, seL4_AllRights, INIT_DATA_NUM_FRAMES, PAGE_BITS_4K);
    if (env.init == NULL) {
        ZF_LOGF("Could not create init_data frame");
    }

    for (int i = 0; i < INIT_DATA_NUM_FRAMES; i++) {
        cspacepath_t src, dest;
        vka_cspace_make_path(&env.vka, vspace_get_cap(&env.vspace, (((char *)env.init) + i * PAGE_SIZE_4K)), &src);

        int error = vka_cspace_alloc(&env.vka, &env.init_frame_cap_copy[i]);
        if (error) {
            ZF_LOGF("Failed to alloc cap");
        }
        vka_cspace_make_path(&env.vka, env.init_frame_cap_copy[i], &dest);
        error = vka_cnode_copy(&dest, &src, seL4_AllRights);
        if (error) {
            ZF_LOGF("Failed to copy cap");
        }
    }

    /* copy the untyped size bits and paddrs lists across to the init frame */
    memcpy(env.init->untyped_size_bits_list, untyped_size_bits_list, sizeof(uint8_t) * num_untypeds);
    memcpy(env.init->untyped_paddr_list, untyped_paddr_list, sizeof(uintptr_t) * num_untypeds);

    /* get the caps we need to set up a timer and serial interrupts */
    init_timer_caps(&env);
    sel4platsupport_init_default_serial_caps(&env.vka, &env.vspace, &env.simple, &env.serial_objects);

    /* Create serial thread */
    int err = create_thread_handler(serial_interrupt, seL4_MaxPrio);
    if (err) {
        ZF_LOGF("Could not create serial thread");
    }
    /* Create idle thread */
    err = create_thread_handler(count_idle, 0);
    if (err) {
        ZF_LOGF("Could not create idle thread");
    }

    /* now set up and run rumprun */
    run_rr();

    return NULL;
}

/* entry point of root task */
int main(void)
{
    info = platsupport_get_bootinfo();

#ifdef SEL4_DEBUG_KERNEL
    seL4_DebugNameThread(seL4_CapInitThreadTCB, "sel4test-driver");
#endif
    /* Check rump kernel config string length */
    compile_time_assert(rump_config_is_too_long, sizeof(RUMPCONFIG) < RUMP_CONFIG_MAX);
    /* We provide two pages to transfer init data */
    compile_time_assert(init_data_fits, sizeof(init_data_t) < (PAGE_SIZE_4K * INIT_DATA_NUM_FRAMES));
    /* initialise libsel4simple, which abstracts away which kernel version
     * we are running on */
    simple_default_init_bootinfo(&env.simple, info);

    /* initialise the test environment - allocator, cspace manager, vspace manager, timer */
    init_env(&env);

    /* enable serial driver */
    platsupport_serial_setup_simple(NULL, &env.simple, &env.vka);

    /* switch to a bigger, safer stack with a guard page
     * before starting the tests, resume on main_continued() */
    ZF_LOGI("Switching to a safer, bigger stack... ");
    fflush(stdout);
    void *res;
    sel4utils_run_on_stack(&env.vspace, main_continued, NULL, &res);

    return 0;

}
