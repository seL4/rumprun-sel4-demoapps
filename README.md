<!--
     Copyright 2017, Data61
     Commonwealth Scientific and Industrial Research Organisation (CSIRO)
     ABN 41 687 119 230.

     This software may be distributed and modified according to the terms of
     the BSD 2-Clause license. Note that NO WARRANTY is provided.
     See "LICENSE_BSD2.txt" for details.

     @TAG(DATA61_BSD)
-->
# Rumprun apps

This repository contains several apps for running with the rumprun unikernel with seL4 as the target platform.  It also conatins a seL4 root-task that creates and initialises a rumprun instance as a separate process.
Rumprun images are generally constructed by building the rumprun sources, compiling an app with the rumprun
toolchains, baking the app using `rumprun-bake` which links in the rumprun and rumpkernel libraries, and finally
running the app via `rumprun`.  The apps in this repo are constructed in mostly the same way, only the commands
have been integrated with the kbuild based seL4 build system so that running make will eventually construct a
seL4 kernel image and potentially root-task user image containing the rumprun unikernel.  These images can then
be run the same as any other seL4 binaries.

[Blogpost](https://research.csiro.au/tsblog/using-rump-kernels-to-run-unmodified-netbsd-drivers-on-sel4/)

# Configurations

Configurations can be found in the configs folder.  A configuration filename follows the pattern
`$(appname)-$(seL4 architecture)-$(platform config)_defconfig`.  The configuration file contains
built in command line arguments, networking configuration and certain memory configurations.  These
can be changed by applying a config, and then editing the values using the visual ncurses menuconfig tool, or
editing the .config file directly.

# Currently supported apps
(This list may be out of date.  For a more up to date list check the configs folder)
* hello
* iperf3
* leveldb
* memcached
* multicore_pthread
* nginx
* redis
* rust
* [erlang](userapps/erlang/README.md)

## Apps that are currently broken
* [Python](userapps/python/README.md)
* lmbench
* netserver

# Dependencies
* https://github.com/SEL4PROJ/sel4-tutorials/blob/master/Prerequisites.md
* google repo tool


# Installation steps
```
repo init -u $(GIT URL TO MANIFEST)
repo sync
source scripts/init-all.sh
make hello-x86_64-x86_64_qemu_defconfig
make
```


# Running the image
https://wiki.sel4.systems/Hardware/IA32

QEMU:
You can run by running `make simulate` or more specifically: 
```
qemu-system-x86_64 \
		-m 512 -nographic  -kernel images/kernel-$(SEL4_ARCH)-$(PLAT) \
		-initrd images/roottask-image-$(SEL4_ARCH)-$(PLAT) -cpu Haswell
# quit with Ctrl-A X
```
To add QEMU networking you will also need: `-net nic,model=e1000 -net tap,script=no,ifname=tap0` which requires
a tap network interface configured.