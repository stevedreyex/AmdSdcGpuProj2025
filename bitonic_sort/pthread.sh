#!/bin/bash

echo "(<POWER_FOR_TWO>, <THREAD_NUM>)" > pthread.txt

POWER_FOR_TWO=10
THREAD_NUM=1

for i in {1..20}; do
    THREAD_NUM=1
    for j in {1..4}; do
        echo "($POWER_FOR_TWO, $THREAD_NUM)" >> pthread.txt
        ./bitonic_pthread $POWER_FOR_TWO $THREAD_NUM >> pthread.txt
        THREAD_NUM=$((THREAD_NUM + 1))
        echo "" >> pthread.txt
    done
    POWER_FOR_TWO=$((POWER_FOR_TWO + 1))
done

