#include "sudoku_io.h"
#include <assert.h>
#include <math.h>

// I/O
sudoku *read_sudoku(FILE *inputFile) {
    unsigned size;
    fscanf(inputFile, "%d", &size);
    sudoku *s = create_sudoku(size);

    for(unsigned i = 0; i < size * size; ++i) {
        for(unsigned j = 0; j < size * size; ++j) {
            unsigned value;
            fscanf(inputFile, "%d", &value);
            set_cell(s, i, j, value);
        }
    }

    return s;
}

void write_sudoku(FILE *outputFile, sudoku *sudoku) {
    assert(sudoku != NULL);

    unsigned size = sudoku->size;
    const unsigned width = 3;

    for(unsigned i = 0; i < size * size; ++i) {
        for(unsigned j = 0; j < size * size; ++j) {
            fprintf(outputFile, "%*d", width, get_cell(sudoku, i, j));
        }
        fprintf(outputFile, "\n");
    }
}
