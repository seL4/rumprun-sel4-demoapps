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

#include <utils/arith.h>
#include <roottask_platform_config/config.h>


struct mmio mmio0s[] = {{0xf7f00000, 17}, {0xf7f39000, 12}};
struct mmio mmio1s[] = {{0xf7dc0000, 17}};

device_t devices[] = {{"wm0", 20, ARRAY_SIZE(mmio0s), mmio0s, {0,25,0}},
                      {"wm1", 16, ARRAY_SIZE(mmio1s), mmio1s, {3,0,0}}};
int num_devices = ARRAY_SIZE(devices);
