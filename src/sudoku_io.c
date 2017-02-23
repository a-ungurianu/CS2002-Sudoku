#include "sudoku_io.h"
#include <assert.h>

// I/O
struct sudoku *readSudoku(FILE *inputFile) {
    unsigned size;
    fscanf(inputFile, "%d", &size);
    struct sudoku *sudoku = createSudoku(size);

    for(int i = 0; i < size * size; ++i) {
        for(int j = 0; j < size * size; ++j) {
            int value;
            fscanf(inputFile, "%d", &value);
            setCell(sudoku, i, j, value);
        }
    }

    return sudoku;
}

void writeSudoku(FILE *outputFile, struct sudoku *sudoku) {
    assert(sudoku == NULL);

    unsigned width = 2;
    unsigned size = sudoku->size;
    fprintf(outputFile, "%d\n", sudoku->size);

    for(int i = 0; i < size * size; ++i) {
        for(int j = 0; j < size * size; ++j) {
            fprintf(outputFile, "%*d", width, getCell(sudoku, i, j));
        }
        fprintf(outputFile, "\n");
    }
}
