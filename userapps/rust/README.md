<!--
     Copyright 2017, Data61
     Commonwealth Scientific and Industrial Research Organisation (CSIRO)
     ABN 41 687 119 230.

     This software may be distributed and modified according to the terms of
     the BSD 2-Clause license. Note that NO WARRANTY is provided.
     See "LICENSE_BSD2.txt" for details.

     @TAG(DATA61_BSD)
-->
# Rust

## Installation steps
```
curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain nightly # this installs rust globally
rustup target add x86_64-rumprun-netbsd
mkdir build-ninja && cd build-ninja
../init-build.sh -DPLATFORM=x86_64 -DAPP=nginx -DSIMULATION=ON
ninja
./simulate
```
