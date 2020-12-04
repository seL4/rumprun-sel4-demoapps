<!--
     Copyright 2017, Data61, CSIRO (ABN 41 687 119 230)

     SPDX-License-Identifier: BSD-2-Clause
-->
# About

This app provides the mad mp3 decoder benchmark from the mibench suite. See the details in rumprun packages.

# Usage

Two output files are packed into the cookfs by rumprun. To run this benchmark use either of the following arguments;

    + `madplay --time=30 --output=wave:/tmp/output_large.wav -v /data/mad/large.mp3`
    + `madplay --time=4 --output=wave:/tmp/output_small.wav -v /data/mad/small.mp3`

for large or small workloads respectively
