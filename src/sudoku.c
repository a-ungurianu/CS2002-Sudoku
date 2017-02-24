#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "sudoku.h"

typedef unsigned __int128 uint128_t;

sudoku *createSudoku(unsigned size) {
    sudoku *new_sudoku = malloc(sizeof(sudoku));
    new_sudoku->size = size;
    new_sudoku->cells = malloc(sizeof(int) * (size * size) * (size * size));

    return new_sudoku;
}
sudoku *copySudoku(sudoku* srcSudoku) {
    assert(srcSudoku != NULL);
    unsigned size = srcSudoku->size;
    sudoku * new_sudoku = createSudoku(size);
    memcpy(new_sudoku->cells, srcSudoku->cells, sizeof(int) * (size * size) * (size * size));

    return new_sudoku;
}

void freeSudoku(sudoku* sudoku) {
    assert(sudoku != NULL);
    free(sudoku->cells);
    free(sudoku);
}

// Getter functions
int getCell(sudoku *sudoku, unsigned row, unsigned col) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size * size);
    assert(col < size * size);

    return sudoku->cells[row * (size * size) + col];

}

int* getSquare(sudoku *sudoku, unsigned square_row, unsigned square_col) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(square_row < size);
    assert(square_col < size);

    int *square_values = malloc(sizeof(int) * (size * size));
    for(unsigned i = 0; i < size; ++i) {
        for(unsigned j = 0; j < size; ++j) {
            square_values[i*size + j] = getCell(sudoku, square_row * size + i,
                                                        square_col * size + j);
        }
    }

    return square_values;
}

int* getRow(sudoku *sudoku, unsigned row) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size * size);

    int* row_values = malloc(sizeof(int) * (size * size));
    memcpy(row_values,&sudoku->cells[row * (size * size)], sizeof(int) * size * size);

    return row_values;
}

int* getCol(sudoku *sudoku, unsigned col) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(col < size * size);
    int* col_values = malloc(sizeof(int) * (size * size));

    for(unsigned i = 0; i < size * size; ++i) {
        col_values[i] = getCell(sudoku, i, col);
    }
    return col_values;
}

// Setter functions
void setCell(sudoku *sudoku, unsigned row, unsigned col, int value) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size * size);
    assert(col < size * size);

    sudoku->cells[row * (size * size) + col] = value;
}

// Checking function
CheckResult check_list(int* values, unsigned size) {
    unsigned listSize = size * size;
    uint128_t valuesSeen = 0;

    for(unsigned i = 0; i < listSize; ++i) {
        if(values[i] != 0) {
            if((valuesSeen & (1 << values[i])) != 0) {
                return CR_INVALID;
            }
            else {
                valuesSeen |= 1 << values[i];
            }
        }
    }

    for(unsigned i = 1; i <= listSize; ++i) {
        if((valuesSeen & (1 << i)) == 0) {
            return CR_INCOMPLETE;
        }
    }

    return CR_COMPLETE;
}
