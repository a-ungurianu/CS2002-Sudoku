#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "sudoku.h"


struct sudoku *createSudoku(unsigned size) {
    struct sudoku *new_sudoku = malloc(sizeof(struct sudoku));
    new_sudoku->size = size;
    new_sudoku->cells = malloc(sizeof(int) * (size * size) * (size * size));

    return new_sudoku;
}
struct sudoku *copySudoku(struct sudoku* sudoku) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    struct sudoku *new_sudoku = createSudoku(size);
    memcpy(new_sudoku->cells, sudoku->cells, sizeof(int) * (size * size) * (size * size));

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
    assert(row < size * size);
    assert(col < size * size);

    return sudoku->cells[row * (size * size) + col];

}

int* getSquare(struct sudoku *sudoku, unsigned square_row, unsigned square_col) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(square_row < size);
    assert(square_col < size);

    int *square_values = malloc(sizeof(int) * (size * size));
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            square_values[i*size + j] = getCell(sudoku, square_row * size + i,
                                                        square_col * size + j);
        }
    }

    return square_values;
}

int* getRow(struct sudoku *sudoku, unsigned row) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size);

    int* row_values = malloc(sizeof(int) * (size * size));
    memcpy(row_values,&sudoku->cells[row * (size * size)], size * size);

    return row_values;
}

int* getCol(struct sudoku *sudoku, unsigned col);

// Setter functions
void setCell(struct sudoku *sudoku, unsigned row, unsigned col, int value) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size * size);
    assert(col < size * size);

    sudoku->cells[row * (size * size) + col] = value;
}
