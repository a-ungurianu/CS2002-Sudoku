#include "sudoku.h"
#include "sudoku_io.h"
#include "sudoku_solve.h"
#include "sudoku_checking.h"
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>


typedef struct table_links {
    struct table_links *up;
    struct table_links *down;
    struct table_links *left;
    struct table_links *right;
    struct column_object *column;
} table_links;

typedef struct column_object {
    table_links links;
    unsigned size;
    char* name;
} column_object;

typedef struct cell_object {
    table_links links;
    unsigned row;
    unsigned col;
    unsigned value;
} cell_object;


typedef struct constraint_table {
    table_links *head;
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

static void free_column_object(column_object* data) {
    free(data->name);
    free(data);
}

static void free_column(column_object* header) {
    table_links *current = header->links.down;
    while(current != header) {
        current = current->down;
        free(current->up);
    }
    free_column_object(header);
}

static void free_constraint_table(constraint_table *table) {
    table_links *current = table->head->right;
    while(current != table->head) {
        current = current->right;
        free_column(current->left);
    }
    free(table->head);
    free(table);
}

static void link_left_of(table_links *node, table_links *toAdd) {
    if(node == NULL) {
        toAdd->right = toAdd;
        toAdd->left = toAdd;
    }
    else {
        toAdd->left = node->left;
        node->left->right = toAdd;
        node->left = toAdd;
        toAdd->right = node;
    }
}

static void link_above_of(table_links *node, table_links *toAdd) {
    if(node == NULL) {
        toAdd->down = toAdd;
        toAdd->up = toAdd;
    }
    else {
        toAdd->up = node->up;
        toAdd->down = node;
        node->up->down = toAdd;
        node->up = toAdd;
    }
}

static void cover_left_right(table_links *nodeToCover) {
    nodeToCover->left->right = nodeToCover->right;
    nodeToCover->right->left = nodeToCover->left;
}

static void cover_up_down( table_links *nodeToCover) {
    nodeToCover->up->down = nodeToCover->down;
    nodeToCover->down->up = nodeToCover->up;
}

static void uncover_left_right(table_links *nodeToUncover) {
    nodeToUncover->left->right = nodeToUncover;
    nodeToUncover->right->left = nodeToUncover;
}

static void uncover_up_down(table_links *nodeToUncover) {
    nodeToUncover->down->up = nodeToUncover;
    nodeToUncover->up->down = nodeToUncover;
}

static column_object *add_column_header(constraint_table *table, char *name) {
    column_object* columnObj = malloc(sizeof(column_object));
    assert(columnObj != NULL);

    columnObj->size = 0;
    columnObj->name = name;

    columnObj->links.column = columnObj;

    link_above_of(NULL, &columnObj->links);

    link_left_of(table->head, &columnObj->links);

    return columnObj;
}

static cell_object *add_constraint_to_column(column_object *columnHeader, unsigned row, unsigned col, unsigned val) {
    cell_object* cellObj = malloc(sizeof(cell_object));
    assert(cellObj != NULL);

    cellObj->links.column = columnHeader;

    link_above_of(&columnHeader->links, &cellObj->links);

    cellObj->row = row;
    cellObj->col = col;
    cellObj->value = val;
    columnHeader->size++;

    return cellObj;
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
    column_object *current = table->head->right;

    while(current != table->head) {
        if(current->size == 0) {;
            cover_left_right(&current->links);
            column_object *toDelete = current;
            current = current->links.right;
            free_column(toDelete);
        } else {
            current = current->links.right;
        }
    }
}

static constraint_table *generate_table(sudoku *s) {
    constraint_table *table = malloc(sizeof(constraint_table));
    assert(table != NULL);

    table->head = malloc(sizeof(table_links));
    assert(table->head != NULL);

    link_left_of(NULL, table->head);

    unsigned sectionSize = s->size * s->size;

    column_object** rowColumnHeaders = malloc(sizeof(column_object*) * sectionSize * sectionSize);
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


    column_object** rowNumberHeaders = malloc(sizeof(column_object*) * sectionSize * sectionSize);
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

    column_object** columnNumberHeaders = malloc(sizeof(column_object*) * sectionSize * sectionSize);
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

    column_object** boxNumberHeaders = malloc(sizeof(column_object*) * sectionSize * sectionSize);
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
                        column_object *rowColumnHeader = rowColumnHeaders[row * sectionSize + col];
                        column_object *rowNumberHeader = rowNumberHeaders[row * sectionSize + val];
                        column_object *colNumberHeader = columnNumberHeaders[col * sectionSize + val];
                        unsigned boxRow = row / s->size;
                        unsigned boxCol = col / s->size;
                        column_object *boxNumberHeader = boxNumberHeaders[(boxRow * s->size + boxCol) * sectionSize + val];

                        cell_object *rowColumnConstraint = add_constraint_to_column(rowColumnHeader, row, col, val);
                        cell_object *rowNumberConstraint = add_constraint_to_column(rowNumberHeader, row, col, val);
                        cell_object *colNumberConstraint = add_constraint_to_column(colNumberHeader, row, col, val);
                        cell_object *boxNumberConstraint = add_constraint_to_column(boxNumberHeader, row, col, val);

                        link_left_of(NULL, &rowColumnConstraint->links);
                        link_left_of(&rowColumnConstraint->links, &rowNumberConstraint->links);
                        link_left_of(&rowColumnConstraint->links, &colNumberConstraint->links);
                        link_left_of(&rowColumnConstraint->links, &boxNumberConstraint->links);

                        int i = 0; //DEBUG
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
    column_object * current = table->head->right;
    while(current != table->head) {
        fprintf(output, "%s(%d) :",current->name, current->size);

        cell_object *currentVal = current->links.down;
        while(currentVal != current) {
            fprintf(output, "(R%d C%d #%d) ", currentVal->row + 1, currentVal->col + 1, currentVal->value + 1);
            currentVal = currentVal->links.down;
        }
        fprintf(output, "\n");
        current = current->links.right;
    }
}

static cell_object **solutionObjects;

static column_object *get_smallest_column(constraint_table *table) {
    column_object * current = table->head->right;

    column_object *smallestColumn = NULL;
    unsigned smallestSize = UINT_MAX;
    while(current->links.right != table->head) {
        if(current->size < smallestSize) {
            smallestSize = current->size;
            smallestColumn = current;
        }
        current = current->links.right;
    }

    return smallestColumn;
}

static void cover_column(column_object *column) {
    cover_left_right(column);

    cell_object* rowToCover = column->links.down;
    while(rowToCover != column) {
        cell_object* attachedCell = rowToCover->links.right;
        while(attachedCell != rowToCover) {
            cover_up_down(&attachedCell->links);
            attachedCell->links.column->size--;
            attachedCell = attachedCell->links.right;
        }
        rowToCover = rowToCover->links.down;
    }
}

static void uncover_column(column_object *column) {
    cell_object* rowToUncover = column->links.up;
    while(rowToUncover != column) {
        cell_object* attachedCell = rowToUncover->links.left;
        while(attachedCell != rowToUncover) {
            attachedCell->links.column->size ++;
            uncover_up_down(&attachedCell->links);
            attachedCell = attachedCell->links.left;
        }
        rowToUncover = rowToUncover->links.up;
    }
    uncover_left_right(column);
}

static sudoku * fill_in_sudoku(const sudoku *s, cell_object** thingsToFill, unsigned noThingsToFill) {
    sudoku *solved = copy_sudoku(s);

    for(unsigned i = 0; i < noThingsToFill; ++i) {
        set_cell(solved, thingsToFill[i]->row, thingsToFill[i]->col, thingsToFill[i]->value + 1);
    }

    return solved;
}

static void solve_table(constraint_table *table, solve_state* state, unsigned depth) {
    if(state->no_solutions < 2) {
        table_links* head = table->head;

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
            column_object* smallestColumn = get_smallest_column(table);

            // Cover column
            cover_column(smallestColumn);
            cell_object* rowToCover = smallestColumn->links.down;

            while(rowToCover != smallestColumn) {
                solutionObjects[depth] = rowToCover;

                cell_object* attachedCell = rowToCover->links.right;
                while(attachedCell != rowToCover) {
                    // Cover column for attachedCell
                    cover_column(attachedCell->links.column);
                    attachedCell = attachedCell->links.right;
                }
                solve_table(table, state, depth + 1);
                rowToCover = solutionObjects[depth];

                attachedCell = rowToCover->links.left;
                while(attachedCell != rowToCover) {
                    // Uncover column for attachedCell
                    uncover_column(attachedCell->links.column);
                    attachedCell = attachedCell->links.left;
                }
                rowToCover = rowToCover->links.down;
            }
            // Uncover column
            uncover_column(smallestColumn);
        }
    }
}

solve_result solve_sudoku(const sudoku *input) {

    sudoku *toSolve = copy_sudoku(input);

    solutionObjects = malloc(sizeof(cell_object*) * no_empty_spaces(input)); // Compute the number by counting the number of zeros.
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

    free_sudoku(toSolve);
    free(solutionObjects);
    free_constraint_table(table);

    return result;
}
