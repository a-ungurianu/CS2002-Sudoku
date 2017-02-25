#include "sudoku_solve.h"
#include "sudoku_io.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef unsigned __int128 uint128_t;

typedef struct {
    int no_solutions;
    sudoku *current;
    sudoku *solution;
} solve_state;

// Checking function
check_result check_list(int* values, unsigned size) {
    unsigned listSize = size * size;
    uint128_t valuesSeen = 0;

    for(unsigned i = 0; i < listSize; ++i) {
        if(values[i] != 0) {
            if((valuesSeen & (1 << values[i])) != 0) {
                return CR_INVALID;
            }
            else {
                valuesSeen |= 1 << values[i];
            }
        }
    }
    for(unsigned i = 1; i <= listSize; ++i) {
        if((valuesSeen & (1 << i)) == 0) {
            return CR_INCOMPLETE;
        }
    }
    return CR_COMPLETE;
}

check_result check_sudoku(sudoku *givenSudoku) {
    check_result result = CR_COMPLETE;
    int *valuesToCheck = malloc(sizeof(int) * givenSudoku->size * givenSudoku->size);
    for(unsigned i = 0; i < givenSudoku->size * givenSudoku->size; ++i) {
        get_row(givenSudoku, i, valuesToCheck);
        switch(check_list(valuesToCheck, givenSudoku->size)) {
        case CR_INCOMPLETE:
            result = CR_INCOMPLETE;
            break;
        case CR_INVALID:
            result = CR_INVALID;
            goto cleanup_and_return;
        }
    }

    for(unsigned i = 0; i < givenSudoku->size * givenSudoku->size; ++i) {
        get_col(givenSudoku, i, valuesToCheck);
        switch(check_list(valuesToCheck, givenSudoku->size)) {
        case CR_INCOMPLETE:
            result = CR_INCOMPLETE;
            break;
        case CR_INVALID:
            result = CR_INVALID;
            goto cleanup_and_return;
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
                result = CR_INVALID;
                goto cleanup_and_return;
            }
        }
    }

cleanup_and_return:
    free(valuesToCheck);
    return result;
}

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator.

   Source: http://benpfaff.org/writings/clc/shuffle.html
   */
void shuffle(int *array, size_t n)
{
    if (n > 1) {
        size_t i;
	for (i = 0; i < n - 1; i++) {
	  size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
	  int t = array[j];
	  array[j] = array[i];
	  array[i] = t;
	}
    }
}

int* shuffled_positions = NULL;


bool check_update(sudoku *s, position pos) {

    const unsigned sec_size =s->size * s->size;
    int* buffer = malloc(sizeof(int) * sec_size);

    get_row(s, pos.row, buffer);
    if(check_list(buffer, s->size) == CR_INVALID) {
        free(buffer);
        return false;
    }

    get_col(s, pos.col, buffer);
    if(check_list(buffer, s->size) == CR_INVALID) {
        free(buffer);
        return false;
    }

    get_square(s, pos.row / s->size, pos.col / s->size, buffer);
    if(check_list(buffer, s->size) == CR_INVALID) {
        free(buffer);
        return false;
    }

    free(buffer);
    return true;
}

void _solve_sudoku(solve_state *state, unsigned startPoint) {
    if(state->no_solutions < 2) {
        const unsigned no_cells = get_no_cells(state->current);
        const unsigned posIdxMax = state->current->size * state->current->size;

        for(unsigned i = startPoint; i < no_cells; ++i) {
            if(state->current->cells[i] == 0) {
                for(unsigned posIdx = 0; posIdx < posIdxMax; ++posIdx) {
                    position pos = index_to_position(state->current, i);
                    state->current->cells[i] = shuffled_positions[posIdx];
                    if(check_update(state->current, pos)) {
                        _solve_sudoku(state, i + 1);
                    }
                    state->current->cells[i] = 0;
                }
                return;
            }
        }

        state->no_solutions++;
        if(state->no_solutions == 1) {
            assert(state->solution == NULL);
            state->solution = copy_sudoku(state->current);
        }
        else {
            assert(state->solution != NULL);
            free_sudoku(state->solution);
            state->solution = copy_sudoku(state->current);
        }
    }
}

solve_result solve_sudoku(sudoku *given_sudoku) {
    sudoku *sudokuCopy = copy_sudoku(given_sudoku);

    shuffled_positions = malloc(sizeof(int) * given_sudoku->size * given_sudoku->size);

    for(int i = 0; i < given_sudoku->size * given_sudoku->size; ++i) {
        shuffled_positions[i] = i+1;
    }

    shuffle(shuffled_positions, given_sudoku->size * given_sudoku->size);

    solve_state state = (solve_state){0,sudokuCopy,NULL};
    _solve_sudoku(&state, 0);
    free_sudoku(sudokuCopy);

    solve_result result;
    switch (state.no_solutions) {
        case 0:
            result.status = SR_UNSOLVABLE;
            break;
        case 1:
            result.status = SR_SOLVED;
            break;
        default:
            result.status = SR_MULTIPLE;
            break;
    }
    result.solution = state.solution;

    return result;
}
