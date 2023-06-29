/**
 * @file chunk.c
 */
#include <ctype.h>
#include <stdlib.h>
#include "util/readfile.h"
#include "util/unless.h"
#include "util/until.h"

Chunk readChunk(Chunk chunk, FILE* file) {
    ptrdiff_t chunkDiff = 0;
    size_t len          = 0;
    long size           = 0;
    unless (
        file && isValid_chunk(chunk) &&
        fseek(file, 0, SEEK_END) == 0 &&
        (size = ftell(file)) > 0 &&
        fseek(file, 0, SEEK_SET) == 0
    ) return NOT_A_CHUNK;
    len = (size_t)((chunkDiff = chunk.end - chunk.start) + size + 2);
    unless (len < chunk.cap) {
        chunk.cap = len + 1;
        unless (
            len < chunk.cap &&
            (chunk.start = realloc(chunk.start, chunk.cap)) &&
            (chunk.end = chunk.start + chunkDiff) >= chunk.start
        ) return NOT_A_CHUNK;
    }
    unless (
        (++chunk.end) > chunk.start &&
        fread(chunk.end, (size_t)size, 1, file) == (size_t)size &&
        (chunk.end += size + 1) > chunk.start
    ) return NOT_A_CHUNK;

    *(chunk.end) = '\0';
    while (isspace(*(chunk.end - 1)) && chunk.end > chunk.start) *(--chunk.end) = '\0';
    return chunk;
}

Chunk readNewChunk(FILE* file) {
    return readChunk(constructEmpty_chunk(BUFSIZ), file);
}

Chunk open_readChunk_close(Chunk chunk, char const* const restrict filename) {
    FILE* file = fopen(filename, "r"); {
        unless (file) return NOT_A_CHUNK;
        chunk = readChunk(chunk, file);
    } if (fclose(file) == EOF) return NOT_A_CHUNK;
    return chunk;
}

Chunk open_readNewChunk_close(char const* const restrict filename) {
    return open_readChunk_close(constructEmpty_chunk(BUFSIZ), filename);
}
