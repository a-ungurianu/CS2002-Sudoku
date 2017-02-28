#! /usr/bin/env bash

for f in $(find $1 -iname '*.in' | sort)
do
    echo -n "$(basename $f): "
    /usr/bin/time -f "%e s" ./sudoku_advanced < $f > /dev/null
done
