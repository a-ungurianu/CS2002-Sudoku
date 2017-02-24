#include "sudoku.h"
#include "sudoku_io.h"
#include <stdlib.h>
#include <stdbool.h>

static const char* INVALID_STRING = "INVALID";
static const char* INCOMPLETE_STRING = "INCOMPLETE";
static const char* COMPLETE_STRING = "COMPLETE";


const char* getSudokuResult(sudoku *givenSudoku) {
    bool isIncomplete = false;
    int *valuesToCheck = malloc(sizeof(int) * givenSudoku->size * givenSudoku->size);
    for(unsigned i = 0; i < givenSudoku->size * givenSudoku->size; ++i) {
        getRow(givenSudoku, i, valuesToCheck);
        switch(check_list(valuesToCheck, givenSudoku->size)) {
            case CR_INCOMPLETE:
                isIncomplete = true;
                break;
            case CR_INVALID:
                return INVALID_STRING;
        }
    }

    for(unsigned i = 0; i < givenSudoku->size * givenSudoku->size; ++i) {
        getCol(givenSudoku, i, valuesToCheck);
        switch(check_list(valuesToCheck, givenSudoku->size)) {
            case CR_INCOMPLETE:
                isIncomplete = true;
                break;
            case CR_INVALID:
                return INVALID_STRING;
        }
    }
    for(unsigned i = 0; i < givenSudoku->size; ++i) {
        for(unsigned j = 0; j < givenSudoku->size; ++j) {
            getSquare(givenSudoku, i, j, valuesToCheck);
            switch(check_list(valuesToCheck, givenSudoku->size)) {
                case CR_INCOMPLETE:
                    isIncomplete = true;
                    break;
                case CR_INVALID:
                    return INVALID_STRING;
            }
        }
    }
    free(valuesToCheck);
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
