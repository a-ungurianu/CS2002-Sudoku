#include "sudoku_io.h"
#include "sudoku_solve.h"

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
            solve_result result = solve_sudoku(givenSudoku);

            switch (result.status) {
                case SR_UNSOLVABLE:
                    printf("%s\n", "UNSOLVABLE");
                    break;
                case SR_MULTIPLE:
                    printf("%s\n", "MULTIPLE");
                    free_sudoku(result.solution);
                    break;
                case SR_SOLVED:
                    write_sudoku(stdout, result.solution);
                    free_sudoku(result.solution);
                    break;
            }
            break;
    }

    free_sudoku(givenSudoku);

    return 0;
}
