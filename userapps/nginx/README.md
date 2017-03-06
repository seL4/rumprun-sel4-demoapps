# NGINX on Rumprun on seL4

A tutorial exists for running NGINX on rumprun [here](https://github.com/rumpkernel/wiki/wiki/Tutorial%3A-Serve-a-static-website-as-a-Unikernel).
Extra configuration is requried for seL4.

## Extra configuration:
NGINX requires a filesystem containing configuration files and the actual website artifacts to serve.  In the
above tutorial this is achieved by using a `.iso` file mounted as a Virtio block device in QEMU that Rumprun mounts.  While
this would also be possible on seL4, it would not work when running on bare-metal hardware.  To get around this we use the
librumprunfs_base that rumprun uses to store files in the binary that is mounted when Rumprun is initialised.

The `directory.txt` file in this directory points to a folder that will be installed in the `/` directory when
when Rumprun is initialised.  The folder name will be the same.
In order to combine the existing Rumprun configuration files and our NGINX files, we need to modify
the librumprunfs_base Makefile as shown below.  Our build scripts then rsync the existing configuration that is stored in
`rootfs` and the NGINX folder pointed to by `directory.txt` into a new `rootfs2` folder which will then be mounted
into `/` when our system boots.
rumprun/lib/librumprunfs_base/Makefile:
```
@@ -15,7 +15,7 @@ dependall:
          RUMPRUN_COOKFS_SIZE="${SIZE}"                                 \
            RUMPRUN_COOKFS_INCDIR="${RROBJ}/dest.stage/include"         \
                ${RROBJ}/app-tools/${TOOLTUPLE}-cookfs -s 1             \
-                   ${MAKEOBJDIR}/librumprunfs_base.a rootfs )
+                   ${MAKEOBJDIR}/librumprunfs_base.a rootfs2 )

```

## Compilation steps
```
make nginx-rumprun-ia32_defconfig
make
```
