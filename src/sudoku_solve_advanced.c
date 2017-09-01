#include "sudoku.h"
#include "sudoku_io.h"
#include "sudoku_solve.h"
#include "sudoku_checking.h"
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

/*
    Structure used as a node for the 2d circular doubly linked list.

*/
typedef struct table_links {
    struct table_links *up;
    struct table_links *down;
    struct table_links *left;
    struct table_links *right;
    struct column_object *column; //< Pointer to the column this cell is a part of.
} table_links;

typedef struct column_object {
    table_links links;
    unsigned size; //< The number of elements in the array
    char* name; //< The name of the column (useful for debugging)
} column_object;

typedef struct cell_object {
    table_links links;
    unsigned row; //< The row this constraint represents
    unsigned col; //< The column this constraint represents
    unsigned value; //< The value this constraint represents
} cell_object;


typedef struct constraint_table {
    table_links *head;
} constraint_table;

typedef struct solve_state {
    int no_solutions; //< number of solutions found
    sudoku *current; //< the sudoku we're trying to solve
    cell_object **solutionObjects;
    sudoku *solution;
} solve_state;


/*
    Calculates the number of empty spaces (zeros) in the sudoku

    \param sudoku the sudoku in which to counting

    \return the number of zeros found
*/
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

/*
    Frees a column_object struct.

    \param object the object to be freed
*/
static void free_column_object(column_object* object) {
    free(object->name);
    free(object);
}

/*
    Frees a column, including all the elements attached to it.

    \param header the header of the column to be freed
*/
static void free_column(column_object* header) {
    table_links *current = header->links.down;
    while(current != (table_links*) header) {
        current = current->down;
        free(current->up);
    }
    free_column_object(header);
}

/*
    Frees all the values held in the constraint table

    \param table the constraint table to be freed
*/

static void free_constraint_table(constraint_table *table) {
    column_object *current = (column_object*) table->head->right;
    while((table_links*) current != table->head) {
        current = (column_object*) current->links.right;
        free_column((column_object*) current->links.left);
    }
    free(table->head);
    free(table);
}

/*
    Add a given node to the left of a node that's in a horizontal cyclic
    doubly linked list.

    \param node the node relative to which we are adding
    \paran toAdd the node to be added to the left of `node`
*/
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

/*
    Add a given node to above a node that's in a vertical cyclic
    doubly linked list.

    \param node the node relative to which we are adding
    \param toAdd the node to be added to above `node`
*/
static void link_above(table_links *node, table_links *toAdd) {
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

/*
    Link the left and right neighbour of a given node together, leaving the
    covered node's links intact.

    \param nodeToCover the node to be covered
*/
static void cover_left_right(table_links *nodeToCover) {
    nodeToCover->left->right = nodeToCover->right;
    nodeToCover->right->left = nodeToCover->left;
}

/*
    Link the up and down neighbour of a given node together, leaving the
    covered node's links intact.

    \param nodeToCover the node to be covered
*/
static void cover_up_down( table_links *nodeToCover) {
    nodeToCover->up->down = nodeToCover->down;
    nodeToCover->down->up = nodeToCover->up;
}

/*
    Restore the left and right neighbour of a given node, using the given node's
    saved links.

    \param nodeToCover the node to be uncovered
*/
static void uncover_left_right(table_links *nodeToUncover) {
    nodeToUncover->left->right = nodeToUncover;
    nodeToUncover->right->left = nodeToUncover;
}

/*
    Restore the up and down neighbour of a given node, using the given node's
    saved links.

    \param nodeToCover the node to be uncovered
*/
static void uncover_up_down(table_links *nodeToUncover) {
    nodeToUncover->down->up = nodeToUncover;
    nodeToUncover->up->down = nodeToUncover;
}

/*
    Add an empty column to the constraint table

    \param table the table to add the column to
    \param name the name of the column being added
*/
static column_object *add_column_header(constraint_table *table, char *name) {
    column_object* columnObj = malloc(sizeof(column_object));
    assert(columnObj != NULL);

    columnObj->size = 0;
    columnObj->name = name;

    columnObj->links.column = columnObj;

    link_above(NULL, &columnObj->links);

    link_left_of(table->head, &columnObj->links);

    return columnObj;
}

/*
    Add a constraint (1 in the exact cover matrix) to the given column.

    \param columnHeader the column to add to
    \param row --
    \param col -|-- values used to identify the matrix row.
    \param val --
*/
static cell_object *add_constraint_to_column(column_object *columnHeader, unsigned row, unsigned col, unsigned val) {
    cell_object* cellObj = malloc(sizeof(cell_object));
    assert(cellObj != NULL);

    cellObj->links.column = columnHeader;

    link_above(&columnHeader->links, &cellObj->links);

    cellObj->row = row;
    cellObj->col = col;
    cellObj->value = val;
    columnHeader->size++;

    return cellObj;
}

/*
    Checks if the row, column and square corresponding to this position are invalid.

    \param s the sudoke in which to check
    \param pos the position around which to check

    \returns if any of the row, column or square are invalid
*/
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

/*
    Removes all the columns that have no values attached to them.

    \param table the table to remove the zero-columns from
*/
static void remove_zero_columns(constraint_table *table) {
    column_object *current = (column_object*) table->head->right;

    while((table_links*) current != table->head) {
        if(current->size == 0) {;
            cover_left_right(&current->links);
            column_object *toDelete = current;
            current = (column_object*) current->links.right;
            free_column(toDelete);
        } else {
            current = (column_object*) current->links.right;
        }
    }
}

/*
    Generates a constraint table from a given sudoku grid

    This firstly generates all the columns of the constraint table and then
    fills them with 1s by iterating through all the possible rows, columns and,
    if the spaces is empty, values (which is equivalent to iterating through all
    the rows of the exact cover matrix) and add in the necessary 1s.

    \param s the partially filled sudoku grid

    \return the generated constraint table
*/
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

/*
    Writes a given constraint table to the given output stream

    Really useful function for debugging

    \param table the table to write_table
    \param output an output stream to write to
*/
static void write_table(constraint_table* table, FILE *output) {
    column_object * current = (column_object*) table->head->right;
    while((table_links*) current != table->head) {
        fprintf(output, "%s(%d) :",current->name, current->size);

        cell_object *currentVal = (cell_object*) current->links.down;
        while((table_links*) currentVal != (table_links*)current) {
            fprintf(output, "(R%d C%d #%d) ", currentVal->row + 1, currentVal->col + 1, currentVal->value + 1);
            currentVal = (cell_object*) currentVal->links.down;
        }
        fprintf(output, "\n");
        current = (column_object*) current->links.right;
    }
}

/*
    Finds the column with the smallest number of elements

    \param table table to search for the column

    \return the smallest column found
*/
static column_object *get_smallest_column(constraint_table *table) {
    column_object *current = (column_object*) table->head->right;

    column_object *smallestColumn = NULL;
    unsigned smallestSize = UINT_MAX;
    while(current->links.right != table->head) {
        if(current->size < smallestSize) {
            smallestSize = current->size;
            smallestColumn = current;
        }
        current = (column_object*) current->links.right;
    }

    return smallestColumn;
}

/*
    Temporarily remove the given column from its coresponding constraint table

    \param column the column to be removed

    \sa uncover_column
*/
static void cover_column(column_object *column) {
    cover_left_right((table_links*) column);

    table_links* rowToCover = column->links.down;
    while(rowToCover != (table_links*) column) {
        cell_object* attachedCell = (cell_object*) rowToCover->right;
        while((table_links*) attachedCell != rowToCover) {
            cover_up_down(&attachedCell->links);
            attachedCell->links.column->size--;
            attachedCell = (cell_object*) attachedCell->links.right;
        }
        rowToCover = rowToCover->down;
    }
}

/*
    Restore a column that was removed with the cover_column function

    \param column pointer to the header of the removed column

    \sa cover_column
*/
static void uncover_column(column_object *column) {
    table_links* rowToUncover = column->links.up;
    while(rowToUncover != (table_links*) column) {
        cell_object* attachedCell = (cell_object*) rowToUncover->left;
        while((table_links*) attachedCell != rowToUncover) {
            attachedCell->links.column->size ++;
            uncover_up_down((table_links*) attachedCell);
            attachedCell = (cell_object*) attachedCell->links.left;
        }
        rowToUncover = rowToUncover->up;
    }
    uncover_left_right((table_links*) column);
}

/*
    Fills in a sudoku with a list of cell objects (which represents the row, column and values to be filled)

    \param s the sudoku to be filled in
    \param thingsToFill array of cell_objects that represents what rows and columns have to be filled with what values.
    \param noThingsToFill the number of objects in the thingsToFill array.
*/
static sudoku * fill_in_sudoku(const sudoku *s, cell_object** thingsToFill, unsigned noThingsToFill) {
    sudoku *solved = copy_sudoku(s);

    for(unsigned i = 0; i < noThingsToFill; ++i) {
        set_cell(solved, thingsToFill[i]->row, thingsToFill[i]->col, thingsToFill[i]->value + 1);
    }

    return solved;
}

/*
    Solves the constraint table and updates the solve state accordingly

    Heavily inspired by the algorithm presented here: 
        https://en.wikipedia.org/wiki/Exact_cover#Sudoku

    \param table the table to be solved
    \param state the intermediary state of solving the sudoku
    \param depth the recursion depth of the algorithm
*/
static void solve_table(constraint_table *table, solve_state* state, unsigned depth) {
    if(state->no_solutions < 2) {
        table_links* head = table->head;

        if(head->right == head) {
            state->no_solutions++;
            if(state->no_solutions == 1) {
                assert(state->solution == NULL);
                state->solution = fill_in_sudoku(state->current, state->solutionObjects, depth);
            }
            else {
                assert(state->solution != NULL);
                free_sudoku(state->solution);
                state->solution = fill_in_sudoku(state->current, state->solutionObjects, depth);
            }
        }
        else {
            // Choose a column header.
            column_object* smallestColumn = get_smallest_column(table);

            // Cover column
            cover_column(smallestColumn);
            table_links* rowToCover = smallestColumn->links.down;

            while(rowToCover != (table_links*) smallestColumn) {
                state->solutionObjects[depth] = (cell_object*) rowToCover;

                cell_object* attachedCell = (cell_object*) rowToCover->right;
                while((table_links*) attachedCell != rowToCover) {
                    // Cover column for attachedCell
                    cover_column(attachedCell->links.column);
                    attachedCell = (cell_object*) attachedCell->links.right;
                }
                solve_table(table, state, depth + 1);
                rowToCover = (table_links*) state->solutionObjects[depth];

                attachedCell = (cell_object*) rowToCover->left;
                while((table_links*) attachedCell != rowToCover) {
                    // Uncover column for attachedCell
                    uncover_column(attachedCell->links.column);
                    attachedCell = (cell_object*) attachedCell->links.left;
                }
                rowToCover = rowToCover->down;
            }
            // Uncover column
            uncover_column(smallestColumn);
        }
    }
}

/*
    Solves the given sudoku, abiding to the interface defined in sudoku_solve.h

    \param input the sudoku to be solved

    \returns a solve result object which contains the solving status and a solution, if found
*/
solve_result solve_sudoku(const sudoku *input) {

    sudoku *toSolve = copy_sudoku(input);

    constraint_table *table = generate_table(toSolve);

    cell_object** solutionObjects = malloc(sizeof(cell_object*) * no_empty_spaces(input)); // Compute the number by counting the number of zeros.
    assert(solutionObjects);

    solve_state state = (solve_state){0,toSolve,solutionObjects, NULL};
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
