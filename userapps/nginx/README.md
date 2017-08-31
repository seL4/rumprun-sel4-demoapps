<!--
     Copyright 2017, Data61
     Commonwealth Scientific and Industrial Research Organisation (CSIRO)
     ABN 41 687 119 230.

     This software may be distributed and modified according to the terms of
     the BSD 2-Clause license. Note that NO WARRANTY is provided.
     See "LICENSE_BSD2.txt" for details.

     @TAG(DATA61_BSD)
-->
# NGINX on Rumprun on seL4

A tutorial exists for running NGINX on rumprun [here](https://github.com/rumpkernel/wiki/wiki/Tutorial%3A-Serve-a-static-website-as-a-Unikernel).
Extra configuration is requried for seL4.


## Compilation steps
```
make nginx-ia32-x86_64_qemu_defconfig
make
```
