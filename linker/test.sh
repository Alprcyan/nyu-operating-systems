#!/bin/bash
for N in {1..19}
do
   echo "tests/input-$N"
   ./linker tests/input-$N > outputs/out-$N; diff outputs/out-$N tests/out-$N -b -q -B
done
