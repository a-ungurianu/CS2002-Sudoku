#ifndef SUDOKU_IO_H
#define SUDOKU_IO_H
#include "sudoku.h"
#include <stdio.h>

// I/O
sudoku *read_sudoku(FILE *inputFile);
void write_sudoku(FILE *outputFile, sudoku *sudoku);

#endif
