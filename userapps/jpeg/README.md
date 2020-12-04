<!--
     Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)

     SPDX-License-Identifier: BSD-2-Clause
-->
# About

This project provides two apps, `cjpeg` and `djpeg`, the encode and decode stages of the jpeg benchmark from the mibench suite. See the details in rumprun packages.

# Usage

## cjpeg

Two output files are packed into the cookfs by rumprun. To run this benchmark use either of the following arguments;

    + `cjpeg -dct int -progressive -opt -outfile /tmp/output_large_encode.jpeg /data/jpeg/input_large.ppm`
    + `cjpeg -dct int -progressive -opt -outfile /tmp/output_small_encode.jpeg /data/jpeg/input_small.ppm`

for large or small workloads respectively

## djpeg

Two output files are packed into the cookfs by rumprun. To run this benchmark use either of the following arguments;

    + `djpeg -dct int -ppm -outfile /tmp/output_large_decode.jpeg /data/jpeg/input_large.ppm`
    + `djpeg -dct int -ppm -outfile /tmp/output_small_decode.jpeg /data/jpeg/input_small.ppm`

for large or small workloads respectively
