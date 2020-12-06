/*
 * Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#pragma once

#include <autoconf.h>
#include <stdint.h>
#include <stddef.h>
#define MAX_DEVICE_NAME 20

struct mmio {
    uintptr_t paddr;
    size_t size_bits;
};

struct pci_addr {
    uint32_t bus;
    uint32_t dev;
    uint32_t function;
};

typedef struct device {
    char name[MAX_DEVICE_NAME];
    int irq_num;
    int num_mmios;
    struct mmio *mmios;
    struct pci_addr pci;
} device_t;


#ifndef CONFIG_RUMPRUN_NETWORK_IFNAME
#define CONFIG_RUMPRUN_NETWORK_IFNAME ""
#endif

#ifdef CONFIG_RUMPRUN_USE_PCI_ETHERNET
#ifdef CONFIG_RUMPRUN_STATIC
#define SUB_NETWORK "\"static\", \"addr\":\""CONFIG_RUMPRUN_NETWORK_STATIC_IP"\", \"mask\": \"24\","
#else
#define SUB_NETWORK "\"dhcp\","
#endif
#define NETWORK "\"net\" : {,\"if\":\""CONFIG_RUMPRUN_NETWORK_IFNAME"\", \"type\":\"inet\",\"method\": "SUB_NETWORK"}"
#else
#define NETWORK ""
#endif

#define RUMP_ENV_VARS "\"env\": \""CONFIG_RUMPRUN_ENV_STRING"\""
#define RUMP_CMDLINE_FMT "\"cmdline\": \"%s\""

#define RUMPCONFIG "{"RUMP_ENV_VARS", "NETWORK", "CONFIG_RUMPRUN_EXTRA_CONFIG", "RUMP_CMDLINE_FMT",},"

int get_num_devices(void);
device_t *get_devices(void);
