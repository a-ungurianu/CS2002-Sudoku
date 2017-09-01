# Sudoku solver

Solves sudokus by translating the given square into an exact cover problem and solve it using Knuth's Algorithm X
with the dancing links optimisation.

## Build

    # Build the sudoku solver
    make sudoku_advanced

    # Build a simpler sudoku solver that uses backtracking
    make sudoku_solver

    # Build a quick sudoku status checker (complete, incomplete, invalid)    
    make sudoku_checker

## Usage

All three executables read the sudoku square from the standard input and when a valid square is read, the programs will output and then terminate.

### Input format

On the first line, a number ```N``` representing the width of a box (a 9x9 sudoku would have N=3)
On the next ```N*N``` lines, ```N*N``` numbers representing the value of that cell, or ```0``` if the cell is yet to be filled in.