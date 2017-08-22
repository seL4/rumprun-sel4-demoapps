# erlang on BEAM on Rumprun on seL4 (on QEMU)

This app is a wrapper around the demo erlang application located in rumprun-packages.  It builds BEAM and
an erlang echo server to run on Rumprun.

For more info see the readme here: https://github.com/rumpkernel/rumprun-packages/tree/master/erlang
(Alternatively there is a local copy at projects/rumprun-packages/erlang/README.md)

## Compilation steps
```bash
$ make erlang-x86_64-x86_64_qemu_defconfig
$ make # Maybe add a -j8 to speed this up.  It may take a while
```

## To run

You will need to set up a tap interface:
```bash
  $ sudo ip tuntap add tap0 mode tap
  $ sudo ip addr add 10.0.120.100/24 dev tap0
  $ sudo ip link set dev tap0 up
```
```bash
$ qemu-system-x86_64 -m 3072 -net nic,model=e1000 -net tap,script=no,ifname=tap0 -kernel images/kernel-x86_64-pc99 -initrd images/roottask-image-x86_64-pc99 -cpu Haswell -nographic -drive if=virtio,file=projects/rumprun-packages/erlang/images/erlang.iso,format=raw -drive if=virtio,file=projects/rumprun-packages/erlang/examples/app.iso,format=raw
# Eventually you will see:
=== calling "bin/beam" main() ===

rumprun: call to ``sigaction'' ignored
rumprun: call to ``_sys___sigprocmask14'' ignored
Echo server started
```

To connect to the echo server:
```bash
$ nc 10.0.120.101 8080
hello
hello
```
