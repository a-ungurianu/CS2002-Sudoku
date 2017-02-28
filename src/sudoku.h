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

// Utility functions
unsigned get_no_cells(const sudoku *sudoku);

// Allocation handling
sudoku *create_sudoku(unsigned size);
sudoku *copy_sudoku(const sudoku* sudoku);
void free_sudoku(sudoku* sudoku);

// Getter functions
int get_cell(const sudoku *sudoku, unsigned row, unsigned col);
void get_square(const sudoku *sudoku, unsigned squareRow, unsigned squareCol, int* dest);
void get_row(const sudoku *sudoku, unsigned row, int* dest);
void get_col(const sudoku *sudoku, unsigned col, int* dest);

// Setter functions
void set_cell(sudoku *sudoku, unsigned row, unsigned col, int value);

position index_to_position(const sudoku *s, unsigned index);
unsigned position_to_index(const sudoku *s, position pos);

#endif
