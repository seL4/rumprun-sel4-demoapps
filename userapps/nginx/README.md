<!--
     Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)

     SPDX-License-Identifier: BSD-2-Clause
-->
# NGINX on Rumprun on seL4

A tutorial exists for running NGINX on rumprun [here](https://github.com/rumpkernel/wiki/wiki/Tutorial%3A-Serve-a-static-website-as-a-Unikernel).
Extra configuration is requried for seL4.


## Compilation steps
```
mkdir build-ninja && cd build-ninja
../init-build.sh -DPLATFORM=x86_64 -DAPP=nginx
ninja
```
