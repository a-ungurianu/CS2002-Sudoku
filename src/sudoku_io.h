#ifndef SUDOKU_IO_H
#define SUDOKU_IO_H
#include "sudoku.h"
#include <stdio.h>

// I/O
struct sudoku *readSudoku(FILE *inputFile);
void writeSudoku(FILE *outputFile, struct sudoku *sudoku);

#endif
