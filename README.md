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
make iperf-rumprun-ia32_defconfig
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
