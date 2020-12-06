/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <utils/arith.h>
#include <roottask_platform_config/config.h>


struct mmio mmio0s[] = {{0xfebc0000, 17}};

static device_t devices[] = {{"wm0", 11, ARRAY_SIZE(mmio0s), mmio0s, {0, 3, 0}}};

device_t *get_devices(void)
{
    return devices;
}

int get_num_devices(void)
{
    return ARRAY_SIZE(devices);
}
