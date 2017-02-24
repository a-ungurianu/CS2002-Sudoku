#include "sudoku_io.h"
#include <assert.h>
#include <math.h>

// I/O
sudoku *readSudoku(FILE *inputFile) {
    unsigned size;
    fscanf(inputFile, "%d", &size);
    sudoku *s = createSudoku(size);

    for(unsigned i = 0; i < size * size; ++i) {
        for(unsigned j = 0; j < size * size; ++j) {
            unsigned value;
            fscanf(inputFile, "%d", &value);
            setCell(s, i, j, value);
        }
    }

    return s;
}

void writeSudoku(FILE *outputFile, sudoku *sudoku) {
    assert(sudoku != NULL);

    unsigned size = sudoku->size;
    const unsigned width = 3;
    // fprintf(outputFile, "%d\n", sudoku->size);

    for(unsigned i = 0; i < size * size; ++i) {
        for(unsigned j = 0; j < size * size; ++j) {
            fprintf(outputFile, "%*d", width, getCell(sudoku, i, j));
        }
        fprintf(outputFile, "\n");
    }
}
