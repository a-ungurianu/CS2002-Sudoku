#! /usr/bin/env bash

for f in $(find $1 -iname '*.in' | sort)
do
    echo -n "Basic - $(basename $f): "
    /usr/bin/time -f "%e s" ./sudoku_solver < $f > /dev/null
    echo -n "Advanced - $(basename $f): "
    /usr/bin/time -f "%e s" ./sudoku_advanced < $f > /dev/null
done
