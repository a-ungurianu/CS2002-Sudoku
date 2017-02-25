#ifndef SUDOKU_SOLVE_H
#define SUDOKU_SOLVE_H

#include "sudoku.h"

#include <assert.h>

typedef enum {
    SR_SOLVED,
    SR_MULTIPLE,
    SR_UNSOLVABLE
} solve_status;

typedef struct {
    solve_status status;
    sudoku *solution;
} solve_result;

// Checking functions
check_result check_list(int *values, unsigned size);
check_result check_sudoku(sudoku *s);

// Solve function
solve_result solve_sudoku(sudoku *givenSudoku);

#endif /* end of include guard: SUDOKU_SOLVE_H */
