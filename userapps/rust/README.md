# Rust

## Installation steps
```
curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain nightly # this installs rust globally
rustup target add x86_64-rumprun-netbsd
make clean
make rust-x86_64-x86_64_qemu_defconfig
make
qemu-system-x86_64 m 512 -nographic -kernel images/kernel-x86_64-pc99 -initrd images/roottask-image-x86_64-pc99 -cpu Haswell
```
