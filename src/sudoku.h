#ifndef SUDOKU_H
#define SUDOKU_H

typedef struct {
    unsigned size;
    int* cells;
} sudoku;

typedef enum {
    CR_INVALID,
    CR_INCOMPLETE,
    CR_COMPLETE
} CheckResult;

// Allocation handling
sudoku *createSudoku(unsigned size);
sudoku *copySudoku(sudoku* sudoku);
void freeSudoku(sudoku* sudoku);

// Getter functions
int getCell(sudoku *sudoku, unsigned row, unsigned col);
void getSquare(sudoku *sudoku, unsigned square_row, unsigned square_col, int* dest);
void getRow(sudoku *sudoku, unsigned row, int* dest);
void getCol(sudoku *sudoku, unsigned col, int* dest);

// Setter functions
void setCell(sudoku *sudoku, unsigned row, unsigned col, int value);

// Checking function
CheckResult check_list(int* values, unsigned size);
#endif
