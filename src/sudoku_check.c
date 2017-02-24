#include "sudoku.h"
#include "sudoku_io.h"
#include <stdlib.h>
#include <stdbool.h>

static const char* INVALID_STRING = "INVALID";
static const char* INCOMPLETE_STRING = "INCOMPLETE";
static const char* COMPLETE_STRING = "COMPLETE";


const char* getSudokuResult(sudoku *givenSudoku) {
    bool isIncomplete = false;
    for(unsigned i = 0; i < givenSudoku->size * givenSudoku->size; ++i) {
        int *row_values = getRow(givenSudoku, i);
        switch(check_list(row_values, givenSudoku->size)) {
            case CR_INCOMPLETE:
                isIncomplete = true;
                break;
            case CR_INVALID:
                return INVALID_STRING;
        }
        free(row_values);
    }

    for(unsigned i = 0; i < givenSudoku->size * givenSudoku->size; ++i) {
        int *col_values = getCol(givenSudoku, i);
        switch(check_list(col_values, givenSudoku->size)) {
            case CR_INCOMPLETE:
                isIncomplete = true;
                break;
            case CR_INVALID:
                return INVALID_STRING;
        }
        free(col_values);
    }
    for(unsigned i = 0; i < givenSudoku->size; ++i) {
        for(unsigned j = 0; j < givenSudoku->size; ++j) {
            int* square_values = getSquare(givenSudoku, i, j);
            switch(check_list(square_values, givenSudoku->size)) {
                case CR_INCOMPLETE:
                    isIncomplete = true;
                    break;
                case CR_INVALID:
                    return INVALID_STRING;
            }
            free(square_values);
        }
    }
    if(isIncomplete) {
        return INCOMPLETE_STRING;
    }
    else {
        return COMPLETE_STRING;
    }
}

int main() {
    sudoku * givenSudoku = readSudoku(stdin);

    writeSudoku(stdout, givenSudoku);
    printf("%s\n",getSudokuResult(givenSudoku));

    freeSudoku(givenSudoku);
    return 0;
}
