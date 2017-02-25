#include "sudoku_io.h"
#include "sudoku_solve.h"
#include <stdbool.h>

static const char* INVALID_STRING = "INVALID";
static const char* INCOMPLETE_STRING = "INCOMPLETE";
static const char* COMPLETE_STRING = "COMPLETE";

int main() {
    sudoku * givenSudoku = read_sudoku(stdin);

    write_sudoku(stdout, givenSudoku);

    switch(check_sudoku(givenSudoku)) {
    case CR_COMPLETE:
        printf("%s\n", COMPLETE_STRING);
        break;
    case CR_INCOMPLETE:
        printf("%s\n", INCOMPLETE_STRING);
        break;
    case CR_INVALID:
        printf("%s\n", INVALID_STRING);
        break;
    }

    free_sudoku(givenSudoku);
    return 0;
}
