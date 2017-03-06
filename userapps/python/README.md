# Python3

This is currently not supported on seL4 due to the requirements for mounting a large amount of Python3
libraries in the filesystem somewhere.  On the other Rumprun targets this is achieved using Virtio block
device drivers and QEMU.  Using this on seL4 would require changing the rumprun-bake configuration to use `sel4_virtio` (this
    can be found in `apps/rumpcommon.mk`)
and would only run in QEMU.  The [README.md in python3](https://github.com/rumpkernel/rumprun-packages/tree/master/python3) explains
how one would do this.
