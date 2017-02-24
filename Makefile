CC = clang
CFLAGS = -c -std=c99 -Wall -Wextra -g
LDFLAGS = -Wall -Wextra -lm -g
OBJ_DIR = out
SRC_DIR = src

${OBJ_DIR}/sudoku.o: ${SRC_DIR}/sudoku.h ${SRC_DIR}/sudoku.c
	@mkdir -p out
	${CC} ${CFLAGS} ${SRC_DIR}/sudoku.c -o $@

${OBJ_DIR}/sudoku_io.o: ${SRC_DIR}/sudoku_io.h ${SRC_DIR}/sudoku_io.c
	@mkdir -p out
	${CC} ${CFLAGS} ${SRC_DIR}/sudoku_io.c -o $@

${OBJ_DIR}/test_stuff.o: ${SRC_DIR}/sudoku_io.h ${SRC_DIR}/sudoku.h ${SRC_DIR}/test_stuff.c
	@mkdir -p out
	${CC} ${CFLAGS} ${SRC_DIR}/test_stuff.c -o $@

${OBJ_DIR}/sudoku_check.o: ${SRC_DIR}/sudoku_io.h ${SRC_DIR}/sudoku.h ${SRC_DIR}/sudoku_check.c
	@mkdir -p out
	${CC} ${CFLAGS} ${SRC_DIR}/test_stuff.c -o $@

test_stuff: ${OBJ_DIR}/test_stuff.o ${OBJ_DIR}/sudoku_io.o ${OBJ_DIR}/sudoku.o
	${CC} ${LDFLAGS} $^ -o $@

sudoku_check: ${OBJ_DIR}/sudoku_check.o ${OBJ_DIR}/sudoku_io.o ${OBJ_DIR}/sudoku.o
	${CC} ${LDFLAGS} $^ -o $@

clean:
	@rm out/*
