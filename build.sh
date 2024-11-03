#!/bin/bash

# make -C build clean --no-print-directory
rm -rf build VNos_v0.1.img VNos_v0.1.iso || true
cmake -B build
make -C build --no-print-directory
