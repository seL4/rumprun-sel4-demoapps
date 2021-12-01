<!--
     Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)

     SPDX-License-Identifier: CC-BY-SA-4.0
-->

# Rumprun apps

[![CI](https://github.com/seL4/rumprun-sel4-demoapps/actions/workflows/push.yml/badge.svg)](https://github.com/seL4/rumprun-sel4-demoapps/actions/workflows/push.yml)
[![Hello World](https://github.com/seL4/rumprun-sel4-demoapps/actions/workflows/rump-deploy.yml/badge.svg)](https://github.com/seL4/rumprun-sel4-demoapps/actions/workflows/rump-deploy.yml)

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
* [susan](userapps/susan/README.md)
* [jpeg](userapps/jpeg/README.md)

## Apps that are currently broken
* [erlang](userapps/erlang/README.md): Currently isn't supported due to a restriction in how hardware resources
  can be given to an instance of a rump kernel. There isn't currently any timeframe for a fix.
* [madplay](userapps/madplay/README.md): Doesn't currently build due to autoconf issues

# Dependencies

* https://docs.sel4.systems/projects/buildsystem/host-dependencies.html
* google repo tool


# Installation steps
```
repo init -u https://github.com/seL4/rumprun-sel4-demoapps
repo sync
(cd projects/rumprun && ./init-sources.sh)
mkdir build-hello && cd build-hello
../init-build.sh -DPLATFORM=x86_64 -DSIMULATION=TRUE -DAPP=hello
ninja
```


# Running the image

## QEMU

You can run by running a generated simulate script:
```
./simulate
# quit with Ctrl-A X
```
To add QEMU networking you will also need: `-net nic,model=e1000 -net tap,script=no,ifname=tap0` which requires
a tap network interface configured.

See https://docs.sel4.systems/Hardware/IA32 for running on actual hardware
