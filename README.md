# Sudoku solver

Solves sudokus by translating the given square into an exact cover problem and solve it using Knuth's Algorithm X
with the dancing links optimisation.

## Build
``` bash
    # Build the sudoku solver
    make sudoku_advanced

    # Build a simpler sudoku solver that uses backtracking
    make sudoku_solver

    # Build a quick sudoku status checker (complete, incomplete, invalid)    
    make sudoku_checker
```

## Usage

All three executables read the sudoku square from the standard input and when a valid square is read, the programs will output and then terminate.

### Input format

On the first line, a number ```N``` representing the width of a box (a 9x9 sudoku would have N=3)

On the next ```N*N``` lines, ```N*N``` numbers representing the value of that cell, or ```0``` if the cell is yet to be filled in.

Examples can be found in ```stacscheck/2_sudoku_solver_tests```

## Overview

In this practical, we have to write a sudoku checker and solver capable of handling various sized sudokus.

## Sudoku structure
The sudoku structure (which can be found in the ```sudoku.h``` file) is quite simple. It keeps the size of the sudoku, using the definition specified in the practical and a pointer to an array of ints which represents the each number in the sudoku square, with zeros representing empty spaces. 

Together with this structure, a couple of helper methods were defined as well:
- ```create_sudoku```, ```copy_sudoku``` and ```free_sudoku``` are responsible for memory management of the sudoku structure.
- ```get_row```, ```get_col```, ```get_square``` are used to retrieve different subsections of our sudoku. To improve the memory footprint, a buffer has to be passed in which will be filled with the requested values.

## Input handling
To improve code structure, the input and output handling were separated into a header file. Also instead of relying on having our input always given to us from the ```stdin``` and having to write our output into ```stdout```, I've made the input and output functions take a file as a parameter. In the main methods that use those functions, the ```stdin``` and ```stdout``` file descriptors defined by the ```stdin.h``` header are passed to the function to imitate the initial behaviour.

## Checking
The checking methods (found in the ```sudoku_checking.h``` header) implement the required specification to the letter. 
The ```check_list``` method takes an array of ```int```s and checks to see if any non-zero numbers are duplicated (for the invalid state), or are missing (for the incomplete state). To improve the performance of this function, I am using a unsigned integer as a bit set, each bit representing if a given value has been seen before or not. To be able to have this function work on our largest inputs (81x81 sudokus, therefore 81 different possible values for each cell), I am making use of the ```__int128``` type which is accessible in recent versions of clang and gcc. Unfortunately, using this reduces portability somewhat, having only 64-bit word sized machines able to use this. Fortunately, the lab machines support this fully.

The ```check_sudoku``` methods simply combines the getters defined in ```sudoku.h``` with the aforementioned function to check if the sudoku constraints hold for all rows, columns and squares.

## Basic solver
The basic solver uses an well known algorithm known as backtracking. To be able to use backtracking, we first have to define a search tree on which the backtracking algorithm should work on. This is done by beginning with the given sudoku and trying to fill every cell with all the possible values. 

If I would only use this strategy, we would have to generate all the possible sudokus from the given empty spaces and then test to see if the produced sudoku is valid. This can be improved drastically by, after each value has been filled in, check in its respective row, column and box for any clashes (repeated numbers). This algorithm alone passes all the basic tests and all by the last of the advanced tests. 

## Advanced solver

After running the tests, it became obvious that the very hard test given to us was beyond the capabilities of my basic solver (running the solver on it for 2 hours produced no result). After a bit of research, converting sudoku to an exact cover problem and then solving it using Knuth's Algorithm X augmented with the sparse matrix and the dancing links way of manipulating it seemed something achievable in the given time frame. 

The exact cover problem can be stated as follows: given a matrix of 1's and 0's find a subset of rows such that the column-wise sum of the subset of rows equals the vector of all 1's.

To translate the sudoku solving problem into an exact cover problem, we first need to define the columns of our exact cover matrix:
- The row-column constraint column represents the constraint that for every row and column, we can only have one number. (We have 1 matrix column for each pair of rows and columns)
- The row-number constraint column represents the constraint that every row should have only one of the same number. (We have 1 matrix column for each pair of rows and numbers)
- The column-number constraint column represents the constraint that every column should have only one of the same number. (We have 1 matrix column for each pair of columns and numbers)
- The box-number constraint column represents the constraint that every box should have only one of the same number. (We have 1 matrix column for each pair of boxes and numbers).

Furthermore, we define one row for each combination of rows, columns and numbers. The first constraint seems obvious, but it is necessary to make sure that we have exactly one value for each row and column, thus providing the solution for our sudoku.

To solve the exact cover, we use Algorithm X which works as following (A representing the matrix to be solved):
```
    If A is empty, the problem is solved; terminate successfully
    
    Otherwise, choose a column, c (deterministically)
    
    Choose a row, r such that A[r,c] = 1 (non-deterministically)
    
    Include r in the partial solution
    
    For each j such that A[r,j] = 1
        delete column j from matrix A
        
        for each i such that A[i,j] = 1
            delete row i from matrix A

    Repeat this algorithm recursively on the reduced matrix A 
```

To improve this algorithm, the remove row and remove column operations are optimized by holding a sparse matrix of all the ones, connected to the next and previous ones on their respective row and column in a circular fashion.

This algorithm provided a great improvement in speed, solving the very hard test in 0.01s and solving most of the additional tests provided by Chris.

For more information, feel free to read the code. I've added a reasonable amount of comments which should make it quite easy to understand.

# Testing

The ```stacscheck``` tool was heavily used to make sure that my program solved the sudokus correctly and help reveal a lot of bugs that would have otherwise pass through unnoticed.

Because we have to do manual memory management, ```valgrind``` proved to be a great tool in finding all the memory leaks and also the places where we accessed unallocated memory without the program crashing in anyway.

The test the difference between the basic solver and the advanced one, I've wrote a short script that measures the run time of both programs using the ```time``` utility.

Output of all three can be found in the submission folder.

# Conclusion

This practical was a great opportunity to learn more about the C programming language, having to use dynamic memory allocation and ```struct```s, and also made me write better structured code.

I've also gained a better understanding of how makefiles work and also how to use ```valgrind``` and various compiler flags to make sure the programs I write are memory safe.
