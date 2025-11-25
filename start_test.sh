#!/bin/bash
for N in 1 2 3 4 5 6 7 8 9 10 20 40 60 80 100 120 140 160
do
    for optimize in "O0" "O1" "O2" "O3" "Ofast"
    do
        echo "Running test with N=$N and optimize=$optimize"
        python3 var714_test.py -N $N --optimize $optimize
    done
done
