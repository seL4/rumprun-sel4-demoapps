<!--
     Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)

     SPDX-License-Identifier: BSD-2-Clause
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
