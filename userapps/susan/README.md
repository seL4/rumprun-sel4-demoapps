<!--
     Copyright 2017, Data61
     Commonwealth Scientific and Industrial Research Organisation (CSIRO)
     ABN 41 687 119 230.

     This software may be distributed and modified according to the terms of
     the BSD 2-Clause license. Note that NO WARRANTY is provided.
     See "LICENSE_BSD2.txt" for details.

     @TAG(DATA61_BSD)
-->
# About

This app provides the susan benchmark from the mibench suite. See the details in rumprun packages.

# Usage

Two output files are packed into the cookfs by rumprun. To run this benchmark use either of the following arguments;

    + `susan /data/susan/input_<SIZE>.pgm /tmp/output_<SIZE>.smoothing.pgm -s`
    + `susan /data/susan/input_<SIZE>.pgm /tmp/output_<SIZE>.edges.pgm -s`
    + `susan /data/susan/input_<SIZE>.pgm /tmp/output_<SIZE>.corners.pgm -s`

with SIZE=`small` or `large`.
