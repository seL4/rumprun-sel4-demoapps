<!--
     Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)

     SPDX-License-Identifier: BSD-2-Clause
-->
# About

This app provides the susan benchmark from the mibench suite. See the details in rumprun packages.

# Usage

Two output files are packed into the cookfs by rumprun. To run this benchmark use either of the following arguments;

    + `susan /data/susan/input_<SIZE>.pgm /tmp/output_<SIZE>.smoothing.pgm -s`
    + `susan /data/susan/input_<SIZE>.pgm /tmp/output_<SIZE>.edges.pgm -s`
    + `susan /data/susan/input_<SIZE>.pgm /tmp/output_<SIZE>.corners.pgm -s`

with SIZE=`small` or `large`.
