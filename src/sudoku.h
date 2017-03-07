/*
    \file sudoku.h
    \brief The sudoku structure and a set of functions to access different portions of it
*/

#ifndef SUDOKU_H
#define SUDOKU_H

/*
    A structure that holds a suduku
*/
typedef struct {
    unsigned size; //< the size of the sudoku
    int* cells; //< an array that holds all the values of the sudoku
} sudoku;

/*
    Helper struct holding a row and column pair.
*/
typedef struct {
    unsigned row;
    unsigned col;
} position;

/**********************
 *  Utility functions *
 **********************/

/*
    Computes the number of cells in the given sudoku.

    \param sudoku sudoku for which to calculate the number of cells

    \return the number of cells
*/
unsigned get_no_cells(const sudoku *sudoku);
/************************
 *  Allocation handling *
 ************************/

/*
    Allocate a new uninitialized sudoku of the given size.

    \param size the size of the sudoku

    \return a new heap-allocated sudoku of the given size
*/
sudoku *create_sudoku(unsigned size);

/*
    Create a new copy of the given sudoku.

    \param sudoku the sudoku to be copied

    \return a new heap-allocated copy of the given sudoku
*/
sudoku *copy_sudoku(const sudoku* sudoku);

/*
    Free the memory of the given sudoku.

    \param sudoku the sudoku to be freed
*/
void free_sudoku(sudoku* sudoku);

/*********************
 *  Getter functions *
 *********************/

/*
    Retrieve the cell at the given row and column

    \param sudoku sudoku to look index
    \param row the row of the cell
    \param col the column of the cell

    \return the value of the cell at that location
*/
int get_cell(const sudoku *sudoku, unsigned row, unsigned col);

/*
    Fill in a buffer with the values in a given square

    \param sudoku where to retrieve the square from
    \param squareRow the row of squares where this square sits
    \param squareCol the column of squares where this square sits
    \param dest the buffer to be filled with the found values (buffer should have room for all the values)

    Square coordinate system:
    \ row  0     1     2
 col \
        ╔═╤═╤═╦═╤═╤═╦═╤═╤═╗
        ╟─┼─┼─╫─┼─┼─╫─┼─┼─╢
     0  ╟─┼─┼─╫─┼─┼─╫─┼─┼─╢
        ╠═╪═╪═╬═╪═╪═╬═╪═╪═╣
        ╟─┼─┼─╫─┼─┼─╫─┼─┼─╢
     1  ╟─┼─┼─╫─┼─┼─╫─┼─┼─╢
        ╠═╪═╪═╬═╪═╪═╬═╪═╪═╣
        ╟─┼─┼─╫─┼─┼─╫─┼─┼─╢
     2  ╟─┼─┼─╫─┼─┼─╫─┼─┼─╢
        ╚═╧═╧═╩═╧═╧═╩═╧═╧═╝
*/
void get_square(const sudoku *sudoku, unsigned squareRow, unsigned squareCol, int* dest);

/*
    Fill in a buffer with the values in a given row

    \param sudoku where to retrieve the row from
    \param row the index of the row of values to retrieve
    \param dest the buffer to be filled with the found values (buffer should have room for all the values)
*/
void get_row(const sudoku *sudoku, unsigned row, int* dest);

/*
    Fill in a buffer with the values in a given column

    \param sudoku where to retrieve the column from
    \param col the index of the column of values to retrieve
    \param dest the buffer to be filled with the found values (buffer should have room for all the values)
*/
void get_col(const sudoku *sudoku, unsigned col, int* dest);

// Setter function

/*
    Set the value of the sudoku at the given coordinates to the given values

    \param sudoku that needs to be updated
    \param row the row of the cell to be set
    \param col the column of the cell to be set
    \param value the value to set the cell to
*/
void set_cell(sudoku *sudoku, unsigned row, unsigned col, int value);

/************************
 *  Position converting *
 ************************/

 /*
    Given an index in a 1-d array, return the row and column position of that index
    \param sudoku the sudoku the index is a part of
    \param index the index value to be converted

    \return the position (row and column) that this index represents
 */
position index_to_position(const sudoku *s, unsigned index);

/*
    Given a position (row and column) return the index where the value is located
    \param sudoku the sudoku the position is a part of
    \param pos the position to be converted

    \return the index synonymous to the given position
*/
unsigned position_to_index(const sudoku *s, position pos);

#endif
