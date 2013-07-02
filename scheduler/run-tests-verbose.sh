#!/bin/bash
for f in F L S R2 R5 R20; do
   for N in {0..5}; do
      echo "tests/input$N ${f}"
      echo '\n\nActual:'
      ./sched -s${f} tests/input$N tests/rfile; 
      echo '\n\nExpecting:'
      cat tests/output${N}_${f};
   done
done
