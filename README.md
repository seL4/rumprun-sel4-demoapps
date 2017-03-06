# Rumprun apps

This repository contains several apps for running with the rumprun unikernel with seL4 as the target platform.  It also conatins a seL4 root-task that creates and initialises a rumprun instance as a separate process.
Rumprun images are generally constructed by building the rumprun sources, compiling an app with the rumprun
toolchains, baking the app using `rumprun-bake` which links in the rumprun and rumpkernel libraries, and finally
running the app via `rumprun`.  The apps in this repo are constructed in mostly the same way, only the commands
have been integrated with the kbuild based seL4 build system so that running make will eventually construct a
seL4 kernel image and potentially root-task user image containing the rumprun unikernel.  These images can then
be run the same as any other seL4 binaries.

[Blogpost](https://research.csiro.au/tsblog/using-rump-kernels-to-run-unmodified-netbsd-drivers-on-sel4/)

# Currently supported apps
Where supported means you can select the config, build the project and then run the project.
* helloworld-rumprun-ia32_defconfig
* helloworld-rumprun-x64_defconfig
* redis-rumprun-ia32_defconfig
* [rust-rumprun-x64_defconfig](userapps/rust/README.md)

## Partially supported apps
Where something is required after applying the config to make the app work.
* [nginx-rumprun-ia32_defconfig](userapps/nginx/README.md)
* [iperf-rumprun-ia32_defconfig](userapps/iperf3/README.md)

## Apps that are currently broken
* [Python](userapps/python/README.md)

# Dependencies
* https://github.com/SEL4PROJ/sel4-tutorials/blob/master/Prerequisites.md
* google repo tool


# Installation steps
```
repo init -u $(GIT URL TO MANIFEST)
repo sync
source scripts/init-all.sh
make helloworld-rumprun-ia32_defconfig
make
```

Making a bmk image (not seL4)
```
make RTARGET=hw
```

# Running the image
https://wiki.sel4.systems/Hardware/IA32

QEMU:
```
qemu-system-i386 -m 512 -nographic -kernel images/kernel-ia32-pc99 -initrd images/roottask-image-ia32-pc99
# quit with Ctrl-A X
```
