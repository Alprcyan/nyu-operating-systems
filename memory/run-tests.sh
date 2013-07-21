#!/bin/bash

./mmu -oOPFS -f$1 -a$2 tests/in$3 tests/rfile | diff -b -B ./tests/out_in$3_$2_$1_OPFS -
