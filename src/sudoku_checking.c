#include "sudoku_checking.h"



// Checking functions

/*
    Checks if the given values are a valid row, column or a box of a sudoku of a given size.

    Due to the practical specifing the max size of the sudoku (n=9) we can guarantee that there will
    be no more then 81 possible values in each list.

    Therefore we're using an unsigned __int128 variable as a bit set for keeping track of what values
    we've already seen when iteration through the values.

    \param values the given values to check
    \param size the size of the sudoku

    \return CR_COMPLETE if the there's one of every single value,
            CR_INVALID if there are duplicates in the values
            CR_INCOMPLETE if it is not invalid, but there are empty spaces (0s) in the values.
*/
check_result check_list(const int* values, unsigned size) {
    const uint128_t ONE = 1;
    unsigned listSize = size * size;
    uint128_t valuesSeenSet = 0; // Using this as a bit set.

    for(unsigned i = 0; i < listSize; ++i) {
        if(values[i] != 0) {
            // Check if the values[i]-th bit is set.
            if((valuesSeenSet & (ONE << values[i])) != 0) {
                return CR_INVALID;
            }
            else {
                // Set the values[i]-th bit.
                valuesSeenSet |= ONE << values[i];
            }
        }
    }
    for(unsigned i = 1; i <= listSize; ++i) {
        // Check if the values[i]-th bit is set.
        if((valuesSeenSet & (ONE << i)) == 0) {
            return CR_INCOMPLETE;
        }
    }
    return CR_COMPLETE;
}

/*
    Checks if the given sudoku is valid.

    To check to see if the state of the sudoku, we retrieve each row, column and box and check the
    state of those using the check_list function.

    If any of those calls return CR_INVALID, just return CR_INVALID.
    Else if we find any CR_INCOMPLETEs return CR_INCOMPLETE.
    Otherwise, return COMPLETE.

    \param givenSudoku the sudoku to check_result

    \return CR_COMPLETE if the sudoku is valid and CR_COMPLETE
            CR_INVALID if any of the rows, columns or boxes contain duplicates
            CR_INCOMPLETE if it is not invalid, but there are some empty spaces in the sudoku.
    \sa check_list, check_result
*/
check_result check_sudoku(const sudoku *givenSudoku) {
    check_result result = CR_COMPLETE;

    const unsigned sectionSize = givenSudoku->size * givenSudoku->size;

    // Initialize an buffer to hold the retrieved values.
    int valuesToCheck[sectionSize];

    for(unsigned i = 0; i < sectionSize; ++i) {
        get_row(givenSudoku, i, valuesToCheck);
        switch(check_list(valuesToCheck, givenSudoku->size)) {
            case CR_INCOMPLETE:
                result = CR_INCOMPLETE;
                break;
            case CR_INVALID:
                return CR_INVALID;
        }
    }

    for(unsigned i = 0; i < sectionSize; ++i) {
        get_col(givenSudoku, i, valuesToCheck);
        switch(check_list(valuesToCheck, givenSudoku->size)) {
            case CR_INCOMPLETE:
                result = CR_INCOMPLETE;
                break;
            case CR_INVALID:
                return CR_INVALID;
        }
    }
    for(unsigned i = 0; i < givenSudoku->size; ++i) {
        for(unsigned j = 0; j < givenSudoku->size; ++j) {
            get_square(givenSudoku, i, j, valuesToCheck);
            switch(check_list(valuesToCheck, givenSudoku->size)) {
                case CR_INCOMPLETE:
                    result = CR_INCOMPLETE;
                    break;
                case CR_INVALID:
                    return CR_INVALID;
            }
        }
    }
    return result;
}
