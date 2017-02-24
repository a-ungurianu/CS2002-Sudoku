#include "sudoku.h"
#include "sudoku_io.h"
#include <stdlib.h>

sudoku *solve_sudoku(sudoku *givenSudoku) {
    sudoku *solvedSudoku = copy_sudoku(givenSudoku);
    return solvedSudoku;
}

int main() {
    sudoku * givenSudoku = read_sudoku(stdin);

    switch (check_sudoku(givenSudoku)) {
        case CR_INVALID:
            printf("%s\n", "UNSOLVABLE");
            break;
        case CR_COMPLETE:
            write_sudoku(stdout, givenSudoku);
            break;
        case CR_INCOMPLETE:
            ; // Makes the variable initalization below work
            sudoku *solvedSudoku = solve_sudoku(givenSudoku);
            write_sudoku(stdout, givenSudoku);
            free_sudoku(solvedSudoku);
            break;
    }

    free_sudoku(givenSudoku);

    return 0;
}
