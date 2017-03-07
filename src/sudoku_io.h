#ifndef SUDOKU_IO_H
#define SUDOKU_IO_H
#include "sudoku.h"
#include <stdio.h>

// I/O

/*
    Reads a sudoku from a given input stream.
    This assumes the sudoku is given in a correct format:
        - size of the sudoku on the first line
        - size^2 other lines, each containing size^2 space separated values representing the value
            of that cell of the sudoku, or 0 if the cell is empty

    /param input the input stream to read from

    /return a pointer of a new heap-allocated sudoku containing the read values
*/
sudoku *read_sudoku(FILE *input);

/*
    Writes a given sudoku to the given output stream.
    This will output the sudoku table, with 3-wide columns padded with spaces and new lines for each row

    /param output the output stream to write to
    /param givenSudoku the sudoku to write to the output stream
*/
void write_sudoku(FILE *output, const sudoku *givenSudoku);

#endif
