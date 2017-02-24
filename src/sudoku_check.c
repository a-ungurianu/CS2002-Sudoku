#include "sudoku.h"
#include "sudoku_io.h"

static const char* INVALID_STRING = "INVALID";
static const char* INCOMPLETE_STRING = "INCOMPLETE";
static const char* COMPLETE_STRING = "COMPLETE";


const char* getSudokuResult(sudoku *givenSudoku) {
    for(unsigned i = 0; i < givenSudoku->size * givenSudoku->size; ++i) {
        switch(check_list(getRow(givenSudoku, i), givenSudoku->size)) {
            case CR_INCOMPLETE:
                return INCOMPLETE_STRING;
            case CR_INVALID:
                return INVALID_STRING;
        }
    }

    for(unsigned i = 0; i < givenSudoku->size * givenSudoku->size; ++i) {
        switch(check_list(getCol(givenSudoku, i), givenSudoku->size)) {
            case CR_INCOMPLETE:
                return INCOMPLETE_STRING;
            case CR_INVALID:
                return INVALID_STRING;
        }
    }
    for(unsigned i = 0; i < givenSudoku->size; ++i) {
        for(unsigned j = 0; j < givenSudoku->size; ++j) {
            switch(check_list(getSquare(givenSudoku, i, j), givenSudoku->size)) {
                case CR_INCOMPLETE:
                    return INCOMPLETE_STRING;
                case CR_INVALID:
                    return INVALID_STRING;
            }
        }
    }
    return COMPLETE_STRING;
}

int main() {
    sudoku * givenSudoku = readSudoku(stdin);

    writeSudoku(stdout, givenSudoku);
    printf("%s\n",getSudokuResult(givenSudoku));

    return 0;
}
