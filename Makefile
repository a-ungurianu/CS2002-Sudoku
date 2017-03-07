CC = clang
CFLAGS = -c -std=c99 -Wall -Wextra -g -fsanitize=address
LDFLAGS = -Wall -Wextra -g -fsanitize=address
OBJ_DIR = out
SRC_DIR = src

DEPS = ${SRC_DIR}/sudoku.h ${SRC_DIR}/sudoku_io.h ${SRC_DIR}/sudoku.h ${SRC_DIR}/sudoku_solve.h ${SRC_DIR}/sudoku_checking.h

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c ${DEPS}
	-mkdir -p out
	${CC} ${CFLAGS} $< -o $@

sudoku_check: ${OBJ_DIR}/sudoku_check.o ${OBJ_DIR}/sudoku_io.o ${OBJ_DIR}/sudoku.o ${OBJ_DIR}/sudoku_solve.o ${OBJ_DIR}/sudoku_checking.o
	${CC} ${LDFLAGS} $^ -o $@

sudoku_solver: ${OBJ_DIR}/sudoku_solver.o ${OBJ_DIR}/sudoku_solve.o ${OBJ_DIR}/sudoku_io.o ${OBJ_DIR}/sudoku.o ${OBJ_DIR}/sudoku_checking.o
	${CC} ${LDFLAGS} $^ -o $@

sudoku_advanced: ${OBJ_DIR}/sudoku_solver.o ${OBJ_DIR}/sudoku_solve_advanced.o ${OBJ_DIR}/sudoku_io.o ${OBJ_DIR}/sudoku.o ${OBJ_DIR}/sudoku_checking.o
	${CC} ${LDFLAGS} $^ -o $@

test:
	stacscheck /cs/studres/CS2002/Practicals/Practical3-C2/stacscheck/

test_local:
	stacscheck stacscheck/

clean:
	-rm out/*
	-rm sudoku_solver sudoku_advanced sudoku_check
