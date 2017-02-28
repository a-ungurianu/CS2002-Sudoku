#include "sudoku_solve.h"
#include "sudoku_io.h"
#include "sudoku_checking.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct {
    int no_solutions;
    sudoku *current;
    sudoku *solution;
} solve_state;

static bool check_update(const sudoku *s, position pos) {

    const unsigned sectionSize = s->size * s->size;
    int checkValuesBuffer[sectionSize];

    get_row(s, pos.row, checkValuesBuffer);
    if(check_list(checkValuesBuffer, s->size) == CR_INVALID) {
        return false;
    }

    get_col(s, pos.col, checkValuesBuffer);
    if(check_list(checkValuesBuffer, s->size) == CR_INVALID) {
        return false;
    }

    get_square(s, pos.row / s->size, pos.col / s->size, checkValuesBuffer);
    if(check_list(checkValuesBuffer, s->size) == CR_INVALID) {
        return false;
    }

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

        // If we reach this place, that means we found a solution.
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

solve_result solve_sudoku(const sudoku *given_sudoku) {
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
