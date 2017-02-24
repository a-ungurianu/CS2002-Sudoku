#ifndef SUDOKU_IO_H
#define SUDOKU_IO_H
#include "sudoku.h"
#include <stdio.h>

// I/O
sudoku *readSudoku(FILE *inputFile);
void writeSudoku(FILE *outputFile, sudoku *sudoku);

#endif
