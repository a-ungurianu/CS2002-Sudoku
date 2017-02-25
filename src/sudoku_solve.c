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
        default:
            ; // Do nothing
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
        default:
            ; // Do nothing
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
            default:
                ; // Do nothing
            }
        }
    }

cleanup_and_return:
    free(valuesToCheck);
    return result;
}

static bool check_update(sudoku *s, position pos) {

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

static void _solve_sudoku(solve_state *state, unsigned startPoint) {
    if(state->no_solutions < 2) {
        const unsigned no_cells = get_no_cells(state->current);
        const unsigned valMax = state->current->size * state->current->size;

        for(unsigned i = startPoint; i < no_cells; ++i) {
            if(state->current->cells[i] == 0) {
                for(unsigned val = 1; val <= valMax; ++val) {
                    position pos = index_to_position(state->current, i);
                    state->current->cells[i] = val;
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
