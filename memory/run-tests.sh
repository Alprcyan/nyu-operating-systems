#!/bin/bash
for alg in l r f s c a N C A; do
   ./mmu -oOPFS -f4 -a${alg} tests/in18 tests/rfile | diff -b -B -q ./tests/out_in18_${alg}_4_OPFS -
done

for alg in l r f s c a N C A; do
   ./mmu -oOPFS -f8 -a${alg} tests/in60 tests/rfile | diff -b -B -q ./tests/out_in60_${alg}_8_OPFS -
done

for file in 1K4 rd1K 10K3; do
   for alg in l r f s c a N C A; do
      ./mmu -oOPFS -f32 -a${alg} tests/in${file} tests/rfile | diff -b -B -q ./tests/out_in${file}_${alg}_32_OPFS -
   done
done

for alg in l r f s c a N C A; do
   ./mmu -oPFS -f32 -a${alg} tests/in1M2 tests/rfile | diff -b -B -q ./tests/out_in1M2_${alg}_32_PFS -
done

