# CC=gcc-13 -std=c99 -Wall -Wextra
COVARGS=--coverage -fprofile-arcs -ftest-coverage
CCARGS=-std=c99 -Weverything -Wno-poison-system-directories
CC=clang
CFILES=src/util/*.c src/srcmetrics/*.c
INCLUDES=-Iinclude
DEBUGFLAGS=-g
EXEC=bin/srcmetrics
DEBUG=${CC} ${COVARGS} ${CCARGS} ${DEBUGFLAGS} ${INCLUDES} ${CFILES}

release: bin compile

debug: clean bin; @\
    ${DEBUG} src/main.c -o ${EXEC}

bin: ; @\
    mkdir bin

compile: ; @\
    ${CC} ${CCARGS} ${INCLUDES} ${CFILES} src/main.c -o ${EXEC}

tests: test230619

test230619: clean bin; @\
    ${DEBUG} src/tests/test230619.c -o bin/test230619.out

clean: ; @\
    rm -rf *.gcno *.gcda *.gcov bin/*
