# Rumprun apps

This repository contains several apps for running with the rumprun unikernel with seL4 as the target platform.  
It also conatins a seL4 root-task that creates and initialises a rumprun instance as a separate process.
Rumprun images are generally constructed by building the rumprun sources, compiling an app with the rumprun
toolchains, baking the app using `rumprun-bake` which links in the rumprun and rumpkernel libraries, and finally
running the app via `rumprun`.  The apps in this repo are constructed in mostly the same way, only the commands
have been integrated with the kbuild based seL4 build system so that running make will eventually construct a
seL4 kernel image and potentially root-task user image containing the rumprun unikernel.  These images can then
be run the same as any other seL4 binaries.

# Dependencies
* https://github.com/SEL4PROJ/sel4-tutorials/blob/master/Prerequisites.md
* google repo tool

For running benchmarks:
* Rust
* Python

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
If internally at Data61:
Setup https://confluence.csiro.au/display/RGPSST/Machine+Queue
```
make mqrun
```
This will run it on the sandy machine
