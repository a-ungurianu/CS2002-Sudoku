#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdio.h>

struct sudoku {
    unsigned size;
    int* cells;
};

// Allocation handling
struct sudoku *createSudoku(unsigned size);
struct sudoku *copySudoku(struct sudoku* sudoku);
void freeSudoku(struct sudoku* sudoku);

// Getter functions
int getCell(struct sudoku *sudoku, unsigned row, unsigned col);
int* getSquare(struct sudoku *sudoku, unsigned square_row, unsigned square_col);
int* getRow(struct sudoku *sudoku, unsigned row);
int* getCol(struct sudoku *sudoku, unsigned col);

// Setter functions
void setCell(struct sudoku *sudoku, unsigned row, unsigned col, int value);

// I/O
struct sudoku *readSudoku(FILE *inputFile);
void writeSudoku(FILE *outputFile, struct sudoku *sudoku);

#endif
