# Iperf3

Iperf requires some extra configuration in order to function correctly.  
Rumprun currently doesn't support calling `mmap` on a file with read and write access.
Iperf calls `mmap` on a temporary file with these permissions even though it doesn't
write back to the file.  So when using Iperf to run benchmarks we temporarily comment
out the r/w permission check.  If we don't do this, all connections get `closed unexpectedly` by the Rumprun iperf server.

`rumprun/lib/librumpkern_mman/sys_mman.c`:
```
@@ -126,14 +126,14 @@ sys_mmap(struct lwp *l, const struct sys_mmap_args *uap, register_t *retval)
        void *v;
        size_t roundedlen;
        int error = 0;
-
+       (void) prot;
        MMAP_PRINTF(("-> mmap: %p %zu, 0x%x, 0x%x, %d, %" PRId64 "\n",
            SCARG(uap, addr), len, prot, flags, fd, pos));

-       if (fd != -1 && prot != PROT_READ) {
-               MMAP_PRINTF(("mmap: trying to r/w map a file. failing!\n"));
-               return EOPNOTSUPP;
-       }
+       // if (fd != -1 && prot != PROT_READ) {
+       //      MMAP_PRINTF(("mmap: trying to r/w map a file. failing!\n"));
+       //      return EOPNOTSUPP;
+       // }
```

## Compilation steps
```
make iperf-rumprun-ia32_defconfig
make
```
