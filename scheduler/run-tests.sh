#!/bin/bash
for f in F L S R2 R5 R20; do
   for N in {0..5}; do
      echo "tests/input$N ${f}"
      ./sched -s${f} tests/input$N tests/rfile > outputs/output$N; diff outputs/output$N tests/output${N}_${f} -b -q -B
   done
done
