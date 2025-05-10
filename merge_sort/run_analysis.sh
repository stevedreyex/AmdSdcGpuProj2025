#!/bin/bash

echo "" > output.txt

ELEMENT_NUM=65536

for i in {1..12}; do
    echo "" > output.txt
    echo "ELEMENT_NUM = $ELEMENT_NUM" >> output.txt
    ./rand $ELEMENT_NUM > dataset.txt
    ./merge-serial $ELEMENT_NUM < dataset.txt >> output.txt
    ./merge-cuda $ELEMENT_NUM < dataset.txt >> output.txt
    ELEMENT_NUM=$((ELEMENT_NUM * 2))
done
