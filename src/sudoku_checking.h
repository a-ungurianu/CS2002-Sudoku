#ifndef SUDOKU_CHECKING_H
#define SUDOKU_CHECKING_H

#include "sudoku.h"


typedef unsigned __int128 uint128_t;

/*
    Return value for the checking functions.
*/
typedef enum {
    CR_INVALID,
    CR_INCOMPLETE,
    CR_COMPLETE
} check_result;

// Checking functions

/*
    Checks if the given values are a valid row, column or a box of a sudoku of a given size.

    \param values the given values to check
    \param size the size of the sudoku

    \return CR_COMPLETE if the there's one of every single value,
            CR_INVALID if there are duplicates in the values
            CR_INCOMPLETE if it is not invalid, but there are empty spaces (0s) in the values.
*/
check_result check_list(const int *values, unsigned size);

/*
    Checks if the given sudoku is valid.

    \param givenSudoku the sudoku to check_result

    \return CR_COMPLETE if the sudoku is valid and CR_COMPLETE
            CR_INVALID if any of the rows, columns or boxes contain duplicates
            CR_INCOMPLETE if it is not invalid, but there are some empty spaces in the sudoku.
*/
check_result check_sudoku(const sudoku *givenSudoku);

#endif /* end of include guard: SUDOKU_CHECKING_H */
