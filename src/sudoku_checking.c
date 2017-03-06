#include "sudoku_checking.h"


// Checking function
check_result check_list(const int* values, unsigned size) {
    const uint128_t ONE = 1;
    unsigned listSize = size * size;
    uint128_t valuesSeenSet = 0; // Using this as a bit set.

    for(unsigned i = 0; i < listSize; ++i) {
        if(values[i] != 0) {
            if((valuesSeenSet & (ONE << values[i])) != 0) {
                return CR_INVALID;
            }
            else {
                valuesSeenSet |= ONE << values[i];
            }
        }
    }
    for(unsigned i = 1; i <= listSize; ++i) {
        if((valuesSeenSet & (ONE << i)) == 0) {
            return CR_INCOMPLETE;
        }
    }
    return CR_COMPLETE;
}


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
