#include "sudoku.h"
#include "sudoku_io.h"
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

typedef struct {
    unsigned size;
    char* name;
} column_data;

typedef struct {
    unsigned row;
    unsigned col;
    unsigned value;
} cell_data;

typedef union {
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

typedef struct {
    data_object *head;
} constraint_table;


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
    colData->size = 0;
    colData->name = name;
    data_object* dataObj = malloc(sizeof(data_object));
    dataObj->data.columnData = colData;
    dataObj->up = dataObj;
    dataObj->down = dataObj;
    dataObj->left = table->head->left;
    dataObj->column = dataObj;
    table->head->left->right = dataObj;
    table->head->left = dataObj;
    dataObj->right = table->head;

    return dataObj;
}

static data_object *add_constraint_to_column(data_object *columnHeader, unsigned row, unsigned col, unsigned val) {
    data_object* dataObj = malloc(sizeof(data_object));
    dataObj->up = columnHeader->up;
    columnHeader->up->down = dataObj;
    columnHeader->up = dataObj;
    dataObj->down = columnHeader;
    dataObj->column = columnHeader;
    cell_data *cellData = malloc(sizeof(cell_data));
    cellData->row = row;
    cellData->col = col;
    cellData->value = val;
    dataObj->data.cellData = cellData;
    columnHeader->data.columnData->size++;

    return dataObj;
}

static void removeZeroColumns(constraint_table *table) {
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

typedef unsigned __int128 uint128_t;

static check_result check_list(int* values, unsigned size) {
    unsigned listSize = size * size;
    uint128_t valuesSeenSet = 0; // Using this as a bit set.

    for(unsigned i = 0; i < listSize; ++i) {
        if(values[i] != 0) {
            if((valuesSeenSet & (1 << values[i])) != 0) {
                return CR_INVALID;
            }
            else {
                valuesSeenSet |= 1 << values[i];
            }
        }
    }
    for(unsigned i = 1; i <= listSize; ++i) {
        if((valuesSeenSet & (1 << i)) == 0) {
            return CR_INCOMPLETE;
        }
    }
    return CR_COMPLETE;
}

static int* checkValuesBuffer;

static bool check_update(sudoku *s, position pos) {

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

static constraint_table *generate_table(sudoku *s) {
    constraint_table *table = malloc(sizeof(constraint_table));
    table->head = malloc(sizeof(data_object));
    table->head->left = table->head;
    table->head->right = table->head;

    unsigned sectionSize = s->size * s->size;

    data_object** rowColumnHeaders = malloc(sizeof(data_object*) * sectionSize * sectionSize);

    //Add Row-Column constraint columns
    for(unsigned r = 0; r < sectionSize; ++r) {
        for(unsigned c = 0; c < sectionSize; ++c) {
            char * name = malloc(sizeof(char) * 20);
            sprintf(name, "R%dC%d", r + 1, c + 1);
            rowColumnHeaders[r * sectionSize + c] = add_column_header(table, name);
        }
    }


    data_object** rowNumberHeaders = malloc(sizeof(data_object*) * sectionSize * sectionSize);

    //Add Row-Number constraint columns
    for(unsigned r = 0; r < sectionSize; ++r) {
        for(unsigned val = 0; val < sectionSize; ++val) {
            char * name = malloc(sizeof(char) * 20);
            sprintf(name, "R%d#%d", r + 1, val + 1);
            rowNumberHeaders[r * sectionSize + val] = add_column_header(table, name);
        }
    }

    data_object** columnNumberHeaders = malloc(sizeof(data_object*) * sectionSize * sectionSize);

    //Add Column-Number constraint columns
    for(unsigned c = 0; c < sectionSize; ++c) {
        for(unsigned val = 0; val < sectionSize; ++val) {
            char * name = malloc(sizeof(char) * 20);
            sprintf(name, "C%d#%d", c + 1, val + 1);
            columnNumberHeaders[c * sectionSize + val] = add_column_header(table, name);
        }
    }

    data_object** boxNumberHeaders = malloc(sizeof(data_object*) * sectionSize * sectionSize);

    //Add Box-Number constraint columns
    for(unsigned r = 0; r < s->size; ++r) {
        for(unsigned c = 0; c < s->size; ++c) {
            for(unsigned val = 0; val < sectionSize; ++val) {
                char * name = malloc(sizeof(char) * 20);
                sprintf(name, "BR%dC%d#%d", r + 1, c + 1, val + 1);
                boxNumberHeaders[(r * s->size + c) * sectionSize + val] = add_column_header(table, name);
            }
        }
    }
    unsigned no_zeros = 0, no_constraints = 0;
    for(unsigned row = 0; row < sectionSize; ++row) {
        for(unsigned col = 0; col < sectionSize; ++col) {
            if(get_cell(s, row, col) == 0) {
                no_zeros ++;
                for(unsigned val = 0; val < sectionSize; ++val) {
                    set_cell(s, row, col, val+1);
                    if(check_update(s, (position){row,col})) {
                        no_constraints++;
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

    printf("No. zeros: %d\n", no_zeros);
    printf("No. constraints: %d\n", no_constraints);
    removeZeroColumns(table);

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

static data_object ** solutionObjects;

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

static void cover_column(data_object *c) {
    c->right->left = c->left;
    c->left->right = c->right;

    data_object* i = c->down;
    while(i != c) {
        data_object* j = i->right;
        while(j != i) {
            j->down->up = j->up;
            j->up->down = j->down;
            j->column->data.columnData->size--;
            j = j->right;
        }
        i = i->down;
    }
}

void DEBUG_print_hor_list_rightwards(data_object *source) {
    data_object *current = source;

    do {
        cell_data *cellData = current->data.cellData;
        printf("(R%d C%d #%d), ", cellData->row + 1, cellData->col + 1, cellData->value + 1);
        current = current->right;
    } while ( current != source );
    printf("\n");
}

static void uncover_column(data_object *c) {
    data_object* i = c->up;
    while(i != c) {
        data_object* j = i->left;
        while(j != i) {
            j->column->data.columnData->size ++;
            j->down->up = j;
            j->up->down = j;
            j = j->left;
        }
        i = i->up;
    }
    c->right->left = c;
    c->left->right = c;
}


typedef struct {
    int no_solutions;
    sudoku *current;
    sudoku *solution;
} solve_state;

static sudoku * fill_in_sudoku(sudoku *s, data_object** thingsToFill, unsigned noThingsToFill) {
    sudoku *solved = copy_sudoku(s);
    printf("No. spaces: %d\n", noThingsToFill);

    for(unsigned i = 0; i < noThingsToFill; ++i) {
        cell_data *cellData = thingsToFill[i]->data.cellData;
        set_cell(solved, cellData->row, cellData->col, cellData->value + 1);
    }

    return solved;
}

static void solve_table(constraint_table *table, solve_state* state, unsigned k) {
    data_object* h = table->head;

    if(h->right == h) {
        // We have a solution
        // Yay
        state->no_solutions++;
        if(state->no_solutions == 1) {
            assert(state->solution == NULL);
            state->solution = fill_in_sudoku(state->current, solutionObjects, k);
        }
        else {
            assert(state->solution != NULL);
            free_sudoku(state->solution);
            state->solution = fill_in_sudoku(state->current, solutionObjects, k);
        }
    }
    else {
        // Choose a column header.
        data_object* c = get_smallest_column(table);

        // Cover column
        cover_column(c);
        data_object* r = c->down;

        while(r != c) {
            solutionObjects[k] = r;
            
            data_object* j = r->right;
            while(j != r) {
                // Cover column for currentRowObj
                cover_column(j->column);
                j = j->right;
            }
            solve_table(table, state, k + 1);
            r = solutionObjects[k];
            assert(c == r->column);
            c = r->column;

            j = r->left;
            while(j != r) {
                // Uncover column for currentRowObj
                uncover_column(j->column);
                j = j->left;
            }
            r = r->down;
        }
        // Uncover column
        uncover_column(c);
    }
}


int main() {
    sudoku * input = read_sudoku(stdin);

    checkValuesBuffer = malloc(sizeof(int) * input->size * input->size);
    solutionObjects = malloc(sizeof(data_object*) * 10000); // Compute the number by counting the number of zeros.

    constraint_table *table = generate_table(input);
    write_table(table, stdout);

    solve_state state = (solve_state){0,input,NULL};
    solve_table(table, &state, 0);

    switch (state.no_solutions) {
        case 0:
            printf("UNSOLVABLE\n");
            break;
        case 1:
            // write_sudoku(stdout, state.solution);
            break;
        default:
            printf("MULTIPLE\n");
            break;
    }

    free(solutionObjects);
    free(checkValuesBuffer);
    free_sudoku(input);
    free_constraint_table(table);
    return 0;
}
