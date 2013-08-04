#!/bin/bash
for num in 0 1 2 3 4 5 6; do
   for alg in f; do
      ./iosched -s${alg} -v tests/input${num} | diff -b -B -q ./tests/out${num}_${alg}_long -
   done
done
