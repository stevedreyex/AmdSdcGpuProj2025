#!/bin/bash

echo "" > cuda.txt

POWER_FOR_TWO=10

for i in {1..20}; do
    echo "(num of element: 2^$POWER_FOR_TWO)" >> cuda.txt
    ./bitonic_cuda $POWER_FOR_TWO >> cuda.txt
    POWER_FOR_TWO=$((POWER_FOR_TWO + 1))
    echo "" >> cuda.txt
done
