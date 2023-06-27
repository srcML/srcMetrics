# CC=gcc-13 -std=c99 -Wall -Wextra
COVARGS=--coverage -fprofile-arcs -ftest-coverage
CCARGS=-std=c99 -Weverything
CC=clang
CFILES=src/util/*.c src/srcmetrics/*.c
INCLUDES=-Iinclude
LIBRARIES=-lsrcml -lsrcsax
DEBUGFLAGS=-g
SRCMETRICS=src/srcmetrics.c
EXEC=bin/srcmetrics
SILENCEDWARNINGS=-Wno-poison-system-directories -Wno-padded -Wno-unused-parameter
COVERAGE=${CC} ${COVARGS} ${CCARGS} ${DEBUGFLAGS} ${INCLUDES} ${LIBRARIES} ${CFILES}
DEBUG=${CC} ${CCARGS} ${DEBUGFLAGS} ${INCLUDES} ${LIBRARIES} ${CFILES}

compile: ; @\
    ${CC} -O2 ${CCARGS} ${SILENCEDWARNINGS} ${INCLUDES} ${LIBRARIES} ${CFILES} ${SRCMETRICS} -o ${EXEC}

release: clean bin compile documentation

documentation: ; @\
    doxygen

coverage: clean bin; @\
    ${COVERAGE} ${SRCMETRICS} -o ${EXEC}

debug: clean bin; @\
    ${DEBUG} ${SRCMETRICS} -o ${EXEC}

bin: ; @\
    mkdir bin

tests: test230619

test230619: clean bin; @\
    ${COVERAGE} src/tests/test230619.c -o bin/test230619.out

clean: ; @\
    rm -rf *.gcno *.gcda *.gcov bin/* html latex
