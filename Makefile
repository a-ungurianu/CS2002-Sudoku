CC = clang
CFLAGS = -c -std=c99 -Wall -Wextra -g
LDFLAGS = -Wall -Wextra -lm -g
OBJ_DIR = out
SRC_DIR = src

DEPS = ${SRC_DIR}/sudoku.h ${SRC_DIR}/sudoku_io.h ${SRC_DIR}/sudoku.h ${SRC_DIR}/sudoku_solve.h

${OBJ_DIR}/sudoku.o: ${SRC_DIR}/sudoku.c ${DEPS}
	-mkdir -p out
	${CC} ${CFLAGS} $< -o $@

${OBJ_DIR}/sudoku_solve.o: ${SRC_DIR}/sudoku_solve.c ${DEPS}
	-mkdir -p out
	${CC} ${CFLAGS} $< -o $@

${OBJ_DIR}/sudoku_io.o: ${SRC_DIR}/sudoku_io.c ${DEPS}
	-mkdir -p out
	${CC} ${CFLAGS} $< -o $@

${OBJ_DIR}/sudoku_check.o: ${SRC_DIR}/sudoku_check.c ${DEPS}
	-mkdir -p out
	${CC} ${CFLAGS} $< -o $@

${OBJ_DIR}/sudoku_solver.o: ${SRC_DIR}/sudoku_solver.c ${DEPS}
	-mkdir -p out
	${CC} ${CFLAGS} $< -o $@

${OBJ_DIR}/test_table_stuff.o: ${SRC_DIR}/test_table_stuff.c ${DEPS}
	-mkdir -p out
	${CC} ${CFLAGS} $< -o $@

sudoku_check: ${OBJ_DIR}/sudoku_check.o ${OBJ_DIR}/sudoku_io.o ${OBJ_DIR}/sudoku.o ${OBJ_DIR}/sudoku_solve.o
	${CC} ${LDFLAGS} $^ -o $@

sudoku_solver: ${OBJ_DIR}/sudoku_solver.o ${OBJ_DIR}/sudoku_io.o ${OBJ_DIR}/sudoku.o ${OBJ_DIR}/sudoku_solve.o
	${CC} ${LDFLAGS} $^ -o $@

sudoku_advanced: ${OBJ_DIR}/sudoku_solver.o ${OBJ_DIR}/sudoku_io.o ${OBJ_DIR}/sudoku.o ${OBJ_DIR}/sudoku_solve.o
	${CC} ${LDFLAGS} $^ -o $@

test_table_stuff: ${OBJ_DIR}/test_table_stuff.o ${OBJ_DIR}/sudoku_io.o ${OBJ_DIR}/sudoku.o
	${CC} ${LDFLAGS} $^ -o $@
test:
	stacscheck /cs/studres/CS2002/Practicals/Practical3-C2/stacscheck/

clean:
	-rm out/*
