# CC=gcc-13 -std=c99 -Wall -Wextra
COVARGS=--coverage -fprofile-arcs -ftest-coverage
CCARGS=-std=c99 -Weverything -Wno-poison-system-directories
CC=clang -v
CFILES=src/util/*.c src/srcmetrics/*.c
INCLUDES=-Iinclude
LIBRARIES=-lsrcml
DEBUGFLAGS=-g
SRCMETRICS=src/srcmetrics.c
EXEC=bin/srcmetrics
DEBUG=${CC} ${COVARGS} ${CCARGS} ${DEBUGFLAGS} ${INCLUDES} ${LIBRARIES} ${CFILES}

release: bin compile

debug: clean bin; @\
    ${DEBUG} ${SRCMETRICS} -o ${EXEC}

bin: ; @\
    mkdir bin

compile: ; @\
    ${CC} ${CCARGS} ${INCLUDES} ${LIBRARIES} ${CFILES} ${SRCMETRICS} -o ${EXEC}

tests: test230619

test230619: clean bin; @\
    ${DEBUG} src/tests/test230619.c -o bin/test230619.out

clean: ; @\
    rm -rf *.gcno *.gcda *.gcov bin/*
