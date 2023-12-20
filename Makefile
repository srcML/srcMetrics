include padkit/compile.mk

VERSION_SRCMETRICS=0.0.2

PREPROCESSOR_MACROS=                               \
    -DVERSION_SRCMETRICS=\"${VERSION_SRCMETRICS}\" \
    -DUNIT_COUNT_GUESS=1024                        \
    -DFN_COUNT_GUESS=16384                         \
    -DSTMT_COUNT_GUESS=524288                      \
    -DLABEL_COUNT_GUESS=1024                       \
    -DCALL_COUNT_GUESS=1024                        \
    -DELEMENT_LEN_GUESS=16

INCS=-Iinclude -Ipadkit/include
CFILES=src/*.c src/*/*.c src/*/*/*.c

SRCML_LIB=lib/${OS}/${ARCH}/libsrcml.a
SRCSAX_LIB=lib/${OS}/${ARCH}/libsrcsax.a

DEBUG_LIBS=${PADKIT_DEBUG_LIB} ${SRCML_LIB} ${SRCSAX_LIB}
RELEASE_LIBS=${PADKIT_LIB} ${SRCML_LIB} ${SRCSAX_LIB}

LIBS=${SRCML_LIB} ${SRCSAX_LIB} padkit/lib/libpadkit.a

ifeq (${OS},Darwin)
BIN_SRCMETRICS=bin/srcmetrics
else
ifeq (${OS},Linux)
BIN_SRCMETRICS=bin/srcmetrics
else
BIN_SRCMETRICS=bin/srcmetrics.exe
endif
endif

${BIN_SRCMETRICS}:          \
    bin                     \
    padkit/compile.mk       \
    padkit/lib/libpadkit.a  \
    ; ${COMPILE} ${PREPROCESSOR_MACROS} ${INCS} ${LIBS} ${CFILES} -o ${BIN_SRCMETRICS}

.PHONY: all clean documentation

all: ${BIN_SRCMETRICS}

bin: ; mkdir bin

clean: ; rm -rf *.gcno *.gcda *.gcov bin/* html latex

documentation: ; doxygen

padkit: ; git clone https://github.com/yavuzkoroglu/padkit.git

padkit/compile.mk: padkit; ${make padkit/compile.mk}

padkit/lib/libpadkit.a: padkit; make -C padkit lib/libpadkit.a
