#! /usr/bin/env bash

for f in $(find stacscheck/ -iname '*.in' | grep '[23]_')
do
    time -f "%U" ./sudoku_advanced < $f
done
