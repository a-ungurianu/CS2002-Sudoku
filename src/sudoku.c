#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "sudoku.h"

typedef unsigned __int128 uint128_t;

sudoku *create_sudoku(unsigned size) {
    sudoku *newSudoku = malloc(sizeof(sudoku));
    newSudoku->size = size;
    newSudoku->cells = malloc(sizeof(int) * (size * size) * (size * size));

    return newSudoku;
}
sudoku *copy_sudoku(sudoku* srcSudoku) {
    assert(srcSudoku != NULL);
    unsigned size = srcSudoku->size;
    sudoku * newSudoku = create_sudoku(size);
    memcpy(newSudoku->cells, srcSudoku->cells, sizeof(int) * (size * size) * (size * size));

    return newSudoku;
}

void free_sudoku(sudoku* sudoku) {
    assert(sudoku != NULL);
    free(sudoku->cells);
    free(sudoku);
}

// Getter functions
int get_cell(sudoku *sudoku, unsigned row, unsigned col) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size * size);
    assert(col < size * size);

    return sudoku->cells[row * (size * size) + col];

}

void get_square(sudoku *sudoku, unsigned squareRow, unsigned squareCol, int* dest) {
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

void get_row(sudoku *sudoku, unsigned row, int* dest) {
    assert(sudoku != NULL);
    unsigned size = sudoku->size;
    assert(row < size * size);

    memcpy(dest, &sudoku->cells[row * (size * size)], sizeof(int) * size * size);

}

void get_col(sudoku *sudoku, unsigned col, int* dest) {
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

// Checking function
check_result check_list(int* values, unsigned size) {
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

check_result check_sudoku(sudoku *givenSudoku) {
    check_result result = CR_COMPLETE;
    int *valuesToCheck = malloc(sizeof(int) * givenSudoku->size * givenSudoku->size);
    for(unsigned i = 0; i < givenSudoku->size * givenSudoku->size; ++i) {
        get_row(givenSudoku, i, valuesToCheck);
        switch(check_list(valuesToCheck, givenSudoku->size)) {
        case CR_INCOMPLETE:
            result = CR_INCOMPLETE;
            break;
        case CR_INVALID:
            result = CR_INVALID;
            goto cleanup_and_return;
        }
    }

    for(unsigned i = 0; i < givenSudoku->size * givenSudoku->size; ++i) {
        get_col(givenSudoku, i, valuesToCheck);
        switch(check_list(valuesToCheck, givenSudoku->size)) {
        case CR_INCOMPLETE:
            result = CR_INCOMPLETE;
            break;
        case CR_INVALID:
            result = CR_INVALID;
            goto cleanup_and_return;
        }
    }
    for(unsigned i = 0; i < givenSudoku->size; ++i) {
        for(unsigned j = 0; j < givenSudoku->size; ++j) {
            get_square(givenSudoku, i, j, valuesToCheck);
            switch(check_list(valuesToCheck, givenSudoku->size)) {
            case CR_INCOMPLETE:
                result = CR_INCOMPLETE;
                break;
            case CR_INVALID:
                result = CR_INVALID;
                goto cleanup_and_return;
            }
        }
    }

cleanup_and_return:
    free(valuesToCheck);
    return result;
}
