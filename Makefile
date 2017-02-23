CC = clang
CFLAGS = -c
OBJ_DIR = out
SRC_DIR = src

${OBJ_DIR}/sudoku.o: ${SRC_DIR}/sudoku.h ${SRC_DIR}/sudoku.c
	@mkdir -p out
	${CC} ${CFLAGS} ${SRC_DIR}/sudoku.c -o ${OBJ_DIR}/sudoku.o
