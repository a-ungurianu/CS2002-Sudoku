#ifndef SUDOKU_SOLVE_H
#define SUDOKU_SOLVE_H

#include "sudoku.h"

#include <assert.h>

typedef enum {
    SR_SOLVED,      //< if the sudoku has been solved
    SR_MULTIPLE,    //< if there are multiple solutions to the sudoku
    SR_UNSOLVABLE   //< if the given sudoku is unsolvable
} solve_status;

typedef struct {
    solve_status status; //< the solve status (solved, unsolvable or if multiple solutions have been found)
    sudoku *solution; //< the solution for the sudoku, if found
} solve_result;

// Solve function

/*
    Tries to solve the given sudoku.

    /param input the sudoku to be solved

    /return the solve status of the sudoku (solved, unsolvable, or if multiple solutions were found)
            and a found solution, if possible

*/
solve_result solve_sudoku(const sudoku *input);

#endif /* end of include guard: SUDOKU_SOLVE_H */
