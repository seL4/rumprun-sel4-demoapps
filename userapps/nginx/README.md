# NGINX on Rumprun on seL4

A tutorial exists for running NGINX on rumprun [here](https://github.com/rumpkernel/wiki/wiki/Tutorial%3A-Serve-a-static-website-as-a-Unikernel).
Extra configuration is requried for seL4.


## Compilation steps
```
make nginx-ia32-x86_64_qemu_defconfig
make
```
