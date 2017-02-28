#ifndef SUDOKU_CHECKING_H
#define SUDOKU_CHECKING_H

#include "sudoku.h"


typedef unsigned __int128 uint128_t;

typedef enum {
    CR_INVALID,
    CR_INCOMPLETE,
    CR_COMPLETE
} check_result;

// Checking functions
check_result check_list(const int *values, unsigned size);
check_result check_sudoku(const sudoku *s);

#endif /* end of include guard: SUDOKU_CHECKING_H */
