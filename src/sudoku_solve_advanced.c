#include "sudoku.h"
#include "sudoku_io.h"
#include "sudoku_solve.h"
#include "sudoku_checking.h"
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#ifndef DEBUG
    #define DEBUG 0
#endif

#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr,fmt, __VA_ARGS__); } while (0)

typedef struct column_data {
    unsigned size;
    char* name;
} column_data;

typedef struct cell_data {
    unsigned row;
    unsigned col;
    unsigned value;
} cell_data;

typedef union cell_metadata {
    column_data *columnData;
    cell_data *cellData;
} cell_metadata;

typedef struct data_object {
    struct data_object *up;
    struct data_object *down;
    struct data_object *left;
    struct data_object *right;
    struct data_object *column;
    cell_metadata data;
} data_object;

typedef struct constraint_table {
    data_object *head;
} constraint_table;

typedef struct solve_state {
    int no_solutions;
    sudoku *current;
    sudoku *solution;
} solve_state;

static unsigned no_empty_spaces(const sudoku *s) {
    const unsigned sectionSize = s->size * s->size;

    unsigned noEmptySpaces = 0;

    for(unsigned i = 0; i < sectionSize * sectionSize; ++i) {
        if(s->cells[i] == 0) {
            noEmptySpaces++;
        }
    }

    return noEmptySpaces;
}

static void free_column_data(column_data* data) {
    free(data->name);
    free(data);
}

static void free_column(data_object* header) {
    free_column_data(header->data.columnData);
    data_object *current = header->down;
    while(current != header) {
        current = current->down;
        free(current->up->data.cellData);
        free(current->up);
    }
    free(header);
}

static void free_constraint_table(constraint_table *table) {
    data_object *current = table->head->right;
    while(current != table->head) {
        current = current->right;
        free_column(current->left);
    }
    free(table->head);
    free(table);
}

static data_object *add_column_header(constraint_table *table, char *name) {
    column_data* colData = malloc(sizeof(column_data));
    assert(colData != NULL);

    colData->size = 0;
    colData->name = name;

    data_object* newColumnHeader = malloc(sizeof(data_object));
    assert(newColumnHeader != NULL);

    newColumnHeader->data.columnData = colData;
    newColumnHeader->column = newColumnHeader;
    
    newColumnHeader->up = newColumnHeader;
    newColumnHeader->down = newColumnHeader;
    newColumnHeader->left = table->head->left;
    table->head->left->right = newColumnHeader;
    table->head->left = newColumnHeader;
    newColumnHeader->right = table->head;

    return newColumnHeader;
}

static data_object *add_constraint_to_column(data_object *columnHeader, unsigned row, unsigned col, unsigned val) {
    data_object* dataObj = malloc(sizeof(data_object));
    assert(dataObj != NULL);

    dataObj->column = columnHeader;

    dataObj->up = columnHeader->up;
    dataObj->down = columnHeader;
    columnHeader->up->down = dataObj;
    columnHeader->up = dataObj;

    cell_data *cellData = malloc(sizeof(cell_data));
    assert(cellData != NULL);

    cellData->row = row;
    cellData->col = col;
    cellData->value = val;
    dataObj->data.cellData = cellData;
    columnHeader->data.columnData->size++;

    return dataObj;
}

static bool check_update(const sudoku *s, position pos) {
    int buffer[s->size * s->size];

    get_row(s, pos.row, buffer);
    if(check_list(buffer, s->size) == CR_INVALID) {
        return false;
    }

    get_col(s, pos.col, buffer);
    if(check_list(buffer, s->size) == CR_INVALID) {
        return false;
    }

    get_square(s, pos.row / s->size, pos.col / s->size, buffer);
    if(check_list(buffer, s->size) == CR_INVALID) {
        return false;
    }

    return true;
}

static void remove_zero_columns(constraint_table *table) {
    data_object *current = table->head->right;

    while(current != table->head) {
        if(current->data.columnData->size == 0) {
            current->left->right = current->right;
            current->right->left = current->left;
            data_object *toDelete = current;
            current = current->right;
            free_column(toDelete);
        } else {
            current = current->right;
        }
    }
}

static constraint_table *generate_table(sudoku *s) {
    constraint_table *table = malloc(sizeof(constraint_table));
    assert(table != NULL);

    table->head = malloc(sizeof(data_object));
    assert(table->head != NULL);

    table->head->left = table->head;
    table->head->right = table->head;

    unsigned sectionSize = s->size * s->size;

    data_object** rowColumnHeaders = malloc(sizeof(data_object*) * sectionSize * sectionSize);
    assert(rowColumnHeaders != NULL);

    //Add Row-Column constraint columns
    for(unsigned r = 0; r < sectionSize; ++r) {
        for(unsigned c = 0; c < sectionSize; ++c) {
            char * name = malloc(sizeof(char) * 20);
            assert(name != NULL);
            sprintf(name, "R%dC%d", r + 1, c + 1);
            rowColumnHeaders[r * sectionSize + c] = add_column_header(table, name);
        }
    }


    data_object** rowNumberHeaders = malloc(sizeof(data_object*) * sectionSize * sectionSize);
    assert(rowNumberHeaders != NULL);

    //Add Row-Number constraint columns
    for(unsigned r = 0; r < sectionSize; ++r) {
        for(unsigned val = 0; val < sectionSize; ++val) {
            char * name = malloc(sizeof(char) * 20);
            assert(name != NULL);
            sprintf(name, "R%d#%d", r + 1, val + 1);
            rowNumberHeaders[r * sectionSize + val] = add_column_header(table, name);
        }
    }

    data_object** columnNumberHeaders = malloc(sizeof(data_object*) * sectionSize * sectionSize);
    assert(columnNumberHeaders != NULL);

    //Add Column-Number constraint columns
    for(unsigned c = 0; c < sectionSize; ++c) {
        for(unsigned val = 0; val < sectionSize; ++val) {
            char * name = malloc(sizeof(char) * 20);
            assert(name != NULL);
            sprintf(name, "C%d#%d", c + 1, val + 1);
            columnNumberHeaders[c * sectionSize + val] = add_column_header(table, name);
        }
    }

    data_object** boxNumberHeaders = malloc(sizeof(data_object*) * sectionSize * sectionSize);
    assert(boxNumberHeaders != NULL);

    //Add Box-Number constraint columns
    for(unsigned r = 0; r < s->size; ++r) {
        for(unsigned c = 0; c < s->size; ++c) {
            for(unsigned val = 0; val < sectionSize; ++val) {
                char * name = malloc(sizeof(char) * 20);
                assert(name != NULL);
                sprintf(name, "BR%dC%d#%d", r + 1, c + 1, val + 1);
                boxNumberHeaders[(r * s->size + c) * sectionSize + val] = add_column_header(table, name);
            }
        }
    }

    for(unsigned row = 0; row < sectionSize; ++row) {
        for(unsigned col = 0; col < sectionSize; ++col) {
            if(get_cell(s, row, col) == 0) {
                for(unsigned val = 0; val < sectionSize; ++val) {
                    set_cell(s, row, col, val+1);
                    if(check_update(s, (position){row,col})) {
                        data_object *rowColumnHeader = rowColumnHeaders[row * sectionSize + col];
                        data_object *rowNumberHeader = rowNumberHeaders[row * sectionSize + val];
                        data_object *colNumberHeader = columnNumberHeaders[col * sectionSize + val];
                        unsigned boxRow = row / s->size;
                        unsigned boxCol = col / s->size;
                        data_object *boxNumberHeader = boxNumberHeaders[(boxRow * s->size + boxCol) * sectionSize + val];

                        data_object *rowColumnConstraint = add_constraint_to_column(rowColumnHeader, row, col, val);
                        data_object *rowNumberConstraint = add_constraint_to_column(rowNumberHeader, row, col, val);
                        data_object *colNumberConstraint = add_constraint_to_column(colNumberHeader, row, col, val);
                        data_object *boxNumberConstraint = add_constraint_to_column(boxNumberHeader, row, col, val);

                        rowColumnConstraint->right = rowNumberConstraint;
                        rowNumberConstraint->right = colNumberConstraint;
                        colNumberConstraint->right = boxNumberConstraint;
                        boxNumberConstraint->right = rowColumnConstraint;

                        rowColumnConstraint->left = boxNumberConstraint;
                        rowNumberConstraint->left = rowColumnConstraint;
                        colNumberConstraint->left = rowNumberConstraint;
                        boxNumberConstraint->left = colNumberConstraint;
                    }
                    set_cell(s, row, col, 0);
                }
            }
        }
    }

    remove_zero_columns(table);

    free(rowColumnHeaders);
    free(rowNumberHeaders);
    free(columnNumberHeaders);
    free(boxNumberHeaders);
    return table;
}

static void write_table(constraint_table* table, FILE *output) {
    data_object * current = table->head;

    while(current->right != table->head) {
        current = current->right;
        fprintf(output, "%s(%d) :",current->data.columnData->name, current->data.columnData->size);

        data_object *currentVal = current;
        while(currentVal->down != current) {
            currentVal = currentVal->down;
            cell_data * cellData = currentVal->data.cellData;
            fprintf(output, "(R%d C%d #%d) ", cellData->row + 1, cellData->col + 1, cellData->value + 1);
        }
        fprintf(output, "\n");
    }
}

static data_object **solutionObjects;

static data_object *get_smallest_column(constraint_table *table) {
    data_object * current = table->head->right;

    data_object *smallestColumn = NULL;
    unsigned smallestSize = UINT_MAX;
    while(current->right != table->head) {
        if(current->data.columnData->size < smallestSize) {
            smallestSize = current->data.columnData->size;
            smallestColumn = current;
        }
        current = current->right;
    }

    return smallestColumn;
}

static void cover_column(data_object *column) {
    column->right->left = column->left;
    column->left->right = column->right;

    data_object* rowToCover = column->down;
    while(rowToCover != column) {
        data_object* attachedCell = rowToCover->right;
        while(attachedCell != rowToCover) {
            attachedCell->down->up = attachedCell->up;
            attachedCell->up->down = attachedCell->down;
            attachedCell->column->data.columnData->size--;
            attachedCell = attachedCell->right;
        }
        rowToCover = rowToCover->down;
    }
}

static void uncover_column(data_object *column) {
    data_object* rowToUncover = column->up;
    while(rowToUncover != column) {
        data_object* attachedCell = rowToUncover->left;
        while(attachedCell != rowToUncover) {
            attachedCell->column->data.columnData->size ++;
            attachedCell->down->up = attachedCell;
            attachedCell->up->down = attachedCell;
            attachedCell = attachedCell->left;
        }
        rowToUncover = rowToUncover->up;
    }
    column->right->left = column;
    column->left->right = column;
}

static sudoku * fill_in_sudoku(const sudoku *s, data_object** thingsToFill, unsigned noThingsToFill) {
    sudoku *solved = copy_sudoku(s);

    for(unsigned i = 0; i < noThingsToFill; ++i) {
        cell_data *cellData = thingsToFill[i]->data.cellData;
        set_cell(solved, cellData->row, cellData->col, cellData->value + 1);
    }

    return solved;
}

static void solve_table(constraint_table *table, solve_state* state, unsigned depth) {
    if(state->no_solutions < 2) {
        data_object* head = table->head;

        if(head->right == head) {
            state->no_solutions++;
            if(state->no_solutions == 1) {
                assert(state->solution == NULL);
                state->solution = fill_in_sudoku(state->current, solutionObjects, depth);
            }
            else {
                assert(state->solution != NULL);
                free_sudoku(state->solution);
                state->solution = fill_in_sudoku(state->current, solutionObjects, depth);
            }
        }
        else {
            // Choose a column header.
            data_object* smallestColumn = get_smallest_column(table);

            // Cover column
            cover_column(smallestColumn);
            data_object* rowToCover = smallestColumn->down;

            while(rowToCover != smallestColumn) {
                solutionObjects[depth] = rowToCover;

                data_object* attachedCell = rowToCover->right;
                while(attachedCell != rowToCover) {
                    // Cover column for attachedCell
                    cover_column(attachedCell->column);
                    attachedCell = attachedCell->right;
                }
                solve_table(table, state, depth + 1);
                rowToCover = solutionObjects[depth];

                attachedCell = rowToCover->left;
                while(attachedCell != rowToCover) {
                    // Uncover column for attachedCell
                    uncover_column(attachedCell->column);
                    attachedCell = attachedCell->left;
                }
                rowToCover = rowToCover->down;
            }
            // Uncover column
            uncover_column(smallestColumn);
        }
    }
}

solve_result solve_sudoku(const sudoku *input) {

    sudoku *toSolve = copy_sudoku(input);

    solutionObjects = malloc(sizeof(data_object*) * no_empty_spaces(input)); // Compute the number by counting the number of zeros.
    assert(solutionObjects);

    constraint_table *table = generate_table(toSolve);

    solve_state state = (solve_state){0,toSolve,NULL};
    solve_table(table, &state, 0);

    solve_result result;

    switch (state.no_solutions) {
        case 0:
            result.status = SR_UNSOLVABLE;
            break;
        case 1:
            result.status = SR_SOLVED;
            result.solution = state.solution;
            break;
        default:
            result.status = SR_MULTIPLE;
            result.solution = state.solution;
            break;
    }

    free(solutionObjects);
    free_constraint_table(table);

    return result;
}
