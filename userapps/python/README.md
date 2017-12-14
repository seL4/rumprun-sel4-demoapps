<!--
     Copyright 2017, Data61
     Commonwealth Scientific and Industrial Research Organisation (CSIRO)
     ABN 41 687 119 230.

     This software may be distributed and modified according to the terms of
     the BSD 2-Clause license. Note that NO WARRANTY is provided.
     See "LICENSE_BSD2.txt" for details.

     @TAG(DATA61_BSD)
-->
# Python3

This is currently not supported on seL4 due to the requirements for mounting a large amount of Python3
libraries in the filesystem somewhere.  On the other Rumprun targets this is achieved using Virtio block
device drivers and QEMU.  Using this on seL4 would require changing the rumprun-bake configuration to use `sel4_virtio` 
from `sel4_generic` in this app's Kbuild 
and would only run in QEMU.  The [README.md in python3](https://github.com/rumpkernel/rumprun-packages/tree/master/python3) explains
how one would do this.
