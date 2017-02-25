#ifndef SUDOKU_H
#define SUDOKU_H

typedef struct {
    unsigned size;
    int* cells;
} sudoku;

typedef struct {
    unsigned row;
    unsigned col;
} position;

typedef enum {
    CR_INVALID,
    CR_INCOMPLETE,
    CR_COMPLETE
} check_result;

// Utility functions
unsigned get_no_cells(sudoku* sudoku);

// Allocation handling
sudoku *create_sudoku(unsigned size);
sudoku *copy_sudoku(sudoku* sudoku);
void free_sudoku(sudoku* sudoku);

// Getter functions
int get_cell(sudoku *sudoku, unsigned row, unsigned col);
void get_square(sudoku *sudoku, unsigned squareRow, unsigned squareCol, int* dest);
void get_row(sudoku *sudoku, unsigned row, int* dest);
void get_col(sudoku *sudoku, unsigned col, int* dest);

// Setter functions
void set_cell(sudoku *sudoku, unsigned row, unsigned col, int value);

position index_to_position(sudoku *s, unsigned index);
unsigned position_to_index(sudoku *s, position pos);

#endif
