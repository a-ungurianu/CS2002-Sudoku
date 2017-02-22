#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "sudoku.h"

struct sudoku *createSudoku(unsigned size) {
    struct sudoku *new_sudoku = malloc(sizeof(struct sudoku));
    new_sudoku->size = size;
    new_sudoku->cells = malloc(sizeof(int) * (size * size));

    return new_sudoku;
}
struct sudoku *copySudoku(struct sudoku* sudoku) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    struct sudoku *new_sudoku = createSudoku(size);
    memcpy(new_sudoku->cells, sudoku->cells, sizeof(int) * (size * size));

    return new_sudoku;
}

void freeSudoku(struct sudoku* sudoku) {
    assert(sudoku != NULL);
    free(sudoku->cells);
    free(sudoku);
}

// Getter functions
int getCell(struct sudoku *sudoku, unsigned row, unsigned col) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size);
    assert(col < size);

    return sudoku->cells[row*size + col];

}

int* getSquare(struct sudoku *sudoku, unsigned square_row, unsigned square_col);

int* getRow(struct sudoku *sudoku, unsigned row) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size);

    int* row_values = malloc(sizeof(int) * size);
    memcpy(row_values,&sudoku->cells[row * size], size);

    return row_values;
}

int* getCol(struct sudoku *sudoku, unsigned col);

// Setter functions
void setCell(struct sudoku *sudoku, unsigned row, unsigned col, int value) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size);
    assert(col < size);

    sudoku->cells[row*size + col] = value;
}
