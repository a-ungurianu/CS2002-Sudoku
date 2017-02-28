#include "sudoku.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

//Utility methods
unsigned get_no_cells(const sudoku *s) {
    return s->size * s->size * s->size * s->size;
}

sudoku *create_sudoku(unsigned size) {
    sudoku *newSudoku = malloc(sizeof(sudoku));
    assert(newSudoku != NULL);

    newSudoku->size = size;
    newSudoku->cells = malloc(sizeof(int) * get_no_cells(newSudoku));
    assert(newSudoku->cells != NULL);

    return newSudoku;
}
sudoku *copy_sudoku(const sudoku* srcSudoku) {
    assert(srcSudoku != NULL);
    unsigned size = srcSudoku->size;
    sudoku * newSudoku = create_sudoku(size);
    memcpy(newSudoku->cells, srcSudoku->cells, sizeof(int) * get_no_cells(newSudoku));

    return newSudoku;
}

void free_sudoku(sudoku* sudoku) {
    assert(sudoku != NULL);
    free(sudoku->cells);
    free(sudoku);
}

// Getter functions
int get_cell(const sudoku *sudoku, unsigned row, unsigned col) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size * size);
    assert(col < size * size);

    return sudoku->cells[row * (size * size) + col];

}

void get_square(const sudoku *sudoku, unsigned squareRow, unsigned squareCol, int* dest) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(squareRow < size);
    assert(squareCol < size);

    for(unsigned i = 0; i < size; ++i) {
        for(unsigned j = 0; j < size; ++j) {
            dest[i*size + j] = get_cell(sudoku, squareRow * size + i,
                                                squareCol * size + j);
        }
    }
}

void get_row(const sudoku *sudoku, unsigned row, int* dest) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size * size);

    memcpy(dest, &sudoku->cells[row * (size * size)], sizeof(int) * size * size);

}

void get_col(const sudoku *sudoku, unsigned col, int* dest) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(col < size * size);

    for(unsigned i = 0; i < size * size; ++i) {
        dest[i] = get_cell(sudoku, i, col);
    }
}

// Setter functions
void set_cell(sudoku *sudoku, unsigned row, unsigned col, int value) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size * size);
    assert(col < size * size);

    sudoku->cells[row * (size * size) + col] = value;
}

// Position convertion
position index_to_position(const sudoku *s, unsigned index) {
    int row = index / (s->size * s->size);
    int col = index % (s->size * s->size);

    return (position){row,col};
}

unsigned position_to_index(const sudoku *s, position pos) {
    int width = s->size * s->size;
    return pos.row * width + pos.col;
}
